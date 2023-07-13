classdef MainParams < handle
% classdef MainParams
%
%   Stores main simulation setup.
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
        simMode % Simulation Mode
        flagGrid = true; % Grid Flag
        nGrid = 1 % Number of Grid
        gridType = "EASE2"; % Grid Type
        grid = struct('ease2', struct('res', "9 km",... % EASE2 Grid Resolution
                                      'proj', "Global"),... % EASE2 Grid Projection
                      'user', struct('name',"New Grid", ...
                                     'pixelSize',9.008055210146,... % User-defined Grid Pixel Size
                                     'origin',[84.656418797143120, -179.9533195020746],... % User-defined Grid Origin @ Upper Left (Lat, Lon) in deg
                                     'nPixel',[1624, 3856])); % User-defined Grid Number of Pixels (Row, Column)
    end
    
    methods

    end
end