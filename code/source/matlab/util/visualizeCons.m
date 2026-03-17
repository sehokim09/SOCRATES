function visualizeCons(SimParamsObj, InorbitParamsObj, RxScParamsObj, SoOpParamsObj)
% function visualizeCons
%
%   See also satObs.
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

    % Set up the satellite scenario.
    startTime = SimParamsObj.epoch;
    stopTime = startTime + seconds(SimParamsObj.duration);
    sampleTime = 60;
    sc = satelliteScenario(startTime,stopTime,sampleTime);

    % Add Tx satellites to the scenario from a TLE file.
    nSoOp = length(SoOpParamsObj);
    for iSoOp = 1 : nSoOp
        filenameTLE = SoOpParamsObj(iSoOp).TxScParamsObj(1).filepath.TLE;
        sat{iSoOp} = satellite(sc,filenameTLE,'OrbitPropagator','sgp4');
    end

    % Set Graphical Properties on the Satellites.
    color = ["#FF6929", "#139FFF", "#64D413", "#EDB120",...
             "#7E2F8E", "#4DBEEE", "#77AC30", "#A2142F",...
             "#00FFFF", "#FF0000", "#00FF00", "#0000FF"];
    for iSoOp = 1 : nSoOp
        orbit = [sat{iSoOp}(:).Orbit];
        set(sat{iSoOp}, MarkerColor=color(iSoOp));
        set(orbit, LineColor=color(iSoOp));
    end

    % Add Rx satellites to the scenario using Keplerian elements.
    semiMajorAxis = [RxScParamsObj.Keplerian.minAlt]*1E3 + earthRadius;
    eccentricity = [RxScParamsObj.Keplerian.ecc];
    inclination = [RxScParamsObj.Keplerian.inc];
    rightAscensionOfAscendingNode = [RxScParamsObj.Keplerian.raan];
    argumentOfPeriapsis = [RxScParamsObj.Keplerian.aop];
    trueAnomaly = [RxScParamsObj.Keplerian.ta];

    RxName = strcat(InorbitParamsObj.RxName, num2str((1:InorbitParamsObj.nRx)'));
    Rx = satellite(sc,semiMajorAxis,eccentricity,inclination, ...
        rightAscensionOfAscendingNode,argumentOfPeriapsis,trueAnomaly, ...
        "Name", RxName,'OrbitPropagator','sgp4');

    % Set Graphical Properties on the Satellites.
    set(Rx, MarkerColor="#FFFF00");
    set(Rx.Orbit, LineColor="#FFFF00");
    
    % Visualize the constellations.
    v = satelliteScenarioViewer(sc);
end