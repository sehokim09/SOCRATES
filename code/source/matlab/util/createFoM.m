function createFoM(projName, DesignParamsObj)
% function createFoM
%
%   Read performance and cost NetCDF files and save FoM_[projName].mat
%   with restart/checkpoint support for long runs.
%
%   Raw case-by-case results are written directly to the MAT file.
%   Post-processing is performed only after all cases are completed.
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
                                strcat(consName(iNrx, iAlt, iInc, iNplane), "_perform_SAI.nc");
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
            filename_perform(1,1,1,1) = strcat(consName(1,1,1,1), "_perform.nc");
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

    %% Initialize MAT file if it does not exist
    if ~isfile(filepath_out)
        fprintf('Creating MAT checkpoint file: %s\n', filepath_out);

        costSats = zeros(nNrx, nAlt, nInc, nNplane);
        costDedicated = zeros(nNrx, nAlt, nInc, nNplane);
        costRideshare = zeros(nNrx, nAlt, nInc, nNplane);
        launchOption = zeros(nNrx, nAlt, nInc, nNplane);
        nLaunch = zeros(nNrx, nAlt, nInc, nNplane);

        coverageMax = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        coverageMap = zeros(nNrx, nAlt, nInc, nNplane, nCol, nRow);

        coverage1T1st = zeros(nNrx, nAlt, nInc, nNplane, nWindow, nFreqComb);
        coverage1T2nd = zeros(nNrx, nAlt, nInc, nNplane, nWindow, nFreqComb);
        coverage2T1st = zeros(nNrx, nAlt, nInc, nNplane, nWindow/2, nFreqComb);
        coverage2T2nd = zeros(nNrx, nAlt, nInc, nNplane, nWindow/2, nFreqComb);

        coverageMap1T = zeros(nNrx, nAlt, nInc, nNplane, nCol, nRow, nFreqComb);
        coverageMap2T = zeros(nNrx, nAlt, nInc, nNplane, nCol, nRow, nFreqComb);

        coverage1Tzonal = zeros(nNrx, nAlt, nInc, nNplane, nRow, nFreqComb);
        coverage1Tglobal = zeros(nNrx, nAlt, nInc, nNplane, nFreqComb);
        coverage2Tzonal = zeros(nNrx, nAlt, nInc, nNplane, nRow, nFreqComb);
        coverage2Tglobal = zeros(nNrx, nAlt, nInc, nNplane, nFreqComb);

        revisitTime50th = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        revisitTime90th = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        revisitTime95th = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        revisitTime98th = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        revisitTimeMean = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);

        nSpValid = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        nSpInvalid = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);
        nSpRatio = zeros(nNrx, nAlt, nInc, nNplane, nTxComb);

        caseDone = false(nNrx, nAlt, nInc, nNplane);
        loopComplete = false;
        postProcessComplete = false;

        save(filepath_out, ...
            'nRx', 'alt', 'inc', 'nPlane', 'TxCombName', 'consName', ...
            'costSats', 'costDedicated', 'costRideshare', 'launchOption', 'nLaunch', ...
            'coverageMax', 'coverageMap', ...
            'coverage1T1st', 'coverage1T2nd', 'coverage2T1st', 'coverage2T2nd', ...
            'coverageMap1T', 'coverageMap2T', ...
            'coverage1Tzonal', 'coverage1Tglobal', 'coverage2Tzonal', 'coverage2Tglobal', ...
            'revisitTime50th', 'revisitTime90th', 'revisitTime95th', 'revisitTime98th', 'revisitTimeMean', ...
            'nSpValid', 'nSpInvalid', 'nSpRatio', ...
            'caseDone', 'loopComplete', 'postProcessComplete', ...
            '-v7.3');
    end

    %% Open MAT file
    M = matfile(filepath_out, 'Writable', true);

    %% Progress tracker
    nCaseTotal = nNrx * nAlt * nInc * nNplane;
    nCaseDone0 = nnz(M.caseDone);
    fprintf('Initial progress: %d / %d cases completed (%.2f%%)\n', ...
        nCaseDone0, nCaseTotal, 100 * nCaseDone0 / nCaseTotal);
    
    %% Main loop
    if M.loopComplete
        fprintf('All raw cases were already completed. Skipping raw loop.\n');
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
                            %% Cost
                            cost = ncread( ...
                                fullfile(dir_spdata(iNrx,iAlt,iInc,iNplane), ...
                                filename_cost(iNrx,iAlt,iInc,iNplane)), "cost");

                            tmpCostSats = cost(1) / 1e6;
                            tmpCostDedicated = cost(2) / 1e6;
                            tmpCostRideshare = cost(3) / 1e6;
                            tmpLaunchOption = cost(4);
                            tmpNLaunch = cost(5);

                            %% Observable frequencies
                            obsFreq1T = ncread( ...
                                fullfile(dir_spdata(iNrx,iAlt,iInc,iNplane), ...
                                filename_perform(iNrx,iAlt,iInc,iNplane)), "obsFreq");

                            obsFreq2T = zeros(nCol, nRow, nWindow/2, 'like', obsFreq1T);
                            for iWindow = 1:nWindow/2
                                obsFreq2T(:,:,iWindow) = bitor(obsFreq1T(:,:,2*iWindow-1), ...
                                                               obsFreq1T(:,:,2*iWindow));
                            end

                            obsFreqCumul = zeros(nCol, nRow, 'like', obsFreq1T);
                            for iWindow = 1:nWindow
                                obsFreqCumul = bitor(obsFreq1T(:,:,iWindow), obsFreqCumul);
                            end
                            tmpCoverageMap = obsFreqCumul;

                            %% MRT for 1T (12-hour windows)
                            nCellGrid = nCol * nRow;
                            winIdx = 1:nWindow;
                            
                            tmpMRT1Tglobal = nan(1, nFreqComb);
                            tmpNmulti1T = zeros(1, nFreqComb);
                            
                            for iFreq = 1:nFreqComb
                            
                                % Multi-frequency observation mask
                                obsMask3D = (bitand(obsFreq1T, freqComb(iFreq)) == freqComb(iFreq));
                            
                                % [nCell x nWindow]
                                obsMask2D = reshape(obsMask3D, nCellGrid, nWindow);
                            
                                % Window indices (NaN where no observation)
                                obsWin = repmat(winIdx, nCellGrid, 1);
                                obsWin(~obsMask2D) = NaN;
                            
                                % Previous observation index
                                prevObsWin = [nan(nCellGrid,1), fillmissing(obsWin(:,1:end-1), 'previous', 2)];
                            
                                % Revisit intervals (hours)
                                revisitIntervals = (obsWin - prevObsWin) * windowHours1T;
                            
                                % Keep only valid successive observations
                                validIntervals = obsMask2D & isfinite(prevObsWin);
                                revisitIntervals(~validIntervals) = NaN;
                            
                                % Cell-wise MRT
                                cellMean = mean(revisitIntervals, 2, 'omitnan');
                            
                                % Apply land mask
                                landmaskVec = reshape(landmask(:,:,iFreq), nCellGrid, 1);
                            
                                validCells = any(validIntervals, 2) & landmaskVec;
                                tmpNmulti1T(iFreq) = nnz(validCells);
                            
                                if tmpNmulti1T(iFreq) > 0
                                    tmpMRT1Tglobal(iFreq) = mean(cellMean(validCells), 'omitnan');
                                end
                            
                            end
                            %% MRT for 2T (24-hour windows)
                            nWindow2T = nWindow / 2;
                            winIdx2 = 1:nWindow2T;
                            
                            tmpMRT2Tglobal = nan(1, nFreqComb);
                            tmpNmulti2T = zeros(1, nFreqComb);
                            
                            for iFreq = 1:nFreqComb
                            
                                obsMask3D = (bitand(obsFreq2T, freqComb(iFreq)) == freqComb(iFreq));
                                obsMask2D = reshape(obsMask3D, nCellGrid, nWindow2T);
                            
                                obsWin = repmat(winIdx2, nCellGrid, 1);
                                obsWin(~obsMask2D) = NaN;
                            
                                prevObsWin = [nan(nCellGrid,1), fillmissing(obsWin(:,1:end-1), 'previous', 2)];
                            
                                revisitIntervals = (obsWin - prevObsWin) * windowHours2T;
                            
                                validIntervals = obsMask2D & isfinite(prevObsWin);
                                revisitIntervals(~validIntervals) = NaN;
                            
                                cellMean = mean(revisitIntervals, 2, 'omitnan');
                            
                                landmaskVec = reshape(landmask(:,:,iFreq), nCellGrid, 1);
                            
                                validCells = any(validIntervals, 2) & landmaskVec;
                                tmpNmulti2T(iFreq) = nnz(validCells);
                            
                                if tmpNmulti2T(iFreq) > 0
                                    tmpMRT2Tglobal(iFreq) = mean(cellMean(validCells), 'omitnan');
                                end
                            
                            end
                            % tmpMRT1Tglobal = tmpMRT1Tglobal / 24;
                            % tmpMRT2Tglobal = tmpMRT2Tglobal / 24;

                            %% 1T coverage
                            obs1Tcumul = zeros(nCol, nRow, nFreqComb);
                            tmpCoverage1T1st = zeros(nWindow, nFreqComb);
                            tmpCoverage1T2nd = zeros(nWindow, nFreqComb);

                            for iWindow = 1:nWindow
                                for iFreq = 1:nFreqComb
                                    flagObs = bitand(obsFreq1T(:,:,iWindow), freqComb(iFreq)) == freqComb(iFreq);
                                    obs1Tcumul(:,:,iFreq) = obs1Tcumul(:,:,iFreq) + flagObs;
                                    tmpCoverage1T1st(iWindow, iFreq) = sum(obs1Tcumul(:,:,iFreq) > 0, 'all');
                                    tmpCoverage1T2nd(iWindow, iFreq) = sum(obs1Tcumul(:,:,iFreq) > 1, 'all');
                                end
                            end

                            tmpCoverage1T1st = tmpCoverage1T1st / double(nCell) * 100;
                            tmpCoverage1T2nd = tmpCoverage1T2nd / double(nCell) * 100;
                            tmpCoverageMap1T = obs1Tcumul;

                            tmpCoverage1Tzonal = zeros(nRow, nFreqComb);
                            for iRow = 1:nRow
                                for iFreq = 1:nFreqComb
                                    idx = landmask(:, iRow, iFreq);
                                    if any(idx)
                                        tmpCoverage1Tzonal(iRow, iFreq) = mean(obs1Tcumul(idx, iRow, iFreq));
                                    else
                                        tmpCoverage1Tzonal(iRow, iFreq) = NaN;
                                    end
                                end
                            end
                            tmpCoverage1Tglobal = mean(reshape(obs1Tcumul(landmask), [], nFreqComb), 1);

                            %% 2T coverage
                            obs2Tcumul = zeros(nCol, nRow, nFreqComb);
                            tmpCoverage2T1st = zeros(nWindow/2, nFreqComb);
                            tmpCoverage2T2nd = zeros(nWindow/2, nFreqComb);

                            for iWindow = 1:nWindow/2
                                for iFreq = 1:nFreqComb
                                    flagObs = bitand(obsFreq2T(:,:,iWindow), freqComb(iFreq)) == freqComb(iFreq);
                                    obs2Tcumul(:,:,iFreq) = obs2Tcumul(:,:,iFreq) + flagObs;
                                    tmpCoverage2T1st(iWindow, iFreq) = sum(obs2Tcumul(:,:,iFreq) > 0, 'all');
                                    tmpCoverage2T2nd(iWindow, iFreq) = sum(obs2Tcumul(:,:,iFreq) > 1, 'all');
                                end
                            end

                            tmpCoverage2T1st = tmpCoverage2T1st / double(nCell) * 100;
                            tmpCoverage2T2nd = tmpCoverage2T2nd / double(nCell) * 100;
                            tmpCoverageMap2T = obs2Tcumul;

                            tmpCoverage2Tzonal = zeros(nRow, nFreqComb);
                            for iRow = 1:nRow
                                for iFreq = 1:nFreqComb
                                    idx = landmask(:, iRow, iFreq);
                                    if any(idx)
                                        tmpCoverage2Tzonal(iRow, iFreq) = mean(obs2Tcumul(idx, iRow, iFreq));
                                    else
                                        tmpCoverage2Tzonal(iRow, iFreq) = NaN;
                                    end
                                end
                            end
                            tmpCoverage2Tglobal = mean(reshape(obs2Tcumul(landmask), [], nFreqComb), 1);

                            %% Tx-combination metrics
                            tmpCoverageMax = zeros(1, nTxComb);
                            tmpRevisitTime50th = zeros(1, nTxComb);
                            tmpRevisitTime90th = zeros(1, nTxComb);
                            tmpRevisitTime95th = zeros(1, nTxComb);
                            tmpRevisitTime98th = zeros(1, nTxComb);
                            tmpRevisitTimeMean = zeros(1, nTxComb);
                            tmpNSpValid = zeros(1, nTxComb);
                            tmpNSpInvalid = zeros(1, nTxComb);
                            tmpNSpRatio = zeros(1, nTxComb);

                            for iTxComb = 1:nTxComb
                                accessTime1st = ncread( ...
                                    fullfile(dir_spdata(iNrx,iAlt,iInc,iNplane), ...
                                    filename_perform(iNrx,iAlt,iInc,iNplane)), ...
                                    strcat("accessTime1st_", TxCombName{iTxComb}));
                                accessTime1st = sort(accessTime1st);
                                coverage = (1:length(accessTime1st)) / double(nCell) * 100;
                                if isempty(coverage)
                                    tmpCoverageMax(iTxComb) = 0;
                                else
                                    tmpCoverageMax(iTxComb) = max(coverage);
                                end

                                revisitTime = ncread( ...
                                    fullfile(dir_spdata(iNrx,iAlt,iInc,iNplane), ...
                                    filename_perform(iNrx,iAlt,iInc,iNplane)), ...
                                    strcat("revisitTime_", TxCombName{iTxComb}));

                                if isempty(revisitTime)
                                    tmpRevisitTime50th(iTxComb) = NaN;
                                    tmpRevisitTime90th(iTxComb) = NaN;
                                    tmpRevisitTime95th(iTxComb) = NaN;
                                    tmpRevisitTime98th(iTxComb) = NaN;
                                    tmpRevisitTimeMean(iTxComb) = NaN;
                                else
                                    percentile = prctile(revisitTime, [50, 90, 95, 98]) / 60 / 60 / 24;
                                    tmpRevisitTime50th(iTxComb) = percentile(1);
                                    tmpRevisitTime90th(iTxComb) = percentile(2);
                                    tmpRevisitTime95th(iTxComb) = percentile(3);
                                    tmpRevisitTime98th(iTxComb) = percentile(4);
                                    tmpRevisitTimeMean(iTxComb) = mean(revisitTime) / 60 / 60 / 24;
                                end

                                nValid = ncread( ...
                                    fullfile(dir_spdata(iNrx,iAlt,iInc,iNplane), ...
                                    filename_perform(iNrx,iAlt,iInc,iNplane)), ...
                                    strcat("nSpValid_", TxCombName{iTxComb}));

                                nInvalid = ncread( ...
                                    fullfile(dir_spdata(iNrx,iAlt,iInc,iNplane), ...
                                    filename_perform(iNrx,iAlt,iInc,iNplane)), ...
                                    strcat("nSpInvalid_", TxCombName{iTxComb}));

                                tmpNSpValid(iTxComb) = nValid;
                                tmpNSpInvalid(iTxComb) = nInvalid;

                                if (nValid + nInvalid) == 0
                                    tmpNSpRatio(iTxComb) = NaN;
                                else
                                    tmpNSpRatio(iTxComb) = nValid / (nValid + nInvalid);
                                end
                            end

                            fprintf('Computation finished. Writing results to MAT file...\n');

                            %% Write this case immediately to MAT file
                            M.costSats(iNrx,iAlt,iInc,iNplane) = tmpCostSats;
                            M.costDedicated(iNrx,iAlt,iInc,iNplane) = tmpCostDedicated;
                            M.costRideshare(iNrx,iAlt,iInc,iNplane) = tmpCostRideshare;
                            M.launchOption(iNrx,iAlt,iInc,iNplane) = tmpLaunchOption;
                            M.nLaunch(iNrx,iAlt,iInc,iNplane) = tmpNLaunch;
                            
                            M.coverageMap(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(tmpCoverageMap, [1 1 1 1 nCol nRow]);
                            
                            M.coverage1T1st(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(tmpCoverage1T1st, [1 1 1 1 nWindow nFreqComb]);
                            M.coverage1T2nd(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(tmpCoverage1T2nd, [1 1 1 1 nWindow nFreqComb]);
                            
                            M.coverage2T1st(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(tmpCoverage2T1st, [1 1 1 1 nWindow/2 nFreqComb]);
                            M.coverage2T2nd(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(tmpCoverage2T2nd, [1 1 1 1 nWindow/2 nFreqComb]);
                            
                            M.coverageMap1T(iNrx,iAlt,iInc,iNplane,:,:,:) = ...
                                reshape(tmpCoverageMap1T, [1 1 1 1 nCol nRow nFreqComb]);
                            M.coverageMap2T(iNrx,iAlt,iInc,iNplane,:,:,:) = ...
                                reshape(tmpCoverageMap2T, [1 1 1 1 nCol nRow nFreqComb]);
                            
                            M.coverage1Tzonal(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(tmpCoverage1Tzonal, [1 1 1 1 nRow nFreqComb]);
                            M.coverage1Tglobal(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpCoverage1Tglobal, [1 1 1 1 nFreqComb]);
                            
                            M.coverage2Tzonal(iNrx,iAlt,iInc,iNplane,:,:) = ...
                                reshape(tmpCoverage2Tzonal, [1 1 1 1 nRow nFreqComb]);
                            M.coverage2Tglobal(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpCoverage2Tglobal, [1 1 1 1 nFreqComb]);
                            
                            M.coverageMax(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpCoverageMax, [1 1 1 1 nTxComb]);
                            
                            M.revisitTime50th(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpRevisitTime50th, [1 1 1 1 nTxComb]);
                            M.revisitTime90th(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpRevisitTime90th, [1 1 1 1 nTxComb]);
                            M.revisitTime95th(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpRevisitTime95th, [1 1 1 1 nTxComb]);
                            M.revisitTime98th(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpRevisitTime98th, [1 1 1 1 nTxComb]);
                            M.revisitTimeMean(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpRevisitTimeMean, [1 1 1 1 nTxComb]);
                            
                            M.nSpValid(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpNSpValid, [1 1 1 1 nTxComb]);
                            M.nSpInvalid(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpNSpInvalid, [1 1 1 1 nTxComb]);
                            M.nSpRatio(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpNSpRatio, [1 1 1 1 nTxComb]);

                            M.MRT1Tglobal(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpMRT1Tglobal, [1 1 1 1 nFreqComb]);
                            
                            M.MRT2Tglobal(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpMRT2Tglobal, [1 1 1 1 nFreqComb]);
                            
                            M.Nmulti1T(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpNmulti1T, [1 1 1 1 nFreqComb]);
                            
                            M.Nmulti2T(iNrx,iAlt,iInc,iNplane,:) = ...
                                reshape(tmpNmulti2T, [1 1 1 1 nFreqComb]);

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

    coverageMax = reshape(S.coverageMax, nCase, nTxComb);

    MSC1T1st = reshape(squeeze(S.coverage1T1st(:,:,:,:,end,:)), [], nFreqComb);
    MSC1T2nd = reshape(squeeze(S.coverage1T2nd(:,:,:,:,end,:)), [], nFreqComb);
    MSC2T1st = reshape(squeeze(S.coverage2T1st(:,:,:,:,end,:)), [], nFreqComb);
    MSC2T2nd = reshape(squeeze(S.coverage2T2nd(:,:,:,:,end,:)), [], nFreqComb);

    MTC1Tglobal = reshape(S.coverage1Tglobal, [], nFreqComb);
    MTC2Tglobal = reshape(S.coverage2Tglobal, [], nFreqComb);

    revisitTime50th = reshape(S.revisitTime50th, nCase, nTxComb);
    revisitTime90th = reshape(S.revisitTime90th, nCase, nTxComb);
    revisitTime95th = reshape(S.revisitTime95th, nCase, nTxComb);
    revisitTime98th = reshape(S.revisitTime98th, nCase, nTxComb);
    revisitTimeMean = reshape(S.revisitTimeMean, nCase, nTxComb);

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
        'coverageMax', ...
        'MSC1T1st', 'MSC1T2nd', 'MSC2T1st', 'MSC2T2nd', ...
        'MTC1Tglobal', 'MTC2Tglobal', ...
        'revisitTime50th', 'revisitTime90th', 'revisitTime95th', 'revisitTime98th', 'revisitTimeMean', ...
        'nSpValid', 'nSpInvalid', 'nSpRatio', ...
        '-append');

    M = matfile(filepath_out, 'Writable', true);
    M.postProcessComplete = true;

    fprintf('Post-processing completed successfully.\n');
end