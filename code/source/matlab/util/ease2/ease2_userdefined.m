function grid = ease2_userdefined(grid, lat1, lon1)
% function ease2_userdefined
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

    map = ease2_map_info('M');
    e4 = map.eccentricity^4;
    e6 = map.eccentricity^6;

    dlon = grid.lon0 - map.reference_longitude;    
    dlon = easeconv_normalize_degrees( dlon );

    sin_phi = sind(grid.lat0);

	% Global Cylindrical EASE-Grid 2.0 Inverse Formulae
    q = (1.0 - map.e2) * ((sin_phi/(1.0 - map.e2*sin_phi*sin_phi)) - (1.0/(2.0*map.eccentricity))*log((1.0 - map.eccentricity*sin_phi)/(1.0 + map.eccentricity*sin_phi)));   
    % qp is the function q evaluated at phi = 90.0
    qp = (1 - map.e2) * ((1/(1 - map.e2)) - (1/(2*map.eccentricity))*log((1 - map.eccentricity)/(1 + map.eccentricity)));
    
    grid.x0 = map.equatorial_radius_m * map.kz * deg2rad(dlon);
    grid.y0 = (map.equatorial_radius_m*q) / (2.0*map.kz);

    % If lat1 and lon1 are given, find nRow and nCol
    if nargin > 1
        grid.nCol = ceil(( map.equatorial_radius_m * map.kz * deg2rad(lon1 - map.reference_longitude) - grid.x0 ) / grid.map_scale_m + 1);
        nRow = 0;
        while true
            nRow = nRow + 1;
            y = grid.y0 - grid.map_scale_m*(nRow-1);
	        beta = asin( 2 * y * map.kz/( map.equatorial_radius_m * qp ) );
            phi = beta + (((map.e2/3) + ((31/180)*e4) + ((517/5040)*e6)) * sin(2*beta)) + ((((23/360)*e4) + ((251/3780)*e6))*sin(4*beta)) + (((761/45360)*e6)*sin(6*beta));
            phi_deg = rad2deg(phi);
            if phi_deg<lat1
                grid.nRow = nRow;
                break;
            end
        end
    end
    % Calculate lat/lon edges
    x = grid.x0 + grid.map_scale_m*[0, grid.nCol-1];
    lam = x / ( map.equatorial_radius_m * map.kz );
    grid.lon = easeconv_normalize_degrees( map.reference_longitude + rad2deg(lam) );

	y = grid.y0 - grid.map_scale_m*[0, grid.nRow-1];
    beta = asin( 2 * y * map.kz/( map.equatorial_radius_m * qp ) );
    phi = beta + (((map.e2/3) + ((31/180)*e4) + ((517/5040)*e6)) * sin(2*beta)) + ((((23/360)*e4) + ((251/3780)*e6))*sin(4*beta)) + (((761/45360)*e6)*sin(6*beta));
    grid.lat = rad2deg(phi);
end
