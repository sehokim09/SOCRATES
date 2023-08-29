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

    % Copy reference file
    filepath = fullfile(projPath, 'Inp_Design.txt');
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
    texts(idxLastLine + 3) = sprintf("%f               ! Minimum Reflected RCG for GPS [1E-27m^-4]\n", DesignParamsObj.minRCG_R);
    texts(idxLastLine + 4) = sprintf("%f               ! Minimum Direct RCG [dB]\n", DesignParamsObj.minRCG_D);
    texts(idxLastLine + 5) = sprintf("%f               ! Maximum RCG Difference [dB]\n", DesignParamsObj.maxRCGdiff);
    texts(idxLastLine + 6) = sprintf("%d                     ! Longitudinal Grid Divisor\n", DesignParamsObj.gridDivisor);
    texts(idxLastLine + 7) = sprintf("%s/ ! Large File Storage Root Directory\n", DesignParamsObj.largeFilePath);
    lines(idxLastLine+1 : idxLastLine+7) = lines(idxLastLine) + (2:8);
    flagAdd(idxLastLine+1 : idxLastLine+7) = 0;

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
        switch DesignParamsObj.TxComb(iTxComb).polRx
            case 'RHCP'
                polRx = 'R';
            case 'LHCP'
                polRx = 'L';
            case 'V-pol'
                polRx = 'X';
            case 'H-pol'
                polRx = 'Y';
        end
        texts(idxLastLine+4+DesignParamsObj.TxComb(iTxComb).nTx) = sprintf("%f      %s                ! Rx Ant Max Gain [dB], Polarization (R,L,X,Y)\n", DesignParamsObj.TxComb(iTxComb).gainRx_dB, polRx);
        switch DesignParamsObj.TxComb(iTxComb).antPatTypeRx
            case 'Ideal'
                antPatType = 'IDEAL';
            case 'Generalized-Gaussian'
                antPatType = 'GAUSSIAN';
            case 'User-defined'
                antPatType = 'USER_DEFINED';
        end
        texts(idxLastLine+5+DesignParamsObj.TxComb(iTxComb).nTx) = sprintf("%s                  ! Rx Norm Ant Pat (IDEAL, GAUSSIAN, USER_DEFINED)\n", antPatType);
        texts(idxLastLine+6+DesignParamsObj.TxComb(iTxComb).nTx) = sprintf("%f %f %f %f  ! (GAUSSIAN) HPBW[deg], Sidelobe[dB], X-pol[dB], Res[deg]\n", ...
            DesignParamsObj.TxComb(iTxComb).antPatGG.HPBW_deg, DesignParamsObj.TxComb(iTxComb).antPatGG.SLL_dB, DesignParamsObj.TxComb(iTxComb).antPatGG.XPL_dB, DesignParamsObj.TxComb(iTxComb).antPatGG.res_deg);
        
        [~, filename, ext] = fileparts(DesignParamsObj.TxComb(iTxComb).antPatFilepath.Co);
        antPatCo = strcat(filename, ext);
        [~, filename, ext] = fileparts(DesignParamsObj.TxComb(iTxComb).antPatFilepath.X);
        antPatX = strcat(filename, ext);
        
        texts(idxLastLine+7+DesignParamsObj.TxComb(iTxComb).nTx) = sprintf("%s ! (USER_DEFINED) Rx Ant Pat Co-pol File\n", ...
            antPatCo);
        texts(idxLastLine+8+DesignParamsObj.TxComb(iTxComb).nTx) = sprintf("%s ! (USER_DEFINED) Rx Ant Pat X-pol File\n", ...
            antPatX);

        lines(idxLastLine+1 : idxLastLine+8+DesignParamsObj.TxComb(iTxComb).nTx) = lines(idxLastLine) + (1:8+DesignParamsObj.TxComb(iTxComb).nTx);
        flagAdd(idxLastLine+1 : idxLastLine+8+DesignParamsObj.TxComb(iTxComb).nTx) = 1;
        idxLastLine = idxLastLine + 8+DesignParamsObj.TxComb(iTxComb).nTx;
    end

    editTextFile(filepath,lines,texts,flagAdd);
end
