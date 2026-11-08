function filepath = runFilePath(projName, kind, tag)
% RUNFILEPATH  Output file of a run in sim/<projName>/out, named by its run tag:
%   kind "fom"    FoM_<projName>_<tag>.mat        (createFoM)
%        "cases"  FoM_<projName>_<tag>_cases/     (createFoM parallel checkpoint)
%        "score"  score_<projName>_<tag>.mat      (calcScore)
%   tag (optional) defaults to readRunTag(projName).
%
%   See also readRunTag, createFoM, calcScore
    if nargin < 3 || strlength(tag) == 0
        tag = readRunTag(projName);
    end
    switch string(kind)
        case "fom",   name = sprintf('FoM_%s_%s.mat', projName, tag);
        case "cases", name = sprintf('FoM_%s_%s_cases', projName, tag);
        case "score", name = sprintf('score_%s_%s.mat', projName, tag);
        otherwise, error('runFilePath:kind', 'Unknown kind "%s".', kind);
    end
    filepath = fullfile('sim', projName, 'out', name);
end
