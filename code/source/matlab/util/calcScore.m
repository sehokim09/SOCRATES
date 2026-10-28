function calcScore(projName, DesignParamsObj, fom)
% function calcScore
%
%   Calculates score and save score_[projName].mat
%
%   A design candidate that violates a threshold, x < L for an SSF3 score
%   (SSC, MSC) or x > U for an SSF9 score (cost, SRT, MRT), is infeasible
%   and excluded before ranking, so the saved ranking holds only the
%   candidates that satisfy all threshold requirements. A NaN FoM counts as
%   a violation. feasible, saved with the ranking, marks them in fom order.
%   The infeasible candidates are ranked apart in infeasible, which has the
%   same fields plus violation, the thresholds each one violates.
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
    score.cost = wymoreSSF9(L,B,U,S,D,cost);
    feasible = true(numel(cost), 1);
    violation = strings(numel(cost), 1);
    [feasible, violation] = checkThreshold(feasible, violation, cost <= U, "Cost");
    
    % Single-frequency Spatial Coverage
    L = [DesignParamsObj.perform.SSC.score.lower];
    B = [DesignParamsObj.perform.SSC.score.baseline];
    U = [DesignParamsObj.perform.SSC.score.upper];
    S = 1./(B-L);
    D = reshape([DesignParamsObj.perform.SSC.score.domain], 2, []);
    score.SSCWeightedSum = 0;
    for iTxComb = 1 : DesignParamsObj.nTxComb
        score.SSC(:,iTxComb) = wymoreSSF3(L(iTxComb),B(iTxComb),U(iTxComb),S(iTxComb),D(:,iTxComb),fom.SSC(:,iTxComb));
        [feasible, violation] = checkThreshold(feasible, violation, fom.SSC(:,iTxComb) >= L(iTxComb), ...
            "SSC " + DesignParamsObj.TxComb(iTxComb).name);
        score.SSCWeightedSum = score.SSCWeightedSum + DesignParamsObj.perform.SSC.score(iTxComb).weight*score.SSC(:,iTxComb);
    end

    % Single-frequency Revisit Time
    L = [DesignParamsObj.perform.SRT.score.lower];
    B = [DesignParamsObj.perform.SRT.score.baseline];
    U = [DesignParamsObj.perform.SRT.score.upper];
    S = 1./(B-L);
    D = reshape([DesignParamsObj.perform.SRT.score.domain], 2, []);
    score.SRTWeightedSum = 0;
    switch DesignParamsObj.perform.SRT.type
        case '1T global'
            SRT = fom.SRT1T;
        case '2T global'
            SRT = fom.SRT2T;
        otherwise
            error('calcScore:revisitType', ...
                ['Revisit-time type "%s" is no longer computed. Set it to ''1T global'' ', ...
                 'or ''2T global'' (SRT over 12- or 24-hour windows).'], DesignParamsObj.perform.SRT.type);
    end
    for iTxComb = 1 : DesignParamsObj.nTxComb
        score.SRT(:,iTxComb) = wymoreSSF9(L(iTxComb),B(iTxComb),U(iTxComb),S(iTxComb),D(:,iTxComb),SRT(:,iTxComb));
        [feasible, violation] = checkThreshold(feasible, violation, SRT(:,iTxComb) <= U(iTxComb), ...
            "SRT " + DesignParamsObj.TxComb(iTxComb).name);
        score.SRTWeightedSum = score.SRTWeightedSum + DesignParamsObj.perform.SRT.score(iTxComb).weight*score.SRT(:,iTxComb);
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
        score.MSC(:,iFreqComb) = wymoreSSF3(L(iFreqComb),B(iFreqComb),U(iFreqComb),S(iFreqComb),D(:,iFreqComb),MSC(:,iFreqComb));
        [feasible, violation] = checkThreshold(feasible, violation, MSC(:,iFreqComb) >= L(iFreqComb), ...
            "MSC " + DesignParamsObj.freqComb(iFreqComb).name);
        score.MSCWeightedSum = score.MSCWeightedSum + DesignParamsObj.perform.MSC.score(iFreqComb).weight*score.MSC(:,iFreqComb);
    end
    
    % Multi-frequency Temporal Coverage
    L = [DesignParamsObj.perform.MRT.score.lower];
    B = [DesignParamsObj.perform.MRT.score.baseline];
    U = [DesignParamsObj.perform.MRT.score.upper];
    S = 1./(B-L);
    D = reshape([DesignParamsObj.perform.MRT.score.domain], 2, []);
    score.MRTWeightedSum = 0;
    switch DesignParamsObj.perform.MRT.type
        case '1T global'
            MRT = fom.MRT1T;
        case '2T global'
            MRT = fom.MRT2T;
    end
    for iFreqComb = 1 : DesignParamsObj.nFreqComb
        score.MRT(:,iFreqComb) = wymoreSSF9(L(iFreqComb),B(iFreqComb),U(iFreqComb),S(iFreqComb),D(:,iFreqComb),MRT(:,iFreqComb));
        [feasible, violation] = checkThreshold(feasible, violation, MRT(:,iFreqComb) <= U(iFreqComb), ...
            "MRT " + DesignParamsObj.freqComb(iFreqComb).name);
        score.MRTWeightedSum = score.MRTWeightedSum + DesignParamsObj.perform.MRT.score(iFreqComb).weight*score.MRT(:,iFreqComb);
    end

    % Linear combination
    score.perform =  DesignParamsObj.perform.SSC.weight*score.SSCWeightedSum ...
                       + DesignParamsObj.perform.SRT.weight*score.SRTWeightedSum ...
                       + DesignParamsObj.perform.MSC.weight*score.MSCWeightedSum ...
                       + DesignParamsObj.perform.MRT.weight*score.MRTWeightedSum;
    score.final = DesignParamsObj.cost.weight*score.cost + DesignParamsObj.perform.weight*score.perform;
    
    % Exclude the infeasible candidates, then rank the rest
    idxFeasible = find(feasible);
    fprintf('calcScore: %d of %d candidates satisfy all thresholds; %d excluded before ranking\n', ...
        numel(idxFeasible), numel(feasible), numel(feasible) - numel(idxFeasible));
    if isempty(idxFeasible)
        warning('calcScore:noFeasible', 'No candidate satisfies all thresholds; the ranking is empty.');
    end

    % Sorting: the feasible candidates, and the infeasible ones apart
    ranked = rankCandidates(score, fom, idxFeasible);
    ranked.feasible = feasible;
    ranked.infeasible = rankCandidates(score, fom, find(~feasible));
    ranked.infeasible.violation = violation(ranked.infeasible.idx);
    
    % Save
    dir_out = fullfile('sim', projName, 'out');
    filename_out = sprintf("score_%s.mat", projName);
    save(fullfile(dir_out, filename_out), '-struct', 'ranked');
end

function [feasible, violation] = checkThreshold(feasible, violation, ok, label)
% CHECKTHRESHOLD  Clear feasible where ok is false and add label to the
%   violations listed there
    bad = ~ok;
    more = bad & violation ~= "";
    violation(more) = violation(more) + ", ";
    violation(bad) = violation(bad) + label;
    feasible = feasible & ok;
end

function R = rankCandidates(score, fom, idx)
% RANKCANDIDATES  Scores of the candidates idx (fom order), sorted by the
%   final score
    [R.final, order] = sort(score.final(idx), 'descend');
    R.idx = idx(order);
    R.name = fom.consName(R.idx);
    R.cost = score.cost(R.idx);
    R.perform = score.perform(R.idx);
    R.SSC = score.SSC(R.idx,:);
    R.SSCWeightedSum = score.SSCWeightedSum(R.idx);
    R.SRT = score.SRT(R.idx,:);
    R.SRTWeightedSum = score.SRTWeightedSum(R.idx);
    R.MSC = score.MSC(R.idx,:);
    R.MSCWeightedSum = score.MSCWeightedSum(R.idx);
    R.MRT = score.MRT(R.idx,:);
    R.MRTWeightedSum = score.MRTWeightedSum(R.idx);
end