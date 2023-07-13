classdef ScParams < handle
% classdef ScParams
%
%   Stores spacecraft parameters.
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

    properties
        name
        orbitType = 'Keplerian';
        Keplerian = struct('type', 'AE', ...
                           'perAlt', 0, ...
                           'apoAlt', 0, ...
                           'minAlt', 0, ...
                           'ecc', 0, ...
                           'inc', 0, ...
                           'raan', 0, ...
                           'aop', 0, ...
                           'ta', 0);
        Cartesian = struct('X', 0, ...
                           'Y', 0, ...
                           'Z', 0, ...
                           'VX', 0, ...
                           'VY', 0, ...
                           'VZ', 0);
        filepath = struct('filename', '', ...
                          'TLE', '', ...
                          'import', ''); 
    end

    methods

    end
end