%--------------------------------------------------------------------------
%
%   create42InputFiles.m
%
%   Download TLE data and create input files for 42 propagator such as
%   Orb_[TxName].txt, SC_[TxName].txt, Inp_TLE_[TxName].txt.
%
%   Tip: Copy content of Inp_Sim_Orb_List.txt, Inp_Sim_SC_List.txt
%        to complete Inp_Sim.txt for 42 propagator!
%
%   Created by Seho Kim (kim300@purdue.edu)
%
% - 09/08/21 Added Rx to the Orb/SC lists.
% - 10/23/21 Added feature to download Rx TLE data, e.g. CYGNSS
%
%--------------------------------------------------------------------------
clear all;

%% PARAMETERS
RxName = 'CYG';
RxOrbitName = 'CYGNSS';
nRx = 1; % How many receivers would you like to operate?
flagGetRxTLE = false; % Would you like to download TLE data for your receiver system?
TxName = {'MUOS', 'ORBCOMM', 'GPS', 'SWARM'};
%TxName = {'MUOS', 'ORBCOMM', 'GPS', 'Glonass', 'Galileo', 'Beidou'}; % MUOS, ORBCOMM, GPS, Glonass, Galileo, Beidou, NOAA, METEOR, Starlink, Iridium, SWARM
Src = 'Web'; % TLE source - Web / File

%%
nTx = length(TxName);
nSC = nRx;
if flagGetRxTLE
    if strcmp(Src,'Web')
        src_Filename = sprintf('TLE_from_CelesTrak_%s.txt', RxName);
        fileID_src = fopen(src_Filename, 'w');
        if strcmp(RxName, 'CYGNSS')
            data = webread('https://celestrak.com/NORAD/elements/weather.txt');
        end
        fprintf(fileID_src, '%s', data);
        fclose(fileID_src);
    end
    input_Filename = sprintf('TLE_from_CelesTrak_%s.txt', RxName);
    fileID_input = fopen(input_Filename, 'r');

    TLE_Filename = sprintf('Inp_TLE_%s.txt', RxName);
    fileID_TLE = fopen(TLE_Filename, 'w');
    if strcmp(RxName, 'CYGNSS')
        ID_label = 'CYGFM%d';
    end
    iRx = 0;
    while ~feof(fileID_input)
        Label = fgets(fileID_input,26);
        ID = sscanf(Label, ID_label);
        TLE1 = fgets(fileID_input,512);
        TLE2 = fgets(fileID_input,512);

        if(isempty(ID))
        else
            iRx = iRx + 1;
            % TLE input
            Label = sprintf('%s %d                  \n', RxName, ID);        
            fprintf(fileID_TLE,'%s',Label);
            fprintf(fileID_TLE,'%s',TLE1);
            fprintf(fileID_TLE,'%s',TLE2);

            % Orbit File
            Orb_Filename{iRx} = sprintf('Orb_%s_FM%d.txt', RxName, ID);
            system(['cp Orb.txt ' Orb_Filename{iRx}]);

            s1 = sprintf('%s %d	              !  Description\n', RxName, ID);
            s2 = sprintf('"Inp_TLE_%s.txt"               !  File name\n', RxName);
            s3 = sprintf('"%s %d"          !  Label to find in TLE or TRV file\n', RxName, ID);
            OverwriteLineInFile(Orb_Filename{iRx},2,s1);
            OverwriteLineInFile(Orb_Filename{iRx},24,s2);
            OverwriteLineInFile(Orb_Filename{iRx},25,s3);

            % SC File
            SC_Filename{iRx} = sprintf('SC_%s_FM%d.txt', RxName, ID);
            system(['cp SC.txt ' SC_Filename{iRx}]);

            s1 = sprintf('%s %d	              !  Description\n', RxName, ID);
            s2 = sprintf('"%s%d"                        !  Label\n', RxName, ID);
            s3 = sprintf('GenTx1SpriteAlpha.ppm         !  Sprite File Name\n');

            OverwriteLineInFile(SC_Filename{iRx},2,s1);
            OverwriteLineInFile(SC_Filename{iRx},3,s2);
            OverwriteLineInFile(SC_Filename{iRx},4,s3);
        end
    end
    fclose(fileID_input);
else
    for iRx = 1 : nRx
        Orb_Filename{iRx} = sprintf('Orb_%s_%d.txt', RxOrbitName, iRx);
        SC_Filename{iRx} = sprintf('SC_%s.txt', RxName);
    end
end


for iTx = 1 : nTx
    if(strcmp(TxName{iTx},'MUOS'))
        %% MUOS
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_MUOS.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://www.celestrak.com/NORAD/elements/geo.txt');
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_MUOS.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_MUOS.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            Label = fgets(fileID_input,26);
            ID = sscanf(Label,'MUOS-%d');
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            if(isempty(ID))
            else
                nSC = nSC + 1;
                % TLE input
                Label = sprintf('MUOS %d                  \n',ID);        
                fprintf(fileID_TLE,'%s',Label);
                fprintf(fileID_TLE,'%s',TLE1);
                fprintf(fileID_TLE,'%s',TLE2);

                % Orbit File
                Orb_Filename{nSC} = sprintf('Orb_MUOS_%d.txt',ID);
                system(['cp Orb.txt ' Orb_Filename{nSC}]);

                s1 = sprintf('MUOS %d	              !  Description\n', ID);
                s2 = sprintf('"Inp_TLE_MUOS.txt"               !  File name\n');
                s3 = sprintf('"MUOS %d"          !  Label to find in TLE or TRV file\n', ID);
                OverwriteLineInFile(Orb_Filename{nSC},2,s1);
                OverwriteLineInFile(Orb_Filename{nSC},24,s2);
                OverwriteLineInFile(Orb_Filename{nSC},25,s3);

                % SC File
                SC_Filename{nSC} = sprintf('SC_MUOS_%d.txt',ID);
                system(['cp SC.txt ' SC_Filename{nSC}]);

                s1 = sprintf('MUOS %d	              !  Description\n', ID);
                s2 = sprintf('"MUOS%d"                        !  Label\n', ID);
                s3 = sprintf('GenTx1SpriteAlpha.ppm         !  Sprite File Name\n');

                OverwriteLineInFile(SC_Filename{nSC},2,s1);
                OverwriteLineInFile(SC_Filename{nSC},3,s2);
                OverwriteLineInFile(SC_Filename{nSC},4,s3);
            end
        end
        fclose(fileID_input);
    end
    if(strcmp(TxName{iTx},'ORBCOMM'))
        %% ORBCOMM
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_ORBCOMM.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://celestrak.com/NORAD/elements/orbcomm.txt');
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_ORBCOMM.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_ORBCOMM.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            Label = fgets(fileID_input,26);
            ID = sscanf(Label,'ORBCOMM FM%d');
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            if(isempty(ID))
            elseif(ismember(ID,[1,2,3,16,17,22,24,25,26,28,29,33,37,38,39,40,41,101,104,105,106,111,119])) % No longer operational
            else
                nSC = nSC + 1;
                % TLE input
                Label = sprintf('ORBCOMM FM %d                 \n',ID);        
                fprintf(fileID_TLE,'%s',Label);
                fprintf(fileID_TLE,'%s',TLE1);
                fprintf(fileID_TLE,'%s',TLE2);

                % Orbit File
                Orb_Filename{nSC} = sprintf('Orb_ORBCOMM_FM%d.txt',ID);
                system(['cp Orb.txt ' Orb_Filename{nSC}]);

                s1 = sprintf('ORBCOMM FM %d	              !  Description\n', ID);
                s2 = sprintf('"Inp_TLE_ORBCOMM.txt"               !  File name\n');
                s3 = sprintf('"ORBCOMM FM %d"          !  Label to find in TLE or TRV file\n', ID);
                OverwriteLineInFile(Orb_Filename{nSC},2,s1);
                OverwriteLineInFile(Orb_Filename{nSC},24,s2);
                OverwriteLineInFile(Orb_Filename{nSC},25,s3);

                % SC File
                SC_Filename{nSC} = sprintf('SC_ORBCOMM_FM%d.txt',ID);
                system(['cp SC.txt ' SC_Filename{nSC}]);

                s1 = sprintf('ORBCOMM FM %d	              !  Description\n', ID);
                s2 = sprintf('"FM%d"                        !  Label\n', ID);
                s3 = sprintf('GenTx2SpriteAlpha.ppm         !  Sprite File Name\n');

                OverwriteLineInFile(SC_Filename{nSC},2,s1);
                OverwriteLineInFile(SC_Filename{nSC},3,s2);
                OverwriteLineInFile(SC_Filename{nSC},4,s3);
            end
        end
        fclose(fileID_input);
    end    
    if(strcmp(TxName{iTx},'GPS'))
        %% GPS  
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_GPS.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://celestrak.com/NORAD/elements/gps-ops.txt');
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_GPS.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_GPS.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            nSC = nSC + 1;
            Label = fgets(fileID_input,26);
            k = strfind(Label,'PRN');
            ID = sscanf(Label(k:end),'PRN %d');
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            % TLE input
            Label = sprintf('GPS PRN %02d               \n',ID);        
            fprintf(fileID_TLE,'%s',Label);
            fprintf(fileID_TLE,'%s',TLE1);
            fprintf(fileID_TLE,'%s',TLE2);

            % Orbit File
            Orb_Filename{nSC} = sprintf('Orb_GPS_PRN%d.txt',ID);
            system(['cp Orb.txt ' Orb_Filename{nSC}]);

            s1 = sprintf('GPS PRN %d	              !  Description\n', ID);
            s2 = sprintf('"Inp_TLE_GPS.txt"               !  File name\n');
            s3 = sprintf('"GPS PRN %02d"          !  Label to find in TLE or TRV file\n', ID);
            OverwriteLineInFile(Orb_Filename{nSC},2,s1);
            OverwriteLineInFile(Orb_Filename{nSC},24,s2);
            OverwriteLineInFile(Orb_Filename{nSC},25,s3);

            % SC File
            SC_Filename{nSC} = sprintf('SC_GPS_PRN%d.txt',ID);
            system(['cp SC.txt ' SC_Filename{nSC}]);

            s1 = sprintf('GPS PRN %d	              !  Description\n', ID);
            s2 = sprintf('"GPS%d"                        !  Label\n', ID);
            s3 = sprintf('GenTx3SpriteAlpha.ppm         !  Sprite File Name\n');

            OverwriteLineInFile(SC_Filename{nSC},2,s1);
            OverwriteLineInFile(SC_Filename{nSC},3,s2);
            OverwriteLineInFile(SC_Filename{nSC},4,s3);
        end
        fclose(fileID_input);
    end    
    if(strcmp(TxName{iTx},'Glonass')) 
        %% Glonass
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_Glonass.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://celestrak.com/NORAD/elements/glo-ops.txt');
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_Glonass.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_Glonass.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            nSC = nSC + 1;
            Label = fgets(fileID_input,26);        
            ID = sscanf(Label,'COSMOS %04d');
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            % TLE input
            Label = sprintf('Glonass COSMOS %04d             \n',ID);        
            fprintf(fileID_TLE,'%s',Label);
            fprintf(fileID_TLE,'%s',TLE1);
            fprintf(fileID_TLE,'%s',TLE2);

            % Orbit File
            Orb_Filename{nSC} = sprintf('Orb_Glonass_COSMOS%04d.txt',ID);
            system(['cp Orb.txt ' Orb_Filename{nSC}]);

            s1 = sprintf('Glonass COSMOS %04d	              !  Description\n', ID);
            s2 = sprintf('"Inp_TLE_Glonass.txt"               !  File name\n');
            s3 = sprintf('"Glonass COSMOS %04d"          !  Label to find in TLE or TRV file\n', ID);
            OverwriteLineInFile(Orb_Filename{nSC},2,s1);
            OverwriteLineInFile(Orb_Filename{nSC},24,s2);
            OverwriteLineInFile(Orb_Filename{nSC},25,s3);

            % SC File
            SC_Filename{nSC} = sprintf('SC_Glonass_COSMOS%04d.txt',ID);
            system(['cp SC.txt ' SC_Filename{nSC}]);

            s1 = sprintf('Glonass COSMOS %04d	              !  Description\n', ID);
            s2 = sprintf('"COSMOS%d"                        !  Label\n', ID);
            s3 = sprintf('GenTx4SpriteAlpha.ppm         !  Sprite File Name\n');

            OverwriteLineInFile(SC_Filename{nSC},2,s1);
            OverwriteLineInFile(SC_Filename{nSC},3,s2);
            OverwriteLineInFile(SC_Filename{nSC},4,s3);
        end
        fclose(fileID_input);
    end
    if(strcmp(TxName{iTx},'Galileo')) 
        %% Galileo    
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_Galileo.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://celestrak.com/NORAD/elements/galileo.txt');
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_Galileo.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_Galileo.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            Label = fgets(fileID_input,26);
            k = strfind(Label,'PRN');
            ID = sscanf(Label(k:end),'PRN E%d)');
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            if(isempty(ID))
            else
                nSC = nSC + 1;
                % TLE input
                Label = sprintf('Galileo E%02d             \n',ID);        
                fprintf(fileID_TLE,'%s',Label);
                fprintf(fileID_TLE,'%s',TLE1);
                fprintf(fileID_TLE,'%s',TLE2);

                % Orbit File
                Orb_Filename{nSC} = sprintf('Orb_Galileo_E%d.txt',ID);
                system(['cp Orb.txt ' Orb_Filename{nSC}]);

                s1 = sprintf('Galileo E%d	              !  Description\n', ID);
                s2 = sprintf('"Inp_TLE_Galileo.txt"               !  File name\n');
                s3 = sprintf('"Galileo E%02d"          !  Label to find in TLE or TRV file\n', ID);
                OverwriteLineInFile(Orb_Filename{nSC},2,s1);
                OverwriteLineInFile(Orb_Filename{nSC},24,s2);
                OverwriteLineInFile(Orb_Filename{nSC},25,s3);

                % SC File
                SC_Filename{nSC} = sprintf('SC_Galileo_E%d.txt',ID);
                system(['cp SC.txt ' SC_Filename{nSC}]);

                s1 = sprintf('Galileo E%d	              !  Description\n', ID);
                s2 = sprintf('"E%d"                        !  Label\n', ID);
                s3 = sprintf('GenTx5SpriteAlpha.ppm         !  Sprite File Name\n');

                OverwriteLineInFile(SC_Filename{nSC},2,s1);
                OverwriteLineInFile(SC_Filename{nSC},3,s2);
                OverwriteLineInFile(SC_Filename{nSC},4,s3);
            end
        end
        fclose(fileID_input);
    end
    if(strcmp(TxName{iTx},'Beidou')) 
        %% Beidou   
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_Beidou.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://celestrak.com/NORAD/elements/beidou.txt');
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_Beidou.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_Beidou.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            Label = fgets(fileID_input,26);
            k = strfind(Label,'(C');
            ID = sscanf(Label(k:end),'(C%d)');
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            if(isempty(ID))
            else
                nSC = nSC + 1;
                % TLE input
                Label = sprintf('Beidou C%02d              \n',ID);        
                fprintf(fileID_TLE,'%s',Label);
                fprintf(fileID_TLE,'%s',TLE1);
                fprintf(fileID_TLE,'%s',TLE2);

                % Orbit File
                Orb_Filename{nSC} = sprintf('Orb_Beidou_C%d.txt',ID);
                system(['cp Orb.txt ' Orb_Filename{nSC}]);

                s1 = sprintf('Beidou C%d	              !  Description\n', ID);
                s2 = sprintf('"Inp_TLE_Beidou.txt"               !  File name\n');
                s3 = sprintf('"Beidou C%02d"          !  Label to find in TLE or TRV file\n', ID);
                OverwriteLineInFile(Orb_Filename{nSC},2,s1);
                OverwriteLineInFile(Orb_Filename{nSC},24,s2);
                OverwriteLineInFile(Orb_Filename{nSC},25,s3);

                % SC File
                SC_Filename{nSC} = sprintf('SC_Beidou_C%d.txt',ID);
                system(['cp SC.txt ' SC_Filename{nSC}]);

                s1 = sprintf('Beidou C%d	              !  Description\n', ID);
                s2 = sprintf('"C%d"                        !  Label\n', ID);
                s3 = sprintf('GenTx6SpriteAlpha.ppm         !  Sprite File Name\n');

                OverwriteLineInFile(SC_Filename{nSC},2,s1);
                OverwriteLineInFile(SC_Filename{nSC},3,s2);
                OverwriteLineInFile(SC_Filename{nSC},4,s3);
            end
        end
        fclose(fileID_input);
    end
    if(strcmp(TxName{iTx},'NOAA'))
        %% NOAA 
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_NOAA.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://celestrak.com/NORAD/elements/noaa.txt');
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_NOAA.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_NOAA.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            Label = fgets(fileID_input,26);
            ID = sscanf(Label,'NOAA %d');
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            if(isempty(ID))
            else
                nSC = nSC + 1;
                % TLE input
                Label = sprintf('NOAA %02d                  \n',ID);        
                fprintf(fileID_TLE,'%s',Label);
                fprintf(fileID_TLE,'%s',TLE1);
                fprintf(fileID_TLE,'%s',TLE2);

                % Orbit File
                Orb_Filename{nSC} = sprintf('Orb_NOAA_%d.txt',ID);
                system(['cp Orb.txt ' Orb_Filename{nSC}]);

                s1 = sprintf('NOAA %d	              !  Description\n', ID);
                s2 = sprintf('"Inp_TLE_NOAA.txt"               !  File name\n');
                s3 = sprintf('"NOAA %02d"          !  Label to find in TLE or TRV file\n', ID);
                OverwriteLineInFile(Orb_Filename{nSC},2,s1);
                OverwriteLineInFile(Orb_Filename{nSC},24,s2);
                OverwriteLineInFile(Orb_Filename{nSC},25,s3);

                % SC File
                SC_Filename{nSC} = sprintf('SC_NOAA_%d.txt',ID);
                system(['cp SC.txt ' SC_Filename{nSC}]);

                s1 = sprintf('NOAA %d	              !  Description\n', ID);
                s2 = sprintf('"NOAA%d"                        !  Label\n', ID);
                s3 = sprintf('GenTx7SpriteAlpha.ppm         !  Sprite File Name\n');

                OverwriteLineInFile(SC_Filename{nSC},2,s1);
                OverwriteLineInFile(SC_Filename{nSC},3,s2);
                OverwriteLineInFile(SC_Filename{nSC},4,s3);
            end
        end
        fclose(fileID_input);
    end
    if(strcmp(TxName{iTx},'METEOR'))
        %% METEOR
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_METEOR.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://celestrak.com/NORAD/elements/weather.txt');
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_METEOR.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_METEOR.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            Label = fgets(fileID_input,26);
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            if(strcmp(TLE1(3:7),'35865'))
                flag = 1;
                ID = 'M1';
            elseif(strcmp(TLE1(3:7),'40069'))
                flag = 1;
                ID = 'M2';
            elseif(strcmp(TLE1(3:7),'44387'))
                flag = 1;
                ID = 'M2-2';
            else
                flag = 0;
            end

            if(flag)
                nSC = nSC + 1;
                % TLE input
                Label = sprintf('METEOR-%s                \n',ID);        
                fprintf(fileID_TLE,'%s',Label);
                fprintf(fileID_TLE,'%s',TLE1);
                fprintf(fileID_TLE,'%s',TLE2);

                % Orbit File
                Orb_Filename{nSC} = sprintf('Orb_METEOR_%s.txt',ID);
                system(['cp Orb.txt ' Orb_Filename{nSC}]);

                s1 = sprintf('METEOR %s	              !  Description\n', ID);
                s2 = sprintf('"Inp_TLE_METEOR.txt"               !  File name\n');
                s3 = sprintf('"METEOR-%s"          !  Label to find in TLE or TRV file\n', ID);
                OverwriteLineInFile(Orb_Filename{nSC},2,s1);
                OverwriteLineInFile(Orb_Filename{nSC},24,s2);
                OverwriteLineInFile(Orb_Filename{nSC},25,s3);

                % SC File
                SC_Filename{nSC} = sprintf('SC_METEOR_%s.txt',ID);
                system(['cp SC.txt ' SC_Filename{nSC}]);

                s1 = sprintf('METEOR %s	              !  Description\n', ID);
                s2 = sprintf('"%s"                        !  Label\n', ID);
                s3 = sprintf('GenTx8SpriteAlpha.ppm         !  Sprite File Name\n');

                OverwriteLineInFile(SC_Filename{nSC},2,s1);
                OverwriteLineInFile(SC_Filename{nSC},3,s2);
                OverwriteLineInFile(SC_Filename{nSC},4,s3);
            end
        end
        fclose(fileID_input);  
    end
    if(strcmp(TxName{iTx},'Starlink'))
        %% Starlink
        input_Filename = 'TLE_Starlink.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_Starlink.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            Label = fgets(fileID_input,26);
            ID = sscanf(Label,'STARLINK-%d');
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            if(isempty(ID))
            else
                nSC = nSC + 1;
                % TLE input
                Label = sprintf('STARLINK %02d                  \n',ID);        
                fprintf(fileID_TLE,'%s',Label);
                fprintf(fileID_TLE,'%s',TLE1);
                fprintf(fileID_TLE,'%s',TLE2);

                % Orbit File
                Orb_Filename{nSC} = sprintf('Orb_Starlink_%d.txt',ID);
                system(['cp Orb.txt ' Orb_Filename{nSC}]);

                s1 = sprintf('STARLINK %d	              !  Description\n', ID);
                s2 = sprintf('"Inp_TLE_Starlink.txt"               !  File name\n');
                s3 = sprintf('"STARLINK %02d"          !  Label to find in TLE or TRV file\n', ID);
                OverwriteLineInFile(Orb_Filename{nSC},2,s1);
                OverwriteLineInFile(Orb_Filename{nSC},24,s2);
                OverwriteLineInFile(Orb_Filename{nSC},25,s3);

                % SC File
                SC_Filename{nSC} = sprintf('SC_Starlink_%d.txt',ID);
                system(['cp SC.txt ' SC_Filename{nSC}]);

                s1 = sprintf('STARLINK %d	              !  Description\n', ID);
                s2 = sprintf('"STARLINK%d"                        !  Label\n', ID);
                s3 = sprintf('GenTx7SpriteAlpha.ppm         !  Sprite File Name\n');

                OverwriteLineInFile(SC_Filename{nSC},2,s1);
                OverwriteLineInFile(SC_Filename{nSC},3,s2);
                OverwriteLineInFile(SC_Filename{nSC},4,s3);
            end
        end
        fclose(fileID_input);
    end
    if(strcmp(TxName{iTx},'Iridium'))
        %% Iridium  
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_Iridium.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://celestrak.com/NORAD/elements/iridium-NEXT.txt'); % fix this
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_Iridium.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_Iridium.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            nSC = nSC + 1;
            Label = fgetl(fileID_input);
            ID = sscanf(Label,'IRIDIUM %d');
            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            % TLE input
            Label = sprintf('Iridium %d               \n',ID);        
            fprintf(fileID_TLE,'%s',Label);
            fprintf(fileID_TLE,'%s',TLE1);
            fprintf(fileID_TLE,'%s',TLE2);

            % Orbit File
            Orb_Filename{nSC} = sprintf('Orb_Iridium_%d.txt',ID);
            system(['cp Orb.txt ' Orb_Filename{nSC}]);

            s1 = sprintf('Iridium %d	              !  Description\n', ID);
            s2 = sprintf('"Inp_TLE_Iridium.txt"               !  File name\n');
            s3 = sprintf('"Iridium %d"          !  Label to find in TLE or TRV file\n', ID);
            OverwriteLineInFile(Orb_Filename{nSC},2,s1);
            OverwriteLineInFile(Orb_Filename{nSC},24,s2);
            OverwriteLineInFile(Orb_Filename{nSC},25,s3);

            % SC File
            SC_Filename{nSC} = sprintf('SC_Iridium_%d.txt',ID);
            system(['cp SC.txt ' SC_Filename{nSC}]);

            s1 = sprintf('Iridium %d	              !  Description\n', ID);
            s2 = sprintf('"Iridium%d"                        !  Label\n', ID);
            s3 = sprintf('GenTx4SpriteAlpha.ppm         !  Sprite File Name\n');

            OverwriteLineInFile(SC_Filename{nSC},2,s1);
            OverwriteLineInFile(SC_Filename{nSC},3,s2);
            OverwriteLineInFile(SC_Filename{nSC},4,s3);
        end
        fclose(fileID_input);
    end
    if(strcmp(TxName{iTx},'SWARM'))
        %% SWARM
        if(strcmp(Src,'Web'))
            src_Filename = 'TLE_from_CelesTrak_SWARM.txt';
            fileID_src = fopen(src_Filename, 'w');
            data = webread('https://celestrak.com/NORAD/elements/swarm.txt'); % fix this
            fprintf(fileID_src, '%s', data);
            fclose(fileID_src);
        end
        input_Filename = 'TLE_from_CelesTrak_SWARM.txt';
        fileID_input = fopen(input_Filename, 'r');

        TLE_Filename = 'Inp_TLE_SWARM.txt';
        fileID_TLE = fopen(TLE_Filename, 'w');

        while ~feof(fileID_input)
            Label = fgetl(fileID_input);
            ID = sscanf(Label,'SPACEBEE%s');

            TLE1 = fgets(fileID_input,512);
            TLE2 = fgets(fileID_input,512);

            nSC = nSC + 1;

            % TLE input
            Label = sprintf('SWARM%s                 \n',ID);        
            fprintf(fileID_TLE,'%s',Label);
            fprintf(fileID_TLE,'%s',TLE1);
            fprintf(fileID_TLE,'%s',TLE2);

            % Orbit File
            Orb_Filename{nSC} = sprintf('Orb_SWARM%s.txt',ID);
            system(['cp Orb.txt ' Orb_Filename{nSC}]);

            s1 = sprintf('SWARM%s  	              !  Description\n', ID);
            s2 = sprintf('"Inp_TLE_SWARM.txt"           !  File name\n');
            s3 = sprintf('"SWARM%s"               !  Label to find in TLE or TRV file\n', ID);
            OverwriteLineInFile(Orb_Filename{nSC},2,s1);
            OverwriteLineInFile(Orb_Filename{nSC},24,s2);
            OverwriteLineInFile(Orb_Filename{nSC},25,s3);

            % SC File
            SC_Filename{nSC} = sprintf('SC_SWARM%s.txt',ID);
            system(['cp SC.txt ' SC_Filename{nSC}]);

            s1 = sprintf('SWARM%s  	              !  Description\n', ID);
            s2 = sprintf('"SWARM%s"                    !  Label\n', ID);
            s3 = sprintf('GenTx4SpriteAlpha.ppm         !  Sprite File Name\n');

            OverwriteLineInFile(SC_Filename{nSC},2,s1);
            OverwriteLineInFile(SC_Filename{nSC},3,s2);
            OverwriteLineInFile(SC_Filename{nSC},4,s3);
        end
        fclose(fileID_input);
    end
end

%% Make SC_ist and Orb List
fileID_list = fopen('Inp_Sim_Orb_List.txt', 'w');
fprintf(fileID_list, '%d                    !  Number of Reference Orbits\n', nSC);
for iSC = 1 : nSC
    fprintf(fileID_list, 'TRUE   %s             !  Input file name for Orb %d\n', Orb_Filename{iSC}, iSC-1);
end
fclose(fileID_list);

fileID_list = fopen('Inp_Sim_SC_List.txt', 'w');
fprintf(fileID_list, '%d                    !  Number of Spacecraft\n', nSC);
for iSC = 1 : nSC
    fprintf(fileID_list, 'TRUE  %d %s           !  Existence, RefOrb, Input file for SC %d\n', iSC-1, SC_Filename{iSC}, iSC-1);
end
fclose(fileID_list);




function OverwriteLineInFile(Filename,LineNum,Text)


      infile = fopen(Filename);
      outfile = fopen('MatlabTemp.txt','w');

% ... Copy infile to outfile, up to line to be replaced
      for i=1:(LineNum-1),
         line = fgets(infile,512);
         %fputs(outfile,line);
         fprintf(outfile,'%s',line);
      end

% ... Read and discard line from infile
      line = fgets(infile,512);
% ... Write replacement to outfile
      fprintf(outfile,'%s',Text);

% ... Copy remainder of infile to outfile
      while(~feof(infile)),
         line = fgets(infile,512);
         %fputs(outfile,line);
         fprintf(outfile,'%s',line);
      end

      fclose(infile);
      fclose(outfile);         

% ... Overwrite old file with modified file
      line = sprintf('mv MatlabTemp.txt %s',Filename);
      system(line);

end