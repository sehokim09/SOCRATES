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

void GyroProcessing(struct AcType *AC);
void MagnetometerProcessing(struct AcType *AC);
void CssProcessing(struct AcType *AC);
void FssProcessing(struct AcType *AC);
void StarTrackerProcessing(struct AcType *AC);
void GpsProcessing(struct AcType *AC);
void AccelProcessing(struct AcType *AC);
void WheelProcessing(struct AcType *AC);
void MtbProcessing(struct AcType *AC);
void ThreeAxisAttitudeCommand(struct SCType *S);

void calcRPY(struct SpecularType *Sp)
{
	double CBL[3][3],Roll,Pitch,Yaw;

	A2C(321, Sp->lookAngAzR, Sp->lookAngElR, 0, CBL);
	C2A(123, CBL, &Roll, &Pitch, &Yaw);
	Sp->roll = Roll;
	Sp->pitch = Pitch;
	Sp->yaw = Yaw;
}

void RxFsw(struct SCType *S)
{
	struct AcType *AC;
	struct AcCfsCtrlType *C;
	struct AcJointType *G;
    double HxB[3];
    double Hb[3];
	long i;

	AC = &S->AC;
	C = &AC->CfsCtrl;
	G = &AC->G[0];

	if (C->Init) {
		C->Init = 0;
		for(i=0;i<3;i++) FindPDGains(AC->MOI[i][i],0.1,0.7,&C->Kr[i],&C->Kp[i]);
		C->Kunl = 1.0E6;
		if(S->Nb>1){
		FindPDGains(100.0,0.2,1.0,&G->AngRateGain[0],&G->AngGain[0]);
		G->MaxAngRate[0] = 1.0*D2R;
		G->MaxTrq[0] = 10.0;
		}
	}
	/* .. Sensor Processing */
	GyroProcessing(AC);
	MagnetometerProcessing(AC);
	CssProcessing(AC);
	FssProcessing(AC);
	StarTrackerProcessing(AC);
	GpsProcessing(AC);

	// Control with Inp_Cmd

	// Commanded Control
	// .. Commanded Attitude
		// Find qrn, wrn and joint angle commands
		ThreeAxisAttitudeCommand(S);

		// .. Attitude Control
		// Form Error Signals
		QxQT(AC->qbn,AC->Cmd.qrn,AC->qbr);
		RECTIFYQ(AC->qbr);
		
		for(i=0;i<3;i++) {
			C->therr[i] = Limit(2.0*AC->qbr[i],-0.1,0.1);
			C->werr[i] = AC->wbn[i] - AC->Cmd.wrn[i];
			AC->Tcmd[i] = Limit(-C->Kr[i]*C->werr[i] - C->Kp[i]*C->therr[i],-0.1,0.1);
		}
		// .. Momentum Management
		if(AC->Nwhl>0){
			for(i=0;i<3;i++)
				Hb[i] = AC->MOI[i][i]*AC->wbn[i] + AC->Whl[i].H;
		}
		else{
			for(i=0;i<3;i++){
				Hb[i] = AC->MOI[i][i]*AC->wbn[i];
			}
		}
		
		if(AC->Nmtb>0){
			VxV(Hb,AC->bvb,HxB);
			for(i=0; i<3; i++) AC->Mcmd[i] = C->Kunl*HxB[i];
			//for(i=0; i<3; i++) AC->Mcmd[i] = C->Kunl*((AC->bvb[i] - AC->bvb_old[i])/0.1 + HxB[i]);
		}

		// .. Solar Array Steering
		//if(S->Nb>1)
		//	G->Cmd.Ang[0] = atan2(AC->svb[0],AC->svb[2]);

	/* .. Actuator Processing */
	WheelProcessing(AC);
	MtbProcessing(AC);
}

void CmdReport(void)
{
    static FILE *qbrFile, *therrFile, *werrFile, *WhlCmdFile, *MtqCmdFile;
    static char First = TRUE;

    if (First) {
       First = FALSE;
       qbrFile = FileOpen(SimPath,"qbr.42","wt");
       therrFile = FileOpen(SimPath,"therr.42","wt");
       werrFile = FileOpen(SimPath,"werr.42","wt");
       WhlCmdFile = FileOpen(SimPath, "WhlCmd.42", "wt");
       MtqCmdFile = FileOpen(SimPath, "MtqCmd.42", "wt");
    }
    fprintf(qbrFile,"%le %le %le %le\n",
       SC[0].AC.qbr[0], SC[0].AC.qbr[1], SC[0].AC.qbr[2], SC[0].AC.qbr[3]);
    fprintf(werrFile,"%le %le %le \n",
       SC[0].AC.CfsCtrl.werr[0], SC[0].AC.CfsCtrl.werr[1], SC[0].AC.CfsCtrl.werr[2]);
    fprintf(therrFile,"%le %le %le \n",
       SC[0].AC.CfsCtrl.therr[0], SC[0].AC.CfsCtrl.therr[1], SC[0].AC.CfsCtrl.therr[2]);
    fprintf(WhlCmdFile,"%le %le %le \n",
       SC[0].AC.Tcmd[0], SC[0].AC.Tcmd[1], SC[0].AC.Tcmd[2]);
    fprintf(MtqCmdFile,"%le %le %le \n",
       SC[0].AC.Mcmd[0], SC[0].AC.Mcmd[1], SC[0].AC.Mcmd[2]);
}

void ReportWhl(struct ScType *Rx)
{
	static FILE **whlFile;
	char filename[20];
	static long First = TRUE;
	long iRx, iWhl;

	if (First) {
		First = FALSE;
		whlFile = (FILE **)calloc(ReportSoOp->nADCS, sizeof(FILE *));
		if (whlFile == NULL) {
			printf("whlFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		for(iRx=0; iRx<ReportSoOp->nADCS; iRx++){	
			sprintf(filename, "Wheel_Rx%ld.out", iRx + ReportSoOp->FirstRxID);
			whlFile[iRx] = FileOpen(OutPath, filename, "w");
		}
	}
	iRx = Rx->ID - ReportSoOp->FirstRxID;

	fprintf(whlFile[iRx], "%le %le %le ", SC[iRx].AC.Tcmd[0], SC[iRx].AC.Tcmd[1], SC[iRx].AC.Tcmd[2]);
	for(iWhl=0; iWhl<SC[iRx].Nw; iWhl++){
		fprintf(whlFile[iRx],"%le ", SC[iRx].Whl[iWhl].Trq);
	}
	for(iWhl=0; iWhl<SC[iRx].Nw; iWhl++){
		fprintf(whlFile[iRx],"%le ", SC[iRx].Whl[iWhl].H);
	}
	fprintf(whlFile[iRx], "\n");

	if(EndFlag){
		fclose(whlFile[iRx]);
	}
}

void ReportMtb(struct ScType *Rx)
{
	static FILE **mtbFile;
	char filename[40];
	static long First = TRUE;
	long iRx, iMtb;

	if (First) {
		First = FALSE;
		mtbFile = (FILE **)calloc(ReportSoOp->nADCS, sizeof(FILE *));
		if (mtbFile == NULL) {
			printf("mtbFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		for(iRx=0; iRx<ReportSoOp->nADCS; iRx++){	
			sprintf(filename, "Magnetorquer_Rx%ld.out", iRx + ReportSoOp->FirstRxID);
			mtbFile[iRx] = FileOpen(OutPath, filename, "w");
		}
	}
	iRx = Rx->ID - ReportSoOp->FirstRxID;

	fprintf(mtbFile[iRx], "%le %le %le ", SC[iRx].AC.Mcmd[0], SC[iRx].AC.Mcmd[1], SC[iRx].AC.Mcmd[2]);
	for(iMtb=0; iMtb<SC[iRx].Nmtb; iMtb++){
		fprintf(mtbFile[iRx],"%le %le %le ", SC[iRx].MTB[iMtb].Trq[0], SC[iRx].MTB[iMtb].Trq[1], SC[iRx].MTB[iMtb].Trq[2]);
	}
	for(iMtb=0; iMtb<SC[iRx].Nmtb; iMtb++){
		fprintf(mtbFile[iRx],"%le ", SC[iRx].MTB[iMtb].M);
	}
	fprintf(mtbFile[iRx], "\n");

	if(EndFlag){
		fclose(mtbFile[iRx]);
	}
}

void ReportGyro(struct ScType *Rx)
{
	static FILE **gyroFile;
	char filename[20];
	static long First = TRUE;
	long iRx, iGyr;

	if (First) {
		First = FALSE;
		gyroFile = (FILE **)calloc(ReportSoOp->nADCS, sizeof(FILE *));
		if (gyroFile == NULL) {
			printf("gyroFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		for(iRx=0; iRx<ReportSoOp->nADCS; iRx++){	
			sprintf(filename, "Gyro_Rx%ld.out", iRx + ReportSoOp->FirstRxID);
			gyroFile[iRx] = FileOpen(OutPath, filename, "w");
		}
	}

	iRx = Rx->ID - ReportSoOp->FirstRxID;

	fprintf(gyroFile[iRx], "%le %le %le ", SC[iRx].AC.wbn[0]*R2D, SC[iRx].AC.wbn[1]*R2D, SC[iRx].AC.wbn[2]*R2D);
	for(iGyr=0; iGyr<SC[iRx].Ngyro; iGyr++){
		fprintf(gyroFile[iRx], "%le ", SC[iRx].Gyro[iGyr].TrueRate*R2D);
	}
	for(iGyr=0; iGyr<SC[iRx].Ngyro; iGyr++){
		fprintf(gyroFile[iRx], "%le ", SC[iRx].Gyro[iGyr].MeasRate*R2D);
	}
	for(iGyr=0; iGyr<SC[iRx].Ngyro; iGyr++){
		fprintf(gyroFile[iRx], "%le ", SC[iRx].Gyro[iGyr].Bias*R2D);
	}
	for(iGyr=0; iGyr<SC[iRx].Ngyro; iGyr++){
		fprintf(gyroFile[iRx], "%le ", SC[iRx].Gyro[iGyr].Angle*R2D);
	}
	fprintf(gyroFile[iRx],"\n");


	if(EndFlag){
		fclose(gyroFile[iRx]);
	}
}

void ReportMtm(struct ScType *Rx)
{
	static FILE **mtmFile;
	char filename[40];
	static long First = TRUE;
	long iRx, iMtm;

	if (First) {
		First = FALSE;
		mtmFile = (FILE **)calloc(ReportSoOp->nADCS, sizeof(FILE *));
		if (mtmFile == NULL) {
			printf("mtmFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		for(iRx=0; iRx<ReportSoOp->nADCS; iRx++){	
			sprintf(filename, "Magnetometer_Rx%ld.out", iRx + ReportSoOp->FirstRxID);
			mtmFile[iRx] = FileOpen(OutPath, filename, "w");
		}
	}

	iRx = Rx->ID - ReportSoOp->FirstRxID;

	fprintf(mtmFile[iRx], "%le %le %le ", SC[iRx].AC.bvb[0], SC[iRx].AC.bvb[1], SC[iRx].AC.bvb[2]);
	for(iMtm=0; iMtm<SC[iRx].Nmag; iMtm++){
		fprintf(mtmFile[iRx], "%le ", SC[iRx].MAG[iMtm].Field);
	}
	fprintf(mtmFile[iRx], "\n");

	if(EndFlag){
		fclose(mtmFile[iRx]);
	}
}

void ReportAcc(struct ScType *Rx)
{
	static FILE **accFile;
	char filename[20];
	static long First = TRUE;
	long iRx, iAcc;

	if (First) {
		First = FALSE;
		accFile = (FILE **)calloc(ReportSoOp->nADCS, sizeof(FILE *));
		if (accFile == NULL) {
			printf("accFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		for(iRx=0; iRx<ReportSoOp->nADCS; iRx++){	
			sprintf(filename, "Accel_Rx%ld.out", iRx + ReportSoOp->FirstRxID);
			accFile[iRx] = FileOpen(OutPath, filename, "w");
		}
	}
	
	iRx = Rx->ID - ReportSoOp->FirstRxID;

	for(iAcc=0; iAcc<SC[iRx].Nacc; iAcc++){
		fprintf(accFile[iRx], "%le ", SC[iRx].Accel[iAcc].TrueAcc);
	}
	for(iAcc=0; iAcc<SC[iRx].Nacc; iAcc++){
		fprintf(accFile[iRx], "%le ", SC[iRx].Accel[iAcc].MeasAcc);
	}

	fprintf(accFile[iRx],"\n");

	if(EndFlag){
		fclose(accFile[iRx]);
	}
}

void ReportTruthAtt(struct ScType *Rx)
{
	static FILE **attFile;
	char filename[40];
	static long First = TRUE;
	long iFile, iRx;
	double CBL[3][3], Roll, Pitch, Yaw;

	if (First) {
		First = FALSE;
		attFile = (FILE **)calloc(ReportSoOp->nADCS, sizeof(FILE *));
		if (attFile == NULL) {
			printf("attFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		for(iFile=0, iRx=ReportSoOp->FirstRxID; iFile<ReportSoOp->nADCS; iFile++, iRx++){	
			sprintf(filename, "TrueAttitude_Rx%ld.out", iRx);
			attFile[iFile] = FileOpen(OutPath, filename, "w");
		}
	}

	iFile = Rx->ID - ReportSoOp->FirstRxID;
	iRx = Rx->ID;

	// Julian Day
	if(ReportSoOp->JulDay){
		fprintf(attFile[iFile], "%lf ", UTC.JulDay);
	}
	// Position in ECI frame
	if(ReportSoOp->PosN){
		fprintf(attFile[iFile], "%lf %lf %lf ", SC[iRx].PosN[0], SC[iRx].PosN[1], SC[iRx].PosN[2]);
	}
	// Roll, Pitch, Yaw
	if(ReportSoOp->RPY){
		MxMT(SC[iRx].B[0].CN, SC[iRx].CLN, CBL);
		C2A(123, CBL, &Roll, &Pitch, &Yaw);
		fprintf(attFile[iFile], "%lf %lf %lf ", Roll*R2D, Pitch*R2D, Yaw*R2D);
	}
	// Angular Velocity
	if(ReportSoOp->AngVel){
		fprintf(attFile[iFile], "%le %le %le ", SC[iRx].B[0].wn[0]*R2D, SC[iRx].B[0].wn[1]*R2D, SC[iRx].B[0].wn[2]*R2D);
	}
	// Angular Momentum
	if(ReportSoOp->AngMom){
		fprintf(attFile[iFile], "%le %le %le ", SC[iRx].Hvb[0], SC[iRx].Hvb[1], SC[iRx].Hvb[2]);
	}
	// Magnetic Field
	if(ReportSoOp->MagField){
		fprintf(attFile[iFile], "%le %le %le ", SC[iRx].bvb[0], SC[iRx].bvb[1], SC[iRx].bvb[2]);
	}
	// Eclipse
	if(ReportSoOp->Eclipse){
		fprintf(attFile[iFile], "%ld ", SC[iRx].Eclipse);
	}
	fprintf(attFile[iFile], "\n");

	if(EndFlag){
		fclose(attFile[iFile]);
	}	
}

void ReportAttitude(struct ScType *Rx)
{	
	// True Data
	if(ReportSoOp->TruthAtt)
		ReportTruthAtt(Rx);
	// Gyro
	if(ReportSoOp->Gyr)
		ReportGyro(Rx);
	// Magnetometer
	if(ReportSoOp->Mtm)
		ReportMtm(Rx);
	// Accelerometer
	if(ReportSoOp->Acc)
		ReportAcc(Rx);
	// Reaction Wheels
	if(ReportSoOp->Whl)
		ReportWhl(Rx);
	// Magnetorquer
	if(ReportSoOp->Mtb)
		ReportMtb(Rx);
}
