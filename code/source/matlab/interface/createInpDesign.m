function createInpDesign(projPath, DesignParamsObj)
% function createInpDesign
%
%   Creates Inp_Design.txt.
%
%   See also missionDesigner, DesignParams.
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
    refFilePath = fullfile(cd, 'code', 'source', 'matlab', 'interface', 'ref', 'Inp_Design.txt');

    % Keep the SAI Observation Model section that export_socrates_inputs.m
    % wrote at the end of an existing file; it is appended again below
    filepath = fullfile(projPath, 'Inp_Design.txt');
    saiSection = strings(0, 1);
    if isfile(filepath)
        oldLines = readlines(filepath);
        iSai = find(contains(oldLines, "SAI Observation Model"), 1);
        if ~isempty(iSai)
            saiSection = oldLines(iSai:end);
            while ~isempty(saiSection) && strlength(strtrim(saiSection(end))) == 0
                saiSection(end) = [];
            end
        end
    end

    % Copy reference file
    system(['cp ', refFilePath, ' ', filepath]);

    lines = [2, 4, 5, 7, 8, 10, 11, 13, 14, 16, 17];
    switch DesignParamsObj.consType
        case 'Walker'
            consType = 'WALKER';
        case 'Hybrid'
            consType = 'HYBRID';
    end
    texts(1) = sprintf("%s                    ! Constellation Type (WALKER, HYBRID)\n", consType);
    texts(2) = sprintf("%d                         ! Number of # Receiver Variable\n", DesignParamsObj.walker.n_nRx);
    str = num2str(DesignParamsObj.walker.nRx(1));
    for i = 2 : DesignParamsObj.walker.n_nRx
        str = [str, ', ', num2str(DesignParamsObj.walker.nRx(i))];
    end
    texts(3) = sprintf("%s    ! # Receiver\n", str);
    texts(4) = sprintf("%d                         ! Number of Altitude Variable\n", DesignParamsObj.walker.nAlt);
    str = num2str(DesignParamsObj.walker.alt(1));
    for i = 2 : DesignParamsObj.walker.nAlt
        str = [str, ', ', num2str(DesignParamsObj.walker.alt(i))];
    end
    texts(5) = sprintf("%s        ! Altitude [km]\n", str);
    texts(6) = sprintf("%d                         ! Number of Inclination Variable\n", DesignParamsObj.walker.nInc);
    str = strjoin(DesignParamsObj.walker.inc, ', ');
    texts(7) = sprintf("%s        ! Inclination [deg]\n", str);
    texts(8) = sprintf("%d                         ! Number of # Orbital Plane Variable\n", DesignParamsObj.walker.n_nPlane);
    str = num2str(DesignParamsObj.walker.nPlane(1));
    for i = 2 : DesignParamsObj.walker.n_nPlane
        str = [str, ', ', num2str(DesignParamsObj.walker.nPlane(i))];
    end
    texts(9) = sprintf("%s        ! # Orbital Plane\n", str);
    texts(10) = sprintf("%s                  ! Constellation Name\n", DesignParamsObj.hybrid.name);
    texts(11) = sprintf("%d        ! # Orbital Plane\n", DesignParamsObj.hybrid.nPlane);
    
    flagAdd = zeros(1,11);
    
    idxLastLine = 11;
    for iPlane = 1 : DesignParamsObj.hybrid.nPlane
        texts(idxLastLine + 1) = sprintf("------------------------------- Plane %d -------------------------------\n", iPlane);
        texts(idxLastLine + 2) = sprintf("%d                        ! # Receiver\n", DesignParamsObj.hybrid.nRx(iPlane));
        texts(idxLastLine + 3) = sprintf("%f                     ! Altitude [km]\n", DesignParamsObj.hybrid.alt(iPlane));
        texts(idxLastLine + 4) = sprintf("%s                     ! Inclination [deg]\n", DesignParamsObj.hybrid.inc(iPlane));
        
        lines(idxLastLine+1 : idxLastLine+4) = lines(idxLastLine) + (1:4);
        flagAdd(idxLastLine+1 : idxLastLine+4) = 1;
        idxLastLine = idxLastLine + 4;
    end

    switch DesignParamsObj.flagTxPropagation
        case true
            flagTxPropagation = 'TRUE';
        case false
            flagTxPropagation = 'FALSE';
    end
    texts(idxLastLine + 1) = sprintf("%s                     ! Real-time Transmitter Propagation\n", flagTxPropagation);
    texts(idxLastLine + 2) = sprintf("%f               ! Observation Time Window [hour]\n", DesignParamsObj.window);
    texts(idxLastLine + 3) = sprintf("%d                     ! Longitudinal Grid Divisor\n", DesignParamsObj.gridDivisor);
    % one trailing slash, however many the stored path has (readInpDesign keeps it)
    texts(idxLastLine + 4) = sprintf("%s/ ! Large File Storage Root Directory\n", regexprep(char(DesignParamsObj.largeFilePath), '/+$', ''));
    lines(idxLastLine+1 : idxLastLine+4) = lines(idxLastLine) + (2:5);
    flagAdd(idxLastLine+1 : idxLastLine+4) = 0;

    idxLastLine = length(lines);
    
    switch DesignParamsObj.flagReport
        case true
            flagReport = 'TRUE';
        case false
            flagReport = 'FALSE';
    end
    texts(idxLastLine + 1) = sprintf("%s                     ! Individual Receiver Data (per Inp_Report.txt)\n", flagReport);
    texts(idxLastLine + 2) = sprintf("%d                         ! Number of SoOp (Tx Combinations)\n", DesignParamsObj.nTxComb);
    
    lines(idxLastLine + 1) = lines(idxLastLine) + 2;
    lines(idxLastLine + 2) = lines(idxLastLine) + 4;
    flagAdd(idxLastLine + 1) = 0;
    flagAdd(idxLastLine + 2) = 0;

    idxLastLine = idxLastLine + 2;
    
    for iTxComb = 1 : DesignParamsObj.nTxComb
        texts(idxLastLine+1) = sprintf("------------------------- SoOp %d ---------------------------------------\n", iTxComb);
        texts(idxLastLine+2) = sprintf("%s                   ! SoOp Name\n", DesignParamsObj.TxComb(iTxComb).name);
        texts(idxLastLine+3) = sprintf("%d                         ! Number of Tx Systems\n", DesignParamsObj.TxComb(iTxComb).nTx);
        for iTx = 1 : DesignParamsObj.TxComb(iTxComb).nTx
            texts(idxLastLine+3+iTx) = sprintf("%s                   ! Tx %d Name (From Inp_Inorbit.txt)\n", DesignParamsObj.TxComb(iTxComb).TxName(iTx), iTx);
        end

        lines(idxLastLine+1 : idxLastLine+3+DesignParamsObj.TxComb(iTxComb).nTx) = lines(idxLastLine) + (1:3+DesignParamsObj.TxComb(iTxComb).nTx);
        flagAdd(idxLastLine+1 : idxLastLine+3+DesignParamsObj.TxComb(iTxComb).nTx) = 1;
        idxLastLine = idxLastLine + 3+DesignParamsObj.TxComb(iTxComb).nTx;
    end

    editTextFile(filepath,lines,texts,flagAdd);

    % Put the SAI Observation Model section back at the end
    if ~isempty(saiSection)
        fid = fopen(filepath, 'a');
        fprintf(fid, '%s\n', saiSection);
        fclose(fid);
    end
end
