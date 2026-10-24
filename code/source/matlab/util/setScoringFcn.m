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
    
    % Single-frequency Spatial Coverage
    for iTxComb = 1 : DesignParamsObj.nTxComb
        DesignParamsObj.perform.SSC.score(iTxComb).weight = 1/DesignParamsObj.nTxComb;
        DesignParamsObj.perform.SSC.score(iTxComb).lower = min(fom.SSC(:,iTxComb));
        DesignParamsObj.perform.SSC.score(iTxComb).baseline = mean(fom.SSC(:,iTxComb));
        DesignParamsObj.perform.SSC.score(iTxComb).upper = max(fom.SSC(:,iTxComb));
        DesignParamsObj.perform.SSC.score(iTxComb).domain = ...
            [DesignParamsObj.perform.SSC.score(iTxComb).lower, DesignParamsObj.perform.SSC.score(iTxComb).upper];
    end
    
    % Single-frequency Revisit Time
    switch DesignParamsObj.perform.SRT.type
        case '1T global'
            SRT = fom.SRT1T;
        case '2T global'
            SRT = fom.SRT2T;
    end
    for iTxComb = 1 : DesignParamsObj.nTxComb
        DesignParamsObj.perform.SRT.score(iTxComb).weight = 1/DesignParamsObj.nTxComb;
        DesignParamsObj.perform.SRT.score(iTxComb).lower = min(SRT(:,iTxComb));
        DesignParamsObj.perform.SRT.score(iTxComb).baseline = mean(SRT(:,iTxComb));
        DesignParamsObj.perform.SRT.score(iTxComb).upper = max(SRT(:,iTxComb));
        DesignParamsObj.perform.SRT.score(iTxComb).domain = ...
            [DesignParamsObj.perform.SRT.score(iTxComb).lower, DesignParamsObj.perform.SRT.score(iTxComb).upper];
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

    % Multi-frequency Revisit Time
    switch DesignParamsObj.perform.MRT.type
        case '1T global'
            MRT = fom.MRT1T;
        case '2T global'
            MRT = fom.MRT2T;
    end
    for iFreqComb = 1 : DesignParamsObj.nFreqComb
        DesignParamsObj.perform.MRT.score(iFreqComb).weight = 1/DesignParamsObj.nFreqComb;
        DesignParamsObj.perform.MRT.score(iFreqComb).lower = min(MRT(:,iFreqComb));
        DesignParamsObj.perform.MRT.score(iFreqComb).baseline = mean(MRT(:,iFreqComb));
        DesignParamsObj.perform.MRT.score(iFreqComb).upper = max(MRT(:,iFreqComb));
        DesignParamsObj.perform.MRT.score(iFreqComb).domain = ...
            [DesignParamsObj.perform.MRT.score(iFreqComb).lower, DesignParamsObj.perform.MRT.score(iFreqComb).upper];
    end
end