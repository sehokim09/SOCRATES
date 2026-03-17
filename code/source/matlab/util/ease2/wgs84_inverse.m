function [lat, lon] = wgs84_inverse(grid_name, r, s)
% function wgs84_inverse
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
      
    % Convert (col, row) to xy coordinates for this projection
    x = ( r - grid.r0 ) * grid.map_scale_m;
    y = ( grid.s0 - s ) * grid.map_scale_m;
    [lat, lon] = wgs84_inverse_xy(grid.projection, x, y);

end