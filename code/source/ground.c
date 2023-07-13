 /* Copyright (C) 2023 Seho Kim

	This file is part of SOCRATES.

    SOCRATES is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SOCRATES is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include "42.h"
#include "socrates.h"

/* n-th polynomial fit by Least-squares approach
* INPUTS
*   n    Degree of polynomial
* 
*/
void polyfitML(long n)
{
    long N; //no. of data-points

	N = NLayer;  

    double x[N], y[N]; //arrays to store the x and y-axis data-points
	double X[2*n+1]; // an array of size 2*n+1 for storing N, Sig xi, Sig xi^2, ...., etc. which are the independent components of the normal matrix
    double B[n+1][n+2]; //the normal augmented matrix
    double Y[n+1];     // rhs
	double A[n+1]; // Coefficients
	
	double e_p[MultiLayer->NSublayer], e_dp[MultiLayer->NSublayer]; // real and imaginary part of dielectric constant

	int i,j;

	// x-axis data
    for(i=0;i<N;i++){
        x[i] = GndData[i].depth;
    }
	/* .. Real part of dielectric constants . */
    for(i=0;i<N;i++){
        y[i] = 0; //creal(Sp->LocalGnd[i].e_c);
    }
    for(i=0;i<=2*n;i++){
        X[i]=0;
        for(j=0;j<N;j++){
            X[i]=X[i]+pow(x[j],i);
        }
    }
    for(i=0;i<=n;i++){
        Y[i]=0;
        for(j=0;j<N;j++){
            Y[i]=Y[i]+pow(x[j],i)*y[j];
        }
    }
    for(i=0;i<=n;i++){
        for(j=0;j<=n;j++){
            B[i][j]=X[i+j]; 
        }
    }
    for(i=0;i<=n;i++){
        B[i][n+1]=Y[i];
    }
	// Compute coefficients of the polynomial
    gaussEliminationLS(n+1,n+2,B,A);

	// Compute real part of dielectric constant
	for(i=0; i<MultiLayer->NSublayer; i++){
		for(j=0; j<=n; j++){
			e_p[i] = e_p[i] + A[j] * pow(MultiLayer->z[i],j);
		}
	}

	/* .. Imaginary part of dielectric constants . */
    for(i=0;i<N;i++){
        y[i] = 0; // cimag(Sp->LocalGnd[i].e_c); // need to modifiy
    }
    for(i=0;i<=2*n;i++){
        X[i]=0;
        for(j=0;j<N;j++){
            X[i]=X[i]+pow(x[j],i);
        }
    }
    for(i=0;i<=n;i++){
        Y[i]=0;
        for(j=0;j<N;j++){
            Y[i]=Y[i]+pow(x[j],i)*y[j];
        }
    }
    for(i=0;i<=n;i++){
        for(j=0;j<=n;j++){
            B[i][j]=X[i+j]; 
        }
    }
    for(i=0;i<=n;i++){
        B[i][n+1]=Y[i];
    }
	// Compute coefficients of the polynomial
    gaussEliminationLS(n+1,n+2,B,A);

	// Compute imaginary part of dielectric constant
	for(i=0; i<MultiLayer->NSublayer; i++){
		for(j=0; j<=n; j++){
			e_dp[i] = e_dp[i] + A[j] * pow(MultiLayer->z[i],j);
		}
	}

	/* .. Dielectric Constant Profile .. */
	for(i=0; i<MultiLayer->NSublayer; i++){
		Gnd->e_c_profile[i] = e_p[i] + I1 * e_dp[i];
	}
}

/* Look up USDA soil texture based on soil fractions of sand, clay, slit
	and assign soil water characteristic values based on Rawls et al. (1982) */
void LookupSoiltype(void)
{
	float sand = Gnd->sand;
	float clay = Gnd->clay;
	float silt = 1 - sand - clay;
	
	if(sand>=0.85 && clay<0.1 && silt < 0.15){
	    Gnd->soilType = SAND; Gnd->resid = 0.020; Gnd->wp = 0.033; Gnd->fc = 0.091; Gnd->poro = 0.437;
	}
	else if(sand<0.9 && sand>=0.7 && clay<0.15 && silt < 0.3){
		Gnd->soilType = LOAMY_SAND; Gnd->resid = 0.035; Gnd->wp = 0.055; Gnd->fc = 0.125; Gnd->poro = 0.437;
	}
	else if(sand>=0.43 && sand<0.85 && clay<0.2 && silt<0.5){
		Gnd->soilType = SANDY_LOAM; Gnd->resid = 0.041; Gnd->wp = 0.095; Gnd->fc = 0.207; Gnd->poro = 0.453;
	}
	else if(sand<0.5 && clay<0.27 && silt<0.88 && silt>=0.5){
		Gnd->soilType = SILT_LOAM;  Gnd->resid = 0.015; Gnd->wp = 0.133; Gnd->fc = 0.330; Gnd->poro = 0.501;
	}
	else if(sand<0.2 && clay<0.12 && silt>=0.8){
		Gnd->soilType = SILT; Gnd->resid = 0.020; Gnd->wp = 0.110; Gnd->fc = 0.370; Gnd->poro = 0.481;
	}
	else if(sand<0.52 && sand>=0.23 && clay<0.27 && clay>=0.07 && silt<0.5 && silt>=0.28){
		Gnd->soilType = LOAM; Gnd->resid = 0.027; Gnd->wp = 0.117; Gnd->fc = 0.270; Gnd->poro = 0.463;
	}
	else if(sand<0.8 && sand>=0.45 && clay<0.35 && clay>=0.2 && silt<0.28){
		Gnd->soilType = SANDY_CLAY_LOAM; Gnd->resid = 0.070; Gnd->wp = 0.148; Gnd->fc = 0.255; Gnd->poro = 0.398;
	}
	else if(sand<0.2 && clay<0.4 && clay>=0.27 && silt<0.73 && silt>=0.4){
		Gnd->soilType = SILTY_CLAY_LOAM; Gnd->resid = 0.040; Gnd->wp = 0.208; Gnd->fc = 0.366; Gnd->poro = 0.471;
	}
	else if(sand<0.45 && sand>=0.2 && clay<0.4 && clay>=0.27 && silt<0.53 && silt>=0.15){
		Gnd->soilType = CLAY_LOAM; Gnd->resid = 0.075; Gnd->wp = 0.197; Gnd->fc = 0.318; Gnd->poro = 0.464;
	}
	else if(sand<0.65 && sand>=0.45 && clay<0.55 && clay>=0.35 && silt<0.2){
		Gnd->soilType = SANDY_CLAY; Gnd->resid = 0.109; Gnd->wp = 0.239; Gnd->fc = 0.339; Gnd->poro = 0.430;
	}
	else if(sand<0.2 && clay<0.6 && clay>=0.4 && silt<0.6 && silt>=0.4){
		Gnd->soilType = SILTY_CLAY; Gnd->resid = 0.056; Gnd->wp = 0.250; Gnd->fc = 0.387; Gnd->poro = 0.479;
	}
	else if(sand<0.45 && clay>=0.4 && silt<0.4){
		Gnd->soilType = CLAY; Gnd->resid = 0.090; Gnd->wp = 0.272; Gnd->fc = 0.396; Gnd->poro = 0.475;
	}
	else
	{
		printf(">> Error - can't define soil type.\n"); exit(1);
	}
	
}

void updateLocalGnd(double lat, double lon, double h)
{
	uint16_t rowSt, colSt, rowDy, colDy, rowSnow, colSnow;
	long ILayer;

	// Mapping indices
	LL2EzRC1km(lat, lon, &rowSt, &colSt);
	LL2EzRC9km(lat, lon, &rowDy, &colDy);

	// Static data
	if(rowSt<14616 && colSt<34704){
		if(SttData->ExistSoilTexture){
			Gnd->sand = SttData->sand[rowSt][colSt];
			Gnd->clay = SttData->clay[rowSt][colSt];
			Gnd->bulk = SttData->bulk[rowSt][colSt];
		}
		if(SttData->ExistLandMask){
			Gnd->landMask = SttData->landMask[rowSt][colSt];
		}
	}
	else{
		Gnd->sand = NAN;
		Gnd->clay = NAN;
		Gnd->bulk = NAN;
		Gnd->landMask = (uint8_t) NAN;		
	}

	if(SttData->ExistSoilPorosity){
		uint16_t rowPoro, colPoro;
		rowPoro = round((lat*R2D + 59.875) / 0.25);
		colPoro = round((lon*R2D + 179.875) / 0.25);
		Gnd->porosity = SttData->porosity[rowPoro][colPoro];
	}

	// Dynamic data
	if(rowDy<1624 && colDy<3856){
		if(DynData->ExistVSM){
			for(ILayer=0; ILayer<NLayer; ILayer++)
				Gnd->VSM[ILayer] = GndData[ILayer].VSM[rowDy][colDy];
		}
		if(DynData->ExistFreezeThaw){
			Gnd->freezeThaw = DynData->freezeThaw[rowDy][colDy];
		}
		if(DynData->ExistSoilTemp){
			Gnd->soilTemp1 = DynData->soilTemp1[rowDy][colDy] - 273.15; // Kelvin to Celsius
		}
	}
	else{
		for(ILayer=0; ILayer<NLayer; ILayer++)
			Gnd->VSM[ILayer] = NAN;
		Gnd->freezeThaw = (uint8_t) NAN;
		Gnd->soilTemp1 = NAN;
	}

	if(DynData->ExistSnowCover){
		rowSnow = 1799.9999 - lat*R2D*20;//(90-lat)*20;
		colSnow = 3599.9999 + lon*R2D*20;//(lon+180)*20;
		Gnd->snowCover = DynData->snowCover[rowSnow][colSnow];
	}

	// Effective roughness parameter
	Gnd->h = h;
}

// freq in Hz
double complex calcDielDebye(double freq)
{
	double T = 10; // Temperature in Celsius
	double e_winf = 4.9; // high-frequency (or optical) limit of e_p_fw from Equation E.16 in Ulaby Vol. III, Appendix E.2
	double e_w0, tau_w;
	double eps_r, eps_i;
	double complex e_c;

	// Relative dielectric constant of free water, given by Debye-type dispersion equation (Ulaby Vol.III, Appendix E.2) */
	// Equation E.19 in Ulaby Vol.III, Appendix E.2
	e_w0 = 88.045 - 0.4147*T + 6.295E-4*T*T + 1.075E-5*T*T*T;

	// Equation E.17 in Ulaby Vol.III, Appnedix E.2
	tau_w = (1.1109E-10 - 3.824E-12*T + 6.938E-14*T*T - 5.096E-16*T*T*T) / TwoPi;

	double temp1, temp2;
	temp1 = TwoPi*freq*tau_w;
	temp2 = 1 + temp1*temp1;
	// Real part
	eps_r = e_winf + (e_w0 - e_winf) / temp2;
	// Imaginary part
	eps_i = (temp1*(e_w0 - e_winf)) / temp2;

	return e_c = eps_r + I1*eps_i;
}

/**********************************************************************/
/* Calculate complex dielectric constant using a semi-empirical model */
/* described in Dobson 1985 for frequency range 1.4-18 GHz with a     */
/* correction for frequency range 0.3 - 1.3 GHz given in Peplinski 1995 */
// Input: EASE-grid2.0 row and column for static and dynamic data, Signal Frequency in Hertz
void calcDielPeplinski(void)
{
	double alpha = 0.65; // empirically determined constant - from Peplinski 1995 p.804
//	double rho_b = 1.2; // bulk density of soil sample in [g/cc] (grams/cubic centimeter)
	double rho_s = 2.66; // specific density of solid soil particles in [g/cm^3]
//	double rho_w = 0.997; // specific density of water in [g/cm^3]
//	double rho_i = 0.917; // specific density of ice in [g/cm^3]
	double e_s; // relative permittivity of solid soil, Equation 22 in Dobson 1985
	double e_i = 3.15; // relative permittivity of inclusions (air, bound water, and free water) from L.Zhang 2003

	double e_p_fw; // real part of the relative dielectric constant of free water
	double e_dp_fw; // imaginary part of the relative dielectric constant of free water
	double e_winf = 4.9; // high-frequency (or optical) limit of e_p_fw from Equation E.16 in Ulaby Vol. III, Appendix E.2
	double e_0 = 8.85418782E-12; // permittivity of free space in [m^-3 kg^-1 s^4 A^2]
	double e_w0; //static dielectric constant of pure water
	double tau_w; // relaxation time of pure water [sec]

	double sigma_eff; // Effective conductivity (dependent on frequency)

	double mvu; // Unfrozen volumetric moisture content (100v/v)
	double mvi; // Ice volumetric content (100v/v)

	double beta_p, beta_dp; // Soil dependent constants
	//double TK; // Soil temperature in [K]
	double e_p, e_dp;

	float sand = Gnd->sand;
	float clay = Gnd->clay;
	float rho_b = Gnd->bulk;
	float Temp = Gnd->soilTemp1;
	float VSM;
	double Freq = Bistatic->Freq;
	long ILayer;

	e_s = (1.01 + 0.44*rho_s)*(1.01 + 0.44*rho_s) - 0.062;

	// Equation 4 in Peplinski 1995
	beta_p = 1.2748 - 0.519*sand - 0.152*clay;
	// Equation 5 in Peplinski 1995
	beta_dp = 1.33797 - 0.603*sand - 0.166*clay;

	/* Relative dielectric constant of free water, given by Debye-type dispersion equation (Ulaby Vol.III, Appendix E.2) */
	// Equation E.19 in Ulaby Vol.III, Appendix E.2
	e_w0 = 88.045 - 0.4147*Temp + 6.295E-4*Temp*Temp + 1.075E-5*Temp*Temp*Temp;
	// Equation E.17 in Ulaby Vol.III, Appnedix E.2
	tau_w = (1.1109E-10 - 3.824E-12*Temp + 6.938E-14*Temp*Temp - 5.096E-16*Temp*Temp*Temp)/(TwoPi);

	// Real part
	// Equation 6 in Peplinski 1995
	e_p_fw = e_winf + (e_w0 - e_winf) / ( 1 + (TwoPi*Freq*tau_w)*(TwoPi*Freq*tau_w));

	if(Freq>1.4E9)
		// Equation 8 in Peplinski 1995
		sigma_eff = -1.645 + 1.939*rho_b - 2.25622*sand + 1.549*clay;
	else
		// Equation 10 in Peplinski 1995
		sigma_eff = 0.0467 + 0.2204*rho_b - 0.4111*sand + 0.6614*clay;


	// Frozen soil - from Zhang 2003. Note that paper used T in K, here we change to deg C  // need to revise...
	//TK = Temp + 273.15;
	//mvu = A * pow(abs(TK-273.2), -B) * rho_b / rho_w;// eq (2) from Zhang 2003
	//mvi = (LocalGnd->m_v - mvu) * (rho_w/rho_i); // eq (3) from Zhang - believed to be corrected.

	mvi = 0;
	for(ILayer=0; ILayer<NLayer; ILayer++){
		VSM = Gnd->VSM[ILayer];
		mvu = VSM;
		// Imaginary part
		// Equation 7 in Peplinski 1995
		e_dp_fw = (TwoPi*Freq*tau_w*(e_w0 - e_winf))/(1+(TwoPi*Freq*tau_w)*(TwoPi*Freq*tau_w))
				+ sigma_eff/(TwoPi*e_0*Freq) * (rho_s - rho_b)/(rho_s*VSM);

		/* Complex dielectric constant */
		// Real part (Equation 2 in Peplinski 1995)
		//epsilon_m_p = (1 + rho_b./rho_s.*(epsilon_s.^alpha-1) + m_v.^beta_p .* epsilon_p_fw.^alpha - m_v).^(1/alpha);
		// updated (6) from Zhang 2003
		e_p = pow(1 + rho_b /rho_s*(pow(e_s,alpha)-1) + pow(mvu,beta_p) * pow(e_p_fw,alpha)
				- VSM + mvi*pow(e_i,alpha) - mvi,1/alpha);

		// Imaginary part (Equation 3 in Peplinski 1995)
		// epsilon_m_dp = (m_v.^beta_dp .* epsilon_dp_fw.^alpha).^(1/alpha);
		// updated from Zhang 2003 (my interpretation ....) also note that this gives a small imag component (~E-16).
		e_dp = newpow(pow(mvu,beta_dp)*newpow(e_dp_fw,alpha), 1/alpha);

		// Correction to the real part if f < 1.4 GHz (Equation 9 in Peplinski 1995)
		if (Freq < 1.4E9)
			e_p = 1.15 * e_p - 0.68;
		Gnd->e_c[ILayer] = e_p - I1*e_dp;
	}
}

/**********************************************************************************/
/*   INPUTS:
 *   Freq:       Frequency (Hz)
 *   VSM:        Volumetric soil moisture (cm3/cm3) [0,1]
 *   clay_ratio: Mass fraction of clay content in soil
 *
 *   Implemented from the following paper:
 *   V. L. Mironov and S. V Fomin, �Temperature dependable microwave
 *   dielectric model for moist soils,� PIERS Proceedings, March, pp. 23-27, 2009.*/
void calcDielMironov(void)
{
	double Freq = Bistatic->Freq;
	float C = Gnd->clay * 100;
	//float Temp = DynData->soilTemp1[rowDy][colDy] - 273.15; // Kelvin to Celsius

	// Mironov's regression expressions based on Curtis, Dobson, and Hallikainen datasets
	double nd, kd, mvt, eps0b, taub, sigb, sigu, eps0u, tauu;
	nd = 1.634 - 0.539e-2 * C + 0.2748e-4 * C * C;     // Eqn 17
	kd = 0.03952 - 0.04038e-2 * C ;                    // Eqn 18
	mvt = 0.02863 + 0.30673e-2 * C ;                   // Eqn 19
	eps0b = 79.8 - 85.4e-2 * C + 32.7e-4 * C * C;      // Eqn 20
	taub = 1.062e-11 + 3.450e-12 * 1e-2 * C ;          // Eqn 21
	sigb = 0.3112 + 0.467e-2 * C ;                     // Eqn 22
	sigu = 0.3631 + 1.217e-2 * C ;                     // Eqn 23
	eps0u = 100 ;                                      // Eqn 24
	tauu = 8.5e-12 ;                                   // Eqn 25

	// Debye relaxation equations for water as a function of frequency
	double eps0 = 8.854e-12; // Vacuum permittivity
	double epsinf = 4.9;
	double epsb_real, epsb_imag, epsu_real, epsu_imag;
	// Section IV - Epn 16
	epsb_real = epsinf + ( (eps0b - epsinf) / (1 + (TwoPi * Freq * taub)*(TwoPi * Freq * taub)) );

	epsb_imag = (eps0b - epsinf) / (1 + (TwoPi * Freq * taub)*(TwoPi * Freq * taub))
	    		* (TwoPi * Freq * taub) + sigb / (TwoPi * eps0 * Freq);

	epsu_real = epsinf + ( (eps0u - epsinf) / (1 + (TwoPi * Freq * tauu)*(TwoPi * Freq * tauu)) );

	epsu_imag = (eps0u - epsinf) / (1 + (TwoPi * Freq * tauu)*(TwoPi * Freq * tauu) )
	    		* (TwoPi * Freq * tauu) + sigu / (TwoPi * eps0 * Freq);

	// Refractive indices - Eqn 14
	double nb, kb, nu, ku;
	nb = 1/sqrt(2) * sqrt( sqrt(epsb_real*epsb_real + epsb_imag*epsb_imag) + epsb_real );
	kb = 1/sqrt(2) * sqrt( sqrt(epsb_real*epsb_real + epsb_imag*epsb_imag) - epsb_real );
	nu = 1/sqrt(2) * sqrt( sqrt(epsu_real*epsu_real + epsu_imag*epsu_imag) + epsu_real );
	ku = 1/sqrt(2) * sqrt( sqrt(epsu_real*epsu_real + epsu_imag*epsu_imag) - epsu_real );

	// n(*) are refractive indices, k(*) are normalized attenuation coefficients
	// m: moist soil
	// d: dry soil
	// b: bound soil water (BSW)
	// u: unbound (free) soil water (FSW)

	double nm, km, e_p, e_dp;//, er_r_real, er_r_imag, tmp;
	long ILayer;
	double VSM;
	
	if(GndModel == MULTI){
		if(SmpModel == POME){
			for(ILayer=0; ILayer<MultiLayer->NSublayer; ILayer++){
				VSM = Gnd->SMP[ILayer];
				if(VSM>mvt){
					nm = nd + (nb - 1) * mvt + (nu - 1) * (VSM - mvt);   // Eqn 12
					km = kd + kb * mvt + ku * (VSM - mvt);               // Eqn 13
				}
				else{
					nm = nd + (nb - 1) * VSM;         //  Eqn 12
					km = kd + kb * VSM;               // Eqn 13
				}
				e_p = nm*nm - km*km;          // Eqn 11
				e_dp = 2 * nm * km;           // Eqn 11

				// Round and Combine the dielectric constant (complex number)
				Gnd->e_c_profile[ILayer] = round(e_p*10)/10 - I1*round(e_dp*10)/10;
			}
		}
		else if(SmpModel == EXPO){
			for(ILayer=0; ILayer<NLayer; ILayer++){
				VSM = Gnd->VSM[ILayer];
				if(VSM>mvt){
					nm = nd + (nb - 1) * mvt + (nu - 1) * (VSM - mvt);   // Eqn 12
					km = kd + kb * mvt + ku * (VSM - mvt);               // Eqn 13
				}
				else{
					nm = nd + (nb - 1) * VSM;         //  Eqn 12
					km = kd + kb * VSM;               // Eqn 13
				}
				e_p = nm*nm - km*km;          // Eqn 11
				e_dp = 2 * nm * km;           // Eqn 11

				// Round and Combine the dielectric constant (complex number)
				Gnd->e_c[ILayer] = round(e_p*10)/10 - I1*round(e_dp*10)/10;
			}
		}
	}
	else if(GndModel == SINGLE){
		VSM = Gnd->VSM[0];
		if(VSM>mvt){
			nm = nd + (nb - 1) * mvt + (nu - 1) * (VSM - mvt);   // Eqn 12
			km = kd + kb * mvt + ku * (VSM - mvt);               // Eqn 13
		}
		else{
			nm = nd + (nb - 1) * VSM;         //  Eqn 12
			km = kd + kb * VSM;               // Eqn 13
		}
		e_p = nm*nm - km*km;          // Eqn 11
		e_dp = 2 * nm * km;           // Eqn 11

		// Round and Combine the dielectric constant (complex number)
		Gnd->e_c[0] = round(e_p*10)/10 - I1*round(e_dp*10)/10;
	}
}

// freq in Hz
double complex calcDielUlabyElRayesMg(double freq, double mg)
{
	double S = 8.5; // salinity
	double sigma_i, v_fw, v_bw, eps_r;
	double eps_w_r, eps_w_i, eps_b_r, eps_b_i;
	double temp1, temp2;
	double e_c_r, e_c_i;
	double complex e_c;

	freq *= 1E-9; // Hz to GHz

	// free water in leaves
	sigma_i = 0.17*S - 0.0013 * S*S; 
	
	temp1 = freq/18;
	temp2 = temp1 * temp1;
	eps_w_r = 4.9 + 74.4 / ( 1 + temp2);

	eps_w_i =  74.4 * temp1 /( 1 + temp2) + 18*sigma_i /freq;

	// bound water in leaves

	temp1 = freq/0.36;
	temp2 = 1 + sqrt(temp1);

	eps_b_r = 2.9 + 55*temp2 / ( temp2*temp2 + temp1 );

	eps_b_i = 55*(temp2 - 1) / ( temp2*temp2 + temp1 );

	// empirical fits
	temp1 = mg*mg;
	v_fw = mg *( 0.55 * mg - 0.076);
	v_bw = 4.64 *temp1 /(1 + 7.36 * temp1); 

	eps_r = 1.7 - 0.74 *mg + 6.16 * temp1;

	e_c_r = eps_r + v_fw * eps_w_r + v_bw *eps_b_r; // real part
	e_c_i = v_fw * eps_w_i + v_bw *eps_b_i; // imaginary part

	return e_c = e_c_r + I1*e_c_i;
}

// freq in Hz
double complex calcDielUlabyElRayesMv(double freq, double mv)
{
	double S = 8.5; // salinity
	double sigma_i, v_fw, v_bw, eps_r;
	double eps_w_r, eps_w_i, eps_b_r, eps_b_i;
	double temp1, temp2;
	double e_c_r, e_c_i;
	double complex e_c;

	freq *= 1E-9; // Hz to GHz

	// free water in leaves
	sigma_i = 0.17*S - 0.0013 * S*S; 
	
	temp1 = freq/18;
	temp2 = temp1 * temp1;
	eps_w_r = 4.9 + 74.4 / ( 1 + temp2);

	eps_w_i =  74.4 * temp1 /( 1 + temp2) + 18*sigma_i /freq;

	// bound water in leaves

	temp1 = freq/0.36;
	temp2 = 1 + sqrt(temp1);

	eps_b_r = 2.9 + 55*temp2 / ( temp2*temp2 + temp1 );

	eps_b_i = 55*(temp2 - 1) / ( temp2*temp2 + temp1 );

	// empirical fits
	temp1 = mv*mv;
	v_fw = mv *( 0.82 * mv + 0.166);
	v_bw = 31.4 *temp1 /(1 + 59.5 * temp1); 

	eps_r = 1.7 + 3.2 *mv + 6.5 * temp1;

	e_c_r = eps_r + v_fw * eps_w_r + v_bw *eps_b_r; // real part
	e_c_i = v_fw * eps_w_i + v_bw *eps_b_i; // imaginary part

	return e_c = e_c_r + I1*e_c_i;
}

// Two-phase mixture dielectric mixing model using Tinga-Voss-Blossey (TVB) formulas
// e_i: dielectric constant of inclusion material
// vi: inclusion volume fraction, vi = V2(inclusion) / V1(host)
// type: 0 (circular disk), 1 (needle), 2 (sphere)
// V1 = 4/3 * Pi * a1 * b1 * c1
// V2 = 4/3 * Pi * a2 * b2 * c2
double complex calcTwoPhaseMixture(long type, double complex e_i, double vi)
{
	double complex e_m, e_h = 1; // host is air
	if(type == DISK){
		e_m = e_h + vi/3 * (e_i - e_h) * (2*e_i*(1-vi) + e_h*(1+2*vi)) / (vi*e_h + (1-vi)*e_i);
	}
	else if(type == CYLINDER){
		e_m = e_h + vi/3*(e_i-e_h) * (e_h*(5+vi) + (1-vi)*e_i) / (e_h*(1+vi) + e_i*(1-vi));
	}
	else if(type == SPHERE){
		e_m = e_h + 3*vi*e_h*(e_i-e_h) / ((2*e_h+e_i) - vi*(e_i-e_h));
	}
	return e_m;
}

/* Construct multi-layered dielectric ground structure using logistic regression */
void calcDielProfileLogistic(void)
{
	double complex e_c1, e_c2;
	double VSM1, VSM2;
	double sL_depth; // Standard variation of layer boundaries : 10%
	double zz[NLayer];
	long ILayer, ISublayer;

	// Initialization
	e_c1 = EPS_DIEL_AIR;
	VSM1 = 0;
	for(ILayer=1; ILayer<NLayer; ILayer++){
		zz[ILayer] = 0;
	}
	for(ISublayer=0; ISublayer<MultiLayer->NSublayer; ISublayer++){
		Gnd->e_c_profile[ISublayer] = e_c1;
		Gnd->SMP[ISublayer] = VSM1;
	}
	// Generate Dielectric Profile
	for(ILayer=0; ILayer<NLayer; ILayer++){
		sL_depth = 0.1 * GndData[ILayer].depth;
		if(ILayer == 0){
			zz[ILayer] = MultiLayer->zA;
		}
		else{
			zz[ILayer] = zz[ILayer-1] + GndData[ILayer-1].layer_thickness;
		}
		e_c2 = Gnd->e_c[ILayer];
		VSM2 = Gnd->VSM[ILayer];
		for(ISublayer=0; ISublayer<MultiLayer->NSublayer; ISublayer++){
			Gnd->e_c_profile[ISublayer] = Gnd->e_c_profile[ISublayer] + (e_c2 - e_c1) / (1 + exp(-2.197 * (MultiLayer->z[ISublayer] - zz[ILayer]) / sL_depth));
			Gnd->SMP[ISublayer] = Gnd->SMP[ISublayer] + (VSM2 - VSM1) / (1 + exp(-2.197 * (MultiLayer->z[ISublayer] - zz[ILayer]) / sL_depth));
		}
		e_c1 = e_c2;
		VSM1 = VSM2;
	}
	// To exclude roughness
	for(ISublayer=0; ISublayer<MultiLayer->NSublayer; ISublayer++){
		if(MultiLayer->z[ISublayer] <= MultiLayer->zA){
			Gnd->e_c_profile[ISublayer] = EPS_DIEL_AIR;
			Gnd->SMP[ISublayer] = 0;
		}
	}
}

/* Solve Lagrange multipler for POME model - wet case */
double SMP_POME_L2solver_wet(double sur, double avg, double bott)
{
	double temp = 0;
    double dat1, dat2, temp2, f1, f2;

    if(avg<((sur+bott)*0.5)){
        dat1 = -50;
        dat2 = -0.00001;
	}
    else{
        dat1 = 0.00001;
        dat2 = 50;
	}

    while(fabs(dat2-dat1)>0.0005){
		f1 = (exp(dat1*sur)-exp(dat1*bott))/(sur*exp(dat1*sur)-bott*exp(dat1*bott)-avg*exp(dat1*sur)+avg*exp(dat1*bott)) - dat1;
        f2 = (exp(dat2*sur)-exp(dat2*bott))/(sur*exp(dat2*sur)-bott*exp(dat2*bott)-avg*exp(dat2*sur)+avg*exp(dat2*bott)) - dat2;

        if(f1*f2<=0){
            temp = dat1;
            dat1 = (dat1+dat2)/2;
		}
        else{
            temp2 = dat2;
            if(temp == 0)
                dat2 = dat1;
            else
                dat2 = temp;
            dat1 = (dat1+temp2)/2;
		}
	}
	return dat1;
}

/* Solve Lagrange multipler for POME model - dry case */
double SMP_POME_L2solver_dry(double sur, double avg, double bott)
{
	double temp = 0;
    double dat1, dat2, temp2, f1, f2;

    if(avg<((sur+bott)*0.5)){
        dat1 = -50;
        dat2 = -0.00001;
	}
    else{
        dat1 = 0.00001;
        dat2 = 50;
	}

    while(fabs(dat2-dat1)>0.0005){
		f1 = (exp(dat1*bott)-exp(dat1*sur))/(bott*exp(dat1*bott)-sur*exp(dat1*sur)-avg*exp(dat1*bott)+avg*exp(dat1*sur)) - dat1;
		f2 = (exp(dat2*bott)-exp(dat2*sur))/(bott*exp(dat2*bott)-sur*exp(dat2*sur)-avg*exp(dat2*bott)+avg*exp(dat2*sur)) - dat2;

        if(f1*f2<=0){
            temp = dat1;
            dat1 = (dat1+dat2)/2;
		}
        else{
            temp2 = dat2;
            if(temp == 0)
                dat2 = dat1;
            else
                dat2 = temp;
            dat1 = (dat1+temp2)/2;
		}
	}
	return dat1;
}

int SMP_POME_wet_case(double sur, double avg, double bott, double D, double delz, double *profile)
{
	int res;
	double dat, dao1;
	long iLayer, nLayer;

	nLayer = (long) round(D/delz) + 1;

    // Solve for Lagrange multipliers
    dat = SMP_POME_L2solver_wet(sur, avg, bott);
	dao1 = log(dat/(exp(dat*sur)-exp(dat*bott))) + 1;

    // Calculate soil moisture profile
    for(iLayer=0; iLayer<nLayer; iLayer++){
        profile[iLayer] = log(exp(dat*sur)-(dat*exp(1-dao1)*(iLayer*delz)/D))/dat;
		if(isnan(profile[iLayer]) || isinf(profile[iLayer])){
			res = POME_FAILURE;
			return res;
		}
	}
	res = POME_WET;

	return res;
}

int SMP_POME_dry_case(double sur, double avg, double bott, double D, double delz, double *profile)
{
	int res;
	double dat, dao1;
	long iLayer, nLayer;

	nLayer = (long) round(D/delz) + 1;

    // Solve for Lagrange multipliers
    dat = SMP_POME_L2solver_dry(sur, avg, bott);    
	dao1 = log(dat/(exp(dat*bott)-exp(dat*sur))) + 1;

    // Calculate soil moisture profile
    for(iLayer=0; iLayer<nLayer; iLayer++){
        profile[iLayer] = log(exp(dat*sur)+(dat*exp(1-dao1)*(iLayer*delz)/D))/dat;
		if(isnan(profile[iLayer]) || isinf(profile[iLayer])){
			res = POME_FAILURE;
			return res;
		}
	}
	res = POME_DRY;

	return res;
}

int SMP_POME_run_case(double sur, double avg, double bott, double infl_val, double D, double delz, double z_infl, double *profile)
{
	int res;
	double upp_avg, upp_bott, low_avg, low_sur, low_depth;
	long iLayer, nLayer_upp, nLayer_low;

	low_depth = D-z_infl;

	nLayer_upp = (long) round(z_infl/delz) + 1;
	nLayer_low = (long) round(low_depth/delz) + 1;

	double profile_upp[nLayer_upp], profile_low[nLayer_low];

    // assumes the average SM is uniformly distributed throughout the column
    upp_avg = avg * (D/z_infl);
    low_avg = avg * (1- (D/z_infl));

    low_sur = infl_val;
    upp_bott = infl_val;
    //printf("%lf %lf %lf %lf %lf %lf %lf\n", sur, avg, bott, upp_avg, low_avg, upp_bott, low_sur);
    if(sur <= avg && avg >= bott){
		// case I
        res = SMP_POME_dry_case(sur, upp_avg, upp_bott, z_infl, delz, profile_upp);
		if(res == POME_FAILURE)
			return res;
        res = SMP_POME_wet_case(low_sur, low_avg, bott, low_depth, delz, profile_low);
		if(res == POME_FAILURE)
			return res;
		for(iLayer=0; iLayer<nLayer_upp; iLayer++){
			profile[iLayer] = profile_upp[iLayer];
		}
		for(iLayer=0; iLayer<nLayer_low-1; iLayer++){
			profile[nLayer_upp+iLayer] = profile_low[iLayer+1];
		}
	}
	else if(sur >= avg && avg <= bott){
		// case II
        res = SMP_POME_wet_case(sur, upp_avg, upp_bott, z_infl, delz, profile_upp);
		if(res == POME_FAILURE)
			return res;
        res = SMP_POME_dry_case(low_sur, low_avg, bott, low_depth, delz, profile_low);
		if(res == POME_FAILURE)
			return res;
		for(iLayer=0; iLayer<nLayer_upp; iLayer++){
			profile[iLayer] = profile_upp[iLayer];
		}
		for(iLayer=0; iLayer<nLayer_low-1; iLayer++){
			profile[nLayer_upp+iLayer] = profile_low[iLayer+1];
		}
	}
	else{
		res = POME_FAILURE;
		return res;
	}

	res = POME_DYNAMIC;
	return res;
}

int SMP_POME_dynamic_case(double sur, double avg, double bott, double D, double delz, double z_infl, double fc, double resid, double sat, double *profile)
{
	int res;
    double infl_init, infl_est, infl_min, infl_step = 0.01;
	double trys_sum, trys_avg, errors, err_min = DBL_MAX;
	long iEst, nEst, iLayer, nLayer, iMin;

	nLayer = floor(D/delz) + 1;

	double trys[nLayer];

	infl_init = (fc - resid) / (sat - resid);

	infl_min = infl_init;

	// Convex ) shape
    if(sur <= avg && avg >= bott){
		nEst = floor((SOILMOISTMAX_EFF - MAX(sur,bott)) / infl_step) + 1;
		for(iEst=0; iEst<nEst; iEst++){
			infl_est = MAX(sur,bott) + (double) iEst*infl_step;
			res = SMP_POME_run_case(sur, avg, bott, infl_est, D, delz, z_infl, trys);
			if(res == POME_DYNAMIC){
				// average
				trys_sum = 0;
				for(iLayer=0; iLayer<nLayer; iLayer++){
					trys_sum += trys[iLayer];
				}
				trys_avg = (double) trys_sum/nLayer;
				errors = fabs(trys_avg-avg);
				if(errors<err_min){
					err_min = errors;
					iMin = iEst;
				}
			}
		}
		infl_min = MAX(sur,bott) + (double) iMin*infl_step;
	}
	// Concave ( shape
    else if(sur >= avg && avg <= bott){
		nEst = floor((MIN(sur,bott) - SOILMOISTMIN_EFF) / infl_step) + 1;
		for(iEst=0; iEst<nEst; iEst++){
			infl_est = SOILMOISTMIN_EFF + (double) iEst*infl_step;
			res = SMP_POME_run_case(sur, avg, bott, infl_est, D, delz, z_infl, trys);
			if(res == POME_DYNAMIC){
				// average
				trys_sum = 0;
				for(iLayer=0; iLayer<nLayer; iLayer++){
					trys_sum += trys[iLayer];
				}
				trys_avg = (double) trys_sum/nLayer;
				errors = fabs(trys_avg-avg);
				if(errors<err_min){
					err_min = errors;
					iMin = iEst;
				}
			}
		}
		infl_min = SOILMOISTMIN_EFF + (double) iMin*infl_step;
	}
	
    res = SMP_POME_run_case(sur, infl_min, bott, infl_min, D, delz, z_infl, profile);

	MultiLayer->infl_val = infl_min * (fc-resid) + resid;

	return res;
}

int SMP_POME_main(double sur, double bott, double avg)
{
	int res;
	double D, delZ;
	MultiLayer->infl_val = 0;
	D = MultiLayer->D;
	delZ = MultiLayer->delZ;

	// Apply POME model to estimate soil moisture profile
    if(sur < avg && avg < bott){
        // dry case
        res = SMP_POME_dry_case(sur, avg, bott, D, delZ, MultiLayer->SMP);
	}
    else if(sur > avg && avg > bott){
        // wet case
        res = SMP_POME_wet_case(sur, avg, bott, D, delZ, MultiLayer->SMP);
	}
    else if (((sur <= avg) && (avg >= bott)) || ((sur >= avg) && (avg <= bott))){
        // dynamic case
        res = SMP_POME_dynamic_case(sur, avg, bott, D, delZ, MultiLayer->z_infl, Gnd->fc, Gnd->resid, Gnd->poro, MultiLayer->SMP);
	}
    else{
		res = POME_FAILURE;
	}
	return res;
}

/************************************************************************************/
/* Calculate penetration and skin depth based on frequency and relative complex     */
/* dielectric constant.                                                             */
/* This calculation is based on Eq.11.43-11.45 (p.847) in Ulaby Book, Vol.II, 1986. */
void calcDepth(struct SoOpType *S, struct SpecularType *Sp)
{
//	double attenuation; // field attenuation coefficient

	// Equation 11.44
//	attenuation = TwoPi/S->Wavelength * cabs(cimag(csqrt(Sp->LocalGnd[0].e_c)));
	// attenuation = pi./wavelength.*(imag(e_c))./(sqrt((real(e_c))));

//	Sp->depthS = 1/attenuation;

	// Equation 11.43
//	Sp->depthP = cabs(1/(2*attenuation));
// Sm.depthP = Transmitter.Wavelength * sqrt(Sm.e_p) / (TwoPi*Sm.e_dp);

	//double e, del;
	//e = sqrt(Sm.e_p*Sm.e_p + Sm.e_dp*Sm.e_dp);
	//del = atan2(Sm.e_dp,Sm.e_p);
	//Sm.depthP = cabs(Transmitter.Wavelength/(2*TwoPi*sqrt(e)*sin(del/2)));
}

/* Generates the selected dielectric profiles if the ground structure is multi-layered */
void calcMLDielProfile(void)
{
	double sum, avg;
	double sur_eff, bott_eff, avg_eff;
	long iLayer;
	int res;

	if(SmpModel == POME){
		// Get soil water characteristic based on soil fractions
		LookupSoiltype();

		// Calculate mean SM
		sum = 0;
		for(iLayer=0; iLayer<NLayer-1; iLayer++)
			sum += (Gnd->VSM[iLayer] + Gnd->VSM[iLayer+1]) * (GndData[iLayer+1].depth - GndData[iLayer].depth);
		avg = sum / 2 / (GndData[NLayer-1].depth - GndData[0].depth);

		// VSM to effective SM
		sur_eff = (Gnd->VSM[0] - Gnd->resid) / (Gnd->poro - Gnd->resid);
		bott_eff = (Gnd->VSM[NLayer-1] - Gnd->resid) / (Gnd->poro - Gnd->resid);
		avg_eff = (avg - Gnd->resid) / (Gnd->poro - Gnd->resid);

		MultiLayer->sur = sur_eff;
		MultiLayer->bott = bott_eff;
		MultiLayer->avg = avg_eff;
		
		res = SMP_POME_main(sur_eff, bott_eff, avg_eff);
		if(res == POME_FAILURE){
			printf(">> POME_FAILURE\n");
			exit(1);
		}

		// Effective SM to VSM
		for(iLayer=0; iLayer<MultiLayer->NSublayer_POME; iLayer++){
			Gnd->SMP[iLayer+MultiLayer->NSublayer_extend] = MultiLayer->SMP[iLayer] * (Gnd->poro - Gnd->resid) + Gnd->resid;
		}
		for(iLayer=0; iLayer<MultiLayer->NSublayer_extend; iLayer++){
			Gnd->SMP[iLayer] = Gnd->SMP[MultiLayer->NSublayer_extend];
		}
	}
	calcDielMironov();

	if(SmpModel == EXPO){
		if(MultiLayer->FitFcn == LOGISTIC)
			calcDielProfileLogistic();
		else if(MultiLayer->FitFcn == POLY2ND)
			polyfitML(2);
		else if(MultiLayer->FitFcn == POLY3RD)
			polyfitML(3);
	}
#if 0
	FILE *outFile1, *outFile2;
	long ISublayer;
	long IData;

	outFile1 = FileOpen(SimPath, "diel_255_Logistic_r.out", "w");
	outFile2 = FileOpen(SimPath, "diel_255_Logistic_i.out", "w");

	for(IData=0; IData < NData; IData++){
		for(ISublayer = 0; ISublayer < MultiLayer->NSublayer; ISublayer++){
			fprintf(outFile1, "%lf ", creal(Gnd->e_c_profile[ISublayer]));
			fprintf(outFile2, "%lf ", cimag(Gnd->e_c_profile[ISublayer]));
		}
		fprintf(outFile1, "\n");
		fprintf(outFile2, "\n");
	}
	fclose(outFile1);
	fclose(outFile2);
#endif
}

/* Calculates the equivalent reflection coeff. of the rough, multi-layered ground for the chosen ones of four dielectric profiles */
/* Reflection response of isotropic or birefringent multilayer structure
Ref: Sophocles J. Orfanidis - 1999-2008 - www.ece.rutgers.edu/~orfanidi/ewa */
void calcRcMulti(void)
{
	long NSublayer;

	NSublayer = MultiLayer->NSublayer;

	double Lz, th, expQZSGMI2;
	double complex n[NSublayer+1];
	double complex Nsin2H, cH[NSublayer+1], nT[NSublayer+1], rH[NSublayer], L_H[NSublayer-1];
	double complex Nsin2V, cV[NSublayer+1], nTinv[NSublayer+1], rV[NSublayer], L_V[NSublayer-1];
	double complex RcH, deltaH, zH;
	double complex RcV, deltaV, zV;
	long Nn, ISublayer;

	Nn = NSublayer + 1;
	Lz = MultiLayer->delZ / Bistatic->Wavelength; // complex optical length in units of wavelength [m]
	th = Bistatic->th;

	// Air - isotropic
	n[0] = sqrte(EPS_DIEL_AIR);
	// Dielectric Profile : isotropic
	if(SmpModel == EXPO){
		for(ISublayer = 1; ISublayer<NSublayer; ISublayer++){
			n[ISublayer] = sqrte(Gnd->e_c_profile[ISublayer]);
		}
		// Soil - isotropic
		n[ISublayer] = sqrte(Gnd->e_c_profile[ISublayer-1]);
	}
	else if(SmpModel == POME){
		for(ISublayer = 0; ISublayer<NSublayer; ISublayer++){
			n[ISublayer+1] = sqrte(Gnd->e_c_profile[ISublayer]);
		}
	}

	/* .. Reflection Coeffficient .. */
	Nsin2H = n[0] * n[0] * sin(th) * sin(th);
	Nsin2V = n[0]*n[0]*n[0]*n[0]*sin(th)*sin(th) / (n[0]*n[0]*cos(th)*cos(th) + n[0]*n[0]*sin(th)*sin(th));
	cH[0] = sqrte(1 - Nsin2H / (n[0]*n[0]));
	cV[0] = sqrte(1 - Nsin2V / (n[0]*n[0]));
	nT[0] = n[0] * cH[0];
	nTinv[0] = cV[0] / n[0];
	for(ISublayer = 1; ISublayer<Nn-1; ISublayer++){
		// Coefficient ci, or cos(th(i)) in isotropic case
		cH[ISublayer] = sqrte(1 - Nsin2H / (n[ISublayer]*n[ISublayer]));
		cV[ISublayer] = sqrte(1 - Nsin2V / (n[ISublayer]*n[ISublayer]));
		// Transverse refractive indices
		nT[ISublayer] = n[ISublayer] * cH[ISublayer];
		nTinv[ISublayer] = cV[ISublayer] / n[ISublayer];
		// Refractive indices to reflection coefficients of M-layer structure
		rH[ISublayer-1] = (nT[ISublayer-1]-nT[ISublayer]) / (nT[ISublayer-1] + nT[ISublayer]);
		rV[ISublayer-1] = (nTinv[ISublayer]-nTinv[ISublayer-1]) / (nTinv[ISublayer-1] + nTinv[ISublayer]);
		// Polarization-dependent optical lengths
		L_H[ISublayer-1] = Lz * n[ISublayer] * cH[ISublayer];
		L_V[ISublayer-1] = Lz * n[ISublayer] * cV[ISublayer];
	}
	cH[ISublayer] = sqrte(1 - Nsin2H / (n[ISublayer]*n[ISublayer]));
	cV[ISublayer] = sqrte(1 - Nsin2V / (n[ISublayer]*n[ISublayer]));
	nT[ISublayer] = n[ISublayer] * cH[ISublayer];
	nTinv[ISublayer] = cV[ISublayer] / n[ISublayer];
	rH[ISublayer-1] = (nT[ISublayer-1]-nT[ISublayer]) / (nT[ISublayer-1] + nT[ISublayer]);
	rV[ISublayer-1] = (nTinv[ISublayer]-nTinv[ISublayer-1]) / (nTinv[ISublayer-1] + nTinv[ISublayer]);

	// Initialize Reflection Coefficient at right-most interface
	RcH = rH[ISublayer-1]; // lambda = 1
	RcV = rV[ISublayer-1]; // lambda = 1
	
	// Forward layer recursion
	for(ISublayer = Nn-3; ISublayer>=0; ISublayer--){
		// Phase thickness in i-th layer
		deltaH = TwoPi * L_H[ISublayer]; // lambda = 1
		deltaV = TwoPi * L_V[ISublayer]; // lambda = 1
		zH = cexp(-2 * I1 * deltaH);
		zV = cexp(-2 * I1 * deltaV);
		RcH = (rH[ISublayer] + RcH * zH) / (1 + rH[ISublayer] * RcH * zH);
		RcV = (rV[ISublayer] + RcV * zV) / (1 + rV[ISublayer] * RcV * zV);
	}

	// .. Apply surface roughness .. //
	expQZSGMI2 = exp(-Gnd->h*cos(th)*cos(th)/2.0);
	RcH *= expQZSGMI2;
	RcV *= expQZSGMI2;

	Gnd->RcH = RcH;
	Gnd->RcV = RcV;

	// .. Determine penetration depth if requested .. //
	if(GndData->flagPenDep){
		// .. Get index of penetration depth from entire profile .. //
		long M = NSublayer-1, flagH = TRUE, flagV = TRUE;
		long ind_depthH, ind_depthV;
		double TT_H, TT_V; // transmissivity at the right side of the boundary
		double complex MatchingH[2][2], MatchingV[2][2], temp[2][2], factor;
		double complex ProgH[2][2] = {{1, 0}, {0, 1}}, ProgV[2][2] = {{1, 0}, {0, 1}};
		double complex MP_H[2][2] = {{1, 0}, {0, 1}}, MP_V[2][2] = {{1, 0}, {0, 1}}; 
		double complex invMP_H[2][2], invMP_V[2][2];
		double cc_H, cc_V, nn_H, nn_V;
		// Backward layer recursion
		for(ISublayer = 0; ISublayer<M; ISublayer++){
			factor = 1 / (1 + rH[ISublayer]);
			MatchingH[0][0] = factor;               MatchingH[0][1] = factor*rH[ISublayer];
			MatchingH[1][0] = factor*rH[ISublayer]; MatchingH[1][1] = factor;
			factor = 1 / (1 + rV[ISublayer]);
			MatchingV[0][0] = factor;               MatchingV[0][1] = factor*rV[ISublayer];
			MatchingV[1][0] = factor*rV[ISublayer]; MatchingV[1][1] = factor;
			if(ISublayer > 0){
				// Phase thickness in i-th layer
				deltaH = TwoPi * L_H[ISublayer]; // lambda = 1
				deltaV = TwoPi * L_V[ISublayer]; // lambda = 1
				ProgH[0][0] = cexp(I1 * deltaH); 
				ProgH[1][1] = cexp(-I1 * deltaH); 
				ProgV[0][0] = cexp(I1 * deltaV); 
				ProgV[1][1] = cexp(-I1 * deltaV);
			}
			M2xM2_complex(MP_H, ProgH, temp);  M2xM2_complex(temp, MatchingH, MP_H);
			M2xM2_complex(MP_V, ProgV, temp);  M2xM2_complex(temp, MatchingV, MP_V);
			MINV2_complex(MP_H, invMP_H);
			MINV2_complex(MP_V, invMP_V);
			factor = cabs(invMP_H[0][0] + invMP_H[0][1] * RcH);
			cc_H = creal(cH[ISublayer]) / cos(th);
			nn_H = creal(n[ISublayer+1]) / creal(n[0]);
			TT_H = factor * factor * cc_H * nn_H; // transmissivity
			if(TT_H<exp(-1) && flagH){
				ind_depthH = ISublayer;
				flagH = FALSE;
			}
			factor = cabs(invMP_V[0][0] + invMP_V[0][1] * RcV);
			cc_V = creal(cV[ISublayer]) / cos(th);
			nn_V = creal(n[ISublayer+1]) / creal(n[0]);
			TT_V = factor * factor * cc_V * nn_V; // transmissivity
			if(TT_V<exp(-1) && flagV){
				ind_depthV = ISublayer;
				flagV = FALSE;
			}
			if(!(flagH || flagV)){
				//printf("Done (fH,fV) = (%ld, %ld) Isublayer = %ld\n", flagH, flagV, ISublayer);
				break;
			}
		}
		if(flagH && flagV){
			ind_depthH = NSublayer - 1;
			ind_depthV = NSublayer - 1;
		}
		else{
			//printf("(fH,fV) = (%ld, %ld) Isublayer = %ld, ind_depthH = %ld, V= %ld, NSublayer = %ld\n", flagH, flagV, ISublayer, ind_depthH, ind_depthV, NSublayer);
		}
		if(SmpModel == EXPO){
			Gnd->pdh = MultiLayer->z[ind_depthH] - MultiLayer->zA;
			Gnd->pdv = MultiLayer->z[ind_depthV] - MultiLayer->zA;
		}
		else if(SmpModel == POME){
			Gnd->pdh = MultiLayer->z[ind_depthH];
			Gnd->pdv = MultiLayer->z[ind_depthV];
		}
	}
#if 0
	FILE *outFile;
	long i;
	outFile = FileOpen(SimPath, "multidiel_r.42", "w");
	for(i=0; i<Nn-2; i++){
		fprintf(outFile, "%lf %lf %lf %lf %lf %lf %lf %lf %lf\n", creal(n[i]), creal(cH[i]), creal(cV[i]), creal(nT[i]), creal(nTinv[i]),
		 creal(rH[i]), creal(rV[i]), creal(L_H[i]), creal(L_V[i]));
	}
	fprintf(outFile, "%lf %lf %lf %lf %lf %lf %lf\n", creal(n[i]), creal(cH[i]), creal(cV[i]), creal(nT[i]), creal(nTinv[i]), creal(rH[i]), creal(rV[i]));
	i++;
	fprintf(outFile, "%lf %lf %lf %lf %lf\n", creal(n[i]), creal(cH[i]), creal(cV[i]), creal(nT[i]), creal(nTinv[i]));
	fclose(outFile);

	outFile = FileOpen(SimPath, "multidiel_i.42", "w");
	for(i=0; i<Nn-2; i++){
		fprintf(outFile, "%lf %lf %lf %lf %lf %lf %lf %lf %lf\n", cimag(n[i]), cimag(cH[i]), cimag(cV[i]), cimag(nT[i]), cimag(nTinv[i]),
		 cimag(rH[i]), cimag(rV[i]), cimag(L_H[i]), cimag(L_V[i]));
	}
	fprintf(outFile, "%lf %lf %lf %lf %lf %lf %lf\n", cimag(n[i]), cimag(cH[i]), cimag(cV[i]), cimag(nT[i]), cimag(nTinv[i]), cimag(rH[i]), cimag(rV[i]));
	i++;
	fprintf(outFile, "%lf %lf %lf %lf %lf\n", cimag(n[i]), cimag(cH[i]), cimag(cV[i]), cimag(nT[i]), cimag(nTinv[i]));
	fclose(outFile);
#endif
}

/* Calculates the equivalent reflection coeff. of the rough ground from the Fresnel refleciton coeff. of an avg. flat surface.*/
void calcRcSingle(void)
{
	double sti, cti, qzsgmi2;
	double complex esqi, RcHF, RcVF;

	sti = sin(Bistatic->th);
	cti = cos(Bistatic->th);

	esqi = csqrt(Gnd->e_c[0] - sti*sti);

	RcHF = (cti - esqi) / (cti + esqi);
	RcVF = (esqi - Gnd->e_c[0]*cti) / (Gnd->e_c[0]*cti + esqi);

	qzsgmi2 = Gnd->h * cti * cti / 2.0;

	Gnd->RcH = RcHF * exp(-qzsgmi2);
	Gnd->RcV = RcVF * exp(-qzsgmi2);
}

/* Calculates scattering amplitudes of a thin elliptic dielectric disk
 with respect to prime coordinates. The major radius is along the x axis. */
void thin_eDisc(double TIP1, double PIP1, double TSP1, double PSP1, double FHZ, double T, double A, double B, double complex EPSDC, double complex *FP1)
{
	double AK0;
	AK0 = TwoPi * FHZ / LIGHTSPEED;

	// Define constants
	double complex DCNST1, D3;
	double VOL;
	DCNST1 = (AK0*AK0) * (EPSDC - 1.0) / (4.0 * Pi);
	VOL = Pi * (A * B) * T;
	D3 = (EPSDC - 1.0) / EPSDC;

	// Define angle
	double PHPD;
	PHPD = PIP1 - PSP1 ;

	// Define trigonometric quantities
	double CTIP, CTSP, CPIP, CPSP, CPHPD, STIP, STSP, SPIP, SPSP, SPHPD;
	CTIP = cos(TIP1) ;
	CTSP = cos(TSP1) ;
	CPIP = cos(PIP1) ;
	CPSP = cos(PSP1) ;
	CPHPD = cos(PHPD) ;
	STIP = sin(TIP1) ;
	STSP = sin(TSP1) ;
	SPIP = sin(PIP1) ;
	SPSP = sin(PSP1) ;
	SPHPD = sin(PHPD) ;

	// Calculate dot products
	double HSHI, HSVI, VSHI, VSVI;
	HSHI = -CPHPD ;
	HSVI = CTIP * SPHPD ;
	VSHI = CTSP * SPHPD ;
	VSVI = CPHPD * CTIP * CTSP + STIP * STSP ;

	// Calculate the bessel function which results from the integration (making use of BESC1 subroutine)
	double ALPHX, ALPHY, A1, ARG, BES;
	double complex DCNST;
	ALPHX = -STIP * CPIP - STSP * CPSP ;
	ALPHY = -STIP * SPIP - STSP * SPSP ;
	A1 = (ALPHX * A)*(ALPHX * A)  + (ALPHY * B)*(ALPHY * B) ;
	ARG = AK0 * sqrt(A1) ;
	// BES = BESC1(ARG) ;
	int nm;
	double complex cj[2], cy[2], cjp[2], cyp[2];
	if(ARG == 0){
		ARG = 1e-5 ;
		cbessjyna(1, ARG, &nm, &cj[0], &cy[0], &cjp[0], &cyp[0]);
		BES =  cj[1] / ARG ;
	}
	else{
		cbessjyna(1, ARG, &nm, &cj[0], &cy[0], &cjp[0], &cyp[0]);
		BES =  cj[1] / ARG ;
	}
	
	DCNST = DCNST1 * VOL * 2.0 * BES ;

	// CALCULATE THE SCATTERING AMPLITUDES
	FP1[0] = DCNST * HSHI ;
	FP1[1] = DCNST * HSVI ;
	FP1[2] = DCNST * VSHI ;
	FP1[3] = DCNST * (VSVI - D3 * STIP * STSP) ;
}

/* function eDisc 
%
%   The bistatic scattering amplitude from a lossy dielectric disc is
%   calculated. The phase center is at the bottom of the disc. Both
%   thin and thick routines are used. 
%
%   F = eDisc(TIN, PIN, TS, PS, TH, PH, FHZ, T, A, B, EPSDC)
%
%   INPUTS:
%   TIN,PIN = INCIDENT ANGLES (RAD)
%   TS,PS = SCATTERED ANGLES (RAD)
%   TH,PH = ROTATION ANGLES (RAD)
%   FHZ = FRQUENCY (HZ)
%   RAD = RADIUS OF CYLINDER (M)
%   L = LENGTH OF CYLINDER (M)
%   EPS = RELATIVE DIELECTRIC CONSTANT
%   F = BISTATIC SCATTERING AMPLITUDES
%   F(1) = FHH, F(2) = FVH, F(3) = FHV, F(4) = FVV
%
%   See also calcPropagation, eCylinder.

%   Copyright © 2017-2018 Mehmet Kurum, Orhan Eroglu, Dylan R. Boyd.
%   Adapted from George Washington University vegetation scattering models

%   This program is free software: You can redistribute it and/or 
%   modify it under the terms of the GNU General Public License as 
%   published by the Free Software Foundation, either version 3 of the 
%   License, or (at your option) any later version.

%   Version: 1.0.0
*/
void eDisc(double TH, double PH, struct VegModelType *V, double complex *F)
{
	/*
%% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
%  Calculates the scattering amplitudes with respect to laboratory
%  coordinates by transforming from prime coordinates.
% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
	double TIN = V->tin;
	double PIN = V->pin;
	double TS = Pi - TIN;
	double PS = Pi + PIN;
	double FHZ = V->freq;
	double T = V->thickness;
	double A = V->semiMajorx;
	double B = V->semiMinorx;
	double complex EPSDC = V->epsilon;
	double TH2, DELTH;
	DELTH = fabs(TIN - TH);
	if(DELTH<0.001) // Avoids end-on scattering
		TH2 = TH + 0.01;
	else
		TH2 = TH;
	// Define angles and trigonometric quantities
	double PSB, PIB, STI, CTH, CTI, STH, STS, CTS, SPIB, SPSB, CPIB, CPSB;
	double XI, XS, YI, YS, ZI, ZS;

	PSB = PH - PS;
	PIB = PH - PIN;

	STI = sin(TIN);
	CTH = cos(TH2);
	CTI = cos(TIN);
	STH = sin(TH2);
	STS = sin(TS);
	CTS = cos(TS);
	SPIB = sin(PIB);
	SPSB = sin(PSB);
	CPIB = cos(PIB);
	CPSB = cos(PSB);

	XI = STI * CTH * CPIB - CTI * STH ;
	XS = STS * CTH * CPSB - CTS * STH ;
	YI = -STI * SPIB ;
	YS = -STS * SPSB ;
	ZI = STI * STH * CPIB + CTI * CTH ;
	ZS = STS * STH * CPSB + CTS * CTH ;

	// Relation to prime angles
	double PIP, PSP, TIP, TSP;
	if(TH==0 && PH ==0){
		PIP = PIN;
		PSP = PS;
		TIP = TIN;
		TSP = TS;
	}
	else{
		TIP = acos(ZI);
		TSP = acos(ZS) ;
		PIP = atan2(YI, XI) ;
		
		if(XS == 0 && YS == 0)
			PSP = atan(-SPSB / CPSB);
		else
			PSP = atan2(YS, XS);
	}
	
	if(TIP>=0.0 && TIP<0.01)
		TIP = 0.01;
	if(TIP>3.1316 && TIP<=Pi)
		TIP = 3.1316;
	if(TSP>=0.0 && TSP<0.01)
		TSP = 0.01;
	if(TSP>3.1316 && TSP<=Pi)
		TSP = 3.1316;
	
	// thin_eDisc
	double complex FP[4];
	thin_eDisc(TIP, PIP, TSP, PSP, FHZ, T, A, B, EPSDC, FP);

	// Calculate dot products
	double SPSP, CPSP, STSP, CTSP, STIP, CTIP, SPIP, CPIP;
	double HIXP, HIYP, HIZP, VIXP, VIYP, VIZP, HSXP, HSYP, HSZP, VSXP, VSYP, VSZP;
	double HHS, HVS, VHS, VVS, HHI, VHI, HVI, VVI;
	SPSP = sin(PSP) ;
	CPSP = cos(PSP) ;
	STSP = sin(TSP) ;
	CTSP = cos(TSP) ;
	STIP = sin(TIP) ;
	CTIP = cos(TIP) ;
	SPIP = sin(PIP) ;
	CPIP = cos(PIP) ;
	
	HIXP = CTH * SPIB ;
	HIYP = CPIB ;
	HIZP = STH * SPIB ;
	VIXP = -CTI * CTH * CPIB - STI * STH ;
	VIYP = CTI * SPIB ;
	VIZP = -CTI * STH * CPIB + STI * CTH ;
	HSXP = -CTH * SPSB ;
	HSYP = -CPSB ;
	HSZP = -STH * SPSB ;
	VSXP = -CTS * CTH * CPSB - STS * STH  ;
	VSYP = CTS * SPSB ;
	VSZP = -CTS * STH * CPSB + STS * CTH ;
	
	HHS = SPSP * HSXP - CPSP * HSYP ;
	HVS = -CTSP * CPSP * HSXP - CTSP * SPSP * HSYP + STSP * HSZP ;
	VHS = SPSP * VSXP - CPSP * VSYP ;
	VVS = -CTSP * CPSP * VSXP - CTSP * SPSP * VSYP + STSP * VSZP ;
	
	HHI = -SPIP * HIXP + CPIP * HIYP ;
	VHI = -SPIP * VIXP + CPIP * VIYP ;
	HVI = -CTIP * CPIP * HIXP - CTIP * SPIP * HIYP + STIP * HIZP ;
	VVI = -CTIP * CPIP * VIXP - CTIP * SPIP * VIYP + STIP * VIZP ;

	// Transformation
	// Fhh=F(1)      Fhv=F(2)     Fvh=F(3)     Fvv=F(4)
	F[0] = FP[0] * HHS * HHI + FP[1] * HHS * HVI + FP[2] * HVS * HHI + FP[3] * HVS * HVI ;
	F[1] = FP[0] * HHS * VHI + FP[1] * HHS * VVI + FP[2] * HVS * VHI + FP[3] * HVS * VVI ;
	F[2] = FP[0] * VHS * HHI + FP[1] * VHS * HVI + FP[2] * VVS * HHI + FP[3] * VVS * HVI ;
	F[3] = FP[0] * VHS * VHI + FP[1] * VHS * VVI + FP[2] * VVS * VHI + FP[3] * VVS * VVI ;
}

// Calculates the internal field coefficients for a vertical infinitely long dielectric cylinder.
void DIEINF(double TIP, long LJ, double RAD, double FHZ, double complex EPSDC,
			double complex *JN1, double complex *JN2, double complex *DJN1, double complex *DJN2,
			double complex *HN, double complex *DHN, double complex DH[5], double complex DV[5])
{
	// Preallocation
	double complex ANV[25] = {0}, ANH[25] = {0}, BNV[25] = {0}, BNH[25] = {0}, CNV[25] = {0}, CNH[25] = {0}, CCB[25] = {0};

	// Input data
	double AK0, CIMP;
	AK0 = TwoPi * FHZ / LIGHTSPEED;

	CIMP = 120 * Pi ;
	
	//
	double STIP, CTIP, S0;
	double complex EMSI, EJN, X0, S1, R1, QN, QN2, JD, DJ, HDJ, DHJ, VN, PN, SN, MN, MNN, PNN, VP, QHJ, QHJ2, J22, QJHJ, DENOM, SX0;
	STIP = sin(TIP) ;
	CTIP = cos(TIP) ;
	EMSI = EPSDC - CTIP * CTIP;
	EJN = cpow(-I1, (double complex) LJ);
	X0 = AK0 * RAD * STIP ;
	S0 = 1 / STIP ;
	S1 = EPSDC / csqrt(EMSI) ;
	R1 = 1 / csqrt(EMSI) ;
	QN = -LJ * CTIP * (R1*R1 - S0*S0) / (AK0 * RAD) ;
	QN2 = QN * QN ;
	// QNB = QN * X0 ;
	JD = JN1[LJ] * DJN2[LJ] ;
	DJ = DJN1[LJ] * JN2[LJ] ;
	HDJ = HN[LJ] * DJN2[LJ] ;
	DHJ = DHN[LJ] * JN2[LJ] ;
	VN = S1 * JD - S0 * DJ ;
	PN = (R1 * HDJ - S0 * DHJ) * 1.0E-09 ;
	SN = (S1 * HDJ - S0 * DHJ) * 1.0E-09 ;
	MN = R1 * JD - S0 * DJ ;
	MNN = MN * SN * 1.0E+09 ;
	PNN = PN * SN ;
	VP = VN * PN * 1.0E+09 ;
	QHJ = QN * HN[LJ] * JN2[LJ] * 1.0E-09 ;
	QHJ2 = QHJ * QHJ ;
	J22 = JN2[LJ] * JN2[LJ] ;
	QJHJ = QN2 * JN1[LJ] * HN[LJ] * J22 ;
	DENOM = PNN - QHJ2 ;
	SX0 = 2 * S0 / (Pi * X0) ;
	
	// Calculates coefficients excatly for argument X0 GT 0.08 ; uses small argument approximations for Hankel function for X0 LT 0.08
	// Exact calculation :
	CNV[LJ] = -(VP - QJHJ) * 1.0E-05 / (DENOM * 1.0E+13) ;
	CNH[LJ] = -(MNN - QJHJ) * 1.0E-05 / (DENOM * 1.0E+13) ;
	CCB[LJ] = (SX0 * QN * J22) * 1.0E-05 / (DENOM * 1.0E+13) ;

	ANV[LJ] = EJN * STIP * (JN1[LJ] + CNV[LJ] * HN[LJ]) / JN2[LJ] ;
	BNV[LJ] = EJN * STIP * (HN[LJ] * CCB[LJ]) / (JN2[LJ] * CIMP) ;
	ANH[LJ] = EJN * STIP * (CCB[LJ] * HN[LJ]) / JN2[LJ] ;
	BNH[LJ] = -EJN * STIP * (JN1[LJ] + CNH[LJ] * HN[LJ]) / (JN2[LJ] * CIMP) ;

	DV[0] = ANV[LJ] ;
	DV[1] = -I1 * R1 * CTIP * ANV[LJ] ;
	DV[2] = -LJ * R1 * R1 * BNV[LJ] * CIMP / AK0 ;
	DV[3] = LJ * R1 * R1 * CTIP * ANV[LJ] / AK0 ;
	DV[4] = -I1 * R1 * BNV[LJ] * CIMP ;

	DH[0] = ANH[LJ] ;
	DH[1] = -I1 * R1 * CTIP * ANH[LJ] ;
	DH[2] = -LJ * R1 * R1 * BNH[LJ] * CIMP / AK0 ;
	DH[3] = LJ * R1 * R1 * CTIP * ANH[LJ] / AK0 ;
	DH[4] = -I1 * R1 * BNH[LJ] * CIMP ;
}

double complex SUM1(long N, long K, double complex X2)
{
	long L;
	double complex SUM, TERMR, TERM;
	double TEST;
	TERMR = 1.0 / (double)(N + K + 1) ;
	TERM = TERMR;
	SUM = TERM ;

	for(L=1; L<=100; L++){
		TERM = -TERM * X2 * (double)(N + K + L) / (double)L /(double)(N + K + L + 1) / (double)(N + L + 1) ;
		SUM = SUM + TERM ;
		TEST = cabs(TERM / SUM) ;
		if(TEST < 1.E-12)
			return SUM;
	}
//  75    FORMAT(44X,'**********I6 (L)* SUM HAS NOT CONVERGED *****')
	return SUM;
}

double complex SUM2(long N, long K, double complex X2)
{
	long L;
	double complex SUM, TERMR, TERM;
	double TEST;
	TERMR = 1.0 / (double)(N + K) ;
	TERM = TERMR;
	SUM = TERM ;

	for(L=1; L<=100; L++){
		TERM = -TERM * X2 * (double)(N + K + L - 1) / (double)L /(double)(N + K + L) / (double)(N + L - 1) ;
		SUM = SUM + TERM ;
		TEST = cabs(TERM / SUM) ;
		if (TEST < 1.E-12)
			return SUM;
	}
	//    76    FORMAT(44X,'**********I7 (L)* SUM HAS NOT CONVERGED *****')
	return SUM;
}

// This subroutine evaluates I6 & I7 for N greater than zero
void I6I7P(double complex XX1, double complex XX2, long LJ, double RAD, double complex *EIP1, double complex *EIP2)
{
	long N, K, KK, JJ;
	double complex X1, X2, TERM1, SI6, B, SNK6, SNK7, TERMK, I6, I7, BB, TERM2, SI7, TRMK;
	double TEST;
	N = LJ;
	X1 = XX1 * XX1 / 4.0 ;
	X2 = XX2 * XX2 / 4.0 ;
	K = 0 ;
	TERM1 = SUM1(N, K, X2) ;
	SI6 = TERM1 ;
	B = 1.0;
	
	for(K = 1; K <= 100; K++){
		B = -B * X1 / (double)K / (double)(N + K) ;
		SNK6 = SUM1(N, K, X2) ;
		TERMK = B * SNK6 ;
		SI6 = SI6 + TERMK ;
		TEST = cabs(TERMK / SI6) ;
		if(TEST < 1.E-12)
			break;  
	}

	I6 = SI6 * XX2 * RAD / 4.0 ;

	if(N == 0){
		EIP1[LJ] = I6  ;
		//if (N == 0)
			I7 = -EIP1[0] ;
		
		EIP2[LJ] = I7 ;
		return ;
	}
	else{
		for(K = 1; K<=N; K++)
			I6 = I6 * XX1 * XX2 / 4.0 / (double)K / (double)(K + 1) ;
	}
	//  <<   I7  <<
	BB = 1.0;
	TERM2 =  SUM2(N, 0, X2) ;
	SI7 = TERM2 ;

	for(KK = 1; KK<=100; KK++){
		BB = -BB * X1 / (double)KK / (double)(N + KK) ;
		SNK7 = SUM2(N, KK, X2) ;
		TRMK = BB * SNK7 ;
		SI7 = SI7 + TRMK ;
		TEST = cabs(TRMK / SI7) ;
		
		if(TEST < 1.E-12)
			break; 
	}
	//  74   *** I7* SUM HAS NOT CONVERGED *****
	I7 = SI7 * XX1 * RAD / 4.0  ;

	if(N == 1){
		EIP1[LJ] = I6  ;
		//if (N == 0)
		//	I7 = -EIP1[0] ;
		
		EIP2[LJ] = I7 ;
		return ;
	}		
	else{
		for(JJ = 2; JJ<=N; JJ++)
			I7 = I7 * XX1 * XX2 / 4.0 / (double)JJ /(double)(JJ - 1) ;
	}
}

double complex I3PM(long LJ, double RAD, double complex *JN1, double complex *JN2, double complex *DJN1, double complex *DJN2, double complex LAM1, double complex LAM2)
{
	double complex I3PMx;

	I3PMx = RAD * (LAM1 * JN2[LJ] * DJN1[LJ] - LAM2 * JN1[LJ] * DJN2[LJ]) / (LAM2*LAM2 - LAM1*LAM1) ;
	return I3PMx;
}

// Integration of product of Bessel functions for N.GE.0.
//  FOR I4&I5 USES RECURRENCE FORMULAS
//  FOR I6&I7 USES SERIES EXPANSIONS
void INTBES(double complex XX1, double complex XX2, long LJ, double RAD, double complex *EIP1, double complex *EIP2,
			double complex *JN1, double complex *JN2, double complex *DJN1, double complex *DJN2, double complex *EI)
{
	long JP, LJ1;
	double complex LAM1, LAM2, I3, I6, I7, I4, I5, I3D;
	LAM1 = XX1 / RAD ;
	LAM2 = XX2 / RAD ;

	LJ1 = LJ - 1;
	JP = LJ + 1;
	
	if (LJ > 0){
		I3 = RAD * (LAM1 * JN2[LJ] * DJN1[LJ] - LAM2 * JN1[LJ] * DJN2[LJ]) / (LAM2*LAM2 - LAM1*LAM1) ;
		I6 = EIP1[LJ] ;
		I7 = EIP2[LJ] ;
		I4 = ((double)LJ * I6) / LAM1 - I3PM(JP, RAD, JN1, JN2, DJN1, DJN2, LAM1, LAM2) ;
		I5 = I3PM(LJ1, RAD, JN1, JN2, DJN1, DJN2, LAM1, LAM2) - ((double)LJ * I7) / LAM1 ;

		EI[0] = I3 ;
		EI[1] = I4 ;
		EI[2] = I5 ;
		EI[3] = I6 ;
		EI[4] = I7 ;
		//printf("LJ:%ld, I3:%le,I4:%le,I5:%le,I6:%le,I7:%le\n",
		// LJ, creal(I3), creal(I4), creal(I5), creal(I6), creal(I7));

		return ;
	}		

	I3 = RAD * (LAM1 * JN2[0] * DJN1[0] - LAM2 * JN1[0] * DJN2[0]) / (LAM2*LAM2 - LAM1*LAM1) ;

	I6 = EIP1[LJ];
	I7 = EIP2[LJ];
	I4 = ((double)LJ * I6) / LAM1 - I3PM(JP, RAD, JN1, JN2, DJN1, DJN2, LAM1, LAM2) ;
	I3D = RAD * (LAM1 * JN2[1] * DJN1[1] - LAM2 * JN1[1] * DJN2[1]) / (LAM2*LAM2 - LAM1*LAM1) ;
	I5 = I3D - ((double)LJ * I7) / LAM1 ;

	EI[0] = I3 ;
	EI[1] = I4 ;
	EI[2] = I5 ;
	EI[3] = I6 ;
	EI[4] = I7 ;
}

// Calculates scattering amplitudes for each N, N=0,1,..
//     End of cylinder is at the origin	
void SCATN(double TIP, double PIP, double TSP, double PSP, double complex *DH, double complex *DV, double L, long LJ, double FHZ,
		   double complex EPSDC, double complex I3, double complex I4, double complex I5, double complex I6, double complex I7, double complex *FNP) 
{
	//  Input data
	double AK0 = TwoPi * FHZ / LIGHTSPEED ;
	double CTIP, SPSP, CPSP, STSP, CTSP;
	// STIP = sin(TIP) ;
	CTIP = cos(TIP) ;
	// SPIP = sin(PIP) ;
	// CPIP = cos(PIP) ;
	SPSP = sin(PSP) ;
	CPSP = cos(PSP) ;
	STSP = sin(TSP) ;
	CTSP = cos(TSP) ;

	// Calculation of dot products
	double HSPXP, HSPYP, HSPZP, VSPXP, VSPYP, VSPZP;
	double complex HA0H, HA0V, VA0H, VA0V, HA1H, HA1V, VA1H, VA1V, HA2H, HA2V, VA2H, VA2V;
	double complex HB1H, HB1V, VB1H, VB1V, HB2H, HB2V, VB2H, VB2V;
	HSPXP = SPSP ;
	HSPYP = -CPSP ;
	HSPZP = 0.0 ;
	VSPXP = -CTSP * CPSP ;
	VSPYP = -CTSP * SPSP ;
	VSPZP = STSP ;

	HA0H = HSPZP * DH[0] ;
	HA0V = HSPZP * DV[0] ;
	VA0H = VSPZP * DH[0] ;
	VA0V = VSPZP * DV[0] ;

	HA1H = HSPXP * DH[1] + HSPYP * DH[4] ;
	HA1V = HSPXP * DV[1] + HSPYP * DV[4] ;
	VA1H = VSPXP * DH[1] + VSPYP * DH[4] ;
	VA1V = VSPXP * DV[1] + VSPYP * DV[4] ;

	HA2H = HSPXP * DH[2] + HSPYP * DH[3] ;
	HA2V = HSPXP * DV[2] + HSPYP * DV[3] ;
	VA2H = VSPXP * DH[2] + VSPYP * DH[3] ;
	VA2V = VSPXP * DV[2] + VSPYP * DV[3] ;

	HB1H = -HSPXP * DH[4] + HSPYP * DH[1] ;
	HB1V = -HSPXP * DV[4] + HSPYP * DV[1] ;
	VB1H = -VSPXP * DH[4] + VSPYP * DH[1] ;
	VB1V = -VSPXP * DV[4] + VSPYP * DV[1] ;

	HB2H = -HSPXP * DH[3] + HSPYP * DH[2] ;
	HB2V = -HSPXP * DV[3] + HSPYP * DV[2] ;
	VB2H = -VSPXP * DH[3] + VSPYP * DH[2] ;
	VB2V = -VSPXP * DV[3] + VSPYP * DV[2] ;

	double complex A, ARG, I11, EJN, COEFF, EPPPM, EPSP, EMPSP;
	double ALPHA, PRMT, PPP;
	A = AK0 * AK0 * (EPSDC - 1) / (4 * Pi) ;
	ALPHA = CTIP + CTSP ;
	PRMT = AK0 * ALPHA * L ;
	ARG = I1 * PRMT ;

	if (PRMT == 0.0)
		I11 = L;
	else
		I11 = (1.0 - cexp(-ARG)) * L / ARG ; // I11 replaced by conj(I11) Lang 9/99
	
	EJN = cpow(-I1, (double complex)LJ);
	COEFF = Pi * A * I11 * EJN ;
	PPP = PIP - PSP ;
	EPPPM = cexp(-I1 * (double complex)LJ * PPP) ;
	EPSP = cexp(I1 * (double complex)PSP) ;
	EMPSP = cexp(-I1 * (double complex)PSP) ;

	double complex I3HA0H, I46AHH, I57AHH, I46BHH, I57BHH;
	double complex I3HA0V, I46AHV, I57AHV, I46BHV, I57BHV;
	double complex I3VA0H, I46AVH, I57AVH, I46BVH, I57BVH;
	double complex I3VA0V, I46AVV, I57AVV, I46BVV, I57BVV;

	I3HA0H = I3 * HA0H * 2 ;
	I46AHH = -I1 * EPSP * (I4 * HA1H + I6 * HA2H) ;
	I57AHH = I1 * EMPSP * (I5 * HA1H + I7 * HA2H) ;
	I46BHH = -EPSP * (I4 * HB1H + I6 * HB2H) ;
	I57BHH = -EMPSP * (I5 * HB1H + I7 * HB2H) ;

	FNP[0] = COEFF * EPPPM * (I3HA0H + I46AHH + I57AHH + I46BHH + I57BHH) ;

	I3HA0V = I3 * HA0V * 2 ;
	I46AHV = -I1 * EPSP * (I4 * HA1V + I6 * HA2V) ;
	I57AHV = I1 * EMPSP * (I5 * HA1V + I7 * HA2V) ;
	I46BHV = -EPSP * (I4 * HB1V + I6 * HB2V) ;
	I57BHV = -EMPSP * (I5 * HB1V + I7 * HB2V) ;

	FNP[1] = COEFF * EPPPM * (I3HA0V + I46AHV + I57AHV + I46BHV + I57BHV) ;

	I3VA0H = I3 * VA0H * 2 ;
	I46AVH = -I1 * EPSP * (I4 * VA1H + I6 * VA2H) ;
	I57AVH = I1 * EMPSP * (I5 * VA1H + I7 * VA2H) ;
	I46BVH = -EPSP * (I4 * VB1H + I6 * VB2H) ;
	I57BVH = -EMPSP *(I5 * VB1H + I7 * VB2H) ;

	FNP[2] = COEFF * EPPPM * (I3VA0H + I46AVH + I57AVH + I46BVH + I57BVH) ;

	I3VA0V = I3 * VA0V * 2 ;
	I46AVV = -I1 * EPSP * (I4 * VA1V + I6 * VA2V) ;
	I57AVV = I1 * EMPSP * (I5 * VA1V + I7 * VA2V) ;
	I46BVV = -EPSP * (I4 * VB1V + I6 * VB2V) ;
	I57BVV = -EMPSP * (I5 * VB1V + I7 * VB2V) ;

	FNP[3] = COEFF * EPPPM * (I3VA0V + I46AVV + I57AVV + I46BVV + I57BVV) ;
}

void THINCYL(double TIP, double PIP, double TSP, double PSP, double RAD, double L, double FHZ, double complex EPSDC, double complex *FPP)
{
/* *****************************************************************
%
%      Calculates scattering amplitudes for thin cylinder wrt prime co.
%            End of cylinder at the origin
%
% *******************************************************************/

	// Input data
	double AK0;
	AK0 = TwoPi * FHZ / LIGHTSPEED ;

	//  Define constants
	double complex DCNST1, D1, D2;
	double VOL;
	DCNST1 = AK0*AK0 * (EPSDC - 1.0) / (4.0 * Pi) ;
	VOL = Pi * RAD*RAD * L ;
	D1 = (EPSDC - 1.0) / (EPSDC + 1.0) ;
	D2 = 2.0 / (EPSDC + 1.0) ;

	double PHPD, CTIP, STIP, CTSP, STSP, CPHPD, SPHPD;
	PHPD = PIP - PSP ;

	CTIP = cos(TIP) ;
	STIP = sin(TIP) ;
	CTSP = cos(TSP) ;
	STSP = sin(TSP) ;
	CPHPD = cos(PHPD) ;
	SPHPD = sin(PHPD) ;

	//  Calculate dot products
	double HSHI, HSVI, VSHI, VSVI;
	HSHI = -CPHPD ;
	HSVI = CTIP * SPHPD ;
	VSHI = CTSP * SPHPD ;
	VSVI = CPHPD * CTIP * CTSP + STIP * STSP ;

	//  Calculate the sinc function 
	double ALPH, ABARG;
	double complex ARG, SCTPTN, DCNST;
	ALPH = -CTIP - CTSP ;
	ARG = I1 * AK0 * ALPH * L ;
	ABARG = cabs(ARG) ;

	if (ABARG < 1.0E-04)
		SCTPTN = 1.0 + ARG / 2.0 ;
	else
		SCTPTN = (cexp(ARG) - 1.0) / ARG ;

	DCNST = DCNST1 * VOL * SCTPTN ;

	// Calculate scattering amplitudes
	//  FPP(1)=FPPHH                FPP(2)=FPPHV
	//  FPP(3)=FPPVH                FPP(4)=FPPVV
	FPP[0] = DCNST * D2 * HSHI ;
	FPP[1] = DCNST * D2 * HSVI ;
	FPP[2] = DCNST * D2 * VSHI ;
	FPP[3] = DCNST * (D2 * VSVI + D1 * STIP * STSP) ;
}

// CALCULATES SCATTERING AMPLITUDES W.R.T PRIME COORDINATES
//    End of cylinder is at the origin
void DIECYN(double TIP, double PIP, double TSP, double PSP, double RAD, double L, double FHZ, double complex EPSDC, double complex *FP)
{
	// Preallocation
	double complex JN1[25] = {0}, DJN1[25] = {0};
	double complex JN2[25] = {0}, DJN2[25] = {0};
	double complex JN3[25] = {0}, DJN3[25] = {0};
	double complex HN[25] = {0}, DHN[25] = {0};
	
	//  Input data
	long NMAX, LJ;
	double AK0, X2R;
	double complex EMSI, X0, X1, X2;
	AK0 = TwoPi * FHZ / LIGHTSPEED ;
	NMAX = 12 ;
	X0 = (double complex) AK0 * RAD * sin(TIP) ;
	EMSI = EPSDC - cos(TIP)*cos(TIP) ;
	X1 = AK0 * RAD * csqrt(EMSI) ;
	X2R = AK0 * RAD * sin(TSP) ;
	X2 = (double complex) X2R;

	int nm;
	double complex YN1[25], DYN1[25];
	double complex YN2[25], DYN2[25];
	double complex YN3[25], DYN3[25];
	cbessjyna(11, X0, &nm, &JN1[0], &YN1[0], &DJN1[0], &DYN1[0]);
	cbessjyna(11, X1, &nm, &JN2[0], &YN2[0], &DJN2[0], &DYN2[0]);
	cbessjyna(11, X2, &nm, &JN3[0], &YN3[0], &DJN3[0], &DYN3[0]);

	cbessh(11, X0, &nm, &HN[0], &DHN[0]);

	double complex DH[5] = {0}, DV[5] = {0};
	double complex EIP1[25] = {0}, EIP2[25] = {0}, EI[5];
	double complex I3, I4, I5, I6, I7, FNP[4];
	double complex FPHH, FPHV, FPVH, FPVV, TESTHH, TESTHV, TESTVH, TESTVV;
	double DIFHHR, DIFHHI, DIFHVR, DIFHVI, DIFVHR, DIFVHI, DIFVVR, DIFVVI;
	double CFHHR, CFHHI, TEHHR, TEHHI, CFHVR, CFHVI, TEHVR, TEHVI, CFVHR, CFVHI, TEVHR, TEVHI, CFVVR, CFVVI, TEVVR, TEVVI;
	int KONHH, KONHV, KONVH, KONVV, KON;

	for(LJ = 0; LJ<NMAX; LJ++){	
//		printf("LJ: %ld\n", LJ);	
		DIEINF(TIP, LJ, RAD, FHZ, EPSDC, JN1, JN2, DJN1, DJN2, HN, DHN, DH, DV) ;
/*		printf("DH: ");
		for(ii=0; ii<5; ii++)
			cprintf(DH[ii], ' ');
		printf("\n");
		printf("DV: ");
		for(ii=0; ii<5; ii++)
			cprintf(DV[ii], ' ');
		printf("\n");*/
		if(isnan(creal(DH[0]))){
			printf(">> DH0 is NAN\n"); exit(1);
		}
		if(isnan(creal(DH[1]))){
			printf(">> DH1 is NAN\n"); exit(1);
		}
		if(isnan(creal(DH[2]))){
			printf(">> DH2 is NAN\n"); exit(1);
		}
		if(isnan(creal(DH[3]))){
			printf(">> DH3 is NAN\n"); exit(1);
		}
		if(isnan(creal(DH[4]))){
			printf(">> DH4 is NAN\n"); exit(1);
		}
		if(isnan(creal(DV[0]))){
			printf(">> DV0 is NAN\n"); exit(1);
		}
		if(isnan(creal(DV[1]))){
			printf(">> DV1 is NAN\n"); exit(1);
		}
		if(isnan(creal(DV[2]))){
			printf(">> DV2 is NAN\n"); exit(1);
		}
		if(isnan(creal(DV[3]))){
			printf(">> DV3 is NAN\n"); exit(1);
		}
		if(isnan(creal(DV[4]))){
			printf(">> DV4 is NAN\n"); exit(1);
		}
		I6I7P(X1, X2, LJ, RAD, EIP1, EIP2);
/*		printf("X1: "); cprintf(X1, '\n');
		printf("X2: "); cprintf(X2, '\n');
		printf("EIP1: ");
		for(ii=0; ii<25; ii++)
			cprintf(EIP1[ii], ' ');
		printf("\n");
		printf("EIP2: ");
		for(ii=0; ii<25; ii++)
			cprintf(EIP2[ii], ' ');
		printf("\n");
*/
		if(isnan(creal(EIP1[LJ]))){
			printf(">> LJ:%ld, EIP1 is NAN\n", LJ); exit(1);
		}
		if(isnan(creal(EIP2[LJ]))){
			printf(">> EIP2 is NAN\n"); exit(1);
		}
		
		INTBES(X1, X2, LJ, RAD, EIP1, EIP2, JN2, JN3, DJN2, DJN3, EI) ;
//		printf("EI: ");
//		for(ii=0; ii<5; ii++)
//			cprintf(EI[ii], ' ');
//		printf("\n");
		if(isnan(creal(EI[0]))){
			printf(">> EI0 is NAN\n"); exit(1);
		}
		if(isnan(creal(EI[1]))){
			printf(">> LJ:%ld, EI1 is NAN\n", LJ); exit(1);
		}
		if(isnan(creal(EI[2]))){
			printf(">> EI2 is NAN\n"); exit(1);
		}
		if(isnan(creal(EI[3]))){
			printf(">> EI3 is NAN\n"); exit(1);
		}
		if(isnan(creal(EI[4]))){
			printf(">> EI4 is NAN\n"); exit(1);
		}		
		I3 = EI[0] ;
		I4 = EI[1] ;
		I5 = EI[2] ;
		I6 = EI[3] ;
		I7 = EI[4] ;

		SCATN(TIP, PIP, TSP, PSP, DH, DV, L, LJ, FHZ, EPSDC, I3, I4, I5, I6, I7, FNP) ;

		if (LJ == 0){
			FPHH = FNP[0] ;
			FPHV = FNP[1] ;
			FPVH = FNP[2] ;
			FPVV = FNP[3] ;
			TESTHH = FPHH ;
			TESTHV = FPHV ;
			TESTVH = FPVH ;
			TESTVV = FPVV ;
		}
		else{
			FPHH = FPHH + FNP[0] ;
			FPHV = FPHV + FNP[1] ;
			FPVH = FPVH + FNP[2] ;
			FPVV = FPVV + FNP[3] ;
		
			DV[0] = pow(-1, LJ) * DV[0] ;
			DV[1] = pow(-1, LJ) * DV[1] ;
			DV[2] = pow(-1, LJ) * DV[2] ;
			DV[3] = -pow(-1, LJ) * DV[3] ;
			DV[4] = -pow(-1, LJ) * DV[4] ;
			DH[0] = -pow(-1, LJ) * DH[0] ;
			DH[1] = -pow(-1, LJ) * DH[1] ;
			DH[2] = -pow(-1, LJ) * DH[2] ;
			DH[3] = pow(-1, LJ) * DH[3] ;
			DH[4] = pow(-1, LJ) * DH[4] ;
			
			//         I3 = I3 ;
			I4 = -EI[2] ;
			I5 = -EI[1] ;
			I6 = -EI[4] ;
			I7 = -EI[3] ;
			
			SCATN(TIP, PIP, TSP, PSP, DH, DV, L, -LJ, FHZ, EPSDC, I3, I4, I5, I6, I7, FNP) ;
/*			printf("TIP: %lf\n", TIP);
			printf("PIP: %lf\n", PIP);
			printf("PSP: %lf\n", PSP);
			printf("DH: ");
			for(ii=0; ii<5; ii++)
				cprintf(DH[ii], ' ');
			printf("\n");
			printf("DV: ");
			for(ii=0; ii<5; ii++)
				cprintf(DV[ii], ' ');
			printf("\n");
			printf("L: %lf\n", L);
			printf("-LJ: %ld\n", -LJ);
			printf("EPSDC: "); cprintf(EPSDC, '\n');
			printf("I3: "); cprintf(I3, '\n');
			printf("I4: "); cprintf(I4, '\n');
			printf("I5: "); cprintf(I5, '\n');
			printf("I6: "); cprintf(I6, '\n');
			printf("I7: "); cprintf(I7, '\n');
			
			printf("FNP: ");
			for(ii=0; ii<4; ii++)
				cprintf(FNP[ii], ' ');
			printf("\n");
*/
			FPHH = FPHH + FNP[0] ;
			FPHV = FPHV + FNP[1] ;
			FPVH = FPVH + FNP[2] ;
			FPVV = FPVV + FNP[3] ;
		
			// Test of convergence for hh pol'n
			CFHHR = creal(FPHH) ;
			CFHHI = cimag(FPHH) ;
			TEHHR = creal(TESTHH) ;
			TEHHI = cimag(TESTHH) ;
			
			if(CFHHR == 0.0 || CFHHI == 0)
				KONHH = 1 ;
			else{
				DIFHHR = (TEHHR - CFHHR) / CFHHR ;
				DIFHHI = (TEHHI - CFHHI) / CFHHI ;
				
				if(fabs(DIFHHR) < 0.01 && fabs(DIFHHI) < 0.01)
					KONHH = 1 ;
				else
					KONHH = 0 ;
			}
			
			// Test of convergence for hv pol'n
			CFHVR = creal(FPHV) ;
			CFHVI = cimag(FPHV) ;
			TEHVR = creal(TESTHV) ;
			TEHVI = cimag(TESTHV) ;
			
			if(CFHVR == 0.0 || CFHVI  == 0.0)
				KONHV = 1 ; 
			else{
				DIFHVR = (TEHVR - CFHVR) / CFHVR ;
				DIFHVI = (TEHVI - CFHVI) / CFHVI ;
				
				if(fabs(DIFHVR) < 0.01 && fabs(DIFHVI) < 0.01)
					KONHV = 1  ;
				else
					KONHV = 0 ;
			}
		
			// Test of convergence for vh pol'n
			CFVHR = creal(FPVH) ;
			CFVHI = cimag(FPVH) ;
			TEVHR = creal(TESTVH) ;
			TEVHI = cimag(TESTVH) ;
			
			if(CFVHR == 0.0 || CFVHI  == 0.0)
				KONVH = 1 ; 
			else{
				DIFVHR = (TEVHR - CFVHR) / CFVHR ;
				DIFVHI = (TEVHI - CFVHI) / CFVHI ;
				
				if(fabs(DIFVHR)< 0.01 && fabs(DIFVHI)< 0.01)
					KONVH = 1 ;
				else
					KONVH = 0 ;
			}
		
			// Test of convergence for vv pol'n
			CFVVR = creal(FPVV) ;
			CFVVI = cimag(FPVV) ;
			TEVVR = creal(TESTVV) ;
			TEVVI = cimag(TESTVV) ;
			
			if(CFVVR == 0.0 || CFVVI  == 0.0)
				KONVV = 1 ; 
			else{
				DIFVVR = (TEVVR - CFVVR) / CFVVR ;
				DIFVVI = (TEVVI - CFVVI) / CFVVI ;
				
				if(fabs(DIFVVR) < 0.01 && fabs(DIFVVI)< 0.01)
					KONVV = 1 ;
				else
					KONVV = 0 ;
			}

			KON = KONHH + KONHV + KONVH + KONVV ;
			
			if(KON == 4){
				//printf(">> break! LJ=%ld\n", LJ);
				break ; 
			}
			TESTHH = FPHH ;
			TESTHV = FPHV ;
			TESTVH = FPVH ;
			TESTVV = FPVV ;
		}
	}
	FP[0] = FPHH ;
	FP[1] = FPHV ;
	FP[2] = FPVH ;
	FP[3] = FPVV ;

	if(isnan(creal(FP[0]))){
		printf(">> FP0 is NAN\n"); exit(1);
	}
	if(isnan(creal(FP[1]))){
		printf(">> FP1 is NAN\n"); exit(1);
	}
	if(isnan(creal(FP[2]))){
		printf(">> FP2 is NAN\n"); exit(1);
	}
	if(isnan(creal(FP[3]))){
		printf(">> FP3 is NAN\n"); exit(1);
	}

}

/*
% function eCylinder 
%
%   The bistatic scattering amplitude from a lossy dielectric cylinder is
%   calculated. The phase center is at the bottom of the cylinder. Both
%   thin and thick routines are used. 
%
%   F = eCylinder(TIN, PIN, TS, PS, TH, PH, FHZ, RAD, L, EPS)
%
%   INPUTS:
%   TIN,PIN = INCIDENT ANGLES (RAD)
%   TS,PS = SCATTERED ANGLES (RAD)
%   TH,PH = ROTATION ANGLES (RAD)
%   FHZ = FRQUENCY (HZ)
%   RAD = RADIUS OF CYLINDER (M)
%   L = LENGTH OF CYLINDER (M)
%   EPS = RELATIVE DIELECTRIC CONSTANT
%   F = BISTATIC SCATTERING AMPLITUDES
%   F(1) = FHH, F(2) = FVH, F(3) = FHV, F(4) = FVV
%
%   See also calcPropagation, eDisc.

%   Copyright © 2017-2018 Mehmet Kurum, Orhan Eroglu, Dylan R. Boyd.
%   Adapted from George Washington University vegetation scattering models

%   This program is free software: You can redistribute it and/or 
%   modify it under the terms of the GNU General Public License as 
%   published by the Free Software Foundation, either version 3 of the 
%   License, or (at your option) any later version.

%   Version: 1.0.0
*/
void eCylinder(double TH, double PH, struct VegModelType *V, double complex *F)
{
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
%
%  Calculates the scattering amplitudes with respect to laboratory
%  coordinates by transforming from prime coordinates.
%      End of cylinder is at the origin
%  1= thick cylinder     4=thin cylinder
%
% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	double TIN = V->tin;
	double PIN = V->pin;
	double TS = Pi - TIN;
	double PS = Pi + PIN;
	double FHZ = V->freq;
	double RAD = V->radius;
	double L = V->length;
	double complex EPSDC = V->epsilon;

	// Input data
	double AK0 = TwoPi * FHZ / LIGHTSPEED ;

	// Avoids end-on scattering
	double DELTH;
	DELTH = fabs(TIN - TH) ;

	if (DELTH < 0.001)
		TH = TH + 0.01 ;
	
	// Initialize
	double complex FP[4] = {0};

	// Define angles and trigonometric quantities
	double PSB, PIB, STI, CTH, CTI, STH, STS, CTS, SPIB, SPSB, CPIB, CPSB;
	double XI, XS, YI, YS, ZI, ZS;

	PSB = PH - PS ;
	PIB = PH - PIN ;

	STI = sin(TIN) ;
	CTH = cos(TH) ;
	CTI = cos(TIN) ;
	STH = sin(TH) ;
	STS = sin(TS) ;
	CTS = cos(TS) ;
	SPIB = sin(PIB) ;
	SPSB = sin(PSB) ;
	CPIB = cos(PIB) ;
	CPSB = cos(PSB) ;
	XI = STI * CTH * CPIB - CTI * STH ;
	XS = STS * CTH * CPSB - CTS * STH ;
	YI = -STI * SPIB ;
	YS = -STS * SPSB ;
	ZI = STI * STH * CPIB + CTI * CTH ;
	ZS = STS * STH * CPSB + CTS * CTH ;

	// Relation to prime angles
	double PIP, PSP, TIP, TSP;
	if(TH == 0.0 && PH == 0){
		PIP = PIN ;
		PSP = PS ;
		TIP = TIN ;
		TSP = TS ;
	}	
	else{
		TIP = acos(ZI) ;
		TSP = acos(ZS) ;
		PIP = atan2(YI, XI) ;
		
		if(XS == 0 && YS == 0)
			PSP = atan(-SPSB / CPSB) ;
		else
			PSP = atan2(YS, XS) ;
	}

	if(TIP >= 0.0 && TIP < 0.01) TIP = 0.01 ;
	if(TIP > 3.1316 && TIP <= Pi) TIP = 3.1316 ;
	if(TSP >= 0.0 && TSP < 0.01) TSP = 0.01 ;
	if(TSP > 3.1316 && TSP <= Pi) TSP = 3.1316 ;

	double SPSP, CPSP, STSP, CTSP, STIP, CTIP, SPIP, CPIP;
	SPSP = sin(PSP) ;
	CPSP = cos(PSP) ;
	STSP = sin(TSP) ;
	CTSP = cos(TSP) ;
	STIP = sin(TIP) ;
	CTIP = cos(TIP) ;
	SPIP = sin(PIP) ;
	CPIP = cos(PIP) ;

	double complex EMSI, X1;
	double X1ABS;
	// X0 = AK0 * RAD * STIP ;
	EMSI = EPSDC - CTIP*CTIP ;
	X1 = AK0 * RAD * csqrt(EMSI) ;
	// X2R = AK0 * RAD * STSP ;
	// X0ABS = fabs(X0) ;
	X1ABS = cabs(X1) ;
	// X2RABS = fabs(X2R) ;
//	printf("AK0: %lf, RAD: %lf ", AK0, RAD);
//	printf("X1: "); cprintf(X1,' ');
//	printf("EMSI: "); cprintf(EMSI,'\n');

	if(X1ABS > 0.05)
		DIECYN(TIP, PIP, TSP, PSP, RAD, L, FHZ, EPSDC, FP) ;
	else if (X1ABS <= 0.05)
		THINCYL(TIP, PIP, TSP, PSP, RAD, L, FHZ, EPSDC, FP) ;
//	printf("FP: ");
//	cprintf(FP[0], ' ');
//	cprintf(FP[1], ' ');
//	cprintf(FP[2], ' ');
//	cprintf(FP[3], '\n');
	// Calculate dot products
	double HIXP, HIYP, HIZP, VIXP, VIYP, VIZP, HSXP, HSYP, HSZP, VSXP, VSYP, VSZP;
	double HHS, HVS, VHS, VVS, HHI, VHI, HVI, VVI;
	HIXP = CTH * SPIB ;
	HIYP = CPIB ;
	HIZP = STH * SPIB ;
	VIXP = -CTI * CTH * CPIB - STI * STH ;
	VIYP = CTI * SPIB ;
	VIZP = -CTI * STH * CPIB + STI * CTH ;
	HSXP = -CTH * SPSB ;
	HSYP = -CPSB ;
	HSZP = -STH * SPSB ;
	VSXP = -CTS * CTH * CPSB - STS * STH ;
	VSYP = CTS * SPSB ;
	VSZP = -CTS * STH * CPSB + STS * CTH ;

	HHS = SPSP * HSXP - CPSP * HSYP ;
	HVS = -CTSP * CPSP * HSXP - CTSP * SPSP * HSYP + STSP * HSZP ;
	VHS = SPSP * VSXP - CPSP * VSYP ;
	VVS = -CTSP * CPSP * VSXP - CTSP * SPSP * VSYP + STSP * VSZP ;

	HHI = -SPIP * HIXP + CPIP * HIYP ;
	VHI = -SPIP * VIXP + CPIP * VIYP ;
	HVI = -CTIP * CPIP * HIXP - CTIP * SPIP * HIYP + STIP * HIZP ;
	VVI = -CTIP * CPIP * VIXP - CTIP * SPIP * VIYP + STIP * VIZP ;

	// Transformation
	//  Fhh = F(1)   Fhv = F(2)   Fvh = F(3)   Fvv = F(4)

	F[0] = FP[0] * HHS * HHI + FP[1] * HHS * HVI + FP[2] * HVS * HHI + FP[3] * HVS * HVI ;
	F[1] = FP[0] * HHS * VHI + FP[1] * HHS * VVI + FP[2] * HVS * VHI + FP[3] * HVS * VVI ;
	F[2] = FP[0] * VHS * HHI + FP[1] * VHS * HVI + FP[2] * VVS * HHI + FP[3] * VVS * HVI ;
	F[3] = FP[0] * VHS * VHI + FP[1] * VHS * VVI + FP[2] * VVS * VHI + FP[3] * VVS * VVI ;
//	printf("F: ");
//	cprintf(F[0],' ');
//	cprintf(F[1],' ');
//	cprintf(F[2],' ');
//	cprintf(F[3],'\n');
//	exit(1);
}

void compute_fscatamp(double th, double ph, struct VegModelType *V, double complex *fScatAmp)
{
	if(V->shape == DISK){
		eDisc(th, ph, V, fScatAmp);
	}
	else if(V->shape == CYLINDER){
		eCylinder(th, ph, V, fScatAmp);
	}
	else{
		printf(">> Error: Unknown vegetation type.\n"); exit(1);
	}
}

void compute_Th_average(double phi, struct VegModelType *V, double complex *avf)
{
	double tol = 1e-2;
	V->phi = phi;
	
	if(V->theta1 == V->theta2) //  || pdf == 1 ?
		compute_fscatamp(V->theta1, phi, V, avf);
	else
		qsimp_pxf(phi, tol, V, avf);
}

void compute_PhTh_average(struct VegModelType *V, double complex *afun)
{
	double tol = 1e-2;
	double complex temp[4];

	qsimp_ThAvg(0, TwoPi, tol, V, temp);
	afun[0] = temp[0] / TwoPi;
	afun[1] = temp[1] / TwoPi;
	afun[2] = temp[2] / TwoPi;
	afun[3] = temp[3] / TwoPi;
}

void compute_avfscatamp(double *tin, double *pin, struct VegModelType *V, double complex *avFScatAmp)
{
	double complex temp[4];
	size_t nTh = 1;
	size_t nPh = 1;
	int iTh, iPh;

	for(iTh=0; iTh<nTh; iTh++){
		for(iPh=0; iPh<nPh; iPh++){
			V->tin = tin[iTh];
			V->pin = pin[iPh];
			compute_PhTh_average(V, temp);
		}
	}
	// Select only hh and vv components
	avFScatAmp[0] = temp[0];
	avFScatAmp[1] = temp[3];
}

// Calculates the contribution of each scatterer type to the attenuation
void compute_dKzn(double tin, double freq, double rho, double complex fXAmp1, double complex fXAmp2, double complex *dKz)
{
	double ko = TwoPi * freq / LIGHTSPEED;
	double kz = ko * cos(tin);
	double cfac = TwoPi / kz;

	dKz[0] = cfac * rho * fXAmp1;
	dKz[1] = cfac * rho * fXAmp2;
}

void trapzd_ThAvg(double a, double b, long n, struct VegModelType *V, double complex s[4])
{
    double x,tnm,del;
    long i,it,j;
	double complex sum, temp1[4], temp2[4];

    if (n == 1) {
		compute_Th_average(a, V, temp1);
		compute_Th_average(b, V, temp2);
		for(i=0; i<4; i++){
           	s[i] = 0.5 * (b-a) * (temp1[i]+temp2[i]);
		}

    }
	else{
		for(i=0; i<4; i++){
			for (it=1,j=1;j<n-1;j++)
				it <<= 1;
			tnm=it;
			del=(b-a)/tnm; // This is the spacing of points to be added.
			x=a+0.5*del;
			for (sum=0.0,j=1;j<=it;j++,x+=del){
				compute_Th_average(x, V, temp1);
				sum += temp1[i];
			}
			s[i] = 0.5 * (s[i]+(b-a)*sum/tnm); // This replaces s by its refined value.
		}
    }
}

void qsimp_ThAvg(double a, double b, double eps, struct VegModelType *V, double complex s[4])
{
    long i, j, imax = 15;
	double sum1, sum2, abs;
    double complex st[4] = {0}, ost[4] = {-1e-30, -1e-30, -1e-30, -1e-30}, os[4] = {-1e-30, -1e-30, -1e-30, -1e-30};
    for (i=1; i<=imax;i++) {
		sum1 = 0; sum2 = 0;
        trapzd_ThAvg(a, b, i, V, st);
		for(j=0; j<4; j++){
	        s[j] = (4.0*st[j] - ost[j])/3.0;
			abs = cabs(s[j]-os[j]);
			sum1 += abs*abs;
			abs = cabs(os[j]);
			sum2 += abs*abs;
		}
		if (i > 5){ // Avoid spurious early convergence.				 
			if (sqrt(sum1) < eps*sqrt(sum2) || (sum1 == 0.0 && sum2 == 0.0))
				return;
		}
		for(j=0; j<4; j++){
			os[j] = s[j];
			ost[j] = st[j];
		}
    }
    printf(">> Too many steps in routine qsimp_ThAvg\n");
    return;
}

void pdfxfunc(double x, double phi, struct VegModelType *V, double complex *pxf)
{
	double pdf;
	double complex fun[4];

	pdf = unifpdf(x, V->theta1, V->theta2);
	compute_fscatamp(x, phi, V, fun);
	if(isnan(pdf)){
		printf(">> pdf is NAN\n"); exit(1);
	}
	if(isnan(creal(fun[0]))){
		printf(">> fun0 is NAN\n"); exit(1);
	}
	if(isnan(creal(fun[1]))){
		printf(">> fun1 is NAN\n"); exit(1);
	}
	if(isnan(creal(fun[2]))){
		printf(">> fun2 is NAN\n"); exit(1);
	}
	if(isnan(creal(fun[3]))){
		printf(">> fun3 is NAN\n"); exit(1);
	}
	pxf[0] = pdf * fun[0];
	pxf[1] = pdf * fun[1];
	pxf[2] = pdf * fun[2];
	pxf[3] = pdf * fun[3];
}

void trapzd_pxf(double phi, double a, double b, long n, struct VegModelType *V, double complex s[4])
{
    double x,tnm,del;
    long i,it,j;
	double complex sum, temp1[4], temp2[4];
    if (n == 1) {
		pdfxfunc(a, phi, V, temp1);
		pdfxfunc(b, phi, V, temp2);
		for(i=0; i<4; i++){
           	s[i] = 0.5 * (b-a) * (temp1[i]+temp2[i]);
		}
    }
	else{
		for(i=0; i<4; i++){
			for (it=1,j=1;j<n-1;j++)
				it <<= 1;
			tnm=it;
			del=(b-a)/tnm; // This is the spacing of points to be added.
			x=a+0.5*del;
			for (sum=0.0,j=1;j<=it;j++,x+=del){
				pdfxfunc(x, phi, V, temp1);
				sum += temp1[i];
			}
			s[i] = 0.5 * (s[i]+(b-a)*sum/tnm); // This replaces s by its refined value.
		}
    }
}

void qsimp_pxf(double phi, double tol, struct VegModelType *V, double complex s[4])
{
    long i, j, imax = 15;
	double sum1, sum2, abs;
    double complex st[4] = {0}, ost[4] = {-1e-30, -1e-30, -1e-30, -1e-30}, os[4] = {-1e-30, -1e-30, -1e-30, -1e-30};
    for (i=1; i<=imax;i++) {
		sum1 = 0; sum2 = 0;
        trapzd_pxf(phi, V->theta1, V->theta2, i, V, st);
		for(j=0; j<4; j++){
	        s[j] = (4.0*st[j] - ost[j])/3.0;
			abs = cabs(s[j]-os[j]);
			sum1 += abs*abs;
			abs = cabs(os[j]);
			sum2 += abs*abs;
		}
		if (i > 5){ // Avoid spurious early convergence.				 
			if (sqrt(sum1) < tol*sqrt(sum2) || (sum1 == 0.0 && sum2 == 0.0))
				return;
		}
		for(j=0; j<4; j++){
			os[j] = s[j];
			ost[j] = st[j];
		}
    }
    printf(">> Too many steps in routine qsimp_pxf\n");
	printf("phi=%lf, s=%le+i%le, %le+i%le, %le+i%le, %le+i%le\n", V->phi, creal(st[0]), cimag(st[0]), creal(st[1]), cimag(st[1]), creal(s[2]), cimag(s[2]), creal(s[3]), cimag(s[3]));
    return;
}

/* Calculates propagation and attenuation due to the vegetation layer, if any. Uses the functions eCylinder and eDisk to make calculations for
dielctric cylinder (stalk, branch, or needle) and dielectric disk (leaf)*/
void calcPropagation(double freq, long iSoOp, long iVeg)
{
	long iLayer, iType, iKind, iAng;
	int nKind;
	double complex temp[2], fXAmp[2][90][VegData[iVeg].nKindMax][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	double tin[1], pin[1];

	// Trace the vegetation layersm Types, and Kinds
	for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
		for(iType=0; iType<VegData[iVeg].nTypeMax; iType++){
			nKind = VegData[iVeg].typknd_new[iLayer][iType];
			for(iKind=0; iKind<nKind; iKind++){
				// Elliptic Disk (Leaf)
				if(VegData[iVeg].shape[iKind][iType][iLayer] == DISK){
					VegModel->semiMajorx = VegData[iVeg].dim1[iKind][iType][iLayer];
					VegModel->semiMinorx = VegData[iVeg].dim2[iKind][iType][iLayer];
					VegModel->thickness = VegData[iVeg].dim3[iKind][iType][iLayer];
					VegModel->shape = DISK;
					// Object = Object_mode1(PROB_deg, A, B, T, EPS, 'L') ;
				}
				// Circular Cylinder, i.e., dim1=dim2
				else{
					VegModel->radius = VegData[iVeg].dim1[iKind][iType][iLayer];
					VegModel->length = VegData[iVeg].dim3[iKind][iType][iLayer];
					VegModel->shape = CYLINDER;
					// Object = Object_mode1(PROB_deg, RAD, RAD, LEN, EPS, 'B') ;
				}
				VegModel->epsilon = VegData[iVeg].e_c[iKind][iType][iLayer][iSoOp];
				VegModel->theta1 = VegData[iVeg].beginAng[iKind][iType][iLayer];
				VegModel->theta2 = VegData[iVeg].endAng[iKind][iType][iLayer];
				VegModel->freq = freq;

				printf("Layer: %ld -Types: %ld -Kinds: %ld, calculating...\n", iLayer+1, iType+1, iKind+1);
				for(iAng=0; iAng<90; iAng++){
					// Average Forward Scattering Amplitude
					tin[0] = iAng*D2R;
					pin[0] = 0;
					compute_avfscatamp(tin, pin, VegModel, temp);
					fXAmp[0][iAng][iKind][iType][iLayer] = temp[0];
					fXAmp[1][iAng][iKind][iType][iLayer] = temp[1];
					//printf("%le+i%le, ", creal(fXAmp[0][iAng][iKind][iType][iLayer]), cimag(fXAmp[0][iAng][iKind][iType][iLayer]));
					//printf("%le+i%le\n", creal(fXAmp[1][iAng][iKind][iType][iLayer]), cimag(fXAmp[1][iAng][iKind][iType][iLayer]));
				}
			}
		}
	}

	// PROPAGATION AND ATTENUATION
	// Incremental Propagation Constant
	double complex dddKz[2][90][VegData[iVeg].nKindMax][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	double complex ddKz[2][90][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	double complex dKz[2][90][VegData[iVeg].nLayer];

	// Attenuation
	double complex ATPIQS[2][90][VegData[iVeg].nKindMax][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	double complex ATTPIQS[2][90][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	double complex ATTENPIQS[2][90][VegData[iVeg].nLayer];
	double complex ATTENH[90], ATTENV[90];

	// Propagation Constants
	FILE *outFile, *outFile2;
	double tho;
	double complex fXAmp01, fXAmp02, dddKz0[2], ddKz0[2], dKz0[2];
	char filename[60];
	double complex ArgH, ArgV;

	sprintf(filename, "veg_%s_dKz_%ldMHz.out", VegData[iVeg].label, (long)(freq/1e6));
	outFile = FileOpen(SttDataPath, filename, "w");
	fprintf(outFile, "%d %ld\n", 90, VegData[iVeg].nLayer);
	
	sprintf(filename, "veg_%s_Arg_%ldMHz.out", VegData[iVeg].label, (long)(freq/1e6));
	outFile2 = FileOpen(SttDataPath, filename, "w");
	fprintf(outFile2, "%d %ld\n", 90, VegData[iVeg].nLayer);

	for(iAng=0; iAng<90; iAng++){
		tho = iAng*D2R;
		ATTENH[iAng] = 0;
		ATTENV[iAng] = 0;
		ArgH = 0;
		ArgV = 0;
		for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
			dKz[0][iAng][iLayer] = 0;
			dKz[1][iAng][iLayer] = 0;
			for(iType=0; iType<VegData[iVeg].nTypeMax; iType++){
				nKind = VegData[iVeg].typknd_new[iLayer][iType];
				ddKz[0][iAng][iType][iLayer] = 0;
				ddKz[1][iAng][iType][iLayer] = 0;
				for(iKind=0; iKind<nKind; iKind++){
					fXAmp01 = fXAmp[0][iAng][iKind][iType][iLayer];
					fXAmp02 = fXAmp[1][iAng][iKind][iType][iLayer];
					compute_dKzn(tho, freq, VegData[iVeg].dsty[iKind][iType][iLayer], fXAmp01, fXAmp02, dddKz0) ;
					
					dddKz[0][iAng][iKind][iType][iLayer] = dddKz0[0];
					dddKz[1][iAng][iKind][iType][iLayer] = dddKz0[1];
					
					// Determine incremental propagation constant for each scatter type
					ddKz[0][iAng][iType][iLayer] += dddKz0[0];
					ddKz[1][iAng][iType][iLayer] += dddKz0[1];
					
					// 20 log10(exp(imag(dkz)*d))
					ATPIQS[0][iAng][iKind][iType][iLayer] = 20 * 0.434294481903252 * cimag(dddKz0[0]) * VegData[iVeg].thickness[iLayer];
					ATPIQS[1][iAng][iKind][iType][iLayer] = 20 * 0.434294481903252 * cimag(dddKz0[1]) * VegData[iVeg].thickness[iLayer];
				}
				
				ddKz0[0] = ddKz[0][iAng][iType][iLayer];
				ddKz0[1] = ddKz[1][iAng][iType][iLayer];

				// Calculate attenuation due to each scatter type in all layers
				// ATTPIQS(4, Ntype, Nlayer) : Incoming attenuation for P
				// poln and outgoing attenuation for Q poln
				ATTPIQS[0][iAng][iType][iLayer] = 20 * 0.434294481903252 * cimag(ddKz0[0]) * VegData[iVeg].thickness[iLayer];
				ATTPIQS[1][iAng][iType][iLayer] = 20 * 0.434294481903252 * cimag(ddKz0[1]) * VegData[iVeg].thickness[iLayer];

				// Determine incremental propagation contant for each layer
				// Sum over the scatter types assigned to each layer
				dKz[0][iAng][iLayer] += ddKz0[0];
				dKz[1][iAng][iLayer] += ddKz0[1];
			}
			
			VegData[iVeg].dKz[iSoOp][0][iAng][iLayer] = dKz[0][iAng][iLayer];
			VegData[iVeg].dKz[iSoOp][1][iAng][iLayer] = dKz[1][iAng][iLayer];
			
			dKz0[0] = dKz[0][iAng][iLayer];
			dKz0[1] = dKz[1][iAng][iLayer];
			// Calculate total incoming and outgoing attenuation for each layer
			ATTENPIQS[0][iAng][iLayer] = 20 * 0.434294481903252 * cimag(dKz0[0]) * VegData[iVeg].thickness[iLayer];
			ATTENPIQS[1][iAng][iLayer] = 20 * 0.434294481903252 * cimag(dKz0[1]) * VegData[iVeg].thickness[iLayer];
			
			// Calculate overall attenuation
			ATTENH[iAng] += ATTENPIQS[0][iAng][iLayer];
			ATTENV[iAng] += ATTENPIQS[1][iAng][iLayer];

			ArgH += VegData[iVeg].dKz[iSoOp][0][iAng][iLayer] * VegData[iVeg].thickness[iLayer];
			ArgV += VegData[iVeg].dKz[iSoOp][1][iAng][iLayer] * VegData[iVeg].thickness[iLayer];

			fprintf(outFile, "%le %le %le %le %le %le\n", creal(dKz[0][iAng][iLayer]), cimag(dKz[0][iAng][iLayer]),
															creal(dKz[1][iAng][iLayer]), cimag(dKz[1][iAng][iLayer]),
															creal(ATTENH[iAng]), creal(ATTENV[iAng]));
		}
		fprintf(outFile2, "%le %le %le %le\n", creal(ArgH), cimag(ArgH), creal(ArgV), cimag(ArgV));
	}
	fclose(outFile);
	fclose(outFile2);
}

/* Calculates propagation and attenuation due to the vegetation layer, if any. Uses the functions eCylinder and eDisk to make calculations for
dielctric cylinder (stalk, branch, or needle) and dielectric disk (leaf)*/
void calcPropagationNetCDF(long iSoOp, long iVeg, long iSample)
{
	long iLayer, iType, iKind, iOut;
	int nKind;
	double complex temp[2], fXAmp[2][VegData[iVeg].nKindMax][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	struct FixedObsType *F;
	double tin[1], pin[1];
	double freq, vegFrac;
//	double complex e_c_canopy = 0, beta = 0.5, e_c_canopy2 = 0;

	F = &Fixed[iSoOp];
	freq = F->Freq;

	iOut = iSoOp*Nc->sampleDim + iSample;

	// Update Geometry						
	if(freq == 137E6){
		F->thTx = Nc->incORBCOMM[iSample]*D2R;
		F->elTx = HalfPi - F->thTx;
		F->thRx = F->thTx;
		F->elRx = HalfPi - F->thRx;
	}
	else if(freq == 255E6 || freq == 370E6){
		F->thTx = Nc->incMUOS[iSample]*D2R;
		F->elTx = HalfPi - F->thTx;
		F->thRx = F->thTx;
		F->elRx = HalfPi - F->thRx;
	}
	else if(freq == 1575.42E6){
		F->thTx = Nc->incGPS[iSample]*D2R;
		F->elTx = HalfPi - F->thTx;
		F->thRx = F->thTx;
		F->elRx = HalfPi - F->thRx;
	}
	calcGeometryFixed(F);

	tin[0] = Bistatic->AngT2S_sf_th0;
	pin[0] = 0;

	// Trace the vegetation layersm Types, and Kinds
	for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
		for(iType=0; iType<VegData[iVeg].nTypeMax; iType++){
			nKind = VegData[iVeg].typknd_new[iLayer][iType];
			for(iKind=0; iKind<nKind; iKind++){
				// Elliptic Disk (Leaf)
				if(VegData[iVeg].shape[iKind][iType][iLayer] == DISK){
					VegModel->semiMajorx = VegData[iVeg].dim1[iKind][iType][iLayer];
					VegModel->semiMinorx = VegData[iVeg].dim2[iKind][iType][iLayer];
					VegModel->thickness = VegData[iVeg].dim3[iKind][iType][iLayer];
					VegModel->shape = DISK;
					// Object = Object_mode1(PROB_deg, A, B, T, EPS, 'L') ;
				}
				// Circular Cylinder, i.e., dim1=dim2
				else{
					VegModel->radius = VegData[iVeg].dim1[iKind][iType][iLayer];
					VegModel->length = VegData[iVeg].dim3[iKind][iType][iLayer];
					VegModel->shape = CYLINDER;
					// Object = Object_mode1(PROB_deg, RAD, RAD, LEN, EPS, 'B') ;
				}
				VegModel->epsilon = VegData[iVeg].e_c[iKind][iType][iLayer][iSoOp];
				VegModel->theta1 = VegData[iVeg].beginAng[iKind][iType][iLayer];
				VegModel->theta2 = VegData[iVeg].endAng[iKind][iType][iLayer];
				VegModel->freq = freq;

				//printf("Layer: %ld -Types: %ld -Kinds: %ld, calculating...\n", iLayer+1, iType+1, iKind+1);
				//printf("tin=%lf, e_c=%lf+j%lf, thickness=%lf, d1=%lf, d2=%lf\n", tin[0], creal(VegModel->epsilon), cimag(VegModel->epsilon), VegData[iVeg].thickness[iLayer], VegModel->semiMajorx, VegModel->semiMinorx);
				
				// Average Forward Scattering Amplitude
				compute_avfscatamp(tin, pin, VegModel, temp);
				fXAmp[0][iKind][iType][iLayer] = temp[0];
				fXAmp[1][iKind][iType][iLayer] = temp[1];

				//vegFrac = VegData[iVeg].dsty[iKind][iType][iLayer] * Pi * VegData[iVeg].dim1[iKind][iType][iLayer] * VegData[iVeg].dim2[iKind][iType][iLayer] * VegData[iVeg].dim3[iKind][iType][iLayer];
				//if(iType == 0)
				//	e_c_canopy = cpow(1 + vegFrac * (VegModel->epsilon - 1), 1/beta);
				//else if(iType == 1)
				//	e_c_canopy2 = cpow(1 + vegFrac * (VegModel->epsilon - 1), 1/beta);
				/*if(iType == 0){
					vegFrac = 4/3*Pi*VegKind[0].dim[0]*VegKind[0].dim[1]*VegKind[0].dim[2] * VegKind[0].density;
					e_c_canopy = calcTwoPhaseMixture(CYLINDER, VegModel->epsilon, vegFrac);
				}
				else if(iType == 1){
					vegFrac = 4/3*Pi*VegKind[1].dim[0]*VegKind[1].dim[1]*VegKind[1].dim[2] * VegKind[1].density;
					e_c_canopy2 = calcTwoPhaseMixture(SPHERE, VegModel->epsilon, vegFrac);
				}*/
			}
		}
	}
//	printf("%lf+i%lf, %lf+i%lf\n", creal(e_c_canopy), cimag(e_c_canopy), creal(e_c_canopy2), cimag(e_c_canopy2));
//	Nc->e_c_can_r[iOut] = creal(e_c_canopy);
//	Nc->e_c_can_i[iOut] = cimag(e_c_canopy);
//	Nc->e_c_can2_r[iOut] = creal(e_c_canopy2);
//	Nc->e_c_can2_i[iOut] = cimag(e_c_canopy2);
	
	// PROPAGATION AND ATTENUATION
	// Incremental Propagation Constant
	//double complex dddKz[2][VegData[iVeg].nKindMax][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	double complex ddKz[2][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	double complex dKz[2][VegData[iVeg].nLayer];

	// Attenuation
	//double complex ATPIQS[2][VegData[iVeg].nKindMax][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	//double complex ATTPIQS[2][VegData[iVeg].nTypeMax][VegData[iVeg].nLayer];
	//double complex ATTENPIQS[2][VegData[iVeg].nLayer];
	//double complex ATTENH, ATTENV;

	// Propagation Constants
	double complex fXAmp01, fXAmp02, dddKz0[2], ddKz0[2];
	double complex ArgH = 0, ArgV = 0;
	
	//ATTENH = 0;
	//ATTENV = 0;
	for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
		dKz[0][iLayer] = 0;
		dKz[1][iLayer] = 0;
		for(iType=0; iType<VegData[iVeg].nTypeMax; iType++){
			nKind = VegData[iVeg].typknd_new[iLayer][iType];
			ddKz[0][iType][iLayer] = 0;
			ddKz[1][iType][iLayer] = 0;
			for(iKind=0; iKind<nKind; iKind++){
				fXAmp01 = fXAmp[0][iKind][iType][iLayer];
				fXAmp02 = fXAmp[1][iKind][iType][iLayer];
				compute_dKzn(tin[0], freq, VegData[iVeg].dsty[iKind][iType][iLayer], fXAmp01, fXAmp02, dddKz0) ;

				//dddKz[0][iKind][iType][iLayer] = dddKz0[0];
				//dddKz[1][iKind][iType][iLayer] = dddKz0[1];
				
				// Determine incremental propagation constant for each scatter type
				ddKz[0][iType][iLayer] += dddKz0[0];
				ddKz[1][iType][iLayer] += dddKz0[1];
				
				// 20 log10(exp(imag(dkz)*d))
				//ATPIQS[0][iKind][iType][iLayer] = 20 * 0.434294481903252 * cimag(dddKz0[0]) * VegData[iVeg].thickness[iLayer];
				//ATPIQS[1][iKind][iType][iLayer] = 20 * 0.434294481903252 * cimag(dddKz0[1]) * VegData[iVeg].thickness[iLayer];
			}
			
			ddKz0[0] = ddKz[0][iType][iLayer];
			ddKz0[1] = ddKz[1][iType][iLayer];

			// Calculate attenuation due to each scatter type in all layers
			// ATTPIQS(4, Ntype, Nlayer) : Incoming attenuation for P
			// poln and outgoing attenuation for Q poln
			//ATTPIQS[0][iType][iLayer] = 20 * 0.434294481903252 * cimag(ddKz0[0]) * VegData[iVeg].thickness[iLayer];
			//ATTPIQS[1][iType][iLayer] = 20 * 0.434294481903252 * cimag(ddKz0[1]) * VegData[iVeg].thickness[iLayer];

			// Determine incremental propagation contant for each layer
			// Sum over the scatter types assigned to each layer
			dKz[0][iLayer] += ddKz0[0];
			dKz[1][iLayer] += ddKz0[1];
		}
		VegData[iVeg].dKzNc[0][iLayer] = dKz[0][iLayer];
		VegData[iVeg].dKzNc[1][iLayer] = dKz[1][iLayer];
		/*
		dKz0[0] = dKz[0][iLayer];
		dKz0[1] = dKz[1][iLayer];
		// Calculate total incoming and outgoing attenuation for each layer
		ATTENPIQS[0][iLayer] = 20 * 0.434294481903252 * cimag(dKz0[0]) * VegData[iVeg].thickness[iLayer];
		ATTENPIQS[1][iLayer] = 20 * 0.434294481903252 * cimag(dKz0[1]) * VegData[iVeg].thickness[iLayer];
		
		// Calculate overall attenuation
		ATTENH += ATTENPIQS[0][iLayer];
		ATTENV += ATTENPIQS[1][iLayer];
		*/
		ArgH += VegData[iVeg].dKzNc[0][iLayer] * VegData[iVeg].thickness[iLayer];
		ArgV += VegData[iVeg].dKzNc[1][iLayer] * VegData[iVeg].thickness[iLayer];
	}

	Nc->argHr[iOut] = creal(ArgH);
	Nc->argHi[iOut] = cimag(ArgH);
	Nc->argVr[iOut] = creal(ArgV);
	Nc->argVi[iOut] = cimag(ArgV);
	//printf("argH = %lf + j %lf, argV = %lf + j %lf\n", creal(ArgH), cimag(ArgH), creal(ArgV), cimag(ArgV));
}

// Calculates vegetation transmissivity 
void calcVegT(long iSoOp, long iVeg)
{
	long thsd, iLayer;
	double complex ArgH = 0, ArgV = 0;
	thsd = round(Bistatic->AngT2S_sf_th0*R2D);

	for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
		ArgH += VegData[iVeg].dKz[iSoOp][0][thsd][iLayer] * VegData[iVeg].thickness[iLayer];
		ArgV += VegData[iVeg].dKz[iSoOp][1][thsd][iLayer] * VegData[iVeg].thickness[iLayer];
	}

	Bistatic->ArgH = ArgH;
	Bistatic->ArgV = ArgV;
}
