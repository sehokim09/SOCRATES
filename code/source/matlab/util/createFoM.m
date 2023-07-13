function createFoM(projName, DesignParamsObj)
% function createFoM
%
%   Read performance and cost NetCDf files and save FoM_[projName].mat
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

    % Directories
    dir_out = fullfile('sim', projName, 'out');
    dir_largefiles = DesignParamsObj.largeFilePath;
    
    % Design variables
    TxCombName = {DesignParamsObj.TxComb.name};
    freqComb = double([DesignParamsObj.freqComb.ID]);

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
            for iNrx = 1 : nNrx
                for iAlt = 1 : nAlt
                    for iInc = 1 : nInc
                        for iNplane = 1 : nNplane
                            consName(iNrx, iAlt, iInc, iNplane) = sprintf("%d_%d_%s_%d", nRx(iNrx), alt(iAlt), inc(iInc), nPlane(iNplane));
                            dir_spdata(iNrx, iAlt, iInc, iNplane) = fullfile(dir_largefiles, consName(iNrx, iAlt, iInc, iNplane));
                            filename_perform(iNrx, iAlt, iInc, iNplane) = strcat(consName(iNrx, iAlt, iInc, iNplane), "_perform_newRCG.nc");
                            filename_cost(iNrx, iAlt, iInc, iNplane) = strcat(consName(iNrx, iAlt, iInc, iNplane), "_cost.nc");
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
            consName(1, 1, 1, 1) = string(DesignParamsObj.hybrid.name);
            dir_spdata(1, 1, 1, 1) = fullfile(dir_largefiles, consName(1, 1, 1, 1));
            filename_perform(1, 1, 1, 1) = strcat(consName(1, 1, 1, 1), "_perform_newRCG.nc");
            filename_cost(1, 1, 1, 1) = strcat(consName(1, 1, 1, 1), "_cost.nc");
    end

    % Load grid landmask
    opts = delimitedTextImportOptions('Delimiter', ' ', 'VariableTypes', 'double');
    landmask = readmatrix(fullfile('sim', projName, 'out', 'Grid1_landmask.out'), opts);
    landmask = repmat(logical(landmask(:,1:end-1)'),1,1,nFreqComb);

    % Dimension of grid
    info = ncinfo(fullfile(dir_spdata(1, 1, 1, 1), filename_perform(1, 1, 1, 1)), 'obsFreq');
    nCol = info.Size(1);
    nRow = info.Size(2);
    nWindow = info.Size(3);

    %% COMPUTE FIGURES OF MERIT 
    % Initialization
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
    
    % Global attributes
    nCell = ncreadatt(fullfile(dir_spdata(1, 1, 1, 1), filename_perform(1, 1, 1, 1)), "/", "nCell");
    
    % Figures of Merit
    for iNrx = 1 : nNrx
        for iAlt = 1 : nAlt
            for iInc = 1 : nInc
                for iNplane = 1 : nNplane
                    % Cost
                    cost = ncread(fullfile(dir_spdata(iNrx, iAlt, iInc, iNplane), filename_cost(iNrx, iAlt, iInc, iNplane)), "cost");
                    costSats(iNrx, iAlt, iInc, iNplane) = cost(1)/1e6;
                    costDedicated(iNrx, iAlt, iInc, iNplane) = cost(2)/1e6;
                    costRideshare(iNrx, iAlt, iInc, iNplane) = cost(3)/1e6;
                    launchOption(iNrx, iAlt, iInc, iNplane) = cost(4);
                    nLaunch(iNrx, iAlt, iInc, iNplane) = cost(5);
                    % Observable Frequencies
                    obsFreq1T = ncread(fullfile(dir_spdata(iNrx, iAlt, iInc, iNplane), filename_perform(iNrx, iAlt, iInc, iNplane)), "obsFreq");
                    obsFreq2T = zeros(nCol, nRow, nWindow/2);
                    for iWindow = 1 : nWindow/2
                        obsFreq2T(:,:,iWindow) = bitor(obsFreq1T(:,:,2*iWindow-1), obsFreq1T(:,:,2*iWindow));
                    end
                    obsFreqCumul = zeros(nCol, nRow);
                    for iWindow = 1 : nWindow
                        obsFreqCumul = bitor(obsFreq1T(:,:,iWindow), obsFreqCumul);
                    end
                    coverageMap(iNrx, iAlt, iInc, iNplane, :, :) = obsFreqCumul;
                    obs1Tcumul = zeros(nCol, nRow, nFreqComb);
                    for iWindow = 1 : nWindow
                        for iFreq = 1 : nFreqComb
                            flagObs = bitand(obsFreq1T(:,:,iWindow),freqComb(iFreq)) == freqComb(iFreq);
                            obs1Tcumul(:,:,iFreq) = obs1Tcumul(:,:,iFreq) + flagObs;
                            coverage1T1st(iNrx, iAlt, iInc, iNplane, iWindow, iFreq) = squeeze(sum(obs1Tcumul(:,:,iFreq)>0, [1 2]));
                            coverage1T2nd(iNrx, iAlt, iInc, iNplane, iWindow, iFreq) = squeeze(sum(obs1Tcumul(:,:,iFreq)>1, [1 2]));
                        end
                    end
                    coverage1T1st(iNrx, iAlt, iInc, iNplane, :, :) = coverage1T1st(iNrx, iAlt, iInc, iNplane, :, :) / double(nCell) * 100;
                    coverage1T2nd(iNrx, iAlt, iInc, iNplane, :, :) = coverage1T2nd(iNrx, iAlt, iInc, iNplane, :, :) / double(nCell) * 100;
                    for iRow = 1 : nRow
                        for iFreqComb = 1 : nFreqComb
                            coverage1Tzonal(iNrx, iAlt, iInc, iNplane, iRow, iFreqComb) = mean(obs1Tcumul(landmask(:,iRow,iFreqComb),iRow,iFreqComb));
                        end
                    end
                    coverage1Tglobal(iNrx, iAlt, iInc, iNplane, :) =  mean(reshape(obs1Tcumul(landmask),[],nFreqComb),1);
                    coverageMap1T(iNrx, iAlt, iInc, iNplane, :, :, :) = obs1Tcumul;

                    obs2Tcumul = zeros(nCol, nRow, nFreqComb);
                    for iWindow = 1 : nWindow/2
                        for iFreq = 1 : nFreqComb
                            flagObs = bitand(obsFreq2T(:,:,iWindow),freqComb(iFreq)) == freqComb(iFreq);
                            obs2Tcumul(:,:,iFreq) = obs2Tcumul(:,:,iFreq) + flagObs;
                            coverage2T1st(iNrx, iAlt, iInc, iNplane, iWindow, iFreq) = squeeze(sum(obs2Tcumul(:,:,iFreq)>0, [1 2]));
                            coverage2T2nd(iNrx, iAlt, iInc, iNplane, iWindow, iFreq) = squeeze(sum(obs2Tcumul(:,:,iFreq)>1, [1 2]));
                        end
                    end
                    coverage2T1st(iNrx, iAlt, iInc, iNplane, :, :) = coverage2T1st(iNrx, iAlt, iInc, iNplane, :, :) / double(nCell) * 100;
                    coverage2T2nd(iNrx, iAlt, iInc, iNplane, :, :) = coverage2T2nd(iNrx, iAlt, iInc, iNplane, :, :) / double(nCell) * 100;
                    for iRow = 1 : nRow
                        for iFreqComb = 1 : nFreqComb
                            coverage2Tzonal(iNrx, iAlt, iInc, iNplane, iRow, iFreqComb) = mean(obs2Tcumul(landmask(:,iRow,iFreqComb),iRow,iFreqComb));
                        end
                    end
                    coverage2Tglobal(iNrx, iAlt, iInc, iNplane, :) =  mean(reshape(obs2Tcumul(landmask),[],nFreqComb),1);
                    coverageMap2T(iNrx, iAlt, iInc, iNplane, :, :, :) = obs2Tcumul;
                    
                    for iTxComb = 1 : nTxComb
                        % Coverage
                        accessTime1st = ncread(fullfile(dir_spdata(iNrx, iAlt, iInc, iNplane),...
                            filename_perform(iNrx, iAlt, iInc, iNplane)), strcat("accessTime1st_", TxCombName{iTxComb}));
                        accessTime1st = sort(accessTime1st);
                        coverage = (1:length(accessTime1st))/double(nCell)*100;
                        coverageMax(iNrx, iAlt, iInc, iNplane, iTxComb) = max(coverage);
                        % Revisit time
                        revisitTime = ncread(fullfile(dir_spdata(iNrx, iAlt, iInc, iNplane),...
                            filename_perform(iNrx, iAlt, iInc, iNplane)), strcat("revisitTime_", TxCombName{iTxComb}));
                        % revisitRow = ncread(fullfile(dir_spdata(iNrx, iAlt, iInc, iNplane),...
                        %     filename_perform(iNrx, iAlt, iInc, iNplane)), strcat("revisitRow_", TxCombName{iTxComb}));
                        % revisitCol = ncread(fullfile(dir_spdata(iNrx, iAlt, iInc, iNplane),...
                        %     filename_perform(iNrx, iAlt, iInc, iNplane)), strcat("revisitCol_", TxCombName{iTxComb}));
                        percentile = prctile(revisitTime, [50, 90, 95, 98])/60/60/24;
                        revisitTime50th(iNrx, iAlt, iInc, iNplane, iTxComb) = percentile(1);
                        revisitTime90th(iNrx, iAlt, iInc, iNplane, iTxComb) = percentile(2);
                        revisitTime95th(iNrx, iAlt, iInc, iNplane, iTxComb) = percentile(3);
                        revisitTime98th(iNrx, iAlt, iInc, iNplane, iTxComb) = percentile(4);
                        revisitTimeMean(iNrx, iAlt, iInc, iNplane, iTxComb) = mean(revisitTime)/60/60/24;
                        % Ratio of valid SP to invalid SP
                        nSpValid(iNrx, iAlt, iInc, iNplane, iTxComb) = ncread(fullfile(dir_spdata(iNrx, iAlt, iInc, iNplane),...
                            filename_perform(iNrx, iAlt, iInc, iNplane)), strcat("nSpValid_", TxCombName{iTxComb}));
                        nSpInvalid(iNrx, iAlt, iInc, iNplane, iTxComb) = ncread(fullfile(dir_spdata(iNrx, iAlt, iInc, iNplane),...
                            filename_perform(iNrx, iAlt, iInc, iNplane)), strcat("nSpInvalid_", TxCombName{iTxComb}));
                        nSpRatio(iNrx, iAlt, iInc, iNplane, iTxComb) = nSpValid(iNrx, iAlt, iInc, iNplane, iTxComb) /...
                            (nSpValid(iNrx, iAlt, iInc, iNplane, iTxComb) + nSpInvalid(iNrx, iAlt, iInc, iNplane, iTxComb));
                    end                    
                end
            end
        end
    end
    %% MULTI TO SINGLE DIMENSION
    consName = reshape(consName, [], 1);
    nCase = length(consName);
    costSats = reshape(costSats, [], 1);
    costDedicated = reshape(costDedicated, [], 1);
    costRideshare = reshape(costRideshare, [], 1);
    launchOption = reshape(launchOption, [], 1);
    nLaunch = reshape(nLaunch, [], 1);
    flagDedicated = launchOption == 1;
    costLaunch = zeros(length(flagDedicated),1);
    costLaunch(flagDedicated) = costDedicated(flagDedicated);
    costLaunch(~flagDedicated) = costRideshare(~flagDedicated);
    costTotal = costSats + costLaunch;
    coverageMax = reshape(coverageMax, nCase, nTxComb);
    MSC1T1st = reshape(squeeze(coverage1T1st(:,:,:,:,end,:)), [], nFreqComb);
    MSC1T2nd = reshape(squeeze(coverage1T2nd(:,:,:,:,end,:)), [], nFreqComb);
    MSC2T1st = reshape(squeeze(coverage2T1st(:,:,:,:,end,:)), [], nFreqComb);
    MSC2T2nd = reshape(squeeze(coverage2T2nd(:,:,:,:,end,:)), [], nFreqComb);
    MTC1Tglobal = reshape(coverage1Tglobal, [], nFreqComb);
    MTC2Tglobal = reshape(coverage2Tglobal, [], nFreqComb);
    revisitTime50th = reshape(revisitTime50th, nCase, nTxComb);
    revisitTime90th = reshape(revisitTime90th, nCase, nTxComb);
    revisitTime95th = reshape(revisitTime95th, nCase, nTxComb);
    revisitTime98th = reshape(revisitTime98th, nCase, nTxComb);
    revisitTimeMean = reshape(revisitTimeMean, nCase, nTxComb);
    nSpValid = reshape(nSpValid, nCase, nTxComb);
    nSpInvalid = reshape(nSpInvalid, nCase, nTxComb);
    nSpRatio = reshape(nSpRatio, nCase, nTxComb);

    %% SAVE FIGURE OF MERIT
    filename_out = sprintf("FoM_%s.mat", projName);
    save(fullfile(dir_out, filename_out), 'nRx', 'alt', 'inc', 'nPlane', 'TxCombName', 'consName', ...
                    'costTotal', 'costSats', 'costDedicated', 'costRideshare', 'launchOption', 'nLaunch', ...
                    'coverageMax', 'coverageMap', 'coverageMap1T', 'coverageMap2T', ...
                    'MSC1T1st', 'coverage1T1st', 'MSC1T2nd', 'coverage1T2nd', ...
                    'MSC2T1st', 'coverage2T1st', 'MSC2T2nd', 'coverage2T2nd', ...
                    'MTC1Tglobal', 'coverage1Tzonal', 'MTC2Tglobal', 'coverage2Tzonal', ...
                    'revisitTime50th', 'revisitTime90th', 'revisitTime95th', 'revisitTime98th', 'revisitTimeMean', ...
                    'nSpValid', 'nSpInvalid', 'nSpRatio', ...
                    '-v7.3');

end