function mass = maxPayloadMass(alt, inc)
% function maxPayloadMass
%
%   Calculates maximum payload mass of a launch vehicle.
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

    %% Fitting
%     alt_ref = [400, 500, 600, 700];
%     inc_ref = [40, 60, 80, 100];
%     [grid_alt, grid_inc] = meshgrid(alt_ref, inc_ref);
% 
%     mass = [270, 264, 257, 250;
%             250, 244, 237, 230;
%             225, 219, 212, 205;
%             200, 194, 187, 180];
% 
%     [xData, yData, zData] = prepareSurfaceData( grid_alt, grid_inc, mass );
% 
%     % Set up fittype and options.
%     ft = fittype( 'poly11' );
% 
%     % Fit model to data.
%     fitresult = fit( [xData, yData], zData, ft );
%     
%     % Output
%     mass = fitresult(alt, inc);

    %% Model
    mass = 345.6 - 0.067*alt - 1.175*inc;
end

