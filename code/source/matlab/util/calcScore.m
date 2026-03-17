function calcScore(projName, DesignParamsObj, fom)
% function calcScore
%
%   Calculates score and save score_[projName].mat
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

    % Cost
    L = DesignParamsObj.cost.lower;
    B = DesignParamsObj.cost.baseline;
    U = DesignParamsObj.cost.upper;
    S = 1/(B-L);
    D = DesignParamsObj.cost.domain;
    switch DesignParamsObj.cost.type
        case 'Total'
            cost = fom.costTotal;
        case 'Launch'
            flagDedicated = fom.launchOption == 1;
            cost = zeros(length(flagDedicated),1);
            cost(flagDedicated) = fom.costDedicated(flagDedicated);
            cost(~flagDedicated) = fom.costRideshare(~flagDedicated);
        case 'Constellation'
            cost = fom.costSats;
    end
    score.cost = penaltyWymoreSSF9(L,B,U,S,D,cost);
    
    % Max coverage
    L = [DesignParamsObj.perform.maxCoverage.score.lower];
    B = [DesignParamsObj.perform.maxCoverage.score.baseline];
    U = [DesignParamsObj.perform.maxCoverage.score.upper];
    S = 1./(B-L);
    D = reshape([DesignParamsObj.perform.maxCoverage.score.domain], 2, []);
    score.coverageMaxWeightedSum = 0;
    for iTxComb = 1 : DesignParamsObj.nTxComb
        score.coverageMax(:,iTxComb) = penaltyWymoreSSF3(L(iTxComb),B(iTxComb),U(iTxComb),S(iTxComb),D(:,iTxComb),fom.coverageMax(:,iTxComb));
        score.coverageMaxWeightedSum = score.coverageMaxWeightedSum + DesignParamsObj.perform.maxCoverage.score(iTxComb).weight*score.coverageMax(:,iTxComb);
    end

    
    % Revisit time
    L = [DesignParamsObj.perform.revisitTime.score.lower];
    B = [DesignParamsObj.perform.revisitTime.score.baseline];
    U = [DesignParamsObj.perform.revisitTime.score.upper];
    S = 1./(B-L);
    D = reshape([DesignParamsObj.perform.revisitTime.score.domain], 2, []);
    score.revisitTimeWeightedSum = 0;
    switch DesignParamsObj.perform.revisitTime.type
        case '50th Percentile'
            revisitTime = fom.revisitTime50th;
        case '90th Percentile'
            revisitTime = fom.revisitTime90th;
        case '95th Percentile'
            revisitTime = fom.revisitTime95th;
        case '98th Percentile'
            revisitTime = fom.revisitTime98th;
        case 'Mean'
            revisitTime = fom.revisitTimeMean;
    end
    for iTxComb = 1 : DesignParamsObj.nTxComb
        score.revisitTime(:,iTxComb) = penaltyWymoreSSF9(L(iTxComb),B(iTxComb),U(iTxComb),S(iTxComb),D(:,iTxComb),revisitTime(:,iTxComb));
        score.revisitTimeWeightedSum = score.revisitTimeWeightedSum + DesignParamsObj.perform.revisitTime.score(iTxComb).weight*score.revisitTime(:,iTxComb);
    end
    
    % Multi-frequency Spatial Coverage
    L = [DesignParamsObj.perform.MSC.score.lower];
    B = [DesignParamsObj.perform.MSC.score.baseline];
    U = [DesignParamsObj.perform.MSC.score.upper];
    S = 1./(B-L);
    D = reshape([DesignParamsObj.perform.MSC.score.domain], 2, []);
    score.MSCWeightedSum = 0;
    switch DesignParamsObj.perform.MSC.type
        case '1T visit'
            MSC = fom.MSC1T1st;
        case '1T revisit'
            MSC = fom.MSC1T2nd;
        case '2T visit'
            MSC = fom.MSC2T1st;
        case '2T revisit'
            MSC = fom.MSC2T2nd;
    end
    for iFreqComb = 1 : DesignParamsObj.nFreqComb
        score.MSC(:,iFreqComb) = penaltyWymoreSSF3(L(iFreqComb),B(iFreqComb),U(iFreqComb),S(iFreqComb),D(:,iFreqComb),MSC(:,iFreqComb));
        score.MSCWeightedSum = score.MSCWeightedSum + DesignParamsObj.perform.MSC.score(iFreqComb).weight*score.MSC(:,iFreqComb);
    end
    
    % Multi-frequency Temporal Coverage
    L = [DesignParamsObj.perform.MTC.score.lower];
    B = [DesignParamsObj.perform.MTC.score.baseline];
    U = [DesignParamsObj.perform.MTC.score.upper];
    S = 1./(B-L);
    D = reshape([DesignParamsObj.perform.MTC.score.domain], 2, []);
    score.MTCWeightedSum = 0;
    switch DesignParamsObj.perform.MTC.type
        case '1T global'
            MTC = fom.MTC1Tglobal;
        case '2T global'
            MTC = fom.MTC2Tglobal;
    end
    for iFreqComb = 1 : DesignParamsObj.nFreqComb
        score.MTC(:,iFreqComb) = penaltyWymoreSSF3(L(iFreqComb),B(iFreqComb),U(iFreqComb),S(iFreqComb),D(:,iFreqComb),MTC(:,iFreqComb));
        score.MTCWeightedSum = score.MTCWeightedSum + DesignParamsObj.perform.MTC.score(iFreqComb).weight*score.MTC(:,iFreqComb);
    end

    % Linear combination
    score.perform =  DesignParamsObj.perform.maxCoverage.weight*score.coverageMaxWeightedSum ...
                       + DesignParamsObj.perform.revisitTime.weight*score.revisitTimeWeightedSum ...
                       + DesignParamsObj.perform.MSC.weight*score.MSCWeightedSum ...
                       + DesignParamsObj.perform.MTC.weight*score.MTCWeightedSum;
    score.final = DesignParamsObj.cost.weight*score.cost + DesignParamsObj.perform.weight*score.perform;
    
    % Sorting
    [final, idx] = sort(score.final, 'descend');
    name = fom.consName(idx);
    cost = score.cost(idx);
    perform = score.perform(idx);
    coverageMax = score.coverageMax(idx,:);
    coverageMaxWeightedSum = score.coverageMaxWeightedSum(idx);
    revisitTime = score.revisitTime(idx,:);
    revisitTimeWeightedSum = score.revisitTimeWeightedSum(idx);
    MSC = score.MSC(idx);
    MSCWeightedSum = score.MSCWeightedSum(idx);
    MTC = score.MTC(idx);
    MTCWeightedSum = score.MTCWeightedSum(idx);
    
    % Save
    dir_out = fullfile('sim', projName, 'out');
    filename_out = sprintf("score_%s.mat", projName);
    save(fullfile(dir_out, filename_out), 'name', 'final', 'idx', 'cost', 'perform', ...
        'coverageMax', 'coverageMaxWeightedSum', 'revisitTime', 'revisitTimeWeightedSum', ...
        'MSC', 'MSCWeightedSum', ...
        'MTC', 'MTCWeightedSum');
end