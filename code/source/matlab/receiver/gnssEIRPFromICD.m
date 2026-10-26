function [EIRP_dB, f_Hz, info] = gnssEIRPFromICD(sigNames, elev_deg, mode, margin_dB)
%GNSSEIRPFROMICD  GNSS EIRP toward a user at the reference elevation.
%
%   [EIRP_dB, f_Hz, info] = gnssEIRPFromICD(sigNames)
%   [EIRP_dB, f_Hz, info] = gnssEIRPFromICD(sigNames, elev_deg, mode, margin_dB)
%
%   mode "margin" (default)  ICD minimum + margin_dB, the same for every signal
%        "min"               ICD minimum
%        "nominal"           typical delivered EIRP from published measurements
%
%   ICD minimum:  EIRP = P_min + FSPL(R(elev)),  FSPL = 20 log10(4 pi R f / c)
%   P_min is the minimum received power the ICD specifies at the output of a
%   0 dBic RHCP-equivalent antenna (3 dBi linear for GPS) at the low-elevation
%   reference, 5 deg by default. R is the slant range from a user on the
%   surface to a satellite at the nominal altitude, on a sphere of the WGS84
%   equatorial radius. Atmospheric loss is neglected.
%
%     signal          P_min        source
%     GPS_L1CA        -158.5 dBW   IS-GPS-200, L1 C/A
%     GPS_L2C         -160.0 dBW   IS-GPS-200 Table 3-Va, CM+CL
%     GPS_L5          -157.9 dBW   IS-GPS-705 Table 3-III, each of I5 and Q5 (summed here)
%     Galileo_E1      -157.25 dBW  Galileo OS SIS ICD Issue 2.2, Table 13, E1 B+C
%     GLONASS_L1OF    -161.0 dBW   GLONASS ICD Ed. 5.1, 3.3.1.6, 1602 MHz
%     BeiDou_B1C      -159.0 dBW   BDS-SIS-ICD-B1C v1.0, MEO
%
%   Nominal EIRP toward a 5 deg elevation user (only defined at elev_deg = 5):
%     GPS_L1CA      28.0 dBW  C/A transmit power x L1 gain at 13.8 deg, Montenbruck
%                             et al. (2023) J Geod 97:96, Table 2, Fig. 3; blocks
%                             span 26.5 (GPS III, Thoelert et al. 2019 GPS Solut)
%                             to 28.9 (IIR, Marquis & Reigh 2015 NAVIGATION 62(4))
%     Galileo_E1    32.7 dBW  FOC, E1-B+C, Montenbruck et al. (2023), Table 2,
%                             Fig. 3; DLR measurement agrees to 0.1 dB
%     GLONASS_L1OF  29.5 dBW  GLONASS-K2, Thoelert et al. (2024) GPS Solut 28:141;
%                             GLONASS-M power groups extend 4-5 dB lower
%     BeiDou_B1C    31.0 dBW  ESTIMATE, lower end: measured total B1 EIRP 35-35.5
%                             dBW (Thoelert et al. 2019 NAVIGATION 66(4), Fig. 9)
%                             with an ASSUMED 40-50 % B1C share
%     GPS_L2C, GPS_L5        no nominal value sourced (error in "nominal" mode)
%   Verify the GPS and Galileo values against Montenbruck et al. (2023) before
%   publication; the gains were read from a figure.
%
%   margin_dB, default 1.0, is applied to every signal in "margin" mode. It is
%   below the nominal-minus-minimum excess of every constellation (GPS +2.1,
%   Galileo +4.5, BeiDou about +5.5, GLONASS +6.3 dB) and of every GPS block
%   except GPS III (+0.6 dB measured), so the results stay conservative.
%
%   EIRP_dB, f_Hz  column vectors, one row per signal
%   info           table of the inputs, for reporting
%
%   This is the single source of the GNSS EIRPs used by optimize_sai_payload
%   and single_ant_model.

    if nargin < 2 || isempty(elev_deg),  elev_deg  = 5;        end
    if nargin < 3 || isempty(mode),      mode      = "margin"; end
    if nargin < 4 || isempty(margin_dB), margin_dB = 1.0;      end
    mode = lower(string(mode));
    if ~ismember(mode, ["margin" "min" "nominal"])
        error('gnssEIRPFromICD:badMode', 'mode must be "margin", "min" or "nominal".');
    end
    if mode == "nominal" && abs(elev_deg - 5) > 1e-9
        error('gnssEIRPFromICD:nominalElev', 'Nominal EIRPs are defined at 5 deg elevation only.');
    end
    sigNames = string(sigNames(:));

    %        name            P_min_dBW              alt_m      f_Hz       nominal_dBW
    T = { "GPS_L1CA",     -158.5,                 20180e3,  1575.42e6,  28.0
          "GPS_L2C",      -160.0,                 20180e3,  1227.60e6,  NaN
          "GPS_L5",       -157.9 + 10*log10(2),   20180e3,  1176.45e6,  NaN
          "Galileo_E1",   -157.25,                23222e3,  1575.42e6,  32.7
          "GLONASS_L1OF", -161.0,                 19130e3,  1602.00e6,  29.5
          "BeiDou_B1C",   -159.0,                 21528e3,  1575.42e6,  31.0 };
    names = string(T(:,1));

    c  = 299792458;
    a  = 6378137;                       % WGS84 equatorial radius, m
    el = elev_deg*pi/180;

    n = numel(sigNames);
    EIRP_dB = zeros(n,1);  f_Hz = zeros(n,1);  EIRP_min = zeros(n,1);
    Pmin = zeros(n,1);  alt = zeros(n,1);  R = zeros(n,1);  EIRP_nom = zeros(n,1);
    for i = 1:n
        k = find(names == sigNames(i), 1);
        if isempty(k)
            error('gnssEIRPFromICD:unknownSignal', 'Unknown signal "%s". Known: %s.', ...
                  sigNames(i), strjoin(names, ', '));
        end
        Pmin(i) = T{k,2};  alt(i) = T{k,3};  f_Hz(i) = T{k,4};  EIRP_nom(i) = T{k,5};
        R(i) = sqrt((a + alt(i))^2 - (a*cos(el))^2) - a*sin(el);
        EIRP_min(i) = Pmin(i) + 20*log10(4*pi*R(i)*f_Hz(i)/c);
        switch mode
            case "min",     EIRP_dB(i) = EIRP_min(i);
            case "margin",  EIRP_dB(i) = EIRP_min(i) + margin_dB;
            case "nominal"
                if isnan(EIRP_nom(i))
                    error('gnssEIRPFromICD:noNominal', 'No sourced nominal EIRP for %s.', sigNames(i));
                end
                EIRP_dB(i) = EIRP_nom(i);
        end
    end
    info = table(sigNames, Pmin, alt/1e3, R/1e3, f_Hz/1e6, EIRP_min, EIRP_nom, EIRP_dB, ...
        'VariableNames', {'signal','Pmin_dBW','alt_km','range_km','f_MHz', ...
                          'EIRPmin_dBW','EIRPnominal_dBW','EIRP_dBW'});
end
