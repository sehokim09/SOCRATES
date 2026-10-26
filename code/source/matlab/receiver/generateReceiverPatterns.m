function [saiAnt, T] = generateReceiverPatterns(payload, opts)
% function generateReceiverPatterns
%
%   Write the pattern files of the SAI receive antenna and summarize them.
%
%   [saiAnt, T] = generateReceiverPatterns(payload)
%   [saiAnt, T] = generateReceiverPatterns(payload, opts)
%
%   payload  solved receiver payload (the struct of solved_payload.m); its
%            D_gp_m, h_m, D_deck_m and alt_Rx_m set the antenna geometry.
%            [] uses the built-in defaults (1.20 m reflector, 0.20 m standoff).
%   opts     optional struct; missing fields take these defaults:
%     ReflectorSize_m  []           overrides payload.D_gp_m
%     Standoff_m       []           overrides payload.h_m
%     patDir           ant_pat/sai  pattern folder; circular files go to its cp/
%     write            true         write the pattern files
%     verbose          false        print the parameter tables
%
%   The I/P-band elements are crossed half-wave dipoles over the reflector,
%   the L-band element a dual-fed patch on the bare deck. For each element
%   both feed bases are written from the same port fields:
%     <TAG>_gn{XX,XY,YX,YY}.csv       dual-linear set (ports X and Y)
%     cp/<TAG>_{L,R}_norm_vol_num.csv circular set, LHCP toward nadir
%   with TAG = CROSSDIPOLE_137, CROSSDIPOLE_255, CROSSDIPOLE_370, PATCH_1575.
%   All pattern physics is in SAIAntennaPattern.
%
%   saiAnt  antenna summary that evaluateReceiver checks against the payload
%           (the struct of sai_antenna.m), per transmitter ORBCOMM, UFO, MUOS,
%           GPS, Galileo, GLONASS, Beidou; values are rounded as sai_antenna.m
%           stores them, so the evaluation sees the same numbers
%   T       per-element parameter table (peak gains, F/B, beamwidths, ...)
%
%   Ported from GenerateSAIAntPatterns.m of the ASR SAI design (2026-10-21).
%   It no longer writes sai_antenna.m or draws the pattern figure: saiAnt and
%   T are returned instead.
%
%   See also evaluateReceiver, SAIAntennaPattern
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

    if nargin < 2, opts = struct(); end
    def = struct('ReflectorSize_m', [], 'Standoff_m', [], ...
                 'patDir', fullfile('ant_pat', 'sai'), 'write', true, 'verbose', false);
    for f = string(fieldnames(def)).'
        if ~isfield(opts, f), opts.(f) = def.(f); end
    end
    if opts.verbose
        vprintf = @(varargin) fprintf(varargin{:});
    else
        vprintf = @(varargin) [];
    end

    %% Geometry from the payload unless overridden
    src = 'arguments';
    D_DECK = sqrt(0.10*0.20);   % fallback: 1U x 2U end face of the 6U
    ALT_KM = 500;               % fallback receiver altitude, km (sets the Earth-limb sky fraction)
    if isempty(opts.ReflectorSize_m) || isempty(opts.Standoff_m)
        if ~isempty(payload)
            if isempty(opts.ReflectorSize_m), opts.ReflectorSize_m = payload.D_gp_m; end
            if isempty(opts.Standoff_m),      opts.Standoff_m      = payload.h_m;    end
            if isfield(payload, 'D_deck_m'), D_DECK = payload.D_deck_m; end
            if isfield(payload, 'alt_Rx_m'), ALT_KM = payload.alt_Rx_m/1e3; end
            src = 'the payload';
        else
            if isempty(opts.ReflectorSize_m), opts.ReflectorSize_m = 1.20; end
            if isempty(opts.Standoff_m),      opts.Standoff_m      = 0.20; end
            src = 'built-in defaults (no payload)';
        end
    end
    D = opts.ReflectorSize_m;  h = opts.Standoff_m;

    vprintf('\n===== SAI REPRESENTATIVE ANTENNAS  (geometry from %s) =====\n', src);
    vprintf('  I/P : crossed half-wave dipoles, %.2f m above a %.2f m square reflector\n', h, D);
    vprintf('  L   : single wideband dual-fed patch on the bare 6U deck (%.3f m area-equivalent)\n', D_DECK);

    BAND = {'I','U','P','L'};
    fMHz = [137, 255, 370, 1575.42];
    KIND = {'CROSSDIPOLE','CROSSDIPOLE','CROSSDIPOLE','PATCH'};
    DGP  = [D, D, D, D_DECK];
    TAG  = {'CROSSDIPOLE_137','CROSSDIPOLE_255','CROSSDIPOLE_370','PATCH_1575'};

    patDir = opts.patDir;
    cpDir = fullfile(patDir, 'cp');
    if opts.write
        if ~exist(patDir, 'dir'), mkdir(patDir); end
        if ~exist(cpDir, 'dir'),  mkdir(cpDir);  end
    end

    %% Pattern files and parameter table
    rows = cell(0, 17);
    for i = 1:4
        A = SAIAntennaPattern(KIND{i}, fMHz(i), DGP(i), h, [], struct('alt_km', ALT_KM));
        if opts.write
            for f = {'XX','XY','YX','YY'}
                writePattern(fullfile(patDir, sprintf('%s_gn%s.csv', TAG{i}, f{1})), A.(f{1}));
            end
            writePattern(fullfile(cpDir, sprintf('%s_L_norm_vol_num.csv', TAG{i})), A.L);
            writePattern(fullfile(cpDir, sprintf('%s_R_norm_vol_num.csv', TAG{i})), A.R);
        end
        rows(end+1,:) = {BAND{i}, lower(KIND{i}), fMHz(i), A.DoverLam, A.G_realized_dBi, ...
            A.FB_bore_dB, A.FB_cone_dB, A.hpbwE, A.hpbwH, 100*A.backFrac, ...
            A.G_realized_cp_dBi, A.AR0_dB, A.FBcp_dB, TAG{i}, ...
            A.D0_dBi, A.L_standoff_dB, A.rho}; %#ok<AGROW>
    end
    % Peak_dBi / PeakCP_dBi are realized gains (directivity plus the
    % reflector-loading penalty); D0_dBi, L_standoff_dB and rho show the split.
    T = cell2table(rows, 'VariableNames', {'Band','Element','f_MHz','DoverLambda', ...
        'Peak_dBi','FB_bore_dB','FB_cone_dB','HPBW_E_deg','HPBW_H_deg','Back_pct', ...
        'PeakCP_dBi','AR0_dB','FBcp_dB','FileTag','D0_dBi','L_standoff_dB','rho'});

    vprintf('\n===== ANTENNA PARAMETERS =====\n');
    vprintf('%-5s %-14s %8s %7s %9s %8s %8s %9s %7s\n', ...
        'band','element','f [MHz]','D/lam','peak','F/B bore','F/B cone','HPBW E/H','back%%');
    for i = 1:height(T)
        vprintf('%-5s %-14s %8.2f %7.3f %6.2f dBi %5.1f dB %5.1f dB %4.0f/%-4.0f %6.1f\n', ...
            T.Band{i}, T.Element{i}, T.f_MHz(i), T.DoverLambda(i), T.Peak_dBi(i), ...
            T.FB_bore_dB(i), T.FB_cone_dB(i), T.HPBW_E_deg(i), T.HPBW_H_deg(i), T.Back_pct(i));
    end
    for i = 1:height(T)
        vprintf('  %-5s  D0 %6.2f dBi  standoff %5.2f dB  rho %.2f\n', ...
            T.Band{i}, T.D0_dBi(i), T.L_standoff_dB(i), T.rho(i));
    end
    vprintf(['  peak = per-port co-polar REALIZED gain (D0 + standoff); F/B cone = 0-60 over 120-180 deg\n' ...
             '  mean power, the quantity the SAI tests feel; delivered G_R/G_D adds\n' ...
             '  the 3.01 dB direct-path polarization mismatch.\n']);
    vprintf('\n  same elements fed in quadrature (LHCP toward nadir), for RX_POL = "LHCP":\n');
    vprintf('%-5s %9s %8s %12s\n', 'band', 'peak L', 'AR0', 'L(0-60)/R(120-180)');
    for i = 1:height(T)
        vprintf('%-5s %6.2f dBi %5.1f dB %9.1f dB\n', T.Band{i}, T.PeakCP_dBi(i), T.AR0_dB(i), T.FBcp_dB(i));
    end

    %% Antenna summary, per transmitter, rounded as sai_antenna.m stores it
    m7 = [1 2 3 4 4 4 4];        % element of ORBCOMM, UFO, MUOS, GPS, Galileo, GLONASS, Beidou
    saiAnt = struct();
    saiAnt.D_gp_m    = stored('%.6g', D);
    saiAnt.h_m       = stored('%.6g', h);
    saiAnt.order     = ["ORBCOMM" "UFO" "MUOS" "GPS" "Galileo" "GLONASS" "Beidou"];
    saiAnt.G_ant_dB  = stored('%.4f', T.Peak_dBi(m7));
    saiAnt.backFrac  = stored('%.5f', T.Back_pct(m7)/100);
    saiAnt.FBcone_dB = stored('%.3f', T.FB_cone_dB(m7));
    saiAnt.G_cp_dB   = stored('%.4f', T.PeakCP_dBi(m7));     % circular feed (LHCP toward nadir)
    saiAnt.FBcp_dB   = stored('%.3f', T.FBcp_dB(m7));
    if opts.write
        vprintf('  wrote %s/*.csv, %s/*.csv\n', patDir, cpDir);
    end
end

function writePattern(fname, M)
    fid = fopen(fname, 'w');
    fmt = [repmat('%.6f,', 1, size(M,2)-1), '%.6f\n'];
    fprintf(fid, fmt, M.');
    fclose(fid);
end

function v = stored(fmt, x)
% STORED  x as read back from its text in sai_antenna.m, row vector.
    v = arrayfun(@(xi) str2double(sprintf(fmt, xi)), x(:).');
end
