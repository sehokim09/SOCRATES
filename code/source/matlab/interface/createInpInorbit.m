function createInpInorbit(projPath, InorbitParamsObj, SoOpParamsObj)
% function createInpInorbit
%
%   Creates Inp_Inorbit.txt.
%
%   See also satObs, missionDesigner, InorbitParams, SoOpParams.
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
    refFilePath = fullfile(cd, 'code', 'source', 'matlab', 'interface', 'ref', 'Inp_Inorbit.txt');

    % Copy reference file
    filepath = fullfile(projPath, 'Inp_Inorbit.txt');
    system(['cp ', refFilePath, ' ', filepath]);

    lines = [2, 3, 4, 5, 7, 8];
    switch InorbitParamsObj.obsBase
        case 'Space'
            obsBase = 'SPACE';
        case 'Tower'
            obsBase = 'TOWER';
    end
    texts(1) = sprintf("%s                         ! Space-based or Tower-based Observation (SPACE, TOWER)\n", obsBase);
    texts(2) = sprintf("%d                             ! Number of Receivers, NRx\n", InorbitParamsObj.nRx);
    texts(3) = sprintf("%d                             ! Number of Transmitter Systems, NSoOp\n", InorbitParamsObj.nSoOp);
    texts(4) = sprintf("%s                            ! Orbit Propagation Model (IMPORT, 42, 42_DECAY)\n", InorbitParamsObj.orbModel);
    texts(5) = sprintf("%d                             ! Number of Spacecrafts\n", InorbitParamsObj.orbImport.nSc);
    texts(6) = sprintf("%d                         ! Number of Lines in Files\n", InorbitParamsObj.orbImport.nLine);
    flagAdd = [0, 0, 0, 0, 0, 0];
    
    for iSc = 1 : InorbitParamsObj.orbImport.nSc
        lines(iSc+6) = 8 + iSc;
        texts(iSc+6) = sprintf("%s        ! SC %d File Name (MJD/Pos/Vel)\n", InorbitParamsObj.orbImport.filename(iSc), iSc);
        flagAdd(iSc+6) = 1;
    end
    idxLastLine = InorbitParamsObj.orbImport.nSc+6;

    % 42 orbits
    lines(idxLastLine+1) = lines(idxLastLine)+3;
    texts(idxLastLine+1) = sprintf("%s    %d    %d          ! Rx Name, First RefOrb, Last RefOrb in Inp_Sim.txt\n", InorbitParamsObj.RxName, InorbitParamsObj.RxRefOrb(1), InorbitParamsObj.RxRefOrb(2));
    flagAdd(idxLastLine+1) = 0;
    lines(idxLastLine+2) = lines(idxLastLine)+5;
    texts(idxLastLine+2) = sprintf("%s  %f  %f  %f  ! Label, Lat[deg], Lng[deg], Height[m]\n", InorbitParamsObj.TwrName, InorbitParamsObj.TwrLLA(1), InorbitParamsObj.TwrLLA(2), InorbitParamsObj.TwrLLA(3));
    flagAdd(idxLastLine+2) = 0;

    idxLastLine = idxLastLine+2;

    for iSoOp = 1 : InorbitParamsObj.nSoOp
        lines(idxLastLine+iSoOp) = lines(idxLastLine)+2;
        texts(idxLastLine+iSoOp) = sprintf("%s   %d   %d        ! Tx %d Name, First RefOrb, Last RefOrb in Inp_Sim.txt\n", SoOpParamsObj(iSoOp).TxName, InorbitParamsObj.TxRefOrb(iSoOp,1), InorbitParamsObj.TxRefOrb(iSoOp,2), iSoOp);
        flagAdd(idxLastLine+iSoOp) = 1;
    end
    idxLastLine = idxLastLine+InorbitParamsObj.nSoOp;
    
    % SoOp Configurations
    for iSoOp = 1 : InorbitParamsObj.nSoOp
        texts(idxLastLine+1) = sprintf("**********************************************************************\n");
        texts(idxLastLine+2) = sprintf("********************* SoOp Configuration %d (Rx-Tx%d) ******************\n", iSoOp, iSoOp);
        texts(idxLastLine+3) = sprintf("**********************************************************************\n");
        texts(idxLastLine+4) = sprintf("%d                             ! Max Number of Specular Points\n", SoOpParamsObj(iSoOp).nSpMax);
        texts(idxLastLine+5) = sprintf("------------------------- Receive Antenna ----------------------------\n");
        switch SoOpParamsObj(iSoOp).polRx
            case 'RHCP'
                polRx = 'R';
            case 'LHCP'
                polRx = 'L';
            case 'V-pol'
                polRx = 'X';
            case 'H-pol'
                polRx = 'Y';
        end
        texts(idxLastLine+6) = sprintf("%f  %s                     ! Max Gain [dB], Polarization (R,L,X,Y)\n", SoOpParamsObj(iSoOp).gainRx_dB, polRx);
        switch SoOpParamsObj(iSoOp).antPatTypeRx
            case 'Ideal'
                antPatType = 'IDEAL';
            case 'Generalized-Gaussian'
                antPatType = 'GAUSSIAN';
            case 'User-defined'
                antPatType = 'USER_DEFINED';
        end
        texts(idxLastLine+7) = sprintf("%s                  ! Normalized Antenna Pattern (IDEAL, GAUSSIAN, USER_DEFINED)\n", antPatType);
        texts(idxLastLine+8) = sprintf("%f %f %f %f  ! (GAUSSIAN) HPBW[deg], Sidelobe[dB], X-pol[dB], Res[deg]\n", ...
            SoOpParamsObj(iSoOp).antPatGG.HPBW_deg, SoOpParamsObj(iSoOp).antPatGG.SLL_dB, SoOpParamsObj(iSoOp).antPatGG.XPL_dB, SoOpParamsObj(iSoOp).antPatGG.res_deg);
        
        [~, filename, ext] = fileparts(SoOpParamsObj(iSoOp).antPatFilepath.Co);
        antPatCo = strcat(filename, ext);
        [~, filename, ext] = fileparts(SoOpParamsObj(iSoOp).antPatFilepath.X);
        antPatX = strcat(filename, ext);
        
        texts(idxLastLine+9) = sprintf("%s    ! (USER_DEFINED) Antenna Pattern XX-pol File Name\n", ...
            antPatCo);
        texts(idxLastLine+10) = sprintf("%s    ! (USER_DEFINED) Antenna Pattern XY-pol File Name\n", ...
            antPatX);
        texts(idxLastLine+11) = sprintf("%s    ! (USER_DEFINED) Antenna Pattern YX-pol File Name\n", ...
            antPatX);
        texts(idxLastLine+12) = sprintf("%s    ! (USER_DEFINED) Antenna Pattern YY-pol File Name\n", ...
            antPatCo);
        texts(idxLastLine+13) = sprintf("------------------------- Transmitter --------------------------------\n");
        texts(idxLastLine+14) = sprintf("%f                         ! Frequency [MHz]\n", SoOpParamsObj(iSoOp).f_MHz);
        switch SoOpParamsObj(iSoOp).polTx
            case 'RHCP'
                polTx = 'R';
            case 'LHCP'
                polTx = 'L';
            case 'V-pol'
                polTx = 'X';
            case 'H-pol'
                polTx = 'Y';
        end
        texts(idxLastLine+15) = sprintf("%f      %s                     ! EIRP [dB], Polarization (R,L,X,Y)\n", SoOpParamsObj(iSoOp).EIRP_Tx_dB, polTx);
        texts(idxLastLine+16) = sprintf("%f                            ! Bandwidth of channel [kHz]\n", SoOpParamsObj(iSoOp).BW_Tx_kHz);
        texts(idxLastLine+17) = sprintf("------------------------- Instrument ---------------------------------\n");
        texts(idxLastLine+18) = sprintf("%f                         ! Coherent Integration Time [sec]\n", SoOpParamsObj(iSoOp).t_coh);
        texts(idxLastLine+19) = sprintf("%f    %f        ! Noise Temperature Sky-view, Earth-view [K]\n", SoOpParamsObj(iSoOp).N.sky, SoOpParamsObj(iSoOp).N.earth);
        texts(idxLastLine+20) = sprintf("------------------------- Other Parameters ---------------------------\n");
        texts(idxLastLine+21) = sprintf("%f                          ! Minimum Local Surface Resolution [m]\n", SoOpParamsObj(iSoOp).minRes);
        texts(idxLastLine+22) = sprintf("%d       %d                    ! Number of Grid Points in theta, phi direction\n", SoOpParamsObj(iSoOp).nTh, SoOpParamsObj(iSoOp).nPh);
        
        lines(idxLastLine+1 : idxLastLine+22) = lines(idxLastLine) + (1:22);
        flagAdd(idxLastLine+1 : idxLastLine+22) = 1;
        idxLastLine = idxLastLine + 22;
    end
    
    editTextFile(filepath,lines,texts,flagAdd);
end
