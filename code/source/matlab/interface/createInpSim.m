function createInpSim(orbPath, SimParamsObj)
% function createInpSim
%
%   Creates Inp_Sim.txt.
%
%   See also satObs, missionDesigner.
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

    % Set paths
    refFilePath = fullfile(cd, 'code', 'source', 'matlab', 'interface', 'ref', 'Inp_Sim.txt');

    % Copy reference file
    filepath = fullfile(orbPath, 'Inp_Sim.txt');
    system(['cp ', refFilePath, ' ', filepath]);

    % Simulation Control
    lines = [4, 5, 9];
    texts(1) = sprintf("%f       %f                    !  Sim Duration, Step Size [sec]\n", SimParamsObj.duration, SimParamsObj.stepSize);
    texts(2) = sprintf("%f                               !  File Output Interval [sec]\n", SimParamsObj.outInterval);
    texts(3) = sprintf("%d                    	        !  Number of Reference Orbits\n", SimParamsObj.nOrb);
    flagAdd = [0, 0, 0];

    % Reference Orbits
    for iOrb = 1 : SimParamsObj.nOrb
        lines(iOrb+3) = lines(3) + iOrb;
        texts(iOrb+3) = sprintf("%s   %s             !  Input file name for Orb %d\n", SimParamsObj.orb(iOrb).exist, SimParamsObj.orb(iOrb).filename, iOrb-1);
        flagAdd(iOrb+3) = 1;
    end
    idxLastLine = SimParamsObj.nOrb+3;

    % Spacecrafts
    lines(idxLastLine+1) = lines(idxLastLine)+2;
    texts(idxLastLine+1) = sprintf("%d                    !  Number of Spacecraft\n", SimParamsObj.nSc);
    flagAdd(idxLastLine+1) = 0;
    for iSc = 1 : SimParamsObj.nSc
        lines(idxLastLine+1+iSc) = lines(idxLastLine+1) + iSc;
        texts(idxLastLine+1+iSc) = sprintf("%s  %d %s                !  Existence, RefOrb, Input file for SC %d\n", SimParamsObj.sc(iSc).exist, SimParamsObj.sc(iSc).orbID, SimParamsObj.sc(iSc).filename, iSc-1);
        flagAdd(idxLastLine+1+iSc) = 1;
    end
    idxLastLine = idxLastLine+1+SimParamsObj.nSc;

    % Environment
    lines(idxLastLine+1) = lines(idxLastLine)+2;
    texts(idxLastLine+1) = sprintf("%d %d %d                        !  Date (UTC) (Month, Day, Year)\n", month(SimParamsObj.epoch), day(SimParamsObj.epoch), year(SimParamsObj.epoch));
    flagAdd(idxLastLine+1) = 0;
    lines(idxLastLine+2) = lines(SimParamsObj.nOrb+SimParamsObj.nSc+4)+2;
    texts(idxLastLine+2) = sprintf("%d %d %f                     !  Time (UTC) (Hr,Min,Sec)\n", hour(SimParamsObj.epoch), minute(SimParamsObj.epoch), second(SimParamsObj.epoch));
    flagAdd(idxLastLine+2) = 0;
    
    % Edit file
    editTextFile(filepath,lines,texts,flagAdd);
end
