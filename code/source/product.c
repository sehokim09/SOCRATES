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

void ReportOrbElements(void)
{
	static FILE *outFile;
	long i;
	double rp, ra;

	outFile = FileOpen(SimPath, "Orbits.out","w");

	for(i=0;i<Norb;i++){
		rp = Orb[i].SMA*(1-Orb[i].ecc) - World[EARTH].rad;
		ra = Orb[i].SMA*(1+Orb[i].ecc) - World[EARTH].rad;
		fprintf(outFile, "%lf %lf %ld %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", SimTime, DynTime, i, (Orb[i].SMA-World[EARTH].rad)/1000, 
        Orb[i].ecc, Orb[i].inc*R2D, Orb[i].MeanMotion, Orb[i].anom*R2D, Orb[i].ArgP*R2D, rp/1000, ra/1000, Orb[i].RAAN*R2D);
	}
	fclose(outFile);
}

void RepeatGroundTrack(struct OrbitType *O)
{
	double nPeriod, nDays = 0;
	double tolerance, dlon_track;
	double dlon, temp;
	double lon[2000], currLon;
	double wE = 7.2921151467E-5;
	uint32_t nOrbits = 0, i, j;
	uint8_t flagTrack = 1, flagRepeat = 0;

	tolerance = 0.1*D2R;
	dlon_track = 0.9*D2R;
	/* ECI to ECEF of Receiver's Position */
//	MxV(World[EARTH].CWN,S->PosN,PosWR);
	/* ECEF to WGS84 */
//	ECEFToWGS84(PosWR, &lat, &lon, &alt);

	lon[nOrbits] = 0;

	nPeriod = TwoPi / (O->MeanMotion + O->ArgPdot);
	dlon = nPeriod * (wE - O->RAANdot);

	printf("dlon: %lf rad, %lf deg\n", dlon, dlon*R2D);

	while(1)
	{
		currLon = lon[nOrbits++] + dlon;
		if(currLon >= TwoPi)
			currLon = currLon - TwoPi;
		if(currLon < 0)
			currLon = -currLon;

		lon[nOrbits] = currLon;

		for(i=0; i<nOrbits; i++){
			for(j=i+1; j<nOrbits; j++){
				if(lon[i]>lon[j]){
					temp = lon[i];
					lon[i] = lon[j];
					lon[j] = temp;
				}
			}
		}
		for(i=0; i<nOrbits; i++){
			if(lon[i+1]-lon[i]>dlon_track){
				flagTrack = 0;
			}
		}
		if(flagTrack)
			break;
		else
			flagTrack = 1;

		if(lon[nOrbits] <= tolerance){
			flagRepeat = 1;
			break;
		}
	}
	printf("0: %lf, 1: %lf\n", lon[0], lon[1]);
	nDays = nOrbits*nPeriod;
	if(flagTrack)
		printf("Distance btw ground tracks within 100km\n");
	if(flagRepeat)
		printf("SC passes over the same location.\n");
	printf("nDays: %lf days = %lf sec \n", nDays/86400, nDays);
	printf("nOrbits: %d\n", nOrbits);
	printf("nPeriod: %lf min\n", nPeriod/60);
	printf("tol: %lf\n", tolerance);
	printf("lon: %lf\n", lon[nOrbits]);
	printf("eccentricity: %lf\n", O->ecc);
	printf("Periapsis altitude: %lf\n", O->rmin-World[EARTH].rad);
	printf("Apoapsis altitude: %lf\n", (1+O->ecc)*O->SMA-World[EARTH].rad);
	printf("Inclination: %lf\n", O->inc*R2D);
	printf("RAAN: %lf\n", O->RAAN*R2D);
	printf("Argument of Periapsis: %lf\n", O->ArgP*R2D);
	printf("True Anomaly: %lf\n", O->anom*R2D);
//		AdvanceTime();
//		EnckeRK4(S);

//		O->RAAN = O->RAAN0 + O->RAANdot*(SimTime - 0.5/O->MeanMotion*sin(2.0*O->ArgP+2.0*O->anom));
//		O->ArgP = O->ArgP0 + O->ArgPdot*SimTime;

//		Eph2RV(O->MuPlusJ2,O->SLR,O->ecc,
//				O->inc,O->RAAN,O->ArgP,
//				DynTime+DTSIM-O->tp,
//				O->PosN,O->VelN,&O->anom);

//		Ephemerides_Earth();
//		GravPertForce(S);

		/* ECI to ECEF of Receiver's Position */
//		MxV(World[EARTH].CWN,S->PosN,PosWR);
		/* ECEF to WGS84 */
//		ECEFToWGS84(PosWR, &lat, &lon, &alt);
}

// Calculate differential phase between direct and reflected signals //
void calcPhase(void)
{
	Out->phaseRD = Bistatic->Wavenumber * (Bistatic->r_tsr - Bistatic->r_tr);
	// Out->phaseRD = fmod(Out->phaseRD, TwoPi);
}

/* Calculate delay and doppler for direct and reflected signals */
void calcDelayDoppler(double PosN[3], double VelN[3], struct SpecularType *Sp, struct ScType *Tx)
{
	double rTx2Rx[3], rmTx2Rx;
	double vRx2Tx[3];
	double rTx2Sp[3], rmTx2Sp, rSp2Rx[3], rmSp2Rx;
	double temp1, temp2;
	// Direct Signal
	rTx2Rx[0] = PosN[0] - Tx->PosN[0];
	rTx2Rx[1] = PosN[1] - Tx->PosN[1];
	rTx2Rx[2] = PosN[2] - Tx->PosN[2];
	rmTx2Rx = UNITV(rTx2Rx);

	vRx2Tx[0] = Tx->VelN[0] - VelN[0];
	vRx2Tx[1] = Tx->VelN[1] - VelN[1];
	vRx2Tx[2] = Tx->VelN[2] - VelN[2];
	temp1 = VoV(vRx2Tx, rTx2Rx);

	Out->dopplerD = temp1/Bistatic->Wavelength;
	Out->delayD = rmTx2Rx/LIGHTSPEED;

	// Reflected Signal
	rTx2Sp[0] = Sp->PosN[0] - Tx->PosN[0];
	rTx2Sp[1] = Sp->PosN[1] - Tx->PosN[1];
	rTx2Sp[2] = Sp->PosN[2] - Tx->PosN[2];
	rmTx2Sp = UNITV(rTx2Sp);

	rSp2Rx[0] = PosN[0] - Sp->PosN[0];
	rSp2Rx[1] = PosN[1] - Sp->PosN[1];
	rSp2Rx[2] = PosN[2] - Sp->PosN[2];
	rmSp2Rx = UNITV(rSp2Rx);

	temp1 = VoV(Tx->VelN, rTx2Sp);
	temp2 = VoV(VelN, rSp2Rx);

	Out->dopplerR = (temp1 - temp2)/Bistatic->Wavelength;
	Out->delayR = (rmTx2Sp + rmSp2Rx)/LIGHTSPEED;

	Bistatic->r_ts = rmTx2Sp;
	Bistatic->r_sr = rmSp2Rx;
}

// Gaussian Random Error
void error_gaussian(double std_dev)
{
	// Reflection coefficient with error
	Out->P_coh1_err[0] = Out->P_coh1[0] + GaussianRandom(RNG) * std_dev;
	
	// Reflectivity with error
	if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
		Out->P_coh2_err[1] = Out->P_coh2[1] + GaussianRandom(RNG) * std_dev;
	}
	else{
		Out->P_coh1_err[1] = Out->P_coh1[1] + GaussianRandom(RNG) * std_dev;
	}
}

// Model for the variance of the auto-correlation
void error_acf(void)
{
    double tau_chips, ambfcn_RD;
    double c1, c2, c3, Re, Re2, dRdd, Re_scale;
	double CN, C_D, G_D, G_R;
	double delayRD;
	double sigma2d_lin[2], sigma2R_lin[2];
	double sigma2d_h, sigma2R_h, sigma2R_fused;
	double mean_num, mean_den, sigma_num, sigma_den, rho_12, mean_num_scaled, mean_den_scaled, hinkley[2];

	calcPhase(); // Calculate differential delay

	// Ambiguity function
	delayRD = (Bistatic->r_tsr - Bistatic->r_tr) / LIGHTSPEED;
	tau_chips = delayRD / Bistatic->t_coh;
	ambfcn_RD = MAX(1-fabs(tau_chips), 0);

	G_D = Bistatic->G_Rx_Lin * Bistatic->g_rt[0][0]; // Direct Gain of Receive Antenna

	// .. CO-pol (XX or RR or RX) .. //
	// Magnitude of Effective Reflection Coefficient
	//Re = cabs(Out->b_coh1b[0]);
	G_R = Bistatic->G_Rx_Lin * Bistatic->g_rs[0][1]; // Reflected Gain of Receive Antenna
	Re_scale = sqrt(G_D/G_R) * Bistatic->r_tsr / Bistatic->r_tr; 
	Re = sqrt(Out->P_coh1[0]) / Re_scale;
	//Re = sqrt(Out->P_coh1b[0]) / Re_scale;
	//Re = Out->P_coh1b[0]; // for test
	Re2 = Re*Re;
	// Direct Power
	//C_D = Out->PWR_d1[0];
	C_D = Bistatic->EIRP_Tx_Lin * Bistatic->Wavelength / (16*Pi*Pi*Bistatic->r_tr*Bistatic->r_tr);

	// Carrier-to-Noise Ratio
	CN = (G_D * C_D) / (BOLTZMANN * Bistatic->Tn);
	//CN = (C_D) / (BOLTZMANN * Bistatic->Tn);
	
	// Coefficients
    c1 = (1 + 4*Re2 + Re2*Re2) * (1 + Re2) / (4*Re2*Re2); 
    c2 = (1 + 4*Re2 + Re2*Re2) / (8*Re2*Re2); 
    c3 = -(1 + Re2) / (2*Re2*Re); 

	// Variance of the observable d
	//sigma2d_lin = (c1 + c2 * Bistatic->Bandwidth / CN + c3 * (1 + ambfcn_RD) * cos(Out->phaseRD)) / (Bistatic->t_coh * CN);
	sigma2d_lin[0] = (c1 + c2 * Bistatic->Bandwidth / CN + c3 * (1 + ambfcn_RD)) / (Bistatic->t_coh * CN);
	sigma2d_lin[1] = (c1 + c2 * Bistatic->Bandwidth / CN - c3 * (1 + ambfcn_RD)) / (Bistatic->t_coh * CN);

	// Variance of reflection coefficient
	dRdd = -2*Re2 / (1 - Re2);
	sigma2R_lin[0] = dRdd * dRdd * sigma2d_lin[0];
	sigma2R_lin[1] = dRdd * dRdd * sigma2d_lin[1];

	// Inputs for Hinkey PDF
	mean_num = 1 + Re2; 
	mean_den = 2*Re; 
	sigma_num = sqrt( 2*(1+Re2) + Bistatic->Bandwidth/CN) / sqrt(CN*Bistatic->t_coh);
	sigma_den = sqrt( (1+Re2) + Bistatic->Bandwidth / (2*CN)) / sqrt(CN*Bistatic->t_coh); 
	rho_12 = Re * (1+ambfcn_RD) / (sigma_num*sigma_den*CN*Bistatic->t_coh);

	mean_num_scaled = mean_num/sigma_num; 
	mean_den_scaled = mean_den/sigma_den;

	hinkley_pdf2(mean_num_scaled, mean_den_scaled, 1, 1, rho_12, 10000, hinkley);

	sigma2d_h = hinkley[1]*(sigma_num/sigma_den)*(sigma_num/sigma_den);
	sigma2R_h = dRdd * dRdd * sigma2d_h;

	if(fabs(hinkley[0]) < 1.01 && fabs(hinkley[0]) > 0.99){
		//sigma2d_fused = sigma2d_h;
		sigma2R_fused = sigma2R_h;
	}
	else{
		//sigma2d_fused = MAX(sigma2d_lin[0], sigma2d_lin[1]);
		sigma2R_fused = MAX(sigma2R_lin[0], sigma2R_lin[1]);
	}
	
	Out->Gamma_Std_mod_lin[0]= 2*Re*sqrt(sigma2R_lin[0])*Re_scale*Re_scale;
	Out->Gamma_Std_mod_fused[0] = 2*Re*sqrt(sigma2R_fused)*Re_scale*Re_scale;
	Out->Gamma_Std_mod_fused_avg[0] = Out->Gamma_Std_mod_fused[0] / sqrt(Bistatic->N_inc);

	// Reflection coefficient with error
	//Re_est = Re + GaussianRandom(RNG) * sqrt(sigma2R_fused);
	//R_est = Re_scale * Re_est;
	//Out->P_coh1b_err[0] = R_est*R_est;
	Out->P_coh1_err[0] = Out->P_coh1[0] + GaussianRandom(RNG) * Out->Gamma_Std_mod_fused_avg[0];

	// .. SNR .. //
	Out->SNR0[0] = sqrt(CN*Bistatic->t_coh)*(1+Re2) / sqrt(2*(1+Re2) + Bistatic->Bandwidth/CN);
	Out->SNR_coh1[0] = sqrt(CN*Bistatic->t_coh)*Re / sqrt(2*(1+Re2) + Bistatic->Bandwidth/CN);

	// .. X-pol (YY or RL or RY) .. //
	G_R = Bistatic->G_Rx_Lin * Bistatic->g_rs[0][0]; // Reflected Gain of Receive Antenna
	Re_scale = sqrt(G_D/G_R) * Bistatic->r_tsr / Bistatic->r_tr; 

	if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
		// Magnitude of Effective Reflection Coefficient
		//Re = cabs(Out->b_coh2b[1]);
		Re = sqrt(Out->P_coh2[1]) / Re_scale;
	}
	else{
		// Magnitude of Effective Reflection Coefficient
		//Re = cabs(Out->b_coh1b[1]);
		Re = sqrt(Out->P_coh1[1]) / Re_scale;
	}	
	Re2 = Re*Re;
	//Re = Out->P_coh1b[1]; // for test

	// Coefficients
    c1 = (1 + 4*Re2 + Re2*Re2) * (1 + Re2) / (4*Re2*Re2); 
    c2 = (1 + 4*Re2 + Re2*Re2) / (8*Re2*Re2); 
    c3 = -(1 + Re2) / (2*Re2*Re); 

	// Variance of the observable d
	//sigma2d_lin = (c1 + c2 * Bistatic->Bandwidth / CN + c3 * (1 + ambfcn_RD) * cos(Out->phaseRD)) / (Bistatic->t_coh * CN);
	sigma2d_lin[0] = (c1 + c2 * Bistatic->Bandwidth / CN + c3 * (1 + ambfcn_RD)) / (Bistatic->t_coh * CN);
	sigma2d_lin[1] = (c1 + c2 * Bistatic->Bandwidth / CN - c3 * (1 + ambfcn_RD)) / (Bistatic->t_coh * CN);

	// Variance of reflection coefficient
	dRdd = -2*(Re2) / (1 - Re2);
	sigma2R_lin[0] = dRdd * dRdd * sigma2d_lin[0];
	sigma2R_lin[1] = dRdd * dRdd * sigma2d_lin[1];

	// Inputs for Hinkey PDF
	mean_num = 1 + Re2; 
	mean_den = 2*Re; 
	sigma_num = sqrt( 2*(1+Re2) + Bistatic->Bandwidth/CN) / sqrt(CN*Bistatic->t_coh);
	sigma_den = sqrt( (1+Re2) + Bistatic->Bandwidth / (2*CN)) / sqrt(CN*Bistatic->t_coh); 
	rho_12 = Re * (1+ambfcn_RD) / (sigma_num*sigma_den*CN*Bistatic->t_coh);

	mean_num_scaled = mean_num/sigma_num; 
	mean_den_scaled = mean_den/sigma_den;

	hinkley_pdf2(mean_num_scaled, mean_den_scaled, 1, 1, rho_12, 10000, hinkley);

	sigma2d_h = hinkley[1]*(sigma_num/sigma_den)*(sigma_num/sigma_den);
	sigma2R_h = dRdd * dRdd * sigma2d_h;

	if(fabs(hinkley[0]) < 1.01 && fabs(hinkley[0]) > 0.99){
		//sigma2d_fused = sigma2d_h;
		sigma2R_fused = sigma2R_h;
	}
	else{
		//sigma2d_fused = MAX(sigma2d_lin[0], sigma2d_lin[1]);
		sigma2R_fused = MAX(sigma2R_lin[0], sigma2R_lin[1]);
	}

	Out->Gamma_Std_mod_lin[1]= 2*Re*sqrt(sigma2R_lin[0])*Re_scale*Re_scale;
	Out->Gamma_Std_mod_fused[1] = 2*Re*sqrt(sigma2R_fused)*Re_scale*Re_scale;
	Out->Gamma_Std_mod_fused_avg[1] = Out->Gamma_Std_mod_fused[1] / sqrt(Bistatic->N_inc);

	// Reflection coefficient with error
	if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
		Out->P_coh2_err[1] = Out->P_coh2[1] + GaussianRandom(RNG) * Out->Gamma_Std_mod_fused_avg[1];
	}
	else{
		Out->P_coh1_err[1] = Out->P_coh1[1] + GaussianRandom(RNG) * Out->Gamma_Std_mod_fused_avg[1];
	}

	// .. SNR .. //
	Out->SNR0[1] = sqrt(CN*Bistatic->t_coh)*(1+Re2) / sqrt(2*(1+Re2) + Bistatic->Bandwidth/CN);
	Out->SNR_coh1[1] = sqrt(CN*Bistatic->t_coh)*Re / sqrt(2*(1+Re2) + Bistatic->Bandwidth/CN);
}

// Model for the variance of the auto-correlation
void error_acf_CN(double CN)
{
    double tau_chips, ambfcn_RD;
    double c1, c2, c3, Re, Re2, dRdd, Re_scale;
	double C_D, G_D, G_R;
	double delayRD;
	double sigma2d_lin[2], sigma2R_lin[2];
	double sigma2d_h, sigma2R_h, sigma2R_fused;
	double mean_num, mean_den, sigma_num, sigma_den, rho_12, mean_num_scaled, mean_den_scaled, hinkley[2];

	calcPhase(); // Calculate differential delay

	// Ambiguity function
	delayRD = (Bistatic->r_tsr - Bistatic->r_tr) / LIGHTSPEED;
	tau_chips = delayRD / Bistatic->t_coh;
	ambfcn_RD = MAX(1-fabs(tau_chips), 0);

	G_D = Bistatic->G_Rx_Lin * Bistatic->g_rt[0][0]; // Direct Gain of Receive Antenna

	// .. CO-pol (XX or RR or RX) .. //
	// Magnitude of Effective Reflection Coefficient
	//Re = cabs(Out->b_coh1b[0]);
	G_R = Bistatic->G_Rx_Lin * Bistatic->g_rs[0][1]; // Reflected Gain of Receive Antenna
	Re_scale = sqrt(G_D/G_R) * Bistatic->r_tsr / Bistatic->r_tr; 
	Re = sqrt(Out->P_coh1[0]) / Re_scale;
	//Re = sqrt(Out->P_coh1b[0]) / Re_scale;
	//Re = Out->P_coh1b[0]; // for test
	Re2 = Re*Re;
	// Direct Power
	//C_D = Out->PWR_d1[0];
	C_D = Bistatic->EIRP_Tx_Lin * Bistatic->Wavelength / (16*Pi*Pi*Bistatic->r_tr*Bistatic->r_tr);

	// Carrier-to-Noise Ratio
	//CN = (G_D * C_D) / (BOLTZMANN * Bistatic->Tn);
	//CN = (C_D) / (BOLTZMANN * Bistatic->Tn);
	
	// Coefficients
    c1 = (1 + 4*Re2 + Re2*Re2) * (1 + Re2) / (4*Re2*Re2); 
    c2 = (1 + 4*Re2 + Re2*Re2) / (8*Re2*Re2); 
    c3 = -(1 + Re2) / (2*Re2*Re); 

	// Variance of the observable d
	//sigma2d_lin = (c1 + c2 * Bistatic->Bandwidth / CN + c3 * (1 + ambfcn_RD) * cos(Out->phaseRD)) / (Bistatic->t_coh * CN);
	sigma2d_lin[0] = (c1 + c2 * Bistatic->Bandwidth / CN + c3 * (1 + ambfcn_RD)) / (Bistatic->t_coh * CN);
	sigma2d_lin[1] = (c1 + c2 * Bistatic->Bandwidth / CN - c3 * (1 + ambfcn_RD)) / (Bistatic->t_coh * CN);

	// Variance of reflection coefficient
	dRdd = -2*Re2 / (1 - Re2);
	sigma2R_lin[0] = dRdd * dRdd * sigma2d_lin[0];
	sigma2R_lin[1] = dRdd * dRdd * sigma2d_lin[1];

	// Inputs for Hinkey PDF
	mean_num = 1 + Re2; 
	mean_den = 2*Re; 
	sigma_num = sqrt( 2*(1+Re2) + Bistatic->Bandwidth/CN) / sqrt(CN*Bistatic->t_coh);
	sigma_den = sqrt( (1+Re2) + Bistatic->Bandwidth / (2*CN)) / sqrt(CN*Bistatic->t_coh); 
	rho_12 = Re * (1+ambfcn_RD) / (sigma_num*sigma_den*CN*Bistatic->t_coh);

	mean_num_scaled = mean_num/sigma_num; 
	mean_den_scaled = mean_den/sigma_den;

	hinkley_pdf2(mean_num_scaled, mean_den_scaled, 1, 1, rho_12, 10000, hinkley);

	sigma2d_h = hinkley[1]*(sigma_num/sigma_den)*(sigma_num/sigma_den);
	sigma2R_h = dRdd * dRdd * sigma2d_h;

	if(fabs(hinkley[0]) < 1.01 && fabs(hinkley[0]) > 0.99){
		//sigma2d_fused = sigma2d_h;
		sigma2R_fused = sigma2R_h;
	}
	else{
		//sigma2d_fused = MAX(sigma2d_lin[0], sigma2d_lin[1]);
		sigma2R_fused = MAX(sigma2R_lin[0], sigma2R_lin[1]);
	}
	
	Out->Gamma_Std_mod_lin[0]= 2*Re*sqrt(sigma2R_lin[0])*Re_scale*Re_scale;
	Out->Gamma_Std_mod_fused[0] = 2*Re*sqrt(sigma2R_fused)*Re_scale*Re_scale;
	Out->Gamma_Std_mod_fused_avg[0] = Out->Gamma_Std_mod_fused[0] / sqrt(Bistatic->N_inc);

	// Reflection coefficient with error
	//Re_est = Re + GaussianRandom(RNG) * sqrt(sigma2R_fused);
	//R_est = Re_scale * Re_est;
	//Out->P_coh1b_err[0] = R_est*R_est;
	Out->P_coh1_err[0] = Out->P_coh1[0] + GaussianRandom(RNG) * Out->Gamma_Std_mod_fused_avg[0];

	// .. SNR .. //
	Out->SNR0[0] = sqrt(CN*Bistatic->t_coh)*(1+Re2) / sqrt(2*(1+Re2) + Bistatic->Bandwidth/CN);
	Out->SNR_coh1[0] = sqrt(CN*Bistatic->t_coh)*Re / sqrt(2*(1+Re2) + Bistatic->Bandwidth/CN);

	// .. X-pol (YY or RL or RY) .. //
	G_R = Bistatic->G_Rx_Lin * Bistatic->g_rs[0][0]; // Reflected Gain of Receive Antenna
	Re_scale = sqrt(G_D/G_R) * Bistatic->r_tsr / Bistatic->r_tr; 

	if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
		// Magnitude of Effective Reflection Coefficient
		//Re = cabs(Out->b_coh2b[1]);
		Re = sqrt(Out->P_coh2[1]) / Re_scale;
	}
	else{
		// Magnitude of Effective Reflection Coefficient
		//Re = cabs(Out->b_coh1b[1]);
		Re = sqrt(Out->P_coh1[1]) / Re_scale;
	}	
	Re2 = Re*Re;
	//Re = Out->P_coh1b[1]; // for test

	// Coefficients
    c1 = (1 + 4*Re2 + Re2*Re2) * (1 + Re2) / (4*Re2*Re2); 
    c2 = (1 + 4*Re2 + Re2*Re2) / (8*Re2*Re2); 
    c3 = -(1 + Re2) / (2*Re2*Re); 

	// Variance of the observable d
	//sigma2d_lin = (c1 + c2 * Bistatic->Bandwidth / CN + c3 * (1 + ambfcn_RD) * cos(Out->phaseRD)) / (Bistatic->t_coh * CN);
	sigma2d_lin[0] = (c1 + c2 * Bistatic->Bandwidth / CN + c3 * (1 + ambfcn_RD)) / (Bistatic->t_coh * CN);
	sigma2d_lin[1] = (c1 + c2 * Bistatic->Bandwidth / CN - c3 * (1 + ambfcn_RD)) / (Bistatic->t_coh * CN);

	// Variance of reflection coefficient
	dRdd = -2*(Re2) / (1 - Re2);
	sigma2R_lin[0] = dRdd * dRdd * sigma2d_lin[0];
	sigma2R_lin[1] = dRdd * dRdd * sigma2d_lin[1];

	// Inputs for Hinkey PDF
	mean_num = 1 + Re2; 
	mean_den = 2*Re; 
	sigma_num = sqrt( 2*(1+Re2) + Bistatic->Bandwidth/CN) / sqrt(CN*Bistatic->t_coh);
	sigma_den = sqrt( (1+Re2) + Bistatic->Bandwidth / (2*CN)) / sqrt(CN*Bistatic->t_coh); 
	rho_12 = Re * (1+ambfcn_RD) / (sigma_num*sigma_den*CN*Bistatic->t_coh);

	mean_num_scaled = mean_num/sigma_num; 
	mean_den_scaled = mean_den/sigma_den;

	hinkley_pdf2(mean_num_scaled, mean_den_scaled, 1, 1, rho_12, 10000, hinkley);

	sigma2d_h = hinkley[1]*(sigma_num/sigma_den)*(sigma_num/sigma_den);
	sigma2R_h = dRdd * dRdd * sigma2d_h;

	if(fabs(hinkley[0]) < 1.01 && fabs(hinkley[0]) > 0.99){
		//sigma2d_fused = sigma2d_h;
		sigma2R_fused = sigma2R_h;
	}
	else{
		//sigma2d_fused = MAX(sigma2d_lin[0], sigma2d_lin[1]);
		sigma2R_fused = MAX(sigma2R_lin[0], sigma2R_lin[1]);
	}

	Out->Gamma_Std_mod_lin[1]= 2*Re*sqrt(sigma2R_lin[0])*Re_scale*Re_scale;
	Out->Gamma_Std_mod_fused[1] = 2*Re*sqrt(sigma2R_fused)*Re_scale*Re_scale;
	Out->Gamma_Std_mod_fused_avg[1] = Out->Gamma_Std_mod_fused[1] / sqrt(Bistatic->N_inc);

	// Reflection coefficient with error
	if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
		Out->P_coh2_err[1] = Out->P_coh2[1] + GaussianRandom(RNG) * Out->Gamma_Std_mod_fused_avg[1];
	}
	else{
		Out->P_coh1_err[1] = Out->P_coh1[1] + GaussianRandom(RNG) * Out->Gamma_Std_mod_fused_avg[1];
	}

	// .. SNR .. //
	Out->SNR0[1] = sqrt(CN*Bistatic->t_coh)*(1+Re2) / sqrt(2*(1+Re2) + Bistatic->Bandwidth/CN);
	Out->SNR_coh1[1] = sqrt(CN*Bistatic->t_coh)*Re / sqrt(2*(1+Re2) + Bistatic->Bandwidth/CN);
}

void ReportError1D(long iSoOp)
{
	FILE *outFile;
	char filename[40];
	double R = 0;
	long iR, nR = 800;
	
	sprintf(filename, "error_model_acf_%ld.out", iSoOp);
	outFile = FileOpen(OutPath, filename, "w");	

	for(iR=0; iR<nR; iR++){
		R = R + 0.001;
		Out->P_coh1[0] = R;
		Out->P_coh1[1] = R;
		calcPower();
		calcSNR();
		error_acf();
		fprintf(outFile, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
				Out->SNR0[0], Out->SNR0[1], Out->SNR_d1[0], Out->SNR_d1[1], Out->SNR_coh1[0], Out->SNR_coh1[1],
				Out->Gamma_Std_mod_lin[0], Out->Gamma_Std_mod_lin[1],
				Out->Gamma_Std_mod_fused[0], Out->Gamma_Std_mod_fused[1],
				Out->Gamma_Std_mod_fused_avg[0], Out->Gamma_Std_mod_fused_avg[1],
				Out->P_coh1[0], Out->P_coh1_err[0],
				Out->P_coh1[1], Out->P_coh1_err[1]);
	}	
}

void ReportError2D(void)
{
	FILE *outFile;
	char filename[40];
	long iSoOp;
	long iR, nR = 50;
	//long iAng, nAng = 81;
	long iSNR, nSNR = 31;
	double CN;
	struct FixedObsType *F;

	for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
		F = &Fixed[iSoOp];
		F->polRx = RxPol[0];
		printf("\n>> SoOp Configuration #%ld: Tx Freq = %lf MHz, Rx Pol = %c\n", iSoOp+1, F->Freq*1E-6, F->polRx);

		sprintf(filename, "error_model_acf_X_2d_%ld.out", iSoOp);
		outFile = FileOpen(OutPath, filename, "w");	

		for(iSNR=0; iSNR<nSNR; iSNR++){
			CN = exp((double)(iSNR+40)/10);
		//for(iAng=0; iAng<nAng; iAng++){
			//F->thRx = iAng*D2R;
			//F->elRx = HalfPi - F->thRx;
			
			//* .. Bistatic Geometry .. *//
			calcTxGeometry(F);
			calcGeometryFixed(F);
			updateBistaticFixed(F);
			calcAntPolRot();
			
			// .. Effective Roughness Paramter .. //
			Gnd->h = F->h;

			//* .. Antenna Pattern .. * //
			if(F->AntTag == IDEAL){
				// Transmitter direction
				Bistatic->g_rt[0][0] = 1; Bistatic->g_rt[0][1] = 0;
				Bistatic->g_rt[1][0] = 0; Bistatic->g_rt[1][1] = 1;
				// Specular direction
				Bistatic->g_rs[0][0] = 1; Bistatic->g_rs[0][1] = 0;
				Bistatic->g_rs[1][0] = 0; Bistatic->g_rs[1][1] = 1;
			}
			else{
				// Transmitter direction
				LookupAntPattern(F->AntPattern, F->AntPatRes, Pi - F->AngT2R_rf[0], Pi - F->AngT2R_rf[1], Bistatic->g_rt);
				// Specular direction
				LookupAntPattern(F->AntPattern, F->AntPatRes, F->AngS2R_rf[0], F->AngS2R_rf[1], Bistatic->g_rs);
			}
			
			// Vegetation
			if(SttData->ExistVeg){
				printf("iSoOp = %ld, AngT2S_sf_th0 = %lf\n", iSoOp, Bistatic->AngT2S_sf_th0*R2D);
				calcVegT(iSoOp, 0);
			}

			// Direct Term
			calcDirectTerm();
		
			for(iR=0; iR<nR; iR++){
				Out->P_coh1[0] = iR*0.01 + 0.01;
				Out->P_coh1[1] = iR*0.01 + 0.01;
				//calcPower();
				//calcSNR();
				error_acf_CN(CN);
				fprintf(outFile, "%lf ", Out->Gamma_Std_mod_fused_avg[1]);
			}
			fprintf(outFile, "\n");
			//printf("%ld: %ld, %lf %lf, %lf %lf\n", iSoOp, iAng, Bistatic->r_tr/1000, Bistatic->r_tsr/1000, creal(Bistatic->g_rt[0][0]), creal(Bistatic->g_rs[0][0]));
		}
		fclose(outFile);
	}
}

void calcPower(void)
{
	double magK, coef;
	
	Out->K = I1 * sqrt(Bistatic->EIRP_Tx_Lin * Bistatic->G_Rx_Lin) * Bistatic->Wavelength / (4 * Pi);

	Out->Kd = Out->K * cexp(I1 * Bistatic->Wavenumber * Bistatic->r_tr) / Bistatic->r_tr;
	Out->Kc = Out->K * cexp(I1 * Bistatic->Wavenumber * Bistatic->r_tsr) / Bistatic->r_tsr;
	
	magK = cabs(Out->K);
	coef = magK * magK / (Bistatic->r_tsr*Bistatic->r_tsr);
	
	Out->PWR_coh1[0] = coef * Out->P_coh1[0];
	Out->PWR_coh1[1] = coef * Out->P_coh1[1];
	Out->PWR_coh2[0] = coef * Out->P_coh2[0];
	Out->PWR_coh2[1] = coef * Out->P_coh2[1];

	coef = magK * magK / (Bistatic->r_tr*Bistatic->r_tr);
	Out->PWR_d1[0] = coef * Out->P_d1[0];
	Out->PWR_d1[1] = coef * Out->P_d1[1];
	Out->PWR_d2[0] = coef * Out->P_d2[0];
	Out->PWR_d2[1] = coef * Out->P_d2[1];
}

void calcSNR(void)
{
	double SNR_Lin;
	SNR_Lin = Out->PWR_d1[0] * Bistatic->t_coh / (BOLTZMANN * Bistatic->Tn);
	Out->SNR_d1[0] = 10*log10(SNR_Lin);
	SNR_Lin = Out->PWR_d1[1] * Bistatic->t_coh / (BOLTZMANN * Bistatic->Tn);
	Out->SNR_d1[1] = 10*log10(SNR_Lin);
	SNR_Lin = Out->PWR_d2[0] * Bistatic->t_coh / (BOLTZMANN * Bistatic->Tn);
	Out->SNR_d2[0] = 10*log10(SNR_Lin);
	SNR_Lin = Out->PWR_d2[1] * Bistatic->t_coh / (BOLTZMANN * Bistatic->Tn);
	Out->SNR_d2[1] = 10*log10(SNR_Lin);

	SNR_Lin = Out->PWR_coh1[0] * Bistatic->t_coh / (BOLTZMANN * Bistatic->Tn);
	Out->SNR_coh1[0] = 10*log10(SNR_Lin);
	SNR_Lin = Out->PWR_coh1[1] * Bistatic->t_coh / (BOLTZMANN * Bistatic->Tn);
	Out->SNR_coh1[1] = 10*log10(SNR_Lin);
	SNR_Lin = Out->PWR_coh2[0] * Bistatic->t_coh / (BOLTZMANN * Bistatic->Tn);
	Out->SNR_coh2[0] = 10*log10(SNR_Lin);
	SNR_Lin = Out->PWR_coh2[1] * Bistatic->t_coh / (BOLTZMANN * Bistatic->Tn);
	Out->SNR_coh2[1] = 10*log10(SNR_Lin);
}

void calcRCGcoh(struct SpecularType *Sp)
{	
	Out->RCG_R = Sp->AntGainR / (Bistatic->r_ts + Bistatic->r_sr) / (Bistatic->r_ts + Bistatic->r_sr) * 1E16;
	Out->RCG_D = Sp->AntGainD / Bistatic->r_tr / Bistatic->r_tr * 1E16;
	//Out->Gamma_eff = Bistatic->r_tr / Bistatic->r_tsr * sqrt(Sp->AntGainR/Sp->AntGainD);
	
	//printf("%le %le %le %le %le\n", creal(Bistatic->g_rs[0][0]), creal(Bistatic->g_rt[0][0]), Out->RCG_R, Out->RCG_D, Out->Gamma_eff);
/*
	Out->RCG_R = creal(Bistatic->g_rs[0][1]) / (Bistatic->r_ts*Bistatic->r_ts) / (Bistatic->r_sr*Bistatic->r_sr) * 1E27;
	Out->RCG_D = creal(Bistatic->g_rt[0][1]) / (Bistatic->r_tr*Bistatic->r_tr) * 1E15;
	Gamma_eff = Bistatic->r_tr / Bistatic->r_tsr * sqrt(creal(Bistatic->g_rs[0][1])/creal(Bistatic->g_rt[0][1]));
	
	printf("%le %le %le %le %le\n", creal(Bistatic->g_rs[0][1]), creal(Bistatic->g_rt[0][1]), Out->RCG_R, Out->RCG_D, Gamma_eff);
	printf("\n");*/
}

void calcRCGincoh(struct SpecularType *Sp)
{	
	Out->RCG_R = Sp->AntGainR / (Bistatic->r_ts*Bistatic->r_ts) / (Bistatic->r_sr*Bistatic->r_sr) * 1E27;
}

void calcDirectTerm(void)
{
	double complex g_t0[2][2] = {{1, 0}, {0, 1}}, G_t0[4][4]; // Ideal transmitter antenna pattern
	double complex G_r[4][4]; // receiver antenna pattern
	double complex U_tr[4][4];
	double complex e_t10[2] = {1, 0}, e_t20[2] = {0, 1}; 
	double complex E_t10[4] = {1, 0, 0, 0}, E_t20[4] = {0, 0, 0, 1}; // Transmitter Pol State
	double complex E_t1[4], E_t2[4];
	double complex Q[4][4] = {{1, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 0}, {0, -I1, -I1, 0}};
	double complex temp1_V2[2], temp2_V2[2], temp1_V4[4], temp2_V4[4];

	M4xV4_complex(Q, E_t10, E_t1);
	M4xV4_complex(Q, E_t20, E_t2);

	calcMueller(Bistatic->g_rt, G_r);
	calcMueller(g_t0, G_t0);
	calcMueller(Bistatic->u_tr, U_tr);

	// Field
	M2xV2_complex(g_t0, e_t10, temp1_V2);
	M2xV2_complex(Bistatic->u_tr, temp1_V2, temp2_V2);
	M2xV2_complex(Bistatic->g_rt, temp2_V2, Out->b_d1);

	M2xV2_complex(g_t0, e_t20, temp1_V2);
	M2xV2_complex(Bistatic->u_tr, temp1_V2, temp2_V2);
	M2xV2_complex(Bistatic->g_rt, temp2_V2, Out->b_d2);

	// Power
	M4xV4_complex(G_t0, E_t1, temp1_V4);
	M4xV4_complex(U_tr, temp1_V4, temp2_V4);
	M4xV4_complex(G_r, temp2_V4, temp1_V4);
	Out->P_d1[0] = creal(temp1_V4[0]);
	Out->P_d1[1] = creal(temp1_V4[1]);
	Out->P_d1[2] = creal(temp1_V4[2]);
	Out->P_d1[3] = creal(temp1_V4[3]);

	M4xV4_complex(G_t0, E_t2, temp1_V4);
	M4xV4_complex(U_tr, temp1_V4, temp2_V4);
	M4xV4_complex(G_r, temp2_V4, temp1_V4);
	Out->P_d2[0] = creal(temp1_V4[0]);
	Out->P_d2[1] = creal(temp1_V4[1]);
	Out->P_d2[2] = creal(temp1_V4[2]);
	Out->P_d2[3] = creal(temp1_V4[3]);
}

void calcSpecularTerm(void)
{
	double complex g_t0[2][2] = {{1, 0}, {0, 1}}, G_t0[4][4]; // Ideal transmitter antenna pattern
	double complex G_r[4][4]; // receiver antenna pattern
	double complex U_ts[4][4], U_sr[4][4];
	double complex e_t10[2] = {1, 0}, e_t20[2] = {0, 1}; 
	double complex E_t10[4] = {1, 0, 0, 0}, E_t20[4] = {0, 0, 0, 1}; // Transmitter Pol State
	double complex E_t1[4], E_t2[4];
	double complex Q[4][4] = {{1, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 0}, {0, -I1, -I1, 0}};
	double complex r_sb[2][2] = {{1, 0}, {0, 1}}, R_sb[4][4];
	double complex temp1_V2[2], temp2_V2[2], temp1_V4[4], temp2_V4[4];

	M4xV4_complex(Q, E_t10, E_t1);
	M4xV4_complex(Q, E_t20, E_t2);
	
	calcMueller(Bistatic->g_rs, G_r);
	calcMueller(g_t0, G_t0);
	calcMueller(Bistatic->u_ts, U_ts);
	calcMueller(Bistatic->u_sr, U_sr);

	// Reflection Coefficient
	if(GndModel == MULTI){
		calcRcMulti();
	}
	else if(GndModel == SINGLE){
		calcRcSingle();
	}
	r_sb[0][0] = Gnd->RcV;
	r_sb[1][1] = Gnd->RcH;

	// Vegetation, if any
	if(SttData->ExistVeg){
		double complex t_sv[2][2], temp1_M22[2][2], r_sv[2][2], R_sv[4][4];
		t_sv[0][0] = cexp(I1*Bistatic->ArgV);
		t_sv[0][1] = 0;
		t_sv[1][0] = 0;
		t_sv[1][1] = cexp(I1*Bistatic->ArgH);
		M2xM2_complex(t_sv, r_sb, temp1_M22);
		M2xM2_complex(temp1_M22, t_sv, r_sv);
		calcMueller(r_sv, R_sv);

		// Field - Vegetation
		//b_coh1v = g_r * u_sr * r_sv * u_ts * g_t * e_t1 ;
		//b_coh2v = g_r * u_sr * r_sv * u_ts * g_t * e_t2 ;
/*		
		M2xV2_complex(g_t0, e_t10, temp1_V2);
		M2xV2_complex(Bistatic->u_ts, temp1_V2, temp2_V2);
		M2xV2_complex(r_sv, temp2_V2, temp1_V2);
		M2xV2_complex(Bistatic->u_sr, temp1_V2, temp2_V2);
		M2xV2_complex(Bistatic->g_rs, temp2_V2, Out->b_coh1v);

		M2xV2_complex(g_t0, e_t20, temp1_V2);
		M2xV2_complex(Bistatic->u_ts, temp1_V2, temp2_V2);
		M2xV2_complex(r_sv, temp2_V2, temp1_V2);
		M2xV2_complex(Bistatic->u_sr, temp1_V2, temp2_V2);
		M2xV2_complex(Bistatic->g_rs, temp2_V2, Out->b_coh2v);
*/
		// Power - Vegetation
		//P_coh1v = G_r * U_sr * R_sv * U_ts * G_t * E_t1 ;
		//P_coh2v = G_r * U_sr * R_sv * U_ts * G_t * E_t2 ;
		
		M4xV4_complex(G_t0, E_t1, temp1_V4);
		M4xV4_complex(U_ts, temp1_V4, temp2_V4);
		M4xV4_complex(R_sv, temp2_V4, temp1_V4);
		M4xV4_complex(U_sr, temp1_V4, temp2_V4);
		M4xV4_complex(G_r, temp2_V4, temp1_V4);
		Out->P_coh1[0] = creal(temp1_V4[0]);
		Out->P_coh1[1] = creal(temp1_V4[1]);
		Out->P_coh1[2] = creal(temp1_V4[2]);
		Out->P_coh1[3] = creal(temp1_V4[3]);

		M4xV4_complex(G_t0, E_t2, temp1_V4);
		M4xV4_complex(U_ts, temp1_V4, temp2_V4);
		M4xV4_complex(R_sv, temp2_V4, temp1_V4);
		M4xV4_complex(U_sr, temp1_V4, temp2_V4);
		M4xV4_complex(G_r, temp2_V4, temp1_V4);
		Out->P_coh2[0] = creal(temp1_V4[0]);
		Out->P_coh2[1] = creal(temp1_V4[1]);
		Out->P_coh2[2] = creal(temp1_V4[2]);
		Out->P_coh2[3] = creal(temp1_V4[3]);
	}
	else{
		calcMueller(r_sb, R_sb);

		// Field - Bare soil
		//b_coh1b = g_r * u_sr * r_sb * u_ts * g_t * e_t1 ;
		//b_coh2b = g_r * u_sr * r_sb * u_ts * g_t * e_t2 ;
/*
		M2xV2_complex(g_t0, e_t10, temp1_V2);
		M2xV2_complex(Bistatic->u_ts, temp1_V2, temp2_V2);
		M2xV2_complex(r_sb, temp2_V2, temp1_V2);
		M2xV2_complex(Bistatic->u_sr, temp1_V2, temp2_V2);
		M2xV2_complex(Bistatic->g_rs, temp2_V2, Out->b_coh1b);

		M2xV2_complex(g_t0, e_t20, temp1_V2);
		M2xV2_complex(Bistatic->u_ts, temp1_V2, temp2_V2);
		M2xV2_complex(r_sb, temp2_V2, temp1_V2);
		M2xV2_complex(Bistatic->u_sr, temp1_V2, temp2_V2);
		M2xV2_complex(Bistatic->g_rs, temp2_V2, Out->b_coh2b);
*/
		// Power - Bare soil
		//P_coh1b = G_r * U_sr * R_sb * U_ts * G_t * E_t1 ;
		//P_coh2b = G_r * U_sr * R_sb * U_ts * G_t * E_t2 ;
		
		// P_coh1b = G_r0 * U_sr * R_sb * U_ts * G_t0 * E_t1
		M4xV4_complex(G_t0, E_t1, temp1_V4);
		M4xV4_complex(U_ts, temp1_V4, temp2_V4);
		M4xV4_complex(R_sb, temp2_V4, temp1_V4);
		M4xV4_complex(U_sr, temp1_V4, temp2_V4);
		M4xV4_complex(G_r, temp2_V4, temp1_V4);
		Out->P_coh1[0] = creal(temp1_V4[0]);
		Out->P_coh1[1] = creal(temp1_V4[1]);
		Out->P_coh1[2] = creal(temp1_V4[2]);
		Out->P_coh1[3] = creal(temp1_V4[3]);
		
		// P_coh2b = G_r0 * U_sr * R_sb * U_ts * G_t0 * E_t2
		M4xV4_complex(G_t0, E_t2, temp1_V4);
		M4xV4_complex(U_ts, temp1_V4, temp2_V4);
		M4xV4_complex(R_sb, temp2_V4, temp1_V4);
		M4xV4_complex(U_sr, temp1_V4, temp2_V4);
		M4xV4_complex(G_r, temp2_V4, temp1_V4);
		Out->P_coh2[0] = creal(temp1_V4[0]);
		Out->P_coh2[1] = creal(temp1_V4[1]);
		Out->P_coh2[2] = creal(temp1_V4[2]);
		Out->P_coh2[3] = creal(temp1_V4[3]);
	}
}

void calcSpecularTermForward(void)
{
	double complex g_t0[2][2] = {{1, 0}, {0, 1}}, G_t0[4][4]; // Ideal transmitter antenna pattern
	double complex G_r[4][4]; // receiver antenna pattern
	double complex U_ts[4][4], U_sr[4][4];
	double complex e_t10[2] = {1, 0}, e_t20[2] = {0, 1}; 
	double complex E_t10[4] = {1, 0, 0, 0}, E_t20[4] = {0, 0, 0, 1}; // Transmitter Pol State
	double complex E_t1[4], E_t2[4];
	double complex Q[4][4] = {{1, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 0}, {0, -I1, -I1, 0}};
	double complex r_sb[2][2] = {{1, 0}, {0, 1}}, R_sb[4][4];
	double complex temp1_V2[2], temp2_V2[2], temp1_V4[4], temp2_V4[4];

	M4xV4_complex(Q, E_t10, E_t1);
	M4xV4_complex(Q, E_t20, E_t2);
	
	calcMueller(Bistatic->g_rs, G_r);
	calcMueller(g_t0, G_t0);
	calcMueller(Bistatic->u_ts, U_ts);
	calcMueller(Bistatic->u_sr, U_sr);

	// Reflection Coefficient
	if(GndModel == MULTI){
		calcRcMulti();
	}
	else if(GndModel == SINGLE){
		calcRcSingle();
	}
	r_sb[0][0] = Gnd->RcV;
	r_sb[1][1] = Gnd->RcH;

	calcMueller(r_sb, R_sb);
	
	// Field - Bare soil
	// b_coh1b = g_r * u_sr * r_sb * u_ts * g_t * e_t1 ;
	// b_coh2b = g_r * u_sr * r_sb * u_ts * g_t * e_t2 ;

	M2xV2_complex(g_t0, e_t10, temp1_V2);
	M2xV2_complex(Bistatic->u_ts, temp1_V2, temp2_V2);
	M2xV2_complex(r_sb, temp2_V2, temp1_V2);
	M2xV2_complex(Bistatic->u_sr, temp1_V2, temp2_V2);
	M2xV2_complex(Bistatic->g_rs, temp2_V2, Out->b_coh1b);

	M2xV2_complex(g_t0, e_t20, temp1_V2);
	M2xV2_complex(Bistatic->u_ts, temp1_V2, temp2_V2);
	M2xV2_complex(r_sb, temp2_V2, temp1_V2);
	M2xV2_complex(Bistatic->u_sr, temp1_V2, temp2_V2);
	M2xV2_complex(Bistatic->g_rs, temp2_V2, Out->b_coh2b);

	// Power - Bare soil
	// P_coh1b = G_r * U_sr * R_sb * U_ts * G_t * E_t1 ;
	// P_coh2b = G_r * U_sr * R_sb * U_ts * G_t * E_t2 ;
	
	// P_coh1b = G_r0 * U_sr * R_sb * U_ts * G_t0 * E_t1
	M4xV4_complex(G_t0, E_t1, temp1_V4);
	M4xV4_complex(U_ts, temp1_V4, temp2_V4);
	M4xV4_complex(R_sb, temp2_V4, temp1_V4);
	M4xV4_complex(U_sr, temp1_V4, temp2_V4);
	M4xV4_complex(G_r, temp2_V4, temp1_V4);
	Out->P_coh1b[0] = creal(temp1_V4[0]);
	Out->P_coh1b[1] = creal(temp1_V4[1]);
	Out->P_coh1b[2] = creal(temp1_V4[2]);
	Out->P_coh1b[3] = creal(temp1_V4[3]);
	
	// P_coh2b = G_r0 * U_sr * R_sb * U_ts * G_t0 * E_t2
	M4xV4_complex(G_t0, E_t2, temp1_V4);
	M4xV4_complex(U_ts, temp1_V4, temp2_V4);
	M4xV4_complex(R_sb, temp2_V4, temp1_V4);
	M4xV4_complex(U_sr, temp1_V4, temp2_V4);
	M4xV4_complex(G_r, temp2_V4, temp1_V4);
	Out->P_coh2b[0] = creal(temp1_V4[0]);
	Out->P_coh2b[1] = creal(temp1_V4[1]);
	Out->P_coh2b[2] = creal(temp1_V4[2]);
	Out->P_coh2b[3] = creal(temp1_V4[3]);

	Out->P_coh1[0] = Out->P_coh1b[0];
	Out->P_coh1[1] = Out->P_coh1b[1];
	Out->P_coh1[2] = Out->P_coh1b[2];
	Out->P_coh1[3] = Out->P_coh1b[3];

	Out->P_coh2[0] = Out->P_coh2b[0];
	Out->P_coh2[1] = Out->P_coh2b[1];
	Out->P_coh2[2] = Out->P_coh2b[2];
	Out->P_coh2[3] = Out->P_coh2b[3];

	// Vegetation, if any
	if(SttData->ExistVeg){
		double complex t_sv[2][2], temp1_M22[2][2], r_sv[2][2], R_sv[4][4];
		
		// Optical depth
		Out->tauH = 2 * cos(Bistatic->th) * cimag(Bistatic->ArgH);
		Out->tauV = 2 * cos(Bistatic->th) * cimag(Bistatic->ArgV);
//		Out->tauH = cos(Bistatic->th) * cimag(Bistatic->ArgH) / Bistatic->Wavenumber;
//		Out->tauV = cos(Bistatic->th) * cimag(Bistatic->ArgV) / Bistatic->Wavenumber;

		t_sv[0][0] = cexp(I1*Bistatic->ArgV);
		t_sv[0][1] = 0;
		t_sv[1][0] = 0;
		t_sv[1][1] = cexp(I1*Bistatic->ArgH);
		M2xM2_complex(t_sv, r_sb, temp1_M22);
		M2xM2_complex(temp1_M22, t_sv, r_sv);
		calcMueller(r_sv, R_sv);

		// Field - Vegetation
		// b_coh1v = g_r * u_sr * r_sv * u_ts * g_t * e_t1 ;
		// b_coh2v = g_r * u_sr * r_sv * u_ts * g_t * e_t2 ;

		M2xV2_complex(g_t0, e_t10, temp1_V2);
		M2xV2_complex(Bistatic->u_ts, temp1_V2, temp2_V2);
		M2xV2_complex(r_sv, temp2_V2, temp1_V2);
		M2xV2_complex(Bistatic->u_sr, temp1_V2, temp2_V2);
		M2xV2_complex(Bistatic->g_rs, temp2_V2, Out->b_coh1v);

		M2xV2_complex(g_t0, e_t20, temp1_V2);
		M2xV2_complex(Bistatic->u_ts, temp1_V2, temp2_V2);
		M2xV2_complex(r_sv, temp2_V2, temp1_V2);
		M2xV2_complex(Bistatic->u_sr, temp1_V2, temp2_V2);
		M2xV2_complex(Bistatic->g_rs, temp2_V2, Out->b_coh2v);

		// Power - Vegetation
		// P_coh1v = G_r * U_sr * R_sv * U_ts * G_t * E_t1 ;
		// P_coh2v = G_r * U_sr * R_sv * U_ts * G_t * E_t2 ;
		
		M4xV4_complex(G_t0, E_t1, temp1_V4);
		M4xV4_complex(U_ts, temp1_V4, temp2_V4);
		M4xV4_complex(R_sv, temp2_V4, temp1_V4);
		M4xV4_complex(U_sr, temp1_V4, temp2_V4);
		M4xV4_complex(G_r, temp2_V4, temp1_V4);

		Out->P_coh1v[0] = creal(temp1_V4[0]);
		Out->P_coh1v[1] = creal(temp1_V4[1]);
		Out->P_coh1v[2] = creal(temp1_V4[2]);
		Out->P_coh1v[3] = creal(temp1_V4[3]);

		M4xV4_complex(G_t0, E_t2, temp1_V4);
		M4xV4_complex(U_ts, temp1_V4, temp2_V4);
		M4xV4_complex(R_sv, temp2_V4, temp1_V4);
		M4xV4_complex(U_sr, temp1_V4, temp2_V4);
		M4xV4_complex(G_r, temp2_V4, temp1_V4);

		Out->P_coh2v[0] = creal(temp1_V4[0]);
		Out->P_coh2v[1] = creal(temp1_V4[1]);
		Out->P_coh2v[2] = creal(temp1_V4[2]);
		Out->P_coh2v[3] = creal(temp1_V4[3]);

		Out->P_coh1[0] = Out->P_coh1v[0];
		Out->P_coh1[1] = Out->P_coh1v[1];
		Out->P_coh1[2] = Out->P_coh1v[2];
		Out->P_coh1[3] = Out->P_coh1v[3];

		Out->P_coh2[0] = Out->P_coh2v[0];
		Out->P_coh2[1] = Out->P_coh2v[1];
		Out->P_coh2[2] = Out->P_coh2v[2];
		Out->P_coh2[3] = Out->P_coh2v[3];
	}
}

void calcSpecularTermSimple(struct FixedObsType *F, double VWC)
{
	double complex g_t0[2][2] = {{1, 0}, {0, 1}}, G_t0[4][4]; // Ideal transmitter antenna pattern
	double complex G_r[4][4]; // receiver antenna pattern
	double complex U_ts[4][4], U_sr[4][4];
	double complex e_t10[2] = {1, 0};//, e_t20[2] = {0, 1}; 
	double complex E_t10[4] = {1, 0, 0, 0}; //, E_t20[4] = {0, 0, 0, 1}; // Transmitter Pol State
	double complex E_t1[4], E_t2[4];
	double complex Q[4][4] = {{1, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 0}, {0, -I1, -I1, 0}};
	double complex r_sb[2][2] = {{1, 0}, {0, 1}}, R_sb[4][4];
	double complex temp1_V4[4], temp2_V4[4];

	M4xV4_complex(Q, E_t10, E_t1);
	//M4xV4_complex(Q, E_t20, E_t2);
	
	calcMueller(Bistatic->g_rs, G_r);
	calcMueller(g_t0, G_t0);
	calcMueller(Bistatic->u_ts, U_ts);
	calcMueller(Bistatic->u_sr, U_sr);

	// Reflection Coefficient
	if(GndModel == MULTI){
		calcRcMulti();
	}
	else if(GndModel == SINGLE){
		calcRcSingle();
	}
	r_sb[0][0] = Gnd->RcV;
	r_sb[1][1] = Gnd->RcH;

	calcMueller(r_sb, R_sb);

	// Power - Bare soil
	//P_coh1b = G_r * U_sr * R_sb * U_ts * G_t * E_t1 ;
	//P_coh2b = G_r * U_sr * R_sb * U_ts * G_t * E_t2 ;
	
	// P_coh1b = G_r0 * U_sr * R_sb * U_ts * G_t0 * E_t1
	M4xV4_complex(G_t0, E_t1, temp1_V4);
	M4xV4_complex(U_ts, temp1_V4, temp2_V4);
	M4xV4_complex(R_sb, temp2_V4, temp1_V4);
	M4xV4_complex(U_sr, temp1_V4, temp2_V4);
	M4xV4_complex(G_r, temp2_V4, temp1_V4);
	Out->P_coh1[0] = creal(temp1_V4[0]);
	Out->P_coh1[1] = creal(temp1_V4[1]);
	//Out->P_coh1[2] = creal(temp1_V4[2]);
	//Out->P_coh1[3] = creal(temp1_V4[3]);
	
	// P_coh2b = G_r0 * U_sr * R_sb * U_ts * G_t0 * E_t2
	//M4xV4_complex(G_t0, E_t2, temp1_V4);
	//M4xV4_complex(U_ts, temp1_V4, temp2_V4);
	//M4xV4_complex(R_sb, temp2_V4, temp1_V4);
	//M4xV4_complex(U_sr, temp1_V4, temp2_V4);
	//M4xV4_complex(G_r, temp2_V4, temp1_V4);
	//Out->P_coh2[0] = creal(temp1_V4[0]);
	//Out->P_coh2[1] = creal(temp1_V4[1]);
	//Out->P_coh2[2] = creal(temp1_V4[2]);
	//Out->P_coh2[3] = creal(temp1_V4[3]);

	// Vegetation, if any
	if(SttData->ExistVeg){
		double VOD_Co, VOD_X;
								
		if(F->polRx == 'R'){
			VOD_Co = b_LUT[F->ID][0] * VWC; // RR-pol
			VOD_X = b_LUT[F->ID][1] * VWC; // RL-pol
			Out->P_coh1[0] *= exp(-2*VOD_Co/cos(Bistatic->th));
			Out->P_coh1[1] *= exp(-2*VOD_X/cos(Bistatic->th));
//			Out->P_coh1[1] *= exp(-4*cimag(Bistatic->ArgH));
		}
		else if(F->polRx == 'X'){
			VOD_Co = b_LUT[F->ID][2] * VWC; // RX-pol
			VOD_X = b_LUT[F->ID][3] * VWC; // RY-pol
			Out->P_coh1[0] *= exp(-2*VOD_Co/cos(Bistatic->th));
			Out->P_coh1[1] *= exp(-2*VOD_X/cos(Bistatic->th));
//			Out->P_coh1[0] *= exp(-4*cimag(Bistatic->ArgV));
//			Out->P_coh1[1] *= exp(-4*cimag(Bistatic->ArgH));
		}
		else{
			printf("error\n");
			exit(1);
		}
	}
}

void ReportSMP(void)
{	
	static FILE *outFile1, *outFile2, *outFile3;
	static char First = TRUE;
	static long ind_10cm, ind_20cm, ind_50cm;
	long ISublayer;

	if(First){
		First = FALSE;
		outFile1 = FileOpen(OutPath, "SMP.out", "w");
		outFile2 = FileOpen(OutPath, "SMP_eff.out", "w");
		outFile3 = FileOpen(OutPath, "POME.out", "w");

		ind_10cm = round((0.1 - 0.05) / MultiLayer->delZ);
		ind_20cm = round((0.2 - 0.05) / MultiLayer->delZ);
		ind_50cm = round((0.5 - 0.05) / MultiLayer->delZ);
	}

	for(ISublayer=0; ISublayer<MultiLayer->NSublayer; ISublayer++){
		fprintf(outFile1, "%lf ", Gnd->SMP[ISublayer]);
		fprintf(outFile2, "%lf ", MultiLayer->SMP[ISublayer]);
	}
	fprintf(outFile1, "\n");
	fprintf(outFile2, "\n");

	fprintf(outFile3, "%lf %lf %lf %lf %lf %lf %lf %lf %lf\n", MultiLayer->sur, MultiLayer->bott, MultiLayer->avg, MultiLayer->infl_val,
			Gnd->SMP[0], Gnd->SMP[ind_10cm], Gnd->SMP[ind_20cm], Gnd->SMP[ind_50cm], Gnd->SMP[MultiLayer->NSublayer-1]);

/*	if(EndFlag){
		fclose(outFile1);
		fclose(outFile2);
		fclose(outFile3);
	}
*/
}

void ReportGridLandmask(void)
{	
	FILE *outFile;
	long iGrid, iRow, iCol;
	char filename[20];

	for(iGrid=0; iGrid<NGrid; iGrid++){
		sprintf(filename, "Grid%ld_landmask.out", iGrid+1);
		outFile = FileOpen(OutPath, filename, "w");

		for(iRow=0; iRow<Grid[iGrid].nRow; iRow++){
			for(iCol=0; iCol<Grid[iGrid].nCol; iCol++)
				fprintf(outFile, "%d ", Design->landmask[iRow][iCol]);
			fprintf(outFile, "\n");
		}
		fclose(outFile);
	}
}

void ReportGrid(void)
{	
	FILE *outFile1, *outFile2;
	long iGrid, iRow, iCol;
	char filename[20];

	for(iGrid=0; iGrid<NGrid; iGrid++){
		sprintf(filename, "Grid%ld_lat.out", iGrid+1);
		outFile1 = FileOpen(OutPath, filename, "w");
		sprintf(filename, "Grid%ld_lon.out", iGrid+1);
		outFile2 = FileOpen(OutPath, filename, "w");

		for(iRow=0; iRow<Grid[iGrid].nRow; iRow++){
			fprintf(outFile1, "%lf\n", Grid[iGrid].lat[iRow]*R2D);
		}
		for(iCol=0; iCol<Grid[iGrid].nCol; iCol++)
			fprintf(outFile2, "%lf\n", Grid[iGrid].lon[iCol]*R2D);

		fclose(outFile1);
		fclose(outFile2);
	}
}

void ReportNoSignal(struct ScType *Rx)
{
	static uint8_t First = TRUE;
    static FILE *outFile;
	
	if(First){
		First = FALSE;
		outFile = FileOpen(OutPath, "noSignalZone.out", "w");
	}

	fprintf(outFile, "%lf %lf %lf %lf\n", UTC.JulDay, Rx->PosN[0], Rx->PosN[1], Rx->PosN[2]);
}


void ReportTrajectory(void)
{
    static uint8_t First = TRUE;
    static FILE *outFile;
	long iRx;
	struct ScType *Rx;
	if (First) {
    	char filename[50];
        First = FALSE;
		sprintf(filename,"%s.out", RxName);
		outFile = FileOpen(OutPath,filename,"w");
    }

	iRx = 100;
	// Julian Day, Position of Receiver in ECI [m], Velocity of Receiver in ECI [m/s]
//    fprintf(outFile, "%lf %lf %lf %lf %lf %lf %lf\n", UTC.JulDay, SC[iRx].PosN[0], SC[iRx].PosN[1], SC[iRx].PosN[2], SC[iRx].VelN[0], SC[iRx].VelN[1], SC[iRx].VelN[2]);

	Rx = &Sc[iRx];
	// ECI to ECEF of Receiver's Position
	MxV(World[EARTH].CWN, Rx->PosN, Rx->PosW);
	// ECEF to GRS80
	ECEFToGRS80(Rx->PosW, &Rx->lat, &Rx->lon, &Rx->alt);

    fprintf(outFile, "%lf %lf %lf %lf\n", UTC.JulDay, Rx->lat*R2D, Rx->lon*R2D, Rx->alt);
}

void ReportFixedObs(struct FixedObsType *F, long iData)
{
	static FILE *outFile1, *outFile2, *outFile3, *outFile4, *outFile5, *outFile6, *outFile7, *outFile8, *outFile9, *outFile10, *outFile11, *outFile12, *outFile13, *outFile14, *outFile15;
	static char First = TRUE;
	long iSoOp;
	char filename[30];

	if(First){
		First = FALSE;
		iSoOp = F->ID;

		sprintf(filename, "reflect_coeff_CO_B_%ld_%c.out", iSoOp, F->polRx);
		outFile1 = FileOpen(OutPath, filename, "w");

		sprintf(filename, "reflect_coeff_X_B_%ld_%c.out", iSoOp, F->polRx);
		outFile2 = FileOpen(OutPath, filename, "w");

		sprintf(filename, "reflectivity_CO_B_%ld_%c.out", iSoOp, F->polRx);
		outFile3 = FileOpen(OutPath, filename, "w");

		sprintf(filename, "reflectivity_X_B_%ld_%c.out", iSoOp, F->polRx);
		outFile4 = FileOpen(OutPath, filename, "w");
		
		if(SttData->ExistVeg){
			sprintf(filename, "reflect_coeff_CO_V_%ld_%c.out", iSoOp, F->polRx);
			outFile12 = FileOpen(OutPath, filename, "w");

			sprintf(filename, "reflect_coeff_X_V_%ld_%c.out", iSoOp, F->polRx);
			outFile13 = FileOpen(OutPath, filename, "w");

			sprintf(filename, "reflectivity_CO_V_%ld_%c.out", iSoOp, F->polRx);
			outFile14 = FileOpen(OutPath, filename, "w");

			sprintf(filename, "reflectivity_X_V_%ld_%c.out", iSoOp, F->polRx);
			outFile15 = FileOpen(OutPath, filename, "w");
		}

		sprintf(filename, "field_D_CO_%ld_%c.out", iSoOp, F->polRx);
		outFile5 = FileOpen(OutPath, filename, "w");

		sprintf(filename, "field_D_X_%ld_%c.out", iSoOp, F->polRx);
		outFile6 = FileOpen(OutPath, filename, "w");

		sprintf(filename, "Power_D_CO_%ld_%c.out", iSoOp, F->polRx);
		outFile7 = FileOpen(OutPath, filename, "w");
		
		sprintf(filename, "Power_D_X_%ld_%c.out", iSoOp, F->polRx);
		outFile8 = FileOpen(OutPath, filename, "w");

		sprintf(filename, "factors_%ld_%c.out", iSoOp, F->polRx);
		outFile9 = FileOpen(OutPath, filename, "w");

		if(GndData->flagPenDep){
			sprintf(filename, "depth_%ld_%c.out", iSoOp, F->polRx);
			outFile10 = FileOpen(OutPath, filename, "w");
		}

		if(SmpModel == POME){
			outFile11 = FileOpen(OutPath, "ANC_POME.out", "w");
		}

		// Direct Field Co-pol
		fprintf(outFile5, "%lf %lf\n", creal(Out->b_d1[0]), cimag(Out->b_d1[0]));
		// Direct Power Co-pol
		fprintf(outFile7, "%lf\n", Out->P_d1[0]);

		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
			// Direct Filed X-pol
			fprintf(outFile6, "%lf %lf\n", creal(Out->b_d2[1]), cimag(Out->b_d2[1]));
			// Direct Power X-pol
			fprintf(outFile8, "%lf\n", Out->P_d2[1]);
		}
		else{
			// Direct Filed X-pol
			fprintf(outFile6, "%lf %lf\n", creal(Out->b_d1[1]), cimag(Out->b_d1[1]));
			// Direct Power X-pol
			fprintf(outFile8, "%lf\n", Out->P_d1[1]);
		}
		fprintf(outFile9, "%le %le %le %le %le %le\n",
				creal(Out->K), cimag(Out->K), creal(Out->Kd), cimag(Out->Kd), creal(Out->Kc), cimag(Out->Kc));

		fclose(outFile5);
		fclose(outFile6);
		fclose(outFile7);
		fclose(outFile8);
		fclose(outFile9);
	}

	// Reflection Coefficient Co-pol
	fprintf(outFile1, "%lf %lf\n", creal(Out->b_coh1b[0]), cimag(Out->b_coh1b[0])); // XX or RR or RX
	// Reflectivity Co-pol
//	fprintf(outFile3, "%lf %lf %lf\n", Out->P_coh1b[0], Out->P_coh1_err[0], Out->Gamma_Std_mod_fused_avg[0]); // XX or RR or RX
	fprintf(outFile3, "%lf\n", Out->P_coh1b[0]); // XX or RR or RX

	if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
		// Reflection Coefficient X-pol
		fprintf(outFile2, "%lf %lf\n", creal(Out->b_coh2b[1]), cimag(Out->b_coh2b[1])); // YY
		// Reflectivity X-pol
//		fprintf(outFile4, "%lf %lf %lf\n", Out->P_coh2b[1], Out->P_coh2_err[1], Out->Gamma_Std_mod_fused_avg[1]); // YY
		fprintf(outFile4, "%lf\n", Out->P_coh2b[1]); // YY
	}
	else{
		// Reflection Coefficient X-pol
		fprintf(outFile2, "%lf %lf\n", creal(Out->b_coh1b[1]), cimag(Out->b_coh1b[1])); // RL or RY
		// Reflectivity X-pol
//		fprintf(outFile4, "%lf %lf %lf\n", Out->P_coh1b[1], Out->P_coh1_err[1], Out->Gamma_Std_mod_fused_avg[1]); // RL or RY
		fprintf(outFile4, "%lf\n", Out->P_coh1b[1]); // RL or RY
	}

	if(SttData->ExistVeg){
		// Reflection Coefficient Co-pol
		fprintf(outFile12, "%lf %lf\n", creal(Out->b_coh1v[0]), cimag(Out->b_coh1v[0])); // XX or RR or RX
		// Reflectivity Co-pol
//		fprintf(outFile14, "%lf %lf %lf\n", Out->P_coh1v[0], Out->P_coh1_err[0], Out->Gamma_Std_mod_fused_avg[0]); // XX or RR or RX
		fprintf(outFile14, "%lf\n", Out->P_coh1v[0]); // XX or RR or RX

		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
			// Reflection Coefficient X-pol
			fprintf(outFile13, "%lf %lf\n", creal(Out->b_coh2v[1]), cimag(Out->b_coh2v[1])); // YY
			// Reflectivity X-pol
//			fprintf(outFile15, "%lf %lf %lf\n", Out->P_coh2v[1], Out->P_coh2_err[1], Out->Gamma_Std_mod_fused_avg[1]); // YY
			fprintf(outFile15, "%lf\n", Out->P_coh2v[1]); // YY
		}
		else{
			// Reflection Coefficient X-pol
			fprintf(outFile13, "%lf %lf\n", creal(Out->b_coh1v[1]), cimag(Out->b_coh1v[1])); // RL or RY
			// Reflectivity X-pol
//			fprintf(outFile15, "%lf %lf %lf\n", Out->P_coh1v[1], Out->P_coh1_err[1], Out->Gamma_Std_mod_fused_avg[1]); // RL or RY
			fprintf(outFile15, "%lf\n", Out->P_coh1v[1]); // RL or RY
		}
	}
	
	// Penetration Depth
	if(GndData->flagPenDep){
		fprintf(outFile10, "%lf %lf\n", Gnd->pdh, Gnd->pdv);
	}

	if(SmpModel == POME){
		fprintf(outFile11, "%lf %lf %lf %lf\n", Gnd->poro, Gnd->resid, Gnd->fc, Gnd->wp);
	}

	if(iData == NData-1){
		First = TRUE;
		fclose(outFile1);
		fclose(outFile2);
		fclose(outFile3);
		fclose(outFile4);
		if(SttData->ExistVeg){
			fclose(outFile12);
			fclose(outFile13);
			fclose(outFile14);
			fclose(outFile15);
		}
		if(GndData->flagPenDep){
			fclose(outFile10);
		}
		if(SmpModel == POME){
			fclose(outFile11);
		}
	}
}

void ReportSpaceObs(struct ScType *Rx, struct SoOpType *S, struct SpecularType *Sp, struct ScType *Tx)
{
    static uint8_t First = TRUE;
    static FILE ***outFile;
	long iRx, iSoOp, iLayer, i;

	if (First) {
    	char filename[50];
        First = FALSE;
		outFile = (FILE ***)calloc(NRx, sizeof(FILE **));
		if (outFile == NULL) {
			printf("outFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
        for(iRx=0; iRx<NRx; iRx++){
			outFile[iRx] = (FILE **)calloc(NSoOp, sizeof(FILE *));
			if (outFile[iRx] == NULL) {
				printf("outFile[iRx] calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
        	for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
				sprintf(filename,"%s%ld_%s.out", RxName, iRx+1, SoOp[iSoOp].TxName);
				outFile[iRx][iSoOp] = FileOpen(OutPath,filename,"w");
            }
        }
    }

	iRx = Rx->ID;
	iSoOp = S->ID;

	// Julian Day
	if(ReportSoOp->JulDay_Sp)
		fprintf(outFile[iRx][iSoOp], "%lf ", UTC.JulDay);
	// Date
	if(ReportSoOp->Date)
		fprintf(outFile[iRx][iSoOp], "%ld %ld %ld %ld %ld %.1lf ", UTC.Year, UTC.Month, UTC.Day, UTC.Hour, UTC.Minute, UTC.Second);
	// LLA of Receiver [deg][deg][m]
	if(ReportSoOp->LLA_Rx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Rx->lat*R2D, Rx->lon*R2D, Rx->alt);		
	// Position of Receiver in ECEF [m]
	if(ReportSoOp->PosW_Rx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Rx->PosW[0], Rx->PosW[1], Rx->PosW[2]);
	// Velocity of Receiver in ECEF [m/s]
	if(ReportSoOp->VelW_Rx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Rx->VelW[0], Rx->VelW[1], Rx->VelW[2]);
	// Transmitter ID number
	if(ReportSoOp->TxID)
		fprintf(outFile[iRx][iSoOp], "%ld ", Sp->TxID);
	// LLA of Transmitter [deg][deg][m]
	if(ReportSoOp->LLA_Tx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Tx->lat*R2D, Tx->lon*R2D, Tx->alt);
	// Position of Transmitter in ECEF [m]
	if(ReportSoOp->PosW_Tx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Tx->PosW[0], Tx->PosW[1], Tx->PosW[2]);
	// Velocity of Transmitter in ECEF [m/s]
	if(ReportSoOp->VelW_Tx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Tx->VelW[0], Tx->VelW[1], Tx->VelW[2]);
	// LLA of Specular Point [deg][deg][m]
	if(ReportSoOp->LLA_Sp)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Sp->lat*R2D, Sp->lon*R2D, Sp->alt);
	// Position of Specular Point in ECEF [m]
	if(ReportSoOp->PosW_Sp)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Sp->PosW[0], Sp->PosW[1], Sp->PosW[2]);
	// Line of Sight of Direct Signal (Azimuth, Elevation) [deg][deg]
	if(ReportSoOp->LoS_D)
		fprintf(outFile[iRx][iSoOp], "%lf %lf ", Sp->lookAngAzD*R2D, Sp->lookAngElD*R2D);
	// Line of Sight of Reflected Signal (Azimuth, Elevation) [deg][deg]
	if(ReportSoOp->LoS_R)
		fprintf(outFile[iRx][iSoOp], "%lf %lf ", Sp->lookAngAzR*R2D, Sp->lookAngElR*R2D);
	// Delay and Doppler for Direct and Reflected Signal
	if(ReportSoOp->DelayDoppler){
		calcDelayDoppler(Rx->PosN, Rx->VelN, Sp, Tx);
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf %lf ", Out->delayD, Out->dopplerD, Out->delayR, Out->dopplerR);
	}
	// Antenna Gain of Direct signal [dB]
	if(ReportSoOp->Gain_D)
		fprintf(outFile[iRx][iSoOp], "%lf ", Sp->AntGainD);
	// Antenna Gain of Reflected Signal [dB]
	if(ReportSoOp->Gain_R)
		fprintf(outFile[iRx][iSoOp], "%lf ", Sp->AntGainR);
	// First Fresnel Zone (a,b,azimuth) [m][m][deg]
	if(ReportSoOp->FresZone){
		Sp->b = sqrt(4*Bistatic->Wavelength*Rx->alt*sin(Sp->elev)+Bistatic->Wavelength*Bistatic->Wavelength) / sin(Sp->elev)/2;
		Sp->a = Sp->b / sin(Sp->elev);
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Sp->a, Sp->b, Sp->az*R2D);
	}
	// Incidence Angle [deg]
	if(ReportSoOp->IncAng)
		fprintf(outFile[iRx][iSoOp], "%lf ", Sp->incAng*R2D);
	// Elevation Angle [deg]
	if(ReportSoOp->elev)
		fprintf(outFile[iRx][iSoOp], "%lf ", Sp->elev*R2D);
	// Roll, Pitch, Yaw [deg][deg][deg]
	if(ReportSoOp->RPY_to_Sp){
		calcRPY(Sp);
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Sp->roll*R2D, Sp->pitch*R2D, Sp->yaw*R2D);
	}
	// Land Mask [1:Land, 0:Water]
	if(ReportSoOp->LandMask)
		fprintf(outFile[iRx][iSoOp], "%d ", Gnd->landMask);
	// Volumetric Soil Moisture [m^3/m^3]
	if(ReportSoOp->VSM)
		for(iLayer=0; iLayer<NLayer; iLayer++)
			fprintf(outFile[iRx][iSoOp], "%f ", Gnd->VSM[iLayer]);
	// Soil Temperature [K]
	if(ReportSoOp->SoilTemp)
		fprintf(outFile[iRx][iSoOp], "%f ", Gnd->soilTemp1);
	// Freeze/Thaw State
	if(ReportSoOp->FreezeThaw)
		fprintf(outFile[iRx][iSoOp], "%d ", Gnd->freezeThaw);
	// Soil Texture [Sand][Clay][Bulk density]
	if(ReportSoOp->SoilTexture)
		fprintf(outFile[iRx][iSoOp], "%f %f %f ", Gnd->sand, Gnd->clay, Gnd->bulk);
	// Snow Cover [%]
	if(ReportSoOp->SnowCover){
		fprintf(outFile[iRx][iSoOp], "%d ", Gnd->snowCover);
	}

	if(ReportSoOp->RcCO || ReportSoOp->RcX || ReportSoOp->GammaCO || ReportSoOp->GammaX || ReportSoOp->SNR_D || ReportSoOp->SNR_R){
		// Ground Structure
		if(isnan(Gnd->VSM[0])){
			for(i=0; i<2; i++){
				Out->b_coh1b[i] = NAN;
				Out->b_coh2b[i] = NAN;
				Out->P_coh1b[i] = NAN;
				Out->P_coh2b[i] = NAN;
			}
		}
		else{
			if(GndModel == MULTI){
				calcMLDielProfile();
			}
			else if(GndModel == SINGLE){
				calcDielMironov();
			}
			calcSpecularTerm();
		}
		if(ReportSoOp->SNR_D || ReportSoOp->SNR_R){
			calcDirectTerm();
			calcPower();
			calcSNR();
		}
		S->t_inc = S->sres_req / MAGV(Rx->VelW);
		S->N_inc = floor(S->t_inc/S->t_coh);
		Bistatic->N_inc = S->N_inc;
		error_acf();
	}

	// Reflection Coefficient Co-pol
	if(ReportSoOp->RcCO){
		fprintf(outFile[iRx][iSoOp], "%lf %lf ", creal(Out->b_coh1[0]), cimag(Out->b_coh1[0])); // XX or RR or RX
	}			
	// Reflection Coefficient X-pol
	if(ReportSoOp->RcX){
		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X')
			fprintf(outFile[iRx][iSoOp], "%lf %lf ", creal(Out->b_coh2[1]), cimag(Out->b_coh2[1])); // YY
		else
			fprintf(outFile[iRx][iSoOp], "%lf %lf ", creal(Out->b_coh1[1]), cimag(Out->b_coh1[1])); // RL or RY
	}
	// Reflectivity Co-pol
	if(ReportSoOp->GammaCO){
		fprintf(outFile[iRx][iSoOp], "%lf %lf ", Out->P_coh1[0], Out->P_coh1_err[0]); // XX or RR or RX
	}			
	// Reflectivity X-pol
	if(ReportSoOp->GammaX){
		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X')
			fprintf(outFile[iRx][iSoOp], "%lf %lf ", Out->P_coh2[1], Out->P_coh2_err[1]); // YY
		else
			fprintf(outFile[iRx][iSoOp], "%lf %lf ", Out->P_coh1[1], Out->P_coh1_err[1]); // RL or RY
	}
	// SNR of Direct signal Co,X-pol
	if(ReportSoOp->SNR_D){
		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X')
			fprintf(outFile[iRx][iSoOp], "%lf %lf ", Out->SNR_d1[0], Out->SNR_d2[1]); // XX, YY
		else
			fprintf(outFile[iRx][iSoOp], "%lf %lf ", Out->SNR_d1[0], Out->SNR_d1[1]); // RR, RL or RX, RY
	}
	// SNR of Reflected signal Co,X-pol
	if(ReportSoOp->SNR_R){
		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X')
			fprintf(outFile[iRx][iSoOp], "%lf %lf ", Out->SNR_coh1[0], Out->SNR_coh2[1]); // XX, YY
		else
			fprintf(outFile[iRx][iSoOp], "%lf %lf ", Out->SNR_coh1[0], Out->SNR_coh1[1]); // RR, RL or RX, RY
	}
	// MUOS Spot Beam Number for Receiver and Specular Point
	if(ReportSoOp->SbNum){
		checkMUOSspotbeam(Rx, Sp, &SpotBeam[Tx->ID - S->RefOrbFirst]);
		fprintf(outFile[iRx][iSoOp], "%ld %ld ", Sp->SpotbeamID[0], Sp->SpotbeamID[1]);
	}
	fprintf(outFile[iRx][iSoOp], "\n");
}

void ReportTowerObs(struct TwrType *T, struct SoOpType *S, struct SpecularType *Sp, struct ScType *Tx)
{
    static uint8_t First = TRUE;
    static FILE ***outFile;
	long iRx, iSoOp;

	if (First) {
    	char filename[50];
        First = FALSE;
		outFile = (FILE ***)calloc(NRx, sizeof(FILE **));
		if (outFile == NULL) {
			printf("outFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
        for(iRx=0; iRx<NRx; iRx++){
			outFile[iRx] = (FILE **)calloc(NSoOp, sizeof(FILE *));
			if (outFile[iRx] == NULL) {
				printf("outFile[iRx] calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
        	for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
				sprintf(filename,"%s_%s.out", T->label, SoOp[iSoOp].TxName);
				outFile[iRx][iSoOp] = FileOpen(OutPath,filename,"w");
            }
        }
    }

	iRx = T->ID;
	iSoOp = S->ID;
	// Julian Day
	if(ReportSoOp->JulDay_Sp)
		fprintf(outFile[iRx][iSoOp], "%lf ", UTC.JulDay);
	// Date
	if(ReportSoOp->Date)
		fprintf(outFile[iRx][iSoOp], "%ld %ld %ld %ld %ld %.1lf ", UTC.Year, UTC.Month, UTC.Day, UTC.Hour, UTC.Minute, UTC.Second);
	// LLA of Receiver [deg][deg][m]
	if(ReportSoOp->LLA_Rx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", T->lat*R2D, T->lon*R2D, T->alt);		
	// Position of Receiver in ECEF [m]
	if(ReportSoOp->PosW_Rx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", T->PosW[0], T->PosW[1], T->PosW[2]);
	// Velocity of Receiver in ECEF [m/s]
	if(ReportSoOp->VelW_Rx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", T->VelW[0], T->VelW[1], T->VelW[2]);
	// Transmitter ID number
	if(ReportSoOp->TxID)
		fprintf(outFile[iRx][iSoOp], "%ld ", Sp->TxID);
	// LLA of Transmitter [deg][deg][m]
	if(ReportSoOp->LLA_Tx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Tx->lat*R2D, Tx->lon*R2D, Tx->alt);
	// Position of Transmitter in ECEF [m]
	if(ReportSoOp->PosW_Tx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Tx->PosW[0], Tx->PosW[1], Tx->PosW[2]);
	// Velocity of Transmitter in ECEF [m/s]
	if(ReportSoOp->VelW_Tx)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Tx->VelW[0], Tx->VelW[1], Tx->VelW[2]);
	// LLA of Specular Point [deg][deg][m]
	if(ReportSoOp->LLA_Sp)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Sp->lat*R2D, Sp->lon*R2D, Sp->alt);
	// Position of Specular Point in ECEF [m]
	if(ReportSoOp->PosW_Sp)
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Sp->PosW[0], Sp->PosW[1], Sp->PosW[2]);
	// Line of Sight of Direct Signal (Azimuth, Elevation) [deg][deg]
	if(ReportSoOp->LoS_D)
		fprintf(outFile[iRx][iSoOp], "%lf %lf ", Sp->lookAngAzD*R2D, Sp->lookAngElD*R2D);
	// Line of Sight of Reflected Signal (Azimuth, Elevation) [deg][deg]
	if(ReportSoOp->LoS_R)
		fprintf(outFile[iRx][iSoOp], "%lf %lf ", Sp->lookAngAzR*R2D, Sp->lookAngElR*R2D);
	// Delay and Doppler for Direct and Reflected Signal
	if(ReportSoOp->DelayDoppler){
		calcDelayDoppler(T->PosN, T->VelN, Sp, Tx);
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf %lf ", Out->delayD, Out->dopplerD, Out->delayR, Out->dopplerR);
	}
	// Antenna Gain of Direct signal [dB]
	if(ReportSoOp->Gain_D)
		fprintf(outFile[iRx][iSoOp], "%lf ", Sp->AntGainD);
	// Antenna Gain of Reflected Signal [dB]
	if(ReportSoOp->Gain_R)
		fprintf(outFile[iRx][iSoOp], "%lf ", Sp->AntGainR);
	// First Fresnel Zone (a,b,azimuth) [m][m][deg]
	if(ReportSoOp->FresZone){
		Sp->b = sqrt(4*S->Wavelength*T->alt*sin(Sp->elev)+S->Wavelength*S->Wavelength) / sin(Sp->elev)/2;
		Sp->a = Sp->b / sin(Sp->elev);
		fprintf(outFile[iRx][iSoOp], "%lf %lf %lf ", Sp->a, Sp->b, Sp->az*R2D);
	}
	// Incidence Angle [deg]
	if(ReportSoOp->IncAng)
		fprintf(outFile[iRx][iSoOp], "%lf ", Sp->incAng*R2D);
	// Elevation Angle [deg]
	if(ReportSoOp->elev)
		//fprintf(outFile[iRx][iSoOp], "%lf ", Sp->elev*R2D);
		fprintf(outFile[iRx][iSoOp], "%lf ", T->elev*R2D);

	fprintf(outFile[iRx][iSoOp], "\n");
}

void postProcessCalVal(uint8_t RxNum, uint8_t TxNum)
{
    FILE *infile, *outfile;
	uint16_t arc[2000] = {0}, iArc = 0, NArc;
	uint32_t iLine = 0, NLine = 0;

	uint8_t flagValid = 0, flagWater =0 , flagVisible = 0;

	char filename[30], chr;
	char TxName[4][10] = {"MUOS", "Orbcomm", "NOAA", "METEOR"};

	sprintf(filename, "CalVal_%s_Case%d.42", TxName[TxNum], RxNum);
	infile=FileOpen(SimPath,filename,"r");
	chr = getc(infile);
	while(chr!=EOF){
		if(chr=='\n')
			NLine++;
		chr = getc(infile);
	}
	printf("# lines of %s = %d\n", filename, NLine);
	rewind(infile);

    Data = (struct PostProcType *) calloc(NLine,sizeof(struct PostProcType));

	fscanf(infile, "%hu %lf %hu %hu %hu %lf %lf %lf %hu %f %f %f %f %f %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
			&Data[iLine].arc, &Data[iLine].t, &Data[iLine].TxID, &Data[iLine].site, &Data[iLine].visible,
			&Data[iLine].lat, &Data[iLine].lon, &Data[iLine].alt,
			&Data[iLine].landMask, &Data[iLine].m_v, &Data[iLine].Temp, &Data[iLine].sand, &Data[iLine].clay, &Data[iLine].rho_b,
			&Data[iLine].gammaLR, &Data[iLine].SnrD, &Data[iLine].SnrR,
			&Data[iLine].a, &Data[iLine].b, &Data[iLine].az,
			&Data[iLine].lookAngAzD, &Data[iLine].lookAngElD, &Data[iLine].lookAngAzR, &Data[iLine].lookAngElR);
	for(iLine = 1; iLine<NLine; iLine++){
		fscanf(infile, "%hu %lf %hu %hu %hu %lf %lf %lf %hu %f %f %f %f %f %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
				&Data[iLine].arc, &Data[iLine].t, &Data[iLine].TxID, &Data[iLine].site, &Data[iLine].visible,
				&Data[iLine].lat, &Data[iLine].lon, &Data[iLine].alt,
				&Data[iLine].landMask, &Data[iLine].m_v, &Data[iLine].Temp, &Data[iLine].sand, &Data[iLine].clay, &Data[iLine].rho_b,
				&Data[iLine].gammaLR, &Data[iLine].SnrD, &Data[iLine].SnrR,
				&Data[iLine].a, &Data[iLine].b, &Data[iLine].az,
				&Data[iLine].lookAngAzD, &Data[iLine].lookAngElD, &Data[iLine].lookAngAzR, &Data[iLine].lookAngElR);

		if(Data[iLine].arc > Data[iLine-1].arc){
			flagValid = 0;
			flagVisible = 0;
			flagWater = 0;
		}

		if(Data[iLine].visible)
			flagVisible = 1;

		if(Data[iLine].landMask==0)
			flagWater = 1;

		if(flagVisible && flagWater && flagValid==0){
			arc[iArc++] = Data[iLine].arc;
			flagValid = 1;
			flagVisible = 0;
			flagWater = 0;
		}
	}
	NArc = iArc;
	iArc = 0;

	printf("# valid arcs of %s = %d\n", filename, NArc);

	strcat(filename,".arc");
	outfile = FileOpen(SimPath,filename,"w");

	for(iLine = 1; iLine<NLine; iLine++){
		if(Data[iLine].arc > arc[iArc] && iArc < NArc){
			iArc++;
		}
		if(Data[iLine].arc == arc[iArc]){
			fprintf(outfile, "%hu %lf %hu %hu %hu %lf %lf %lf %hu %f %f %f %f %f %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
					Data[iLine].arc, Data[iLine].t, Data[iLine].TxID, Data[iLine].site, Data[iLine].visible,
					Data[iLine].lat, Data[iLine].lon, Data[iLine].alt,
					Data[iLine].landMask, Data[iLine].m_v, Data[iLine].Temp, Data[iLine].sand, Data[iLine].clay, Data[iLine].rho_b,
					Data[iLine].gammaLR, Data[iLine].SnrD, Data[iLine].SnrR,
					Data[iLine].a, Data[iLine].b, Data[iLine].az,
					Data[iLine].lookAngAzD, Data[iLine].lookAngElD, Data[iLine].lookAngAzR, Data[iLine].lookAngElR);
		}
	}
	printf("%s saved!\n", filename);
}
