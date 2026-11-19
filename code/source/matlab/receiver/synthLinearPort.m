function P = synthLinearPort(g, psi, f, w, refloor)
%SYNTHLINEARPORT Power of the linear port cos(psi)*X + sin(psi)*w*Y.
%
%   P = synthLinearPort(g, psi, f, w, refloor)
%
%   g        struct of the floored Ludwig-3 port fields XX, XY, YX, YY
%            (SAIAntennaPattern / gn files), one size
%   psi      port angle from the X dipole [rad], size of g's fields or scalar
%   f        [fX fY] port floors: the minimum field magnitude over the grid
%   w        complex Y-channel weight, 10^(amp_dB/20)*exp(1j*phase_deg*pi/180)
%   refloor  true: add the floor (mean of fX, fY) back to the synthesized port
%
%   The pattern model applied its diffraction floor to each port field
%   (|E|^2 -> |E|^2 + f^2). A sum of floored fields could cancel that floor,
%   so each port's floor is removed first, the ports are combined, and the
%   floor is added back. At psi = 0, w = 1 this returns port X.
%   Used by LookupAntGainSAI (ports V, H) and optimizeReceiver; sai.c
%   saiSynthPower is the C twin.
%
%   See also LookupAntGainSAI, optimizeReceiver, evaluateReceiver
    [xx, xy] = localUnfloor(g.XX, g.XY, f(1));
    [yx, yy] = localUnfloor(g.YX, g.YY, f(2));
    c = cos(psi);  s = sin(psi);
    p1 = c.*xx + s.*w.*yx;
    p2 = c.*xy + s.*w.*yy;
    P = abs(p1).^2 + abs(p2).^2;
    if refloor, P = P + mean(f)^2; end
end

function [a, b] = localUnfloor(a, b, f)
%LOCALUNFLOOR Invert the pattern floor: |E|^2 -> |E|^2 - f^2, direction kept.
    E2 = a.^2 + b.^2;
    s = sqrt(max(E2 - f^2, 0) ./ max(E2, realmin));
    a = a.*s;  b = b.*s;
end
