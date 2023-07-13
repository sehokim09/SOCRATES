function [lat, lon, r, s] = ease2grid(grid_name)
% function ease2grid
%
%   Compute latitude and longtidue coordinates of EASE 2.0 grid.
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

    grid = ease2_grid_info(grid_name);
    [lat, ~] = wgs84_inverse(grid_name, 1, 0:(grid.rows-1));
    [~, lon] = wgs84_inverse(grid_name, 0:(grid.cols-1), 1);
    
    [r, ~] = wgs84_convert(grid_name, lat(1), lon);
    [~, s] = wgs84_convert(grid_name, lat, lon(1));
    r = round(r);
    s = round(s);
end