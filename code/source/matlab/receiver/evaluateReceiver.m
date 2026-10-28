function R = evaluateReceiver(payload, saiAnt, opts)
% function evaluateReceiver
%
%   Evaluate a solved receiver payload against the SAI and GNSS-R observation
%   tests on a pooled single-receiver geometry sample, and derive the SAI
%   observation model that the SOCRATES C code reads from Inp_Design.txt.
%
%   R = evaluateReceiver(payload, saiAnt)
%   R = evaluateReceiver(payload, saiAnt, opts)
%
%   payload  solved receiver payload (the struct of solved_payload.m)
%   saiAnt   antenna summary of the pattern files (the struct of sai_antenna.m)
%   opts     optional struct; missing fields take these defaults:
%     RX_POL         "HVC"   receive polarization: "HV", "LHCP" or "HVC"
%     RX_PORT        "both"  HV only: "X", "Y" or "both"
%     T_h            12      inter-frequency observation window [hour]
%     RMSE_target    NaN     soil-moisture RMSE target; NaN = 0.06 (HV, HVC), 0.10 (LHCP)
%     RMSE_target_L  NaN     HVC only: target of the LHCP product; NaN = 0.10
%     patDir         ant_pat/sai                                 pattern files (cp/ below)
%     cdfFile        earthdata/reflectivity/reflectivity_CDF_v2.mat  per-port reflectivity CDFs
%     geomDir        ""      folder searched for a payload.geomFiles entry not found as given
%     source         ""      source stamp written to R.model.source
%     verbose        false   print the diagnostic tables
%     progressFcn    []      called as progressFcn(frac, msg) at each stage,
%                            frac the fraction of the run done (0 to 1)
%
%   Ported from single_ant_model.m of the ASR SAI design (2026-10-21). The
%   observation tests are unchanged: Test 1 (minimum direct SNR), Test 2
%   (effective reflectivity window), the GNSS RCG test, and the per-port
%   application with the synthesized L port of the HVC receiver. Nothing is
%   written to disk; R holds every result:
%     R.Tx, R.idxSoOp, R.idxGNSS, R.iCDF, R.cfgMap   transmitters and their CDF column and pattern
%     R.link       frequencies, EIRP, noise temperatures, S0, N_coh, dwell
%     R.budget     retrieval targets and the eta budgets
%     R.gamma, R.W_D, R.portList, R.scoreList
%     R.port       per-port results (single_ant_model.m's Rp), per specular point
%     R.acc        HVC: achieved reflectivity error and product tiers per transmitter
%     R.hvc        HVC: detection on L, validity on X and Y, LHCP product
%     R.combo      H&V port combination (Frechet bounds)
%     R.evalSum    HVC: product availability per transmitter
%     R.geomFiles, R.grp   pooled geometry files and the file index of each point
%     R.cdf        the reflectivity CDFs as loaded
%     R.model      HVC: SAI Observation Model section of Inp_Design.txt and its CDF tables
%
%   See also createInpDesign, readInpDesign
%
%   Copyright (C) 2023 Seho Kim
%
%   This file is part of SOCRATES.
%
%   SOCRATES is free software: you can redistribute it and/or modify
%   it under the terms of the GNU General Public License as published by
%   the Free Software Foundation, either version 3 of the License, or
%   (at your option) any later version.
%
%   SOCRATES is distributed in the hope that it will be useful,
%   but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%   GNU General Public License for more details.
%
%   You should have received a copy of the GNU General Public License
%   along with this program.  If not, see <https://www.gnu.org/licenses/>.
%
%   Version 1.0

    if nargin < 3, opts = struct(); end
    opts = setDefaults(opts, struct('RX_POL', "HVC", 'RX_PORT', "both", 'T_h', 12, ...
        'RMSE_target', NaN, 'RMSE_target_L', NaN, ...
        'patDir', fullfile('ant_pat', 'sai'), ...
        'cdfFile', fullfile('earthdata', 'reflectivity', 'reflectivity_CDF_v2.mat'), ...
        'geomDir', "", 'source', "", 'verbose', false, 'progressFcn', []));
    if opts.verbose
        vprintf = @(varargin) fprintf(varargin{:});
    else
        vprintf = @(varargin) [];
    end
    if isempty(opts.progressFcn), progress = @(frac, msg) []; else, progress = opts.progressFcn; end
    RX_POL  = string(opts.RX_POL);  RX_PORT = string(opts.RX_PORT);  T = opts.T_h;
    RMSE_SM_target = opts.RMSE_target;  RMSE_L_target = opts.RMSE_target_L;

    %% Payload and antenna
    vprintf('\n===== PARAMETERS FROM THE PAYLOAD (RX_POL %s, maximin %.3f) =====\n', payload.RX_POL, payload.maximin);
    if payload.RX_POL ~= RX_POL
        error('evaluateReceiver:payloadPolMismatch', ...
            'The payload was optimized for RX_POL = "%s" but this run is "%s".', payload.RX_POL, RX_POL);
    end
    if payload.altIsScaled
        warning('evaluateReceiver:altScaled', ...
            'Payload altitude %.0f km differs from the simulated 500 km: link geometry was scaled, the specular-point population was not.', payload.alt_Rx_m/1e3);
    end

    % Peaks, sky fraction and geometry come from saiAnt, written next to the
    % pattern files
    antDir = opts.patDir;
    cfgMap = struct( ...
        'ORBCOMM', struct('tag','CROSSDIPOLE_137', 'useDirect', true ), ...
        'UFO',     struct('tag','CROSSDIPOLE_255', 'useDirect', true ), ...
        'MUOS',    struct('tag','CROSSDIPOLE_370', 'useDirect', true ), ...
        'GPS',     struct('tag','PATCH_1575',      'useDirect', false), ...
        'Galileo', struct('tag','PATCH_1575',      'useDirect', false), ...
        'GLONASS', struct('tag','PATCH_1575',      'useDirect', false), ...
        'Beidou',  struct('tag','PATCH_1575',      'useDirect', false));
    if abs(payload.D_gp_m - saiAnt.D_gp_m) > 1e-3 || abs(payload.h_m - saiAnt.h_m) > 1e-3
        error('evaluateReceiver:antennaMismatch', ...
            'Payload antenna (D_gp %.3f m, h %.3f m) and pattern files (%.3f m, %.3f m) differ.', ...
            payload.D_gp_m, payload.h_m, saiAnt.D_gp_m, saiAnt.h_m);
    end
    if any(RX_POL == ["LHCP","HVC"]) && ~isfile(fullfile(antDir,'cp','CROSSDIPOLE_137_L_norm_vol_num.csv'))
        error('evaluateReceiver:saiNoCP', 'The circular pattern files in %s are missing.', fullfile(antDir, 'cp'));
    end
    G_ant_dB = saiAnt.G_ant_dB;                    % per-port peak directivity, dBi
    backFrac = saiAnt.backFrac;                    % zenith-hemisphere pattern fraction
    vprintf('  antenna: reflector %.2f m, standoff %.2f m; peaks [%s] dBi\n', saiAnt.D_gp_m, saiAnt.h_m, strtrim(sprintf('%.1f ', G_ant_dB)));

    %% Transmitters, noise, link budget
    c   = physconst("lightspeed");
    k   = physconst("boltzmann");

    SoOp = ["ORBCOMM", "UFO", "MUOS"];
    GNSS = ["GPS", "Galileo", "GLONASS", "Beidou"];
    Tx   = [SoOp, GNSS];
    nTx  = numel(Tx);
    idxSoOp = find(ismember(Tx, SoOp));
    idxGNSS = find(ismember(Tx, GNSS));

    % GNSS EIRP from each ICD's minimum received power at 5 deg elevation
    % (gnssEIRPFromICD), in the mode the payload was optimized with. GPS takes
    % its signal set from payload.GNSS_SIG; the other three use one L1-band
    % signal. MUOS 37 dBW per 5 MHz carrier (Garrison et al. 2024, ASR 73, Table 1).
    if ~isfield(payload, 'GNSS_EIRP_MODE')
        error('evaluateReceiver:noEIRPmode', 'The payload has no GNSS_EIRP_MODE.');
    end
    [EIRP_GNSS, f_GNSS] = gnssEIRPFromICD(["GPS_L1CA" "Galileo_E1" "GLONASS_L1OF" "BeiDou_B1C"], 5, ...
                                          payload.GNSS_EIRP_MODE, payload.GNSS_EIRP_MARGIN_dB);
    EIRP_dB   = [12, 27, 37, EIRP_GNSS.'];                                     % dBW
    freq      = [137e6, 255e6, 370e6, f_GNSS.'];                               % Hz
    BW_signal = [25e3, 25e3, 5e6, 2.046e6*[1 1 1 1]];                          % Hz; ORBCOMM 25 kHz channel (A80TD0008 Rev G, Table D.1)
    lambda    = c./freq;

    % System noise temperature from components, the same model as the
    % optimizer: Earth 260 K through the nadir beam, galactic sky 50 K at
    % 408 MHz scaling as f^-2.75 plus 2.7 K through the back lobe, then the
    % front-end loss ahead of the LNA and the LNA itself. The front-end loss
    % attenuates the signal and adds noise, so L_misc is the same number.
    T_earth = 260;
    T_sky   = 50*(408e6./freq).^2.75 + 2.7;
    L_fe_dB = payload.L_misc_dB*ones(1, nTx);
    NF_dB   = [payload.NF_VU_dB*ones(1,3), payload.NF_L_dB*ones(1,4)];
    L_fe    = 10.^(L_fe_dB/10);
    T_rx    = (L_fe - 1)*290 + L_fe.*(10.^(NF_dB/10) - 1)*290;
    T_ant   = (1 - backFrac)*T_earth + backFrac.*T_sky;
    T_noise = T_ant + T_rx;
    P_noise = k.*BW_signal.*T_noise;
    L_misc  = L_fe;                                % linear, per transmitter
    vprintf('\n===== SYSTEM NOISE TEMPERATURE =====\n');
    vprintf('%-9s %7s %7s %7s %7s %7s %7s\n','Tx','f[MHz]','back%%','T_sky','T_ant','T_rx','T_sys');
    for iTx = 1:nTx
        vprintf('%-9s %7.0f %7.1f %7.0f %7.0f %7.0f %7.0f\n', Tx(iTx), freq(iTx)/1e6, ...
                100*backFrac(iTx), T_sky(iTx), T_ant(iTx), T_rx(iTx), T_noise(iTx));
    end

    % S_0, the geometry-free part of the direct SNR: S_D = S_0 G_D / R_D^2.
    % GNSS: signal-averaged over the tracked signals of payload.GNSS_SIG; with
    % N_chan carrying the count, S_0*N_c is the sum over signals.
    EAIA = lambda.^2/(4*pi);
    S0   = EAIA .* 10.^(EIRP_dB/10) ./ P_noise ./ L_misc / (4*pi);
    sig  = payload.GNSS_SIG;                      % [f_MHz EIRP_dBW] rows, GPS
    lam_i = c ./ (sig(:,1)*1e6);
    nSig  = size(sig, 1);
    iGPS  = find(Tx == "GPS");
    if abs(sig(1,2) - EIRP_dB(iGPS)) > 0.05
        warning('evaluateReceiver:gpsEIRP', ...
            'payload.GNSS_SIG L1 EIRP (%.2f dBW) differs from the ICD-derived value %.2f dBW.', ...
            sig(1,2), EIRP_dB(iGPS));
    end
    for iTx = idxGNSS
        if iTx == iGPS
            S0(iTx) = mean((lam_i.^2/(4*pi)) .* 10.^(sig(:,2)/10)) / P_noise(iTx) / L_misc(iTx) / (4*pi);
        else
            % One L1-band signal. N_c carries N_chan = nSig for every GNSS
            % transmitter, so S_0 is divided by nSig to keep S_0*N_c that of
            % this single signal.
            S0(iTx) = EAIA(iTx) * 10^(EIRP_dB(iTx)/10) / P_noise(iTx) / L_misc(iTx) / (4*pi) / nSig;
        end
    end
    S0_dB = 10*log10(S0);

    %% Receive polarization and retrieval budget
    %   One switch fixes what must agree: the ports evaluated, the retrieval
    %   coefficients (Kim et al. 2023, Table V, averaged over inclinations), and
    %   the RMSE target.
    coefHV = struct('a', (0.4369+0.4232)/2, 'aT', (2.363e-4+3.204e-4)/2, 'b', (0.0378+0.0370)/2, 'target', 0.06);
    coefL  = struct('a', (0.6222+0.6385)/2, 'aT', (2.542e-4+2.277e-4)/2, 'b', (0.0844+0.0789+0.0810+0.0803)/4, 'target', 0.10);
    switch RX_POL
        case "LHCP"
            portList = "L";                cf = coefL;   polLabel = "LHCP";
        case "HV"
            if RX_PORT == "both", portList = ["X" "Y"]; else, portList = RX_PORT; end
            cf = coefHV;                                 polLabel = "H&V (dual-linear)";
        case "HVC"
            portList = ["L" "X" "Y"];      cf = coefHV;  polLabel = "H&V coherent (synthesized LHCP for detection)";
        otherwise
            error('evaluateReceiver:badPol', 'RX_POL must be "HV", "LHCP" or "HVC".');
    end
    a_SM_eta = cf.a;  a_SM_T = cf.aT;  b_SM = cf.b;
    if isnan(RMSE_SM_target), RMSE_SM_target = cf.target; end
    if abs(RMSE_SM_target - payload.assumedRMSE) > 1e-12 || abs(T - payload.assumedT_h) > 1e-12
        warning('evaluateReceiver:payloadMismatch', ...
            'RMSE target / T here (%g, %g h) differ from the payload (%g, %g h).', ...
            RMSE_SM_target, T, payload.assumedRMSE, payload.assumedT_h);
    end

    % Retrieval budget -> per-look reflectivity error, eta_bar = sqrt(1+eta^2)-1.
    % Below b_SM the squaring hides the sign, and between b_SM and the floor
    % the radicand is negative, so both are caught.
    RMSE_floor = b_SM + a_SM_T*T;
    if RMSE_SM_target <= RMSE_floor
        error('evaluateReceiver:infeasibleRMSE', ...
            'RMSE target %.4f is at or below the retrieval floor %.4f (b_SM %.4f + a_SM_T*T %.4f).', ...
            RMSE_SM_target, RMSE_floor, b_SM, a_SM_T*T);
    end
    eta_max_avg = sqrt((RMSE_SM_target - b_SM)^2 - (a_SM_T*T)^2)/a_SM_eta;
    eta_bar_avg = sqrt(eta_max_avg^2 + 1) - 1;

    %% Dwell budget
    %   T_dwell = N_incoh * T_I is the along-track averaging the retrieval
    %   consumes. The per-look error eta_bar*N_c is bounded above by
    %   (eta_avg^2/2) T_dwell B_n.
    T_integ = payload.T_integ_s;
    N_incoh = payload.N_incoh;
    N_chan  = payload.N_chan;
    T_dwell = N_incoh .* T_integ;                    % derived, never set directly
    N_coh   = T_integ .* BW_signal .* N_chan;
    eta_max_single = eta_max_avg * sqrt(N_incoh);
    eta_bar_single = sqrt(eta_max_single.^2 + 1) - 1;

    % HVC: the synthesized L port is scored a second time for the LHCP product
    scoreList = portList;
    eta_L_avg = NaN;  eta_bar_single_L = nan(1, nTx);
    if RX_POL == "HVC"
        a_L_eta = coefL.a;  a_L_T = coefL.aT;  b_L = coefL.b;
        if isnan(RMSE_L_target), RMSE_L_target = coefL.target; end
        if RMSE_L_target <= b_L + a_L_T*T
            error('evaluateReceiver:infeasibleRMSE_L', 'LHCP product target %.4f is at or below its floor %.4f.', RMSE_L_target, b_L + a_L_T*T);
        end
        eta_L_avg        = sqrt((RMSE_L_target - b_L)^2 - (a_L_T*T)^2)/a_L_eta;
        eta_bar_single_L = sqrt((eta_L_avg*sqrt(N_incoh)).^2 + 1) - 1;
        scoreList = [portList "Lcp"];
    end

    vprintf('\n===== CONFIGURATION =====\n');
    vprintf('RX_POL = %s (%s)   RMSE target %.3f m^3/m^3   T = %g h\n', RX_POL, polLabel, RMSE_SM_target, T);
    vprintf('eta_avg = %.5f   eta_bar_avg = %.3e', eta_max_avg, eta_bar_avg);
    if RX_POL == "HVC", vprintf('   LHCP product: target %.3f, eta_avg %.5f', RMSE_L_target, eta_L_avg); end
    vprintf('\nN_chan = [%s]\n', strtrim(sprintf('%d ', N_chan)));
    vprintf('\n===== DWELL BUDGET =====\n');
    vprintf('%-10s %9s %9s %10s %11s %11s %8s\n', 'Tx','T_I [ms]','N_incoh','T_dwell[s]','ebar*Nc','ceiling','frac');
    ebNc_ceil   = eta_max_avg^2/2 .* T_dwell .* BW_signal;
    ebNc_actual = eta_bar_single .* N_coh;
    for iTx = 1:nTx
        vprintf('%-10s %9.2f %9.0f %10.2f %11.2f %11.2f %8.3f\n', Tx(iTx), T_integ(iTx)*1e3, ...
                N_incoh(iTx), T_dwell(iTx), ebNc_actual(iTx), ebNc_ceil(iTx), ebNc_actual(iTx)/ebNc_ceil(iTx));
    end
    if isfield(payload, 'res_km')
        vprintf('  footprint (Fresnel + smear) [%s] km, requirement %g km\n', strtrim(sprintf('%.1f ', payload.res_km(1:4))), payload.RES_MAX_km);
    end

    KAPPA_MODE  = "geometry";   % kappa per specular point from tau_RD and T_I
    KAPPA_FIXED = -1;           % used only if KAPPA_MODE = "fixed"
    W_D = 1;                    % Test 1 margin factor (no extra derating)
    DIR_LIN_LOSS_dB = 3.01;     % direct-arm loss of a circular wave on a linear port

    %% Specular point data and reflectivity CDF
    %   The single-receiver files the optimizer pooled (payload.geomFiles) are
    %   pooled here, so the design is evaluated on the sample it was solved
    %   for; grp records the source file of each observation.
    geomFiles = string(payload.geomFiles);
    for ig = 1:numel(geomFiles)
        if ~isfile(geomFiles(ig)) && strlength(opts.geomDir) > 0
            [~, nm, ext] = fileparts(geomFiles(ig));
            geomFiles(ig) = fullfile(opts.geomDir, nm + ext);
        end
    end
    geomFiles = geomFiles(arrayfun(@isfile, geomFiles));
    if isempty(geomFiles), error('evaluateReceiver:noGeom', 'No specular-point file from payload.geomFiles found.'); end
    data = struct();
    for ig = 1:numel(geomFiles)
        [~, fname, ext] = fileparts(geomFiles(ig));
        progress(0.35*(ig-1)/numel(geomFiles), sprintf('reading geometry %d/%d (%s)', ig, numel(geomFiles), fname + ext));
        d1 = readSpDataNc(geomFiles(ig), Tx);
        d1.MUOS = d1.MUOS(logical(d1.flagSpotbeam),:);     % spot beam for MUOS
        for iTx = 1:numel(Tx)
            nm = Tx(iTx);  T1 = d1.(nm);  T1.grp = ig*ones(height(T1),1);
            if ig == 1, data.(nm) = T1; else, data.(nm) = [data.(nm); T1]; end
        end
    end
    vprintf('  geometry sample: %d file(s) pooled (%s)\n', numel(geomFiles), strjoin(geomFiles, ', '));

    gamma = 0.8;               % an observation is "valid" when P > gamma
    iCDF  = [1 2 3 4 4 4 4];   % CDF column per transmitter (137/255/370/1575 MHz)
    cdfFile = opts.cdfFile;
    if ~isfile(cdfFile), error('evaluateReceiver:noCDF', '%s not found.', cdfFile); end
    Scdf = load(cdfFile);      % Scdf.X.(port), Scdf.F.(port); ports X (V), Y (H), L (LHCP)

    %% Per-port analysis loop
    %   Every result is written into Rp.(port), so nothing is overwritten.
    Rp = struct();

    for kPort = 1:numel(scoreList)
        thisPort = scoreList(kPort);
        portLabel = string(thisPort);  if thisPort == "Lcp", portLabel = "L, LHCP product"; end
        progress(0.35 + 0.45*(kPort-1)/numel(scoreList), sprintf('port %s (%d/%d)', portLabel, kPort, numel(scoreList)));

        % Port selection. The reflector's nadir-to-zenith asymmetry is in the
        % pattern files; polMismatch_dB carries only the circular-on-linear
        % loss of the DIRECT arm on a linear port (the reflected arm's split is
        % inside the port-referenced reflectivity CDF).
        eta_bar_use = eta_bar_single;        % the H&V (or LHCP-mode) budget
        rpKey       = char(thisPort);        % where this score is stored in Rp
        if thisPort == "Lcp"                 % HVC: L port scored for the LHCP product
            cdfField = 'L';   antPort = 'lhcp';    polMismatch_dB = [0 0];
            eta_bar_use = eta_bar_single_L;
        elseif thisPort == "L"               % physical (LHCP) or synthesized (HVC) circular port
            cdfField = 'L';   antPort = 'lhcp';    polMismatch_dB = [0 0];
        else
            cdfField = char(thisPort);             polMismatch_dB = [DIR_LIN_LOSS_dB 0];
            if thisPort == "Y", antPort = 'port2'; else, antPort = 'port1'; end
        end
        if thisPort == "Lcp"
            vprintf('\n===== RECEIVE PORT L, scored for the LHCP product (target %.3f) =====\n', RMSE_L_target);
        else
            vprintf('\n===== RECEIVE PORT %s =====\n', cdfField);
        end
        vprintf('  antenna port = %s   polMismatch = [%.2f %.2f] dB\n', ...
                antPort, polMismatch_dB(1), polMismatch_dB(2));

        % reset the per-observation structs so a field can never survive from
        % the previous port
        alpha_dB = struct(); alpha = struct(); SD_dB = struct(); SD = struct();
        tau_RD = struct();   kap = struct();   feasible = struct();
        N_coh_min = struct(); SD_thresh = struct(); idx1 = struct();
        beta = struct(); beta_dB = struct(); GRGD_dB = struct(); RDRR = struct();
        rho_upper = struct(); rho_lower = struct(); P = struct(); Pfull = struct();
        margin_dB = struct(); beta_over_alpha = struct(); idxG = struct();

        [G_D_dB, G_R_dB] = LookupAntGainSAI(Tx, data, G_ant_dB, antPort, ...
                                            polMismatch_dB, antDir, cfgMap);

        X = Scdf.X.(cdfField);
        F = Scdf.F.(cdfField);

        % Test 1:  S_D > S_D,th = 4/(eta_bar*N_c - kappa - 4)
        for iTx = idxSoOp
            nm = Tx(iTx);

            alpha_dB.(nm) = 20*log10(data.(nm).rangeTR) - G_D_dB.(nm);
            alpha.(nm)    = 10.^(alpha_dB.(nm)/10);
            SD_dB.(nm)    = S0_dB(iTx) - alpha_dB.(nm);
            SD.(nm)       = 10.^(SD_dB.(nm)/10);

            tau_RD.(nm) = (data.(nm).rangeTS + data.(nm).rangeSR - data.(nm).rangeTR)/c;
            if KAPPA_MODE == "geometry"
                kap.(nm) = 1 - 2*min(tau_RD.(nm), T_integ(iTx)) / T_integ(iTx);
            else
                kap.(nm) = KAPPA_FIXED * ones(size(tau_RD.(nm)));
            end

            N_coh_min.(nm) = (4 + kap.(nm)) ./ eta_bar_use(iTx);
            feasible.(nm)  = N_coh(iTx) > N_coh_min.(nm);
            SD_thresh.(nm) = 4 ./ (eta_bar_use(iTx).*N_coh(iTx) - kap.(nm) - 4);
            SD_thresh.(nm)(~feasible.(nm)) = Inf;          % no S_D suffices

            idx1.(nm) = feasible.(nm) & (alpha.(nm) < S0(iTx)./(W_D*SD_thresh.(nm)));
        end

        vprintf('\n  Test 1 (minimum direct SNR)\n');
        vprintf('  %-9s %8s %8s %9s %11s %11s %11s %9s\n', ...
                'Tx','kap min','kap max','N_c','N_c,min max','S_D med[dB]','S_D,th[dB]','pass');
        nTest1Pass = zeros(1,nTx);
        for iTx = idxSoOp
            nm = Tx(iTx);
            nTest1Pass(iTx) = mean(idx1.(nm));
            sd = SD_thresh.(nm); sd = sd(isfinite(sd));
            if isempty(sd), sdStr = '     Inf'; else, sdStr = sprintf('%11.2f', 10*log10(median(sd))); end
            vprintf('  %-9s %+8.3f %+8.3f %9.0f %11.1f %11.2f %s %9.3f\n', nm, ...
                    min(kap.(nm)), max(kap.(nm)), N_coh(iTx), max(N_coh_min.(nm)), ...
                    median(SD_dB.(nm)), sdStr, nTest1Pass(iTx));
        end

        % Test 2:  rho_2 < Gamma_e < rho_1,  rho = C +/- sqrt(C^2 - N_D^2),
        %          C = (eta_bar*N_c - kappa - 2*N_D)/2
        % The window is log-symmetric about N_D because rho_1*rho_2 = N_D^2.
        vprintf('\n  Test 2 (effective reflectivity window), CDF field %s, median Gamma = %.4f\n', ...
                cdfField, gammaQuantile(F(:,1), X(:,1), 0.5));

        for iTx = idxSoOp
            nm = Tx(iTx);
            sel = idx1.(nm);                       % Test 2 only defined where Test 1 passes

            ND = 1 + 1./SD.(nm)(sel);
            kp = kap.(nm)(sel);
            C  = (eta_bar_use(iTx).*N_coh(iTx) - kp - 2*ND)/2;

            rho_e_upper = C + sqrt(C.^2 - ND.^2);
            rho_e_lower = C - sqrt(C.^2 - ND.^2);

            GRGD_dB.(nm) = G_R_dB.(nm)(sel) - G_D_dB.(nm)(sel);
            RDRR.(nm)    = data.(nm).rangeTR(sel) ./ ...
                           (data.(nm).rangeTS(sel) + data.(nm).rangeSR(sel));
            beta_dB.(nm) = 20*log10(RDRR.(nm)) + GRGD_dB.(nm);
            beta.(nm)    = 10.^(beta_dB.(nm)/10);

            % window expressed in surface reflectivity: Gamma = Gamma_e/beta
            rho_upper.(nm) = 10.^((10*log10(rho_e_upper) - beta_dB.(nm))/10);
            rho_lower.(nm) = 10.^((10*log10(rho_e_lower) - beta_dB.(nm))/10);

            [Xu, iu] = unique(X(:,iCDF(iTx)), 'last');
            Fu = F(iu, iCDF(iTx));

            Fhi = interp1(Xu, Fu, rho_upper.(nm), 'previous', 'extrap');
            Flo = interp1(Xu, Fu, rho_lower.(nm), 'previous', 'extrap');
            Fhi(rho_upper.(nm) < Xu(1)) = 0;
            Flo(rho_lower.(nm) < Xu(1)) = 0;
            P.(nm) = Fhi - Flo;

            % full-length copy: observations failing Test 1 cannot pass, so
            % they carry P = 0
            pf = zeros(numel(idx1.(nm)), 1);
            pf(sel) = P.(nm);
            Pfull.(nm) = pf;
        end

        nValid = zeros(1,nTx);
        vprintf('  %-9s %12s %12s %12s %12s %10s\n', ...
                'Tx','beta med[dB]','Gam lo med','Gam hi med','window[dB]','valid');
        for iTx = idxSoOp
            nm = Tx(iTx);
            nValid(iTx) = mean(Pfull.(nm) > gamma);
            vprintf('  %-9s %12.2f %12.4f %12.4f %12.2f %10.3f\n', nm, ...
                    10*log10(median(beta.(nm))), median(rho_lower.(nm)), ...
                    median(rho_upper.(nm)), ...
                    10*log10(median(rho_upper.(nm))./median(rho_lower.(nm))), ...
                    nValid(iTx));
        end

        % GNSS (known code):  beta/alpha > 1/(eta_bar*S_0*N_c*F^{-1}(1-gamma))
        % kappa is absent: with a clean local replica the correlator has no
        % self-noise, so sigma_nu^2 = 1/(S_D*N_c) exactly.
        [Xu4, i4] = unique(X(:,4), 'last');
        Fu4 = F(i4, 4);
        Ginv_GNSS = gammaQuantile(Fu4, Xu4, 1-gamma);    % F^{-1}(1-gamma)

        RCG_threshold = zeros(1,nTx);
        RCG_threshold(idxGNSS) = 1 ./ (eta_bar_use(idxGNSS) .* S0(idxGNSS) ...
                                       .* N_coh(idxGNSS) .* Ginv_GNSS);

        nTestGNSSPass = zeros(1,nTx);
        for iTx = idxGNSS
            nm = Tx(iTx);
            beta_over_alpha.(nm) = 10.^((G_R_dB.(nm) - ...
                20*log10(data.(nm).rangeTS + data.(nm).rangeSR))/10);
            idxG.(nm)      = beta_over_alpha.(nm) > RCG_threshold(iTx);
            margin_dB.(nm) = 10*log10(beta_over_alpha.(nm) ./ RCG_threshold(iTx));
            nTestGNSSPass(iTx) = mean(idxG.(nm));
        end

        vprintf('\n  GNSS (known-code) RCG test,  F^{-1}(1-gamma) = %.4g\n', Ginv_GNSS);
        vprintf('  %-9s %14s %12s %13s %13s\n', ...
                'Tx','RCG thresh','pass frac','med margin','p10 margin');
        for iTx = idxGNSS
            nm = Tx(iTx);
            ms = sort(margin_dB.(nm)(:));
            p10 = ms(max(1, ceil(0.10*numel(ms))));
            vprintf('  %-9s %14.4g %12.3f %10.2f dB %10.2f dB\n', nm, ...
                    RCG_threshold(iTx), nTestGNSSPass(iTx), median(ms), p10);
        end

        % Margin budget, 10*log10 of the GNSS test rearranged; every term is an
        % independent, additive design knob. Positive => pass.
        iG = idxGNSS(1);
        terms = [median(10*log10(beta_over_alpha.(Tx(iG)))), ...
                 10*log10(eta_bar_use(iG)), 10*log10(S0(iG)), ...
                 10*log10(N_coh(iG)), 10*log10(Ginv_GNSS)];
        names = ["10log10(beta/alpha)","10log10(eta_bar)","10log10(S_0)", ...
                 "10log10(N_c)","10log10(F^-1(1-gamma))"];
        vprintf('\n  Margin budget (median geometry, %s):\n', Tx(iG));
        for kk = 1:numel(terms)
            vprintf('     %-26s %+9.2f dB\n', names(kk), terms(kk));
        end
        vprintf('     %-26s %+9.2f dB\n', 'TOTAL MARGIN', sum(terms));
        vprintf('     break-even F^{-1}(1-gamma) = %.4g\n', Ginv_GNSS*10^(-sum(terms)/10));

        % store everything this port produced
        Rp.(rpKey).port      = string(cdfField);
        Rp.(rpKey).antPort   = string(antPort);
        Rp.(rpKey).SD        = SD;
        Rp.(rpKey).SD_dB     = SD_dB;
        Rp.(rpKey).SD_thresh = SD_thresh;
        Rp.(rpKey).alpha     = alpha;
        Rp.(rpKey).tau_RD    = tau_RD;
        Rp.(rpKey).kap       = kap;
        Rp.(rpKey).idx1      = idx1;
        Rp.(rpKey).pass1     = nTest1Pass;
        Rp.(rpKey).beta      = beta;
        Rp.(rpKey).beta_dB   = beta_dB;
        Rp.(rpKey).rho_lo    = rho_lower;
        Rp.(rpKey).rho_hi    = rho_upper;
        Rp.(rpKey).P         = P;
        Rp.(rpKey).Pfull     = Pfull;
        Rp.(rpKey).idxG      = idxG;            % GNSS per-observation pass
        Rp.(rpKey).nValid    = nValid;
        Rp.(rpKey).ba        = beta_over_alpha;
        Rp.(rpKey).margin    = margin_dB;
        Rp.(rpKey).thrRCG    = RCG_threshold;
        Rp.(rpKey).Ginv      = Ginv_GNSS;
        Rp.(rpKey).gnssPass  = nTestGNSSPass;
        Rp.(rpKey).G_R_dB    = G_R_dB;          % per-observation reflected-arm gain
        Rp.(rpKey).G_D_dB    = G_D_dB;          % and direct-arm gain
    end   % kPort

    %% Per-port summary
    vprintf('\n===== PER-PORT SUMMARY =====\n');
    vprintf('%-10s %6s %12s %12s %14s %14s\n', ...
            'Tx','port','Test1 pass','SoOp valid','GNSS pass','RCG thresh');
    for kPort = 1:numel(portList)
        fld = char(portList(kPort));
        for iTx = idxSoOp
            vprintf('%-10s %6s %12.3f %12.3f %14s %14s\n', Tx(iTx), fld, ...
                    Rp.(fld).pass1(iTx), Rp.(fld).nValid(iTx), '-', '-');
        end
        for iTx = idxGNSS
            vprintf('%-10s %6s %12s %12s %14.3f %14.4g\n', Tx(iTx), fld, '-', '-', ...
                    Rp.(fld).gnssPass(iTx), Rp.(fld).thrRCG(iTx));
        end
        vprintf('   F^{-1}(1-gamma) = %.4g for port %s\n', Rp.(fld).Ginv, fld);
    end

    %% Achieved reflectivity error per observation (HVC)
    %   The appendix error model evaluated forward for every observation at
    %   the median surface reflectivity of the port's CDF:
    %     SAI:   sigma_nu^2 = [(1+Ge+1/SD)^2 + kappa*Ge]/N_c,   Ge = beta*Gamma
    %     GNSS:  sigma_nu^2 = 1/(SD*N_c)                         (local replica)
    %     sigma_Ge^2 = 2*Ge*sigma_nu^2 + sigma_nu^4,  eta_1 = sigma_Ge/Ge,
    %     eta = eta_1/sqrt(N_incoh)                          (dwell averaging)
    %   Not mapped to RZSM RMSE: the retrieval error model describes a
    %   retrieval that consumes all four frequencies at one common eta.
    acc = struct();
    if RX_POL == "HVC"
        progress(0.8, 'achieved reflectivity error');
        vprintf('\n===== ACHIEVED REFLECTIVITY ERROR eta (forward error model, median Gamma) =====\n');
        vprintf('%-10s %8s %8s %8s | %8s %8s %8s | %8s %8s %8s\n', 'Tx', 'eta_L 20', 'median', '80 pct', ...
                'eta_X 20', 'median', '80 pct', 'eta_Y 20', 'median', '80 pct');
        for iTx = 1:nTx
            nm = Tx(iTx);
            etaP = struct();
            for fld = ["L" "X" "Y"]
                Rq = Rp.(char(fld));
                Gm = gammaQuantile(Scdf.F.(char(fld))(:,iCDF(iTx)), Scdf.X.(char(fld))(:,iCDF(iTx)), 0.5);
                GR = Rq.G_R_dB.(nm);  GD = Rq.G_D_dB.(nm);
                RD = data.(nm).rangeTR;  RR = data.(nm).rangeTS + data.(nm).rangeSR;
                if any(iTx == idxGNSS)
                    % local replica: Y estimates sqrt(Ge) with variance
                    % 1/(SD Nc), SD the reflected-channel SNR normalization
                    % S0/alpha with alpha = R_R^2/G_R, Ge -> Gamma
                    SDr = S0(iTx) .* 10.^(GR/10) ./ RR.^2;
                    Ge  = Gm * ones(size(SDr));
                    s2  = 1 ./ (SDr * N_coh(iTx));
                else
                    SDd = S0(iTx) .* 10.^(GD/10) ./ RD.^2;
                    bet = 10.^((20*log10(RD./RR) + GR - GD)/10);
                    Ge  = bet * Gm;
                    kp  = Rq.kap.(nm);
                    s2  = ((1 + Ge + 1./SDd).^2 + kp.*Ge) / N_coh(iTx);
                end
                sG2  = 2*Ge.*s2 + s2.^2;
                etaP.(fld) = sqrt(sG2) ./ Ge / sqrt(N_incoh(iTx));
            end
            acc.(nm).etaL = etaP.L;  acc.(nm).etaX = etaP.X;  acc.(nm).etaY = etaP.Y;
            vprintf('%-10s %8.4f %8.4f %8.4f | %8.4f %8.4f %8.4f | %8.4f %8.4f %8.4f\n', nm, ...
                    localQuantile(etaP.L, [0.2 0.5 0.8]), localQuantile(etaP.X, [0.2 0.5 0.8]), localQuantile(etaP.Y, [0.2 0.5 0.8]));
            % Product tiers per observation: the H&V product needs X and Y to
            % pass at the H&V budget; the LHCP product needs L (Lcp) at the
            % LHCP budget.
            if any(iTx == idxGNSS)
                okHV = Rp.X.idxG.(nm) & Rp.Y.idxG.(nm);   okL = Rp.Lcp.idxG.(nm);
            else
                okHV = (Rp.X.Pfull.(nm) > gamma) & (Rp.Y.Pfull.(nm) > gamma);
                okL  =  Rp.Lcp.Pfull.(nm) > gamma;
            end
            acc.(nm).okHV = okHV(:);  acc.(nm).okL = okL(:);
            acc.(nm).fracBoth  = mean(okHV & okL);
            acc.(nm).fracLonly = mean(okL & ~okHV);
            acc.(nm).fracNone  = mean(~okL);
            acc.(nm).etaLonly  = localQuantile(etaP.L(okL(:) & ~okHV(:)), [0.2 0.5 0.8]);
        end
        vprintf('  budgets: eta_max_avg %.4f (H&V product on X and Y), eta_L_avg %.4f (LHCP product on L)\n', eta_max_avg, eta_L_avg);
        vprintf('\n  product tiers per transmitter (fraction of observations) and eta_L on the LHCP-only tier\n');
        vprintf('%-10s %10s %10s %10s | %s\n', 'Tx', 'H&V+LHCP', 'LHCP only', 'neither', 'eta_L on LHCP-only tier (20/50/80 pct)');
        for iTx = 1:nTx
            nm = Tx(iTx);
            vprintf('%-10s %10.3f %10.3f %10.3f | %7.4f %7.4f %7.4f\n', nm, acc.(nm).fracBoth, ...
                    acc.(nm).fracLonly, acc.(nm).fracNone, acc.(nm).etaLonly);
        end
    end

    %% Port combination for H&V retrieval
    %   An observation is usable for H&V only when both ports deliver. The
    %   joint probability is bounded (Frechet-Hoeffding):
    %       max(0, P_X + P_Y - 1)  <=  P_both  <=  min(P_X, P_Y)
    %   and sits near the upper bound, since Gamma_X and Gamma_Y scale with the
    %   same surface reflectivity.
    combo = struct();
    hvc = struct();
    evalSum = struct();
    hasXY = all(ismember(["X" "Y"], portList));
    if RX_POL == "HVC"
        vprintf('\n===== H&V COHERENT: DETECTION ON L, RETRIEVAL ON X AND Y =====\n');
        vprintf('%-10s %12s %12s %12s %14s\n', 'Tx', 'detect (L)', 'valid X', 'valid Y', 'LHCP prod (L)');
        hvc = struct('detL', nan(1,numel(Tx)), 'vX', nan(1,numel(Tx)), 'vY', nan(1,numel(Tx)), 'lcp', nan(1,numel(Tx)));
        for iTx = idxSoOp
            hvc.detL(iTx) = Rp.L.nValid(iTx);   hvc.vX(iTx) = Rp.X.nValid(iTx);
            hvc.vY(iTx)   = Rp.Y.nValid(iTx);   hvc.lcp(iTx) = Rp.Lcp.nValid(iTx);
        end
        for iTx = idxGNSS
            hvc.detL(iTx) = Rp.L.gnssPass(iTx); hvc.vX(iTx) = Rp.X.gnssPass(iTx);
            hvc.vY(iTx)   = Rp.Y.gnssPass(iTx); hvc.lcp(iTx) = Rp.Lcp.gnssPass(iTx);
        end
        for iTx = 1:numel(Tx)
            vprintf('%-10s %12.3f %12.3f %12.3f %14.3f\n', Tx(iTx), hvc.detL(iTx), hvc.vX(iTx), hvc.vY(iTx), hvc.lcp(iTx));
        end
        % Product availability (single_ant_model.m wrote it to evaluation_summary.m)
        evalSum.order     = Tx;
        evalSum.HV_lo     = max(0, hvc.vX + hvc.vY - 1);   % H&V product, Frechet lower bound
        evalSum.HV_hi     = min(hvc.vX, hvc.vY);           % H&V product, upper bound
        evalSum.LHCP      = hvc.lcp;                       % LHCP product on the synthesized port
        evalSum.LHCP_only = arrayfun(@(k) acc.(Tx(k)).fracLonly, 1:nTx);
    end
    if hasXY
        vprintf('\n===== H&V RETRIEVAL: PORT X, PORT Y, AND BOTH =====\n');
        vprintf('%-10s %10s %10s %14s %14s %12s\n', ...
                'Tx','valid X','valid Y','both (lower)','both (upper)','governed by');
        for iTx = idxSoOp
            nm = Tx(iTx);
            pX = Rp.X.Pfull.(nm);
            pY = Rp.Y.Pfull.(nm);
            P_lo = max(0, pX + pY - 1);              % Frechet lower
            P_hi = min(pX, pY);                      % Frechet upper
            combo.lo.(nm) = P_lo;
            combo.hi.(nm) = P_hi;
            vX = mean(pX > gamma);  vY = mean(pY > gamma);
            combo.vX(iTx) = vX;     combo.vY(iTx) = vY;
            combo.vLo(iTx) = mean(P_lo > gamma);
            combo.vHi(iTx) = mean(P_hi > gamma);
            if vX <= vY, gov = 'X'; else, gov = 'Y'; end
            vprintf('%-10s %10.3f %10.3f %14.3f %14.3f %12s\n', ...
                    nm, vX, vY, combo.vLo(iTx), combo.vHi(iTx), gov);
        end
    end

    %% Results
    R = struct();
    R.Tx = Tx;  R.idxSoOp = idxSoOp;  R.idxGNSS = idxGNSS;  R.iCDF = iCDF;  R.cfgMap = cfgMap;
    R.link = struct('freq', freq, 'EIRP_dB', EIRP_dB, 'BW_signal', BW_signal, ...
        'T_sky', T_sky, 'T_ant', T_ant, 'T_rx', T_rx, 'T_noise', T_noise, 'P_noise', P_noise, ...
        'S0', S0, 'S0_dB', S0_dB, 'T_integ', T_integ, 'N_incoh', N_incoh, 'N_chan', N_chan, ...
        'T_dwell', T_dwell, 'N_coh', N_coh, 'ebNc_actual', ebNc_actual, 'ebNc_ceil', ebNc_ceil);
    R.budget = struct('RX_POL', RX_POL, 'T_h', T, 'RMSE_target', RMSE_SM_target, 'RMSE_target_L', RMSE_L_target, ...
        'eta_max_avg', eta_max_avg, 'eta_bar_avg', eta_bar_avg, 'eta_L_avg', eta_L_avg, ...
        'eta_bar_single', eta_bar_single, 'eta_bar_single_L', eta_bar_single_L);
    R.gamma = gamma;  R.W_D = W_D;
    R.portList = portList;  R.scoreList = scoreList;
    R.port = Rp;  R.acc = acc;  R.hvc = hvc;  R.combo = combo;  R.evalSum = evalSum;
    R.geomFiles = geomFiles;
    R.grp = struct();
    for iTx = 1:nTx
        R.grp.(Tx(iTx)) = data.(Tx(iTx)).grp;
    end
    R.cdf = Scdf;
    R.model = struct();
    if RX_POL == "HVC"
        R.model = observationModel(R, G_ant_dB, DIR_LIN_LOSS_dB, string(opts.source));
    end
end

function M = observationModel(R, G_ant_dB, dirLinLoss_dB, source)
% OBSERVATIONMODEL  SAI Observation Model of Inp_Design.txt: the constants the
%   C code reads per transmitter, and the per-port CDF tables on one F grid.
    Tx = R.Tx;  L = R.link;  B = R.budget;
    % SOCRATES derives eta_bar from eta and N_incoh; make sure that gives the
    % evaluator's values exactly
    ebHV = sqrt((B.eta_max_avg*sqrt(L.N_incoh)).^2 + 1) - 1;
    ebL  = sqrt((B.eta_L_avg*sqrt(L.N_incoh)).^2 + 1) - 1;
    if ~isequal(ebHV, B.eta_bar_single) || ~isequal(ebL, B.eta_bar_single_L)
        error('evaluateReceiver:etaBar', 'eta_bar from eta and N_incoh differs from the evaluator.');
    end

    M = struct();
    M.source = source;
    M.gamma = R.gamma;
    M.WD = R.W_D;
    M.etaHV = B.eta_max_avg;
    M.etaL = B.eta_L_avg;
    M.dirLinLoss_dB = dirLinLoss_dB;
    M.nTx = numel(Tx);
    M.tx = struct('name', cellstr(Tx), 'type', "SAI", 'cdfCol', 0, 'patTag', "", ...
                  'Glin_dBi', 0, 'S0', 0, 'Nc', 0, 'TI_s', 0, 'Nincoh', 0, 'spotbeam', false);
    for iTx = 1:numel(Tx)
        nm = Tx(iTx);
        M.tx(iTx).name = nm;
        if ~ismember(iTx, R.idxSoOp), M.tx(iTx).type = "GNSS"; end
        M.tx(iTx).cdfCol = R.iCDF(iTx);
        M.tx(iTx).patTag = string(R.cfgMap.(nm).tag);
        M.tx(iTx).Glin_dBi = G_ant_dB(iTx);
        M.tx(iTx).S0 = L.S0(iTx);
        M.tx(iTx).Nc = L.N_coh(iTx);
        M.tx(iTx).TI_s = L.T_integ(iTx);
        M.tx(iTx).Nincoh = L.N_incoh(iTx);
        M.tx(iTx).spotbeam = nm == "MUOS";    % only the MUOS spot-beam points are evaluated
    end

    % CDF tables: one F grid shared by the ports and the four frequency columns
    F0 = R.cdf.F.X(:,1);
    for p = ["X" "Y" "L"]
        Fp = R.cdf.F.(p);  Xp = R.cdf.X.(p);
        if size(Xp, 2) < 4 || size(Fp, 2) < 4
            error('evaluateReceiver:cdfCols', 'CDF of port %s has fewer than 4 columns.', p);
        end
        if any(Fp(:,1:4) ~= F0, 'all')
            error('evaluateReceiver:cdfGrid', 'The F grid of port %s differs from that of port X.', p);
        end
        M.cdf.(p) = [F0, Xp(:,1:4)];
    end
end

function s = setDefaults(s, d)
    for f = string(fieldnames(d)).'
        if ~isfield(s, f), s.(f) = d.(f); end
    end
end

function q = gammaQuantile(Fv, Xv, p)
% GAMMAQUANTILE  F^{-1}(p) from a tabulated CDF, with F as the independent
%   variable. Duplicated F values are removed so interp1 stays monotone.
    Fv = Fv(:); Xv = Xv(:);
    ok = isfinite(Fv) & isfinite(Xv);
    [Fu, iu] = unique(Fv(ok), 'first');
    Xu = Xv(ok);  Xu = Xu(iu);
    q = interp1(Fu, Xu, p, 'linear', 'extrap');
end

function q = localQuantile(v, p)
    v = sort(v(isfinite(v)));  n = numel(v);
    if n == 0, q = nan(size(p)); return; end
    q = v(max(1, min(n, round(p*n))));
end
