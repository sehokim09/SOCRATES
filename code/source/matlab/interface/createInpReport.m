function createInpReport(projPath, ReportParamsObj)
% function createInpReport
%
%   Creates Inp_Report.txt.
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
    refFilePath = fullfile(cd, 'code', 'source', 'matlab', 'interface', 'ref', 'Inp_Report.txt');

    % Copy reference file
    filepath = fullfile(projPath, 'Inp_Report.txt');
    system(['cp ', refFilePath, ' ', filepath]);

    lines = [3:21, 23:36, 39, 41, 43:49, 51:53, 55, 56];

    texts(1) = sprintf("%s                         ! Julian Day\n", ReportParamsObj.flagStr(1));
    texts(2) = sprintf("%s                         ! Date (year, month, day, hour, min, sec)\n", ReportParamsObj.flagStr(2));
    texts(3) = sprintf("%s                         ! LLA of Receiver [deg][deg][m]\n", ReportParamsObj.flagStr(3));
    texts(4) = sprintf("%s                         ! Position of Receiver in ECEF (x,y,z) [m]\n", ReportParamsObj.flagStr(4));
    texts(5) = sprintf("%s                         ! (SPACE only) Velocity of Receiver in ECEF (x,y,z) [m/s]\n", ReportParamsObj.flagStr(5));
    texts(6) = sprintf("%s                         ! Transmitter ID number\n", ReportParamsObj.flagStr(6));
    texts(7) = sprintf("%s                         ! LLA of Transmitter [deg][deg][m]\n", ReportParamsObj.flagStr(7));
    texts(8) = sprintf("%s                         ! Position of Transmitter in ECEF (x,y,z) [m]\n", ReportParamsObj.flagStr(8));
    texts(9) = sprintf("%s                         ! Velocity of Transmitter in ECEF (x,y,z) [m/s]\n", ReportParamsObj.flagStr(9));
    texts(10) = sprintf("%s                         ! LLA of Specular Point [deg][deg][m]\n", ReportParamsObj.flagStr(10));
    texts(11) = sprintf("%s                         ! Position of Specular Point in ECEF (x,y,z) [m]\n", ReportParamsObj.flagStr(11));
    texts(12) = sprintf("%s                         ! Line of Sight of Direct signal (Azimuth, Elevation) [deg][deg]\n", ReportParamsObj.flagStr(12));
    texts(13) = sprintf("%s                         ! Line of Sight of Reflected signal (Azimuth, Elevation) [deg][deg]\n", ReportParamsObj.flagStr(13));
    texts(14) = sprintf("%s                         ! Delay [sec] and Doppler [Hz] (Delay_D, Doppler_D, Delay_R, Doppler_R)\n", ReportParamsObj.flagStr(14));
    texts(15) = sprintf("%s                         ! Antenna Gain of Direct signal [dB]\n", ReportParamsObj.flagStr(15));
    texts(16) = sprintf("%s                         ! Antenna Gain of Reflected signal [dB]\n", ReportParamsObj.flagStr(16));
    texts(17) = sprintf("%s                         ! First Fresnel Zone (a,b,azimuth) [m][m][deg]\n", ReportParamsObj.flagStr(17));
    texts(18) = sprintf("%s                         ! Incidence Angle [deg]\n", ReportParamsObj.flagStr(18));
    texts(19) = sprintf("%s                         ! Elevation Angle [deg]\n", ReportParamsObj.flagStr(19));
    texts(20) = sprintf("%s                         ! Roll, Pitch, Yaw in 1-2-3 Rotations from Local-Nadir to Specular Point [deg][deg][deg]\n", ReportParamsObj.flagStr(20));
    texts(21) = sprintf("%s                         ! Land Mask [1:Land, 0:Water]\n", ReportParamsObj.flagStr(21));
    texts(22) = sprintf("%s                         ! Volumetric Soil Moisture (N-Layer) [m^3/m^3]\n", ReportParamsObj.flagStr(22));
    texts(23) = sprintf("%s                         ! Soil Temperature [K]\n", ReportParamsObj.flagStr(23));
    texts(24) = sprintf("%s                         ! Freeze/Thaw State\n", ReportParamsObj.flagStr(24));
    texts(25) = sprintf("%s                         ! Soil Texture (Sand)(Clay)(Bulk density)\n", ReportParamsObj.flagStr(25));
    texts(26) = sprintf("%s                         ! Snow Cover (valid: 0-100) [%%]\n", ReportParamsObj.flagStr(26));
    texts(27) = sprintf("%s                         ! Reflection Coefficent Co-pol (XX/RX/RR) (True (real, imag), w/Error (real, imag))\n", ReportParamsObj.flagStr(27));
    texts(28) = sprintf("%s                         ! Reflection Coefficent X-pol (XX/RX/RR) (True (real, imag), w/Error (real, imag))\n", ReportParamsObj.flagStr(28));
    texts(29) = sprintf("%s                         ! Reflectivity Co-pol (XX/RX/RR) (True, w/Error)\n", ReportParamsObj.flagStr(29));
    texts(30) = sprintf("%s                         ! Reflectivity X-pol (YY/RY/RL) (True, w/Error)\n", ReportParamsObj.flagStr(30));
    texts(31) = sprintf("%s                         ! SNR of Direct signal (Co-pol, X-pol)\n", ReportParamsObj.flagStr(31));
    texts(32) = sprintf("%s                         ! SNR of Reflected signal (Co-pol, X-pol)\n", ReportParamsObj.flagStr(32));
    texts(33) = sprintf("%s                         ! MUOS Spot beam Number (Rx, Sp)\n", ReportParamsObj.flagStr(33));
    texts(34) = sprintf("%s                         ! Access Time to Ground Stations\n", ReportParamsObj.CS.flagStr(1));
    texts(35) = sprintf("%d   %d                        ! First Rx ID, Last Rx ID of Interest\n", ReportParamsObj.ADCS.RxRefID(1), ReportParamsObj.ADCS.RxRefID(2));
    texts(36) = sprintf("%s                         ! Julian Day\n", ReportParamsObj.ADCS.flagStr(1));
    texts(37) = sprintf("%s                         ! Position in ECI frame (x,y,z) [m]\n", ReportParamsObj.ADCS.flagStr(2));
    texts(38) = sprintf("%s                         ! Roll, Pitch, Yaw in LVLH frame (x,y,z) [deg]\n", ReportParamsObj.ADCS.flagStr(3));
    texts(39) = sprintf("%s                         ! Angular Velocity in Body frame (x,y,z) [deg/sec]\n", ReportParamsObj.ADCS.flagStr(4));
    texts(40) = sprintf("%s                         ! Angular Momentum in Body frame (x,y,z) [N-m-sec]\n", ReportParamsObj.ADCS.flagStr(5));
    texts(41) = sprintf("%s                         ! Magnetic Field in Body frame (x,y,z) [T]\n", ReportParamsObj.ADCS.flagStr(6));
    texts(42) = sprintf("%s                         ! Eclipse Flag (1: Eclipse, 0: Daylight)\n", ReportParamsObj.ADCS.flagStr(7));
    texts(43) = sprintf("%s                         ! Gyros (Meas Angular Velocity(x,y,z) [deg/s], True Rate/axis [deg/s], Meas Rate/axis [deg/s], Bias/axis [deg/s], Angle/axis[deg])\n", ReportParamsObj.ADCS.flagStr(8));
    texts(44) = sprintf("%s                         ! Magnetometers (Meas Magnetic Field(x,y,z), Magnetic Field Meas per axis) [T]\n", ReportParamsObj.ADCS.flagStr(9));
    texts(45) = sprintf("%s                         ! Accelerometers (True Acceleration per axis, Meas Acceleration per axis) [m/s^2]\n", ReportParamsObj.ADCS.flagStr(10));
    texts(46) = sprintf("%s                         ! Reaction Wheels (Torque Command (x,y,z) [N-m], Torque per whl [N-m], Angular Momentum per whl [N-m-sec])\n", ReportParamsObj.ADCS.flagStr(11));
    texts(47) = sprintf("%s                         ! Magnetorquers (Momentum Command (x,y,z) [A-m^2], Torque (x,y,z) per mtb [N-m], Magnetic Momentum per mtb [A-m^2])\n", ReportParamsObj.ADCS.flagStr(12));
    
    flagAdd = zeros(47,1);
    
    editTextFile(filepath,lines,texts,flagAdd);
end
