function [InorbitParamsObj, SoOpParamsObj] = readInpInorbit(projPath)
% function readInpInorbit
%
%   Initializes InorbitParamsObj and SoOpParamsObj from Inp_Inorbit.txt.
%
%   See also satObs, missionDesigner, InorbitParamsObj, and
%   SoOpParamsObj.
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

    % Path to antenna pattern folder 
    antPath = fullfile(fileparts(fileparts(projPath)), 'ant_pat');

    InorbitParamsObj = InorbitParams;
    
    fileID = fopen(fullfile(projPath,"Inp_Inorbit.txt"), "r");

    fgetl(fileID);

    % Space-based or Tower-based Observation (SPACE, TOWER)
    line = fgetl(fileID); 
    temp = sscanf(line, "%s %*[\n]");
    switch temp
        case 'SPACE'
            InorbitParamsObj.obsBase = 'Space';
        case 'TOWER'
            InorbitParamsObj.obsBase = 'Tower';
    end
    % Number of Receivers, NRx
    line = fgetl(fileID); 
    InorbitParamsObj.nRx = sscanf(line, "%d %*[\n]");
    % Number of Transmitter Systems, NSoOp
    line = fgetl(fileID); 
    InorbitParamsObj.nSoOp = sscanf(line, "%d %*[\n]");
    % Orbit Propagation Model (IMPORT, 42, 42_DECAY)
    line = fgetl(fileID); 
    InorbitParamsObj.orbModel = sscanf(line, "%s %*[\n]");

    fgetl(fileID);

    % Import Orbits
    % Number of Spacecrafts
    line = fgetl(fileID); 
    InorbitParamsObj.orbImport.nSc = sscanf(line, "%d %*[\n]");
    % Number of Lines in Files
    line = fgetl(fileID); 
    InorbitParamsObj.orbImport.nLine = sscanf(line, "%d %*[\n]");
    for iSc = 1 : InorbitParamsObj.orbImport.nSc
        % SC File Name
        line = fgetl(fileID); 
        InorbitParamsObj.orbImport.filename(iSc,1) = string(sscanf(line, "%s %*[\n]"));
    end
    
    % 42 Orbits
    fgetl(fileID);
    fgetl(fileID);
    % Space-based Rx
    % Rx Name, First RefOrb, Last RefOrb in Inp_Sim.txt
    line = fgetl(fileID); 
    temp = sscanf(line, "%s %d %d %*[\n]", [1 inf]);
    InorbitParamsObj.RxName = char(temp(1:end-2));
    InorbitParamsObj.RxRefOrb = [temp(end-1), temp(end)];
    fgetl(fileID);
    % Tower-based Rx
    % Label, Lat[deg], Lng[deg], Height[m] 
    line = fgetl(fileID); 
    temp = sscanf(line, "%s %f %f %f %*[\n]", [1 inf]);
    InorbitParamsObj.TwrName = char(temp(1:end-3));
    InorbitParamsObj.TwrLLA = [temp(end-2:end)];
    fgetl(fileID);
    % Tx Constellations
    for iSoOp = 1 : InorbitParamsObj.nSoOp
        SoOpParamsObj(iSoOp) = SoOpParams;
        % Tx Name, First RefOrb, Last RefOrb in Inp_Sim.txt
        line = fgetl(fileID); 
        temp = sscanf(line, "%s %d %d %*[\n]", [1 inf]);
        SoOpParamsObj(iSoOp).TxName = char(temp(1:end-2));
        InorbitParamsObj.TxRefOrb(iSoOp,:) = [temp(end-1), temp(end)];
    end

    % SoOp Configuration
    for iSoOp = 1 : InorbitParamsObj.nSoOp
        fgetl(fileID);
        fgetl(fileID);
        fgetl(fileID);
        % Max Number of Specular Points
        line = fgetl(fileID); 
        SoOpParamsObj(iSoOp).nSpMax = sscanf(line, "%d %*[\n]");
        fgetl(fileID);
        % Receiver Max Gain [dB], Polarization (R,L,X,Y)
        line = fgetl(fileID);
        temp = sscanf(line, "%f %s %*[\n]", [1 Inf]);
        SoOpParamsObj(iSoOp).gainRx_dB = temp(1);
        switch char(temp(2:end))
            case 'R'
                SoOpParamsObj(iSoOp).polRx = 'RHCP';
            case 'L'
                SoOpParamsObj(iSoOp).polRx = 'LHCP';
            case 'X'
                SoOpParamsObj(iSoOp).polRx = 'V-pol';
            case 'Y'
                SoOpParamsObj(iSoOp).polRx = 'H-pol';
        end
        % Receiver Normalized Antenna Pattern (IDEAL, GAUSSIAN, USER_DEFINED)
        line = fgetl(fileID);
        temp = sscanf(line, "%s %*[\n]");
        switch temp
            case 'IDEAL'
                SoOpParamsObj(iSoOp).antPatTypeRx = 'Ideal';
            case 'GAUSSIAN'
                SoOpParamsObj(iSoOp).antPatTypeRx = 'Generalized-Gaussian';
            case 'USER_DEFINED'
                SoOpParamsObj(iSoOp).antPatTypeRx = 'User-defined';
        end
        % (GAUSSIAN) Receiver HPBW[deg], Sidelobe[dB], X-pol[dB], Res[deg]
        line = fgetl(fileID);
        temp = sscanf(line, "%f %f %f %f %*[\n]");
        SoOpParamsObj(iSoOp).antPatGG.HPBW_deg = temp(1);
        SoOpParamsObj(iSoOp).antPatGG.SSL_dB = temp(2);
        SoOpParamsObj(iSoOp).antPatGG.XPL_dB = temp(3);
        SoOpParamsObj(iSoOp).antPatGG.res_deg = temp(4);
        % (USER_DEFINED) Receiver Antenna Pattern File Names
        line = fgetl(fileID);
        temp = sscanf(line, "%s %*[\n]");
        SoOpParamsObj(iSoOp).antPatFilepath.Co = fullfile(antPath, temp);
        line = fgetl(fileID);
        temp = sscanf(line, "%s %*[\n]");
        SoOpParamsObj(iSoOp).antPatFilepath.X = fullfile(antPath, temp);
        fgetl(fileID); % skip
        fgetl(fileID); % skip
        
        fgetl(fileID);
        % Transmitter Frequency [MHz]
        line = fgetl(fileID);
        SoOpParamsObj(iSoOp).f_MHz = sscanf(line, "%f %*[\n]");
        % Transmitter EIRP [dB], Polarization (R,L,X,Y)
        line = fgetl(fileID);
        temp = sscanf(line, "%f %s %*[\n]", [1 Inf]);
        SoOpParamsObj(iSoOp).EIRP_Tx_dB = temp(1);
        switch char(temp(2:end))
            case 'R'
                SoOpParamsObj(iSoOp).polTx = 'RHCP';
            case 'L'
                SoOpParamsObj(iSoOp).polTx = 'LHCP';
            case 'X'
                SoOpParamsObj(iSoOp).polTx = 'V-pol';
            case 'Y'
                SoOpParamsObj(iSoOp).polTx = 'H-pol';
        end
        % Transmitter Bandwidth of channel [kHz]
        line = fgetl(fileID);
        SoOpParamsObj(iSoOp).BW_Tx_kHz = sscanf(line, "%f %*[\n]");
        fgetl(fileID);
        % Instrument
        % Coherent Integration Time [sec]
        line = fgetl(fileID); 
        SoOpParamsObj(iSoOp).t_coh = sscanf(line, "%f %*[\n]");
        % Noise Temperature Sky-view, Earth-view [K]
        line = fgetl(fileID); 
        temp = sscanf(line, "%f %f %*[\n]");
        SoOpParamsObj(iSoOp).N.sky = temp(1);
        SoOpParamsObj(iSoOp).N.earth = temp(2);

        fgetl(fileID);
        % Other Parameters
        % Minimum Local Surface Resolution [m]
        line = fgetl(fileID); 
        SoOpParamsObj(iSoOp).minRes = sscanf(line, "%f %*[\n]");
        % Number of Grid Points in theta, phi direction
        line = fgetl(fileID); 
        temp = sscanf(line, "%d %d %*[\n]");
        SoOpParamsObj(iSoOp).nTh = temp(1);
        SoOpParamsObj(iSoOp).nPh = temp(2);
    end

    fclose(fileID);
end
