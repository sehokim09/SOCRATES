function ScParamsObj = readInpOrb(filepath)
% function readInpOrb
%
%   Initializes ScParamsObj from Orb input files.
%
%   See also satObs, missionDesigner, and ScParams.
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

    ScParamsObj = ScParams;

    fileID = fopen(filepath, "r");

    fgetl(fileID);

    % Description
    line = fgetl(fileID); 
    ScParamsObj.name = sscanf(line, "%s %*[\n]");
    % Orbit Type (ZERO, FLIGHT, CENTRAL, THREE_BODY)
    fgetl(fileID); % Skip

    for i = 1 : 7
        fgetl(fileID);
    end
    % Orbit Center
    fgetl(fileID); % Skip
    % Secular Orbit Drift Due to J2
    fgetl(fileID); % Skip
    % Use Keplerian elements (KEP) or (RV) or FILE
    line = fgetl(fileID); 
    temp = sscanf(line, "%s %*[\n]");
    switch temp
        case 'KEP'
            ScParamsObj.orbitType = 'Keplerian';
        case 'RV'
            ScParamsObj.orbitType = 'Cartesian';
        case 'FILE'
            ScParamsObj.orbitType = 'TLE';
    end
    % Use Peri/Apoapsis (PA) or min alt/ecc (AE)
    line = fgetl(fileID); 
    ScParamsObj.Keplerian.type = sscanf(line, "%s %*[\n]");
    % Periapsis & Apoapsis Altitude, km
    line = fgetl(fileID); 
    temp = sscanf(line, "%f %f %*[\n]");
    ScParamsObj.Keplerian.perAlt = temp(1);
    ScParamsObj.Keplerian.apoAlt = temp(2);
    %  Min Altitude (km), Eccentricity
    line = fgetl(fileID); 
    temp = sscanf(line, "%f %f %*[\n]");
    ScParamsObj.Keplerian.minAlt = temp(1);
    ScParamsObj.Keplerian.ecc = temp(2);
    % Inclination (deg)
    line = fgetl(fileID); 
    ScParamsObj.Keplerian.inc = sscanf(line, "%f %*[\n]");
    % Right Ascension of Ascending Node (deg)
    line = fgetl(fileID); 
    ScParamsObj.Keplerian.raan = sscanf(line, "%f %*[\n]");
    % Argument of Periapsis (deg)
    line = fgetl(fileID); 
    ScParamsObj.Keplerian.aop = sscanf(line, "%f %*[\n]");
    % True Anomaly (deg)
    line = fgetl(fileID); 
    ScParamsObj.Keplerian.ta = sscanf(line, "%f %*[\n]");
    % RV Initial Position (km)
    line = fgetl(fileID); 
    temp = sscanf(line, "%f %f %f %*[\n]");
    ScParamsObj.Cartesian.X = temp(1);
    ScParamsObj.Cartesian.Y = temp(2);
    ScParamsObj.Cartesian.Z = temp(3);
    % RV Initial Velocity (km/sec)
    line = fgetl(fileID); 
    temp = sscanf(line, "%f %f %f %*[\n]");
    ScParamsObj.Cartesian.VX = temp(1);
    ScParamsObj.Cartesian.VY = temp(2);
    ScParamsObj.Cartesian.VZ = temp(3);
    % TLE, TRV, or SPLINE file format
    fgetl(fileID); % Skip. Must be 'TLE'
    % File name
    line = fgetl(fileID); 
    ScParamsObj.filepath.filename = sscanf(line, '\"%[^\"] %*[\n]');
    % Label to find in TLE or TRV file
    fgetl(fileID); % Skip. Must be same as name
    
    fclose(fileID);
end