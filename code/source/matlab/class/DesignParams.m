classdef DesignParams < handle
% classdef DesignParams
%
%   Stores inputs for mission design.
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
        consType % Constellation Type
        % Walker Design Variables
        walker = struct('n_nRx', 0, ... % Number of # Receiver Variable
                        'nRx', 0, ... % # Receiver
                        'nAlt', 0, ... % Number of Altitude Variable
                        'alt', 0.0, ... % Altitude [km]
                        'nInc', 0, ... % Number of Inclination Variable
                        'inc', "SSO", ... % Inclination [deg]
                        'n_nPlane', 0, ... % Number of # Orbital Plane Variable
                        'nPlane', 0); % # Orbital Plane
        % Hybrid Design Variables
        hybrid = struct('name', 'Hybrid-1', ... % Constellation Name
                        'nPlane', 0, ... % # Orbital Plane
                        'nRx', 0, ... % # Receiver
                        'alt', 0.0, ... % Altitude [km]
                        'inc', "SSO"); % Inclination [deg]
        flagTxPropagation % Flag for Tx Propagation
        window % Observation Time Window [hour]
        minRCG_R % Minimum Reflected RCG for GPS [1E-27*m^-4]
        minRCG_D % Minimum Direct RCG [dB]
        maxRCGdiff % Maximum RCG Difference [dB]
        gridDivisor % Longitudinal Grid Divisor
        largeFilePath % Large File Storage Root Directory
        flagReport % Individual Receiver Data (per Inp_Report.txt)
        nTxComb % Number of Transmitter Combinations
        nFreqComb % Number of Frequency Combinations
        TxComb = struct('name', 'NewTxComb', ... % Combination Name
                        'nTx', 0, ... % Number of Included Tx Systems
                        'TxName', "", ... % Included Tx Names
                        'gainRx_dB', 0, ... % Rx Antenna Gain 
                        'polRx', 'RHCP', ... % Rx Polarization
                        'antPatTypeRx', 'Ideal', ... % Rx antenna pattern type
                        'antPatGG', struct('HPBW_deg', 0, ... % Beamwidth (degrees)
                                           'SLL_dB', 0, ... % Sidelobe Level (dB)
                                           'XPL_dB', 0, ... % Cross-pol level (dB)
                                           'res_deg', 0), ... % Resoultion (deg)
                        'antPatFilepath', struct('Co', [], ... % User-defined Co-pol antenna pattern file path
                                                 'X', [])); % User-defined X-pol antenna pattern file path
        freqComb = struct('name', 'NewFreqComb', ... % Combination name
                          'ID', 0, ... % Identifier
                          'nTxComb', 0, ... % Number of Included Tx combinations
                          'TxCombName', [], ... % Included Tx Combo names
                          'TxCombID', []); ... % Included Tx Combo IDs
        cost = struct('weight', 0.5, ...
                      'type', 'Total', ...
                      'domain', [0 100], ...
                      'baseline', 50, ...
                      'lower', 10, ...
                      'upper', 90);
        perform = struct('weight', 0.5, ...
                         'SSC', struct('weight', 0.2, ...
                                       'score', struct('weight', 1, ...
                                                       'domain', [0 100], ...
                                                       'baseline', 50, ...
                                                       'lower', 10, ...
                                                       'upper', 90)), ...
                         'SRT', struct('weight', 0.2, ...
                                       'type', '1T global', ...
                                       'score', struct('weight', 1, ...
                                                       'domain', [0 14], ...
                                                       'baseline', 7, ...
                                                       'lower', 1, ...
                                                       'upper', 13)), ...
                         'MSC', struct('weight', 0.3, ...
                                       'type', '1T visit', ...
                                       'score', struct('weight', 1, ...
                                                       'domain', [0 100], ...
                                                       'baseline', 50, ...
                                                       'lower', 10, ...
                                                       'upper', 90)), ...
                         'MRT', struct('weight', 0.3, ...
                                       'type', '1T global', ...
                                       'score', struct('weight', 1, ...
                                                       'domain', [0 14], ...
                                                       'baseline', 4, ...
                                                       'lower', 1, ...
                                                       'upper', 7)));
    end
    
    methods

    end
end