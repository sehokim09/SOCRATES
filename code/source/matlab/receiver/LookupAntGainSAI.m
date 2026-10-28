function [G_D_dB, G_R_dB] = LookupAntGainSAI(Tx, data, G_ant_dB, port, polMismatch_dB, antDir, cfgMap)
%LOOKUPANTGAINSAI Scalar per-port direct/reflected gains for the SAI model.
%
%   [G_D_dB, G_R_dB] = LookupAntGainSAI(Tx, data, G_ant_dB)
%
%   Drop-in for LookupAntGain: returns G_D_dB.(Tx) and G_R_dB.(Tx) as plain
%   numeric column vectors, one per specular point, so
%       alpha_dB = 20*log10(rangeTR) - G_D_dB.(nm);
%       GRGD_dB  = G_R_dB.(nm) - G_D_dB.(nm);
%   work unchanged. Run the model once per port to get the two dual-pol
%   channels:
%       [G_D_dB, G_R_dB] = LookupAntGainSAI(Tx, data, G_ant_dB, 'port1');
%       [G_D_dB, G_R_dB] = LookupAntGainSAI(Tx, data, G_ant_dB, 'port2');
%
% Inputs
%   port            'port1' (X, default) or 'port2' (Y)
%   polMismatch_dB  [direct reflected] losses in dB, default [3.01 0].
%                   Scalar input is expanded to both paths.
%
%                   DIRECT path: the transmitters here are all circular and a
%                   linearly polarized port collects exactly half the power of
%                   a circular wave, at every angle (A-17). 3.01 dB is exact
%                   and surface independent, so it belongs here.
%
%                   REFLECTED path: 0 by default, because a measured per-port
%                   reflectivity (Gamma_X or Gamma_Y) ALREADY contains the
%                   port's response to the reflected wave. Adding 3.01 dB here
%                   as well would count it twice.
%
%                   Use [3.01 3.01] only with an LHCP reflectivity, which is
%                   the R_vv = R_hh idealization: there Gamma_X = Gamma_Y =
%                   Gamma_LHCP/2, so the constant 1/2 is the whole story and
%                   carrying it in G_R is equivalent.
%
%                   Whatever the pair, a component common to both paths
%                   cancels in beta and survives in S_D (A-18, A-19).
%   antDir          pattern directory (default ./antPatternFiles/nominal)
%   cfgMap          (optional) struct mapping each Tx name to .tag and
%                   .useDirect, passed through to LookupAntGainDualPol. Use
%                   it to point at a different pattern SET, e.g. the SAI set
%                   in antPatternFiles/sai, where UFO has its own 255 MHz
%                   pattern (CROSSDIPOLE_255) instead of borrowing MUOS's.
%
% Where the 3.01 dB does and does not matter
%   It CANCELS in beta_dB = 20*log10(R_D/R_R) + (G_R_dB - G_D_dB), so the SAI
%   reflectivity observable is untouched by it. It does NOT cancel in
%   alpha_dB / S_D, where it is a real 3 dB reduction in direct-signal SNR
%   relative to a polarization-matched receiver. That is the honest cost of
%   receiving a circular signal on a linear port.
%
%   The departure from 3.01 dB on the reflected path -- caused by
%   R_vv ~= R_hh making the reflected wave elliptical -- is precisely the
%   dual-polarization signal. It belongs in your per-port reflectivity, not
%   here, which is why this function applies the same constant to both ports
%   and both paths.
%
% Note
%   At L-band the direct path is not used (nadir-only GNSS-R, PRN replica as
%   reference), so G_D_dB is NaN for the GNSS transmitters, as in
%   LookupAntGain. The SoOp tests loop over idxSoOp only, so that is inert.
%
%   Peak gains should match the elements: for the nominal set,
%       G_ant_dB = [3.0, 4.1, 4.1, 7.0, 7.0, 7.0, 7.0];   % per-port, dBi
%   for [ORBCOMM UFO MUOS GPS Galileo GLONASS Beidou].

    if nargin < 4 || isempty(port),           port = 'port1';       end
    if nargin < 5 || isempty(polMismatch_dB), polMismatch_dB = [3.01 0]; end
    if isscalar(polMismatch_dB), polMismatch_dB = [polMismatch_dB polMismatch_dB]; end
    if numel(polMismatch_dB) ~= 2
        error('LookupAntGainSAI:badMismatch', ...
              'polMismatch_dB must be a scalar or [direct reflected].');
    end
    if nargin < 6, antDir = ''; end
    if nargin < 7, cfgMap = []; end
    if ~ismember(lower(port), {'port1','port2','lhcp','rhcp'})
        error('LookupAntGainSAI:badPort', ...
              'port must be port1, port2, lhcp or rhcp.');
    end

    % ---- circular ports come from the cp/ files ----------------------------
    % The cp/ set is NOT an independent model: it is the same port fields with
    % the hybrid applied and then projected onto the circular basis,
    %     gx = gXX + q*gYX,  gy = gXY + q*gYY,  q = the hybrid transfer
    %     R = |gx - j*gy|/sqrt(2),  L = |gx + j*gy|/sqrt(2)
    % which reproduces the files to 1.4e-6. The quadrature enters through the
    % FEED q, not through the projection; with q real the two senses come out
    % identical and there is no discrimination at all.
    if any(strcmpi(port, {'lhcp','rhcp'}))
        [G_D_dB, G_R_dB] = localCircularGain(Tx, data, lower(port), antDir, polMismatch_dB, cfgMap);
        return
    end

    [~, ~, GD, GR] = LookupAntGainDualPol(Tx, data, G_ant_dB, antDir, cfgMap);

    G_D_dB = struct();  G_R_dB = struct();
    for iTx = 1:numel(Tx)
        nm = Tx(iTx);
        G_D_dB.(nm) = GD.(nm).(port)(:) - polMismatch_dB(1);
        G_R_dB.(nm) = GR.(nm).(port)(:) - polMismatch_dB(2);
    end
end

% ========================================================================
function [G_D_dB, G_R_dB] = localCircularGain(Tx, data, port, antDir, pm, cfgMap)
%LOCALCIRCULARGAIN Per-observation gains for a circularly polarized port.
%
%   Files:  <antDir>/cp/<TAG>_{L,R}_norm_vol_num.csv, 361 phi x 181 theta,
%           non-negative POWER patterns sharing one normalization (the
%           co-polar peak is exactly 1).
%
%   L is the LHCP component radiated by the LHCP-fed port and R the RHCP
%   component of the same port. By reciprocity the port's response to an
%   incident LHCP wave follows L and to an incident RHCP wave follows R.
%
%   In SAI the two paths carry OPPOSITE senses: the transmitters are RHCP
%   and a specular reflection flips the handedness. So for an LHCP port
%       reflected (LHCP)  -> co-polar   -> L
%       direct    (RHCP)  -> cross-polar-> R
%   and the direct path is attenuated by the element's axial ratio. That is
%   the honest cost of receiving both SAI paths on one circular port, and it
%   is why polMismatch_dB should be [0 0] here: the efficiency is already in
%   the patterns.
%
%   G_ant_dB is NOT taken from the caller. The peak co-polar directivity is
%   derived from the pattern itself,
%       D = 4*pi*max(P_co) / integral( (P_co + P_cx) dOmega )
%   which reproduces the published linear per-port peaks to 0.03 dB.

    if isempty(antDir), antDir = fullfile('antPatternFiles','nominal'); end
    cpDir = fullfile(antDir, 'cp');

    % Tag map: the caller's cfgMap if supplied (e.g. the sai set with
    % CROSSDIPOLE_255 for UFO), else the same default LookupAntGainDualPol uses.
    if nargin >= 6 && ~isempty(cfgMap)
        cfg = cfgMap;
    else
    cfg = struct( ...
        'ORBCOMM', struct('tag','CROSSDIPOLE_137', 'useDirect', true ), ...
        'UFO',     struct('tag','CROSSDIPOLE_370', 'useDirect', true ), ...
        'MUOS',    struct('tag','CROSSDIPOLE_370', 'useDirect', true ), ...
        'GPS',     struct('tag','PATCH_1575',      'useDirect', false), ...
        'Galileo', struct('tag','PATCH_1575',      'useDirect', false), ...
        'GLONASS', struct('tag','PATCH_1575',      'useDirect', false), ...
        'Beidou',  struct('tag','PATCH_1575',      'useDirect', false));
    end

    if strcmp(port,'lhcp'), coTag = 'L'; cxTag = 'R';
    else,                   coTag = 'R'; cxTag = 'L';
    end

    G_D_dB = struct();  G_R_dB = struct();  cache = struct();
    for iTx = 1:numel(Tx)
        nm = char(Tx(iTx));
        if ~isfield(cfg, nm)
            error('LookupAntGainSAI:noConfig', 'No antenna config for "%s".', nm);
        end
        tag = cfg.(nm).tag;
        if ~isfield(cache, tag)
            cache.(tag) = localLoadCP(cpDir, tag, coTag, cxTag);
        end
        P = cache.(tag);
        d = data.(Tx(iTx));

        % reflected: co-polar, angles already in the nadir antenna frame.
        % The files are FIELD amplitudes, hence 20*log10.
        G_R_dB.(Tx(iTx)) = 20*log10(localPick(P, P.co, d.lookAngElR, d.lookAngAzR)) ...
                           + P.Gpeak_dB - pm(2);

        % direct: through the BACK hemisphere of the same element, at the
        % stored nadir-frame angles (see the CORRECTED note in
        % LookupAntGainDualPol). A CP element reverses sense through the back,
        % so the RHCP direct signal is co-polar to an LHCP-at-nadir port there;
        % the cross-polar file P.cx therefore remains the right one to read.
        if cfg.(nm).useDirect
            G_D_dB.(Tx(iTx)) = 20*log10(localPick(P, P.cx, d.lookAngElD, d.lookAngAzD)) ...
                               + P.Gpeak_dB - pm(1);
        else
            G_D_dB.(Tx(iTx)) = nan(numel(d.lookAngElD), 1);
        end
    end
end

% ------------------------------------------------------------------------
function P = localLoadCP(cpDir, tag, coTag, cxTag)
    fco = fullfile(cpDir, sprintf('%s_%s_norm_vol_num.csv', tag, coTag));
    fcx = fullfile(cpDir, sprintf('%s_%s_norm_vol_num.csv', tag, cxTag));
    for f = {fco, fcx}
        if ~isfile(f{1})
            error('LookupAntGainSAI:noCPfile', 'Missing %s', f{1});
        end
    end
    P.co = readmatrix(fco);
    P.cx = readmatrix(fcx);
    if ~isequal(size(P.co), size(P.cx))
        error('LookupAntGainSAI:cpSize', '%s and %s differ in size.', fco, fcx);
    end
    [P.nPhi, P.nTheta] = size(P.co);
    P.resDeg = 180/(P.nTheta - 1);          % derived, never hardcoded
    if abs(360/(P.nPhi - 1) - P.resDeg) > 1e-9
        error('LookupAntGainSAI:cpGrid', ...
              '%s: theta step %.4g deg but phi step %.4g deg.', ...
              fco, P.resDeg, 360/(P.nPhi - 1));
    end
    % Peak co-polar directivity from the pattern integral. The files hold FIELD
    % amplitudes, so the power pattern is the square and the total radiated
    % power is co^2 + cx^2. Drop the duplicated phi = 360 row.
    th = (0:P.nTheta-1) * P.resDeg * pi/180;
    dOm = sin(th) * (P.resDeg*pi/180)^2;
    tot = sum( (P.co(1:end-1,:).^2 + P.cx(1:end-1,:).^2) .* dOm, 'all' );
    P.Gpeak_dB = 10*log10( 4*pi*max(P.co(:)).^2 / tot );
end

% ------------------------------------------------------------------------
function v = localPick(P, M, th, ph)
%LOCALPICK Nearest-neighbour lookup, same convention as LookupAntGainDualPol.
    R2D = 180/pi;
    th = double(th(:));  ph = double(ph(:));
    bad = ~isfinite(th) | ~isfinite(ph);
    th(bad) = 0;  ph(bad) = 0;
    ph = mod(ph + 2*pi, 2*pi);
    th = min(max(th, 0), pi);
    iTheta = max(1, min(round(th*R2D / P.resDeg) + 1, P.nTheta));
    iPhi   = max(1, min(round(ph*R2D / P.resDeg) + 1, P.nPhi));
    v = M(sub2ind([P.nPhi, P.nTheta], iPhi, iTheta));
    v(bad) = NaN;
end
