classdef SoOpParams < handle
% classdef SoOpParams
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
        TxName % Transmitter name
        nSpMax % Max number of simultaneously trackable specular points
        f_MHz % Freqeuncy in MHz
        polRx % Rx polarization
        polTx % Tx polarization
        EIRP_Tx_dB 
        BW_Tx_kHz
        gainRx_dB
        antPatTypeRx % Rx antenna pattern type
        antPatGG = struct('HPBW_deg', 0, ... % Beamwidth (degrees)
                          'SLL_dB', 0, ... % Sidelobe Level (dB)
                          'XPL_dB', 0, ... % Cross-pol level (dB)
                          'res_deg', 0); % Resoultion (deg)
        antPatFilepath = struct('Co', '', ... % User-defined Co-pol antenna pattern file path
                                'X', ''); % User-defined X-pol antenna pattern file path
        TxScParamsObj = ScParams;
        t_coh = 0.005 % Coherent integration time (sec)
        % Noise Temperature Sky-view, Earth-view [K]
        N = struct('sky', 1373, ... 
                    'earth', 290); 
        minRes = 1000; % Minimum Local Surface Resolution [m]
        % Number of Grid Points in theta, phi direction
        nTh = 4;
        nPh = 6;
    end
    
    methods
        function obj = SoOpParams(TxName)
            if nargin == 1
                obj.TxName = TxName;
                switch TxName
                    case 'ORBCOMM'
                        obj.nSpMax = 20;
                        obj.f_MHz = 137;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 12;
                        obj.BW_Tx_kHz = 25;
                        obj.gainRx_dB = 4.7;
                        obj.antPatTypeRx = 'Ideal';
                    case 'MUOS'
                        obj.nSpMax = 5;
                        obj.f_MHz = 370;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 43;
                        obj.BW_Tx_kHz = 5000;
                        obj.gainRx_dB = 6.1;
                        obj.antPatTypeRx = 'Ideal';
                    case 'GPS'
                        obj.nSpMax = 30;
                        obj.f_MHz = 1575.42;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 32;
                        obj.BW_Tx_kHz = 2046;
                        obj.gainRx_dB = 14.7;
                        obj.antPatTypeRx = 'Ideal';
                    case 'Galileo' % dummy
                        obj.nSpMax = 30;
                        obj.f_MHz = 1575.42;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 32;
                        obj.BW_Tx_kHz = 2046;
                        obj.gainRx_dB = 14.7;
                        obj.antPatTypeRx = 'Ideal';
                    case 'GLONASS' % dummy
                        obj.nSpMax = 30;
                        obj.f_MHz = 1575.42;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 32;
                        obj.BW_Tx_kHz = 2046;
                        obj.gainRx_dB = 14.7;
                        obj.antPatTypeRx = 'Ideal';
                    case 'Beidou' % dummy
                        obj.nSpMax = 30;
                        obj.f_MHz = 1575.42;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 32;
                        obj.BW_Tx_kHz = 2046;
                        obj.gainRx_dB = 14.7;
                        obj.antPatTypeRx = 'Ideal';
                    case 'SWARM' % dummy
                        obj.nSpMax = 30;
                        obj.f_MHz = 1575.42;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 32;
                        obj.BW_Tx_kHz = 2046;
                        obj.gainRx_dB = 14.7;
                        obj.antPatTypeRx = 'Ideal';
                    case 'Starlink' % dummy
                        obj.nSpMax = 30;
                        obj.f_MHz = 1575.42;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 32;
                        obj.BW_Tx_kHz = 2046;
                        obj.gainRx_dB = 14.7;
                        obj.antPatTypeRx = 'Ideal';
                    case 'Iridium' % dummy
                        obj.nSpMax = 30;
                        obj.f_MHz = 1575.42;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 32;
                        obj.BW_Tx_kHz = 2046;
                        obj.gainRx_dB = 14.7;
                        obj.antPatTypeRx = 'Ideal';
                    case 'NOAA' % dummy
                        obj.nSpMax = 30;
                        obj.f_MHz = 1575.42;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 32;
                        obj.BW_Tx_kHz = 2046;
                        obj.gainRx_dB = 14.7;
                        obj.antPatTypeRx = 'Ideal';
                    case 'Ad-hoc' % dummy
                        obj.nSpMax = 30;
                        obj.f_MHz = 1575.42;
                        obj.polRx = 'RHCP';
                        obj.polTx = 'RHCP';
                        obj.EIRP_Tx_dB = 32;
                        obj.BW_Tx_kHz = 2046;
                        obj.gainRx_dB = 14.7;
                        obj.antPatTypeRx = 'Ideal';
                end
            end
        end
    end
end