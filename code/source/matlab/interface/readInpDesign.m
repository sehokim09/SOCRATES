function DesignParamsObj = readInpDesign(projPath)
% function readInpDesign
%
%   Initializes DesignParamsObj from Inp_Design.txt.
%
%   See also missionDesigner, DesignParams.
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
%   along with this program. If not, see <https://www.gnu.org/licenses/>.
%
%   Version 1.0

    DesignParamsObj = DesignParams;

    fileID = fopen(fullfile(projPath,"Inp_Design.txt"), "r");
    
    fgetl(fileID);

    % Constellation Type (WALKER, HYBRID)
    line = fgetl(fileID);
    temp = sscanf(line, "%s %*[\n]");
    switch temp
        case 'WALKER'
            DesignParamsObj.consType = 'Walker';
        case 'HYBRID'
            DesignParamsObj.consType = 'Hybrid';
    end
    fgetl(fileID);
    
    % Walker Design Variables
    % Number of # Receiver Variable
    line = fgetl(fileID);
    DesignParamsObj.walker.n_nRx = sscanf(line, "%d %*[\n]");
    % # Receiver
    line = fgetl(fileID);
    DesignParamsObj.walker.nRx = sscanf(line, "%d,")';
    fgetl(fileID);
    % Number of Altitude Variable
    line = fgetl(fileID);
    DesignParamsObj.walker.nAlt = sscanf(line, "%d %*[\n]");
    % Altitude [km]
    line = fgetl(fileID);
    DesignParamsObj.walker.alt = sscanf(line, "%f,")';
    fgetl(fileID);
    % Number of Inclination Variable
    line = fgetl(fileID);
    DesignParamsObj.walker.nInc = sscanf(line, "%d %*[\n]");
    % Inclination [deg]
    line = fgetl(fileID);
    incList = strsplit(line, ", ");
    for i = 1 : length(incList)
        DesignParamsObj.walker.inc(i) = string(sscanf(incList{i}, "%s %*[\n]"));
    end
    fgetl(fileID);
    % Number of # Orbital Plane Variable
    line = fgetl(fileID);
    DesignParamsObj.walker.n_nPlane = sscanf(line, "%d %*[\n]");
    % # Orbital Plane
    line = fgetl(fileID);
    DesignParamsObj.walker.nPlane = sscanf(line, "%d,")';
    fgetl(fileID);
    
    % Hybrid Design Variables
    % Constellatoin Name
    line = fgetl(fileID);
    DesignParamsObj.hybrid.name = sscanf(line, "%s %*[\n]");
    % # Orbital Plane
    line = fgetl(fileID);
    DesignParamsObj.hybrid.nPlane = sscanf(line, "%d %*[\n]");
    for iPlane = 1 : DesignParamsObj.hybrid.nPlane
        fgetl(fileID);
        % # Receiver
        line = fgetl(fileID);
        DesignParamsObj.hybrid.nRx(iPlane) = sscanf(line, "%d %*[\n]");
        % Altitude [km]
        line = fgetl(fileID);
        DesignParamsObj.hybrid.alt(iPlane) = sscanf(line, "%f %*[\n]");
        % Inclination [deg]
        line = fgetl(fileID);
        DesignParamsObj.hybrid.inc(iPlane) = string(sscanf(line, "%s %*[\n]"));
    end
    fgetl(fileID);

    % Real-time Transmitter Propagation
    line = fgetl(fileID);
    temp = sscanf(line, "%s %*[\n]");
    if strcmp(temp,"TRUE")
        DesignParamsObj.flagTxPropagation = true;
    else
        DesignParamsObj.flagTxPropagation = false;
    end
    % Observation Time Window [hour]
    line = fgetl(fileID);
    DesignParamsObj.window = sscanf(line, "%f %*[\n]");
    % Longitudinal Grid Divisor
    line = fgetl(fileID);
    DesignParamsObj.gridDivisor = sscanf(line, "%d %*[\n]");
    % Large File Storage Root Directory
    line = fgetl(fileID);
    DesignParamsObj.largeFilePath = sscanf(line, "%s %*[\n]");
    fgetl(fileID);
    
    % Individual Receiver Data (per Inp_Report.txt)
    line = fgetl(fileID);
    temp = sscanf(line, "%s %*[\n]");
    if strcmp(temp,"TRUE")
        DesignParamsObj.flagReport = true;
    else
        DesignParamsObj.flagReport = false;
    end
    fgetl(fileID);

    % Coverage Analysis Parameters
    % Number of SoOp (Transmitter Combinations)
    line = fgetl(fileID);
    DesignParamsObj.nTxComb = sscanf(line, "%d %*[\n]");
    
    for iTxComb = 1 : DesignParamsObj.nTxComb
        fgetl(fileID);
        % SoOp Name
        line = fgetl(fileID);
        DesignParamsObj.TxComb(iTxComb).name = sscanf(line, "%s %*[\n]");
        % Number of Tx Systems
        line = fgetl(fileID);
        DesignParamsObj.TxComb(iTxComb).nTx = sscanf(line, "%d %*[\n]");
        for iTx = 1 : DesignParamsObj.TxComb(iTxComb).nTx
            % Tx Name (From Inp_Inorbit.txt)
            line = fgetl(fileID);
            DesignParamsObj.TxComb(iTxComb).TxName(iTx) = string(sscanf(line, "%s %*[\n]"));
        end
    end

    % The SAI Observation Model section that follows is written by
    % export_socrates_inputs.m and kept as is by createInpDesign.
    fclose(fileID);
    
    % Initialize frequency combinations
    DesignParamsObj.nFreqComb = 1;
    DesignParamsObj.freqComb.name = 'All';
    DesignParamsObj.freqComb.nTxComb = DesignParamsObj.nTxComb;
    DesignParamsObj.freqComb.TxCombName = string({DesignParamsObj.TxComb.name});
    DesignParamsObj.freqComb.TxCombID = pow2(0:DesignParamsObj.freqComb.nTxComb-1);
    DesignParamsObj.freqComb.ID = pow2(DesignParamsObj.freqComb.nTxComb)-1;

    % Initialize scoring function parameters
    DesignParamsObj.perform.SSC.score(1).weight = 1 / DesignParamsObj.nTxComb;
    DesignParamsObj.perform.SRT.score(1).weight = 1 / DesignParamsObj.nTxComb;
    for iTxComb = 2 : DesignParamsObj.nTxComb
        DesignParamsObj.perform.SSC.score(iTxComb) = DesignParamsObj.perform.SSC.score(1);
        DesignParamsObj.perform.SRT.score(iTxComb) = DesignParamsObj.perform.SRT.score(1);
    end
    DesignParamsObj.perform.MSC.score(1).weight = 1 / DesignParamsObj.nFreqComb;
    DesignParamsObj.perform.MRT.score(1).weight = 1 / DesignParamsObj.nFreqComb;
    for iFreqComb = 2 : DesignParamsObj.nFreqComb
        DesignParamsObj.perform.MSC.score(iFreqComb) = DesignParamsObj.perform.MSC.score(1);
        DesignParamsObj.perform.MRT.score(iFreqComb) = DesignParamsObj.perform.MRT.score(1);
    end
end
