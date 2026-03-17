function overwriteLinesInFile(Filename,LineNums,Texts)
% function overwriteLineInFile
%   
%   Overwrites the line LineNum of Filename with Text. 
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

    infile = fopen(Filename);
    outfile = fopen('MatlabTemp.txt','w');
    
    intervals = diff(LineNums);
    
    % Copy infile to outfile, up to the first line to be replaced
    for iCopy = 1 : LineNums(1) - 1 
        line = fgets(infile,512);
        fprintf(outfile,'%s',line);
    end
    % Read and discard line from infile
    fgets(infile,512);

    % Write replacement to outfile
    fprintf(outfile,'%s',Texts(1));

    % Copy infile to outfile, up to the next line to be replaced
    for iLine = 1 : length(intervals)
        for iCopy = 1 : intervals(iLine) - 1 
            line = fgets(infile,512);
            fprintf(outfile,'%s',line);
        end
        % Read and discard line from infile
        fgets(infile,512);
    
        % Write replacement to outfile
        fprintf(outfile,'%s',Texts(iLine+1));
    end    

    % Copy remainder of infile to outfile
    while(~feof(infile))
        line = fgets(infile,512);
        fprintf(outfile,'%s',line);
    end
    
    fclose(infile);
    fclose(outfile);         
    
    % Overwrite old file with modified file
    line = sprintf('mv MatlabTemp.txt %s',Filename);
    system(line);
end