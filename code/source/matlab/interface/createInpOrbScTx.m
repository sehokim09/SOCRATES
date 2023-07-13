function createInpOrbScTx(orbPath, configData)
% function createInpOrbScTx
%
%   Creates Tx SC and Orb input files for 42 simulatior.
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
    orbFilePath = fullfile(cd, 'code', 'source', 'matlab', 'interface', 'ref', 'Orb.txt');
    scTxFilePath = fullfile(cd, 'code', 'source', 'matlab', 'interface', 'ref', 'SC.txt');
    parentPath = fileparts(orbPath);
    orbFolderPath = fullfile(parentPath, 'Orb');
    scFolderPath = fullfile(parentPath, 'SC');
    
    % Lines to edit
    linesOrb = [2, 13:22, 24, 25]; % For Orb file
    linesSc = 2; % For SC file
    
    % Tx
    iOrb = configData.InorbitParamsObj.nRx + 1;
    for iSoOp = 1 : configData.InorbitParamsObj.nSoOp
        for iTx = 1 : length(configData.SoOpParamsObj(iSoOp).TxScParamsObj)
            filePath = fullfile(orbFolderPath, char(configData.SimParamsObj.orb(iOrb).filename));
            system(['cp ', orbFilePath, ' ', filePath]);
            textsOrb(1) = sprintf("%s               !  Description\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).name);
            switch configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).orbitType
                case 'Keplerian'
                    orbType = 'KEP';
                case 'Cartesian'
                    orbType = 'RV';
                case 'TLE'
                    orbType = 'FILE';
            end
            textsOrb(2) = sprintf("%s                          !  Use Keplerian elements (KEP) or (RV) or FILE\n", orbType);
            textsOrb(3) = sprintf("%s                            !  Use Peri/Apoapsis (PA) or min alt/ecc (AE)\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Keplerian.type);
            textsOrb(4) = sprintf("%f      %f              !  Periapsis & Apoapsis Altitude, km\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Keplerian.perAlt, configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Keplerian.apoAlt);
            textsOrb(5) = sprintf("%f  %f                    !  Min Altitude (km), Eccentricity\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Keplerian.minAlt, configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Keplerian.ecc);
            textsOrb(6) = sprintf("%f                          !  Inclination (deg)\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Keplerian.inc);
            textsOrb(7) = sprintf("%f                         !  Right Ascension of Ascending Node (deg)\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Keplerian.raan);
            textsOrb(8) = sprintf("%f                           !  Argument of Periapsis (deg)\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Keplerian.aop);
            textsOrb(9) = sprintf("%f                           !  True Anomaly (deg)\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Keplerian.ta);
            textsOrb(10) = sprintf("%f  %f  %f                 !  RV Initial Position (km)\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Cartesian.X, configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Cartesian.Y, configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Cartesian.Z);
            textsOrb(11) = sprintf("%f  %f  %f                 !  RV Initial Velocity (km/sec)\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Cartesian.VX, configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Cartesian.VY, configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).Cartesian.VZ);
            textsOrb(12) = string(sprintf('\"%s\"                !  File name\n', configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).filepath.filename));
            textsOrb(13) = string(sprintf('\"%s\"                   !  Label to find in TLE or TRV file\n', configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).name));
            editTextFile(filePath,linesOrb,textsOrb,zeros(1,13));
            
            % SC
            filePath = fullfile(scFolderPath, char(configData.SimParamsObj.sc(iOrb).filename));
            system(['cp ', scTxFilePath, ' ', filePath]);
            textsSc(1) = sprintf("%s           !  Description\n", configData.SoOpParamsObj(iSoOp).TxScParamsObj(iTx).name);
            editTextFile(filePath,linesSc,textsSc,0);

            iOrb = iOrb + 1;
        end
    end
end
