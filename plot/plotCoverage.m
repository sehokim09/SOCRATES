%--------------------------------------------------------------------------
%
%   plotCoverage.m
%
%   Read and plot trajectories of receiver, transmitters, and specular
%   points.
%
%   Created 09/16/2020 by Seho Kim @ Purdue University
%
% - 09/06/21 Updated
% - 09/13/22 Updated with the single CYGNSS demo
%
%--------------------------------------------------------------------------
close all; clear all;

%% PARAMETERS
RxName = 'CYG';
TxName = {'GPS'};
nRx = 1;
nTx = length(TxName);

%% CONSTANTS
M_TO_KM = 1E-3;

%% GET DIRECTORIES
dir_parent = fileparts(pwd);
dir_out = strcat(dir_parent, '/sim/Demo-space/out/');
dir_figure = strcat(pwd, '/figure/');
if ~exist(dir_figure, 'dir')
    mkdir(dir_figure)
end

%% READ DATA
opts = delimitedTextImportOptions('Delimiter', ' ', 'VariableTypes', 'double');
for iRx = 1 : nRx
    for iTx = 1 : nTx
        filename = sprintf('%s%d_%s.out', RxName, iRx, TxName{iTx});
        temp = readmatrix(strcat(dir_out, filename), opts);

        % Time
        time.julianDay{iRx, iTx} = temp(:,1);
        time.date{iRx, iTx} = datetime(time.julianDay{iRx, iTx}+1E-6, 'ConvertFrom', 'juliandate');

        % LLA of receiver
        lat.Rx{iRx, iTx} = temp(:,2);
        lon.Rx{iRx, iTx} = temp(:,3);
        alt.Rx{iRx, iTx} = temp(:,4)*M_TO_KM;

        % LLA of transmitters
        lat.Tx{iRx, iTx} = temp(:,6);
        lon.Tx{iRx, iTx} = temp(:,7);
        alt.Tx{iRx, iTx} = temp(:,8)*M_TO_KM;

        % LLA of specular points
        lat.Sp{iRx, iTx} = temp(:,9);
        lon.Sp{iRx, iTx} = temp(:,10);
        alt.Sp{iRx, iTx} = temp(:,11)*M_TO_KM;
        
        % Ancillary data
        anc.TxID{iRx, iTx} = temp(:,5);
        anc.incAng{iRx, iTx} = temp(:,12);
    end
end


%% PLOTS
load coastlines

%% Coverage by GPS constellation
% Index for transmitter system of interest
iTx = 1; % Tx system ID defined from Inp_Inorbit.txt
hf = figure('visible','on','Position',[0 0 1600 700]); hold on; grid on;
axesm('MapProjection','mercator','MapLatLimit',[-60 60],'MapLonLimit',[-180 180],...
 'Frame','on','Grid','on','MeridianLabel','on','ParallelLabel','on', 'MLabelParallel', 'south', 'FontSize', 7);
plotm(coastlat,coastlon,'k')
for iRx = 1 : nRx
    h(1) = scatterm(lat.Rx{iRx, iTx}, lon.Rx{iRx, iTx}, 'm.');
    h(2) = scatterm(lat.Tx{iRx, iTx}, lon.Tx{iRx, iTx}, 'r.');
    h(3) = scatterm(lat.Sp{iRx, iTx}, lon.Sp{iRx, iTx}, [], anc.incAng{iRx, iTx}, '.');
end
axis tight
cb = colorbar;
cb.Label.String = 'Incidence Angle (deg)';
legend(h, 'Receiver', 'Transmitter', 'Specular Point');
title(sprintf('Specular Point Map of %s and %s', RxName, TxName{iTx}))
print(hf, strcat(dir_figure, 'coverage_', RxName, '_', TxName{iTx}), '-dpng');