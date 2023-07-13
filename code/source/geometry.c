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

void FindSP_MPL(struct ScType *Rx, struct SpecularType *Sp, struct ScType *Tx)
{
	double estCurr[3], corrEst[3], estNext[3], estGradi[3], corr[3];
	double PosWR[3], PosWT[3], ST[3], SR[3], SurfaceNorVec[3], BistaticVec[3];
	double MAG1, MAG2, Lat;
//	double temp[3];

//	double vecRx2SpN[3], vecNadir[3];
	// Earth ellipsoid
    double a = 6378137.0;
    //double f = 1.0/298.257223563;	// WGS84
	double f = 1.0/298.257222100882711; // GRS80
    double b = a*(1.0-f);
    double r;
    // Tolerance
	double sigma = 0.1;
	double K;
	double correction = 20;
	long cnt = 0;

	K = Sp->K;
	//K = 400000.0;
	PosWR[0] = Rx->PosW[0];
	PosWR[1] = Rx->PosW[1];
	PosWR[2] = Rx->PosW[2];

	PosWT[0] = Tx->PosW[0];
	PosWT[1] = Tx->PosW[1];
	PosWT[2] = Tx->PosW[2];

	MAG1 = PosWR[0]*PosWR[0] + PosWR[1]*PosWR[1] + PosWR[2]*PosWR[2];
	MAG2 = PosWT[0]*PosWT[0] + PosWT[1]*PosWT[1] + PosWT[2]*PosWT[2];

	/* Unconstrained Weighted Initial Estimate */
	estCurr[0] = PosWR[0]/MAG1 + PosWT[0]/MAG2;
	estCurr[1] = PosWR[1]/MAG1 + PosWT[1]/MAG2;
	estCurr[2] = PosWR[2]/MAG1 + PosWT[2]/MAG2;

	/* Initial specular point guess, on the surface directly below R */
	MAG1 = sqrt(estCurr[0]*estCurr[0] + estCurr[1]*estCurr[1] + estCurr[2]*estCurr[2]);
    Lat = asin(estCurr[2]/MAG1); // Geocentric latitude
    r = a*b/sqrt(a*a*sin(Lat)*sin(Lat)+b*b*cos(Lat)*cos(Lat));
//    r = a*sqrt((1-e*e)/(1-(e*e*cos(Lat)*cos(Lat))));

	estCurr[0] = r/MAG1*estCurr[0];
	estCurr[1] = r/MAG1*estCurr[1];
	estCurr[2] = r/MAG1*estCurr[2];

	while(correction > sigma && cnt<20000) {
		// Gain adaptation
		//if(correction<10) K=100000;

	    /* Step 1: Calculate gradient of path length function */
		ST[0] = estCurr[0]-PosWT[0];
		ST[1] = estCurr[1]-PosWT[1];
		ST[2] = estCurr[2]-PosWT[2];

		SR[0] = estCurr[0]-PosWR[0];
		SR[1] = estCurr[1]-PosWR[1];
		SR[2] = estCurr[2]-PosWR[2];

		MAG1 = sqrt(ST[0]*ST[0] + ST[1]*ST[1] + ST[2]*ST[2]);
		MAG2 = sqrt(SR[0]*SR[0] + SR[1]*SR[1] + SR[2]*SR[2]);

		estGradi[0] = ST[0]/MAG1 + SR[0]/MAG2;
		estGradi[1] = ST[1]/MAG1 + SR[1]/MAG2;
		estGradi[2] = ST[2]/MAG1 + SR[2]/MAG2;

		BistaticVec[0] = -estGradi[0];
		BistaticVec[1] = -estGradi[1];
		BistaticVec[2] = -estGradi[2];

		MAG1 = sqrt(BistaticVec[0]*BistaticVec[0] + BistaticVec[1]*BistaticVec[1] + BistaticVec[2]*BistaticVec[2]);
		BistaticVec[0] /= MAG1;
		BistaticVec[1] /= MAG1;
		BistaticVec[2] /= MAG1;

		SurfaceNorVec[0] = estCurr[0]*2.0/a/a;
		SurfaceNorVec[1] = estCurr[1]*2.0/a/a;
		SurfaceNorVec[2] = estCurr[2]*2.0/b/b;	

	    /* Step 2 */
		// Tangential correction optimization
		//VxV(SurfaceNorVec,estGradi,temp);
		//VxV(temp, SurfaceNorVec, estGradi);
		//////////////////////////////////////////
		MAG2 = sqrt(SurfaceNorVec[0]*SurfaceNorVec[0] + SurfaceNorVec[1]*SurfaceNorVec[1] + SurfaceNorVec[2]*SurfaceNorVec[2]);
		SurfaceNorVec[0] /= MAG2;
		SurfaceNorVec[1] /= MAG2;
		SurfaceNorVec[2] /= MAG2;

		corrEst[0] = estCurr[0] - K*estGradi[0];
		corrEst[1] = estCurr[1] - K*estGradi[1];
		corrEst[2] = estCurr[2] - K*estGradi[2];

		/* Step 3 */
		MAG1 = sqrt(corrEst[0]*corrEst[0] + corrEst[1]*corrEst[1] + corrEst[2]*corrEst[2]);
	    Lat = asin(corrEst[2]/MAG1);
	    r = a*b/sqrt(a*a*sin(Lat)*sin(Lat)+b*b*cos(Lat)*cos(Lat));

		estNext[0] = r/MAG1*corrEst[0];
		estNext[1] = r/MAG1*corrEst[1];
		estNext[2] = r/MAG1*corrEst[2];

	    /* Step 4 */
		corr[0] = estNext[0]-estCurr[0];
		corr[1] = estNext[1]-estCurr[1];
		corr[2] = estNext[2]-estCurr[2];

		correction = sqrt(corr[0]*corr[0] + corr[1]*corr[1] + corr[2]*corr[2]);
//		printf("c:%le %le %le\n", estCurr[0], estCurr[1], estCurr[2]);
//		printf("n:%le %le %le\n", estNext[0], estNext[1], estNext[2]);
//		printf("n:%le %le %le\n", corr[0], corr[1], corr[2]);
		estCurr[0] = estNext[0];
		estCurr[1] = estNext[1];
		estCurr[2] = estNext[2];
		cnt++;
	}
	Sp->gradient = MAGV(estGradi);
	double RS[3], TS[3];
	//CopyUnitV(estCurr,uSpPosW);
	ST[0] = PosWT[0]-estCurr[0];
	ST[1] = PosWT[1]-estCurr[1];
	ST[2] = PosWT[2]-estCurr[2];

	SR[0] = PosWR[0]-estCurr[0];
	SR[1] = PosWR[1]-estCurr[1];
	SR[2] = PosWR[2]-estCurr[2];

	MAG1 = sqrt(ST[0]*ST[0] + ST[1]*ST[1] + ST[2]*ST[2]);
	MAG2 = sqrt(SR[0]*SR[0] + SR[1]*SR[1] + SR[2]*SR[2]);

	BistaticVec[0] = ST[0]/MAG1 + SR[0]/MAG2;
	BistaticVec[1] = ST[1]/MAG1 + SR[1]/MAG2;
	BistaticVec[2] = ST[2]/MAG1 + SR[2]/MAG2;
	UNITV(BistaticVec);

	SurfaceNorVec[0] = estCurr[0]*2.0/a/a;
	SurfaceNorVec[1] = estCurr[1]*2.0/a/a;
	SurfaceNorVec[2] = estCurr[2]*2.0/b/b;
	UNITV(SurfaceNorVec);

	RS[0] = PosWR[0] - estCurr[0];
	RS[1] = PosWR[1] - estCurr[1];
	RS[2] = PosWR[2] - estCurr[2];
	UNITV(RS);

	TS[0] = PosWT[0] - estCurr[0];
	TS[1] = PosWT[1] - estCurr[1];
	TS[2] = PosWT[2] - estCurr[2];
	UNITV(TS);

	Sp->incAng2 = acos(VoV(SurfaceNorVec, TS));
	Sp->angErr = Sp->incAng2 - acos(VoV(SurfaceNorVec, RS));
	Sp->angBisNor = acos(VoV(BistaticVec, SurfaceNorVec));
	Sp->correction = correction;
	Sp->cnt = cnt;
	//printf("%ld\n", cnt);
	Sp->PosW[0] = estCurr[0];
	Sp->PosW[1] = estCurr[1];
	Sp->PosW[2] = estCurr[2];

	/* Check visibility of specular point from receiver*/
//	vecRx2SpN[0] = Sp->PosN[0] - Rx->PosN[0];
//	vecRx2SpN[1] = Sp->PosN[1] - Rx->PosN[1];
//	vecRx2SpN[2] = Sp->PosN[2] - Rx->PosN[2];

//	UNITV(vecRx2SpN);
//	vecNadir[0] = -Rx->PosN[0];
//	vecNadir[1] = -Rx->PosN[1];
//	vecNadir[2] = -Rx->PosN[2];
//	UNITV(vecNadir);

//	Sp->lookAng = acos(VoV(vecRx2SpN,vecNadir));
	//printf("%lf\n", Sp->lookAng*R2D);

	//if(Sp->lookAng<FOV[Rx->ID].Width/2)
	//	Sp->Exists = 1;
}

void updateBistaticFixed(struct FixedObsType *F)
{
	Bistatic->Freq = F->Freq; // Transmitter Frequency [Hz]
	Bistatic->polTx = F->polTx; // Transmit Antenna Polarization
	Bistatic->Wavelength = F->Wavelength; // [m]
	Bistatic->Wavenumber = TwoPi / Bistatic->Wavelength;
	Bistatic->th = F->thTx;
	Bistatic->EIRP_Tx = F->EIRP_Tx; // Transmitter EIRP [dB]
	Bistatic->EIRP_Tx_Lin = pow(10, F->EIRP_Tx/10);
	Bistatic->G_Rx = F->G_Rx; // Receive Antenna Gain [dB]
	Bistatic->G_Rx_Lin= pow(10, F->G_Rx/10);
	Bistatic->t_coh = F->t_coh;
	Bistatic->Bandwidth = F->Bandwidth;
	Bistatic->Tn = F->Tn;
	Bistatic->N_inc = F->N_inc;
}

void updateBistaticInorbit(struct SoOpType *S, double incAng)
{
	Bistatic->Freq = S->Freq;
	Bistatic->Wavelength = S->Wavelength;
	Bistatic->Wavenumber = TwoPi / S->Wavelength;
	Bistatic->th = incAng;
	Bistatic->EIRP_Tx = S->EIRP_Tx;
	Bistatic->EIRP_Tx_Lin = pow(10, S->EIRP_Tx/10);
	Bistatic->G_Rx = S->G_Rx;
	Bistatic->G_Rx_Lin = pow(10, S->G_Rx/10);
	Bistatic->polRx = S->polRx;
	Bistatic->polTx = S->polTx;
	Bistatic->t_coh = S->t_coh;
	Bistatic->Bandwidth = S->Bandwidth;
	Bistatic->Tn = S->NoiseTemS;
	Bistatic->N_inc = S->N_inc;
}

// Return a length of a tangent line to the curve of the earth and an external angle between the line and pos
void tanLineLengAng(double *pos, double *length, double *angle)
{
	double R = World[EARTH].rad;
	double r2 = pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2];

	*length = sqrt(r2 - R*R);
	*angle = Pi - atan2(R, *length);
}

void FindSP_UD(double *PosWR, double *PosWT, struct SpecularType *Sp)
{
	double estCurr[3], corrEst[3], estNext[3], estGradi[3], corr[3];
	double SurfaceNorVec[3], BistaticVec[3], TS[3], RS[3];
	double MAG1, MAG2, Lat;

	// Earth Ellipsoid
    double a = 6378137.0;
    //double f = 1.0/298.257223563; // WGS84
	double f = 1.0/298.257222100882711; // GRS80
    double b = a*(1.0-f);
    double r;
 //   double e = sqrt(2.0*f-f*f);
    // Tolerance
	double sigma = 0.03;
	double K;
	double correction = 20;
	long cnt = 0;

	K = Sp->K;

	MAG1 = PosWR[0]*PosWR[0] + PosWR[1]*PosWR[1] + PosWR[2]*PosWR[2];
	MAG2 = PosWT[0]*PosWT[0] + PosWT[1]*PosWT[1] + PosWT[2]*PosWT[2];

	/* Unconstrained Weighted Initial Estimate */
	estCurr[0] = PosWR[0]/MAG1 + PosWT[0]/MAG2;
	estCurr[1] = PosWR[1]/MAG1 + PosWT[1]/MAG2;
	estCurr[2] = PosWR[2]/MAG1 + PosWT[2]/MAG2;

	/* Constrain back to the surface */
	MAG1 = sqrt(estCurr[0]*estCurr[0] + estCurr[1]*estCurr[1] + estCurr[2]*estCurr[2]);
    Lat = asin(estCurr[2]/MAG1); // Geocentric latitude
    r = a*b/sqrt(a*a*sin(Lat)*sin(Lat)+b*b*cos(Lat)*cos(Lat));
//    r = a*sqrt((1-e*e)/(1-(e*e*cos(Lat)*cos(Lat))));

	estCurr[0] = r/MAG1*estCurr[0];
	estCurr[1] = r/MAG1*estCurr[1];
	estCurr[2] = r/MAG1*estCurr[2];

	//printf("TxID = %ld, Lat = %lf, r = %lf, estCurr = [%lf, %lf, %lf]\n", Sp->TxID, Lat*R2D, r/1e3, estCurr[0], estCurr[1], estCurr[2]);
	
	while(correction > sigma && cnt<1E7) {
		// Gain adaptation
		//if(correction<10 && ObsBase == SPACE){
			//printf("Adjust gain (%lf)\n", correction);
		//	K=100000;
		//}
		if(cnt==9999999){
			printf("Exceed max iteration. Adjust gain K=%lf for UD method. TxID: %ld\n", Sp->K, Sp->TxID);
			exit(1);
		} 
	    /* Step 1: Calculate gradient of path length function */
		TS[0] = PosWT[0]-estCurr[0];
		TS[1] = PosWT[1]-estCurr[1];
		TS[2] = PosWT[2]-estCurr[2];

		RS[0] = PosWR[0]-estCurr[0];
		RS[1] = PosWR[1]-estCurr[1];
		RS[2] = PosWR[2]-estCurr[2];

		MAG1 = sqrt(TS[0]*TS[0] + TS[1]*TS[1] + TS[2]*TS[2]);
		MAG2 = sqrt(RS[0]*RS[0] + RS[1]*RS[1] + RS[2]*RS[2]);

		BistaticVec[0] = TS[0]/MAG1 + RS[0]/MAG2;
		BistaticVec[1] = TS[1]/MAG1 + RS[1]/MAG2;
		BistaticVec[2] = TS[2]/MAG1 + RS[2]/MAG2;

		MAG1 = sqrt(BistaticVec[0]*BistaticVec[0] + BistaticVec[1]*BistaticVec[1] + BistaticVec[2]*BistaticVec[2]);

		BistaticVec[0] /= MAG1;
		BistaticVec[1] /= MAG1;
		BistaticVec[2] /= MAG1;

		SurfaceNorVec[0] = estCurr[0]*2.0/a/a;
		SurfaceNorVec[1] = estCurr[1]*2.0/a/a;
		SurfaceNorVec[2] = estCurr[2]*2.0/b/b;

		MAG2 = sqrt(SurfaceNorVec[0]*SurfaceNorVec[0] + SurfaceNorVec[1]*SurfaceNorVec[1] + SurfaceNorVec[2]*SurfaceNorVec[2]);

		SurfaceNorVec[0] /= MAG2;
		SurfaceNorVec[1] /= MAG2;
		SurfaceNorVec[2] /= MAG2;

		estGradi[0] = BistaticVec[0] - SurfaceNorVec[0];
		estGradi[1] = BistaticVec[1] - SurfaceNorVec[1];
		estGradi[2] = BistaticVec[2] - SurfaceNorVec[2];

	    /* Step 2 */
		// Tangential correction optimization
		//VxV(SurfaceNorVec,estGradi,temp);
		//VxV(temp, SurfaceNorVec, estGradi);

		corrEst[0] = estCurr[0] + K*estGradi[0];
		corrEst[1] = estCurr[1] + K*estGradi[1];
		corrEst[2] = estCurr[2] + K*estGradi[2];

		/* Step 3 */
		MAG1 = sqrt(corrEst[0]*corrEst[0] + corrEst[1]*corrEst[1] + corrEst[2]*corrEst[2]);
	    Lat = asin(corrEst[2]/MAG1); // Geocentric latitude
	    r = a*b/sqrt(a*a*sin(Lat)*sin(Lat)+b*b*cos(Lat)*cos(Lat));
	    //r = a*sqrt((1-e*e)/(1-(e*e*cos(Lat)*cos(Lat))));

		estNext[0] = r/MAG1*corrEst[0];
		estNext[1] = r/MAG1*corrEst[1];
		estNext[2] = r/MAG1*corrEst[2];

	    /* Step 4 */
		corr[0] = estNext[0]-estCurr[0];
		corr[1] = estNext[1]-estCurr[1];
		corr[2] = estNext[2]-estCurr[2];

		correction = sqrt(corr[0]*corr[0] + corr[1]*corr[1] + corr[2]*corr[2]);

		estCurr[0] = estNext[0];
		estCurr[1] = estNext[1];
		estCurr[2] = estNext[2];
		cnt++;
	}
	//printf("%ld, %lf, TxID: %ld\n", cnt, correction, Sp->TxID);
	Sp->PosW[0] = estCurr[0];
	Sp->PosW[1] = estCurr[1];
	Sp->PosW[2] = estCurr[2];

	// Validation
	#if 0
	RS[0] = PosWR[0] - estCurr[0];
	RS[1] = PosWR[1] - estCurr[1];
	RS[2] = PosWR[2] - estCurr[2];
	UNITV(RS);

	TS[0] = PosWT[0] - estCurr[0];
	TS[1] = PosWT[1] - estCurr[1];
	TS[2] = PosWT[2] - estCurr[2];
	UNITV(TS);

	Sp->incAng2 = acos(VoV(SurfaceNorVec, TS));
	Sp->scaAng = acos(VoV(SurfaceNorVec, RS));
	Sp->angErr = Sp->incAng2 - Sp->scaAng;

	Sp->angBisNor = acos(VoV(BistaticVec, SurfaceNorVec));
	Sp->correction = correction;
	Sp->cnt = cnt;
	#endif
}

/**********************************************************************/
/* Calculate swath area of FOV. Assume spherical Earth, symmetric FOV */
void FindSwathArea(long RxNum)
{
	double alpha;
	double alpha1;
	double alpha2;
	double CBL[3][3];
	double vl[3] = {0.0, 0.0, 1.0};
	double vb[3];
	double theta, theta1, theta2, thetaMax;
	double halfFOV;
	double ratio;

	ratio = MAGV(Sc[RxNum].PosN)/World[EARTH].rad;
	thetaMax = asin(1/ratio);
	halfFOV = FOV[RxNum].Width/2;

	/* DCM from LocalGrid to Body */
	MT(Sc[RxNum].CLN,CBL);
	/* +Z vector of Local frame is expressed in Body frame */
	MxV(CBL,vl,vb);
	/* Angle between +Z vector in Local and Body */
	theta = acos(VoV(vl,vb));

	/* Take into account FOV */
	if(theta > halfFOV){
		theta1 = theta - halfFOV;
		theta2 = Limit_max(theta + halfFOV,thetaMax);

		alpha1 = asin(sin(theta1)*ratio)-theta1;
		alpha2 = asin(sin(theta2)*ratio)-theta2;

		alpha = alpha2 - alpha1;
	}
	else if(theta < halfFOV){
		theta1 = Limit_max(halfFOV - theta,thetaMax);
		theta2 = Limit_max(halfFOV + theta,thetaMax);

		alpha1 = asin(sin(theta1)*ratio)-theta1;
		alpha2 = asin(sin(theta2)*ratio)-theta2;

		alpha = alpha2 + alpha1;
	}
	else{
		theta1 = theta;
		theta2 = Limit_max(halfFOV + theta,thetaMax);

		alpha1 = asin(sin(theta1)*ratio)-theta1;
		alpha2 = asin(sin(theta2)*ratio)-theta2;

		alpha = alpha2 + alpha1;
	}

	//Rx[RxNum].SwathWidth = alpha/TwoPi*World[EARTH].rad;
	//Rx[RxNum].SwathArea = TwoPi*World[EARTH].rad*World[EARTH].rad*(1-cos(alpha/2));
}

long *FindVisibleTx(struct ScType *Rx, long *TxNum, long *TxCnt)
{
	static long bufferTx[100];
	static double bufferAngLoS[100];
	double Rhat[3], RelPosN[3];
	long ISoOp, ITx, TxClosest[100];
	long tempTx, cntTx = 0, cnt, i, j;
	double tempLoS, angLoS, angLoSmax, dist;
    double tanLineLength, tanLineAngle;

    CopyUnitV(Rx->PosN,Rhat);

    tanLineLengAng(Rx->PosN, &tanLineLength, &tanLineAngle);
    
	angLoSmax = HalfPi;
//	angLoSmax = tanLineAngle - 10*D2R;
	
    /* Aim at Tx closest to Rx's Zenith */
    for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
		if (!strcmp(SoOp[ISoOp].TxName,"MUOS") && SoOp[ISoOp].NSp == 1){
	    	cnt = 0;
			if(Rx->orbitNum%2 == 0){
				for(ITx=SoOp[ISoOp].RefOrbFirst; ITx<SoOp[ISoOp].RefOrbLast; ITx++) {
					RelPosN[0] = Sc[ITx].PosN[0] - Rx->PosN[0];
					RelPosN[1] = Sc[ITx].PosN[1] - Rx->PosN[1];
					RelPosN[2] = Sc[ITx].PosN[2] - Rx->PosN[2];
					UNITV(RelPosN);
					angLoS = acos(VoV(RelPosN,Rhat));
					if(angLoS < angLoSmax) {
						bufferTx[cnt] = ITx;
						bufferAngLoS[cnt++] = angLoS;
						break;
					}
				}
			}
			else{
				for(ITx=SoOp[ISoOp].RefOrbLast-1; ITx>=SoOp[ISoOp].RefOrbFirst; ITx--) {
					RelPosN[0] = Sc[ITx].PosN[0] - Rx->PosN[0];
					RelPosN[1] = Sc[ITx].PosN[1] - Rx->PosN[1];
					RelPosN[2] = Sc[ITx].PosN[2] - Rx->PosN[2];
					UNITV(RelPosN);
					angLoS = acos(VoV(RelPosN,Rhat));
					if(angLoS < angLoSmax) {
						bufferTx[cnt] = ITx;
						bufferAngLoS[cnt++] = angLoS;
						break;
					}
				}
			}
			// Sort ascending order
			for(i=0; i<cnt; ++i) {
				for (j=i+1; j<cnt; ++j) {
					if (bufferAngLoS[i] > bufferAngLoS[j]) {
						tempLoS = bufferAngLoS[i];
						bufferAngLoS[i] = bufferAngLoS[j];
						bufferAngLoS[j] = tempLoS;

						tempTx = bufferTx[i];
						bufferTx[i] = bufferTx[j];
						bufferTx[j] = tempTx;
					}
				}
			}
			if(cnt > SoOp[ISoOp].NSp){
				for(i=0; i<SoOp[ISoOp].NSp; i++)
					TxClosest[cntTx++] = bufferTx[i];
			}
			else
				for(i=0; i<cnt; i++)
					TxClosest[cntTx++] = bufferTx[i];		
		}
		else{
			cnt = 0;
			for(ITx=SoOp[ISoOp].RefOrbFirst; ITx<SoOp[ISoOp].RefOrbLast; ITx++) {
				RelPosN[0] = Sc[ITx].PosN[0] - Rx->PosN[0];
				RelPosN[1] = Sc[ITx].PosN[1] - Rx->PosN[1];
				RelPosN[2] = Sc[ITx].PosN[2] - Rx->PosN[2];
				dist = UNITV(RelPosN);
				angLoS = acos(VoV(RelPosN,Rhat));
				if(angLoS < angLoSmax || dist < tanLineLength) {
					bufferTx[cnt] = ITx;
					bufferAngLoS[cnt++] = angLoS;
				}
			}
			// Sort ascending order
			for(i=0; i<cnt; ++i) {
				for (j=i+1; j<cnt; ++j) {
					if (bufferAngLoS[i] > bufferAngLoS[j]) {
						tempLoS = bufferAngLoS[i];
						bufferAngLoS[i] = bufferAngLoS[j];
						bufferAngLoS[j] = tempLoS;

						tempTx = bufferTx[i];
						bufferTx[i] = bufferTx[j];
						bufferTx[j] = tempTx;
					}
				}
			}
			if(cnt > SoOp[ISoOp].NSp){
				for(i=0; i<SoOp[ISoOp].NSp; i++)
					TxClosest[cntTx++] = bufferTx[i];
				//printf(">> You're seeing much more specular points than ever!\n");
			}
			else
				for(i=0; i<cnt; i++)
					TxClosest[cntTx++] = bufferTx[i];
		}
    }

	// Sum all
    *TxCnt = cntTx;

    TxNum = (long *) calloc(cntTx,sizeof(long));

	for(ITx=0; ITx<cntTx; ITx++) {
		TxNum[ITx] = TxClosest[ITx];
	}
	return TxNum;
}

long *FindVisibleTxTwr(double *PosN, long *TxNum, long *TxCnt)
{
	static long bufferTx[100];
	static double bufferAngLoS[100];
	double Rhat[3], RelPosN[3];
	long ISoOp, ITx, TxClosest[100];
	long tempTx, cntTx = 0, cnt, i, j;
	double tempLoS, angLoS, angLoSmax;

	angLoSmax = 110*D2R;

    CopyUnitV(PosN,Rhat);

    /* Aim at Tx closest to Rx's Zenith */
    for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
		cnt = 0;
		for(ITx=SoOp[ISoOp].RefOrbFirst; ITx<SoOp[ISoOp].RefOrbLast; ITx++) {
			RelPosN[0] = Sc[ITx].PosN[0] - PosN[0];
			RelPosN[1] = Sc[ITx].PosN[1] - PosN[1];
			RelPosN[2] = Sc[ITx].PosN[2] - PosN[2];
			UNITV(RelPosN);
			angLoS = acos(VoV(RelPosN,Rhat));
			if(angLoS < angLoSmax) {
				bufferTx[cnt] = ITx;
				bufferAngLoS[cnt++] = angLoS;
			}
		}
		// Sort ascending order
		for(i=0; i<cnt; ++i) {
			for (j=i+1; j<cnt; ++j) {
				if (bufferAngLoS[i] > bufferAngLoS[j]) {
					tempLoS = bufferAngLoS[i];
					bufferAngLoS[i] = bufferAngLoS[j];
					bufferAngLoS[j] = tempLoS;

					tempTx = bufferTx[i];
					bufferTx[i] = bufferTx[j];
					bufferTx[j] = tempTx;
				}
			}
		}
		if(cnt > SoOp[ISoOp].NSp){
			for(i=0; i<SoOp[ISoOp].NSp; i++)
				TxClosest[cntTx++] = bufferTx[i];
		}
		else
			for(i=0; i<cnt; i++)
				TxClosest[cntTx++] = bufferTx[i];
    }

	// Sum all
    *TxCnt = cntTx;

    TxNum = (long *) calloc(cntTx,sizeof(long));

	for(ITx=0; ITx<cntTx; ITx++) {
		TxNum[ITx] = TxClosest[ITx];
	}
	return TxNum;
}

uint8_t FindVisibleSpotbeamTarget(struct ScType *Rx, struct SpecularType *Sp, struct SpotBeamType *MuosSb, struct TargetAreaType *TARGET)
{
	double vTarget2RxW[3], vTarget2RxT[3];
	double vTarget2SpW[3], vTarget2SpT[3];
	double radRx, radSp, x[3], y[3], z[3];

	CopyUnitV(TARGET->PosW, z);

	vTarget2RxW[0] = Rx->PosW[0] - TARGET->PosW[0];
	vTarget2RxW[1] = Rx->PosW[1] - TARGET->PosW[1];
	vTarget2RxW[2] = Rx->PosW[2] - TARGET->PosW[2];

	vTarget2SpW[0] = Sp->PosW[0] - TARGET->PosW[0];
	vTarget2SpW[1] = Sp->PosW[1] - TARGET->PosW[1];
	vTarget2SpW[2] = Sp->PosW[2] - TARGET->PosW[2];

	// Direction Cosine Matrix from ECEF to Specular frame
	x[0] = vTarget2RxW[0] - VoV(vTarget2RxW, z)*z[0];
	x[1] = vTarget2RxW[1] - VoV(vTarget2RxW, z)*z[1];
	x[2] = vTarget2RxW[2] - VoV(vTarget2RxW, z)*z[2];
	UNITV(x);
	VxV(z,x,y);

	TARGET->CTW[0][0] = x[0];
	TARGET->CTW[0][1] = x[1];
	TARGET->CTW[0][2] = x[2];
	TARGET->CTW[1][0] = y[0];
	TARGET->CTW[1][1] = y[1];
	TARGET->CTW[1][2] = y[2];
	TARGET->CTW[2][0] = z[0];
	TARGET->CTW[2][1] = z[1];
	TARGET->CTW[2][2] = z[2];

	MxV(TARGET->CTW, vTarget2RxW, vTarget2RxT);
	if(vTarget2RxT[2]<0)
		return FALSE;
	MxV(TARGET->CTW, vTarget2SpW, vTarget2SpT);

	radRx = (MuosSb->altTx - vTarget2RxT[2]) * tan(MuosSb->halfBW);
	radSp = (MuosSb->altTx - vTarget2SpT[2]) * tan(MuosSb->halfBW);

	if((radRx - vTarget2RxT[0])>0 && (radSp - sqrt(vTarget2SpT[0]*vTarget2SpT[0]+vTarget2SpT[1]*vTarget2SpT[1]))>0)
	{
//		printf("Rx= %lf, radRx= %lf, Sp= %lf, radSp= %lf\n", vTarget2RxT[0]/1000, radSp/1000, sqrt(vTarget2SpT[0]*vTarget2SpT[0]+vTarget2SpT[1]*vTarget2SpT[1])/1000, radRx/1000);

		return TRUE; // visible: Inside spotbeam
	}
	else
		return FALSE; // invisible: Outside spotbeam
}

/* Compute delay and doppler spread around the initial specular point */
void LocalGeometry(double vSp2RxW[3], struct ScType *Rx, struct SoOpType *S, struct SpecularType *Sp, struct ScType *Tx)
{
	double vPS[3], vRxS[3], vTxS[3], vP2Rx[3], vP2Tx[3], velRxS[3], velTxS[3];
	double vTemp1[3], vTemp2[3];
	double mSp, mP2RxS, mP2TxS, mVelRxS, mVelTxS;
	long Itheta, Iphi;
	double dtheta, dphi, theta, phi, dAngMin;//, dArea;

	double x[3], y[3], z[3];
	
	// Specular frame
//	vSp2RxW[0] = Rx->PosW[0] - Sp->PosW[0];
//	vSp2RxW[1] = Rx->PosW[1] - Sp->PosW[1];
//	vSp2RxW[2] = Rx->PosW[2] - Sp->PosW[2];

	mSp = CopyUnitV(Sp->PosW, z);
	Sp->r = mSp;

	x[0] = vSp2RxW[0] - VoV(vSp2RxW, z)*z[0];
	x[1] = vSp2RxW[1] - VoV(vSp2RxW, z)*z[1];
	x[2] = vSp2RxW[2] - VoV(vSp2RxW, z)*z[2];
	UNITV(x);
	VxV(z,x,y);

	// Direction Cosine Matrix from ECEF to Specular frame
	Sp->CSW[0][0] = x[0];
	Sp->CSW[0][1] = x[1];
	Sp->CSW[0][2] = x[2];
	Sp->CSW[1][0] = y[0];
	Sp->CSW[1][1] = y[1];
	Sp->CSW[1][2] = y[2];
	Sp->CSW[2][0] = z[0];
	Sp->CSW[2][1] = z[1];
	Sp->CSW[2][2] = z[2];

	MxV(Sp->CSW, Rx->PosW, vRxS);
	MxV(Sp->CSW, Tx->PosW, vTxS);
	MxV(Sp->CSW, Rx->VelW, velRxS);
	MxV(Sp->CSW, Tx->VelW, velTxS);

			vP2Rx[0] = vRxS[0];
			vP2Rx[1] = vRxS[1];
			vP2Rx[2] = vRxS[2] - mSp;

			vP2Tx[0] = vTxS[0];
			vP2Tx[1] = vTxS[1];
			vP2Tx[2] = vTxS[2] - mSp;

			mP2RxS = UNITV(vP2Rx);
			mP2TxS = UNITV(vP2Tx);

			mVelRxS = VoV(vP2Rx,velRxS);
			mVelTxS = VoV(vP2Tx,velTxS);

			Out->delayR = (mP2RxS+mP2TxS)/LIGHTSPEED;// - Sp->LocalGrid[0][0].delay;
			Out->dopplerR = -(mVelRxS + mVelTxS)/S->Wavelength;// - Sp->LocalGrid[0][0].doppler;

	// Minimum angle step
	dAngMin = S->minSurfRes/mSp;

	// delta theta
	dtheta = asin(Sp->a/mSp)/S->NthetaLocal;
	if(dtheta < dAngMin)
		dtheta = dAngMin;

	// delta phi
	dphi = TwoPi/S->NphiLocal;
	if(dphi < dAngMin)
		dphi = dAngMin;

//	Sp->LocalGrid = (struct LocalGridType **)calloc(S->NthetaLocal, sizeof(struct LocalGridType));
	for(Itheta=0; Itheta<S->NthetaLocal; Itheta++){
    	//Sp->LocalGrid[Itheta] = (struct LocalGridType *)calloc(S->NphiLocal, sizeof(struct LocalGridType));
		for(Iphi=0; Iphi<S->NphiLocal; Iphi++){
			theta = (Itheta+1)*dtheta;
			phi = Iphi*dphi;
			Sp->LocalGrid[Itheta][Iphi].theta = theta;
			Sp->LocalGrid[Itheta][Iphi].phi = phi;

			// Differential area over surface
			Sp->LocalGrid[Itheta][Iphi].dArea = mSp*mSp*sin(theta)*dtheta*dphi;

			/* Location of surface point in the specular frame */
			vPS[0] = mSp * sin(theta) * cos(phi);
			vPS[1] = mSp * sin(theta) * sin(phi);
			vPS[2] = mSp * cos(theta);

			MTxV(Sp->CSW, vPS, Sp->LocalGrid[Itheta][Iphi].PosW);
			/* ECEF to GRS80 */
			ECEFToGRS80(Sp->LocalGrid[Itheta][Iphi].PosW, &Sp->LocalGrid[Itheta][Iphi].lat, &Sp->LocalGrid[Itheta][Iphi].lon, &Sp->LocalGrid[Itheta][Iphi].alt);

			vP2Rx[0] = vRxS[0] - vPS[0];
			vP2Rx[1] = vRxS[1] - vPS[1];
			vP2Rx[2] = vRxS[2] - vPS[2];

			vP2Tx[0] = vTxS[0] - vPS[0];
			vP2Tx[1] = vTxS[1] - vPS[1];
			vP2Tx[2] = vTxS[2] - vPS[2];

			mP2RxS = UNITV(vP2Rx);
			mP2TxS = UNITV(vP2Tx);

			mVelRxS = VoV(vP2Rx,velRxS);
			mVelTxS = VoV(vP2Tx,velTxS);

			Sp->CLS[0][0] = sin(theta)*cos(phi);
			Sp->CLS[0][1] = sin(theta)*sin(phi);
			Sp->CLS[0][2] = cos(theta);
			Sp->CLS[1][0] = cos(theta)*cos(phi);
			Sp->CLS[1][1] = cos(theta)*sin(phi);
			Sp->CLS[1][2] = -sin(theta);
			Sp->CLS[2][0] = -sin(phi);
			Sp->CLS[2][1] = cos(phi);
			Sp->CLS[2][2] = 0;

			Sp->LocalGrid[Itheta][Iphi].range = mP2RxS+mP2TxS;
			Sp->LocalGrid[Itheta][Iphi].delay = Sp->LocalGrid[Itheta][Iphi].range/LIGHTSPEED - Out->delayR;// - Sp->LocalGrid[0][0].delay;
			Sp->LocalGrid[Itheta][Iphi].doppler = -(mVelRxS + mVelTxS)/S->Wavelength - Out->dopplerR;// - Sp->LocalGrid[0][0].doppler;
			Sp->LocalGrid[Itheta][Iphi].incAngL2Rx = acos(VoV(Sp->CLS[0], vP2Rx));
			Sp->LocalGrid[Itheta][Iphi].incAngL2Tx = acos(VoV(Sp->CLS[0], vP2Tx));

			// Scattering Vector q
			vTemp1[0] = vP2Tx[0] + vP2Rx[0];
			vTemp1[1] = vP2Tx[1] + vP2Rx[1];
			vTemp1[2] = vP2Tx[2] + vP2Rx[2];

			MxV(Sp->CLS, vTemp1, vTemp2);
			Sp->LocalGrid[Itheta][Iphi].scaVec[0] = TwoPi/S->Wavelength*vTemp2[0];
			Sp->LocalGrid[Itheta][Iphi].scaVec[1] = TwoPi/S->Wavelength*vTemp2[1];
			Sp->LocalGrid[Itheta][Iphi].scaVec[2] = TwoPi/S->Wavelength*vTemp2[2];

			// Scattering angle
			vP2Tx[0] = -vP2Tx[0];
			vP2Tx[1] = -vP2Tx[1];
			vP2Tx[2] = -vP2Tx[2];
			Sp->LocalGrid[Itheta][Iphi].scaAng = 0.5*acos(VoV(vP2Tx,vP2Rx));
		}
	}
}
double bilinInterpDEM(double lat, double lon)
{
	uint16_t row0, col0, row1, row2, col1, col2;
	double lat0, lat1, lat2, lon0, lon1, lon2, height;

	row0 = (uint16_t) floor(lat*R2D*60 + 5400);
	col0 = (uint16_t) floor(lon*R2D*60 + 10800);
	lat0 = ((double) row0/60 - 89.991666666666660)*D2R;
	lon0 = ((double) col0/60 - 179.9916666666667)*D2R;

	// Quadrant 1
	if(lat > lat0 && lon < lon0){
		row1 = row0;
		row2 = row0 + 1;
		col1 = col0 - 1;
		col2 = col0;
	}
	// Quadrant 2
	else if(lat > lat0 && lon > lon0){
		row1 = row0;
		row2 = row0 + 1;
		col1 = col0;
		col2 = col0 + 1;
	}
	// Quadrant 3
	else if(lat < lat0 && lon < lon0){
		row1 = row0 - 1;
		row2 = row0;
		col1 = col0 - 1;
		col2 = col0;
	}
	// Quadrant 4
	else{
		row1 = row0 - 1;
		row2 = row0;
		col1 = col0;
		col2 = col0 + 1;
	}

	lat1 = ((double) row1/60 - 89.991666666666660)*D2R;
	lat2 = ((double) row2/60 - 89.991666666666660)*D2R;
	lon1 = ((double) col1/60 - 179.9916666666667)*D2R;
	lon2 = ((double) col2/60 - 179.9916666666667)*D2R;
	
	height = ((lon2 - lon)*((double)SttData->height[row1][col1]*(lat2-lat) + (double)SttData->height[row2][col1]*(lat-lat1))
			 +(lon - lon1)*((double)SttData->height[row1][col2]*(lat2-lat) + (double)SttData->height[row2][col2]*(lat-lat1)))/(lon2-lon1)/(lat2-lat1);

	return height;
}

void LocalGeometryDEM(double *PosWR, double *PosWT, struct SoOpType *S, struct SpecularType *Sp)
{
	double vPS[3], vPW[3], vRxS[3], vTxS[3], vP2Rx[3], vP2Tx[3];
//	double velRxS[3], velTxS[3];
//	double vTemp1[3], vTemp2[3];
	double mSp, mP2RxS, mP2TxS;
	double vSp2RxW[3];
//	double mVelRxS, mVelTxS;
	long Itheta, Iphi, thetaSp, phiSp;
	double dtheta, dphi, theta, phi;//, dAngMin;//, dArea;
	double x[3], y[3], z[3];

	uint8_t updateFlag = 0;
	double heightInterp, rP, rE, sinLat2, localPathLen, minPathLen;
    double a = 6378137.0;
    //double f = 1.0/298.257223563; // WGS84
	double f = 1.0/298.257222100882711; // GRS80
    double e2 = f*(2.0-f);
	double incAng, scaAng, mPS;

	// Specular frame
	vSp2RxW[0] = PosWR[0] - Sp->PosW[0];
	vSp2RxW[1] = PosWR[1] - Sp->PosW[1];
	vSp2RxW[2] = PosWR[2] - Sp->PosW[2];

	mSp = CopyUnitV(Sp->PosW, z);
	Sp->r = mSp;

	// Direction Cosine Matrix from ECEF to Specular frame
	x[0] = vSp2RxW[0] - VoV(vSp2RxW, z)*z[0];
	x[1] = vSp2RxW[1] - VoV(vSp2RxW, z)*z[1];
	x[2] = vSp2RxW[2] - VoV(vSp2RxW, z)*z[2];
	UNITV(x);
	VxV(z,x,y);

	Sp->CSW[0][0] = x[0];
	Sp->CSW[0][1] = x[1];
	Sp->CSW[0][2] = x[2];
	Sp->CSW[1][0] = y[0];
	Sp->CSW[1][1] = y[1];
	Sp->CSW[1][2] = y[2];
	Sp->CSW[2][0] = z[0];
	Sp->CSW[2][1] = z[1];
	Sp->CSW[2][2] = z[2];

	MxV(Sp->CSW, PosWR, vRxS);
	MxV(Sp->CSW, PosWT, vTxS);
	//MxV(Sp->CSW, Rx->VelW, velRxS);
	//MxV(Sp->CSW, Tx->VelW, velTxS);

	// Minimum angle step
	//dAngMin = S->minSurfRes/mSp;
	dtheta = S->minSurfRes/mSp;
	// delta theta
	//dtheta = asin(Sp->a/mSp)/S->NthetaLocal;
	//printf("%lf %lf", dAngMin, dtheta);
	//if(dtheta < dAngMin)
	//	dtheta = dAngMin;

	// delta phi
	dphi = TwoPi/S->NphiLocal;
	//if(dphi < dAngMin)
	//	dphi = dAngMin;
	//printf(" %lf\n", dphi);
	/* Update Specular point integrating DEM */
	heightInterp = bilinInterpDEM(Sp->lat, Sp->lon);

	//h_ellip = dem[row][col] + 6371000 - mSp;
	rP = heightInterp + 6371000;
	Sp->alt = rP - mSp; // rE == mSp

	// Specular point on GRS80 Ellipsoid and add elevation
	minPathLen = sqrt(vRxS[0]*vRxS[0] + vRxS[1]*vRxS[1] + (vRxS[2]-rP)*(vRxS[2]-rP)) +
		         sqrt(vTxS[0]*vTxS[0] + vTxS[1]*vTxS[1] + (vTxS[2]-rP)*(vTxS[2]-rP));

	for(Itheta=0; Itheta<S->NthetaLocal; Itheta++){
		for(Iphi=0; Iphi<S->NphiLocal; Iphi++){
			theta = (Itheta+1)*dtheta;
			phi = Iphi*dphi;
			Sp->LocalGrid[Itheta][Iphi].theta = theta;
			Sp->LocalGrid[Itheta][Iphi].phi = phi;

			// Differential area over surface
			//Sp->LocalGrid[Itheta][Iphi].dArea = mSp*mSp*sin(theta)*dtheta*dphi;

			/* Location of surface point in the specular frame */
			vPS[0] = mSp * sin(theta) * cos(phi);
			vPS[1] = mSp * sin(theta) * sin(phi);
			vPS[2] = mSp * cos(theta);

			/* Specular frame to ECEF */
			MTxV(Sp->CSW, vPS, vPW);

			/* ECEF to GRS80 */
			ECEFToGRS80(vPW, &Sp->LocalGrid[Itheta][Iphi].lat, &Sp->LocalGrid[Itheta][Iphi].lon, &Sp->LocalGrid[Itheta][Iphi].alt);

			/* Integrate DEM */
			//row = Sp->LocalGrid[Itheta][Iphi].lat*R2D*60+5399.9999;
			//col = Sp->LocalGrid[Itheta][Iphi].lon*R2D*60+10799.9999;
			heightInterp = bilinInterpDEM(Sp->LocalGrid[Itheta][Iphi].lat, Sp->LocalGrid[Itheta][Iphi].lon);
			sinLat2 = sin(Sp->LocalGrid[Itheta][Iphi].lat)*sin(Sp->LocalGrid[Itheta][Iphi].lat);
			rE = a*sqrt((1-e2*(2-e2)*sinLat2) / (1-e2*sinLat2));
			
			//Sp->LocalGrid[Itheta][Iphi].alt = SttData->height[row][col] + 6371000 - rE;
			Sp->LocalGrid[Itheta][Iphi].alt = heightInterp + 6371000 - rE;

			/* GRS80 to ECEF */
	    	GRS80ToECEF(Sp->LocalGrid[Itheta][Iphi].lat, Sp->LocalGrid[Itheta][Iphi].lon, Sp->LocalGrid[Itheta][Iphi].alt, vPW);

			/* ECEF to Specular frame */
	    	MxV(Sp->CSW, vPW, vPS);

			vP2Rx[0] = vRxS[0] - vPS[0];
			vP2Rx[1] = vRxS[1] - vPS[1];
			vP2Rx[2] = vRxS[2] - vPS[2];

			vP2Tx[0] = vTxS[0] - vPS[0];
			vP2Tx[1] = vTxS[1] - vPS[1];
			vP2Tx[2] = vTxS[2] - vPS[2];

			mP2RxS = UNITV(vP2Rx);
			mP2TxS = UNITV(vP2Tx);
			
			mPS = UNITV(vPS);
			incAng = acos(VoV(vP2Tx, vPS) / (mP2TxS*mPS));
			scaAng = acos(VoV(vP2Rx, vPS) / (mP2RxS*mPS));

			/* Find minimum reflection path length */
			localPathLen = mP2RxS + mP2TxS;
			
			if(localPathLen < minPathLen){
				minPathLen = localPathLen;
				thetaSp = Itheta;
				phiSp = Iphi;
				updateFlag = 1;
				/*
				mVelRxS = VoV(vP2Rx,velRxS);
				mVelTxS = VoV(vP2Tx,velTxS);
				Sp->LocalGrid[Itheta][Iphi].delay = minPathLen/LIGHTSPEED;
				Sp->LocalGrid[Itheta][Iphi].doppler = -(mVelRxS + mVelTxS)/S->Wavelength;
				*/
			}
/*
			mVelRxS = VoV(vP2Rx,velRxS);
			mVelTxS = VoV(vP2Tx,velTxS);

			Sp->CLS[0][0] = sin(theta)*cos(phi);
			Sp->CLS[0][1] = sin(theta)*sin(phi);
			Sp->CLS[0][2] = cos(theta);
			Sp->CLS[1][0] = cos(theta)*cos(phi);
			Sp->CLS[1][1] = cos(theta)*sin(phi);
			Sp->CLS[1][2] = -sin(theta);
			Sp->CLS[2][0] = -sin(phi);
			Sp->CLS[2][1] = cos(phi);
			Sp->CLS[2][2] = 0;

			Sp->LocalGrid[Itheta][Iphi].range = localPathLen;
			Sp->LocalGrid[Itheta][Iphi].delay = Sp->LocalGrid[Itheta][Iphi].range/LIGHTSPEED - Sp->LocalGrid[0][0].delay;
			Sp->LocalGrid[Itheta][Iphi].doppler = -(mVelRxS + mVelTxS)/S->Wavelength - Sp->LocalGrid[0][0].doppler;
			Sp->LocalGrid[Itheta][Iphi].incAngL2Rx = acos(VoV(Sp->CLS[0], vP2Rx));
			Sp->LocalGrid[Itheta][Iphi].incAngL2Tx = acos(VoV(Sp->CLS[0], vP2Tx));

			// Scattering Vector q
			vTemp1[0] = vP2Tx[0] + vP2Rx[0];
			vTemp1[1] = vP2Tx[1] + vP2Rx[1];
			vTemp1[2] = vP2Tx[2] + vP2Rx[2];

			MxV(Sp->CLS, vTemp1, vTemp2);
			Sp->LocalGrid[Itheta][Iphi].scaVec[0] = TwoPi/S->Wavelength*vTemp2[0];
			Sp->LocalGrid[Itheta][Iphi].scaVec[1] = TwoPi/S->Wavelength*vTemp2[1];
			Sp->LocalGrid[Itheta][Iphi].scaVec[2] = TwoPi/S->Wavelength*vTemp2[2];

			// Scattering angle
			vP2Tx[0] = -vP2Tx[0];
			vP2Tx[1] = -vP2Tx[1];
			vP2Tx[2] = -vP2Tx[2];
			Sp->LocalGrid[Itheta][Iphi].scaAng = 0.5*acos(VoV(vP2Tx,vP2Rx));
*/
		}
	}
	/* Update new specular point integrating DEM */
	//Bistatic->r_tsr = minPathLen;
	if(updateFlag){
		Sp->lat = Sp->LocalGrid[thetaSp][phiSp].lat;
		Sp->lon = Sp->LocalGrid[thetaSp][phiSp].lon;
		Sp->alt = Sp->LocalGrid[thetaSp][phiSp].alt;
		Sp->flagUpdatedSp = updateFlag;
		//Sp->dopplerR = Sp->LocalGrid[thetaSp][phiSp].doppler;
		//Sp->delayR = Sp->LocalGrid[thetaSp][phiSp].delay;
		//printf("(%ld, %ld) = %lf, %lf, %lf, snell: %lf\n",
		//		thetaSp, phiSp,
		//		(Sp->latGRS-Sp->latSUR)*R2D,
		//		(Sp->lonGRS-Sp->lonSUR)*R2D,
		//		(Sp->altGRS-Sp->altSUR)*R2D,
		//		(Sp->LocalGrid[thetaSp][phiSp].incAngL2Rx - Sp->LocalGrid[thetaSp][phiSp].incAngL2Tx)*R2D);
	}
}

void calcSpotbeamGeometry(struct SpotBeamType *MuosSb)
{
	double r_beams, r0, rS1, rS2, rS3; // Beam spacing
	int i;
	double a = 6378137.0;
    double f = 1.0/298.257223563;
    double b = a*(1.0-f);
	r_beams = 1+4*cos(36*D2R);
//	r_beams = 3+2*sin(18*D2R);
//	r_beams = 4*cos(36*D2R);
//	r_beams = sqrt(1+8*cos(36*D2R)*cos(36*D2R)+8*sin(18*D2R)*sin(18*D2R)+8*sin(18*D2R));
	r0 = b/r_beams;
	rS1 = 2*r0*cos(36*D2R);
	rS2 = (2+2*sin(18*D2R))*r0;
	rS3 = 2*rS1;

	MuosSb->el[0] = 0;
	MuosSb->el[1] = atan2(rS1, MuosSb->altTx);
	MuosSb->el[2] = atan2(rS2, MuosSb->altTx);
	MuosSb->el[3] = atan2(rS3, MuosSb->altTx);

	for(i=0; i<5; i++){
		MuosSb->az[0][i] = (-18+72*i)*D2R;
		MuosSb->az[1][i] = (18+72*i)*D2R;
		MuosSb->az[2][i] = (-18+72*i)*D2R;
	}
	//MuosSb->r0Target = MuosSb->altTx * tan(MuosSb->halfBW);
	//MuosSb->halfBW_whole = asin(b/(MuosSb->altTx + b));
}

void TrackingSpotbeam(struct ScType *Tx, struct SpotBeamType *MuosSb)
{
	long iSb, iEl, iAz, iTh, NAz;

	double a = 6378137.0;
	//double f = 1.0/298.257223563;
	double f = 1.0/298.257222100882711; // GRS80
	double b = a*(1.0-f);

	double r, PosFpS[3], PosFpL[3], PosFpN[3], theta, A, B, C, az, el, CSL[3][3], CNL[3][3], z0;
	double vecConeS[3], vecConeL[3], vecConeN[3];
	double lonMin, lonMax;

	// ECI to ECEF of Transmitter's Position and Velocity
	//MxV(World[EARTH].CWN,Tx->PosN,Tx->PosW);
	// ECEF to GRS80
	//ECEFToGRS80(Tx->PosW, &Tx->lat, &Tx->lon, &Tx->alt);

	MuosSb->altTx = Tx->alt;
	calcSpotbeamGeometry(MuosSb);

	iSb = 0;
	z0 = MAGV(Tx->PosN);
	MT(Tx->CLN, CNL);

	for(iEl=0; iEl<4; iEl++){
		if(iEl!=0){
			el = MuosSb->el[iEl];
			NAz = 5;
		}
		else{
			el = 0;
			NAz = 1;
		}

		for(iAz=0; iAz<NAz; iAz++){
			if(iEl!=0)
				az = MuosSb->az[iEl-1][iAz];
			else{
				az = 0;
			}
			// From LVLH to Spotbeam frame
			CSL[0][0] = cos(az)*cos(el); CSL[0][1] = sin(az)*cos(el); CSL[0][2] = -sin(el);
			CSL[1][0] = -sin(az);        CSL[1][1] = cos(az);         CSL[1][2] = 0;
			CSL[2][0] = cos(az)*sin(el); CSL[2][1] = sin(az)*sin(el); CSL[2][2] = cos(el);

			lonMin = Pi;
			lonMax = -Pi;
			for(iTh=0; iTh<64; iTh++){
				theta = iTh*0.1;

				vecConeS[0] = cos(theta);
				vecConeS[1] = sin(theta);
				vecConeS[2] = 1/tan(MuosSb->halfBW);

				MTxV(CSL, vecConeS, vecConeL);
				MTxV(Tx->CLN, vecConeL, vecConeN);

				A = (vecConeN[0]*vecConeN[0]+vecConeN[1]*vecConeN[1])/a/a+vecConeN[2]*vecConeN[2]/b/b;
				B = ((CNL[0][2]*vecConeN[0] + CNL[1][2]*vecConeN[1])/a/a + CNL[2][2]*vecConeN[2]/b/b)*z0;
				C = ((CNL[0][2]*CNL[0][2]+CNL[1][2]*CNL[1][2])/a/a + CNL[2][2]*CNL[2][2]/b/b)*z0*z0 - 1;

				r = (B - sqrt(B*B-A*C)) / A;

				if(r>0){
					MuosSb->Exist[iSb][iTh] = TRUE;
					PosFpS[0] = r * vecConeS[0];
					PosFpS[1] = r * vecConeS[1];
					PosFpS[2] = r * vecConeS[2];

					MTxV(CSL, PosFpS, PosFpL);

					PosFpL[2] -= z0;

					MxV(CNL, PosFpL, PosFpN);

					// ECI to ECEF
					MxV(World[EARTH].CWN, PosFpN, MuosSb->PosW[iSb][iTh]);
					//printf("r = %lf, PosFpN[0] = %lf, PosFpN[1] = %lf, PosFpN[2] = %lf\n", r, PosFpN[0], PosFpN[1], PosFpN[2]);
					
					// ECEF to GRS80
					ECEFToGRS80(MuosSb->PosW[iSb][iTh], &MuosSb->lat[iSb][iTh], &MuosSb->lon[iSb][iTh], &MuosSb->alt[iSb][iTh]);

					//MuosSb->r[iSb] = r;
					
					lonMin = (lonMin > MuosSb->lon[iSb][iTh])? MuosSb->lon[iSb][iTh] : lonMin;
					lonMax = (lonMax < MuosSb->lon[iSb][iTh])? MuosSb->lon[iSb][iTh] : lonMax;
				}
				else{
					MuosSb->Exist[iSb][iTh] = FALSE;
					MuosSb->lat[iSb][iTh] = FILLVALUE;
					MuosSb->lon[iSb][iTh] = FILLVALUE;
					MuosSb->alt[iSb][iTh] = FILLVALUE;
					//MUO[iSb][iTh] = NAN;
					//r = NAN;
				}
			}
			if(lonMax - lonMin > Pi){
				MuosSb->flagShifted[iSb] = TRUE;
				for(iTh=0; iTh<64; iTh++){
					if(MuosSb->lon[iSb][iTh]>0){
						MuosSb->lon[iSb][iTh] -= Pi;
					}
					else{
						MuosSb->lon[iSb][iTh] += Pi;
					}
				}
			}
			else{
				MuosSb->flagShifted[iSb] = FALSE;
			}
			iSb++;
		}
	}
	// Calculate entire beamwidth
	/*
	for(iTh=0; iTh<64; iTh++){
		theta = iTh*0.1;

		vecConeL[0] = cos(theta);
		vecConeL[1] = sin(theta);
		vecConeL[2] = 1/tan(MuosSb->halfBW_whole);
		printf("%lf\n", MuosSb->halfBW_whole*R2D);

		MTxV(Orb[iTx+1].CLN, vecConeL, vecConeN);

		A = (vecConeN[0]*vecConeN[0]+vecConeN[1]*vecConeN[1])/a/a+vecConeN[2]*vecConeN[2]/b/b;
		B = ((CNL[0][2]*vecConeN[0] + CNL[1][2]*vecConeN[1])/a/a + CNL[2][2]*vecConeN[2]/b/b)*z0;
		C = ((CNL[0][2]*CNL[0][2]+CNL[1][2]*CNL[1][2])/a/a + CNL[2][2]*CNL[2][2]/b/b)*z0*z0 - 1;

		r = (B - sqrt(B*B-A*C)) / A;

		PosFpL[0] = r * vecConeL[0];
		PosFpL[1] = r * vecConeL[1];
		PosFpL[2] = r * vecConeL[2] - z0;

		MxV(CNL, PosFpL, PosFpN);

		// ECI to ECEF of Transmitter's Position
		MxV(World[EARTH].CWN, PosFpN, PosFpW);

		// ECEF to GRS80
		ECEFToGRS80(PosFpW, &MuosSb->latWGS[iTx][iSb], &MuosSb->lonWGS[iTx][iSb], &MuosSb->altWGS[iTx][iSb]);

		MuosSb->r[iSb] = r;

		fprintf(outFile[iTx],"%lf %ld %lf %lf %lf %lf %lf %lf %lf\n",
			AbsTime, iSb, latT, lonT, altT,
			MuosSb->latWGS[iTx][iSb], MuosSb->lonWGS[iTx][iSb], MuosSb->altWGS[iTx][iSb], r);
	}
	*/
}

long *FindSpotbeam(struct SpotBeamType *MuosSb, double lat, double lon, long *SbNum, long *SbCnt)
{
	char flagNW, flagNE, flagSW, flagSE;
	long iSb, iTh, cntSb = 0;
	static long  bufferSb[16];
	double latSb, lonSb;
	double lonCopy = lon;

	for(iSb=0; iSb<16; iSb++){
		flagNW = FALSE, flagNE = FALSE, flagSW = FALSE, flagSE = FALSE;
		if(MuosSb->flagShifted[iSb]){
			if(lon>0){
				lon = lonCopy - Pi;
			}
			else{
				lon = lonCopy + Pi;
			}
		}
		else{
			lon = lonCopy;
		}
		for(iTh=0; iTh<64; iTh++){
			if(MuosSb->Exist[iSb][iTh]){
				// ECEF to GRS80
				//ECEFToGRS80(MuosSb->PosW[iSb][iTh], &latSb, &lonSb, &altSb);
				latSb = MuosSb->lat[iSb][iTh];
				lonSb = MuosSb->lon[iSb][iTh];

				if(lat<latSb && lon<lonSb)
					flagNE = TRUE;
				else if(lat>latSb && lon<lonSb)
					flagSE = TRUE;
				else if(lat>latSb && lon>lonSb)
					flagSW = TRUE;
				else if(lat<latSb && lon>lonSb)
					flagNW = TRUE;
					
				if(flagNW && flagNE && flagSW && flagSE){
					bufferSb[cntSb++] = iSb;
					break;
				}
			}
		}
	}

	*SbCnt = cntSb;

    SbNum = (long *) calloc(cntSb,sizeof(long));

	for(iSb=0; iSb<cntSb; iSb++) {
		SbNum[iSb] = bufferSb[iSb];
	}
	
	return SbNum;
}

uint8_t checkMUOSspotbeam(struct ScType *Rx, struct SpecularType *Sp, struct SpotBeamType *MuosSb)
{
    uint8_t flagSameSb = FALSE;
	long *SbNumRx = NULL, *SbNumSp = NULL, iSbRx, iSbSp;
	static long cntSbRx[1], cntSbSp[1];

    SbNumRx = FindSpotbeam(MuosSb, Rx->lat, Rx->lon, SbNumRx, cntSbRx);
    SbNumSp = FindSpotbeam(MuosSb, Sp->lat, Sp->lon, SbNumSp, cntSbSp);
	
	for(iSbRx=0; iSbRx<*cntSbRx; iSbRx++){
		for(iSbSp=0; iSbSp<*cntSbSp; iSbSp++){
			if(SbNumRx[iSbRx] == SbNumSp[iSbSp]){
				flagSameSb = TRUE;
				Sp->SpotbeamID[0] = SbNumRx[iSbRx];
				Sp->SpotbeamID[1] = SbNumSp[iSbSp];
				break;
			}
		}
		if(flagSameSb)
			break;
	}

	if(!flagSameSb){
		Sp->SpotbeamID[0] = (cntSbRx[0]>0) ? SbNumRx[0] : FILLVALUE;
		Sp->SpotbeamID[1] = (cntSbSp[0]>0) ? SbNumSp[0] : FILLVALUE;
	}
	
	free(SbNumRx);
	free(SbNumSp);
	
    return flagSameSb;
}

void reportSpotbeam(void)
{
	FILE *outFile;
	char filename[20];
	long iTx, iSb, iTh;
	struct ScType *Tx;
	struct SpotBeamType *MuosSb;
	double lon;

	sprintf(filename,"MUOS_Spotbeam.out");
	outFile = FileOpen(OutPath,filename,"w");

	for(iTx=0; iTx<5; iTx++){
		Tx = &Sc[SoOp[0].RefOrbFirst+iTx];
		MuosSb = &SpotBeam[iTx];

		// ECI to ECEF of Transmitter's Position and Velocity
		MxV(World[EARTH].CWN, Tx->PosN, Tx->PosW);
		
		// ECEF to GRS80
		ECEFToGRS80(Tx->PosW, &Tx->lat, &Tx->lon, &Tx->alt);
				
		TrackingSpotbeam(Tx, MuosSb);
		for(iSb=0; iSb<16; iSb++){
			for(iTh=0; iTh<64; iTh++){
				if(MuosSb->flagShifted[iSb]){
					if(MuosSb->lon[iSb][iTh]>0){
						lon = MuosSb->lon[iSb][iTh] + Pi;
					}
					else{
						lon = MuosSb->lon[iSb][iTh] - Pi;
					}
				}
				else{
					lon = MuosSb->lon[iSb][iTh];
				}
				fprintf(outFile,"%ld %ld %d %lf %lf %lf %lf %lf %lf\n",
					iTx, iSb, MuosSb->flagShifted[iSb], Tx->lat, Tx->lon, Tx->alt,
					MuosSb->lat[iSb][iTh], lon, MuosSb->alt[iSb][iTh]);
			}
		}		
	}
	fclose(outFile);
}

/* Calculates the Fresnel zones for the specular reflection */
void calcFresnelZones(struct FixedObsType *F)
{
	double dist, hd, hs, rd2, rs, del0, sinTh, cosTh, secTh, del, a, b, c, nump, denp, p;
	long nn;

	// Transmitter/Receiver ground range
	dist = F->hTx * tan(F->thTx);
	// Distance of specular point away from the receiver ground projection
	F->S0x = dist / (1 + F->hTx/F->hRx);

	// Parameters for Fresnel zone calculation
	hd = F->hTx - F->hRx;
	hs = F->hTx + F->hRx;
	rd2 = sqrt(hd*hd + dist*dist);
	rs = sqrt(hs*hs + dist*dist);
	del0 = rs - rd2; // the shortest distance after the direct path

	sinTh = hd / rd2;
	cosTh = dist / rd2;
	secTh = 1/ cosTh;

	// Iterate for the decided number of Fresnel zones
	// This iteration may be meaningful, when incoherent contribution is considered. Nfz is always 1 for the current version.
	for(nn = 0; nn < F->Nfz; nn++){
		// the path for the n-th Fresnel zone
		del = del0 + nn * Bistatic->Wavelength / 2;

		a = (dist * secTh + del) / 2;
		b = sqrt(del*del + 2*dist*del*secTh) / 2;
		c = (F->hTx + F->hRx) / 2;

		nump = c * (a*a - b*b) * sinTh * cosTh;
		denp = (b*b*cosTh*cosTh + a*a*sinTh*sinTh);
		p = -nump / denp;
		// distance to the center of the ellipse
		F->x1 = dist/2 + p;
		// semi-minor axis
		F->by1 = b * sqrt(1 - c*c/denp);
		// semi-major axis
		F->ax1 = F->by1 * a / sqrt(denp);
	}
}

/* Calculates the transmitter geometry-related transformation matrices */
void calcTxGeometry(struct FixedObsType *F)
{
	double ux[3] = {1, 0, 0}, uy[3] = {0, 1, 0}, uz[3] = {0, 0, 1}; // Ground Reference Coordinate System (ENU)
	double phInTx, AntRotZ_Tx[3][3], AntRotY_Tx[3][3], AntRot_Tx[3][3];

	/* .. Incoming Signal .. */
	// Azimuth angle (in standard spherical coords) of incoming signal
	phInTx = HalfPi - F->phTx + Pi;
	
	/* .. Tx Antenna Rotation Matrices .. */
	RotZ(phInTx, AntRotZ_Tx);
	RotY(Pi - F->thTx, AntRotY_Tx);
	MxM(AntRotZ_Tx, AntRotY_Tx, AntRot_Tx);

	/* .. Tx Antenna Coordinate Systems .. */
	double uxt[3], uyt[3], uzt[3];
	MxV(AntRot_Tx, ux, uxt);
	MxV(AntRot_Tx, uy, uyt);
	MxV(AntRot_Tx, uz, uzt);

	/* .. Transformation .. */
	// Transformation matrix for transforming a vector from the ground frame to transmitter system
	Bistatic->Tgt[0][0] = uxt[0]; Bistatic->Tgt[0][1] = uxt[1]; Bistatic->Tgt[0][2] = uxt[2];
	Bistatic->Tgt[1][0] = uyt[0]; Bistatic->Tgt[1][1] = uyt[1]; Bistatic->Tgt[1][2] = uyt[2];
	Bistatic->Tgt[2][0] = uzt[0]; Bistatic->Tgt[2][1] = uzt[1]; Bistatic->Tgt[2][2] = uzt[2];
}

/* Calculates the bistatic geometry-related transformation matrices, direction vectors, and rotation matrices for fixed observation */
void calcGeometryFixed(struct FixedObsType *F)
{
	double ux[3] = {1, 0, 0}, uy[3] = {0, 1, 0}, uz[3] = {0, 0, 1}; // Ground Reference Coordinate System (ENU)
	//double posGnd[3] = {0, 0, 0}; // Center of reference coordinate system
	double AntRotZ_Rx[3][3], AntRotY_Rx[3][3], AntRot_Rx[3][3], AntRotZ_Tx[3][3];
	double rd, posTx[3], posSp[3], posRx[3] = {0, 0, 0};//, ht;
	double tempV3[3] = {0, 0, 0};
	double phRx, phTx;
	double osp_rf[3], idn_rf[3], isn_sf[3];
	double th0, ph0;

	phRx = HalfPi - F->phRx;
	phTx = HalfPi - F->phTx;

	// Antenna Rotation Matrix for Receiver
	RotZ(phRx, AntRotZ_Rx);
	RotY(Pi - F->thRx, AntRotY_Rx);
	MxM(AntRotZ_Rx, AntRotY_Rx, AntRot_Rx);
	// Antenna Rotation Matrix for Transmitter
	RotZ(phTx, AntRotZ_Tx);

	/* .. Transmitter Position and Related Locations .. */
	// Slant range (Approximated)
	rd = sqrt(F->rTx*F->rTx - World[EARTH].rad*World[EARTH].rad*cos(F->elTx)*cos(F->elTx)) - World[EARTH].rad*sin(F->elTx);
	// Transmitter Antenna position
	posTx[0] = rd * sin(F->thTx) * cos(phTx);
	posTx[1] = rd * sin(F->thTx) * sin(phTx);
	posTx[2] = rd * cos(F->thTx);
	// Transmitter altitude
	F->hTx = posTx[2];
	// Specular reflection point and 1st Fresnel zone ellipse
	F->Nfz = 1; // number of fresnel zone is 1 since only the specular contribution is considered
	calcFresnelZones(F);
	// Specular reflection point position
	tempV3[0] = F->S0x;
	MxV(AntRotZ_Tx, tempV3, posSp);
	
	/* .. Receiver Position and Pointing Locations .. */
	// Receiver Antenna position
	posRx[2] = F->hRx;

	/* .. Propagation Vectors .. */
	double RT[3], ST[3], RS[3];
	long i;
	for(i=0; i<3; i++){
		// Transmitter to Receiver
		RT[i] = posRx[i] - posTx[i];
		// Tansmitter to Specular point
		ST[i] = posSp[i] - posTx[i];
		// Specular point to Receiver
		RS[i] = posRx[i] - posSp[i];
	}
	Bistatic->r_tr = CopyUnitV(RT, Bistatic->idn);
	Bistatic->r_ts = CopyUnitV(ST, Bistatic->isn);
	Bistatic->r_sr = CopyUnitV(RS, Bistatic->osp);
	Bistatic->r_tsr = Bistatic->r_ts + Bistatic->r_sr;

	/* .. Coordinate Systems .. */
	double uxr[3], uyr[3], uzr[3], uxs[3], uys[3], uzs[3];
	// Receiver Antenna Coordinate System
	MxV(AntRot_Rx, ux, uxr);
	MxV(AntRot_Rx, uy, uyr);
	MxV(AntRot_Rx, uz, uzr);
	// Specular Point Coordinate System
	MxV(AntRotZ_Tx, ux, uxs);
	MxV(AntRotZ_Tx, uy, uys);
	MxV(AntRotZ_Tx, uz, uzs);

	/* .. Transformations .. */
	// Transformation matrix for transforming a vector from the ground frame to local (specular) ground system
	Bistatic->Tgs[0][0] = uxs[0]; Bistatic->Tgs[0][1] = uxs[1]; Bistatic->Tgs[0][2] = uxs[2];
	Bistatic->Tgs[1][0] = uys[0]; Bistatic->Tgs[1][1] = uys[1]; Bistatic->Tgs[1][2] = uys[2];
	Bistatic->Tgs[2][0] = uzs[0]; Bistatic->Tgs[2][1] = uzs[1]; Bistatic->Tgs[2][2] = uzs[2];
	
	// Transformation matrix for transforming a vector from the ground frame to receiver system
	Bistatic->Tgr[0][0] = uxr[0]; Bistatic->Tgr[0][1] = uxr[1]; Bistatic->Tgr[0][2] = uxr[2];
	Bistatic->Tgr[1][0] = uyr[0]; Bistatic->Tgr[1][1] = uyr[1]; Bistatic->Tgr[1][2] = uyr[2];
	Bistatic->Tgr[2][0] = uzr[0]; Bistatic->Tgr[2][1] = uzr[1]; Bistatic->Tgr[2][2] = uzr[2];

	/* .. Angles .. */
	// The incidence angle on the receiver in receiver coordinates
	// propagation vector from Transmitter to reciever in receiver antenna system
	MxV(Bistatic->Tgr, Bistatic->idn, idn_rf);

	// Transmitter to Receiver
	// off-axis angle of zr towards Transmitter
	th0 = acos(-idn_rf[2]);
	// orientation - azimuth
	ph0 = atan2TwoPi(-idn_rf[1], -idn_rf[0]);

	F->AngT2R_rf[0] = th0;
	F->AngT2R_rf[1] = ph0;
	//printf("T->R (ph,th) = %lf %lf\n", ph0*R2D, th0*R2D);
	// Propagation vector from specular point to receiver in receiver antenna system
	MxV(Bistatic->Tgr, Bistatic->osp, osp_rf);
	// Specular Point to Receiver
	// off-axis angle of zr towards specular point
	th0 = acos(-osp_rf[2]);
	// orientation - azimuth
	ph0 = atan2TwoPi(-osp_rf[1], -osp_rf[0]);

	F->AngS2R_rf[0] = th0;
	F->AngS2R_rf[1] = ph0;
	//printf("S->R (ph,th) = %lf %lf\n", ph0*R2D, th0*R2D);

	// The incidence angle in specular frame
	// propagation vector from Transmitter to ground in local (specular) ground system
	MxV(Bistatic->Tgs, Bistatic->isn, isn_sf);
	// Transmitter to Specular Point
	// off-axis angle of zs towards Transmitter
	th0 = acos(-isn_sf[2]);
	// orientation - azimuth
	ph0 = atan2TwoPi(-isn_sf[1], -isn_sf[0]);

	F->AngT2S_sf[0] = th0;
	F->AngT2S_sf[1] = ph0;

	Bistatic->AngT2S_sf_th0 = F->AngT2S_sf[0];
}

/* Calculates the bistatic geometry-related transformation matrices, direction vectors, and rotation matrices for in-orbit observation */
void calcGeometryInorbit(double *PosWR, double *PosWT, struct SpecularType *Sp)
{
	double AntRotZ[3][3], AntRotX[3][3], CEW[3][3];

	// Rotation Matrix from ECEF to ENU
	RotX(Sp->lat-HalfPi, AntRotX);
	RotZ(-Sp->lon-HalfPi, AntRotZ);
	MxM(AntRotX, AntRotZ, CEW);
	
	/* .. Propagation Vectors .. */	
	double RT_W[3], ST_W[3], RS_W[3], RT[3], ST[3], RS[3];
	long i;
	for(i=0; i<3; i++){
		// Transmitter to Receiver
		RT_W[i] = PosWR[i] - PosWT[i];
		// Tansmitter to Specular point
		ST_W[i] = Sp->PosW[i] - PosWT[i];
		// Specular point to Receiver
		RS_W[i] = PosWR[i] - Sp->PosW[i];
	}
	MxV(CEW, RT_W, RT);
	MxV(CEW, ST_W, ST);
	MxV(CEW, RS_W, RS);
	Bistatic->r_tr = CopyUnitV(RT, Bistatic->idn);
	Bistatic->r_ts = CopyUnitV(ST, Bistatic->isn);
	Bistatic->r_sr = CopyUnitV(RS, Bistatic->osp);
	Bistatic->r_tsr = Bistatic->r_ts + Bistatic->r_sr;

	//* .. Transformations .. *//
	// Transformation matrix for transforming a vector from the ground frame to transmitter system
	CopyMT3(CEW, Bistatic->Tgt);
	// Transformation matrix for transforming a vector from the ground frame to local (specular) ground system
	CopyMT3(CEW, Bistatic->Tgs);	
	// Transformation matrix for transforming a vector from the ground frame to receiver system
	CopyMT3(CEW, Bistatic->Tgr);

	//* .. Azimuth/Elevation and Incidence Angle .. *//
	Sp->az = atan2TwoPi(Bistatic->osp[0], Bistatic->osp[1]);
	Sp->elev = asin(Bistatic->osp[2]);
	Sp->incAng = HalfPi - Sp->elev;
}

/* Calculates the bistatic geometry-related transformation matrices, direction vectors, and rotation matrices for in-orbit observation */
void calcGeometryInorbitLocal(double *PosWR, double *PosWT, struct LocalGridType *Lg)
{
	double AntRotZ[3][3], AntRotX[3][3], CEW[3][3];
	double th0, ph0;

	// Rotation Matrix from ECEF to ENU
	RotX(Lg->lat-HalfPi, AntRotX);
	RotZ(-Lg->lon-HalfPi, AntRotZ);
	MxM(AntRotX, AntRotZ, CEW);
	
	/* .. Propagation Vectors .. */	
	double RT_W[3], ST_W[3], RS_W[3], RT[3], ST[3], RS[3];
	long i;
	for(i=0; i<3; i++){
		// Transmitter to Receiver
		RT_W[i] = PosWR[i] - PosWT[i];
		// Tansmitter to Specular point
		ST_W[i] = Lg->PosW[i] - PosWT[i];
		// Specular point to Receiver
		RS_W[i] = PosWR[i] - Lg->PosW[i];
	}
	MxV(CEW, RT_W, RT);
	MxV(CEW, ST_W, ST);
	MxV(CEW, RS_W, RS);
	Bistatic->r_tr = CopyUnitV(RT, Bistatic->idn);
	Bistatic->r_ts = CopyUnitV(ST, Bistatic->isn);
	Bistatic->r_sr = CopyUnitV(RS, Bistatic->osp);
	Bistatic->r_tsr = Bistatic->r_ts + Bistatic->r_sr;

	//* .. Transformations .. *//
	// Transformation matrix for transforming a vector from the ground frame to transmitter system
	CopyMT3(CEW, Bistatic->Tgt);
	// Transformation matrix for transforming a vector from the ground frame to local (specular) ground system
	CopyMT3(CEW, Bistatic->Tgs);	
	// Transformation matrix for transforming a vector from the ground frame to receiver system
	CopyMT3(CEW, Bistatic->Tgr);

	//* .. Azimuth/Elevation and Incidence Angle .. *//
	Lg->az = atan2TwoPi(Bistatic->osp[0], Bistatic->osp[1]);
	Lg->elev = asin(Bistatic->osp[2]);
	Lg->incAng = HalfPi - Lg->elev;

	// off-axis angle of zs towards Transmitter
	th0 = asin(-Bistatic->isn[2]);
	// orientation - azimuth
	ph0 = atan2TwoPi(-Bistatic->isn[0], -Bistatic->isn[1]);

	Lg->AngT2S_sf[0] =  HalfPi - th0;
	Lg->AngT2S_sf[1] = ph0;
}