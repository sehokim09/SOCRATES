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

    % The SAI Observation Model section is written from receiver.model. Before
    % the receiver is designed, the section at the end of an existing file is
    % kept; it is appended again below
    filepath = fullfile(projPath, 'Inp_Design.txt');
    model = DesignParamsObj.receiver.model;
    saiSection = strings(0, 1);
    if isempty(model) && isfile(filepath)
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

    % SAI Observation Model section at the end
    if ~isempty(model)
        writeSaiSection(projPath, model, DesignParamsObj.receiver.runTag);
    elseif ~isempty(saiSection)
        fid = fopen(filepath, 'a');
        fprintf(fid, '%s\n', saiSection);
        fclose(fid);
    end
end

function writeSaiSection(projPath, model, runTag)
% WRITESAISECTION  Append the SAI Observation Model section to Inp_Design.txt
%   and write the reflectivity CDF tables it names. Numbers are written with
%   %.17g, which round-trips a double exactly, so the C code starts from the
%   values evaluateReceiver derived.
    if strlength(runTag) == 0
        error('createInpDesign:noRunTag', 'receiver.runTag is empty; designReceiver sets it.');
    end
    ports = ["X" "Y" "L"];
    for ip = 1:3
        fid = fopen(fullfile(projPath, sprintf('reflectivity_cdf_%s.txt', ports(ip))), 'w');
        fprintf(fid, '%.17g, %.17g, %.17g, %.17g, %.17g\n', model.cdf.(ports(ip)).');
        fclose(fid);
    end

    fid = fopen(fullfile(projPath, 'Inp_Design.txt'), 'a');
    putLine = @(val, cmt) fprintf(fid, '%-42s ! %s\n', val, cmt);
    fprintf(fid, '=================== SAI Observation Model (designReceiver) ===================\n');
    putLine(model.source,                        'Source and timestamp');
    putLine(sprintf('%.17g', model.gamma),       'gamma');
    putLine(sprintf('%.17g', model.WD),          'W_D');
    putLine(sprintf('%.17g', model.etaHV),       'eta_HV (dwell-averaged)');
    putLine(sprintf('%.17g', model.etaL),        'eta_L  (dwell-averaged)');
    putLine(sprintf('%.17g', model.dirLinLoss_dB), 'Direct-arm loss on linear ports [dB]');
    putLine('reflectivity_cdf_X.txt',            'CDF file, port X (V)');
    putLine('reflectivity_cdf_Y.txt',            'CDF file, port Y (H)');
    putLine('reflectivity_cdf_L.txt',            'CDF file, port L (LHCP)');
    putLine('sai/',                              'Pattern subdirectory of ./ant_pat/');
    putLine(runTag,                              'Run tag: output file <constellation>_perform_<tag>.nc');
    putLine(sprintf('%d', model.nTx),            'Number of transmitters');
    fprintf(fid, '! name  type  cdfCol  patTag  Glin_dBi  S0  Nc  TI_s  Nincoh  spotbeam\n');
    for iTx = 1:model.nTx
        t = model.tx(iTx);
        fprintf(fid, '%-8s %-5s %d %-16s %.17g %.17g %.17g %.17g %.17g %d\n', ...
            t.name, t.type, t.cdfCol, t.patTag, t.Glin_dBi, t.S0, t.Nc, t.TI_s, t.Nincoh, t.spotbeam);
    end
    fclose(fid);
end
