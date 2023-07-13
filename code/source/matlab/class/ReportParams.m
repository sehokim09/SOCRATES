classdef ReportParams < handle
% classdef ReportParams
%
%   Stores output flag setup.
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
%   along with this program.  If not, see <https://www.gnu.org/licenses/>.
%
%   Version 1.0

    properties
        flagStr = "FALSE"; % Existence flag in string format
        flagInt = false; % Existence flag in integer
        tag = ["Julian Day", "Date", ...
                "Rx LLA", "Rx Pos ECEF", "Rx Vel ECEF", ...
                "Tx ID", "Tx LLA", "Tx Pos ECEF", "Tx Vel ECEF", ...
                "Sp LLA", "Sp Pos ECEF", "LoS Direct", "LoS Reflect", ...
                "Delay Doppler", "Ant Gain Direct", "Ant Gain Reflect", ...
                "FFZ", "Incidence Angle", "Elevation", "RPY to Sp", ...
                "Land mask", "VSM", "Soil Temperature", "Freeze Thaw", ...
                "Soil Texture", "Snow Cover", "Refl Coeff Co", "Refl Coeff X", ...
                "Refl Co", "Refl X", "SNR Direct", "SNR Reflect", "MUOS Spotbeam"];
        CS = struct('flagStr', "FALSE", ...
                    'flagInt', false, ...
                    'tag', "Access Time");
        ADCS = struct('RxRefID', [0, 0], ...
                      'flagStr', "FALSE",...
                      'flagInt', false, ...
                      'tag', ["Julian Day", "Pos ECI", "RPY", "Ang Vel", "Ang Mom", "Mag Field", "Eclipse Flag",...
                      "Gyros", "Magnetometers", "Accelerometers", "Reaction Wheels", "Magnetorquers"]);
    end
    
    methods

    end
end