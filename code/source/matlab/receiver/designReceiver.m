function [DesignParamsObj, res] = designReceiver(projName, DesignParamsObj, opts)
% function designReceiver
%
%   Design the SAI receiver of a project: optimize the payload, write its
%   antenna pattern files and evaluate it, and keep the results in
%   DesignParamsObj.receiver.
%
%   DesignParamsObj = designReceiver(projName, DesignParamsObj)
%   [DesignParamsObj, res] = designReceiver(projName, DesignParamsObj, opts)
%
%   projName         project folder name under sim/
%   DesignParamsObj  its DesignParams; receiver.inputs sets the design and
%                    its window is the inter-frequency observation window
%   opts             optional struct; missing fields take these defaults:
%     patDir   ant_pat/sai   pattern folder; circular files go to its cp/
%     cdfFile  earthdata/reflectivity/reflectivity_CDF_v2.mat
%     save     true          write sim/<projName>/out/Receiver_<projName>.mat
%     verbose  false         print the diagnostics of the three steps
%     progressFcn  []        called as progressFcn(frac, msg) at each stage, frac
%                            the fraction of the design done (0 to 1); e.g.
%                            @(f, m) set(dlg, 'Value', f, 'Message', m)
%
%   Progress is printed to the command window: one line per stage with the
%   elapsed time, then the solved channels and the pass fractions.
%
%   The steps are optimizeReceiver, generateReceiverPatterns and
%   evaluateReceiver. The geometry sample is the first receiver of the
%   smallest one-plane Walker constellation at each design inclination, at
%   the pinned receiver altitude (receiver.inputs.FIX(8)), as Step 1 writes
%   it below the Large File Storage Root Directory; receiver.inputs.geomFiles
%   overrides the sample and geomDir the folder, which holds the files either
%   in that layout or directly. On return
%     receiver.payload  the solved payload
%     receiver.antenna  the antenna summary of the pattern files
%     receiver.model    the SAI Observation Model that createInpDesign writes
%                       into Inp_Design.txt for the C code; its CDF tables
%                       go next to cdfFile (<name>_X.txt, _Y.txt, _L.txt)
%     receiver.runTag   set to the project name without its punctuation if empty
%   res holds the results saved to Receiver_<projName>.mat: inputs, payload,
%   info (optimizer), antenna, antTable, eval (evaluator results without
%   the per-specular-point arrays) and model.
%
%   See also optimizeReceiver, generateReceiverPatterns, evaluateReceiver,
%   createInpDesign
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
    def = struct('patDir', fullfile('ant_pat', 'sai'), ...
                 'cdfFile', fullfile('earthdata', 'reflectivity', 'reflectivity_CDF_v2.mat'), ...
                 'save', true, 'verbose', false, 'progressFcn', []);
    for f = string(fieldnames(def)).'
        if ~isfield(opts, f), opts.(f) = def.(f); end
    end
    in = DesignParamsObj.receiver.inputs;
    if in.RX_POL ~= "HVC"
        error('designReceiver:pol', 'RX_POL is "%s"; SOCRATES needs the HVC design (ports X, Y and L).', in.RX_POL);
    end

    t0 = tic;
    say = @(frac, msg) report(frac, msg, t0, opts.progressFcn);
    stage = @(a, b, label) @(frac, msg) say(a + (b - a)*frac, label + ": " + msg);
    fprintf('\n===== DESIGN RECEIVER: %s =====\n', projName);

    % 1  Payload
    GEOM = geometrySample(DesignParamsObj);
    fprintf('  geometry sample (%d files):\n', numel(GEOM));
    for ig = 1:numel(GEOM)
        fprintf('    %-7s %s%s\n', GEOM(ig).label, GEOM(ig).file, repmat('  (not found)', 1, ~isfile(GEOM(ig).file)));
    end
    optOpts = struct('GEOM', GEOM, 'RX_POL', in.RX_POL, 'RMSE_TARGET', in.RMSE_target, ...
        'T_WINDOW_H', DesignParamsObj.window, 'RES_MAX_km', in.resMax_km, 'EQUAL_RES', in.equalRes, ...
        'ROUND_IMPL', in.roundImpl, 'T_I_STEP', in.T_I_step, 'DWELL_EPOCH', in.dwellEpoch, ...
        'FIX', in.FIX, 'NADIR_FACE', in.nadirFace, 'GNSS_EIRP_MODE', in.gnssEirpMode, ...
        'GNSS_EIRP_MARGIN_dB', in.gnssEirpMargin_dB, 'cdfFile', opts.cdfFile, 'verbose', opts.verbose, ...
        'progressFcn', stage(0, 0.3, "1/3 payload"));
    optOpts.GNSS_SIG_OPTIONS = in.gnssSignals;     % struct() would expand a cell
    [payload, info] = optimizeReceiver(optOpts);
    say(0.3, "1/3 payload: " + info.summary);

    % 2  Antenna pattern files
    say(0.3, "2/3 antenna patterns: writing to " + opts.patDir);
    [saiAnt, antTable] = generateReceiverPatterns(payload, ...
        struct('patDir', opts.patDir, 'verbose', opts.verbose));

    % 3  Evaluation and the observation model
    R = evaluateReceiver(payload, saiAnt, struct('RX_POL', in.RX_POL, ...
        'T_h', DesignParamsObj.window, 'RMSE_target', in.RMSE_target, 'RMSE_target_L', in.RMSE_target_L, ...
        'patDir', opts.patDir, 'cdfFile', opts.cdfFile, ...
        'source', "optimizeReceiver " + info.generated, 'verbose', opts.verbose, ...
        'progressFcn', stage(0.31, 0.98, "3/3 evaluation")));
    % The C code reads the CDF tables as text files next to opts.cdfFile
    [cdfDir, cdfBase] = fileparts(opts.cdfFile);
    R.model.cdfName = cdfDir + "/" + cdfBase + "_" + ["X" "Y" "L"] + ".txt";

    DesignParamsObj.receiver.payload = payload;
    DesignParamsObj.receiver.antenna = saiAnt;
    DesignParamsObj.receiver.model = R.model;
    if strlength(DesignParamsObj.receiver.runTag) == 0
        DesignParamsObj.receiver.runTag = string(regexprep(projName, '[^A-Za-z0-9]', ''));
    end

    res = struct('inputs', in, 'payload', payload, 'info', info, 'antenna', saiAnt, ...
        'antTable', antTable, 'eval', perTransmitter(rmfield(R, {'grp', 'cdf', 'model'}), numel(R.Tx)), ...
        'model', R.model);
    if opts.save
        dirOut = fullfile('sim', projName, 'out');
        if ~isfolder(dirOut), mkdir(dirOut); end
        fileOut = fullfile(dirOut, sprintf('Receiver_%s.mat', projName));
        say(0.99, "saving " + fileOut);
        save(fileOut, '-struct', 'res');
    end
    say(1, "done");

    % Solved channels and the fraction of observations each product passes
    fprintf('  %-8s %5s %8s %8s %6s %6s\n', 'Tx', 'Nc', 'T_I[ms]', 'N_incoh', 'H&V', 'LHCP');
    for iTx = 1:R.model.nTx
        t = R.model.tx(iTx);
        fprintf('  %-8s %5d %8.1f %8d %6.2f %6.2f\n', t.name, t.Nc, t.TI_s*1e3, t.Nincoh, ...
            R.evalSum.HV_hi(iTx), R.evalSum.LHCP(iTx));
    end
end

function report(frac, msg, t0, progressFcn)
% REPORT  One progress line in the command window, also passed to
%   progressFcn (an app's progress dialog) when given.
    fprintf('  [%6.1f s] %s\n', toc(t0), msg);
    if ~isempty(progressFcn)
        progressFcn(frac, char(msg));
        drawnow limitrate
    end
end

function GEOM = geometrySample(DPO)
% GEOMETRYSAMPLE  Single-receiver specular-point files the payload is solved on.
    in = DPO.receiver.inputs;
    alt_km = in.FIX(8);
    if isnan(alt_km), alt_km = DPO.walker.alt(1); end
    files = string(in.geomFiles);
    files = files(strlength(files) > 0);
    if isempty(files)
        if DPO.consType ~= "Walker"
            error('designReceiver:geom', 'Set receiver.inputs.geomFiles for a %s constellation.', DPO.consType);
        end
        % Step 1 writes <root>/<nRx>_<alt>_<inc>_<nPlane>/<nRx>_<alt>_<inc>_<nPlane>_Rx1.nc
        name = min(DPO.walker.nRx) + "_" + round(alt_km) + "_" + string(DPO.walker.inc) + "_1";
        files = fullfile(string(DPO.largeFilePath), name, name + "_Rx1.nc");
        labels = string(DPO.walker.inc);
        labels(labels ~= "SSO") = labels(labels ~= "SSO") + " deg";
    else
        [~, labels] = fileparts(files);
    end
    if strlength(in.geomDir) > 0
        % the Step 1 layout below geomDir (<name>/<name>_Rx1.nc), else the file directly in it
        [sub, nm, ext] = fileparts(files);
        [~, sub] = fileparts(sub);
        files = fullfile(string(in.geomDir), sub, nm + ext);
        flat = ~isfile(files);
        files(flat) = fullfile(string(in.geomDir), nm(flat) + ext(flat));
    end
    GEOM = struct('file', num2cell(files(:).'), 'label', num2cell(labels(:).'), 'alt_m', alt_km*1e3);
end

function s = perTransmitter(s, nTx)
% PERTRANSMITTER  Keep the per-transmitter results: drop every field with
%   more than nTx elements (the per-specular-point arrays).
    for f = string(fieldnames(s)).'
        v = s.(f);
        if isstruct(v) && isscalar(v)
            s.(f) = perTransmitter(v, nTx);
        elseif numel(v) > nTx
            s = rmfield(s, f);
        end
    end
end
