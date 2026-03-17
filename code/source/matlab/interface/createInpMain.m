function createInpMain(projPath, MainParamsObj)
% function createInpMain
%
%   Creates Inp_Main.txt.
%
%   See also satObs, missionDesigner, MainParams.
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

    % Set paths
    refFilePath = fullfile(cd, 'code', 'source', 'matlab', 'interface', 'ref', 'Inp_Main.txt');

    % Copy reference file
    filepath = fullfile(projPath, 'Inp_Main.txt');
    system(['cp ', refFilePath, ' ', filepath]);

    lines = [2, 77, 78, 80, 82];

    texts(1) = sprintf("%s                             ! Simulation Mode (FIXED, INORBIT, DESIGN, RETRIEVAL)\n", MainParamsObj.simMode);

    if MainParamsObj.flagGrid
        flagGrid = 'TRUE';
    else
        flagGrid = 'FALSE';
    end
    texts(2) = sprintf("%s                               ! Use Grid? (Generate outputs only within grids)\n", flagGrid);
    switch MainParamsObj.gridType
        case 'EASE2'
            gridType = "EASE2";
        case 'User-defined'
            gridType = "USER_DEFINED";
    end
    texts(3) = sprintf("%s                              ! Grid Type (EASE2, USER_DEFINED)\n", gridType);

    switch MainParamsObj.grid.ease2.res
        case "1 km"
            res = 1;
        case "3 km"
            res = 3;
        case "3.125 km"
            res = 3.125;
        case "6.25 km"
            res = 6.25;
        case "9 km"
            res = 9;
        case "12.5 km"
            res = 12.5;
        case "25 km"
            res = 25;
        case "36 km"
            res = 36;
    end
    switch MainParamsObj.grid.ease2.proj
        case "Global"
            proj = 'M';
        case "Northern Hemisphere"
            proj = 'N';
        case "Southern Hemisphere"
            proj = 'S';
    end
    texts(4) = sprintf("%f    %s                             ! EASE2 grid resolution [km], projection (M,N,S)\n", res, proj);
    texts(5) = sprintf("%d                                  ! Number of Local grids\n", MainParamsObj.nGrid);
    flagAdd = [0, 0, 0, 0, 0];
    
    idxLastLine = 5;
    for iGrid = 1 : MainParamsObj.nGrid
        texts(idxLastLine+1) = sprintf("--------------------------- Grid %d -----------------------------------\n", iGrid);
        texts(idxLastLine+2) = sprintf("%f                     ! Pixel Size [km]\n", MainParamsObj.grid.user(iGrid).pixelSize);
        texts(idxLastLine+3) = sprintf("%f   %f    ! Origin @ Upper left (Latitude, Longitude) [deg]\n", MainParamsObj.grid.user(iGrid).origin(1), MainParamsObj.grid.user(iGrid).origin(2));
        texts(idxLastLine+4) = sprintf("%d     %d                      ! Number of Pixels (Row, Column)\n", MainParamsObj.grid.user(iGrid).nPixel(1), MainParamsObj.grid.user(iGrid).nPixel(2));
        lines(idxLastLine+1 : idxLastLine+4) = lines(idxLastLine) + (1:4);
        flagAdd(idxLastLine+1 : idxLastLine+4) = 1;
        idxLastLine = idxLastLine + 4;
    end
    
    editTextFile(filepath,lines,texts,flagAdd);
end
