function ReportParamsObj = readInpReport(projPath)
% function readInpReport
%
%   Initializes ReportParamsObj from Inp_Report.txt.
%
%   See also satObs, missionDesigner, and ReportParams.
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

    ReportParamsObj = ReportParams;

    fileID = fopen(fullfile(projPath,"Inp_Report.txt"), "r");

    fgetl(fileID);
    fgetl(fileID);

    % SPACE & TOWER
    
    for i = 1 : 19
        line = fgetl(fileID);
        ReportParamsObj.flagStr(i) = string(sscanf(line, "%s %*[\n]"));
    end
    fgetl(fileID);
    for i = 20 : 33
        line = fgetl(fileID);
        ReportParamsObj.flagStr(i) = string(sscanf(line, "%s %*[\n]"));
    end  
    fgetl(fileID);
    fgetl(fileID);

    % Comm 
    % Access Time to Ground Stations
    line = fgetl(fileID);
    ReportParamsObj.CS.flagStr(1) = string(sscanf(line, "%s %*[\n]"));
    fgetl(fileID);

    % ADCS
    % First Rx ID, Last Rx ID of Interest
    line = fgetl(fileID);
    ReportParamsObj.ADCS.RxRefID = sscanf(line, "%d %d %*[\n]");
    fgetl(fileID);
    for i = 1 : 7
        line = fgetl(fileID);
        ReportParamsObj.ADCS.flagStr(i) = string(sscanf(line, "%s %*[\n]"));
    end
    fgetl(fileID);
    for i = 8 : 10
        line = fgetl(fileID);
        ReportParamsObj.ADCS.flagStr(i) = string(sscanf(line, "%s %*[\n]"));
    end
    fgetl(fileID);
    for i = 11 : 12
        line = fgetl(fileID);
        ReportParamsObj.ADCS.flagStr(i) = string(sscanf(line, "%s %*[\n]"));
    end
    
    ReportParamsObj.flagInt = strcmp(ReportParamsObj.flagStr, "TRUE");
    ReportParamsObj.CS.flagInt = strcmp(ReportParamsObj.CS.flagStr, "TRUE");
    ReportParamsObj.ADCS.flagInt = strcmp(ReportParamsObj.ADCS.flagStr, "TRUE");
    
    return
    % Julian Day
    line = fgetl(fileID);
    ReportParamsObj.jd = sscanf(line, "%s %*[\n]");
    % Date (year, month, day, hour, min, sec)
    line = fgetl(fileID);
    ReportParamsObj.date = sscanf(line, "%s %*[\n]");
    % LLA of Receiver [deg][deg][m]
    line = fgetl(fileID);
    ReportParamsObj.RxLLA = sscanf(line, "%s %*[\n]");
    % Position of Receiver in ECEF (x,y,z) [m]
    line = fgetl(fileID);
    ReportParamsObj.RxPosECEF = sscanf(line, "%s %*[\n]");
    % (SPACE only) Velocity of Receiver in ECEF (x,y,z) [m/s]
    line = fgetl(fileID);
    ReportParamsObj.RxVelECEF = sscanf(line, "%s %*[\n]");
    % Transmitter ID number
    line = fgetl(fileID);
    ReportParamsObj.TxID = sscanf(line, "%s %*[\n]");
    % LLA of Transmitter [deg][deg][m]
    line = fgetl(fileID);
    ReportParamsObj.TxLLA = sscanf(line, "%s %*[\n]");
    % Position of Transmitter in ECEF (x,y,z) [m]
    line = fgetl(fileID);
    ReportParamsObj.TxPosECEF = sscanf(line, "%s %*[\n]");
    % Velocity of Transmitter in ECEF (x,y,z) [m/s]
    line = fgetl(fileID);
    ReportParamsObj.TxVelECEF = sscanf(line, "%s %*[\n]");
    % LLA of Specular Point [deg][deg][m]
    line = fgetl(fileID);
    ReportParamsObj.SpLLA = sscanf(line, "%s %*[\n]");
    % Position of Specular Point in ECEF (x,y,z) [m]
    line = fgetl(fileID);
    ReportParamsObj.SpPosECEF = sscanf(line, "%s %*[\n]");
    % Line of Sight of Direct signal (Azimuth, Elevation) [deg][deg]
    line = fgetl(fileID);
    ReportParamsObj.LoS_D = sscanf(line, "%s %*[\n]");
    % Line of Sight of Reflected signal (Azimuth, Elevation) [deg][deg]
    line = fgetl(fileID);
    ReportParamsObj.LoS_R = sscanf(line, "%s %*[\n]");
    % Delay [sec] and Doppler [Hz] (Delay_D, Doppler_D, Delay_R, Doppler_R)
    line = fgetl(fileID);
    ReportParamsObj.delayDoppler = sscanf(line, "%s %*[\n]");
    % Antenna Gain of Direct signal [dB]
    line = fgetl(fileID);
    ReportParamsObj.antGainD = sscanf(line, "%s %*[\n]");
    % Antenna Gain of Reflected signal [dB]
    line = fgetl(fileID);
    ReportParamsObj.antGainR = sscanf(line, "%s %*[\n]");
    % First Fresnel Zone (a,b,azimuth) [m][m][deg]
    line = fgetl(fileID);
    ReportParamsObj.FFZ = sscanf(line, "%s %*[\n]");
    % Incidence Angle [deg]
    line = fgetl(fileID);
    ReportParamsObj.incAng = sscanf(line, "%s %*[\n]");
    % Elevation Angle [deg]
    line = fgetl(fileID);
    ReportParamsObj.elev = sscanf(line, "%s %*[\n]");
    fgetl(fileID);
    
    % SPACE only
    
    % Roll, Pitch, Yaw in 1-2-3 Rotations from Local-Nadir to Specular Point [deg][deg][deg]
    line = fgetl(fileID);
    ReportParamsObj.RPY = sscanf(line, "%s %*[\n]");
    % Land Mask [1:Land, 0:Water]
    line = fgetl(fileID);
    ReportParamsObj.landmask = sscanf(line, "%s %*[\n]");
    % Volumetric Soil Moisture (N-Layer) [m^3/m^3]
    line = fgetl(fileID);
    ReportParamsObj.VSM = sscanf(line, "%s %*[\n]");
    % Soil Temperature [K]
    line = fgetl(fileID);
    ReportParamsObj.soilTemp = sscanf(line, "%s %*[\n]");
    % Freeze/Thaw State
    line = fgetl(fileID);
    ReportParamsObj.freezeThaw = sscanf(line, "%s %*[\n]");
    % Soil Texture (Sand)(Clay)(Bulk density)
    line = fgetl(fileID);
    ReportParamsObj.soilTexture = sscanf(line, "%s %*[\n]");
    % Snow Cover (valid: 0-100) [%]
    line = fgetl(fileID);
    ReportParamsObj.snowCover = sscanf(line, "%s %*[\n]");
    % Reflection Coefficent Co-pol (XX/RX/RR) (True (real, imag), w/Error (real, imag))
    line = fgetl(fileID);
    ReportParamsObj.reflCoeffCo = sscanf(line, "%s %*[\n]");
    % Reflection Coefficent X-pol (YY/RY/RL) (True (real, imag), w/Error (real, imag))
    line = fgetl(fileID);
    ReportParamsObj.reflCoeffX = sscanf(line, "%s %*[\n]");
    % Reflectivity Co-pol (XX/RX/RR) (True, w/Error)
    line = fgetl(fileID);
    ReportParamsObj.reflCo = sscanf(line, "%s %*[\n]");
    % Reflectivity X-pol (YY/RY/RL) (True, w/Error)
    line = fgetl(fileID);
    ReportParamsObj.reflX = sscanf(line, "%s %*[\n]");
    % SNR of Direct signal (Co-pol, X-pol)
    line = fgetl(fileID);
    ReportParamsObj.SNR_D = sscanf(line, "%s %*[\n]");
    % SNR of Reflected signal (Co-pol, X-pol)
    line = fgetl(fileID);
    ReportParamsObj.SNR_R = sscanf(line, "%s %*[\n]");
    % MUOS Spot beam Number (Rx, Sp)
    line = fgetl(fileID);
    ReportParamsObj.spotbeam = sscanf(line, "%s %*[\n]");
    fgetl(fileID);
    fgetl(fileID);

    % Subsystems
    
    % Comm 
    % Access Time to Ground Stations
    line = fgetl(fileID);
    ReportParamsObj.accessTime = sscanf(line, "%s %*[\n]");
    fgetl(fileID);

    % ADCS
    % First Rx ID, Last Rx ID of Interest
    line = fgetl(fileID);
    ReportParamsObj.ADCS.RxRefID = sscanf(line, "%d %d %*[\n]");
    fgetl(fileID);
    % Truth File 
    % Julian Day
    line = fgetl(fileID);
    ReportParamsObj.ADCS.jd = sscanf(line, "%s %*[\n]");
    % Position in ECI frame (x,y,z) [m]
    line = fgetl(fileID);
    ReportParamsObj.ADCS.posECI = sscanf(line, "%s %*[\n]");
    % Roll, Pitch, Yaw in LVLH frame (x,y,z) [deg]
    line = fgetl(fileID);
    ReportParamsObj.ADCS.RPY = sscanf(line, "%s %*[\n]");
    % Angular Velocity in Body frame (x,y,z) [deg/sec]
    line = fgetl(fileID);
    ReportParamsObj.ADCS.angVel = sscanf(line, "%s %*[\n]");
    % Angular Momentum in Body frame (x,y,z) [N-m-sec]
    line = fgetl(fileID);
    ReportParamsObj.ADCS.angMom = sscanf(line, "%s %*[\n]");
    % Magnetic Field in Body frame (x,y,z) [T]
    line = fgetl(fileID);
    ReportParamsObj.ADCS.magField = sscanf(line, "%s %*[\n]");
    % Eclipse Flag (1: Eclipse, 0: Daylight)
    line = fgetl(fileID);
    ReportParamsObj.ADCS.eclipse = sscanf(line, "%s %*[\n]");
    fgetl(fileID);

    % Sensors
    % Gyros (Meas Angular Velocity(x,y,z) [deg/s], True Rate/axis [deg/s], Meas Rate/axis [deg/s], Bias/axis [deg/s], Angle/axis[deg]) 
    line = fgetl(fileID);
    ReportParamsObj.ADCS.gyro = sscanf(line, "%s %*[\n]");
    % Magnetometers (Meas Magnetic Field(x,y,z), Magnetic Field Meas per axis) [T] 
    line = fgetl(fileID);
    ReportParamsObj.ADCS.magmtr = sscanf(line, "%s %*[\n]");
    % Accelerometers (True Acceleration per axis, Meas Acceleration per axis) [m/s^2]
    line = fgetl(fileID);
    ReportParamsObj.ADCS.accmtr = sscanf(line, "%s %*[\n]");
    fgetl(fileID);

    % Actuators
    % Reaction Wheels (Torque Command (x,y,z) [N-m], Torque per whl [N-m], Angular Momentum per whl [N-m-sec])
    line = fgetl(fileID);
    ReportParamsObj.ADCS.whl = sscanf(line, "%s %*[\n]");
    % Magnetorquers (Momentum Command (x,y,z) [A-m^2], Torque (x,y,z) per mtb [N-m], Magnetic Momentum per mtb [A-m^2])
    line = fgetl(fileID);
    ReportParamsObj.ADCS.mtq = sscanf(line, "%s %*[\n]");

    fclose(fileID);
end
