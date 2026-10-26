function createFoM(projName, DesignParamsObj, performTag, useParallel)
% function createFoM
%
%   Read performance and cost NetCDF files and save FoM_[projName].mat
%   with restart/checkpoint support for long runs.
%
%   performTag (optional) selects <constellation>_perform_<tag>.nc; by default
%   it is the run tag of the SAI Observation Model section of the project's
%   Inp_Design.txt (e.g. "SAI2610").
%
%   useParallel (optional, default false) computes the cases in a parfor loop:
%   each case is saved to its own file in sim/<projName>/out/FoM_<projName>_cases/
%   (the checkpoint; a re-run skips cases whose file exists), and the case files
%   are merged into the MAT file once all of them exist. Without the Parallel
%   Computing Toolbox the parfor loop runs serially.
%
%   Otherwise raw case-by-case results are written directly to the MAT file.
%   Post-processing is performed only after all cases are completed.
%
%   Every coverage and revisit metric comes from obsFreq (bit k-1 = TxComb k,
%   one 12-hour window per slice of the third dimension). MRT1T/MRT2T (per
%   frequency combination) and SRT1T/SRT2T (per TxComb) are the per-cell Poisson
%   rate censored at T_sim: m_i = T_sim / max(N_i, 1) averaged over land cells,
%   N_i the number of windows (12 h for 1T, 24 h for 2T) with an observation.
%
%   See also missionDesigner
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

    if nargin < 3 || isempty(performTag)
        performTag = readRunTag(projName);
    end
    if nargin < 4 || isempty(useParallel)
        useParallel = false;
    end

    %% Directories
    dir_out = fullfile('sim', projName, 'out');
    dir_largefiles = DesignParamsObj.largeFilePath;
    if ~exist(dir_out, 'dir')
        mkdir(dir_out);
    end

    filename_out = sprintf("FoM_%s.mat", projName);
    filepath_out = fullfile(dir_out, filename_out);

    %% Design variables
    TxCombName = {DesignParamsObj.TxComb.name};
    freqComb = double([DesignParamsObj.freqComb.ID]);
    windowHours1T = DesignParamsObj.window;
    windowHours2T = 2*windowHours1T;

    nTxComb = DesignParamsObj.nTxComb;
    nFreqComb = DesignParamsObj.nFreqComb;

    switch DesignParamsObj.consType
        case 'Walker'
            nRx = DesignParamsObj.walker.nRx;
            alt = DesignParamsObj.walker.alt;
            inc = DesignParamsObj.walker.inc;
            nPlane = DesignParamsObj.walker.nPlane;

            nNrx = length(nRx);
            nAlt = length(alt);
            nInc = length(inc);
            nNplane = length(nPlane);

            consName = strings(nNrx, nAlt, nInc, nNplane);
            dir_spdata = strings(nNrx, nAlt, nInc, nNplane);
            filename_perform = strings(nNrx, nAlt, nInc, nNplane);
            filename_cost = strings(nNrx, nAlt, nInc, nNplane);

            for iNrx = 1:nNrx
                for iAlt = 1:nAlt
                    for iInc = 1:nInc
                        for iNplane = 1:nNplane
                            consName(iNrx, iAlt, iInc, iNplane) = sprintf("%d_%d_%s_%d", ...
                                nRx(iNrx), alt(iAlt), inc(iInc), nPlane(iNplane));
                            dir_spdata(iNrx, iAlt, iInc, iNplane) = ...
                                fullfile(dir_largefiles, consName(iNrx, iAlt, iInc, iNplane));
                            filename_perform(iNrx, iAlt, iInc, iNplane) = ...
                                strcat(consName(iNrx, iAlt, iInc, iNplane), "_perform_", performTag, ".nc");
                            filename_cost(iNrx, iAlt, iInc, iNplane) = ...
                                strcat(consName(iNrx, iAlt, iInc, iNplane), "_cost.nc");
                        end
                    end
                end
            end

        case 'Hybrid'
            nRx = DesignParamsObj.hybrid.nRx;
            alt = DesignParamsObj.hybrid.alt;
            inc = DesignParamsObj.hybrid.inc;
            nPlane = DesignParamsObj.hybrid.nPlane;

            nNrx = 1;
            nAlt = 1;
            nInc = 1;
            nNplane = 1;

            consName = strings(1,1,1,1);
            dir_spdata = strings(1,1,1,1);
            filename_perform = strings(1,1,1,1);
            filename_cost = strings(1,1,1,1);

            consName(1,1,1,1) = string(DesignParamsObj.hybrid.name);
            dir_spdata(1,1,1,1) = fullfile(dir_largefiles, consName(1,1,1,1));
            filename_perform(1,1,1,1) = strcat(consName(1,1,1,1), "_perform_", performTag, ".nc");
            filename_cost(1,1,1,1) = strcat(consName(1,1,1,1), "_cost.nc");

        otherwise
            error('Unsupported constellation type: %s', DesignParamsObj.consType);
    end

    %% Load grid landmask
    opts = delimitedTextImportOptions('Delimiter', ' ', 'VariableTypes', 'double');
    landmask = readmatrix(fullfile('sim', projName, 'out', 'Grid1_landmask.out'), opts);
    landmask = repmat(logical(landmask(:,1:end-1)'), 1, 1, nFreqComb);

    %% Dimension of grid
    info = ncinfo(fullfile(dir_spdata(1,1,1,1), filename_perform(1,1,1,1)), 'obsFreq');
    nCol = info.Size(1);
    nRow = info.Size(2);
    nWindow = info.Size(3);

    %% Global attribute
    nCell = ncreadatt(fullfile(dir_spdata(1,1,1,1), filename_perform(1,1,1,1)), "/", "nCell");

    %% Inputs shared by every case
    P = struct('landmask', landmask, 'freqComb', freqComb, 'nCell', double(nCell), ...
               'nWindow', nWindow, 'nCol', nCol, 'nRow', nRow, ...
               'nFreqComb', nFreqComb, 'nTxComb', nTxComb, ...
               'windowHours1T', windowHours1T, 'windowHours2T', windowHours2T);
    P.TxCombName = TxCombName;
    costFile = dir_spdata + filesep + filename_cost;
    performFile = dir_spdata + filesep + filename_perform;
    nCaseTotal = nNrx * nAlt * nInc * nNplane;

    %% Initialize MAT file if it does not exist
    if ~isfile(filepath_out)
        fprintf('Creating MAT checkpoint file: %s\n', filepath_out);

        costSats = zeros(nNrx, nAlt, nInc, nNplane);
        costDedicated = zeros(nNrx, nAlt, nInc, nNplane);
        costRideshare = zeros(nNrx, nAlt, nInc, nNplane);
        launchOption = zeros(nNrx, nAlt, nInc, nNplane);
        nLaunch = zeros(nNrx, nAlt, nInc, nNplane);

        SSC = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);

        coverage1T1st = zeros(nNrx, nAlt, nInc, nNplane, nWindow, nFreqComb);
        coverage1T2nd = zeros(nNrx, nAlt, nInc, nNplane, nWindow, nFreqComb);
        coverage2T1st = zeros(nNrx, nAlt, nInc, nNplane, nWindow/2, nFreqComb);
        coverage2T2nd = zeros(nNrx, nAlt, nInc, nNplane, nWindow/2, nFreqComb);

        coverage1Tzonal = zeros(nNrx, nAlt, nInc, nNplane, nRow, nFreqComb);
        coverage1Tglobal = zeros(nNrx, nAlt, nInc, nNplane, nFreqComb);
        coverage2Tzonal = zeros(nNrx, nAlt, nInc, nNplane, nRow, nFreqComb);
        coverage2Tglobal = zeros(nNrx, nAlt, nInc, nNplane, nFreqComb);

        SRT1T = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        SRT2T = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);

        MRT1T = zeros(nNrx, nAlt, nInc, nNplane, nFreqComb);
        MRT2T = zeros(nNrx, nAlt, nInc, nNplane, nFreqComb);
        Nmulti1T = zeros(nNrx, nAlt, nInc, nNplane, nFreqComb);
        Nmulti2T = zeros(nNrx, nAlt, nInc, nNplane, nFreqComb);

        nSpValid = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        nSpInvalid = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        nSpRatio = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);

        caseDone = false(nNrx, nAlt, nInc, nNplane);
        loopComplete = false;
        postProcessComplete = false;

        save(filepath_out, ...
            'nRx', 'alt', 'inc', 'nPlane', 'TxCombName', 'consName', ...
            'costSats', 'costDedicated', 'costRideshare', 'launchOption', 'nLaunch', ...
            'SSC', ...
            'coverage1T1st', 'coverage1T2nd', 'coverage2T1st', 'coverage2T2nd', ...
            'coverage1Tzonal', 'coverage1Tglobal', 'coverage2Tzonal', 'coverage2Tglobal', ...
            'SRT1T', 'SRT2T', ...
            'MRT1T', 'MRT2T', 'Nmulti1T', 'Nmulti2T', ...
            'nSpValid', 'nSpInvalid', 'nSpRatio', ...
            'caseDone', 'loopComplete', 'postProcessComplete', ...
            '-v7.3');
    end

    %% Open MAT file
    M = matfile(filepath_out, 'Writable', true);

    %% Progress tracker
    nCaseDone0 = nnz(M.caseDone);
    fprintf('Initial progress: %d / %d cases completed (%.2f%%)\n', ...
        nCaseDone0, nCaseTotal, 100 * nCaseDone0 / nCaseTotal);

    %% Main loop
    if M.loopComplete
        fprintf('All raw cases were already completed. Skipping raw loop.\n');
    elseif useParallel
        %% Parallel: one checkpoint file per case, merged once all exist
        caseDir = fullfile(dir_out, sprintf('FoM_%s_cases', projName));
        if ~exist(caseDir, 'dir')
            mkdir(caseDir);
        end
        caseFile = string(caseDir) + filesep + consName + ".mat";
        todo = find(~arrayfun(@isfile, caseFile));
        fprintf('Parallel run: %d of %d cases to compute; finished cases are kept in %s\n', ...
            numel(todo), nCaseTotal, caseDir);

        ok = false(size(todo));
        msg = cell(size(todo));
        caseTodo = caseFile(todo);
        costTodo = costFile(todo);
        performTodo = performFile(todo);
        parfor k = 1:numel(todo)
            try
                saveCaseFile(caseTodo(k), fomCase(costTodo(k), performTodo(k), P));
                ok(k) = true;
            catch ME
                msg{k} = ME.message;
            end
        end
        for k = reshape(find(~ok), 1, [])
            warning('Case %s failed: %s', consName(todo(k)), msg{k});
        end
        if ~all(arrayfun(@isfile, caseFile), 'all')
            fprintf('Some cases are still missing. Skipping post-processing; re-run createFoM to compute them.\n');
            return;
        end

        fprintf('All %d cases computed. Merging the case files into %s\n', nCaseTotal, filepath_out);
        R = load(caseFile(1));
        fields = fieldnames(R);
        A = struct();
        for f = 1:numel(fields)
            A.(fields{f}) = zeros(nCaseTotal, numel(R.(fields{f})));
        end
        for ic = 1:nCaseTotal
            R = load(caseFile(ic));
            for f = 1:numel(fields)
                A.(fields{f})(ic, :) = R.(fields{f})(:).';
            end
        end
        for f = 1:numel(fields)
            M.(fields{f}) = reshape(A.(fields{f}), [nNrx, nAlt, nInc, nNplane, caseSize(R.(fields{f}))]);
        end
        M.caseDone = true(nNrx, nAlt, nInc, nNplane);
        M.loopComplete = true;
        fprintf('All raw cases are complete.\n');
    else
        for iNrx = 1:nNrx
            for iAlt = 1:nAlt
                for iInc = 1:nInc
                    for iNplane = 1:nNplane
                        caseLinearIdx = sub2ind([nNrx, nAlt, nInc, nNplane], ...
                            iNrx, iAlt, iInc, iNplane);

                        nDoneNow = nnz(M.caseDone);
                        progressPct = 100 * nDoneNow / nCaseTotal;

                        currentDir = char(dir_spdata(iNrx,iAlt,iInc,iNplane));
                        currentConsName = char(consName(iNrx,iAlt,iInc,iNplane));

                        fprintf('\n============================================================\n');
                        fprintf('Case %d / %d\n', caseLinearIdx, nCaseTotal);
                        fprintf('Completed so far: %d / %d (%.2f%%)\n', ...
                            nDoneNow, nCaseTotal, progressPct);
                        fprintf('Constellation: %s\n', currentConsName);
                        fprintf('Directory: %s\n', currentDir);
                        fprintf('Indices: iNrx=%d, iAlt=%d, iInc=%d, iNplane=%d\n', ...
                            iNrx, iAlt, iInc, iNplane);
                        fprintf('============================================================\n');

                        if M.caseDone(iNrx, iAlt, iInc, iNplane)
                            fprintf('Status: already completed. Skipping this case.\n');
                            continue;
                        end

                        fprintf('Status: running this case now.\n');

                        try
                            R = fomCase(costFile(caseLinearIdx), performFile(caseLinearIdx), P);

                            fprintf('Computation finished. Writing results to MAT file...\n');

                            %% Write this case immediately to MAT file
                            M.costSats(iNrx,iAlt,iInc,iNplane) = R.costSats;
                            M.costDedicated(iNrx,iAlt,iInc,iNplane) = R.costDedicated;
                            M.costRideshare(iNrx,iAlt,iInc,iNplane) = R.costRideshare;
                            M.launchOption(iNrx,iAlt,iInc,iNplane) = R.launchOption;
                            M.nLaunch(iNrx,iAlt,iInc,iNplane) = R.nLaunch;

                            M.coverage1T1st(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(R.coverage1T1st, [1 1 1 1 nWindow nFreqComb]);
                            M.coverage1T2nd(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(R.coverage1T2nd, [1 1 1 1 nWindow nFreqComb]);

                            M.coverage2T1st(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(R.coverage2T1st, [1 1 1 1 nWindow/2 nFreqComb]);
                            M.coverage2T2nd(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(R.coverage2T2nd, [1 1 1 1 nWindow/2 nFreqComb]);

                            M.coverage1Tzonal(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(R.coverage1Tzonal, [1 1 1 1 nRow nFreqComb]);
                            M.coverage1Tglobal(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.coverage1Tglobal, [1 1 1 1 nFreqComb]);

                            M.coverage2Tzonal(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(R.coverage2Tzonal, [1 1 1 1 nRow nFreqComb]);
                            M.coverage2Tglobal(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.coverage2Tglobal, [1 1 1 1 nFreqComb]);

                            M.SSC(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.SSC, [1 1 1 1 nTxComb]);

                            M.SRT1T(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.SRT1T, [1 1 1 1 nTxComb]);
                            M.SRT2T(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.SRT2T, [1 1 1 1 nTxComb]);

                            M.nSpValid(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.nSpValid, [1 1 1 1 nTxComb]);
                            M.nSpInvalid(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.nSpInvalid, [1 1 1 1 nTxComb]);
                            M.nSpRatio(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.nSpRatio, [1 1 1 1 nTxComb]);

                            M.MRT1T(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.MRT1T, [1 1 1 1 nFreqComb]);

                            M.MRT2T(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.MRT2T, [1 1 1 1 nFreqComb]);

                            M.Nmulti1T(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.Nmulti1T, [1 1 1 1 nFreqComb]);

                            M.Nmulti2T(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(R.Nmulti2T, [1 1 1 1 nFreqComb]);

                            fprintf('MAT file write finished. Marking this case as complete...\n');

                            %% Mark this case complete only after all writes succeed
                            tmpCaseDone = M.caseDone;
                            tmpCaseDone(iNrx, iAlt, iInc, iNplane) = true;
                            M.caseDone = tmpCaseDone;

                            nDoneAfter = nnz(M.caseDone);
                            progressPctAfter = 100 * nDoneAfter / nCaseTotal;

                            fprintf('Case completed successfully.\n');
                            fprintf('Updated progress: %d / %d (%.2f%%)\n', ...
                                nDoneAfter, nCaseTotal, progressPctAfter);
                        catch ME
                            fprintf(2, 'Case failed.\n');
                            fprintf(2, 'Directory: %s\n', currentDir);
                            fprintf(2, 'Error message: %s\n', ME.message);
                            warning('Case (%d,%d,%d,%d) failed: %s', ...
                                iNrx, iAlt, iInc, iNplane, ME.message);
                        end

                    end
                end
            end
        end

        %% Final raw-loop completion check
        tmpCaseDone = M.caseDone;
        if all(tmpCaseDone(:))
            M.loopComplete = true;
            fprintf('All raw cases are complete.\n');
        else
            fprintf('Some raw cases are still incomplete. Skipping post-processing.\n');
            return;
        end
    end

    %% Post-process only after all raw cases are complete
    if ~M.loopComplete
        fprintf('Raw loop is not complete. Skipping post-processing.\n');
        return;
    end

    if M.postProcessComplete
        fprintf('Post-processing was already completed. Nothing more to do.\n');
        return;
    end

    fprintf('Starting post-processing.\n');

    S = load(filepath_out);

    consName_vec = reshape(S.consName, [], 1);
    nCase = length(consName_vec);

    costSats_vec = reshape(S.costSats, [], 1);
    costDedicated_vec = reshape(S.costDedicated, [], 1);
    costRideshare_vec = reshape(S.costRideshare, [], 1);
    launchOption_vec = reshape(S.launchOption, [], 1);
    nLaunch_vec = reshape(S.nLaunch, [], 1);

    flagDedicated = launchOption_vec == 1;
    costLaunch = zeros(length(flagDedicated), 1);
    costLaunch(flagDedicated) = costDedicated_vec(flagDedicated);
    costLaunch(~flagDedicated) = costRideshare_vec(~flagDedicated);
    costTotal = costSats_vec + costLaunch;

    SSC = reshape(S.SSC, nCase, nTxComb);

    MSC1T1st = reshape(squeeze(S.coverage1T1st(:,:,:,:,end,:)), [], nFreqComb);
    MSC1T2nd = reshape(squeeze(S.coverage1T2nd(:,:,:,:,end,:)), [], nFreqComb);
    MSC2T1st = reshape(squeeze(S.coverage2T1st(:,:,:,:,end,:)), [], nFreqComb);
    MSC2T2nd = reshape(squeeze(S.coverage2T2nd(:,:,:,:,end,:)), [], nFreqComb);

    MRT1T = reshape(S.MRT1T, [], nFreqComb);
    MRT2T = reshape(S.MRT2T, [], nFreqComb);

    SRT1T = reshape(S.SRT1T, nCase, nTxComb);
    SRT2T = reshape(S.SRT2T, nCase, nTxComb);

    nSpValid = reshape(S.nSpValid, nCase, nTxComb);
    nSpInvalid = reshape(S.nSpInvalid, nCase, nTxComb);
    nSpRatio = reshape(S.nSpRatio, nCase, nTxComb);

    consName = consName_vec;
    costSats = costSats_vec;
    costDedicated = costDedicated_vec;
    costRideshare = costRideshare_vec;
    launchOption = launchOption_vec;
    nLaunch = nLaunch_vec;

    save(filepath_out, ...
        'consName', ...
        'costTotal', 'costSats', 'costDedicated', 'costRideshare', 'launchOption', 'nLaunch', ...
        'SSC', ...
        'MSC1T1st', 'MSC1T2nd', 'MSC2T1st', 'MSC2T2nd', ...
        'MRT1T', 'MRT2T', ...
        'SRT1T', 'SRT2T', ...
        'nSpValid', 'nSpInvalid', 'nSpRatio', ...
        '-append');

    M = matfile(filepath_out, 'Writable', true);
    M.postProcessComplete = true;

    fprintf('Post-processing completed successfully.\n');
end

function tag = readRunTag(projName)
% READRUNTAG  Run tag of the SAI Observation Model section of
%   sim/<projName>/Inp_Design.txt, which names the performance files.
    file = fullfile('sim', projName, 'Inp_Design.txt');
    lines = readlines(file);
    iTag = find(contains(lines, "! Run tag"), 1);
    if isempty(iTag)
        error('createFoM:noRunTag', 'No run tag in %s; pass performTag.', file);
    end
    tag = strtrim(extractBefore(lines(iTag), "!"));
end

function [rtDays, nMulti] = revisitTimeWindows(obsMask, windowHours, landmaskVec)
% REVISITTIMEWINDOWS  Revisit time from window observations, in days.
%
%   obsMask [nCol x nRow x nWindow] is true where a cell was observed in a
%   window. Per land cell, N_i = number of windows with an observation and
%   m_i = T_sim / max(N_i, 1), so a cell never observed counts as T_sim; the
%   result is the mean of m_i over the land cells (per-cell Poisson rate,
%   censored at T_sim). nMulti = land cells observed in at least two windows.
    nWindow = size(obsMask, 3);
    Tsim = nWindow * windowHours;                      % hours
    N = sum(reshape(obsMask, [], nWindow), 2);
    N = N(landmaskVec);
    nMulti = nnz(N >= 2);
    if isempty(N)
        rtDays = NaN;
    else
        rtDays = mean(Tsim ./ max(double(N), 1)) / 24;
    end
end

function R = fomCase(costFile, performFile, P)
% FOMCASE  FoMs of one constellation from its cost and performance files.
%
%   Every field of R is the per-case value of the FoM variable of the same
%   name in FoM_<projName>.mat; P holds the inputs shared by all cases.
    nWindow = P.nWindow;  nCol = P.nCol;  nRow = P.nRow;
    nFreqComb = P.nFreqComb;  nTxComb = P.nTxComb;  freqComb = P.freqComb;
    landmask = P.landmask;  nCell = P.nCell;

    %% Cost
    cost = ncread(costFile, "cost");
    R.costSats = cost(1) / 1e6;
    R.costDedicated = cost(2) / 1e6;
    R.costRideshare = cost(3) / 1e6;
    R.launchOption = cost(4);
    R.nLaunch = cost(5);

    %% Observable frequencies
    obsFreq1T = ncread(performFile, "obsFreq");

    obsFreq2T = zeros(nCol, nRow, nWindow/2, 'like', obsFreq1T);
    for iWindow = 1:nWindow/2
        obsFreq2T(:,:,iWindow) = bitor(obsFreq1T(:,:,2*iWindow-1), ...
                                       obsFreq1T(:,:,2*iWindow));
    end

    obsFreqCumul = zeros(nCol, nRow, 'like', obsFreq1T);
    for iWindow = 1:nWindow
        obsFreqCumul = bitor(obsFreq1T(:,:,iWindow), obsFreqCumul);
    end

    %% MRT, Nmulti and coverage, per frequency combination
    %   MRT [days]: per land cell m_i = T_sim / max(N_i, 1), N_i the number
    %   of windows in which the cell saw every frequency of the combination
    %   (a cell never observed counts as T_sim); MRT is the mean of m_i over
    %   land cells. Nmulti: land cells observed in at least two windows.
    %   Coverage: share of grid cells observed at least once (1st) and at
    %   least twice (2nd) by the end of each window, from the cumulative
    %   observation count; zonal and global: mean count over land cells.
    R.MRT1T = nan(1, nFreqComb);
    R.MRT2T = nan(1, nFreqComb);
    R.Nmulti1T = zeros(1, nFreqComb);
    R.Nmulti2T = zeros(1, nFreqComb);
    R.coverage1T1st = zeros(nWindow, nFreqComb);
    R.coverage1T2nd = zeros(nWindow, nFreqComb);
    R.coverage2T1st = zeros(nWindow/2, nFreqComb);
    R.coverage2T2nd = zeros(nWindow/2, nFreqComb);
    obs1Tcumul = zeros(nCol, nRow, nFreqComb);
    obs2Tcumul = zeros(nCol, nRow, nFreqComb);
    for iFreq = 1:nFreqComb
        landmaskVec = reshape(landmask(:,:,iFreq), [], 1);
        obsMask1T = bitand(obsFreq1T, freqComb(iFreq)) == freqComb(iFreq);
        obsMask2T = bitand(obsFreq2T, freqComb(iFreq)) == freqComb(iFreq);
        [R.MRT1T(iFreq), R.Nmulti1T(iFreq)] = revisitTimeWindows(obsMask1T, P.windowHours1T, landmaskVec);
        [R.MRT2T(iFreq), R.Nmulti2T(iFreq)] = revisitTimeWindows(obsMask2T, P.windowHours2T, landmaskVec);

        nObs1T = cumsum(uint8(obsMask1T), 3);       % observations up to each window
        nObs2T = cumsum(uint8(obsMask2T), 3);
        R.coverage1T1st(:, iFreq) = reshape(sum(nObs1T > 0, [1 2]), [], 1);
        R.coverage1T2nd(:, iFreq) = reshape(sum(nObs1T > 1, [1 2]), [], 1);
        R.coverage2T1st(:, iFreq) = reshape(sum(nObs2T > 0, [1 2]), [], 1);
        R.coverage2T2nd(:, iFreq) = reshape(sum(nObs2T > 1, [1 2]), [], 1);
        obs1Tcumul(:,:,iFreq) = double(nObs1T(:,:,end));
        obs2Tcumul(:,:,iFreq) = double(nObs2T(:,:,end));
    end

    R.coverage1T1st = R.coverage1T1st / double(nCell) * 100;
    R.coverage1T2nd = R.coverage1T2nd / double(nCell) * 100;
    R.coverage2T1st = R.coverage2T1st / double(nCell) * 100;
    R.coverage2T2nd = R.coverage2T2nd / double(nCell) * 100;

    nLandRow = reshape(sum(landmask, 1), nRow, nFreqComb);
    R.coverage1Tzonal = reshape(sum(obs1Tcumul .* landmask, 1), nRow, nFreqComb) ./ nLandRow;
    R.coverage2Tzonal = reshape(sum(obs2Tcumul .* landmask, 1), nRow, nFreqComb) ./ nLandRow;
    R.coverage1Tzonal(nLandRow == 0) = NaN;
    R.coverage2Tzonal(nLandRow == 0) = NaN;
    R.coverage1Tglobal = mean(reshape(obs1Tcumul(landmask), [], nFreqComb), 1);
    R.coverage2Tglobal = mean(reshape(obs2Tcumul(landmask), [], nFreqComb), 1);

    %% Tx-combination metrics
    R.SSC = zeros(1, nTxComb);
    R.SRT1T = nan(1, nTxComb);
    R.SRT2T = nan(1, nTxComb);
    R.nSpValid = zeros(1, nTxComb);
    R.nSpInvalid = zeros(1, nTxComb);
    R.nSpRatio = zeros(1, nTxComb);

    landmaskVec = reshape(landmask(:,:,1), [], 1);
    for iTxComb = 1:nTxComb
        txBit = 2^(iTxComb-1);      % obsFreq bit of this TxComb

        % Coverage: grid cells observed in any window
        R.SSC(iTxComb) = nnz(bitand(obsFreqCumul, txBit)) / double(nCell) * 100;

        % SRT [days]: the MRT definition applied to this TxComb alone
        R.SRT1T(iTxComb) = revisitTimeWindows(bitand(obsFreq1T, txBit) ~= 0, P.windowHours1T, landmaskVec);
        R.SRT2T(iTxComb) = revisitTimeWindows(bitand(obsFreq2T, txBit) ~= 0, P.windowHours2T, landmaskVec);

        nValid = ncread(performFile, strcat("nSpValid_", P.TxCombName{iTxComb}));
        nInvalid = ncread(performFile, strcat("nSpInvalid_", P.TxCombName{iTxComb}));

        R.nSpValid(iTxComb) = nValid;
        R.nSpInvalid(iTxComb) = nInvalid;

        if (nValid + nInvalid) == 0
            R.nSpRatio(iTxComb) = NaN;
        else
            R.nSpRatio(iTxComb) = nValid / (nValid + nInvalid);
        end
    end
end

function saveCaseFile(file, R)
% SAVECASEFILE  Write one case's FoMs; the rename makes an interrupted write
%   leave no file behind, so a re-run computes that case again.
    tmp = file + ".tmp.mat";
    save(tmp, '-struct', 'R');
    movefile(tmp, file);
end

function sz = caseSize(v)
% CASESIZE  Trailing dimensions of a per-case value in the FoM arrays: a row
%   vector [1 n] is stored as n, a scalar adds no dimension.
    sz = size(v);
    if sz(1) == 1
        sz(1) = [];
    end
end
