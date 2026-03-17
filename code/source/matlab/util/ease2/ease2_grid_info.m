function grid = ease2_grid_info(grid_name)
% function ease2_grid_info
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

    grid_info{1}.gpd = 'EASE2_N25KM';   grid_info{1}.map_scale_m = 25000.0;
    grid_info{1}.cols = 720;            grid_info{1}.rows = 720;
    
    grid_info{2}.gpd = 'EASE2_N12.5KM'; grid_info{2}.map_scale_m = 12500.0;
    grid_info{2}.cols = 1440;           grid_info{2}.rows = 1440;
    
    grid_info{3}.gpd = 'EASE2_N6.25KM'; grid_info{3}.map_scale_m = 6250.0;
    grid_info{3}.cols = 2880;           grid_info{3}.rows = 2880;
    
    grid_info{4}.gpd = 'EASE2_N36KM';   grid_info{4}.map_scale_m = 36000.0;
    grid_info{4}.cols = 500;            grid_info{4}.rows = 500;
    
    grid_info{5}.gpd = 'EASE2_N09KM';   grid_info{5}.map_scale_m = 9000.0;
    grid_info{5}.cols = 2000;           grid_info{5}.rows = 2000;
    
    grid_info{6}.gpd = 'EASE2_N03KM';   grid_info{6}.map_scale_m = 3000.0;
    grid_info{6}.cols = 6000;           grid_info{6}.rows = 6000;
    
    grid_info{7}.gpd = 'EASE2_N01KM';   grid_info{7}.map_scale_m = 1000.0;
    grid_info{7}.cols = 18000;          grid_info{7}.rows = 18000;
    
    grid_info{8}.gpd = 'EASE2_N100KM';  grid_info{8}.map_scale_m = 100000.0;
    grid_info{8}.cols = 180;            grid_info{8}.rows = 180;
    
    grid_info{9}.gpd = 'EASE2_S25KM';   grid_info{9}.map_scale_m = 25000.0;
    grid_info{9}.cols = 720;            grid_info{9}.rows = 720;
    
    grid_info{10}.gpd = 'EASE2_S12.5KM'; grid_info{10}.map_scale_m = 12500.0;
    grid_info{10}.cols = 1440;           grid_info{10}.rows = 1440;
    
    grid_info{11}.gpd = 'EASE2_S6.25KM'; grid_info{11}.map_scale_m = 6250.0;
    grid_info{11}.cols = 2880;           grid_info{11}.rows = 2880;
    
    grid_info{12}.gpd = 'EASE2_S36KM';   grid_info{12}.map_scale_m = 36000.0;
    grid_info{12}.cols = 500;            grid_info{12}.rows = 500;
    
    grid_info{13}.gpd = 'EASE2_S09KM';   grid_info{13}.map_scale_m = 9000.0;
    grid_info{13}.cols = 2000;           grid_info{13}.rows = 2000;
    
    grid_info{14}.gpd = 'EASE2_S03KM';   grid_info{14}.map_scale_m = 3000.0;
    grid_info{14}.cols = 6000;           grid_info{14}.rows = 6000;
    
    grid_info{15}.gpd = 'EASE2_S01KM';   grid_info{15}.map_scale_m = 1000.0;
    grid_info{15}.cols = 18000;          grid_info{15}.rows = 18000;
    
    grid_info{16}.gpd = 'EASE2_M25KM';   grid_info{16}.map_scale_m = 25025.2600081;
    grid_info{16}.cols = 1388;           grid_info{16}.rows = 584;
    
    grid_info{17}.gpd = 'EASE2_M12.5KM'; grid_info{17}.map_scale_m = 12512.63000405;
    grid_info{17}.cols = 2776;           grid_info{17}.rows = 1168;
    
    grid_info{18}.gpd = 'EASE2_M6.25KM'; grid_info{18}.map_scale_m = 6256.315002025;
    grid_info{18}.cols = 5552;           grid_info{18}.rows = 2336;
    
    grid_info{19}.gpd = 'EASE2_M36KM';   grid_info{19}.map_scale_m = 36032.220840584;
    grid_info{19}.cols = 964;            grid_info{19}.rows = 406;
    
    grid_info{20}.gpd = 'EASE2_M09KM';   grid_info{20}.map_scale_m = 9008.055210146;
    grid_info{20}.cols = 3856;           grid_info{20}.rows = 1624;
    
    grid_info{21}.gpd = 'EASE2_M03KM';   grid_info{21}.map_scale_m = 3002.6850700487;
    grid_info{21}.cols = 11568;          grid_info{21}.rows = 4872;
    
    grid_info{22}.gpd = 'EASE2_M01KM';   grid_info{22}.map_scale_m = 1000.89502334956;
    grid_info{22}.cols = 34704;          grid_info{22}.rows = 14616;
    
    idx = -1;
    for i = 1 : 22
        grid_info{i}.r0 = ( grid_info{i}.cols - 1 ) / 2.0;
        grid_info{i}.s0 = ( grid_info{i}.rows - 1 ) / 2.0;
        grid_info{i}.projection = grid_info{i}.gpd(7);
        if strcmpi(grid_name, grid_info{i}.gpd)
            idx = i;
            break
        end
    end
    
    if idx == -1
        error('Error. Unknown EASE2 name')
    end

    grid.projection = grid_info{idx}.projection;
    grid.map_scale_m = grid_info{idx}.map_scale_m;
    grid.cols = grid_info{idx}.cols;
    grid.rows = grid_info{idx}.rows;
    grid.r0 = grid_info{idx}.r0;
    grid.s0 = grid_info{idx}.s0;

end