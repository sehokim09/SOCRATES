function A = SAIAntennaPattern(kind, f_MHz, D_gp, h_m, resDeg, opts)
%SAIANTENNAPATTERN  Dual-linear pattern of one SAI receive element, as arrays.
%
%   A = SAIAntennaPattern(kind, f_MHz, D_gp, h_m)
%
%   THE ONE ANTENNA MODEL.  This is the pattern physics of
%   GenerateNominalAntPatterns (localConfig + localBuild, 'DUALLIN' basis)
%   exposed as a function that RETURNS the arrays instead of writing files.
%   It exists so that the optimizer, the pattern generator and the evaluator
%   all use the same antenna:
%
%       optimize_sai_payload.m   calls it inside the objective, so the search
%                                moves over REAL patterns rather than an
%                                aperture-bound proxy
%       GenerateSAIAntPatterns.m calls it to write the CSV files and the
%                                parameter table
%       single_ant_model.m       reads those CSV files
%
%   Any change to the antenna physics belongs HERE and nowhere else.  It is
%   validated against the CSV files GenerateNominalAntPatterns writes: for the
%   bare deck it reproduces them to the file write precision (5e-7).
%
%   Inputs
%     kind    'CROSSDIPOLE' (two half-wave dipoles, two ports) or 'PATCH'
%             (rectangular microstrip, two feeds)
%     f_MHz   137, 255, 370 or 1575.42 (any value works; the patch epsEff is
%             chosen by nearest band)
%     D_gp    ground-plane / reflector side, m (0.30 is the bare 6U deck)
%     h_m     wire-element standoff above the reflector, m (ignored for PATCH)
%     resDeg  grid resolution, default 1
%
%   Output struct A
%     XX XY YX YY   signed normalized field, [nPhi x nTheta], rows phi 0:360,
%                   columns theta 0:180 from NADIR; co-polar peak exactly 1
%     L R           the SAME two port fields fed in quadrature and projected
%                   onto the circular basis: normalized field MAGNITUDES of
%                   the LHCP and RHCP responses, jointly normalized by the
%                   co-polar peak so L./R is the true cross-polar level.
%                   Sense is LHCP toward NADIR, hence RHCP toward zenith,
%                   which is the single-port SAI configuration: co-polar to
%                   the LHCP reflected signal from below AND to the RHCP
%                   direct signal from above.  Hybrid imbalance 0.5 dB and
%                   5 deg, as in GenerateNominalAntPatterns.  The dual-linear
%                   and circular outputs describe one physical antenna with
%                   two feed networks; no floor is re-applied.
%     D0cp_dBi      peak co-polar (L) directivity, D = 4*pi*max(L^2)/int(L^2+R^2)
%     AR0_dB        boresight axial ratio
%     FBcp_dB       mean L^2 in the 0-60 cone over mean R^2 in the 120-180
%                   cone: the reflected-arm to direct-arm ratio on the single
%                   circular port, which is what Test 2 feels in LHCP mode
%     backFrac_cp   fraction of (L^2+R^2) beyond the Earth limb (sky)
%     th ph         grids in degrees
%     D0_dBi        per-port peak co-polar directivity (port X)
%     FB_bore_dB    boresight nadir-to-zenith ratio, P(0)/P(180)
%     FB_cone_dB    ratio of mean power in the 0-60 cone to the 120-180 cone,
%                   the quantity the SAI tests actually feel
%     backFrac      fraction of radiated power beyond the Earth limb,
%                   theta > thLimb_deg = asind(R_E/(R_E+alt_km)) (sky fraction
%                   for the antenna temperature; 68 deg at 500 km)
%     hpbwE hpbwH   half-power beamwidths, deg
%     eta_rad       radiated power relative to the free-space element (standoff
%                   loading; 1 when no reflector couples)
%     L_standoff_dB realized-gain penalty from that loading, <= 0, referenced so
%                   it is 0 without a reflector
%     G_realized_dBi  D0_dBi + L_standoff_dB -- USE THIS, not D0_dBi, as the
%                   peak gain in the link budget
%     G_realized_cp_dBi  the same for the circular feed (D0cp_dBi + L_standoff_dB)
%     rho           reflector coupling actually used, 0..1
%     DoverLam FB_model_dB hOverLam useImage legacy  the configuration it used
%
%   REVISION 2026-10-09. Three defects let the payload optimizer exploit the
%   model (see docs/Payload_optimizer_antenna_findings.md): a binary useImage
%   switch at D/lam = 0.5 (a 3.6 dB cliff the solution parked 4 mm below), an
%   image term that amplified the back hemisphere instead of blocking it, and
%   no penalty for a dipole sitting close to the reflector. All three are fixed
%   by default; opts.legacy = true reproduces the old model exactly.

    if nargin < 5 || isempty(resDeg), resDeg = 1.0; end
    if nargin < 6 || isempty(opts),   opts   = struct(); end
    % ---- model options (2026-10-09 revision) --------------------------------
    %   legacy      true reproduces the pre-revision model exactly: binary
    %               useImage at D/lam = 0.5, no back blocking, no standoff
    %               penalty, h/lam clamped at 0.25. Default false.
    %   rampLo/Hi   D/lam range over which the reflector coupling rho ramps
    %               0 -> 1 (smoothstep). Replaces the binary switch.
    %   backBlock   true: the image term is weighted by the front-hemisphere
    %               window, so a reflector suppresses the back instead of
    %               amplifying it. Default true.
    %   R_loss_ohm  loss resistance against which the reflector-loaded
    %               radiation resistance is judged. Sets the standoff penalty.
    %               1 ohm is an excellent structure, 3 (default) a realistic
    %               deployable tape-spring dipole with its balun and match,
    %               5 a poor one. At 137 MHz, h = 0.15 m, D = 2 m the penalty
    %               is -0.3 / -0.9 / -1.4 dB for 1 / 3 / 5 ohm.
    %   R_dipole_ohm free-space half-wave dipole radiation resistance.
    %   alt_km      receiver altitude, km, default 500.  Sets the Earth-limb
    %               nadir angle asind(R_E/(R_E+alt)) that splits the pattern
    %               into Earth (inside) and sky (outside) for backFrac and
    %               backFrac_cp, the sky fraction of the antenna temperature.
    def = struct('legacy', false, 'rampLo', 0.30, 'rampHi', 1.00, ...
                 'backBlock', true, 'R_loss_ohm', 3.0, 'R_dipole_ohm', 73.1, ...
                 'alt_km', 500);
    fn = fieldnames(def);
    for i = 1:numel(fn)
        if ~isfield(opts, fn{i}), opts.(fn{i}) = def.(fn{i}); end
    end
    c0  = 299792458;
    lam = c0/(f_MHz*1e6);
    Dl  = D_gp/lam;

    % ---- configuration: identical to GenerateNominalAntPatterns/localConfig --
    FB_dB    = min(max(20*log10(1 + 3*Dl^1.2), 1.5), 25.0);
    transDeg = min(max(25.0/(1 + Dl), 4.0), 25.0);
    floor_dB = min(max(-(FB_dB + 15.0), -45.0), -18.0);
    if opts.legacy
        hOverLam = min(0.25, h_m/lam);
        useImage = (Dl >= 0.5);
        rho      = double(useImage);
    else
        hOverLam = h_m/lam;                              % no clamp: h/lam > 0.25 is real
        t   = min(max((Dl - opts.rampLo)/(opts.rampHi - opts.rampLo), 0), 1);
        rho = t^2*(3 - 2*t);                             % smoothstep, C1 continuous
        useImage = rho > 0;
    end
    kind = upper(kind);
    if strcmp(kind, 'PATCH')
        [~, ib] = min(abs(f_MHz - [137 255 370 1575.42]));
        epsTab = [60.0 20.0 9.0 2.4];
        Le = lam/(2*sqrt(epsTab(ib)));  W = 1.2*Le;
        useImage = false;  rho = 0;
    end

    thDeg = 0:resDeg:180;  phDeg = 0:resDeg:360;
    [TH, PH] = meshgrid(thDeg*pi/180, phDeg*pi/180);

    % ---- port fields ------------------------------------------------------
    switch kind
        case 'CROSSDIPOLE'
            [pTh{1}, pPh{1}] = localDipole('x', TH, PH);
            [pTh{2}, pPh{2}] = localDipole('y', TH, PH);
        case 'PATCH'
            kLe2 = pi*Le/lam;  kW2 = pi*W/lam;
            [pTh{1}, pPh{1}] = localPatch(TH, PH,        kLe2, kW2);
            [pTh{2}, pPh{2}] = localPatch(TH, PH - pi/2, kLe2, kW2);
        otherwise
            error('SAIAntennaPattern:kind', 'kind must be CROSSDIPOLE or PATCH.');
    end

    % ---- reflector image and finite-deck taper ----------------------------
    wFront = 0.5*(1 - tanh((thDeg - 90)/transDeg));                 % 1 x nTheta
    e      = 10^(-FB_dB/20);
    Ataper = e + (1-e)*wFront;
    if opts.legacy
        AF = 1;
        if useImage, AF = 2*sin(2*pi*hOverLam*cos(TH)); end
    else
        AFimg = 2*sin(2*pi*hOverLam*cos(TH));
        if opts.backBlock
            AF = (1-rho) + rho*AFimg.*wFront;   % image only in front of the plane
        else
            AF = (1-rho) + rho*AFimg;
        end
    end
    % bare element power, kept for the standoff efficiency below
    Pbare = 0;
    for k = 1:2, Pbare = Pbare + pTh{k}.^2 + pPh{k}.^2; end
    for k = 1:2
        pTh{k} = pTh{k}.*AF.*Ataper;
        pPh{k} = pPh{k}.*AF.*Ataper;
    end

    % ---- diffraction floor, applied once to the port fields (magnitude, ---
    %      direction-preserving; see localVectorFloor) ---------------------
    pk = 0;
    for k = 1:2, pk = max([pk, max(abs(pTh{k}(:))), max(abs(pPh{k}(:)))]); end
    fF = 10^(floor_dB/20)*pk;
    for k = 1:2
        [pTh{k}, pPh{k}] = localVectorFloor(pTh{k}, pPh{k}, fF);
    end

    % ---- Ludwig-3 projection onto the two linear ports --------------------
    cp = cos(PH);  sp = sin(PH);
    for k = 1:2
        gx{k} = pTh{k}.*cp - pPh{k}.*sp;
        gy{k} = pTh{k}.*sp + pPh{k}.*cp;
    end
    sg0 = sign(gx{1}(1,1));  if sg0 == 0, sg0 = 1; end
    XX = sg0*gx{1};  XY = sg0*gy{1};  YX = sg0*gx{2};  YY = sg0*gy{2};
    s2 = max([abs(XX(:)); abs(YY(:))]);
    A = struct('XX', XX/s2, 'XY', XY/s2, 'YX', YX/s2, 'YY', YY/s2, ...
               'th', thDeg, 'ph', phDeg, 'f_MHz', f_MHz, 'lam', lam, ...
               'DoverLam', Dl, 'FB_model_dB', FB_dB, 'hOverLam', hOverLam, ...
               'useImage', useImage, 'rho', rho, 'legacy', opts.legacy, ...
               'kind', kind, 'D_gp', D_gp, 'h_m', h_m);

    % ---- circular basis: quadrature feed on the SAME port fields ----------
    % q = a*exp(+j*(pi/2+err)) gives RHCP toward nadir; the minus sign gives
    % LHCP toward nadir, the SAI single-port choice (see the header).
    ampImb_dB = 0.5;  phaseErr_deg = 5.0;
    q   = 10^(ampImb_dB/20) * exp(-1j*(pi/2 + phaseErr_deg*pi/180));
    Eth = pTh{1} + q*pTh{2};
    Eph = pPh{1} + q*pPh{2};
    ER  = abs((Eth - 1j*Eph)/sqrt(2));
    EL  = abs((Eth + 1j*Eph)/sqrt(2));
    scp = max([ER(:); EL(:)]);
    A.L = EL/scp;  A.R = ER/scp;

    % ---- metrics ------------------------------------------------------------
    P   = A.XX.^2 + A.XY.^2;
    dth = thDeg*pi/180;  dph = phDeg*pi/180;
    inner = trapz(dth, P.*sin(dth), 2);
    tot   = trapz(dph(:), inner(:));
    A.D0_dBi    = 10*log10(4*pi*max(P(:))/tot);

    % ---- standoff loading: realized-gain penalty from the model's own AF ------
    % eta_rad = element-weighted mean of |AF|^2 over the sphere = radiated power
    % relative to the same element in free space at the same current. It falls
    % toward zero as the element approaches the plane. Judged against a loss
    % resistance and referenced to free space so it is 0 dB when the reflector
    % does not load the element (plain ohmic loss stays in payload.L_misc_dB).
    if opts.legacy || strcmp(kind, 'PATCH')
        A.eta_rad = 1;  A.L_standoff_dB = 0;
    else
        num = trapz(dth, (Pbare.*abs(AF).^2).*sin(dth), 2);
        den = trapz(dth,  Pbare.*sin(dth), 2);
        A.eta_rad = trapz(dph(:), num(:))/trapz(dph(:), den(:));
        rl  = opts.R_loss_ohm/opts.R_dipole_ohm;
        A.L_standoff_dB = 10*log10( (A.eta_rad/(A.eta_rad + rl)) / (1/(1 + rl)) );
    end
    A.G_realized_dBi = A.D0_dBi + A.L_standoff_dB;
    % the circular feed is the same element, so it carries the same loading
    A.FB_bore_dB = 10*log10(max(P(:,1))/max(max(P(:,end)), eps));
    iF = thDeg <= 60;  iB = thDeg >= 120;
    A.FB_cone_dB = 10*log10(mean(P(:,iF), 'all')/mean(P(:,iB), 'all'));
    %%% SKY FRACTION (2026-10-15).  Every direction beyond the Earth limb sees
    %%% the sky, not only the 60 deg cone around zenith.  At 500 km the limb is
    %%% 68 deg from nadir; counting 68-120 deg as 260 K Earth understated the
    %%% 137 MHz antenna temperature by about 1.9 dB (galactic sky ~1000 K) and
    %%% overstated it at 255/370 MHz and L-band.  FB_cone keeps the 0-60 and
    %%% 120-180 cones: it is a pattern metric, not a noise one.
    A.thLimb_deg = asind(6371/(6371 + opts.alt_km));
    iS = thDeg > A.thLimb_deg;
    innerB = trapz(dth(iS), P(:,iS).*sin(dth(iS)), 2);
    A.backFrac = trapz(dph(:), innerB(:))/tot;
    [~, i90 ] = min(abs(phDeg - 90));  [~, i180] = min(abs(phDeg - 180));
    [~, i270] = min(abs(phDeg - 270));
    A.hpbwE = localHpbw(abs(A.XX(1,:)),    abs(A.XX(i180,:)), resDeg);
    A.hpbwH = localHpbw(abs(A.XX(i90,:)),  abs(A.XX(i270,:)), resDeg);
    A.xpol_dB = 20*log10(max(max(abs(A.XY(:,iF))))/max(max(abs(A.XX(:,iF)))));

    Pcp = A.L.^2 + A.R.^2;
    innerCP = trapz(dth, Pcp.*sin(dth), 2);  totCP = trapz(dph(:), innerCP(:));
    A.D0cp_dBi = 10*log10(4*pi*max(A.L(:))^2/totCP);
    A.G_realized_cp_dBi = A.D0cp_dBi + A.L_standoff_dB;
    hi = max(A.L(1,1), A.R(1,1));  lo = min(A.L(1,1), A.R(1,1));
    if hi - lo < 1e-6*hi, A.AR0_dB = Inf; else, A.AR0_dB = 20*log10((hi+lo)/(hi-lo)); end
    A.FBcp_dB = 10*log10(mean(A.L(:,iF).^2, 'all')/mean(A.R(:,iB).^2, 'all'));
    innerB2 = trapz(dth(iS), Pcp(:,iS).*sin(dth(iS)), 2);
    A.backFrac_cp = trapz(dph(:), innerB2(:))/totCP;
end

% ------------------------------------------------------------------------
function [Eth, Eph] = localDipole(ax, TH, PH)
    st = sin(TH); ct = cos(TH); sp = sin(PH); cp = cos(PH);
    if strcmp(ax, 'x'), u = st.*cp;  pth = ct.*cp;  pph = -sp;
    else,               u = st.*sp;  pth = ct.*sp;  pph =  cp;
    end
    den = 1 - u.^2;
    C   = cos(pi*u/2)./den;
    C(abs(den) < 1e-9) = pi/4;
    Eth = C.*pth;  Eph = C.*pph;
end

function [Eth, Eph] = localPatch(TH, PH, kLe2, kW2)
    ct = cos(TH); st = sin(TH); sp = sin(PH); cp = cos(PH);
    u = st.*cp;  v = st.*sp;
    F = localSinc(kW2*v).*cos(kLe2*u);
    Eth = -cp.*F;  Eph = ct.*sp.*F;
end

function y = localSinc(x)
    y = ones(size(x));  m = abs(x) > 1e-12;  y(m) = sin(x(m))./x(m);
end

function [a, b] = localVectorFloor(a, b, f)
%  Raise the port-field MAGNITUDE to at least f while keeping its polarization
%  direction:  (a,b) -> (a,b)*sqrt(|E|^2+f^2)/|E|,  |E|^2 = a^2+b^2.
%  No sign is introduced, so the floored field is continuous wherever the bare
%  field is.  The former per-component signed floor, sign(v)*sqrt(v^2+f^2/2),
%  flipped sign at every zero crossing of a component; for the dipoles
%  p_theta ~ cos(theta) crosses zero at the horizon, which put a 2-3 dB step
%  in L, R, XX and YY at theta = 90 deg (found 2026-10-09).  The floor level
%  in power is unchanged, |E|^2 >= f^2, and it is still applied once to the
%  port fields, so the linear and circular outputs stay mutually consistent.
    E = sqrt(a.^2 + b.^2);
    s = sqrt(E.^2 + f^2)./max(E, 1e-9*f);
    a = a.*s;  b = b.*s;
    %  An exact null (the dipole's own axis, 3 grid points at theta = 90) has
    %  no polarization direction to scale; put the floor on theta-hat there,
    %  the continuous choice for a dipole near its axis.  Without this the
    %  guard above left those points at about -87 dB (found 2026-10-10).
    tiny = E < 1e-9*f;
    a(tiny) = f;  b(tiny) = 0;
end

function bw = localHpbw(cutA, cutB, res)
    cut = [fliplr(cutB(2:end)), cutA];
    pdB = 20*log10(max(cut, 1e-12)/max(cut));
    [~, i0] = max(pdB);
    iu = i0;  while iu < numel(pdB) && pdB(iu+1) >= -3, iu = iu + 1; end
    il = i0;  while il > 1          && pdB(il-1) >= -3, il = il - 1; end
    if il == 1 && iu == numel(pdB), bw = Inf; else, bw = (iu - il)*res; end
end
