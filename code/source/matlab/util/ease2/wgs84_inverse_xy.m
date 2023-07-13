function [lat, lon] = wgs84_inverse_xy(projection, x, y)
% function wgs84_inverse_xy
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

%   Source and credit: https://github.com/nsidc/easegrids-easeconv
%   Modified by Seho Kim (kim3002@purdue.edu), 08/2022

    proj = upper(projection);
    map = ease2_map_info(proj);
    e4 = map.eccentricity^4;
    e6 = map.eccentricity^6;
    
    % qp is the function q evaluated at phi = 90.0
    qp = (1 - map.e2) * ((1/(1 - map.e2)) - (1/(2*map.eccentricity))*log((1 - map.eccentricity)/(1 + map.eccentricity)));
    
    if proj == 'N'
        rho = sqrt( ( x .* x ) + ( y .* y ) );
        beta = asin( 1 - ( rho.^2 / ( map.equatorial_radius_m^2 * qp ) ) );
        lam = atan2( x, ( -1 * y ) );
    elseif proj == 'S' 
        rho = sqrt( ( x .* x ) + ( y .* y ) );
        beta = -1 * asin( 1 - ( rho.^2 / ( map.equatorial_radius_m^2 * qp ) ) );
        lam = atan2( x,  y );
    elseif proj == 'M'
        beta = asin( 2 * y * map.kz/( map.equatorial_radius_m * qp ) );
        lam = x / ( map.equatorial_radius_m * map.kz );
    else
        error('Error. Wrong projection.')
    end
    
    phi = beta + (((map.e2/3) + ((31/180)*e4) + ((517/5040)*e6)) * sin(2*beta)) + ((((23/360)*e4) + ((251/3780)*e6))*sin(4*beta)) + (((761/45360)*e6)*sin(6*beta));
    lat = rad2deg( phi );
    lon = easeconv_normalize_degrees( map.reference_longitude + ( rad2deg( lam ) ) );

end