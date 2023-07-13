function TxScParams = updateTxSc(TxConName, dir, src, flagTLEupdate)
% function updateTxSc
%
%   Updates and creates Tx TLE data
%
%   See also satObs and missionDesigner.
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

    iSc = 0;
    switch TxConName
        case 'MUOS'
            filename_src = 'TLE_from_CelesTrak_MUOS.txt';
            filename_web = 'https://www.celestrak.com/NORAD/elements/geo.txt';
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_MUOS.txt';
            while ~feof(fileID_src)
                Label = fgets(fileID_src,26);
                ID = sscanf(Label,'MUOS-%d');
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                if(isempty(ID))
                else
                    iSc = iSc + 1;
                    out.name{iSc} = sprintf('MUOS-%d',ID);
                    out.TLE1{iSc} = TLE1;
                    out.TLE2{iSc} = TLE2;
                end
            end
            fclose(fileID_src);
        case 'ORBCOMM'
            filename_src = 'TLE_from_CelesTrak_ORBCOMM.txt';
            filename_web = 'https://celestrak.com/NORAD/elements/orbcomm.txt';
            filePath = fullfile(dir, filename_src);
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_ORBCOMM.txt';    
            while ~feof(fileID_src)
                Label = fgets(fileID_src,26);
                ID = sscanf(Label,'ORBCOMM FM%d');
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                if(isempty(ID))
                elseif(ismember(ID,[1,2,3,16,17,22,24,25,26,28,29,33,37,38,39,40,41,101,104,105,106,111,119])) % No longer operational
                else
                    iSc = iSc + 1;
                    out.name{iSc} = sprintf('FM-%03d',ID);
                    out.TLE1{iSc} = TLE1;
                    out.TLE2{iSc} = TLE2;
                end
            end
            fclose(fileID_src);
        case 'GPS'
            filename_src = 'TLE_from_CelesTrak_GPS.txt';
            filename_web = 'https://celestrak.com/NORAD/elements/gps-ops.txt';
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_GPS.txt';    
            while ~feof(fileID_src)
                Label = fgets(fileID_src,26);
                k = strfind(Label,'PRN');
                ID = sscanf(Label(k:end),'PRN %d');
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                iSc = iSc + 1;
                out.name{iSc} = sprintf('PRN-%02d',ID);
                out.TLE1{iSc} = TLE1;
                out.TLE2{iSc} = TLE2;
            end
            fclose(fileID_src);
        case 'GLONASS'
            filename_src = 'TLE_from_CelesTrak_GLONASS.txt';
            filename_web = 'https://celestrak.com/NORAD/elements/glo-ops.txt';
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_GLONASS.txt';
            while ~feof(fileID_src)
                Label = fgets(fileID_src,26);        
                ID = sscanf(Label,'COSMOS %04d');
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                iSc = iSc + 1;
                out.name{iSc} = sprintf('COSMOS-%04d',ID);
                out.TLE1{iSc} = TLE1;
                out.TLE2{iSc} = TLE2;
            end
            fclose(fileID_src);
        case 'Galileo'
            filename_src = 'TLE_from_CelesTrak_Galileo.txt';
            filename_web = 'https://celestrak.com/NORAD/elements/galileo.txt';
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_Galileo.txt';    
            while ~feof(fileID_src)
                Label = fgets(fileID_src,26);
                k = strfind(Label,'PRN');
                ID = sscanf(Label(k:end),'PRN E%d)');
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                if(isempty(ID))
                else
                    iSc = iSc + 1;
                    out.name{iSc} = sprintf('PRN-E%02d',ID);
                    out.TLE1{iSc} = TLE1;
                    out.TLE2{iSc} = TLE2;
                end
            end
            fclose(fileID_src);
        case 'Beidou'
            filename_src = 'TLE_from_CelesTrak_Beidou.txt';
            filename_web = 'https://celestrak.com/NORAD/elements/beidou.txt';
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_Beidou.txt';    
            while ~feof(fileID_src)
                Label = fgets(fileID_src,26);
                k = strfind(Label,'(C');
                ID = sscanf(Label(k:end),'(C%d)');
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                if(isempty(ID))
                else
                    iSc = iSc + 1;
                    out.name{iSc} = sprintf('C%02d',ID);
                    out.TLE1{iSc} = TLE1;
                    out.TLE2{iSc} = TLE2;
                end
            end
            fclose(fileID_src);
        case 'NOAA'
            filename_src = 'TLE_from_CelesTrak_NOAA.txt';
            filename_web = 'https://celestrak.com/NORAD/elements/noaa.txt';
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_NOAA.txt';    
            while ~feof(fileID_src)
                Label = fgets(fileID_src,26);
                ID = sscanf(Label,'NOAA %d');
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                if(isempty(ID))
                else
                    iSc = iSc + 1;
                    out.name{iSc} = sprintf('NOAA-%02d',ID);
                    out.TLE1{iSc} = TLE1;
                    out.TLE2{iSc} = TLE2;
                end
            end
            fclose(fileID_src);
        case 'Starlink'
            filename_src = 'TLE_from_CelesTrak_Starlink.txt';
            filename_web = 'https://celestrak.com/NORAD/elements/starlink.txt';
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_Starlink.txt';    
            while ~feof(fileID_src)
                Label = fgets(fileID_src,26);
                ID = sscanf(Label,'STARLINK-%d');
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                if(isempty(ID))
                else
                    iSc = iSc + 1;
                    out.name{iSc} = sprintf('STARLINK-%d',ID);
                    out.TLE1{iSc} = TLE1;
                    out.TLE2{iSc} = TLE2;
                end
            end
            fclose(fileID_src);
        case 'Iridium'
            filename_src = 'TLE_from_CelesTrak_Iridium.txt';
            filename_web = 'https://celestrak.com/NORAD/elements/iridium-NEXT.txt';
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_Iridium.txt';
            while ~feof(fileID_src)
                Label = fgetl(fileID_src);
                ID = sscanf(Label,'IRIDIUM %d');
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                iSc = iSc + 1;
                out.name{iSc} = sprintf('Iridium-%d',ID);
                out.TLE1{iSc} = TLE1;
                out.TLE2{iSc} = TLE2;
            end
            fclose(fileID_src);
        case 'SWARM'
            filename_src = 'TLE_from_CelesTrak_SWARM.txt';
            filename_web = 'https://celestrak.com/NORAD/elements/swarm.txt';
            filePath = fullfile(dir, filename_src);
            checkTLEfile(filePath, src, filename_web);
            fileID_src = fopen(filePath, 'r');
            filename_TLE = 'Inp_TLE_SWARM.txt';
            while ~feof(fileID_src)
                Label = fgetl(fileID_src);
                ID = sscanf(Label,'SPACEBEE%s');
    
                TLE1 = fgets(fileID_src,512);
                TLE2 = fgets(fileID_src,512);
    
                iSc = iSc + 1;
                out.name{iSc} = sprintf('SPACEBEE%s',ID);
                out.TLE1{iSc} = TLE1;
                out.TLE2{iSc} = TLE2;
            end
            fclose(fileID_src);
        otherwise
            return;
    end
    % Total number of spacecraft
    nSc = iSc;
    % Sorting
    [out.name, idx] = sort(out.name);
    out.TLE1 = out.TLE1(idx);
    out.TLE2 = out.TLE2(idx);
    % Output
    filePath = fullfile(dir, filename_TLE);
    if ~exist(filePath, "file") || flagTLEupdate
        fileID_TLE = fopen(filePath, 'w');
        for iSc = 1 : nSc
            % TLE input     
            fprintf(fileID_TLE,'%s\n',out.name{iSc});
            fprintf(fileID_TLE,'%s',out.TLE1{iSc});
            fprintf(fileID_TLE,'%s',out.TLE2{iSc});
        end
        fclose(fileID_TLE);
    end
    for iSc = 1 : nSc
        % Tx parameters
        TxScParams(iSc) = ScParams;
        TxScParams(iSc).name = out.name{iSc};
        TxScParams(iSc).orbitType = 'TLE';
        TxScParams(iSc).filepath.filename = filename_TLE;
        TxScParams(iSc).filepath.TLE = filePath;
    end
end

function checkTLEfile(filePath, src, filenameWeb)
    if strcmp(src, 'Web')
        fileID = fopen(filePath, 'w');
        data = webread(filenameWeb);
        fprintf(fileID, '%s', data);
        fclose(fileID);
    elseif strcmp(src, 'File')
        if exist(filePath,'file')
            % Do nothing
        else
            % Download from web
            fileID = fopen(filePath, 'w');
            data = webread(filenameWeb);
            fprintf(fileID, '%s', data);
            fclose(fileID);
        end
    end
end