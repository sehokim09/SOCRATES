function tag = readRunTag(projName)
% READRUNTAG  Run tag of the SAI Observation Model section of
%   sim/<projName>/Inp_Design.txt, which names the performance, FoM and score files.
%
%   See also runFilePath, createFoM, calcScore
    file = fullfile('sim', projName, 'Inp_Design.txt');
    lines = readlines(file);
    iTag = find(contains(lines, "! Run tag"), 1);
    if isempty(iTag)
        error('readRunTag:noRunTag', 'No run tag in %s.', file);
    end
    tag = strtrim(extractBefore(lines(iTag), "!"));
end
