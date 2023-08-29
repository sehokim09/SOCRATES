function createInpOrbScRx(orbPath, configData)
% function createInpOrbScRx
%
%   Creates Rx SC and Orb input files for 42 simulatior.
%
%   See also satObs, missionDesigner.
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
    orbFilePath = fullfile(cd, 'code', 'source', 'matlab', 'interface', 'ref', 'Orb_Rx.txt');
    scRxFilePath = fullfile(cd, 'code', 'source', 'matlab', 'interface', 'ref', 'SC_Rx.txt');
    parentPath = fileparts(orbPath);
    orbFolderPath = fullfile(parentPath, 'Orb');
    scFolderPath = fullfile(parentPath, 'SC');
    
    % Lines to edit
    linesOrb = [2, 13:22, 24, 25]; % For Orb file
    linesSc = 2; % For SC file
    
    % Receivers
    for iRx = 1 : configData.InorbitParamsObj.nRx
        % Orb
        filePath = fullfile(orbFolderPath, char(configData.SimParamsObj.orb(iRx).filename));
        system(['cp ', orbFilePath, ' ', filePath]);
        textsOrb(1) = sprintf("%s               !  Description\n", configData.RxScParamsObj(iRx).name);
        switch configData.RxScParamsObj(iRx).orbitType
            case 'Keplerian'
                orbType = 'KEP';
            case 'Cartesian'
                orbType = 'RV';
            case 'TLE'
                orbType = 'FILE';
        end
        textsOrb(2) = sprintf("%s                          !  Use Keplerian elements (KEP) or (RV) or FILE\n", orbType);
        textsOrb(3) = sprintf("%s                            !  Use Peri/Apoapsis (PA) or min alt/ecc (AE)\n", configData.RxScParamsObj(iRx).Keplerian.type);
        textsOrb(4) = sprintf("%f      %f              !  Periapsis & Apoapsis Altitude, km\n", configData.RxScParamsObj(iRx).Keplerian.perAlt, configData.RxScParamsObj(iRx).Keplerian.apoAlt);
        textsOrb(5) = sprintf("%f  %f                    !  Min Altitude (km), Eccentricity\n", configData.RxScParamsObj(iRx).Keplerian.minAlt, configData.RxScParamsObj(iRx).Keplerian.ecc);
        textsOrb(6) = sprintf("%f                          !  Inclination (deg)\n", configData.RxScParamsObj(iRx).Keplerian.inc);
        textsOrb(7) = sprintf("%f                         !  Right Ascension of Ascending Node (deg)\n", configData.RxScParamsObj(iRx).Keplerian.raan);
        textsOrb(8) = sprintf("%f                           !  Argument of Periapsis (deg)\n", configData.RxScParamsObj(iRx).Keplerian.aop);
        textsOrb(9) = sprintf("%f                           !  True Anomaly (deg)\n", configData.RxScParamsObj(iRx).Keplerian.ta);
        textsOrb(10) = sprintf("%f  %f  %f                 !  RV Initial Position (km)\n", configData.RxScParamsObj(iRx).Cartesian.X, configData.RxScParamsObj(iRx).Cartesian.Y, configData.RxScParamsObj(iRx).Cartesian.Z);
        textsOrb(11) = sprintf("%f  %f  %f                 !  RV Initial Velocity (km/sec)\n", configData.RxScParamsObj(iRx).Cartesian.VX, configData.RxScParamsObj(iRx).Cartesian.VY, configData.RxScParamsObj(iRx).Cartesian.VZ);
        textsOrb(12) = string(sprintf('\"%s\"                !  File name\n', configData.RxScParamsObj(iRx).filepath.filename));
        textsOrb(13) = string(sprintf('\"%s\"                   !  Label to find in TLE or TRV file\n', configData.RxScParamsObj(iRx).name));
        editTextFile(filePath,linesOrb,textsOrb,zeros(1,13));
        
        % SC
        filePath = fullfile(scFolderPath, char(configData.SimParamsObj.sc(iRx).filename));
        system(['cp ', scRxFilePath, ' ', filePath]);
        textsSc(1) = sprintf("%s           !  Description\n", configData.RxScParamsObj(iRx).name);
        editTextFile(filePath,linesSc,textsSc,0);
    end
end
