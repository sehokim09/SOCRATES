function map = ease2_map_info(projection)
% function ease2_map_info
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
    map.equatorial_radius_m = 6378137.0; % WGS84
    map.eccentricity = 0.081819190843; % WGS84
    map.e2 = map.eccentricity^2;
    if projection == 'N'
        map.reference_latitude = 90;
        map.reference_longitude = 0;
    elseif projection == 'S'
        map.reference_latitude = -90;
        map.reference_longitude = 0;
    elseif projection == 'M'
        map.reference_latitude = 0;
        map.reference_longitude = 0;
        map.second_reference_latitude = 30;
        map.sin_phi1 = sind(map.second_reference_latitude);
        map.cos_phi1 = cosd(map.second_reference_latitude);
        map.kz = map.cos_phi1 / sqrt(1 - map.e2 * map.sin_phi1^2);
    else
        error('Error. Wrong projection name.')
    end
end