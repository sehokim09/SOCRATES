function lon = easeconv_normalize_degrees(inlon)
% function easeconv_normalize_degrees
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

%   Source and credit: https://github.com/nsidc/easegrids-easeconv
%   Modified by Seho Kim (kim3002@purdue.edu), 08/2022
    inlon(inlon < -180) = inlon(inlon < -180) + 360;
    inlon(inlon > 180) = inlon(inlon > 180) - 360;
    lon = inlon;
end