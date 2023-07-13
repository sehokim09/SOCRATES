function MainParamsObj = readInpMain(projPath)
% function readInpMain
%
%   Initializes MainParamsObj from Inp_Main.txt.
%
%   See also satObs, missionDesigner, and MainParamsObj.
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

    MainParamsObj = MainParams;

    fileID = fopen(fullfile(projPath,"Inp_Main.txt"), "r");
    
    fgetl(fileID);
    % Simulation Mode
    line = fgetl(fileID); 
    MainParamsObj.simMode = sscanf(line, "%s %*[\n]");
    
    % Skip
    for i = 1 : 11
        fgetl(fileID);
    end

    % Number of Input Soil Layers
    line = fgetl(fileID);
    nLayer = sscanf(line, "%d %*[\n]");
        
    % Skip
    for i = 1 : 16
        fgetl(fileID);
    end

    for iLayer = 1 : nLayer
        fgetl(fileID);
        % Depth [cm]
        line = fgetl(fileID);
        sm.depth = sscanf(line, "%f %*[\n]");
        % VSM File Name [FIXED:txt, INORBIT:float32]
        line = fgetl(fileID);
        sm.filename = sscanf(line, "%s %*[\n]");
    end

    % Skip
    for i = 1 : 40
        fgetl(fileID);
    end

    % Use Grid?
    line = fgetl(fileID);
    temp = sscanf(line, "%s %*[\n]");
    if strcmp(temp,"TRUE")
        MainParamsObj.flagGrid = true;
    else
        MainParamsObj.flagGrid = false;
    end
    % Grid Type
    line = fgetl(fileID);
    temp = sscanf(line, "%s %*[\n]");
    switch temp
        case 'EASE2'
            MainParamsObj.gridType = "EASE2";
        case 'USER_DEFINED'
            MainParamsObj.gridType = "User-defined";
    end
    fgetl(fileID);
    % EASE2 grid resolution [km], projection (M,N,S)
    line = fgetl(fileID);
    temp = sscanf(line, "%f %s %*[\n]", [1 Inf]);
    switch temp(1)
        case 1
            MainParamsObj.grid.ease2.res = "1 km";
        case 3
            MainParamsObj.grid.ease2.res = "3 km";
        case 3.125
            MainParamsObj.grid.ease2.res = "3.125 km";
        case 6.25
            MainParamsObj.grid.ease2.res = "6.25 km";
        case 9
            MainParamsObj.grid.ease2.res = "9 km";
        case 12.5
            MainParamsObj.grid.ease2.res = "12.5 km";
        case 25
            MainParamsObj.grid.ease2.res = "25 km";
        case 36
            MainParamsObj.grid.ease2.res = "36 km";
    end
    switch char(temp(2:end))
        case 'M'
            MainParamsObj.grid.ease2.proj = "Global";
        case 'N'
            MainParamsObj.grid.ease2.proj = "Northern Hemisphere";
        case 'S'
            MainParamsObj.grid.ease2.proj = "Southern Hemisphere";
    end

    fgetl(fileID);
    % Number of Local grids
    line = fgetl(fileID);
    MainParamsObj.nGrid = sscanf(line, "%d %*[\n]");
    
    for iGrid = 1 : MainParamsObj.nGrid
        fgetl(fileID);
        MainParamsObj.grid.user(iGrid).name = string(sprintf("Grid%d", iGrid));
        % Pixel Size [km]
        line = fgetl(fileID);
        MainParamsObj.grid.user(iGrid).pixelSize = sscanf(line, "%f %*[\n]");
        % Origin @ Upper left (Latitude, Longitude) [deg]
        line = fgetl(fileID);
        MainParamsObj.grid.user(iGrid).origin = sscanf(line, "%f %f %*[\n]");
        % Number of Pixels (Row, Column)
        line = fgetl(fileID);
        MainParamsObj.grid.user(iGrid).nPixel = sscanf(line, "%d %d %*[\n]");
    end

    fclose(fileID);
end
