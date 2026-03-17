function DesignParamsObj = setScoringFcn(DesignParamsObj, fom)
% function setScoringFcn
%
%   Set parameters of scoring functions.
%
%   See also missionDesigner.
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
    DesignParamsObj.cost.lower = min(cost);
    DesignParamsObj.cost.baseline = mean(cost);
    DesignParamsObj.cost.upper = max(cost);
    DesignParamsObj.cost.domain = [DesignParamsObj.cost.lower, DesignParamsObj.cost.upper];
    
    % Max coverage
    for iTxComb = 1 : DesignParamsObj.nTxComb
        DesignParamsObj.perform.maxCoverage.score(iTxComb).weight = 1/DesignParamsObj.nTxComb;
        DesignParamsObj.perform.maxCoverage.score(iTxComb).lower = min(fom.coverageMax(:,iTxComb));
        DesignParamsObj.perform.maxCoverage.score(iTxComb).baseline = mean(fom.coverageMax(:,iTxComb));
        DesignParamsObj.perform.maxCoverage.score(iTxComb).upper = max(fom.coverageMax(:,iTxComb));
        DesignParamsObj.perform.maxCoverage.score(iTxComb).domain = ...
            [DesignParamsObj.perform.maxCoverage.score(iTxComb).lower, DesignParamsObj.perform.maxCoverage.score(iTxComb).upper];
    end
    
    % Revisit time
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
        DesignParamsObj.perform.revisitTime.score(iTxComb).weight = 1/DesignParamsObj.nTxComb;
        DesignParamsObj.perform.revisitTime.score(iTxComb).lower = min(revisitTime(:,iTxComb));
        DesignParamsObj.perform.revisitTime.score(iTxComb).baseline = mean(revisitTime(:,iTxComb));
        DesignParamsObj.perform.revisitTime.score(iTxComb).upper = max(revisitTime(:,iTxComb));
        DesignParamsObj.perform.revisitTime.score(iTxComb).domain = ...
            [DesignParamsObj.perform.revisitTime.score(iTxComb).lower, DesignParamsObj.perform.revisitTime.score(iTxComb).upper];
    end
    
    % Multi-frequency Spatial Coverage
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
        DesignParamsObj.perform.MSC.score(iFreqComb).weight = 1/DesignParamsObj.nFreqComb;
        DesignParamsObj.perform.MSC.score(iFreqComb).lower = min(MSC(:,iFreqComb));
        DesignParamsObj.perform.MSC.score(iFreqComb).baseline = mean(MSC(:,iFreqComb));
        DesignParamsObj.perform.MSC.score(iFreqComb).upper = max(MSC(:,iFreqComb));
        DesignParamsObj.perform.MSC.score(iFreqComb).domain = ...
            [DesignParamsObj.perform.MSC.score(iFreqComb).lower, DesignParamsObj.perform.MSC.score(iFreqComb).upper];
    end

    % Multi-frequency Temporal Coverage
    switch DesignParamsObj.perform.MTC.type
        case '1T global'
            MTC = fom.MTC1Tglobal;
        case '2T global'
            MTC = fom.MTC2Tglobal;
    end
    for iFreqComb = 1 : DesignParamsObj.nFreqComb
        DesignParamsObj.perform.MTC.score(iFreqComb).weight = 1/DesignParamsObj.nFreqComb;
        DesignParamsObj.perform.MTC.score(iFreqComb).lower = min(MTC(:,iFreqComb));
        DesignParamsObj.perform.MTC.score(iFreqComb).baseline = mean(MTC(:,iFreqComb));
        DesignParamsObj.perform.MTC.score(iFreqComb).upper = max(MTC(:,iFreqComb));
        DesignParamsObj.perform.MTC.score(iFreqComb).domain = ...
            [DesignParamsObj.perform.MTC.score(iFreqComb).lower, DesignParamsObj.perform.MTC.score(iFreqComb).upper];
    end
end