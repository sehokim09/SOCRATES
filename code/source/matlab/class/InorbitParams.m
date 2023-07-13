classdef InorbitParams < handle
% classdef InorbitParams
%
%   Stores SoOp system parameters.
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
        obsBase % Space-based or Tower-based observation
        nRx % Number of Receivers, NRx
        nSoOp % Number of Transmitter systems, NSoOp
        orbModel % Orbit Propagation Model (IMPORT, 42, 42_DECAY)
        % Import Orbits
        orbImport = struct('nSc', 0,... % Number of Spacecrafts
                           'nLine', 0,... % Number of Lines in Files
                           'filename', ""); % SC File Name
        % 42 Orbits
        RxName % Receiver Name
        RxRefOrb % Receiver First RefOrb, Last RefOrb in Inp_Sim.txt, 1-by-2 matrix
        TwrName % Tower-based Receiver Name
        TwrLLA % Tower Lat[deg], Lng[deg], Height[m] 
        TxRefOrb % Transmitter First RefOrb, Last RefOrb in Inp_Sim.txt, nSoOp-by-2 matrix
    end
    
    methods

    end
end