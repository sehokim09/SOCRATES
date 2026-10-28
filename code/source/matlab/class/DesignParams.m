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
        gridDivisor % Longitudinal Grid Divisor
        largeFilePath % Large File Storage Root Directory
        flagReport % Individual Receiver Data (per Inp_Report.txt)
        nTxComb % Number of Transmitter Combinations
        nFreqComb % Number of Frequency Combinations
        TxComb = struct('name', 'NewTxComb', ... % Combination Name
                        'nTx', 0, ... % Number of Included Tx Systems
                        'TxName', ""); % Included Tx Names
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
        % Receiver Design (see designReceiver)
        receiver = struct('inputs', struct( ...
                              'RX_POL', "HVC", ... % Receive Polarization (HVC; SOCRATES reads ports X, Y and L)
                              'RMSE_target', 0.06, ... % Soil-moisture RMSE Target [m^3/m^3]
                              'RMSE_target_L', NaN, ... % RMSE Target of the LHCP Product (NaN = 0.10)
                              'resMax_km', 25, ... % Ground Resolution Requirement [km]
                              'equalRes', true, ... % One Footprint for All Bands
                              'roundImpl', true, ... % Round T_I and Dwell to Implementable Values
                              'T_I_step', 1e-4, ... % T_I Granularity [s]
                              'dwellEpoch', 0, ... % Dwell Output Epoch [s] (0 = none)
                              'FIX', [NaN NaN NaN NaN 1.0 0.8 0.5 500], ... % Pinned [D_gp h T_dwell_VU T_dwell_L NF_VU NF_L L_fe alt_Rx] (NaN = free)
                              'nadirFace', "large", ... % Nadir Face of the 6U (large, end)
                              'gnssSignals', {{"GPS_L1CA"}}, ... % GNSS Signal Sets
                              'gnssEirpMode', "margin", ... % GNSS EIRP Mode (margin, min, nominal)
                              'gnssEirpMargin_dB', 1.0, ... % GNSS EIRP Margin [dB]
                              'geomFiles', "", ... % Geometry Sample Files ("" = Rx1 of each Walker inclination)
                              'geomDir', ""), ... % Geometry Sample Folder ("" = Large File Storage Root Directory)
                          'runTag', "", ... % Run Tag of the Performance Files
                          'payload', [], ... % Solved Payload (optimizeReceiver)
                          'antenna', [], ... % Antenna Summary (generateReceiverPatterns)
                          'model', []); % SAI Observation Model (evaluateReceiver)
    end
    
    methods (Static)
        function obj = loadobj(s)
            % Projects saved before the RCG and receiver-antenna inputs were
            % removed carry properties the class no longer has, so MATLAB
            % passes a struct: copy the properties that still exist. Their
            % TxComb entries also carry the old fields; keep only the current
            % ones, so that a new default TxComb can be appended to the list.
            if isstruct(s)
                obj = DesignParams;
                names = intersect(fieldnames(s), properties(obj));
                for k = 1:numel(names)
                    obj.(names{k}) = s.(names{k});
                end
            else
                obj = s;
            end
            default = DesignParams;
            extra = setdiff(fieldnames(obj.TxComb), fieldnames(default.TxComb));
            if ~isempty(extra)
                obj.TxComb = rmfield(obj.TxComb, extra);
            end
        end
    end
end