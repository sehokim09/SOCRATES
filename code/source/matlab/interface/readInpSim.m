function [SimParamsObj, SoOpParamsObj, RxScParamsObj] = readInpSim(orbPath, InorbitParamsObj, SoOpParamsObj)
% function readInpSim
%
%   Initializes SimParamsObj, SoOpParamsObj, and RxScParamsObj from
%   Inp_Sim.txt.
%
%   See also satObs, missionDesigner, SimParms, SoOpParams, ScParams,
%   and InorbitParams.
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

    SimParamsObj = SimParams;

    fileID = fopen(fullfile(orbPath,"Inp_Sim.txt"), "r");

    fgetl(fileID);
    fgetl(fileID);

    %  Time Mode (FAST, REAL, or EXTERNAL)
    fgetl(fileID); % Skip
    %  Sim Duration, Step Size [sec]
    line = fgetl(fileID); 
    temp = sscanf(line, "%f %f %*[\n]");
    SimParamsObj.duration = temp(1);
    SimParamsObj.stepSize = temp(2);
    %  File Output Interval [sec]
    line = fgetl(fileID); 
    SimParamsObj.outInterval = sscanf(line, "%f %*[\n]");
    %  Graphics Front End?
    fgetl(fileID); % Skip
    %  Command Script File Name
    fgetl(fileID); % Skip
    
    fgetl(fileID);
    % Reference Orbits
    % Number of Reference Orbits
    line = fgetl(fileID); 
    SimParamsObj.nOrb = sscanf(line, "%d %*[\n]");
    for iOrb = 1 : SimParamsObj.nOrb
        % Existence, Input file name for Orb
        line = fgetl(fileID);
        [SimParamsObj.orb(iOrb).exist, ~, ~, nextIdx] = sscanf(line, "%s %*[\n]");
        line = line(nextIdx:end);
        temp = sscanf(line, "%s %*[\n]");
        SimParamsObj.orb(iOrb).filename = string(temp);
    end
    fgetl(fileID); 
    
    % Spacecraft
    % Number of Spacecraft
    line = fgetl(fileID); 
    SimParamsObj.nSc = sscanf(line, "%d %*[\n]");
    for iSc = 1 : SimParamsObj.nSc
        % Existence, RefOrb, Input file for SC 0
        line = fgetl(fileID);
        [SimParamsObj.sc(iSc).exist, ~, ~, nextIdx] = sscanf(line, "%s %*[\n]");
        line = line(nextIdx:end);
        temp = sscanf(line, "%d %s %*[\n]", [1 inf]);
        SimParamsObj.sc(iSc).orbID = temp(1);
        SimParamsObj.sc(iSc).filename = char(temp(2:end));
    end
    
    fgetl(fileID);
    % Environment
    % Date (UTC) (Month, Day, Year)
    line = fgetl(fileID);
    temp = sscanf(line, "%d %d %d %*[\n]");
    month = temp(1);
    day = temp(2);
    year = temp(3);
    % Time (UTC) (Hr,Min,Sec)
    line = fgetl(fileID);
    temp = sscanf(line, "%d %d %f %*[\n]");
    hour = temp(1);
    min = temp(2);
    sec = temp(3);
    % Leap Seconds (sec)
    line = fgetl(fileID);
    leapSec = sscanf(line, "%f %*[\n]");
    % Date
    SimParamsObj.epoch = datetime(year, month, day, hour, min, sec, 'TimeZone', 'UTC');
    fclose(fileID);

    orbFilePath = fullfile(fileparts(orbPath), 'Orb');
    
    % Rx
    for iRx = 1 : InorbitParamsObj.nRx
        RxScParamsObj(iRx) = readInpOrb(fullfile(orbFilePath, SimParamsObj.orb(iRx).filename));
        RxScParamsObj(iRx).filepath.TLE = fullfile(orbPath, RxScParamsObj(iRx).filepath.filename);
        RxScParamsObj(iRx).filepath.import = fullfile(fileparts(fileparts(orbPath)), 'import');
    end
    % Tx
    for iSoOp = 1 : InorbitParamsObj.nSoOp
        i = 1;
        for iTx = InorbitParamsObj.TxRefOrb(iSoOp,1)+1 : InorbitParamsObj.TxRefOrb(iSoOp,2)+1
            SoOpParamsObj(iSoOp).TxScParamsObj(i,1) = readInpOrb(fullfile(orbFilePath, SimParamsObj.orb(iTx).filename));
            SoOpParamsObj(iSoOp).TxScParamsObj(i).filepath.TLE = fullfile(orbPath, SoOpParamsObj(iSoOp).TxScParamsObj(i).filepath.filename);
            SoOpParamsObj(iSoOp).TxScParamsObj(i).filepath.import = fullfile(fileparts(fileparts(orbPath)), 'import');
            i = i + 1;
        end
    end
end
