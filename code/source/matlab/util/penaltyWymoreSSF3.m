function score = penaltyWymoreSSF3(L, B, U, S, D, v)
% function penaltyWymoreSSF3
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

    % L is the lower threshold
    % B is the baseline value
    % S is the slope of the tangent to the curve at B
    % D is the domain of the figure of merit, [lower bound, upper bound]
    % v is the value of the figure of merit
    
    %if any(any(v < D(1))) || any(any(v > D(2)))
    %    error('v is outside of the domain D.')
    %end
    score = zeros(length(v),1);
    score(v<L) = -10;
    idx = v>=L & v<B;
    score(idx) = wymoreSSF1(L, B, S, D, v(idx));
    idx = v>=B & v<U;
    score(idx) = 1 - wymoreSSF1(2*B-U, B, S, D, 2*B-v(idx));
    score(v>=U) = 1;
end