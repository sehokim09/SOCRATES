classdef SimParams < handle
% classdef SimParams
%
%   Stores simulation parameters.
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
        epoch % Sim Epoch in UTC (datetime)
        duration % Sim Duration (sec)
        stepSize % Sim Step Size (sec)
        outInterval % File Output Interval (sec)
        nOrb % Number of Reference Orbits
        % Reference Orbit
        orb = struct('exist', false, ... % Existence
                     'filename', ""); % Input File Name
        nSc % Number of Spacecraft
        % Spacecraft
        sc = struct('exist', false, ... % Existence
                    'orbID', 0, ... % Reference Orbit
                    'filename', ""); % Input File Name
    end
    
    methods

    end
end