function [g_D, g_R, G_D_dB, G_R_dB] = LookupAntGainDualPol(Tx, data, G_ant_dB, antDir, cfgMap)
%LOOKUPANTGAINDUALPOL Dual-linear (2x2) receive antenna response per specular point.
%
%   [g_D, g_R, G_D_dB, G_R_dB] = LookupAntGainDualPol(Tx, data, G_ant_dB)
%
%   Dual-polarization replacement for LookupAntGain. Where the scalar version
%   returns one gain per path, this returns the full 2x2 polarimetric response
%
%       g = [ gXX  gXY ;     row    = receive port (X along +x, Y along +y)
%             gYX  gYY ]     column = incident component, Ludwig-3 basis
%
%   which is the same matrix SOCRATES' LookupAntPattern fills into g[2][2],
%   in the basis tanUnitVec builds for polRx = 'X'.
%
% Inputs
%   Tx         string array of transmitter names, as for LookupAntGain
%   data       struct of tables from readSpDataNc
%   G_ant_dB   per-Tx peak CO-POLAR port gain in dBi, same order as Tx
%   antDir     pattern directory (default ./antPatternFiles/nominal)
%   cfgMap     struct mapping each Tx name to .tag and .useDirect
%
% Outputs
%   g_D, g_R   struct per Tx with fields XX, XY, YX, YY, each [nPts x 1].
%              SIGNED field (voltage) values normalized to a co-polar peak of
%              1. The signs are physical: rotating from the port frame into
%              the local h/v scattering frame is a signed linear combination,
%              so do not take abs() before that rotation.
%   G_D_dB,    struct per Tx with fields port1, port2, each [nPts x 1], in dBi
%   G_R_dB         port1 = 10*log10(|gXX|^2 + |gXY|^2) + G_ant_dB(iTx)
%                  port2 = 10*log10(|gYX|^2 + |gYY|^2) + G_ant_dB(iTx)
%              These are the per-port TOTAL power responses, i.e. what each
%              channel can collect toward that direction, so they sit a few
%              hundredths of a dB above G_ant_dB at boresight (the cross-pol
%              term). Do NOT add the two ports to get a "total gain": the
%              ports capture complementary components, so their sum is only
%              meaningful once the incident polarization has been applied
%              via g.
%
% Architecture
%   I- and P-band are single-antenna SAI: the direct and reflected paths read
%   the SAME four files at different angles, with the direct-path angles
%   mapped into the nadir antenna frame by (pi-th, pi-ph) -- a 180 deg
%   rotation about y, the same transform LookupAntGain already used.
%   L-band is nadir-only GNSS-R (the PRN replica is the reference), so the
%   direct path returns NaN, as in LookupAntGain.
%
% Notes
%   The angular resolution is DERIVED from each file (180/(nTheta-1)) rather
%   than hardcoded. LookupAntGain hardcodes resDeg = 1 while the files it
%   points at (default_ant_pat_gn**, 121 x 61) are 3 deg, which silently
%   mis-maps every look angle -- 30 deg reads the 90 deg entry, and anything
%   past 60 deg clamps to the back-lobe floor.
%
%   No plotting happens here. Use PlotNominalAntPatterns for that.
%
% Example
%   G_ant_dB = [3.0, 4.1, 4.1, 7.0, 7.0, 7.0, 7.0];   % per-port peak, dBi
%   [g_D, g_R, G_D_dB, G_R_dB] = LookupAntGainDualPol(Tx, data, G_ant_dB);
%   histogram(G_R_dB.MUOS.port1);

    if nargin < 4 || isempty(antDir)
        antDir = fullfile('.','antPatternFiles','nominal');
    end
    if nargin < 5 || isempty(cfgMap)
        % UFO is 250 MHz and is mapped to the 370 MHz set. To model it
        % properly, add a band to localConfig in GenerateNominalAntPatterns
        % and regenerate as CROSSDIPOLE_250.
        cfgMap = struct( ...
            'ORBCOMM', struct('tag','CROSSDIPOLE_137', 'useDirect', true ), ...
            'UFO',     struct('tag','CROSSDIPOLE_370', 'useDirect', true ), ...
            'MUOS',    struct('tag','CROSSDIPOLE_370', 'useDirect', true ), ...
            'GPS',     struct('tag','PATCH_1575',      'useDirect', false), ...
            'Galileo', struct('tag','PATCH_1575',      'useDirect', false), ...
            'GLONASS', struct('tag','PATCH_1575',      'useDirect', false), ...
            'Beidou',  struct('tag','PATCH_1575',      'useDirect', false));
    end
    if numel(G_ant_dB) < numel(Tx)
        error('LookupAntGainDualPol:gainSize', ...
            'G_ant_dB has %d entries for %d transmitters.', numel(G_ant_dB), numel(Tx));
    end

    g_D = struct();  g_R = struct();  G_D_dB = struct();  G_R_dB = struct();
    cache = struct();

    for iTx = 1:numel(Tx)
        name = char(Tx(iTx));
        if ~isfield(cfgMap, name)
            error('LookupAntGainDualPol:noConfig', ...
                'No antenna configuration for Tx "%s". Add it to cfgMap.', name);
        end
        cfg = cfgMap.(name);

        if ~isfield(cache, cfg.tag)
            cache.(cfg.tag) = localLoadSet(antDir, cfg.tag);
        end
        P = cache.(cfg.tag);
        d = data.(Tx(iTx));

        % ---- reflected path: angles are already in the nadir antenna frame
        g_R.(Tx(iTx))    = localLookup(P, d.lookAngElR, d.lookAngAzR);
        G_R_dB.(Tx(iTx)) = localPortGain(g_R.(Tx(iTx)), G_ant_dB(iTx));

        % ---- direct path
        %%% CORRECTED. lookAngElD is stored in the SAME nadir frame as
        %%% lookAngElR (it runs 91 to 153 deg, i.e. above the horizon, and exact
        %%% spherical geometry reproduces it as the angle from nadir to the
        %%% transmitter to within 0.3 deg). The former (pi-th, pi-ph) was the
        %%% transform from a ZENITH-referenced frame, inherited from the
        %%% two-antenna layout with a separate zenith element; applied here it
        %%% moved the direct arrival into the FRONT hemisphere, so the pattern's
        %%% back lobe was never consulted for G_D and single-antenna SAI was not
        %%% actually being modelled. The single nadir-facing element receives
        %%% the direct signal through its back hemisphere at the stored angles.
        if cfg.useDirect
            g_D.(Tx(iTx))    = localLookup(P, d.lookAngElD, d.lookAngAzD);
            G_D_dB.(Tx(iTx)) = localPortGain(g_D.(Tx(iTx)), G_ant_dB(iTx));
        else
            v = nan(numel(d.lookAngElD), 1);
            g_D.(Tx(iTx))    = struct('XX', v, 'XY', v, 'YX', v, 'YY', v);
            G_D_dB.(Tx(iTx)) = struct('port1', v, 'port2', v);
        end
    end
end

% ------------------------------------------------------------------------
function P = localLoadSet(antDir, tag)
%LOCALLOADSET Load the four polarimetric files for one configuration.
    comp = {'XX','XY','YX','YY'};
    P = struct();
    for i = 1:numel(comp)
        f = fullfile(antDir, sprintf('%s_gn%s.csv', tag, comp{i}));
        if ~exist(f, 'file')
            error('LookupAntGainDualPol:missingFile', 'Missing pattern file: %s', f);
        end
        A = readmatrix(f);
        if i == 1
            [nPhi, nTheta] = size(A);
            res = 180/(nTheta - 1);                     % derived, not hardcoded
            if abs((nPhi - 1)*res - 360) > 1e-6
                warning('LookupAntGainDualPol:gridMismatch', ...
                    ['%s is %d x %d: theta implies %.4g deg resolution but phi ' ...
                     'then spans %.4g deg, not 360. Check that rows are phi and ' ...
                     'columns are theta.'], tag, nPhi, nTheta, res, (nPhi-1)*res);
            end
            P.resDeg = res;  P.nPhi = nPhi;  P.nTheta = nTheta;
        elseif ~isequal(size(A), [P.nPhi, P.nTheta])
            error('LookupAntGainDualPol:sizeMismatch', ...
                '%s_gn%s.csv is %s but %s_gnXX.csv is %s.', tag, comp{i}, ...
                mat2str(size(A)), tag, mat2str([P.nPhi, P.nTheta]));
        end
        P.(comp{i}) = A;
    end
end

% ------------------------------------------------------------------------
function g = localLookup(P, th, ph)
%LOCALLOOKUP Nearest-neighbour lookup of all four terms in one index pass.
%   th, ph in radians. Non-finite angles (no specular point) return NaN.
    R2D = 180/pi;
    th = double(th(:));  ph = double(ph(:));

    bad = ~isfinite(th) | ~isfinite(ph);
    th(bad) = 0;  ph(bad) = 0;

    ph = mod(ph + 2*pi, 2*pi);
    th = min(max(th, 0), pi);      % pi - El can land just outside [0, pi]

    iTheta = round(th*R2D / P.resDeg) + 1;
    iPhi   = round(ph*R2D / P.resDeg) + 1;
    iTheta = max(1, min(iTheta, P.nTheta));
    iPhi   = max(1, min(iPhi,   P.nPhi));

    idx = sub2ind([P.nPhi, P.nTheta], iPhi, iTheta);

    g = struct();
    for c = {'XX','XY','YX','YY'}
        v = P.(c{1})(idx);
        v(bad) = NaN;
        g.(c{1}) = v;
    end
end

% ------------------------------------------------------------------------
function G = localPortGain(g, Gpeak_dB)
%LOCALPORTGAIN Per-port total power response in dBi. G_ant_dB is the peak
%   CO-POLAR gain and the files carry a co-polar peak of exactly 1, so at
%   boresight this returns Gpeak_dB plus the cross-polar contribution --
%   +0.034 dB at I-band, +0.003 dB at L-band. That is physical, not an
%   error: the port does collect the leakage. The exact power in a channel
%   depends on the incident polarization and must be formed from g.
    G = struct();
    G.port1 = 10*log10(abs(g.XX).^2 + abs(g.XY).^2) + Gpeak_dB;
    G.port2 = 10*log10(abs(g.YX).^2 + abs(g.YY).^2) + Gpeak_dB;
end
