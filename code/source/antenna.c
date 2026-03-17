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

/* Calculates unit vectors on the tangential plane */
void tanUnitVec(double CB1[3][3], double CB2[3][3], double u[3], char pol1, char pol2, double complex u1p1[3], double complex u1p2[3], double complex u2p1[3], double complex u2p2[3])
{
	double u1[3], u2[3];
	double th1, th2, ph1, ph2;
	double AR, AT, AP;
	double temp[3];
	double complex u1ph[3], u2ph[3]; // H-pol
	double complex u1th[3], u2th[3]; // V-pol
	double complex u1X[3], u2X[3]; // X-pol
	double complex u1Y[3], u2Y[3]; // Y-pol
	double complex u1R[3], u2R[3]; // R-pol
	double complex u1L[3], u2L[3]; // L-pol
	long i;

	/* ... Angles to frames #1 and #2 ... */

	// Unit propagation vector in frame #1
	MxV(CB1,u,u1);
	// Unit propagation vector in frame #2
	MxV(CB2,u,u2);

	for(i=0; i<3; i++){
		u1[i] = roundzero(u1[i]);
		u2[i] = roundzero(u2[i]);
	}

	// Off-axis angle of z1 towards frame 2
	th1 = acos(u1[2]);
	// Orientation - azimuth
	ph1 = newatan2(u1[1], u1[0]);
	// Off-axis angle of z2 towards frame 1
	th2 = acos(-u2[2]);
	// Orientation - azimuth
	ph2 = newatan2(-u2[1], -u2[0]);

	/* ... Unit vectors in phi direction [H-pol] ... */
	AR = 0; AT = 0; AP = 1;
	// phi vector of frame 1 in reference frame
	sph2car(th1, ph1, AR, AT, AP, temp);
	MTxV_complex(CB1, temp, u1ph);
	// phi vector of frame 2 in reference frame
	sph2car(th2, ph2, AR, AT, AP, temp);
	MTxV_complex(CB2, temp, u2ph);

	/* ... Unit vectors in theta direction [V-pol] ... */
	
	AR = 0; AT = 1; AP = 0;
	// theta vector of frame 1 in reference frame
	sph2car(th1, ph1, AR, AT, AP, temp);
	MTxV_complex(CB1, temp, u1th);
	// theta vector of frame 2 in reference frame
	sph2car(th2, ph2, AR, AT, AP, temp);
	MTxV_complex(CB2, temp, u2th);

	/* ... Unit vectors in Ludwig basis (reference: Y-pol) ... */

	// Y-axis vector of frame 1 in reference frame
	AR = 0; AT = sin(ph1); AP = cos(ph1);
	sph2car(th1, ph1, AR, AT, AP, temp);
	MTxV_complex(CB1, temp, u1Y);
	// Y-axis vector of frame 2 in reference frame
	AR = 0; AT = sin(ph2); AP = cos(ph2);
	sph2car(th2, ph2, AR, AT, AP, temp);
	MTxV_complex(CB2, temp, u2Y);

	/* ... Unit vectors in Ludwig basis (cross: X-pol) ... */

	// X-axis vector of frame 1 in reference frame
	AR = 0; AT = cos(ph1); AP = -sin(ph1);
	sph2car(th1, ph1, AR, AT, AP, temp);
	MTxV_complex(CB1, temp, u1X);
	// X-axis vector of frame 2 in reference frame
	AR = 0; AT = cos(ph2); AP = -sin(ph2);
	sph2car(th2, ph2, AR, AT, AP, temp);
	MTxV_complex(CB2, temp, u2X);

	/* ... Unit vectors in circular polarization (R-pol and L-pol) ... */

	for(i=0; i<3; i++){
		// frame 1
		u1R[i] = 1.0/sqrt(2.0) * (u1X[i]-I1*u1Y[i]); // port 1 RHCP
		u1L[i] = 1.0/sqrt(2.0) * (u1X[i]+I1*u1Y[i]); // port 2 LHCP
		// frame 2
		u2R[i] = 1.0/sqrt(2.0) * (u2X[i]-I1*u2Y[i]); // port 1 RHCP
		u2L[i] = 1.0/sqrt(2.0) * (u2X[i]+I1*u2Y[i]); // port 2 LHCP
	}

	/* ... Determine w.r.t the polarization ... */

	switch(pol1){
		case 'H':
			for(i=0; i<3; i++){
				u1p1[i] = u1ph[i];
				u1p2[i] = u1th[i];
			}
			break;
		case 'V':
			for(i=0; i<3; i++){
				u1p1[i] = u1th[i];
				u1p2[i] = u1ph[i];
			}
			break;
		case 'X':
			for(i=0; i<3; i++){
				u1p1[i] = u1X[i];
				u1p2[i] = u1Y[i];
			}
			break;
		case 'Y':
			for(i=0; i<3; i++){
				u1p1[i] = u1Y[i];
				u1p2[i] = u1X[i];
			}
			break;
		case 'R':
			for(i=0; i<3; i++){
				u1p1[i] = u1R[i];
				u1p2[i] = u1L[i];
			}
			break;
		case 'L':
			for(i=0; i<3; i++){
				u1p1[i] = u1L[i];
				u1p2[i] = u1R[i];
			}
			break;
	}

	switch(pol2){
		case 'H':
			for(i=0; i<3; i++){
				u2p1[i] = u2ph[i];
				u2p2[i] = u2th[i];
			}
			break;
		case 'V':
			for(i=0; i<3; i++){
				u2p1[i] = u2th[i];
				u2p2[i] = u2ph[i];
			}
			break;
		case 'X':
			for(i=0; i<3; i++){
				u2p1[i] = u2X[i];
				u2p2[i] = u2Y[i];
			}
			break;
		case 'Y':
			for(i=0; i<3; i++){
				u2p1[i] = u2Y[i];
				u2p2[i] = u2X[i];
			}
			break;
		case 'R':
			for(i=0; i<3; i++){
				u2p1[i] = u2R[i];
				u2p2[i] = u2L[i];
			}
			break;
		case 'L':
			for(i=0; i<3; i++){
				u2p1[i] = u2L[i];
				u2p2[i] = u2R[i];
			}
			break;
	}
}

/* Calculates and outputs the 4 x 4 Mueller matrix of a 2 x 2 input matrix. */
void calcMueller(double complex u[2][2], double complex U[4][4])
{
	double complex u11, u12, u21, u22;

	// Get the elements of the 2 x 2 input matrix
	u11 = u[0][0]; u12 = u[0][1];
	u21 = u[1][0]; u22 = u[1][1];

	// Calculate the elements of 4 x 4 Mueller matrix
	U[0][0] = cabs(u11)*cabs(u11);
	U[0][1] = cabs(u12)*cabs(u12);
	U[0][2] = creal(u11 * conj(u12));
	U[0][3] = -cimag(u11 * conj(u12));

	U[1][0] = cabs(u21)*cabs(u21);
	U[1][1] = cabs(u22)*cabs(u22);
	U[1][2] = creal(u21*conj(u22));
	U[1][3] = -cimag(u21*conj(u22));

	U[2][0] = 2 * creal(u11 * conj(u21));
	U[2][1] = 2 * creal(u12 * conj(u22));
	U[2][2] = creal(u11 * conj(u22) + u12 * conj(u21));
	U[2][3] = -cimag(u11 * conj(u22) - u12 * conj(u21));

	U[3][0] = 2 * cimag(u11 * conj(u21));
	U[3][1] = 2 * cimag(u12 * conj(u22));
	U[3][2] = cimag(u11 * conj(u22) + u12 * conj(u21));
	U[3][3] = creal(u11 * conj(u22) - u12 * conj(u21));
}

/* Calculates Generalized-Gaussian receiver antenna parameters */
double*** LoadAntPatternGG(double SLL, double XPL, double hpbw, double antPatRes, double*** antPattern)
{
    double th = 0, dth, dph, a, V, alpha = 0.2; // sidelobe width parameter
	double p1 = -1.066666666666640e-05, p2 = 0.001160000000000, p3 = -0.044733333333333, p4 = 0.625999999999994;
	double pV1 = 4.643259430540436e-05, pV2 = -0.003643551991047, pV3 = 0.081085762783132, pV4 = -0.375433361195809;
	long iPol, iPhi, iTheta, Nth, Nph;
	double *gXX, *gXY, *gYX, *gYY;
	double XX = 1, XY = 1;
	double maggX, maxgX = 0;
	double maggY, maxgY = 0;

	a = p1 * SLL*SLL*SLL + p2 * SLL*SLL + p3 * SLL + p4;
	V = pV1 * XPL*XPL*XPL + pV2 * XPL*XPL + pV3 * XPL + pV4;

	dth = ANT_PAT_TH_RANGE_DEG / antPatRes;
	dph = ANT_PAT_PH_RANGE_DEG / antPatRes;
	Nth = floor(dth) + 1;
	Nph = floor(dph) + 1;
	
	gXX = (double *)calloc(Nth, sizeof(double));
	gXY = (double *)calloc(Nth, sizeof(double));
	gYX = (double *)calloc(Nth, sizeof(double));
	gYY = (double *)calloc(Nth, sizeof(double));
	
	antPatRes *= D2R;
	hpbw *= D2R;

	for(iTheta=0; iTheta<Nth; iTheta++){
		th = iTheta*antPatRes;
		gXX[iTheta] = fabs(1 / (1-a) * exp( -(tan(th) / tan(hpbw))*(tan(th) / tan(hpbw)))
			- a / (1-a) * exp(-(alpha * tan(th) / tan(hpbw))*(alpha * tan(th) / tan(hpbw))));
		gXY[iTheta] = V * fabs(1 / (1-a) * exp( -(tan(th) / tan(2*hpbw))*(tan(th) / tan(2*hpbw)))
			- a / (1-a) * exp(-(alpha * tan(th) / tan(2*hpbw))*(alpha * tan(th) / tan(2*hpbw))));		
		if(th > 0.4*Pi && th < 0.45*Pi){
			if(gXX[iTheta]<XX)
				XX = gXX[iTheta];
			if(gXY[iTheta]<XY)
				XY = gXY[iTheta];
		}
	}

	for(iTheta=0; iTheta<Nth; iTheta++){
		th = iTheta*antPatRes;
		if(th > 0.5*Pi){
			gXX[iTheta] = XX;
			gXY[iTheta] = XY;
		}

		if(gXX[iTheta]<XX)
			gXX[iTheta] = XX;
		if(gXY[iTheta]<XY)
			gXY[iTheta] = XY;
		
		gYX[iTheta] = gXY[iTheta];
		gYY[iTheta] = gXX[iTheta];

		// Complex (voltage) co- and x- patterns: X-port (V-pol)
		maggX = sqrt(fabs(gXX[iTheta])*fabs(gXX[iTheta]) + fabs(gXY[iTheta])*fabs(gXY[iTheta]));
		if(maggX > maxgX)
			maxgX = maggX;
		// Complex (voltage) co- and x- patterns: Y-port (H-pol)
		maggY = sqrt(fabs(gYX[iTheta])*fabs(gYX[iTheta]) + fabs(gYY[iTheta])*fabs(gYY[iTheta]));
		if(maggY > maxgY)
			maxgY = maggY;
	}
	
	//* .. Memory allocation for the antenna pattern array .. *//
	antPattern = (double ***)calloc(4, sizeof(double**));
	for(iPol=0; iPol<4; iPol++){
		antPattern[iPol] = (double **)calloc(Nph, sizeof(double*));
		for(iPhi=0; iPhi<Nph; iPhi++){
			antPattern[iPol][iPhi] = (double *)calloc(Nth, sizeof(double));
		}
	}
	for(iPhi=0; iPhi<Nph; iPhi++){
		for(iTheta=0; iTheta<Nth; iTheta++){
			// Complex normalized (voltage) pattern
			antPattern[0][iPhi][iTheta] = gXX[iTheta] / maxgX;
			antPattern[1][iPhi][iTheta] = gXY[iTheta] / maxgX;
			antPattern[2][iPhi][iTheta] = gYX[iTheta] / maxgY;
			antPattern[3][iPhi][iTheta] = gYY[iTheta] / maxgY;
		}	
	}
	return antPattern;
}

/* Load receiver antenna pattern */
double*** LoadAntPattern(char antFileName[4][40], double *antPatRes, double ***antPattern)
{
	FILE *infile;
    const char delimiter[2] = ",";
    char *token;
    int iTheta = 0, iPhi = 0, nTheta, nPhi, iPol, i;
    char line[3600];

	for(iPol=0; iPol<4; iPol++){
		infile = FileOpen(AntPath,antFileName[iPol],"r");
		if(iPol == 0){
			//* .. Find number of theta angles .. *//
			fgets(line, sizeof line, infile);
			// Get the first token
			token = strtok(line, delimiter); 
			// Walk through other tokens
			while(token!=NULL)
			{
				iTheta++;
				token = strtok(NULL, delimiter);
			}
			nTheta = iTheta;

			//* .. Find number of phi angles .. *//
			rewind(infile);
			while(fgets(line, sizeof line, infile) != NULL)
			{	
				iPhi++;
			}
			nPhi = iPhi;

			// Calculate the antenna pattern resolution in degrees
            *antPatRes = 180 / (nTheta - 1);
			rewind(infile);

			//* .. Memory allocation for the antenna pattern array .. *//
			antPattern = (double ***)calloc(4, sizeof(double**));
			for(i=0; i<4; i++){
				antPattern[i] = (double **)calloc(nPhi, sizeof(double*));
				for(iPhi=0; iPhi<nPhi; iPhi++)
					antPattern[i][iPhi] = (double *)calloc(nTheta, sizeof(double));
			}
		}
		//* .. Read values .. *//
		iTheta = 0; iPhi = 0;
		while(fgets(line, sizeof line, infile) != NULL)
		{
			token = strtok(line, delimiter);
			while(token!=NULL)
			{	
				antPattern[iPol][iPhi][iTheta] = atof(token);
				iTheta++;
				token = strtok(NULL, delimiter);
			}
			iPhi++;
			iTheta=0;
		}
		fclose(infile);
	}

	return antPattern;
}

/* Look up USER_DEFINED antenna pattern */
void LookupAntPattern(double ***antPat, double res, double th, double ph, double complex g[2][2])
{
	long iTheta, iPhi;
	
	// Convert (-180,180) to (0,360)
	ph = fmod(ph+TwoPi, TwoPi);

	iTheta = (long) round(th*R2D / res);
	iPhi = (long) round(ph*R2D / res);

	//printf("res=%lf, th=%lf, ph=%lf, th = %ld, ph = %ld\n", res, th*R2D, ph*R2D, iTheta, iPhi);
	g[0][0] = antPat[0][iPhi][iTheta];
	g[0][1] = antPat[1][iPhi][iTheta];
	g[1][0] = antPat[2][iPhi][iTheta];
	g[1][1] = antPat[3][iPhi][iTheta];	
}

void ReportAntPattern(char name[], double antPatRes, double*** antPattern)
{
	FILE *outFileXX, *outFileXY, *outFileYX, *outFileYY;
	char filename[30];
	long iPhi, iTheta;
	long Nth, Nph;
	double dth, dph;

	dth = ANT_PAT_TH_RANGE_DEG / antPatRes;
	dph = ANT_PAT_PH_RANGE_DEG / antPatRes;
	Nth = floor(dth) + 1;
	Nph = floor(dph) + 1;

	sprintf(filename, "%s_XX.csv", name);
	outFileXX = FileOpen(OutPath, filename, "w");
	sprintf(filename, "%s_XY.csv", name);
	outFileXY = FileOpen(OutPath, filename, "w");
	sprintf(filename, "%s_YX.csv", name);
	outFileYX = FileOpen(OutPath, filename, "w");
	sprintf(filename, "%s_YY.csv", name);
	outFileYY = FileOpen(OutPath, filename, "w");
	
	for(iPhi=0; iPhi<Nph; iPhi++){
		for(iTheta=0; iTheta<Nth; iTheta++){
			// Complex normalized (voltage) pattern
			fprintf(outFileXX, "%lf ", antPattern[0][iPhi][iTheta]);
			fprintf(outFileXY, "%lf ", antPattern[1][iPhi][iTheta]);
			fprintf(outFileYX, "%lf ", antPattern[2][iPhi][iTheta]);
			fprintf(outFileYY, "%lf ", antPattern[3][iPhi][iTheta]);			
		}
		fprintf(outFileXX, "\n");
		fprintf(outFileXY, "\n");
		fprintf(outFileYX, "\n");
		fprintf(outFileYY, "\n");
	}
	fclose(outFileXX);
	fclose(outFileXY);
	fclose(outFileYX);
	fclose(outFileYY);
}

/* Calculates the antenna polarization rotation matrices */
void calcAntPolRot(void)
{
	double complex u1p1[3], u1p2[3], u2p1[3], u2p2[3];
	char Pol_G = 'V';

	// Transmitter to Receiver
	tanUnitVec(Bistatic->Tgt, Bistatic->Tgr, Bistatic->idn, Bistatic->polTx, Bistatic->polRx, u1p1, u1p2, u2p1, u2p2);
	Bistatic->u_tr[0][0] = VoV_complex(u1p1, u2p1);
	Bistatic->u_tr[0][1] = VoV_complex(u1p2, u2p1);
	Bistatic->u_tr[1][0] = VoV_complex(u1p1, u2p2);
	Bistatic->u_tr[1][1] = VoV_complex(u1p2, u2p2);

	// Transmitter to Specular Point
	tanUnitVec(Bistatic->Tgt, Bistatic->Tgs, Bistatic->isn, Bistatic->polTx, Pol_G, u1p1, u1p2, u2p1, u2p2);
	Bistatic->u_ts[0][0] = VoV_complex(u1p1, u2p1);
	Bistatic->u_ts[0][1] = VoV_complex(u1p2, u2p1);
	Bistatic->u_ts[1][0] = VoV_complex(u1p1, u2p2);
	Bistatic->u_ts[1][1] = VoV_complex(u1p2, u2p2);

	// Specular Point to Receiver
	tanUnitVec(Bistatic->Tgs, Bistatic->Tgr, Bistatic->osp, Pol_G, Bistatic->polRx, u1p1, u1p2, u2p1, u2p2);
	Bistatic->u_sr[0][0] = VoV_complex(u1p1, u2p1);
	Bistatic->u_sr[0][1] = VoV_complex(u1p2, u2p1);
	Bistatic->u_sr[1][0] = VoV_complex(u1p1, u2p2);
	Bistatic->u_sr[1][1] = VoV_complex(u1p2, u2p2);
}
