function [r, s] = wgs84_convert(grid_name, lat, lon)
% function wgs84_convert
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

%   Source and credit: https://github.com/nsidc/easegrids-easeconv
%   Modified by Seho Kim (kim3002@purdue.edu), 08/2022

    grid = ease2_grid_info(grid_name);
    
    [x, y] = wgs84_convert_xy(grid.projection, lat, lon);
    
    % Convert xy coordinates to row/col for this particular grid
    r = grid.r0 + ( x / grid.map_scale_m );
    s = grid.s0 - ( y / grid.map_scale_m );
    
    % Check for actual grid boundaries
    if any(r < -0.5) || any(r >= (grid.cols - 0.5)) || any(s < -0.5) || any(s >= (grid.rows - 0.5)) 
        error('Error. Point not on grid.')
    end
end