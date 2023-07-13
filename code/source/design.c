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

void resetTime(void)
{
    long itime;

    SimTime = 0.0;
    itime = (long) ((SimTime+0.5*DTSIM)/(DTSIM));
    SimTime = ((double) itime)*DTSIM;
    DynTime = DynTime0 + SimTime;
    AtomicTime = DynTime - 32.184; /* TAI */
    CivilTime = AtomicTime - LeapSec; /* UTC "clock" time */
    GpsTime = AtomicTime - 19.0;

    TT.JulDay = TimeToJD(DynTime);
    TimeToDate(DynTime,&TT.Year,&TT.Month,&TT.Day,
        &TT.Hour,&TT.Minute,&TT.Second,DTSIM);
    TT.doy = MD2DOY(TT.Year,TT.Month,TT.Day);

    UTC.JulDay = TimeToJD(CivilTime);
    TimeToDate(CivilTime,&UTC.Year,&UTC.Month,&UTC.Day,
        &UTC.Hour,&UTC.Minute,&UTC.Second,DTSIM);
    UTC.doy = MD2DOY(UTC.Year,UTC.Month,UTC.Day);

    JDToGpsTime(TT.JulDay,&GpsRollover,&GpsWeek,&GpsSecond);
}

void manageOutFlag(long iout)
{
      long nout;

      nout = ((long) (DTOUT/DTSIM + 0.5));
      iout++;
      if(iout >= nout) {
         iout=0;
         OutFlag = TRUE;
      }
      else OutFlag = FALSE;
}

void reallocDesign(void)
{
    int iRx;

    Design->ncID = (int *) realloc(Design->ncID,sizeof(int)*Design->nRx);
	if (Design->ncID == NULL) {
		printf("Design->ncID calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

    Design->ncSuccessPropID = (int *) realloc(Design->ncSuccessPropID,sizeof(int)*Design->nRx);
	if (Design->ncSuccessPropID == NULL) {
		printf("Design->ncSuccessPropID calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

    Design->ncFlagSpotbeamID = (int *) realloc(Design->ncFlagSpotbeamID,sizeof(int)*Design->nRx);
	if (Design->ncFlagSpotbeamID == NULL) {
		printf("Design->ncFlagSpotbeamID calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

    for(iRx=0; iRx<Design->nRxPrev; iRx++){
        free(Design->ncCnt[iRx]);
        free(Design->ncSecID[iRx]);
        free(Design->ncLatID[iRx]);
        free(Design->ncLonID[iRx]);
        free(Design->ncLookAngAzDID[iRx]);
        free(Design->ncLookAngElDID[iRx]);
        free(Design->ncLookAngAzRID[iRx]);
        free(Design->ncLookAngElRID[iRx]);
        free(Design->ncRangeTRID[iRx]);
        free(Design->ncRangeTSID[iRx]);
        free(Design->ncRangeSRID[iRx]);
    }
    free(Design->ncCnt);
    free(Design->ncSecID);
    free(Design->ncLatID);
    free(Design->ncLonID);
    free(Design->ncLookAngAzDID);
    free(Design->ncLookAngElDID);
    free(Design->ncLookAngAzRID);
    free(Design->ncLookAngElRID);
    free(Design->ncRangeTRID);
    free(Design->ncRangeTSID);
    free(Design->ncRangeSRID);

    Design->ncCnt = (size_t **) calloc(Design->nRx, sizeof(size_t *));
    Design->ncSecID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLatID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLonID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLookAngAzDID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLookAngElDID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLookAngAzRID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLookAngElRID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncRangeTRID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncRangeTSID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncRangeSRID = (int **) calloc(Design->nRx, sizeof(int *));
    for(iRx=0; iRx<Design->nRx; iRx++){
        Design->ncCnt[iRx] = (size_t *) calloc(NSoOp, sizeof(size_t));
        Design->ncSecID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLatID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLonID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLookAngAzDID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLookAngElDID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLookAngAzRID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLookAngElRID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncRangeTRID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncRangeTSID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncRangeSRID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        if (Design->ncCnt[iRx] == NULL) {
            printf("Design->ncCnt[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncSecID[iRx] == NULL) {
            printf("Design->ncSecID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLatID[iRx] == NULL) {
            printf("Design->ncLatID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLonID[iRx] == NULL) {
            printf("Design->ncLonID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngAzDID[iRx] == NULL) {
            printf("Design->ncLookAngAzDID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngElDID[iRx] == NULL) {
            printf("Design->ncLookAngElDID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngAzRID[iRx] == NULL) {
            printf("Design->ncLookAngAzRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngElRID[iRx] == NULL) {
            printf("Design->ncLookAngElRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncRangeTRID[iRx] == NULL) {
            printf("Design->ncRangeTRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncRangeTSID[iRx] == NULL) {
            printf("Design->ncRangeTSID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncRangeSRID[iRx] == NULL) {
            printf("Design->ncRangeSRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
    }
}

void freeDesign(void)
{
    int iRx;

    for(iRx=0; iRx<Design->nRx; iRx++){
        free(Design->ncCnt[iRx]);
        free(Design->ncSecID[iRx]);
        free(Design->ncLatID[iRx]);
        free(Design->ncLonID[iRx]);
        free(Design->ncLookAngAzDID[iRx]);
        free(Design->ncLookAngElDID[iRx]);
        free(Design->ncLookAngAzRID[iRx]);
        free(Design->ncLookAngElRID[iRx]);
        free(Design->ncRangeTRID[iRx]);
        free(Design->ncRangeTSID[iRx]);
        free(Design->ncRangeSRID[iRx]);
    }
    free(Design->ncCnt);
    free(Design->ncSecID);
    free(Design->ncLatID);
    free(Design->ncLonID);
    free(Design->ncLookAngAzDID);
    free(Design->ncLookAngElDID);
    free(Design->ncLookAngAzRID);
    free(Design->ncLookAngElRID);
    free(Design->ncRangeTRID);
    free(Design->ncRangeTSID);
    free(Design->ncRangeSRID);
}

void freeRigidDyn(struct SCType *S)
{
    struct DynType *D;
    struct JointType *G;
    long i, Ig;

    D = &S->Dyn;

    for(i=0; i<S->Nb; i++){
        free(D->ConnectTable[i]);
        free(D->JointPathTable[i]);
        free(D->BodyPathTable[i]);
    }
    free(D->ConnectTable);
    free(D->JointPathTable);
    free(D->BodyPathTable);

    for(Ig=0; Ig<S->Ng; Ig++){
        G = &S->G[Ig];
        free(G->Anc);
    }

    DestroyMatrix(D->PAngVel, 3*S->Nb);
    DestroyMatrix(D->IPAngVel, 3*S->Nb);
    DestroyMatrix(D->PVel, 3*S->Nb);
    DestroyMatrix(D->mPVel, 3*S->Nb);
    free(D->BodyTrq);
    free(D->BodyFrc);
    free(D->u);
    free(D->uu);
    free(D->du);
    free(D->udot);
    free(D->x);
    free(D->xx);
    free(D->dx);
    free(D->xdot);
    free(D->h);
    free(D->hh);
    free(D->dh);
    free(D->hdot);
    free(D->xw);
    free(D->xxw);
    free(D->dxw);
    free(D->xwdot);    

    if(S->ConstraintsRequested){
        DestroyMatrix(D->PAngVelc, 3*S->Nb);
        DestroyMatrix(D->PVelc, 3*S->Nb);
        free(D->TotalTrq);
        free(D->TotalFrc);
        free(D->GenConstraintFrc);
    }
}

void freeFlexModes(struct SCType *S)
{
    struct DynType *D;
    struct BodyType *B;
    struct JointType *G;
    long Ib, Ig;

    for(Ib=0;Ib<S->Nb;Ib++) {
        B = &S->B[Ib];
        if (strcmp(B->FlexFileName,"NONE")) {
            // TO-DO-LIST
        }
    }
    for(Ig=0;Ig<S->Ng;Ig++) {
        G = &S->G[Ig];
        B = &S->B[G->Bin];
        if (B->Nf > 0) {
            DestroyMatrix(G->PSIi, 3);
            DestroyMatrix(G->THETAi, 3);
        }
        B = &S->B[G->Bout];
        if (B->Nf > 0) {
            DestroyMatrix(G->PSIo, 3);
            DestroyMatrix(G->THETAo, 3);
        }
    }
    D = &S->Dyn;
    DestroyMatrix(D->PAngVelf, 3*S->Nb);
    DestroyMatrix(D->IPAngVelf, 3*S->Nb);
    DestroyMatrix(D->PVelf, 3*S->Nb);
    DestroyMatrix(D->mPVelf, 3*S->Nb);
    free(D->uf);
    free(D->uuf);
    free(D->duf);
    free(D->ufdot);
    free(D->xf);
    free(D->xxf);
    free(D->dxf);
    free(D->xfdot);
    free(D->FlexAcc);
    free(D->FlexFrc);
    free(D->FlexInertiaFrc);
    DestroyMatrix(D->Mf, D->Nf);
    DestroyMatrix(D->PCPVelf, D->Nf);
    DestroyMatrix(D->HplusQetaPAngVelf, D->Nf);
}

void freeOrderNDynamics(struct SCType *S)
{
    long Ib;
    struct BodyType *B;

    for(Ib=0;Ib<S->Nb;Ib++) {
        B = &S->B[Ib];
        if (B->Nd > 0) {
            free(B->Gd);
        } 
    }
}

void freeAC(struct SCType *S)
{
    struct AcType *AC;

    AC = &S->AC;
    if (AC->Nb > 0) {
        free(AC->B);
    }
    if (AC->Ng > 0) {
        free(AC->G);
    }
    if (AC->Ngyro > 0) {
        free(AC->Gyro);
    }
    if (AC->Nmag > 0) {
        free(AC->MAG);
    }
    if (AC->Ncss>0){
        free(AC->CSS);
    }
    if(AC->Nfss>0){
        free(AC->FSS);
    }
    if(AC->Nst>0){
        free(AC->ST);
    }
    if(AC->Ngps>0){
        free(AC->GPS);
    }
    if(AC->Nacc>0){
        free(AC->Accel);
    }
    if(AC->Nwhl>0){
        free(AC->Whl);
    }
    if(AC->Nmtb>0){
        free(AC->MTB);
    }
    if(AC->Nthr>0){
        free(AC->Thr);
    }
}

void freeSC(void)
{
    long iSC;
    struct SCType *S;
    struct DynType *D;

    for(iSC=0; iSC<Design->nSCPrev; iSC++){
        S = &SC[iSC];
        D = &S->Dyn;
        free(D->ActiveState);
        free(D->ActiveStateIdx);
        DestroyMatrix(D->COEF, D->Nu + D->Nf);
        free(D->RHS);
        freeAC(S);
        freeOrderNDynamics(S);
        freeFlexModes(S);
        freeRigidDyn(S);
        free(S->B);
        free(S->G);
        free(S->Whl);
        free(S->MTB);
        free(S->Thr);
        free(S->Gyro);
        free(S->MAG);
        free(S->CSS);
        free(S->FSS);
        free(S->ST);
        free(S->GPS);
        free(S->Accel);
    }
    free(SC);
}

// Input: altitude in km, Output: inclination in deg
double calcSSOinc(double alt)
{   
    double rp, ra, sma, ecc = 0, mm, slr, ipar0, inc0, inc1, pmm;
    double drdt;
    int niter;
    double j2, req, mu;
    
    j2 = World[EARTH].J2; // zonal coefficient (nd)
    req = World[EARTH].rad * 1E-3; // Earth equatorial radius (m to km)
    mu = World[EARTH].mu * 1E-9; // gravitational constant (to km^3/sec^2)
    
    // compute perigee and apogee geocentric radii
    rp = req + alt;
    ra = rp;

    // compute semimajor axis and eccentricity
    sma = 0.5 * (rp + ra);
    
    // required nodal regression rate (radians/second)
    drdt = D2R * (360.0 / 365.2422) / 86400.0;
    
    // Keplerian mean motion (radians/second)
    mm = sqrt(mu / (sma * sma * sma));
    
    // orbital semiparameter
    slr = sma * (1.0 - ecc * ecc);
    
    // inclination initial guess parameter
    ipar0 = -(2.0 / 3.0) * slr * slr * drdt / (j2 * req * req * mm);
    
    // check for possible solution
    if(fabs(ipar0) > 1)
        printf(">> Error: No sun-synchronous solution.\n");
    else{
        // perform iteration
        inc0 = acos(ipar0);
        niter = 0;

        while(1){
            niter = niter + 1;
            // perturbed mean motion (radians/second)
            pmm = mm * (1.0 + 1.5 * j2 * req * req * sqrt(1.0 - ecc * ecc)
                 * (1.0 - 1.5 * sin(inc0) * sin(inc0)) / (slr * slr));

            // updated inclination
            inc1 = acos(-(2.0 / 3.0) * slr * slr * drdt / (j2 * req * req * pmm));

            // check for convergence or more than 100 iterations
            if (fabs(inc1 - inc0) <= 0.00000001 || niter > 100)    
                break;
            // reset inclination
            inc0 = inc1;
        }
    }
    return inc1*R2D;
}

void LoadEarth(void)
{
      struct OrbitType *Eph;
      double Zaxis[3] = {0.0,0.0,1.0};
      double GMST;

      char PlanetName[10][20] ={"Sun","Mercury","Venus","Earth","Mars",
                                "Jupiter","Saturn","Uranus","Neptune",
                                "Pluto"};
      char MapFileName[10][20] = {"NONE","Rockball","Venus.ppm","Earth.ppm","Mars.ppm",
         "Jupiter.ppm","Saturn.ppm","Uranus.ppm","Neptune.ppm","Iceball"};
      double Mu[10] = {1.32715E20,2.18E13,3.2485E14,3.986004418E14,
                       4.293E13,1.2761E17,3.792E16,5.788E15,6.8E15,
                       3.2E14};
                       //3.98604E14,
      double J2[10] = {0.0,0.0,0.0,1.08263E-3,1.96045E-3,0.0,0.0,0.0,0.0,0.0};
      double Rad[10] = {6.98E8,2.42E6,6.1E6,6.378145E6,3.41E6,
                        7.14E7,6.04E7,2.35E7,2.23E7,7.0E6};
      double W[10] = {2.69E-6,1.23E-6,2.94E-7,7.292115E-5,7.0882E-5,
                      1.7659E-4,1.6728E-4,1.631E-4,1.105E-4,0.0};
      double PriMerAngJ2000[10] = {0.0,329.71,160.26,190.16,176.868,284.95,38.90,203.81,253.18,236.77};
      /* Magnetic Field Dipole Strength, Wb-m */
      double DipoleMoment[10] = {0.0,0.0,0.0,7.943E15,0.0,
                                 0.0,0.0,0.0,0.0,0.0};
      /* Magnetic Field Dipole Axis Unit Vector */
      double DipoleAxis[10][3] = {{0.0,0.0,1.0},
                                  {0.0,0.0,1.0},
                                  {0.0,0.0,1.0},
                                  {-6.53286E-2,0.186549,-0.980271},
                                  {0.0,0.0,1.0},
                                  {0.0,0.0,1.0},
                                  {0.0,0.0,1.0},
                                  {0.0,0.0,1.0},
                                  {0.0,0.0,1.0},
                                  {0.0,0.0,1.0}};
      /* Magnetic Field Dipole Offset from Center, m */
      double DipoleOffset[10][3] = {{0.0,0.0,0.0},
                                    {0.0,0.0,0.0},
                                    {0.0,0.0,0.0},
                                    {-3.74461E5,2.44108E5,-1.58291E5},
                                    {0.0,0.0,0.0},
                                    {0.0,0.0,0.0},
                                    {0.0,0.0,0.0},
                                    {0.0,0.0,0.0},
                                    {0.0,0.0,0.0},
                                    {0.0,0.0,0.0}};
      float Color[11][3] = {{1.0f    , 1.0f     , 0.9f},      /* Sun */
                           {0.400318f, 0.347338f, 0.253973f}, /* Mercury */
                           {0.716824f, 0.676952f, 0.623907f}, /* Venus */
                           {0.212f   , 0.293502f, 0.522072f}, /* Earth */
                           {0.687493f, 0.454481f, 0.365368f}, /* Mars */
                           {0.793131f, 0.627618f, 0.477430f}, /* Jupiter */
                           {0.705187f, 0.677713f, 0.620916f}, /* Saturn */
                           {0.486074f, 0.584573f, 0.769742f}, /* Uranus */
                           {0.187558f, 0.243884f, 0.413025f}, /* Neptune */
                           {0.268063f, 0.268183f, 0.268204f}, /* Pluto */
                           {0.440417f, 0.441343f, 0.441084f}};/* Luna */
      unsigned char Glyph[11][14] = {
{0xc0,0xc0,0x00,0x00,0x18,0x66,0x42,0x99,0x99,0x42,0x66,0x18,0x00,0x00}, /* Sun */
{0xc0,0xc0,0x00,0x10,0x7c,0x10,0x38,0x44,0x82,0x82,0x44,0x38,0x44,0x82}, /* Mercury */
{0xc0,0xc0,0x00,0x10,0x10,0x7c,0x10,0x38,0x44,0x82,0x82,0x44,0x38,0x00}, /* Venus */
{0xc0,0xc0,0x00,0x00,0x38,0x54,0x92,0xfe,0x92,0x54,0x38,0x00,0x00,0x00}, /* Earth */
{0xc0,0xc0,0x00,0x00,0x38,0x44,0x82,0x82,0x82,0x44,0x3c,0x05,0x01,0x07}, /* Mars */
{0xc0,0xc0,0x00,0x04,0x04,0x7f,0x44,0x24,0x14,0x14,0x24,0xc2,0x00,0x00}, /* Jupiter */
{0xc0,0xc0,0x00,0x0c,0x08,0x44,0x42,0x42,0x64,0x58,0x40,0x40,0xe0,0x00}, /* Saturn */
{0xc0,0xc0,0x00,0x38,0x44,0x82,0x92,0x82,0x44,0x38,0x10,0x54,0x38,0x10}, /* Uranus */
{0xc0,0xc0,0x00,0x10,0xfe,0x10,0x38,0x54,0x92,0x92,0xd6,0x92,0x00,0x00}, /* Neptune */
{0xc0,0xc0,0x00,0x00,0xf8,0x80,0x80,0xf0,0x88,0x88,0xf0,0x00,0x00,0x00}, /* Pluto */
{0xc0,0xc0,0x00,0x00,0x18,0x70,0x60,0xe0,0xe0,0x60,0x70,0x18,0x00,0x00}};/* Luna */
    long HasAtmo[11] = {0,0,0,1,1,0,0,0,0,0,0};
    long i = EARTH, j;

    strcpy(World[i].Name,PlanetName[i]);
    strcpy(World[i].MapFileName,MapFileName[i]);
    strcpy(World[i].ColTexFileName,"NONE");
    strcpy(World[i].BumpTexFileName,"NONE");
    World[i].mu=Mu[i];
    World[i].J2 = J2[i];
    World[i].rad=Rad[i];
    World[i].w=W[i];
    World[i].PriMerAngJ2000 = PriMerAngJ2000[i]*D2R;
    World[i].Parent = SOL;
    World[i].eph.World = SOL;
    World[i].eph.mu = World[SOL].mu;
    World[i].DipoleMoment = DipoleMoment[i];
    for(j=0;j<3;j++) {
        World[i].DipoleAxis[j] = DipoleAxis[i][j];
        World[i].DipoleOffset[j] = DipoleOffset[i][j];
        World[i].Color[j] = Color[i][j];
    }
    World[i].Color[3] = 1.0;
    for(j=0;j<14;j++) World[i].Glyph[j] = Glyph[i][j];
    World[i].Atmo.Exists = HasAtmo[i];


    World[EARTH].Atmo.GasColor[0] = 0.17523;
    World[EARTH].Atmo.GasColor[1] = 0.40785;
    World[EARTH].Atmo.GasColor[2] = 1.0;
    World[EARTH].Atmo.DustColor[0] = 1.0;
    World[EARTH].Atmo.DustColor[1] = 1.0;
    World[EARTH].Atmo.DustColor[2] = 1.0;
    World[EARTH].Atmo.RayScat[0] = 5.8E-6;
    World[EARTH].Atmo.RayScat[1] = 13.5E-6;
    World[EARTH].Atmo.RayScat[2] = 33.1E-6;
    World[EARTH].Atmo.RayScaleHt = 8000.0;
    World[EARTH].Atmo.MieScat = 4.0E-6;
    World[EARTH].Atmo.MieScaleHt = 1200.0;
    World[EARTH].Atmo.MieG = 0.76;
    World[EARTH].Atmo.MaxHt = 8.0*World[EARTH].Atmo.RayScaleHt;
    World[EARTH].Atmo.rad = World[EARTH].rad + World[EARTH].Atmo.MaxHt;

/* .. Load planetary orbit elements for date of interest */
    PlanetEphemerides(i,TT.JulDay,World[i].eph.mu,
                        &World[i].eph.SMA,
                        &World[i].eph.ecc,
                        &World[i].eph.inc,
                        &World[i].eph.RAAN,
                        &World[i].eph.ArgP,
                        &World[i].eph.tp,
                        &World[i].eph.anom,
                        &World[i].eph.SLR,
                        &World[i].eph.alpha,
                        &World[i].eph.rmin,
                        &World[i].eph.MeanMotion,
                        &World[i].eph.Period);
      

    /* Planetocentric Inertial Reference Frames */
    A2C(123,-23.4392911*D2R,0.0,0.0,World[EARTH].CNH);
    C2Q(World[EARTH].CNH,World[EARTH].qnh);

    World[i].RadOfInfluence =
    RadiusOfInfluence(World[i].eph.mu,World[i].mu,World[i].eph.SMA);
    World[i].Type = PLANET;

    Eph = &World[i].eph;
    Eph2RV(Eph->mu,Eph->SLR,Eph->ecc,Eph->inc,Eph->RAAN,Eph->ArgP,
            DynTime-Eph->tp,Eph->PosN,Eph->VelN,&Eph->anom);
    for(j=0;j<3;j++) World[i].PosH[j] = Eph->PosN[j];
    World[i].PriMerAng = fmod(World[i].PriMerAngJ2000+World[i].w*DynTime,TwoPi);
    SimpRot(Zaxis,World[i].PriMerAng,World[i].CWN);
    C2Q(World[i].CWN,World[i].qwn);

/* .. Earth rotation is a special case */
    GMST = JD2GMST(UTC.JulDay);
    World[EARTH].PriMerAng = TwoPi*GMST;
    SimpRot(Zaxis,World[EARTH].PriMerAng,World[EARTH].CWN);
    C2Q(World[EARTH].CWN,World[EARTH].qwn);

    strcpy(World[EARTH].BumpTexFileName,"EarthBump.ppm");
}

void loadDesign(void)
{
    FILE *infile;
    char junk[200],newline, response[120];
    const char delimiter[2] = ",";
    long dummy, i, iSoOp, iTx;

    /* .. Read from file Inp_Report.txt */
    infile=FileOpen(SimPath,"Inp_Design.txt","r");

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    // Constellation Type
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	Design->consType = decodeStr(response);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    // Walker
    if(Design->consType == WALKER){
        // .. Number of # Rx Variables .. //
        fscanf(infile,"%ld %[^\n] %[\n]",&Design->nCandi_nRx,junk,&newline);
        Design->candi_nRx = (double *) calloc(Design->nCandi_nRx, sizeof(double));
        if(Design->candi_nRx == NULL){
            printf("Design->candi_nRx calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        readLineParsor(infile, Design->candi_nRx, delimiter);
        fscanf(infile,"%[^\n] %[\n]",junk,&newline);

        // .. Number of Altitude Variables .. //
        fscanf(infile,"%ld %[^\n] %[\n]",&Design->nCandi_alt,junk,&newline);
        Design->candi_alt = (double *) calloc(Design->nCandi_alt, sizeof(double));
        if(Design->candi_alt == NULL){
            printf("Design->candi_alt calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        readLineParsor(infile, Design->candi_alt, delimiter);

        fscanf(infile,"%[^\n] %[\n]",junk,&newline);

        // .. Number of Inclination Variables .. //
        fscanf(infile,"%ld %[^\n] %[\n]",&Design->nCandi_inc,junk,&newline);
        Design->candi_inc_str = (char **) calloc(Design->nCandi_inc, sizeof(char *));
        if(Design->candi_inc_str == NULL){
            printf("Design->candi_inc_str calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        for(i=0; i<Design->nCandi_inc; i++){
            Design->candi_inc_str[i] = (char *) calloc(10, sizeof(char));
            if(Design->candi_inc_str[i] == NULL){
                printf("Design->candi_inc_str[i] calloc returned null pointer.  Bailing out!\n");
                exit(1);
            }
        }
        readLineParsorStr(infile, Design->candi_inc_str);

        fscanf(infile,"%[^\n] %[\n]",junk,&newline);

        // .. Number of # Orbital Plane Variables .. //
        fscanf(infile,"%ld %[^\n] %[\n]",&Design->nCandi_nPlane,junk,&newline);
        Design->candi_nPlane = (double *) calloc(Design->nCandi_nPlane, sizeof(double));
        if(Design->candi_nPlane == NULL){
            printf("Design->candi_nPlane calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        readLineParsor(infile, Design->candi_nPlane, delimiter);  

        // Skip hybrid section
        fscanf(infile,"%[^\n] %[\n]",junk,&newline);
        fscanf(infile,"%[^\n] %[\n]",junk,&newline);
        fscanf(infile,"%ld %[^\n] %[\n]",&dummy,junk,&newline);
        for(i=0; i<dummy; i++){
            fscanf(infile,"%[^\n] %[\n]",junk,&newline);
            fscanf(infile,"%[^\n] %[\n]",junk,&newline);
            fscanf(infile,"%[^\n] %[\n]",junk,&newline);
            fscanf(infile,"%[^\n] %[\n]",junk,&newline);    
        }
    }
    else if(Design->consType == HYBRID){
        // Skip Walker section
        for(i=0; i<12; i++){
            fscanf(infile,"%[^\n] %[\n]",junk,&newline); 
        }
        // .. Hybrid Constellation Name    
    	fscanf(infile,"%s %[^\n] %[\n]",Design->consName,junk,&newline);

        // .. Number of # Orbital Plane Variables .. //
        fscanf(infile,"%ld %[^\n] %[\n]",&Design->nPlane,junk,&newline);
        Design->nRxPerPlane = (int *) calloc(Design->nPlane, sizeof(int));
        if(Design->nRxPerPlane == NULL){
            printf("Design->nRxPerPlane calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        Design->altPerPlane = (double *) calloc(Design->nPlane, sizeof(double));
        if(Design->altPerPlane == NULL){
            printf("Design->altPerPlane calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        Design->incPerPlane_str = (char **) calloc(Design->nPlane, sizeof(char *));
        for(i=0; i<Design->nPlane; i++){
            Design->incPerPlane_str[i] = (char *) calloc(10, sizeof(char));
            if(Design->incPerPlane_str[i] == NULL){
                printf("Design->incPerPlane_str calloc returned null pointer.  Bailing out!\n");
                exit(1);
            }
        }
        for(i=0; i<Design->nPlane; i++){
            fscanf(infile,"%[^\n] %[\n]",junk,&newline); 
            // .. Number of # Rx Variables .. //
            fscanf(infile,"%d %[^\n] %[\n]",&Design->nRxPerPlane[i],junk,&newline);
            // .. Number of Altitude Variables .. //
            fscanf(infile,"%lf %[^\n] %[\n]",&Design->altPerPlane[i],junk,&newline);
            // .. Number of Inclination Variables .. //
            fscanf(infile,"%s %[^\n] %[\n]",Design->incPerPlane_str[i],junk,&newline);
        }
    }
    else{
        printf("Wrong constellation type!\n");
        exit(1);
    }

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    // .. Real-time Transmitter Propagation .. //
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	Design->flagTxPropagation = decodeStr(response);
  	// .. Observation Time Window .. //
    fscanf(infile,"%ld %[^\n] %[\n]",&Design->timeWindow,junk,&newline);
    Design->timeWindow *= 3600; // hour to sec
  	// .. Minimum Reflected RCG .. //
    fscanf(infile,"%lf %[^\n] %[\n]",&Design->minReflectRCG,junk,&newline);
    Design->minReflectRCG *= 1E-27;
  	// .. Minimum Direct RCG .. //
    fscanf(infile,"%lf %[^\n] %[\n]",&Design->minDirectRCG,junk,&newline);
  	// .. Maximum RCG Difference .. //
    fscanf(infile,"%lf %[^\n] %[\n]",&Design->maxRCGdiff,junk,&newline);
    // .. Longitudinal Grid Divisior .. //
    fscanf(infile,"%d %[^\n] %[\n]",&Design->gridDivisor,junk,&newline);
    // .. Large file storage root directory .. //
	fscanf(infile,"%s %[^\n] %[\n]",Design->largeFileRoot,junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

    // .. Individual Receiver Data .. //
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	Design->flagReportSatData = decodeStr(response);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

    // .. Number of SoOp .. //
	fscanf(infile,"%ld %[^\n] %[\n]",&Design->nSoOp,junk,&newline);

    Design->SoOp = (struct DesignSoOpType *) calloc(Design->nSoOp, sizeof(struct DesignSoOpType));
    if(Design->SoOp == NULL){
        printf("Design->SoOp calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
        fscanf(infile,"%[^\n] %[\n]",junk,&newline);
        // .. SoOp Name .. //
	    fscanf(infile,"%s %[^\n] %[\n]",Design->SoOp[iSoOp].name,junk,&newline);
        // .. Number of Tx .. //
	    fscanf(infile,"%ld %[^\n] %[\n]",&Design->SoOp[iSoOp].nTx,junk,&newline);
        // .. Tx Name .. //
        Design->SoOp[iSoOp].TxName = (char **) calloc(Design->SoOp[iSoOp].nTx, sizeof(char *));
        if(Design->SoOp[iSoOp].TxName == NULL){
            printf("Design->SoOp[iSoOp].TxName calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }        
        for(iTx=0; iTx<Design->SoOp[iSoOp].nTx; iTx++){        
            Design->SoOp[iSoOp].TxName[iTx] = (char *) calloc(20, sizeof(char));
            if(Design->SoOp[iSoOp].TxName[iTx] == NULL){
                printf("Design->SoOp[iSoOp].TxName[iTx] calloc returned null pointer.  Bailing out!\n");
                exit(1);
            }  
    	    fscanf(infile,"%s %[^\n] %[\n]",Design->SoOp[iSoOp].TxName[iTx],junk,&newline);
        }
        // Receive Antenna Gain, Polarization
		fscanf(infile,"%lf %c %[^\n] %[\n]", &Design->SoOp[iSoOp].G_Rx, &Design->SoOp[iSoOp].polRx, junk,&newline);
        Design->SoOp[iSoOp].G_Rx_num = pow(10, Design->SoOp[iSoOp].G_Rx/10.0);
		// Receive Antenna Pattern
		fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
		Design->SoOp[iSoOp].antTag = decodeStr(response);
		// (GAUSSIAN) HPBW[deg], Sidelobe[dB], X-pol[dB], Res[deg]
		fscanf(infile, "%lf %lf %lf %lf %[^\n] %[\n]", &Design->SoOp[iSoOp].hpbw, &Design->SoOp[iSoOp].SLL, &Design->SoOp[iSoOp].XPL, &Design->SoOp[iSoOp].antPatRes, junk, &newline);
		// (USER_DEFINED) File Name for Antenna Pattern
		fscanf(infile,"%s %[^\n] %[\n]", Design->SoOp[iSoOp].antFileName[0],junk,&newline);
		fscanf(infile,"%s %[^\n] %[\n]", Design->SoOp[iSoOp].antFileName[1],junk,&newline);
        strcpy(Design->SoOp[iSoOp].antFileName[2], Design->SoOp[iSoOp].antFileName[1]);
        strcpy(Design->SoOp[iSoOp].antFileName[3], Design->SoOp[iSoOp].antFileName[0]);
    }
	fclose(infile);
}

void initOrbit(void)
{
    long Iorb, j, iPlane, iRxPerPlane;
    struct OrbitType *O, *Eph, *OrbBackup;
    double r1[3],rh[3],vh[3];
    double Zaxis[3] = {0.0,0.0,1.0};

    if (TimeMode == EXTERNAL_TIME) {
        printf("Initializing with External Time\n");
        RealSystemTime(&UTC.Year,&UTC.doy,&UTC.Month,&UTC.Day,
        &UTC.Hour,&UTC.Minute,&UTC.Second,DTSIM);
    }         
    CivilTime = DateToTime(UTC.Year,UTC.Month,UTC.Day,
        UTC.Hour,UTC.Minute,UTC.Second);
    AtomicTime = CivilTime + LeapSec;
    DynTime0 = AtomicTime + 32.184;
    GpsTime = AtomicTime - 19.0;
    DynTime = DynTime0;

    TT.JulDay = TimeToJD(DynTime);
    TimeToDate(DynTime,&TT.Year,&TT.Month,&TT.Day,
        &TT.Hour,&TT.Minute,&TT.Second,DTSIM);
    TT.doy = MD2DOY(TT.Year,TT.Month,TT.Day);
    
    UTC.JulDay = TimeToJD(CivilTime);
    UTC.doy = MD2DOY(UTC.Year,UTC.Month,UTC.Day);
    
    JDToGpsTime(TT.JulDay,&GpsRollover,&GpsWeek,&GpsSecond);

// .. Load Sun and Planets 
    LoadSun();
//    LoadPlanets();
    LoadEarth();
    // JPL planetary ephems 
    if (EphemOption == EPH_DE430) LoadDE430(ModelPath, TT.JulDay);

    /* .. Load Moons */
    if (World[EARTH].Exists) LoadMoonOfEarth();

// .. Locate Luna 
    if (World[LUNA].Exists){
        Eph = &World[LUNA].eph;
        /* Meeus computes Luna Position in geocentric ecliptic */
        LunaPosition(TT.JulDay,rh);
        LunaPosition(TT.JulDay+0.01,r1);
        for(j=0;j<3;j++) vh[j] = (r1[j]-rh[j])/(864.0);
        /* Convert to Earth's N frame */
        MxV(World[EARTH].CNH,rh,Eph->PosN);
        MxV(World[EARTH].CNH,vh,Eph->VelN);
        /* Find Luna's osculating elements */
        RV2Eph(DynTime,Eph->mu,Eph->PosN,Eph->VelN,
        &Eph->SMA,&Eph->ecc,&Eph->inc,&Eph->RAAN,
        &Eph->ArgP,&Eph->anom,&Eph->tp,&Eph->SLR,&Eph->alpha,
        &Eph->rmin,&Eph->MeanMotion,&Eph->Period);
        World[LUNA].PriMerAng =
        atan2(Eph->PosN[1],Eph->PosN[0])+Pi;
        SimpRot(Zaxis,World[LUNA].PriMerAng,World[LUNA].CWN);
        C2Q(World[LUNA].CWN,World[LUNA].qwn);
        for(j=0;j<3;j++) {
        World[LUNA].PosH[j] = rh[j] + World[EARTH].PosH[j];
        World[LUNA].VelH[j] = vh[j] + World[EARTH].VelH[j];
        }
    }
    
    //printf("Norb=%ld, NRx=%ld, NRxPrev=%ld\n", Norb, Design->nRx, Design->nRxPrev);
    if(Design->flagTxPropagation){
        OrbBackup = (struct OrbitType *) calloc(NTx, sizeof(struct OrbitType));
        memcpy(OrbBackup, Orb + Design->nRxPrev, sizeof(struct OrbitType)*NTx);
    }

    Orb = (struct OrbitType *) realloc(Orb, sizeof(struct OrbitType)*Norb);
    if (Orb == NULL) {
        printf("Orb calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    if(Design->consType == WALKER){
        for(Iorb=0; Iorb<Design->nRx; Iorb++){
            O = &Orb[Iorb];
            O->Exists = TRUE;
            O->World = EARTH;
            O->J2DriftEnabled = FALSE;
            O->Tag = Iorb;
            O->Epoch = DynTime;
            O->Regime = ORB_CENTRAL;
            O->mu = World[O->World].mu;
            O->ecc = Design->ecc;
            O->rmin = World[O->World].rad + Design->alt * 1.0E3;
            O->SLR = O->rmin*(1.0+O->ecc);
            O->alpha = (1.0-O->ecc)/O->rmin;
            if (O->alpha != 0.0) O->SMA = 1.0/O->alpha;
            if (O->alpha > 0.0) O->MeanMotion = sqrt(O->mu*O->alpha)*O->alpha;
            else O->MeanMotion = sqrt(-O->mu*O->alpha)*O->alpha;

            O->inc = Design->inc * D2R;
            O->RAAN = Design->RAAN[Iorb] * D2R;
            O->ArgP = Design->argP * D2R;
            O->anom = Design->anom[Iorb] * D2R;
            O->tp = O->Epoch - TimeSincePeriapsis(O->mu,O->SLR,O->ecc,O->anom);

            if (O->J2DriftEnabled) {
                OscEphToMeanEph(O->mu,World[O->World].J2,World[O->World].rad,DynTime0,O);
            }
            Eph2RV(O->mu,O->SLR,O->ecc,O->inc,
                    O->RAAN,O->ArgP,O->Epoch-O->tp,
                    O->PosN,O->VelN,&O->anom);

            FindCLN(O->PosN,O->VelN,O->CLN,O->wln);
        }
    }
    else if(Design->consType == HYBRID){
        Iorb = 0;
        for(iPlane=0; iPlane<Design->nPlane; iPlane++){
            for(iRxPerPlane=0; iRxPerPlane<Design->nRxPerPlane[iPlane]; iRxPerPlane++){
                O = &Orb[Iorb];
                O->Exists = TRUE;
                O->World = EARTH;
                O->J2DriftEnabled = FALSE;
                O->Tag = Iorb;
                O->Epoch = DynTime;
                O->Regime = ORB_CENTRAL;
                O->mu = World[O->World].mu;
                O->ecc = Design->ecc;
                O->rmin = World[O->World].rad + Design->altPerPlane[iPlane] * 1.0E3;
                O->SLR = O->rmin*(1.0+O->ecc);
                O->alpha = (1.0-O->ecc)/O->rmin;
                if (O->alpha != 0.0) O->SMA = 1.0/O->alpha;
                if (O->alpha > 0.0) O->MeanMotion = sqrt(O->mu*O->alpha)*O->alpha;
                else O->MeanMotion = sqrt(-O->mu*O->alpha)*O->alpha;
                
                if(strcmp(Design->incPerPlane_str[iPlane],"SSO")) // If not SSO
                    Design->inc = atof(Design->incPerPlane_str[iPlane]);
                else{ // If SSO
                    Design->inc = calcSSOinc(Design->altPerPlane[iPlane]);
                }
                O->inc = Design->inc * D2R;
                O->RAAN = Design->RAAN[Iorb] * D2R;
                O->ArgP = Design->argP * D2R;
                O->anom = Design->anom[Iorb] * D2R;
                O->tp = O->Epoch - TimeSincePeriapsis(O->mu,O->SLR,O->ecc,O->anom);

                if (O->J2DriftEnabled) {
                    OscEphToMeanEph(O->mu,World[O->World].J2,World[O->World].rad,DynTime0,O);
                }
                Eph2RV(O->mu,O->SLR,O->ecc,O->inc,
                        O->RAAN,O->ArgP,O->Epoch-O->tp,
                        O->PosN,O->VelN,&O->anom);

                FindCLN(O->PosN,O->VelN,O->CLN,O->wln);
                Iorb++;
            }
        }
    }
    if(Design->flagTxPropagation){
        memcpy(Orb + Design->nRx, OrbBackup, sizeof(struct OrbitType)*NTx);
        free(OrbBackup);
        for(Iorb=Design->nRx;Iorb<Norb;Iorb++) {
            if (Orb[Iorb].Exists) InitOrbit(&Orb[Iorb]);
        }
    }
}

void initSC(void)
{
    char filename[20];
    long iSC;
    struct SCType *S, *SCbackup;

    if(Design->flagTxPropagation){
        SCbackup = (struct SCType *) calloc(NTx, sizeof(struct SCType));
        memcpy(SCbackup, SC + Design->nRxPrev, sizeof(struct SCType)*NTx);
    }

    // Copy Receiver SC name
    strcpy(filename, SC[0].FileName);
    freeSC();
    SC = (struct SCType *) calloc(Nsc, sizeof(struct SCType));
    //SC = (struct SCType *) realloc(SC, sizeof(struct SCType)*Nsc);
    if (SC == NULL) {
        printf("SC calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    
    // Initialize Receivers
    for(iSC=0;iSC<Design->nRx;iSC++){
        S = &SC[iSC];
        S->Exists = TRUE;
        S->ID = iSC;
        S->RefOrb = iSC;
        strcpy(S->FileName, filename);
        InitSpacecraft(S);
    }

    // Initialize Transmitters
    if(Design->flagTxPropagation){
        memcpy(SC + Design->nRx, SCbackup, sizeof(struct SCType)*NTx);
        free(SCbackup);
        for(iSC=Design->nRx; iSC<Nsc; iSC++){
            S = &SC[iSC];
            S->Exists = TRUE;
            S->ID = iSC;
            S->RefOrb = iSC;
            InitSpacecraft(S);
        }
    }
}

void initConsPropagator(void)
{
    long iRow, iCol, iSoOp, iSc, iRx;
    //long rowSt, colSt;
    uint16_t rowSt, colSt;
    
    free(Sc);
    NTx = 0;
    for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
        NTx += SoOp[iSoOp].NTx;
    }
    
    // Update reference orbit number for transmitters
    for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
        SoOp[iSoOp].RefOrbFirst -= NRx;
        SoOp[iSoOp].RefOrbLast -= NRx;
    }

    RxSc = (struct ScType *) calloc(NRx,sizeof(struct ScType));
	if (RxSc == NULL) {
		printf("RxSc calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

    TxSc = (struct ScType *) calloc(NTx,sizeof(struct ScType));
	if (TxSc == NULL) {
		printf("TxSc calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
    for(iSc=0; iSc<NTx; iSc++){
        TxSc[iSc].ID = iSc;
    }

    Design->nCell.total = Grid[0].nRow * Grid[0].nCol;

    Design->landmask = (uint8_t **) calloc(Grid[0].nRow, sizeof(uint8_t *));
    struct EASEgridType  gridM1;
    struct MapType mapM1;
    char grid_name[8] = "M1.00";

    ease2_grid_info(&gridM1, grid_name);
    gridM1.lat = (double *)calloc(gridM1.nRow, sizeof(double));
    if (gridM1.lat == NULL) {
        printf("gridM1.>lat calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    gridM1.lon = (double *)calloc(gridM1.nCol, sizeof(double));
    if (gridM1.lon == NULL) {
        printf("gridM1.lon calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    gridM1.row = (long *)calloc(gridM1.nRow, sizeof(long));
    if (gridM1.row == NULL) {
        printf("gridM1.row calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    gridM1.col = (long *)calloc(gridM1.nCol, sizeof(long));
    if (gridM1.col == NULL) {
        printf("gridM1.col calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    ease2_map_info(&mapM1, gridM1.projection);
    ease2_inverseM(&gridM1);
    gridM1.latLimits[0] = gridM1.lat[gridM1.nRow-1];
    gridM1.latLimits[1] = gridM1.lat[0];
    gridM1.lonLimits[0] = gridM1.lon[0];
    gridM1.lonLimits[1] = gridM1.lon[gridM1.nCol-1];

    for(iRow=0; iRow<Grid[0].nRow; iRow++){
        Design->landmask[iRow] = (uint8_t *) calloc(Grid[0].nCol, sizeof(uint8_t));
        if (Design->landmask[iRow] == NULL) {
            printf("Design->landmask[iRow] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        for(iCol=0; iCol<Grid[0].nCol; iCol++){
            // Mapping indices
            LL2EzRC1km(Grid[0].lat[iRow], Grid[0].lon[iCol], &rowSt, &colSt);
            //ease2_LL2RC(&gridM1, Grid[0].lat[iRow], Grid[0].lon[iCol], &rowSt, &colSt);
            // Static data
            if(rowSt<14616 && colSt<34704){
                if(SttData->ExistLandMask){
                    Design->landmask[iRow][iCol] = SttData->landMask[rowSt][colSt];
                }
            }
            else{
                Design->landmask[iRow][iCol] = (uint8_t) NAN;		
            }

            Design->nCell.land += Design->landmask[iRow][iCol];
            Design->nCell.water += !Design->landmask[iRow][iCol];
        }
    }
    // Report grid landmask
    ReportGridLandmask();
    
    Design->nRx = NRx;
    Design->ncCnt = (size_t **) calloc(Design->nRx, sizeof(size_t *));
    Design->ncSecID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLatID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLonID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLookAngAzDID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLookAngElDID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLookAngAzRID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncLookAngElRID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncRangeTRID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncRangeTSID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncRangeSRID = (int **) calloc(Design->nRx, sizeof(int *));
    Design->ncFlagSpotbeamID = (int *) calloc(Design->nRx, sizeof(int));
    if (Design->ncFlagSpotbeamID == NULL) {
        printf("Design->ncFlagSpotbeamID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    for(iRx=0; iRx<Design->nRx; iRx++){
        Design->ncCnt[iRx] = (size_t *) calloc(NSoOp, sizeof(size_t));
        Design->ncSecID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLatID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLonID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLookAngAzDID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLookAngElDID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLookAngAzRID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncLookAngElRID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncRangeTRID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncRangeTSID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        Design->ncRangeSRID[iRx] = (int *) calloc(NSoOp, sizeof(int));
        if (Design->ncCnt[iRx] == NULL) {
            printf("Design->ncCnt[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncSecID[iRx] == NULL) {
            printf("Design->ncSecID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLatID[iRx] == NULL) {
            printf("Design->ncLatID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLonID[iRx] == NULL) {
            printf("Design->ncLonID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngAzDID[iRx] == NULL) {
            printf("Design->ncLookAngAzDID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngElDID[iRx] == NULL) {
            printf("Design->ncLookAngElDID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngAzRID[iRx] == NULL) {
            printf("Design->ncLookAngAzRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngElRID[iRx] == NULL) {
            printf("Design->ncLookAngElRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncRangeTRID[iRx] == NULL) {
            printf("Design->ncRangeTRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncRangeTSID[iRx] == NULL) {
            printf("Design->ncRangeTSID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncRangeSRID[iRx] == NULL) {
            printf("Design->ncRangeSRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
    }

    // Initialization
    Design->ecc = 0;
    
    free(SttData);
}

void initConsPerform(void)
{
    int iSoOp, iRow, iCol;
    uint16_t rowSt, colSt;
    struct DesignSoOpType *DS;
    char antFileName[40];

    Design->nCol = Grid[0].nCol / Design->gridDivisor;

    // .. Memory allocation  .. //
    Design->griddata = (int ****) calloc(Design->nSoOp, sizeof(int ***));
    Design->griddataCnt = (int ***) calloc(Design->nSoOp, sizeof(int **));
    for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
        Design->griddata[iSoOp] = (int ***) calloc(Grid[0].nRow, sizeof(int **));
        Design->griddataCnt[iSoOp] = (int **) calloc(Grid[0].nRow, sizeof(int *));
        for(iRow=0; iRow<Grid[0].nRow; iRow++){
            Design->griddata[iSoOp][iRow] = (int **) calloc(Design->nCol, sizeof(int *));
            Design->griddataCnt[iSoOp][iRow] = (int *) calloc(Design->nCol, sizeof(int));
            if (Design->griddataCnt[iSoOp][iRow] == NULL) {
                printf("Design->griddataCnt[iRow] calloc returned null pointer.  Bailing out!\n");
                exit(1);
            } 
            for(iCol=0; iCol<Design->nCol; iCol++){
                Design->griddata[iSoOp][iRow][iCol] = (int *) calloc(MAX_NSP, sizeof(int));
                if (Design->griddata[iSoOp][iRow][iCol] == NULL) {
                    printf("Design->griddata[iSoOp][iRow][iCol] calloc returned null pointer.  Bailing out!\n");
                    exit(1);
                }
            }
        }
        // .. Receive Antenna for Analysis .. //
        DS = &Design->SoOp[iSoOp];
        if(DS->antTag == USER_DEFINED){
            DS->antPattern = LoadAntPattern(DS->antFileName, &DS->antPatRes, DS->antPattern);
            sprintf(antFileName, "UD_%s", DS->name);
        }
        else if(DS->antTag == GAUSSIAN){
            DS->antPattern = LoadAntPatternGG(DS->SLL, DS->XPL, DS->hpbw, DS->antPatRes, DS->antPattern);
            sprintf(antFileName, "GG_%s", DS->name);
        }
        //ReportAntPattern(antFileName, DS->antPatRes, DS->antPattern);
    }

    Design->ncRevisitTimeID = (int *) calloc(Design->nSoOp, sizeof(int));
    if (Design->ncRevisitTimeID == NULL) {
        printf("Design->ncRevisitTimeID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->ncRevisitRowID = (int *) calloc(Design->nSoOp, sizeof(int));
    if (Design->ncRevisitRowID == NULL) {
        printf("Design->ncRevisitRowID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->ncRevisitColID = (int *) calloc(Design->nSoOp, sizeof(int));
    if (Design->ncRevisitColID == NULL) {
        printf("Design->ncRevisitColID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->ncAccessTime1stID = (int *) calloc(Design->nSoOp, sizeof(int));
    if (Design->ncAccessTime1stID == NULL) {
        printf("Design->ncAccessTime1stID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->ncAccessTime2ndID = (int *) calloc(Design->nSoOp, sizeof(int));
    if (Design->ncAccessTime2ndID == NULL) {
        printf("Design->ncAccessTime2ndID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->ncNspValidID = (int *) calloc(Design->nSoOp, sizeof(int));
    if (Design->ncNspValidID == NULL) {
        printf("Design->ncNspValidID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->ncNspInvalidID = (int *) calloc(Design->nSoOp, sizeof(int));
    if (Design->ncNspInvalidID == NULL) {
        printf("Design->ncNspInvalidID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->startpRevisit = (size_t *) calloc(Design->nSoOp, sizeof(size_t));
    if (Design->startpRevisit == NULL) {
        printf("Design->startpRevisit calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
        
    Design->startpAccessTime1st = (size_t *) calloc(Design->nSoOp, sizeof(size_t));
    if (Design->startpAccessTime1st == NULL) {
        printf("Design->startpAccessTime1st calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
        
    Design->startpAccessTime2nd = (size_t *) calloc(Design->nSoOp, sizeof(size_t));
    if (Design->startpAccessTime2nd == NULL) {
        printf("Design->startpAccessTime2nd calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->nWindow = (int) STOPTIME/Design->timeWindow;
    Design->obsFreq = (int *) calloc(Grid[0].nRow*Design->nCol, sizeof(int));
    if (Design->obsFreq == NULL) {
        printf("Design->obsFreq calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Design->nSpValid = (int *) calloc(Design->nSoOp, sizeof(int));
    if (Design->nSpValid == NULL) {
        printf("Design->nSpValid calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Design->nSpInvalid = (int *) calloc(Design->nSoOp, sizeof(int));
    if (Design->nSpInvalid == NULL) {
        printf("Design->nSpInvalid calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Design->nCell.total = Grid[0].nRow * Grid[0].nCol;

    Design->landmask = (uint8_t **) calloc(Grid[0].nRow, sizeof(uint8_t *));
    for(iRow=0; iRow<Grid[0].nRow; iRow++){
        Design->landmask[iRow] = (uint8_t *) calloc(Grid[0].nCol, sizeof(uint8_t));
        if (Design->landmask[iRow] == NULL) {
            printf("Design->landmask[iRow] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        for(iCol=0; iCol<Grid[0].nCol; iCol++){
            // Mapping indices
            LL2EzRC1km(Grid[0].lat[iRow], Grid[0].lon[iCol], &rowSt, &colSt);
            // Static data
            if(rowSt<14616 && colSt<34704){
                if(SttData->ExistLandMask){
                    Design->landmask[iRow][iCol] = SttData->landMask[rowSt][colSt];
                }
            }
            else{
                Design->landmask[iRow][iCol] = (uint8_t) NAN;		
            }

            Design->nCell.land += Design->landmask[iRow][iCol];
            Design->nCell.water += !Design->landmask[iRow][iCol];
        }
    }

    free(SttData);
    for(iRow=0; iRow<Grid[0].nRow; iRow++){
        free(Design->landmask[iRow]);
    }
    free(Design->landmask);

    Design->revisitThreshold = 3600;
}

void calcLaunchCost(long nRx, double alt, double inc, long nPlane, double *costDedicated, double *costRideshare, int *launchOption, int *nLaunch)
{
    double massSat = 25; // satellite unit mass (kg)
    double massPayloadMax = 345.6 - 0.067*alt - 1.175*inc;
    double c_rocket = 7.43E6; // cost of building a rocket

    double aL = World[EARTH].rad + 400.0E3;
    double aH = World[EARTH].rad + alt*1.0E3;
    double aT = (aL + aH) / 2;
    double I_sp = 300.0; // specific impulse of the propellant
    double alpha = 0.1;

    // delta V of launch vehicle
    double dV0 = 10000.0; // delV required to go from altitude 0km to 400km and 28.7 inc (m/s)
    double dV1 = sqrt(World[EARTH].mu) * (sqrt(2/aL - 1/aT) - sqrt(1/aL));
    double dV2 = sqrt(World[EARTH].mu*(2/aH - 1/aT) + World[EARTH].mu/aH - 2*sqrt(World[EARTH].mu*(2/aH-1/aT))*sqrt(World[EARTH].mu/aH)*cos(inc*D2R-0.500909495322373));
    double dV = dV0 + fabs(dV1) + fabs(dV2);
    
    // delta V to kg of propellant
    double g0 = 9.81; // (m/s2)
    double m_dry = 1200.0; // predefined structure mass of launch vehicle (kg)
    double m_pload = (double) nRx / (double) nPlane * massSat;
    double n_launch = ceil(m_pload/massPayloadMax) * nPlane;
    double m_pload_per_launch = (double) nRx / n_launch * massSat;
    double m_propD = (m_dry + m_pload_per_launch) * ( exp(dV/g0/I_sp) - 1 ); // dedicated
    double m_propR = m_pload_per_launch*( exp(dV/g0/I_sp) - 1 ); // rideshare
    
    // Translate amount of propellant into dollars
    double c_propellant = 1; // specific cost of propellant (dollar/kg)
    double c_dedicated = c_rocket + c_propellant * m_propD; // 4.26M$ corresponds to the cost of building the rocket
    double c_rideshare = c_propellant * m_propR;

    if(m_pload_per_launch < alpha * massPayloadMax){
        // rideshare added
        *launchOption = RIDESHARE;
    }
    else{
        // dedicated
        *launchOption = DEDICATED;
    }
    
    *costDedicated = n_launch * c_dedicated;
    *costRideshare = n_launch * c_rideshare;
    *nLaunch = (int) n_launch;
}

// This cost model provides relative cost estimates between constellations rather than accurate absolute cost estimates.
// ref: http://doi.org/10.1016/j.actaastro.2019/05/007
void calcCost(void)
{  
    // .. Constellation cost .. //
    double massSat = 25; // satellite unit mass (kg)
    double c_sat = (1064 + 35.5*pow(massSat, 1.261))*1.0E3; // cost of a single satellite (SMAD Table 11-11 SSCM)
    double b = 0.9; // parameter for learning curve
    double L = pow((double)Design->nRx, 1+log2(b)); // Learning curve factor

    Design->costCons = c_sat*L; // Constellation cost

    // .. Launch cost .. //    
    if(Design->consType == WALKER){
        calcLaunchCost(Design->nRx, Design->alt, Design->inc, Design->nPlane, &Design->costDedicated, &Design->costRideshare, &Design->launchOption, &Design->nLaunch);
    }
    else if(Design->consType == HYBRID){
        int iPlane, launchOption, nLaunch;
        long nRxPerPlane;
        double alt, inc, costDedicated, costRideshare;
        
        Design->costDedicated = 0;
        Design->costRideshare = 0;
        Design->launchOption = 0;
        Design->nLaunch = 0;
        for(iPlane=0; iPlane<Design->nPlane; iPlane++){
            nRxPerPlane = (long) Design->nRxPerPlane[iPlane];
            alt = Design->altPerPlane[iPlane];
            inc = atof(Design->incPerPlane_str[iPlane]);
            calcLaunchCost(nRxPerPlane, alt, inc, 1, &costDedicated, &costRideshare, &launchOption, &nLaunch);
            Design->costDedicated += costDedicated;
            Design->costRideshare += costRideshare;
            Design->launchOption = launchOption;
            Design->nLaunch += nLaunch;
        }
    }
}

void resetPerformVars(void)
{
    int iRx, iSoOp;

    Design->ncID = (int *) realloc(Design->ncID,sizeof(int)*Design->nRx);
    if (Design->ncID == NULL) {
        printf("Design->ncID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Design->ncSuccessPropID = (int *) realloc(Design->ncSuccessPropID,sizeof(int)*Design->nRx);
    if (Design->ncSuccessPropID == NULL) {
        printf("Design->ncSuccessPropID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Design->ncFlagSpotbeamID = (int *) realloc(Design->ncFlagSpotbeamID, sizeof(int)*Design->nRx);
    if (Design->ncFlagSpotbeamID == NULL) {
        printf("Design->ncFlagSpotbeamID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->ncSecID = (int **) realloc(Design->ncSecID, sizeof(int *)*Design->nRx);
    Design->ncLatID = (int **) realloc(Design->ncLatID, sizeof(int *)*Design->nRx);
    Design->ncLonID = (int **) realloc(Design->ncLonID, sizeof(int *)*Design->nRx);
    Design->ncLookAngAzDID = (int **) realloc(Design->ncLookAngAzDID, sizeof(int *)*Design->nRx);
    Design->ncLookAngElDID = (int **) realloc(Design->ncLookAngElDID, sizeof(int *)*Design->nRx);
    Design->ncLookAngAzRID = (int **) realloc(Design->ncLookAngAzRID, sizeof(int *)*Design->nRx);
    Design->ncLookAngElRID = (int **) realloc(Design->ncLookAngElRID, sizeof(int *)*Design->nRx);
    Design->ncRangeTRID = (int **) realloc(Design->ncRangeTRID, sizeof(int *)*Design->nRx);
    Design->ncRangeTSID = (int **) realloc(Design->ncRangeTSID, sizeof(int *)*Design->nRx);
    Design->ncRangeSRID = (int **) realloc(Design->ncRangeSRID, sizeof(int *)*Design->nRx);
    for(iRx=0; iRx<Design->nRx; iRx++){
        Design->ncSecID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        Design->ncLatID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        Design->ncLonID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        Design->ncLookAngAzDID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        Design->ncLookAngElDID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        Design->ncLookAngAzRID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        Design->ncLookAngElRID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        Design->ncRangeTRID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        Design->ncRangeTSID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        Design->ncRangeSRID[iRx] = (int *) calloc(Design->nSoOp, sizeof(int));
        if (Design->ncSecID[iRx] == NULL) {
            printf("Design->ncSecID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLatID[iRx] == NULL) {
            printf("Design->ncLatID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLonID[iRx] == NULL) {
            printf("Design->ncLonID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngAzDID[iRx] == NULL) {
            printf("Design->ncLookAngAzDID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngElDID[iRx] == NULL) {
            printf("Design->ncLookAngElDID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngAzRID[iRx] == NULL) {
            printf("Design->ncLookAngAzRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncLookAngElRID[iRx] == NULL) {
            printf("Design->ncLookAngElRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncRangeTRID[iRx] == NULL) {
            printf("Design->ncRangeTRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncRangeTSID[iRx] == NULL) {
            printf("Design->ncRangeTSID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
        if (Design->ncRangeSRID[iRx] == NULL) {
            printf("Design->ncRangeSRID[iRx] calloc returned null pointer.  Bailing out!\n");
            exit(1);
        }
    }

    for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
        Design->startpRevisit[iSoOp] = 0;
        Design->startpAccessTime1st[iSoOp] = 0;
        Design->startpAccessTime2nd[iSoOp] = 0;
        Design->nSpValid[iSoOp] = 0;
        Design->nSpInvalid[iSoOp] = 0;
    }
}

int checkSpDataNc(void)
{
    // IDs for the netCDF file, dimensions, and variables.
    int dimID;
    // Error handling
    int retval;
    // Output file name, dimension name
    char fullpath[300], dimName[40];
    // Loop
    int iRx, iSoOp;
    // flag
    int flagSuccess[1], flagSkip = TRUE, flagNoTx = FALSE;

    Design->ncID = (int *) realloc(Design->ncID,sizeof(int)*Design->nRx);
	if (Design->ncID == NULL) {
		printf("Design->ncID calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

    Design->ncSuccessPropID = (int *) realloc(Design->ncSuccessPropID,sizeof(int)*Design->nRx);
	if (Design->ncSuccessPropID == NULL) {
		printf("Design->ncSuccessPropID calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

    for(iRx=0; iRx<Design->nRx; iRx++){
        if(Design->consType == WALKER){
            sprintf(fullpath, "%s%ld_%d_%s_%ld_Rx%d.nc", Design->largeFilePath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane, iRx+1);        
        }
        else if(Design->consType == HYBRID){
            sprintf(fullpath, "%s%s_Rx%d.nc", Design->largeFilePath, Design->consName, iRx+1);
        }
        retval = nc_open(fullpath, NC_NOWRITE, &Design->ncID[iRx]);
        if(retval == NC_NOERR){
            // Check if all transmitters exist
            for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
                sprintf(dimName, "nSp_%s", SoOp[iSoOp].TxName);
                retval = nc_inq_dimid(Design->ncID[iRx], dimName, &dimID);
                if(retval != NC_NOERR){
                    flagNoTx = TRUE;
                }
            }
            /* Get the varid of the data variable, based on its name. */
            retval = nc_inq_varid(Design->ncID[iRx], "success", &Design->ncSuccessPropID[iRx]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_close(Design->ncID[iRx])))
                    ERR(retval);
                flagSkip = FALSE;
                break;
            }
            else{
                /* Read the data. */
                if ((retval = nc_get_var_int(Design->ncID[iRx], Design->ncSuccessPropID[iRx], &flagSuccess[0])))
                    ERR(retval);
                if ((retval = nc_close(Design->ncID[iRx])))
                    ERR(retval);
                if(flagSuccess[0] == TRUE)
                    flagSkip = TRUE;
                else{
                    flagSkip = FALSE;
                    break;
                }
            }
        }
        else{
            flagSkip = FALSE;
            break;
        }
    }

    if(flagNoTx)
        flagSkip = FALSE;

    return flagSkip;
}


int checkPerformNc(void)
{
    // Error handling
    int retval;
    // Output file name
    char fullpath[300];
    // flag
    int flagSuccess[1], flagSkip = TRUE;

    if(Design->consType == WALKER){
        sprintf(fullpath,"%s%ld_%d_%s_%ld_perform_newRCG.nc", Design->largeFilePath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
    }
    else if(Design->consType == HYBRID){
        sprintf(fullpath, "%s%s_perform_newRCG.nc", Design->largeFilePath, Design->consName);
    }
    retval = nc_open(fullpath, NC_NOWRITE, &Design->ncPerformID);
    if(retval == NC_NOERR){
        /* Get the varid of the data variable, based on its name. */
        retval = nc_inq_varid(Design->ncPerformID, "success", &Design->ncSuccessPerfID);
        if(retval == NC_ENOTVAR){
            if ((retval = nc_close(Design->ncPerformID)))
                ERR(retval);
            flagSkip = FALSE;
        }
        else{
            /* Read the data. */
            if ((retval = nc_get_var_int(Design->ncPerformID, Design->ncSuccessPerfID, &flagSuccess[0])))
                ERR(retval);
            if ((retval = nc_close(Design->ncPerformID)))
                ERR(retval);
            if(flagSuccess[0] == TRUE)
                flagSkip = TRUE;
            else{
                flagSkip = FALSE;
            }
        }
    }
    else{
        flagSkip = FALSE;
    }

    return flagSkip;
}

int checkCostNc(void)
{
    // Error handling
    int retval;
    // Output file name
    char fullpath[300];
    // flag
    int flagSkip = TRUE;
   
    if(Design->consType == WALKER){
        sprintf(fullpath,"%s%ld_%d_%s_%ld_cost.nc", Design->largeFilePath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
    }
    else if(Design->consType == HYBRID){
        sprintf(fullpath, "%s%s_cost.nc", Design->largeFilePath, Design->consName);
    }
    retval = nc_open(fullpath, NC_NOWRITE, &Design->ncCostID);
    if(retval != NC_NOERR){
        return flagSkip = FALSE;
    }

    return flagSkip;
}

void reopenSpDataNc(void)
{    
    int retval;
    int iRx;
    char fullpath[300];
    
    for(iRx=0; iRx<Design->nRx; iRx++){
        if(Design->consType == WALKER){
            sprintf(fullpath,"%s%ld_%d_%s_%ld_Rx%d.nc", Design->largeFilePath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane, iRx+1);
        }
        else if(Design->consType == HYBRID){
            sprintf(fullpath,"%s%s_Rx%d.nc", Design->largeFilePath, Design->consName, iRx+1);
        }
        if ((retval = nc_close(Design->ncID[iRx])))
            ERR(retval);        
        if ((retval = nc_open(fullpath, NC_WRITE, &Design->ncID[iRx])))
            ERR(retval);
    }
}

void writeFailNc(int ncid, int varid)
{
    int retval, flagSuccess[1];

    flagSuccess[0] = 0;
    if ((retval = nc_put_var_int(ncid, varid, &flagSuccess[0])))
        ERR(retval)
}

void writeSuccessNc(int ncid, int varid)
{
    int retval, flagSuccess[1];

    flagSuccess[0] = 1;
    if ((retval = nc_put_var_int(ncid, varid, &flagSuccess[0])))
        ERR(retval)
}

void createSpDataNc(void)
{
    // IDs for the netCDF file, dimensions, and variables.
    int dimID;
    // Error handling
    int retval;
    // Fill values
    int fillvalue_int = -9999;
    double fillvalue_double = -9999.0;
    // Variable name
    char dimName[30], varName[40];
    // Output file name
    char fullpath[300];
    // Loop
    int iRx, iSoOp;

    /* Create the file. */
    for(iRx=0; iRx<Design->nRx; iRx++){
        if(Design->consType == WALKER){
            sprintf(fullpath,"%s%ld_%d_%s_%ld_Rx%d.nc", Design->largeFilePath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane, iRx+1);
        }
        else if(Design->consType == HYBRID){
            sprintf(fullpath,"%s%s_Rx%d.nc", Design->largeFilePath, Design->consName, iRx+1);
        }
        retval = nc_create(fullpath, NC_NETCDF4, &Design->ncID[iRx]);
        if(retval == NC_EEXIST){
            if ((retval = nc_open(fullpath, NC_WRITE, &Design->ncID[iRx])))
                ERR(retval);
            if ((retval = nc_redef(Design->ncID[iRx])))
                ERR(retval);
        }
        else if(retval != NC_NOERR){
            ERR(retval);
        }

        retval = nc_inq_varid(Design->ncID[iRx], "success", &Design->ncSuccessPropID[iRx]);
        if(retval == NC_ENOTVAR){
            if ((retval = nc_def_dim(Design->ncID[iRx], "nSuccess", 1, &dimID)))
                ERR(retval);
            if ((retval = nc_def_var(Design->ncID[iRx], "success", NC_INT, 1, &dimID, &Design->ncSuccessPropID[iRx])))
                ERR(retval);
            if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncSuccessPropID[iRx], NC_FILL, &fillvalue_int)))
                ERR(retval);
        }

        for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
            /* Define the dimensions. */
            sprintf(dimName, "nSp_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_dimid(Design->ncID[iRx], dimName, &dimID);
            if(retval == NC_EBADDIM){
                if ((retval = nc_def_dim(Design->ncID[iRx], dimName, NC_UNLIMITED, &dimID)))
                    ERR(retval);
            }
            /* Define netCDF variables.*/
            sprintf(varName, "sec_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncSecID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_INT, 1, &dimID, &Design->ncSecID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncSecID[iRx][iSoOp], NC_FILL, &fillvalue_int)))
                    ERR(retval);
            }
            sprintf(varName, "lat_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLatID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_DOUBLE, 1, &dimID, &Design->ncLatID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncLatID[iRx][iSoOp], NC_FILL, &fillvalue_double)))
                    ERR(retval);
            }
            sprintf(varName, "lon_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLonID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_DOUBLE, 1, &dimID, &Design->ncLonID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncLonID[iRx][iSoOp], NC_FILL, &fillvalue_double)))
                    ERR(retval);
            }
            sprintf(varName, "lookAngAzD_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLookAngAzDID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_DOUBLE, 1, &dimID, &Design->ncLookAngAzDID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncLookAngAzDID[iRx][iSoOp], NC_FILL, &fillvalue_double)))
                    ERR(retval);
            }
            sprintf(varName, "lookAngElD_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLookAngElDID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_DOUBLE, 1, &dimID, &Design->ncLookAngElDID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncLookAngElDID[iRx][iSoOp], NC_FILL, &fillvalue_double)))
                    ERR(retval);
            }
            sprintf(varName, "lookAngAzR_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLookAngAzRID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_DOUBLE, 1, &dimID, &Design->ncLookAngAzRID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncLookAngAzRID[iRx][iSoOp], NC_FILL, &fillvalue_double)))
                    ERR(retval);
            }
            sprintf(varName, "lookAngElR_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLookAngElRID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_DOUBLE, 1, &dimID, &Design->ncLookAngElRID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncLookAngElRID[iRx][iSoOp], NC_FILL, &fillvalue_double)))
                    ERR(retval);
            }
            sprintf(varName, "rangeTR_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncRangeTRID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_DOUBLE, 1, &dimID, &Design->ncRangeTRID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncRangeTRID[iRx][iSoOp], NC_FILL, &fillvalue_double)))
                    ERR(retval);
            }
            sprintf(varName, "rangeTS_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncRangeTSID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_DOUBLE, 1, &dimID, &Design->ncRangeTSID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncRangeTSID[iRx][iSoOp], NC_FILL, &fillvalue_double)))
                    ERR(retval);
            }
            sprintf(varName, "rangeSR_%s", SoOp[iSoOp].TxName);
            retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncRangeSRID[iRx][iSoOp]);
            if(retval == NC_ENOTVAR){
                if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_DOUBLE, 1, &dimID, &Design->ncRangeSRID[iRx][iSoOp])))
                    ERR(retval);
                if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncRangeSRID[iRx][iSoOp], NC_FILL, &fillvalue_double)))
                    ERR(retval);                
            }
            if(!strcmp(SoOp[iSoOp].TxName, "MUOS")){
                /* Define netCDF variables.*/
                sprintf(varName, "flagSpotbeam");
                retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncFlagSpotbeamID[iRx]);
                if(retval == NC_ENOTVAR){
                    if ((retval = nc_def_var(Design->ncID[iRx], varName, NC_INT, 1, &dimID, &Design->ncFlagSpotbeamID[iRx])))
                        ERR(retval);
                    if ((retval = nc_def_var_fill(Design->ncID[iRx], Design->ncFlagSpotbeamID[iRx], NC_FILL, &fillvalue_int)))
                        ERR(retval);
                }
            }
            // Initialize
            Design->ncCnt[iRx][iSoOp] = 0;
        }
        /* End define mode. */
        if ((retval = nc_enddef(Design->ncID[iRx])))
            ERR(retval);

        writeFailNc(Design->ncID[iRx], Design->ncSuccessPropID[iRx]);    
    }
}

void createPerformNc(void)
{
    // IDs for the netCDF file, dimensions, and variables.
    int dimID, dim1ID, dim2ID, dim3ID, dimIDs[3];
    // Error handling
    int retval;
    // Fill values
    int fillvalue_int = -9999;
    // Attributes
    int nCell;
    // Variable name
    char dimName[40], varName[40];
    // Output file name
    char fullpath[300];
    // Loop
    int iSoOp;

    // Output directory
    if(Design->consType == WALKER){
        sprintf(Design->largeFilePath, "%s%ld_%d_%s_%ld/", Design->largeFileRoot, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
        sprintf(fullpath,"%s%ld_%d_%s_%ld_perform_newRCG.nc", Design->largeFilePath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
    }
    else if(Design->consType == HYBRID){
        sprintf(Design->largeFilePath, "%s%s/", Design->largeFileRoot, Design->consName);
        sprintf(fullpath,"%s%s_perform_newRCG.nc", Design->largeFilePath, Design->consName);
    }
    
    // .. Create the file .. //
    printf("path = %s\n", fullpath);

    retval = nc_create(fullpath, NC_NETCDF4, &Design->ncPerformID);
    if(retval == NC_EEXIST){
        if ((retval = nc_open(fullpath, NC_WRITE, &Design->ncPerformID)))
            ERR(retval);
        if ((retval = nc_redef(Design->ncPerformID)))
            ERR(retval);
    }
    else if(retval != NC_NOERR){
        ERR(retval);
    }

    // .. Global Attributes .. //
    nCell = (int) Design->nCell.land;
    
    if ((retval = nc_put_att_int(Design->ncPerformID, NC_GLOBAL, "nCell", NC_INT, 1, &nCell)))
        ERR(retval);

    // .. Variables for coverage metrics .. //
    for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
        // .. Revisit time .. //
        sprintf(dimName, "nRevisit_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_dim(Design->ncPerformID, dimName, NC_UNLIMITED, &dimID)))
            ERR(retval);

        sprintf(varName, "revisitTime_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_var(Design->ncPerformID, varName, NC_INT, 1, &dimID, &Design->ncRevisitTimeID[iSoOp])))
            ERR(retval);
        if ((retval = nc_def_var_fill(Design->ncPerformID, Design->ncRevisitTimeID[iSoOp], NC_FILL, &fillvalue_int)))
            ERR(retval);

        sprintf(varName, "revisitRow_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_var(Design->ncPerformID, varName, NC_INT, 1, &dimID, &Design->ncRevisitRowID[iSoOp])))
            ERR(retval);
        if ((retval = nc_def_var_fill(Design->ncPerformID, Design->ncRevisitRowID[iSoOp], NC_FILL, &fillvalue_int)))
            ERR(retval);

        sprintf(varName, "revisitCol_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_var(Design->ncPerformID, varName, NC_INT, 1, &dimID, &Design->ncRevisitColID[iSoOp])))
            ERR(retval);
        if ((retval = nc_def_var_fill(Design->ncPerformID, Design->ncRevisitColID[iSoOp], NC_FILL, &fillvalue_int)))
            ERR(retval);

        // .. Coverage .. // 
        // Visited
        sprintf(dimName, "nAccess1st_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_dim(Design->ncPerformID, dimName, NC_UNLIMITED, &dimID)))
            ERR(retval);
        sprintf(varName, "accessTime1st_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_var(Design->ncPerformID, varName, NC_INT, 1, &dimID, &Design->ncAccessTime1stID[iSoOp])))
            ERR(retval);
        if ((retval = nc_def_var_fill(Design->ncPerformID, Design->ncAccessTime1stID[iSoOp], NC_FILL, &fillvalue_int)))
            ERR(retval);

        // Revisited
        sprintf(dimName, "nAccess2nd_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_dim(Design->ncPerformID, dimName, NC_UNLIMITED, &dimID)))
            ERR(retval);
        sprintf(varName, "accessTime2nd_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_var(Design->ncPerformID, varName, NC_INT, 1, &dimID, &Design->ncAccessTime2ndID[iSoOp])))
            ERR(retval);
        if ((retval = nc_def_var_fill(Design->ncPerformID, Design->ncAccessTime2ndID[iSoOp], NC_FILL, &fillvalue_int)))
            ERR(retval);

        // .. Valid/Invalid points .. //
        // # valid points
        sprintf(dimName, "n_nSpValid_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_dim(Design->ncPerformID, dimName, 1, &dimID)))
            ERR(retval);
        sprintf(varName, "nSpValid_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_var(Design->ncPerformID, varName, NC_INT, 1, &dimID, &Design->ncNspValidID[iSoOp])))
            ERR(retval);
        if ((retval = nc_def_var_fill(Design->ncPerformID, Design->ncNspValidID[iSoOp], NC_FILL, &fillvalue_int)))
            ERR(retval);
        // # invalid points
        sprintf(dimName, "n_nSpInvalid_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_dim(Design->ncPerformID, dimName, 1, &dimID)))
            ERR(retval);
        sprintf(varName, "nSpInvalid_%s", Design->SoOp[iSoOp].name);
        if ((retval = nc_def_var(Design->ncPerformID, varName, NC_INT, 1, &dimID, &Design->ncNspInvalidID[iSoOp])))
            ERR(retval);
        if ((retval = nc_def_var_fill(Design->ncPerformID, Design->ncNspInvalidID[iSoOp], NC_FILL, &fillvalue_int)))
            ERR(retval);
    }
    // Frequency Combination
    if ((retval = nc_def_dim(Design->ncPerformID, "nWindow", Design->nWindow, &dim1ID)))
        ERR(retval);
    if ((retval = nc_def_dim(Design->ncPerformID, "nRow", Grid[0].nRow, &dim2ID)))
        ERR(retval);
    if ((retval = nc_def_dim(Design->ncPerformID, "nCol", Grid[0].nCol, &dim3ID)))
        ERR(retval);
    
    dimIDs[0] = dim1ID;
    dimIDs[1] = dim2ID;
    dimIDs[2] = dim3ID;

    if ((retval = nc_def_var(Design->ncPerformID, "obsFreq", NC_INT, 3, dimIDs, &Design->ncObsFreqID)))
        ERR(retval);
    if ((retval = nc_def_var_fill(Design->ncPerformID, Design->ncObsFreqID, NC_FILL, &fillvalue_int)))
        ERR(retval); 

    if ((retval = nc_def_dim(Design->ncPerformID, "nSuccess", 1, &dimID)))
        ERR(retval);
    if ((retval = nc_def_var(Design->ncPerformID, "success", NC_INT, 1, &dimID, &Design->ncSuccessPerfID)))
        ERR(retval);
    if ((retval = nc_def_var_fill(Design->ncPerformID, Design->ncSuccessPerfID, NC_FILL, &fillvalue_int)))
        ERR(retval); 

    /* End define mode. */
    if ((retval = nc_enddef(Design->ncPerformID)))
        ERR(retval);

    writeFailNc(Design->ncPerformID, Design->ncSuccessPerfID);
}

void createCostNc(void)
{
    // IDs for the netCDF file, dimensions, and variables.
    int dimID;
    // Error handling
    int retval;
    // Fill values
    float fillvalue_float = -9999.0;
    // Output file name
    char fullpath[300];

    // Output directory
    if(Design->consType == WALKER){
        sprintf(Design->largeFilePath, "%s%ld_%d_%s_%ld/", Design->largeFileRoot, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
        sprintf(fullpath,"%s%ld_%d_%s_%ld_cost.nc", Design->largeFilePath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
    }
    else if(Design->consType == HYBRID){
        sprintf(Design->largeFilePath, "%s%s/", Design->largeFileRoot, Design->consName);
        sprintf(fullpath,"%s%s_cost.nc", Design->largeFilePath, Design->consName);
    }
    
    // .. Create the file .. //
    if ((retval = nc_create(fullpath, NC_NETCDF4, &Design->ncCostID)))
        ERR(retval);
    // Cost
    if ((retval = nc_def_dim(Design->ncCostID, "nCost", 5, &dimID)))
        ERR(retval);

    if ((retval = nc_def_var(Design->ncCostID, "cost", NC_FLOAT, 1, &dimID, &Design->ncCostVarID)))
        ERR(retval);
    if ((retval = nc_def_var_fill(Design->ncCostID, Design->ncCostVarID, NC_FILL, &fillvalue_float)))
        ERR(retval); 

    /* End define mode. */
    if ((retval = nc_enddef(Design->ncCostID)))
        ERR(retval);
}

void writeFlagSpotbeam(long iRx, long iSoOp)
{
    int retval;
    size_t count[1] = {1};

    if ((retval = nc_put_vara_int(Design->ncID[iRx], Design->ncFlagSpotbeamID[iRx], &Design->ncCnt[iRx][iSoOp], count, &Design->flagSpotbeam)))
        ERR(retval)
}

void writeSpDataNc(long iRx, long iSoOp, struct SpecularType *Sp)
{
    int retval;
    int simTime = (int) SimTime;
    size_t count[1] = {1};

    // Second
    if ((retval = nc_put_vara_int(Design->ncID[iRx], Design->ncSecID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &simTime)))
        ERR(retval)
    // Latitude
    if ((retval = nc_put_vara_double(Design->ncID[iRx], Design->ncLatID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &Sp->lat)))
        ERR(retval)
    // Longitude
    if ((retval = nc_put_vara_double(Design->ncID[iRx], Design->ncLonID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &Sp->lon)))
        ERR(retval)
    // Look Angle Azimuth - Direct
    if ((retval = nc_put_vara_double(Design->ncID[iRx], Design->ncLookAngAzDID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &Sp->lookAngAzD)))
        ERR(retval)
    // Look Angle Elevation - Direct
    if ((retval = nc_put_vara_double(Design->ncID[iRx], Design->ncLookAngElDID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &Sp->lookAngElD)))
        ERR(retval)
    // Look Angle Azimuth - Reflected
    if ((retval = nc_put_vara_double(Design->ncID[iRx], Design->ncLookAngAzRID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &Sp->lookAngAzR)))
        ERR(retval)
    // Look Angle Elevation - Reflected
    if ((retval = nc_put_vara_double(Design->ncID[iRx], Design->ncLookAngElRID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &Sp->lookAngElR)))
        ERR(retval)
    // Range from Transmitter to Receiver
    if ((retval = nc_put_vara_double(Design->ncID[iRx], Design->ncRangeTRID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &Bistatic->r_tr)))
        ERR(retval)
    // Range from Transmitter to Specular Point
    if ((retval = nc_put_vara_double(Design->ncID[iRx], Design->ncRangeTSID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &Bistatic->r_ts)))
        ERR(retval)
    // Range from Specular Point to Receiver
    if ((retval = nc_put_vara_double(Design->ncID[iRx], Design->ncRangeSRID[iRx][iSoOp], &Design->ncCnt[iRx][iSoOp], count, &Bistatic->r_sr)))
        ERR(retval)

    Design->ncCnt[iRx][iSoOp]++;
}

void writeRevisitTimeNc(void)
{
    int retval;
    size_t count[1] = {1};
    int iRow, iCol, iSoOp, iSp, nSp;
    int revisitTime;

    for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
        for(iRow=0; iRow<Grid[0].nRow; iRow++){
            for(iCol=0; iCol<Design->nCol; iCol++){
                nSp = Design->griddataCnt[iSoOp][iRow][iCol];
                // Compute revisit time
                for(iSp=0; iSp<nSp-1; iSp++){
                    revisitTime = Design->griddata[iSoOp][iRow][iCol][iSp+1] - Design->griddata[iSoOp][iRow][iCol][iSp];
                    if(revisitTime > Design->revisitThreshold){
                        if ((retval = nc_put_vara_int(Design->ncPerformID, Design->ncRevisitTimeID[iSoOp], &Design->startpRevisit[iSoOp], count, &revisitTime)))
                            ERR(retval)
                        if ((retval = nc_put_vara_int(Design->ncPerformID, Design->ncRevisitRowID[iSoOp], &Design->startpRevisit[iSoOp], count, &iRow)))
                            ERR(retval)
                        if ((retval = nc_put_vara_int(Design->ncPerformID, Design->ncRevisitColID[iSoOp], &Design->startpRevisit[iSoOp], count, &iCol)))
                            ERR(retval)
                        Design->startpRevisit[iSoOp]++;
                    }
                }
            }
        }
    }
}

void writeAccessTimeNc(void)
{
    int retval;
    size_t count[1] = {1};
    int iRow, iCol, iSoOp;

    for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
        // Write time of 1st and 2nd accesses on each cell
        for(iRow=0; iRow<Grid[0].nRow; iRow++){
            for(iCol=0; iCol<Design->nCol; iCol++){
                if(Design->griddataCnt[iSoOp][iRow][iCol]>0){
                    if ((retval = nc_put_vara_int(Design->ncPerformID, Design->ncAccessTime1stID[iSoOp], &Design->startpAccessTime1st[iSoOp], count, &Design->griddata[iSoOp][iRow][iCol][0])))
                        ERR(retval)
                    Design->startpAccessTime1st[iSoOp]++;
                }
                if(Design->griddataCnt[iSoOp][iRow][iCol]>1){
                    if ((retval = nc_put_vara_int(Design->ncPerformID, Design->ncAccessTime2ndID[iSoOp], &Design->startpAccessTime2nd[iSoOp], count, &Design->griddata[iSoOp][iRow][iCol][1])))
                        ERR(retval)
                    Design->startpAccessTime2nd[iSoOp]++;
                }
            }
        }
    }
}

void writeNspNc(void)
{
    int retval, iSoOp;

    for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
        if ((retval = nc_put_var_int(Design->ncPerformID, Design->ncNspValidID[iSoOp], &Design->nSpValid[iSoOp])))
            ERR(retval)
        if ((retval = nc_put_var_int(Design->ncPerformID, Design->ncNspInvalidID[iSoOp], &Design->nSpInvalid[iSoOp])))
            ERR(retval)
    }
}

void writeObsFreqNc(int iteration)
{
    int retval;
    size_t start[3], count[3];
    int iRow, iCol, iSoOp, iSp, nSp, iWindow;
    int windowNum, obsFreqWindow;

    start[1] = 0;
    start[2] = iteration*Design->nCol;
    count[0] = 1;
    count[1] = Grid[0].nRow;
    count[2] = Design->nCol;

    for(iWindow=0; iWindow<Design->nWindow; iWindow++){
        for(iRow=0; iRow<Grid[0].nRow; iRow++){
            for(iCol=0; iCol<Design->nCol; iCol++){ // Sort SP data
                obsFreqWindow = NO_OBS;
                for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
                    nSp = Design->griddataCnt[iSoOp][iRow][iCol];
                    for(iSp=0; iSp<nSp; iSp++){
                        windowNum = Design->griddata[iSoOp][iRow][iCol][iSp] / Design->timeWindow;
                        if(windowNum == iWindow){
                            obsFreqWindow |= 1<<(iSoOp);
                        }
                    }
                }
                Design->obsFreq[iRow*Design->nCol + iCol] = obsFreqWindow; 
            }
        }
        start[0] = iWindow;
        if ((retval = nc_put_vara_int(Design->ncPerformID, Design->ncObsFreqID, start, count, &Design->obsFreq[0])))
            ERR(retval);
    }
}
void writeCostNc(void)
{
    int retval;
    float cost[5];

    // Calculate cost
    calcCost();
    // Result
    cost[0] = (float) Design->costCons;
    cost[1] = (float) Design->costDedicated;
    cost[2] = (float) Design->costRideshare;
    cost[3] = (float) Design->launchOption;
    cost[4] = (float) Design->nLaunch;
    // Write to NetCDF
    if ((retval = nc_put_var_float(Design->ncCostID, Design->ncCostVarID, &cost[0])))
        ERR(retval)
}

void readSpDataNc(int iteration)
{
    // IDs for the netCDF file, dimensions, and variables.
    int dimID;
    // Error handling
    int retval;
    // Dimension and variable
    char dimName[30], varName[40], TxName[20];
    size_t dimNSp;
    // Output file name
    char fullpath[300];
    // Loop
    int iRx, iSoOp, iTx, iSp;
    long iRow, iCol;
    // Flag
    int flagValid;
    // For RCG constraints
    double RCG_D, RCG_R, antGainD, antGainR;
    double threshold_RCG_D[3] = {2.51353004264460e-15, 4.70350610913321e-16, 1.28655266830211e-16};
    struct DesignSoOpType *DS;
    // Grid
    struct EASEgridType *G;

    // Reset variables
    for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
        for(iRow=0; iRow<Grid[0].nRow; iRow++){
            for(iCol=0; iCol<Design->nCol; iCol++){
                Design->griddataCnt[iSoOp][iRow][iCol] = 0;
                for(iSp=0; iSp<MAX_NSP; iSp++)
                    Design->griddata[iSoOp][iRow][iCol][iSp] = 0;
            }
        }
    }
    
    /* Create the file. */
    for(iRx=0; iRx<Design->nRx; iRx++){
        if(Design->consType == WALKER){
            sprintf(fullpath,"%s%ld_%d_%s_%ld_Rx%d.nc", Design->largeFilePath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane, iRx+1);
        }
        else if(Design->consType == HYBRID){
            sprintf(fullpath,"%s%s_Rx%d.nc", Design->largeFilePath, Design->consName, iRx+1);
        }
        /* Open the file. NC_NOWRITE tells netCDF we want read-only access to the file.*/
        if ((retval = nc_open(fullpath, NC_NOWRITE, &Design->ncID[iRx])))
            ERR(retval);
            
        for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
            DS = &Design->SoOp[iSoOp];
            for(iTx=0; iTx<DS->nTx; iTx++){
                strcpy(TxName, DS->TxName[iTx]);
                sprintf(dimName, "nSp_%s", TxName);
                if ((retval = nc_inq_dimid(Design->ncID[iRx], dimName, &dimID)))
                    ERR(retval);
                if ((retval = nc_inq_dimlen(Design->ncID[iRx], dimID, &dimNSp)))
                    ERR(retval);

                if(dimNSp > 0){
                    Design->accessSec = (int *) realloc(Design->accessSec, sizeof(int)*dimNSp);
                    if (Design->accessSec == NULL) {
                        printf("Design->accessSec calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }
                    Design->accessLat = (double *) realloc(Design->accessLat, sizeof(double)*dimNSp);
                    if (Design->accessLat == NULL) {
                        printf("Design->accessLat calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }
                    Design->accessLon = (double *) realloc(Design->accessLon, sizeof(double)*dimNSp);
                    if (Design->accessLon == NULL) {
                        printf("Design->accessLon calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }
                    Design->accessLookAngAzD = (double *) realloc(Design->accessLookAngAzD, sizeof(double)*dimNSp);
                    if (Design->accessLookAngAzD == NULL) {
                        printf("Design->accessLookAngAzD calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }
                    Design->accessLookAngElD = (double *) realloc(Design->accessLookAngElD, sizeof(double)*dimNSp);
                    if (Design->accessLookAngElD == NULL) {
                        printf("Design->accessLookAngElD calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }
                    Design->accessLookAngAzR = (double *) realloc(Design->accessLookAngAzR, sizeof(double)*dimNSp);
                    if (Design->accessLookAngAzR == NULL) {
                        printf("Design->accessLookAngAzR calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }
                    Design->accessLookAngElR = (double *) realloc(Design->accessLookAngElR, sizeof(double)*dimNSp);
                    if (Design->accessLookAngElR == NULL) {
                        printf("Design->accessLookAngElR calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }
                    Design->accessRangeTR = (double *) realloc(Design->accessRangeTR, sizeof(double)*dimNSp);
                    if (Design->accessRangeTR == NULL) {
                        printf("Design->accessRangeTR calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }
                    Design->accessRangeTS = (double *) realloc(Design->accessRangeTS, sizeof(double)*dimNSp);
                    if (Design->accessRangeTS == NULL) {
                        printf("Design->accessRangeTS calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }
                    Design->accessRangeSR = (double *) realloc(Design->accessRangeSR, sizeof(double)*dimNSp);
                    if (Design->accessRangeSR == NULL) {
                        printf("Design->accessRangeSR calloc returned null pointer.  Bailing out!\n");
                        exit(1);
                    }

                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "sec_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncSecID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_int(Design->ncID[iRx], Design->ncSecID[iRx][iSoOp], &Design->accessSec[0])))
                        ERR(retval);
                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "lat_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLatID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_double(Design->ncID[iRx], Design->ncLatID[iRx][iSoOp], &Design->accessLat[0])))
                        ERR(retval);
                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "lon_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLonID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_double(Design->ncID[iRx], Design->ncLonID[iRx][iSoOp], &Design->accessLon[0])))
                        ERR(retval);
                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "lookAngAzD_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLookAngAzDID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_double(Design->ncID[iRx], Design->ncLookAngAzDID[iRx][iSoOp], &Design->accessLookAngAzD[0])))
                        ERR(retval);
                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "lookAngElD_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLookAngElDID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_double(Design->ncID[iRx], Design->ncLookAngElDID[iRx][iSoOp], &Design->accessLookAngElD[0])))
                        ERR(retval);
                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "lookAngAzR_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLookAngAzRID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_double(Design->ncID[iRx], Design->ncLookAngAzRID[iRx][iSoOp], &Design->accessLookAngAzR[0])))
                        ERR(retval);
                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "lookAngElR_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncLookAngElRID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_double(Design->ncID[iRx], Design->ncLookAngElRID[iRx][iSoOp], &Design->accessLookAngElR[0])))
                        ERR(retval);
                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "rangeTR_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncRangeTRID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_double(Design->ncID[iRx], Design->ncRangeTRID[iRx][iSoOp], &Design->accessRangeTR[0])))
                        ERR(retval);
                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "rangeTS_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncRangeTSID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_double(Design->ncID[iRx], Design->ncRangeTSID[iRx][iSoOp], &Design->accessRangeTS[0])))
                        ERR(retval);
                    /* Get the varid of the data variable, based on its name. */
                    sprintf(varName, "rangeSR_%s", TxName);
                    if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncRangeSRID[iRx][iSoOp])))
                        ERR(retval);
                    /* Read the data. */
                    if ((retval = nc_get_var_double(Design->ncID[iRx], Design->ncRangeSRID[iRx][iSoOp], &Design->accessRangeSR[0])))
                        ERR(retval);
                    if(!strcmp(DS->name, "MUOS")){
                        Design->accessSpotbeam = (int *) realloc(Design->accessSpotbeam, sizeof(int)*dimNSp);
                        if (Design->accessSpotbeam == NULL) {
                            printf("Design->accessSpotbeam calloc returned null pointer.  Bailing out!\n");
                            exit(1);
                        }
                        /* Get the varid of the data variable, based on its name. */
                        sprintf(varName, "flagSpotbeam");
                        if ((retval = nc_inq_varid(Design->ncID[iRx], varName, &Design->ncFlagSpotbeamID[iRx])))
                            ERR(retval);
                        /* Read the data. */
                        if ((retval = nc_get_var_int(Design->ncID[iRx], Design->ncFlagSpotbeamID[iRx], &Design->accessSpotbeam[0])))
                            ERR(retval);
                    }
                    for(iSp=0; iSp<dimNSp; iSp++){

                        if(!strcmp(DS->name, "MUOS"))
                            flagValid = Design->accessSpotbeam[iSp];
                        else
                            flagValid = TRUE;
                        if(flagValid){
                            G = &Grid[0];
                            // Mapping indices
                            ease2_LL2RC(G, Design->accessLat[iSp], Design->accessLon[iSp], &iRow, &iCol);
                            iCol = iCol - iteration*Design->nCol;
                            if(iCol < Design->nCol && iCol >= 0){
                                // Read antenna gain
                                if(DS->antTag == IDEAL){
                                    // Transmitter direction
                                    Bistatic->g_rt[0][0] = 1; Bistatic->g_rt[0][1] = 0;
                                    Bistatic->g_rt[1][0] = 0; Bistatic->g_rt[1][1] = 1;
                                    // Specular direction
                                    Bistatic->g_rs[0][0] = 1; Bistatic->g_rs[0][1] = 0;
                                    Bistatic->g_rs[1][0] = 0; Bistatic->g_rs[1][1] = 1;
                                }
                                else if(DS->antTag == GAUSSIAN){
                                    // Transmitter direction
                                    LookupAntPattern(DS->antPattern, DS->antPatRes, Pi - Design->accessLookAngElD[iSp], Pi - Design->accessLookAngAzD[iSp], Bistatic->g_rt);
                                    // Specular direction
                                    LookupAntPattern(DS->antPattern, DS->antPatRes, Design->accessLookAngElR[iSp], Design->accessLookAngAzR[iSp], Bistatic->g_rs);
                                }
                                else if(DS->antTag == USER_DEFINED){
                                    // Transmitter direction
                                    LookupAntPattern(DS->antPattern, DS->antPatRes, Design->accessLookAngElD[iSp], Design->accessLookAngAzD[iSp], Bistatic->g_rt);
                                    // Specular direction
                                    LookupAntPattern(DS->antPattern, DS->antPatRes, Design->accessLookAngElR[iSp], Design->accessLookAngAzR[iSp], Bistatic->g_rs);
                                }
                                
                                antGainR = cabs(Bistatic->g_rs[0][1])*cabs(Bistatic->g_rs[0][1]) * DS->G_Rx_num;

                                // Check RCG constraints
                                if(!strcmp(TxName, "GPS")){
                                    RCG_R = antGainR / (Design->accessRangeTS[iSp]*Design->accessRangeTS[iSp]) / (Design->accessRangeSR[iSp]*Design->accessRangeSR[iSp]);
                                    if(RCG_R > Design->minReflectRCG)
                                        flagValid = TRUE;
                                    else
                                        flagValid = FALSE;
                                }
                                else{
                                    //RCG_R = antGainR / (Design->accessRangeTS[iSp] + Design->accessRangeSR[iSp]) / (Design->accessRangeTS[iSp] + Design->accessRangeSR[iSp]);
                                    
                                    antGainD = cabs(Bistatic->g_rt[0][0])*cabs(Bistatic->g_rt[0][0]) * DS->G_Rx_num;
                                    RCG_D = antGainD / Design->accessRangeTR[iSp] / Design->accessRangeTR[iSp];

                                    //if(RCG_D > Design->minDirectRCG){ // Constraint 1
                                    //if(fabs(10*log10(RCG_D) - 10*log10(RCG_R)) < Design->maxRCGdiff) // Constraint 2
                                    if(RCG_D > threshold_RCG_D[iSoOp])
                                        flagValid = TRUE;
                                    else
                                        flagValid = FALSE;
                                    //}
                                    //else
                                    //    flagValid = FALSE;
                                }

                                // Sort data into a grid
                                if(flagValid){
                                    Design->nSpValid[iSoOp]++;
                                    Design->griddata[iSoOp][iRow][iCol][Design->griddataCnt[iSoOp][iRow][iCol]++] = Design->accessSec[iSp];
                                    if(Design->griddataCnt[iSoOp][iRow][iCol] == MAX_NSP){
                                        printf("Error: nSp exceeds MAX_NSP, file:%s\n", fullpath);
                                        exit(1);
                                    }
                                }
                                else{
                                    Design->nSpInvalid[iSoOp]++;
                                }
                            }
                        }
                    }
                }
            }
        }
        if ((retval = nc_close(Design->ncID[iRx])))
            ERR(retval);
    }
/*
    free(Design->accessSec);
    free(Design->accessRow);
    free(Design->accessCol);
    free(Design->accessLookAngAzD);
    free(Design->accessLookAngElD);
    free(Design->accessLookAngAzR);
    free(Design->accessLookAngElR);
    free(Design->accessRangeTR);
    free(Design->accessRangeTS);
    free(Design->accessRangeSR);*/
}

void syncSpDataNc(void)
{
    int retval, iRx;

    for(iRx=0; iRx<Design->nRx; iRx++){
        if ((retval = nc_sync(Design->ncID[iRx])))
            ERR(retval);
    }

}

void closeSpDataNc(void)
{
    int retval, iRx;

    for(iRx=0; iRx<Design->nRx; iRx++){    
        writeSuccessNc(Design->ncID[iRx], Design->ncSuccessPropID[iRx]);
        if ((retval = nc_close(Design->ncID[iRx])))
            ERR(retval);
    }
}

void closePerformNc(void)
{
    int retval;

    writeSuccessNc(Design->ncPerformID, Design->ncSuccessPerfID);
    if ((retval = nc_close(Design->ncPerformID)))
        ERR(retval);
}

void closeCostNc(void)
{
    int retval;
    if ((retval = nc_close(Design->ncCostID)))
        ERR(retval);
}

void storeSpDataNc(void)
{
    char oldFilename[300], newFilename[300];
    int iRx, retval;
    
    // File directory
    sprintf(Design->outPath, "%s%ld_%d_%s_%ld/", OutPath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);

    // Make a large file storage directory if not exist
    sprintf(Design->largeFilePath, "%s%ld_%d_%s_%ld/", Design->largeFileRoot, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
    retval = mkdir(Design->largeFilePath, 0777);
    if(retval != 0 && errno != EEXIST)
        ERRNO(retval);       

    // Move the file to the designated directory for storage
    for(iRx=0; iRx<Design->nRx; iRx++){
        sprintf(oldFilename,"%s%ld_%d_%s_%ld_Rx%d.nc", Design->outPath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane, iRx+1);
        sprintf(newFilename,"%s%ld_%d_%s_%ld_Rx%d.nc", Design->largeFilePath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane, iRx+1);
        retval = rename(oldFilename, newFilename);
        if(retval != 0 && errno != EEXIST)
            ERRNO(retval);  
        
    }
}

void deleteSpDataNc(void)
{
    char fullpath[300];
    int iRx, retval;

    // File directory
    sprintf(Design->outPath, "%s%ld_%d_%s_%ld/", OutPath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);

    // Delete the file
    for(iRx=0; iRx<Design->nRx; iRx++){
        sprintf(fullpath,"%s%ld_%d_%s_%ld_Rx%d.nc", Design->outPath, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane, iRx+1);
        if((retval = remove(fullpath)))
            ERRNO(retval);
    }
}

void sortGriddata(void)
{
    int iRow, iCol, iSoOp, i, j, nSp, temp;

    for(iSoOp=0; iSoOp<Design->nSoOp; iSoOp++){
        for(iRow=0; iRow<Grid[0].nRow; iRow++){
            for(iCol=0; iCol<Design->nCol; iCol++){
                nSp = Design->griddataCnt[iSoOp][iRow][iCol];
                // Sort in ascending order    
                for(i=0; i<nSp; ++i){
                    for(j=i+1; j<nSp; ++j){
                        if (Design->griddata[iSoOp][iRow][iCol][i] > Design->griddata[iSoOp][iRow][iCol][j]) {
                            temp = Design->griddata[iSoOp][iRow][iCol][i];
                            Design->griddata[iSoOp][iRow][iCol][i] = Design->griddata[iSoOp][iRow][iCol][j];
                            Design->griddata[iSoOp][iRow][iCol][j] = temp;
                        }
                    }
                }
            }
        }
    }
}

void freeGriddata(void)
{
    int iRow, iCol, iSoOp;

    for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
        for(iRow=0; iRow<Grid[0].nRow; iRow++){
            for(iCol=0; iCol<Design->nCol; iCol++){
                free(Design->griddata[iSoOp][iRow][iCol]);
            }
            free(Design->griddata[iSoOp][iRow]);
            free(Design->griddataCnt[iSoOp][iRow]);
        }
        free(Design->griddata[iSoOp]);
        free(Design->griddataCnt[iSoOp]);
    }
    free(Design->griddata);
    free(Design->griddataCnt);
}

void update_design(int taskID)
{
    long iRx, iPlane, iSc, iSoOp, iRxPerPlane;
    long nRxPerPlane, phasing;
    double RAAN[Design->nPlane];
    char filename[100];
    int retval;

    // Update total number of satellites to propagate
    Design->nSCPrev = Nsc;
    Design->nRxPrev = NRx;
    NRx = Design->nRx;

    // Choose if propagate only receivers or receivers and transmitters
    if(Design->flagTxPropagation){
        Norb += Design->nRx - Design->nRxPrev;
        Nsc = Norb;
    }
    else{
        Norb = Design->nRx;
        Nsc = Design->nRx;
    }
    
    RxSc = (struct ScType *) realloc(RxSc,sizeof(struct ScType)*Design->nRx);
	if (RxSc == NULL) {
		printf("RxSc calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
    
    for(iSc=0; iSc<Design->nRx; iSc++){
        RxSc[iSc].ID = iSc;
    }

    Design->ID = (long *) realloc(Design->ID, sizeof(long)*Design->nRx);
    if (Design->ID == NULL) {
        printf("Design->ID calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->anom = (double *) realloc(Design->anom, sizeof(double)*Design->nRx);
    if (Design->anom == NULL) {
        printf("Design->anom calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Design->RAAN = (double *) realloc(Design->RAAN, sizeof(double)*Design->nRx);
    if (Design->RAAN == NULL) {
        printf("Design->RAAN calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    for(iPlane=0; iPlane<Design->nPlane; iPlane++){
        RAAN[iPlane] = (double) iPlane * (360 / Design->nPlane);
    }

    if(Design->consType == WALKER){
        // Walker Delta Constellation
        nRxPerPlane = Design->nRx / Design->nPlane; // T/P
        phasing = Design->nPlane - 1 - nRxPerPlane; // F = P - 1 - T/P
        while(!(phasing>0)) // If F<0, add P repeatly until F>=0, F<P, F=integer
            phasing += Design->nPlane;
        if(phasing >= Design->nPlane)
            phasing -= Design->nPlane;
        printf("Task %d >> Walker Delta Constellation @ %.1lf km, i:T/P/F = %.1lf:%ld/%ld/%ld\n", taskID, Design->alt, Design->inc, Design->nRx, Design->nPlane, phasing);
        
        for(iRx=0; iRx<Design->nRx; iRx++){
            Design->ID[iRx] = iRx;
            Design->anom[iRx] = (double) (iRx % nRxPerPlane) * (360 / nRxPerPlane) + (iRx / nRxPerPlane) * phasing*360/Design->nRx;
            Design->anom[iRx] = fmod(Design->anom[iRx], 360.0);
            Design->RAAN[iRx] = RAAN[iRx / nRxPerPlane];
            //printf("Orb %ld: TA=%.2lfdeg, RAAN=%.2lfdeg\n", iRx, Design->anom[iRx], Design->RAAN[iRx]);
        }
    }
    else if(Design->consType == HYBRID){
        printf("Task %d >> Hybrid Constellation - %s\n", taskID, Design->consName);
        for(iPlane=0; iPlane<Design->nPlane; iPlane++){
            printf("Task %d >>     Plane %ld @ RAAN %.1lf deg, %.1lf km, i:T = %s:%d\n", taskID, iPlane, RAAN[iPlane], Design->altPerPlane[iPlane], Design->incPerPlane_str[iPlane], Design->nRxPerPlane[iPlane]);
        }
        iRx = 0;
        for(iPlane=0; iPlane<Design->nPlane; iPlane++){
            for(iRxPerPlane=0; iRxPerPlane<Design->nRxPerPlane[iPlane]; iRxPerPlane++){
                Design->ID[iRx] = iRx;
                Design->anom[iRx] = (double) (iRxPerPlane % Design->nRxPerPlane[iPlane]) * (360 / Design->nRxPerPlane[iPlane]);
                Design->anom[iRx] = fmod(Design->anom[iRx], 360.0);
                Design->RAAN[iRx] = RAAN[iPlane];
                //printf("Orb %ld: TA=%.2lfdeg, RAAN=%.2lfdeg\n", iRx, Design->anom[iRx], Design->RAAN[iRx]);
                iRx++;
            }
        }
    }
    // Make a large file storage directory if not exist
    if(Design->consType == WALKER){
        sprintf(Design->largeFilePath, "%s%ld_%d_%s_%ld/", Design->largeFileRoot, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
    }
    else if(Design->consType == HYBRID){
        sprintf(Design->largeFilePath, "%s%s/", Design->largeFileRoot, Design->consName);
    }
    retval = mkdir(Design->largeFilePath, 0777);
    if(retval != 0 && errno != EEXIST){
        printf("Task %d >> mkdir fail: %s\n", taskID, Design->largeFilePath);
        ERRNO(retval);       
    }
    else{
        printf("Task %d >> Storage dir: %s\n", taskID, Design->largeFilePath);
    }

    // Update variables for individual receiver data
    if(Design->flagReportSatData){
        // Make an output directory if not exist
        if(Design->consType == WALKER){
            sprintf(Design->outPath, "%s%ld_%d_%s_%ld/", Design->largeFileRoot, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
        }
        else if(Design->consType == HYBRID){
            sprintf(Design->outPath, "%s%s/", Design->largeFileRoot, Design->consName);
        }
        retval = mkdir(Design->outPath, 0777);
        if(retval != 0 && errno != EEXIST){
            printf("Task %d >> mkdir fail: %s\n", taskID, Design->outPath);
            exit(ERRCODE);            
        }
        else{
            printf("Task %d >> Output dir: %s\n", taskID, Design->outPath);
        }
        // Memory allocation
        Design->outFileSp = (FILE ***)realloc(Design->outFileSp, sizeof(FILE **)*Design->nRx);
		if (Design->outFileSp == NULL) {
			printf("Design->outFileSp calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
        for(iRx=0; iRx<Design->nRx; iRx++){
			Design->outFileSp[iRx] = (FILE **)calloc(NSoOp, sizeof(FILE *));
			if (Design->outFileSp[iRx] == NULL) {
				printf("Design->outFileSp[iRx] calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
        	for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
				sprintf(filename,"%s%ld_%s.out", RxName, iRx+1, SoOp[iSoOp].TxName);
				Design->outFileSp[iRx][iSoOp] = FileOpen(Design->outPath,filename,"w");
            }
        }
    }
}

void errorMessageMPI(int error, int rank, char* string)
{
        fprintf(stderr, "Process %d: Error %d in %s\n", rank, error, string);
        MPI_Finalize();
        exit(-1);
}

void readTxOrbit(long SimComplete)
{
	static FILE ***inFile;
	static long First = TRUE;
	long iSoOp, iTx, iSc;
	char newline;
    double JulDayTx;

	//* .. Open Trajectory Files .. *//
	if (First) {
        char filename[100];
        First = FALSE;

		inFile = (FILE ***)calloc(NSoOp, sizeof(FILE **));
		if (inFile == NULL) {
			printf("inFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
        for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
			inFile[iSoOp] = (FILE **)calloc(SoOp[iSoOp].NTx, sizeof(FILE *));
			if (inFile[iSoOp] == NULL) {
				printf("inFile[iSoOp] calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
            for(iTx=0; iTx<SoOp[iSoOp].NTx; iTx++){
				sprintf(filename,"TxOrb/%s_%ld.out", SoOp[iSoOp].TxName, iTx);
				inFile[iSoOp][iTx] = FileOpen(OutPath,filename,"r");
            }
        }
    }

	if(SimComplete){
	    //* .. Rewind Files .. *//
        for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
            for(iTx=0; iTx<SoOp[iSoOp].NTx; iTx++){
                rewind(inFile[iSoOp][iTx]);
            }
        }
	}
    else{
        //* .. Update Position, Velocity, and CLN .. *//
        for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
            for(iTx=0; iTx<SoOp[iSoOp].NTx; iTx++){
                iSc = SoOp[iSoOp].RefOrbFirst + iTx;
                // Julian Day, Position of Receiver in ECI [m], Velocity of Receiver in ECI [m/s]
                fscanf(inFile[iSoOp][iTx], "%lf %lf %lf %lf %lf %lf %lf %[\n]", &JulDayTx, &TxSc[iSc].PosN[0], &TxSc[iSc].PosN[1], &TxSc[iSc].PosN[2],
                                                                                        &TxSc[iSc].VelN[0], &TxSc[iSc].VelN[1], &TxSc[iSc].VelN[2], &newline);
                newFindCLN(TxSc[iSc].PosN, TxSc[iSc].VelN, TxSc[iSc].CLN);
            }
        }
        /*long year, month, day, hour, min;
        double sec;
        JDToDate(JulDayTx, &year, &month, &day, &hour, &min, &sec);
        printf("%ld-%ld-%ld, %ld:%ld:%.1lf, UTC: %ld-%ld-%ld, %ld:%ld:%.1lf\n", year, month, day, hour, min, sec, UTC.Year,UTC.Month,UTC.Day,
        UTC.Hour,UTC.Minute,UTC.Second);*/
    }
}

void broadcastTxOrbit(void)
{
	long iSoOp, iTx, iSc;

    for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
        for(iTx=0; iTx<SoOp[iSoOp].NTx; iTx++){
            iSc = SoOp[iSoOp].RefOrbFirst + iTx;
            MPI_Bcast(TxSc[iSc].PosN, 3, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Bcast(TxSc[iSc].VelN, 3, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Bcast(TxSc[iSc].CLN, 9, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            //printf("Task %d: %lf SC %ld, %lf %lf %lf %lf %lf %lf\n", taskid, iSc, JulDayTx[0], Sc[iSc].PosN[0], Sc[iSc].PosN[1], Sc[iSc].PosN[2],
            //                                                Sc[iSc].VelN[0], Sc[iSc].VelN[1], Sc[iSc].VelN[2]);
        }
    }
}

void reportTxOrbit(long SimComplete)
{
    static uint8_t First = TRUE;
    static FILE ***outFile;
	long iSoOp, iTx, iSC;

	if (First) {
    	char filename[50];
        char outPathTxOrb[50];
        int check;

        First = FALSE;

        // Make an output directory if not exist
        sprintf(outPathTxOrb, "%sTxOrb/", OutPath);
        check = mkdir(outPathTxOrb, 0777);
        if(check != 0 && errno != EEXIST){
            printf("mkdir fail: %s\n", outPathTxOrb);
            exit(1);            
        }
        else{
            printf("Output dir for Tx orbits: %s\n", outPathTxOrb);
        }

		outFile = (FILE ***)calloc(NSoOp, sizeof(FILE **));
		if (outFile == NULL) {
			printf("outFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
        for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
			outFile[iSoOp] = (FILE **)calloc(SoOp[iSoOp].NTx, sizeof(FILE *));
			if (outFile[iSoOp] == NULL) {
				printf("outFile[iSoOp] calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
            for(iTx=0; iTx<SoOp[iSoOp].NTx; iTx++){
				sprintf(filename,"%s_%ld.out", SoOp[iSoOp].TxName, iTx);
				outFile[iSoOp][iTx] = FileOpen(outPathTxOrb,filename,"w");
            }
        }
    }

    for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
        for(iTx=0; iTx<SoOp[iSoOp].NTx; iTx++){
            iSC = SoOp[iSoOp].RefOrbFirst + iTx + Design->nRx;
            // Julian Day, Position of Receiver in ECI [m], Velocity of Receiver in ECI [m/s]
            fprintf(outFile[iSoOp][iTx], "%lf %lf %lf %lf %lf %lf %lf\n", UTC.JulDay, SC[iSC].PosN[0], SC[iSC].PosN[1], SC[iSC].PosN[2], SC[iSC].VelN[0], SC[iSC].VelN[1], SC[iSC].VelN[2]);
        }
    }
    if(SimComplete){
        First = TRUE;
        for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
            for(iTx=0; iTx<SoOp[iSoOp].NTx; iTx++){
                fclose(outFile[iSoOp][iTx]);
            }
        }
    }
}

void reportRxOrbit(long SimComplete)
{
    static uint8_t First = TRUE;
    static FILE **outFile;
	long iRx;
    double posW[3];
    double lat, lon, alt;

	if (First) {
    	char filename[50];
        First = FALSE;
		outFile = (FILE **)calloc(Design->nRx, sizeof(FILE *));
		if (outFile == NULL) {
			printf("outFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
        for(iRx=0; iRx<Design->nRx; iRx++){
            sprintf(filename,"Rx%ld.out", iRx+1);
            outFile[iRx] = FileOpen(Design->outPath,filename,"w");
        }
    }

    for(iRx=0; iRx<Design->nRx; iRx++){
        // ECI to ECEF of Receiver's Position and Velocity
        MxV(World[EARTH].CWN, SC[iRx].PosN, posW);
        
        // ECEF to GRS80
        ECEFToGRS80(posW, &lat, &lon, &alt);

        // Julian Day, LLA
        fprintf(outFile[iRx], "%lf %lf %lf %lf\n", UTC.JulDay, lat*R2D, lon*R2D, alt);    
        // Julian Day, Position of Receiver in ECI [m], Velocity of Receiver in ECI [m/s]
        //fprintf(outFile[iRx], "%lf %lf %lf %lf %lf %lf %lf\n", UTC.JulDay, SC[iRx].PosN[0], SC[iRx].PosN[1], SC[iRx].PosN[2], SC[iRx].VelN[0], SC[iRx].VelN[1], SC[iRx].VelN[2]);    
    }

    if(SimComplete){
        First = TRUE;
        for(iRx=0; iRx<Design->nRx; iRx++)
            fclose(outFile[iRx]);
    }
}

void reportSpeculaPoint(struct ScType *Rx, struct SoOpType *S, struct SpecularType *Sp, struct ScType *Tx)
{
	long iRx, iSoOp, iLayer;

	iRx = Rx->ID;
	iSoOp = S->ID;

	// Julian Day
	if(ReportSoOp->JulDay_Sp)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf ", UTC.JulDay);
	// Date
	if(ReportSoOp->Date)
		fprintf(Design->outFileSp[iRx][iSoOp], "%ld %ld %ld %ld %ld %.1lf ", UTC.Year, UTC.Month, UTC.Day, UTC.Hour, UTC.Minute, UTC.Second);
	// LLA of Receiver [deg][deg][m]
	if(ReportSoOp->LLA_Rx)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Rx->lat*R2D, Rx->lon*R2D, Rx->alt);		
	// Position of Receiver in ECEF [m]
	if(ReportSoOp->PosW_Rx)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Rx->PosW[0], Rx->PosW[1], Rx->PosW[2]);
	// Velocity of Receiver in ECEF [m/s]
	if(ReportSoOp->VelW_Rx)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Rx->VelW[0], Rx->VelW[1], Rx->VelW[2]);
	// Transmitter ID number
	if(ReportSoOp->TxID)
		fprintf(Design->outFileSp[iRx][iSoOp], "%ld ", Sp->TxID);
	// LLA of Transmitter [deg][deg][m]
	if(ReportSoOp->LLA_Tx)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Tx->lat*R2D, Tx->lon*R2D, Tx->alt);
	// Position of Transmitter in ECEF [m]
	if(ReportSoOp->PosW_Tx)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Tx->PosW[0], Tx->PosW[1], Tx->PosW[2]);
	// Velocity of Transmitter in ECEF [m/s]
	if(ReportSoOp->VelW_Tx)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Tx->VelW[0], Tx->VelW[1], Tx->VelW[2]);
	// LLA of Specular Point [deg][deg][m]
	if(ReportSoOp->LLA_Sp)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Sp->lat*R2D, Sp->lon*R2D, Sp->alt);
	// Position of Specular Point in ECEF [m]
	if(ReportSoOp->PosW_Sp)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Sp->PosW[0], Sp->PosW[1], Sp->PosW[2]);
	// Line of Sight of Direct Signal (Azimuth, Elevation) [deg][deg]
	if(ReportSoOp->LoS_D)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Sp->lookAngAzD*R2D, Sp->lookAngElD*R2D);
	// Line of Sight of Reflected Signal (Azimuth, Elevation) [deg][deg]
	if(ReportSoOp->LoS_R)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Sp->lookAngAzR*R2D, Sp->lookAngElR*R2D);
	// Delay and Doppler for Direct and Reflected Signal
	if(ReportSoOp->DelayDoppler){
		calcDelayDoppler(Rx->PosN, Rx->VelN, Sp, Tx);
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf %lf ", Out->delayD, Out->dopplerD, Out->delayR, Out->dopplerR);
	}
	// Antenna Gain of Direct signal [dB]
	if(ReportSoOp->Gain_D){
        //* .. Antenna Pattern .. * //
        if(S->AntTag == IDEAL){
            // Transmitter direction
            Bistatic->g_rt[0][0] = 1; Bistatic->g_rt[0][1] = 0;
            Bistatic->g_rt[1][0] = 0; Bistatic->g_rt[1][1] = 1;
        }
        else{
            // Transmitter direction
            LookupAntPattern(S->AntPattern, S->AntPatRes, Sp->lookAngElD, Sp->lookAngAzD, Bistatic->g_rt);
        }
        Sp->AntGainD = 10*log10(cabs(Bistatic->g_rt[0][0])*cabs(Bistatic->g_rt[0][0])) + S->G_Rx;
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf ", Sp->AntGainD);
    }
	// Antenna Gain of Reflected Signal [dB]
	if(ReportSoOp->Gain_R){
        //* .. Antenna Pattern .. * //
        if(S->AntTag == IDEAL){
            // Specular direction
            Bistatic->g_rs[0][0] = 1; Bistatic->g_rs[0][1] = 0;
            Bistatic->g_rs[1][0] = 0; Bistatic->g_rs[1][1] = 1;
        }
        else{
            // Specular direction
            LookupAntPattern(S->AntPattern, S->AntPatRes, Sp->lookAngElR, Sp->lookAngAzR, Bistatic->g_rs);
        }
        Sp->AntGainR = 10*log10(cabs(Bistatic->g_rs[0][1])*cabs(Bistatic->g_rs[0][1])) + S->G_Rx;
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf ", Sp->AntGainR);
    }
	// First Fresnel Zone (a,b,azimuth) [m][m][deg]
	if(ReportSoOp->FresZone){
		Sp->b = sqrt(4*Bistatic->Wavelength*Rx->alt*sin(Sp->elev)+Bistatic->Wavelength*Bistatic->Wavelength) / sin(Sp->elev)/2;
		Sp->a = Sp->b / sin(Sp->elev);
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Sp->a, Sp->b, Sp->az*R2D);
	}
	// Incidence Angle [deg]
	if(ReportSoOp->IncAng)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf ", Sp->incAng*R2D);
	// Elevation Angle [deg]
	if(ReportSoOp->elev)
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf ", Sp->elev*R2D);
	// Roll, Pitch, Yaw [deg][deg][deg]
	if(ReportSoOp->RPY_to_Sp){
		calcRPY(Sp);
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf ", Sp->roll*R2D, Sp->pitch*R2D, Sp->yaw*R2D);
	}
	// Land Mask [1:Land, 0:Water]
	if(ReportSoOp->LandMask)
		fprintf(Design->outFileSp[iRx][iSoOp], "%d ", Gnd->landMask);
	// Volumetric Soil Moisture [m^3/m^3]
	if(ReportSoOp->VSM)
		for(iLayer=0; iLayer<NLayer; iLayer++)
			fprintf(Design->outFileSp[iRx][iSoOp], "%f ", Gnd->VSM[iLayer]);
	// Soil Temperature [K]
	if(ReportSoOp->SoilTemp)
		fprintf(Design->outFileSp[iRx][iSoOp], "%f ", Gnd->soilTemp1);
	// Freeze/Thaw State
	if(ReportSoOp->FreezeThaw)
		fprintf(Design->outFileSp[iRx][iSoOp], "%d ", Gnd->freezeThaw);
	// Soil Texture [Sand][Clay][Bulk density]
	if(ReportSoOp->SoilTexture)
		fprintf(Design->outFileSp[iRx][iSoOp], "%f %f %f ", Gnd->sand, Gnd->clay, Gnd->bulk);
	// Snow Cover [%]
	if(ReportSoOp->SnowCover){
		fprintf(Design->outFileSp[iRx][iSoOp], "%d ", Gnd->snowCover);
	}
/*
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
*//*
	// Reflection Coefficient Co-pol
	if(ReportSoOp->RcCO){
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", creal(Out->b_coh1[0]), cimag(Out->b_coh1[0])); // XX or RR or RX
	}			
	// Reflection Coefficient X-pol
	if(ReportSoOp->RcX){
		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X')
			fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", creal(Out->b_coh2[1]), cimag(Out->b_coh2[1])); // YY
		else
			fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", creal(Out->b_coh1[1]), cimag(Out->b_coh1[1])); // RL or RY
	}
	// Reflectivity Co-pol
	if(ReportSoOp->GammaCO){
		fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Out->P_coh1[0], Out->P_coh1_err[0]); // XX or RR or RX
	}			
	// Reflectivity X-pol
	if(ReportSoOp->GammaX){
		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X')
			fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Out->P_coh2[1], Out->P_coh2_err[1]); // YY
		else
			fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Out->P_coh1[1], Out->P_coh1_err[1]); // RL or RY
	}
	// SNR of Direct signal Co,X-pol
	if(ReportSoOp->SNR_D){
		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X')
			fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Out->SNR_d1[0], Out->SNR_d2[1]); // XX, YY
		else
			fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Out->SNR_d1[0], Out->SNR_d1[1]); // RR, RL or RX, RY
	}
	// SNR of Reflected signal Co,X-pol
	if(ReportSoOp->SNR_R){
		if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X')
			fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Out->SNR_coh1[0], Out->SNR_coh2[1]); // XX, YY
		else
			fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Out->SNR_coh1[0], Out->SNR_coh1[1]); // RR, RL or RX, RY
	}
	// MUOS Spot Beam Number for Receiver and Specular Point
	if(ReportSoOp->SbNum){
		fprintf(Design->outFileSp[iRx][iSoOp], "%ld %ld ", Sp->SpotbeamID[0], Sp->SpotbeamID[1]);
	}

    //fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf ", Out->Gamma_Std_mod_fused_avg[0], Out->Gamma_Std_mod_fused_avg[1]);
    fprintf(Design->outFileSp[iRx][iSoOp], "%lf %lf %lf %lf %lf %lf ", Out->RCG_R, Out->RCG_D, Out->Gamma_eff, Bistatic->r_ts, Bistatic->r_sr, Bistatic->r_tr);
    */
	fprintf(Design->outFileSp[iRx][iSoOp], "\n");
}

long *FindVisibleTxDesignMultiThread(double RxPosN[3], long *TxNum, long *TxCnt)
{
	static long bufferTx[100];
	static double bufferAngLoS[100];
	double Rhat[3], RelPosN[3];
	long ISoOp, ITx, TxClosest[100];
	long tempTx, cntTx = 0, cnt, i, j, idx_min;
	double tempLoS, angLoS, angLoSmax = HalfPi, dist;
    double tanLineLength, tanLineAngle;

    CopyUnitV(RxPosN,Rhat);
    
    tanLineLengAng(RxPosN, &tanLineLength, &tanLineAngle);
    
    angLoSmax = tanLineAngle - 10*D2R;
    //angLoSmax = HalfPi;
    /* Aim at Tx closest to Rx's Zenith */
    for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
        cnt = 0;
        for(ITx=SoOp[ISoOp].RefOrbFirst; ITx<SoOp[ISoOp].RefOrbLast; ITx++) {
            RelPosN[0] = TxSc[ITx].PosN[0] - RxPosN[0];
            RelPosN[1] = TxSc[ITx].PosN[1] - RxPosN[1];
            RelPosN[2] = TxSc[ITx].PosN[2] - RxPosN[2];
            dist = UNITV(RelPosN);
            angLoS = acos(VoV(RelPosN,Rhat));
            if(angLoS < angLoSmax || dist < tanLineLength) {
                bufferTx[cnt] = ITx;
                bufferAngLoS[cnt++] = angLoS;
                //printf("%ld %lf %lf %lf %lf\n", ITx, angLoS*R2D, angLoSmax*R2D, dist/1e3, tanLineLength/1e3);
            }
        }
        // Sort ascending order
        for(i=0; i<cnt-1; i++) {
            idx_min = i;
            for (j=i+1; j<cnt; j++) {
                if (bufferAngLoS[j] < bufferAngLoS[idx_min]) {
                    idx_min = j;
                }
            }
            tempLoS = bufferAngLoS[idx_min];
            bufferAngLoS[idx_min] = bufferAngLoS[i];
            bufferAngLoS[i] = tempLoS;

            tempTx = bufferTx[idx_min];
            bufferTx[idx_min] = bufferTx[i];
            bufferTx[i] = tempTx;
        }
        if(cnt > SoOp[ISoOp].NSp){
            for(i=0; i<SoOp[ISoOp].NSp; i++)
                TxClosest[cntTx++] = bufferTx[i];
            printf(">> You're seeing much more specular points than ever! Tx: %s\n", SoOp[ISoOp].TxName);
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

long *FindVisibleTxDesign(struct ScType *Rx, long *TxNum, long *TxCnt)
{
	static long bufferTx[100];
	static double bufferAngLoS[100];
	double Rhat[3], RelPosN[3];
	long ISoOp, ITx, TxClosest[100];
	long tempTx, cntTx = 0, cnt, i, j, idx_min;
	double tempLoS, angLoS, angLoSmax = HalfPi;//, dist;
    //double tanLineLength, tanLineAngle;

    CopyUnitV(Rx->PosN,Rhat);
    
//    tanLineLengAng(Rx->PosN, &tanLineLength, &tanLineAngle);
    
//    angLoSmax = tanLineAngle - 15*D2R;
    //angLoSmax = HalfPi;
    /* Aim at Tx closest to Rx's Zenith */
    for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
        cnt = 0;
        for(ITx=SoOp[ISoOp].RefOrbFirst; ITx<SoOp[ISoOp].RefOrbLast; ITx++) {
            RelPosN[0] = TxSc[ITx].PosN[0] - Rx->PosN[0];
            RelPosN[1] = TxSc[ITx].PosN[1] - Rx->PosN[1];
            RelPosN[2] = TxSc[ITx].PosN[2] - Rx->PosN[2];
            UNITV(RelPosN);
            angLoS = acos(VoV(RelPosN,Rhat));
            if(angLoS < angLoSmax) { //  || dist < tanLineLength
                bufferTx[cnt] = ITx;
                bufferAngLoS[cnt++] = angLoS;
                //printf("%ld %lf %lf %lf %lf\n", ITx, angLoS*R2D, angLoSmax*R2D, dist/1e3, tanLineLength/1e3);
            }
        }
        // Sort ascending order
        for(i=0; i<cnt-1; i++) {
            idx_min = i;
            for (j=i+1; j<cnt; j++) {
                if (bufferAngLoS[j] < bufferAngLoS[idx_min]) {
                    idx_min = j;
                }
            }
            tempLoS = bufferAngLoS[idx_min];
            bufferAngLoS[idx_min] = bufferAngLoS[i];
            bufferAngLoS[i] = tempLoS;

            tempTx = bufferTx[idx_min];
            bufferTx[idx_min] = bufferTx[i];
            bufferTx[i] = tempTx;
        }
        if(cnt > SoOp[ISoOp].NSp){
            for(i=0; i<SoOp[ISoOp].NSp; i++)
                TxClosest[cntTx++] = bufferTx[i];
            printf(">> You're seeing much more specular points than ever! Tx: %s, Rx %ld\n", SoOp[ISoOp].TxName, Rx->ID);
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

void observation(void)
{
    static uint8_t First = TRUE;
    struct ScType *Rx, *Tx;
    struct SoOpType *S;
    struct SpecularType *Sp;
    long iRx, iTx, iSoOp;
    long *TxNum = NULL;
    uint8_t i, flagOnGrid = FALSE;

    static long *TxCntTotal;
    long SpCnt[MAXNUM_TX];

    long iGrid;
    struct EASEgridType *G;
    long iRow, iCol;
    
    double vRx2TxN[3], vRx2TxA[3], vRx2SpN[3], vRx2SpA[3];
    double vNadir[3] = {0.0, 0.0, 1.0};
    
    if (First) {
        TxCntTotal = (long *) calloc(1, sizeof(long));
        First = FALSE;
    }

    for(iTx=0; iTx<NTx; iTx++){
        TxSc[iTx].flagUpdated = FALSE;
    }

    for(iRx=0; iRx<NRx; iRx++){
    	Rx = &RxSc[iRx];        
    	// ECI to ECEF of Receiver's Position
    	MxV(World[EARTH].CWN, Rx->PosN, Rx->PosW);
       	// ECI to ECEF of Receiver's Velocity
    	MxV(World[EARTH].CWN, Rx->VelN, Rx->VelW);
    	// ECEF to GRS80
    	ECEFToGRS80(Rx->PosW, &Rx->lat, &Rx->lon, &Rx->alt);
			
		if(Rx->alt<100E3){
		//	EndFlag = TRUE;
			printf(">> Deorbit of Rx %ld. Rx[%ld].alt < 100km\n", iRx, iRx);
		}
		
		// .. Science Observation .. //
		// Select transmitters with the smallest off-boresight angle
	  	TxNum = FindVisibleTxDesign(Rx, TxNum, TxCntTotal);

    	for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
    		SpCnt[iSoOp] = 0;
		}
				
		if(*TxCntTotal>0){
			for(iTx=0; iTx<*TxCntTotal; iTx++){
				for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
					S = &SoOp[iSoOp];
					if(TxNum[iTx] > S->RefOrbFirst-1 && TxNum[iTx] < S->RefOrbLast){
						Sp = &S->Sp[SpCnt[iSoOp]];
						Sp->SpNum = SpCnt[iSoOp]++;
						Sp->TxID = TxNum[iTx];
						break;
					}
				}

				Tx = &TxSc[Sp->TxID];
				
                if(!Tx->flagUpdated){
					Tx->flagUpdated = TRUE;
                    // ECI to ECEF of Transmitter's Position and Velocity
                    MxV(World[EARTH].CWN, Tx->PosN, Tx->PosW);
                    MxV(World[EARTH].CWN, Tx->VelN, Tx->VelW);
                    
                    // ECEF to GRS80
                    ECEFToGRS80(Tx->PosW, &Tx->lat, &Tx->lon, &Tx->alt);
                    					
                    // Update MUOS spotbeam
					if(!strcmp(S->TxName,"MUOS")){
					    TrackingSpotbeam(Tx, &SpotBeam[Tx->ID - S->RefOrbFirst]);
					}
                }
				//* .. Determine Specular Reflection Point on GRS80 in ECEF coordinate system .. *//
				GainAdjustUD(Tx, Sp);
				FindSP_UD(Rx->PosW, Tx->PosW, Sp);
				
				// ECEF to GRS80
				ECEFToGRS80(Sp->PosW, &Sp->lat, &Sp->lon, &Sp->alt);

                // .. Discretize specular point onto grids .. //
				if(ExistGrid){
					for(iGrid=0; iGrid<NGrid; iGrid++){
						G = &Grid[iGrid];
                        // Mapping indices
                        ease2_LL2RC(G, Sp->lat, Sp->lon, &iRow, &iCol);
                        if(iRow < G->nRow && iCol < G->nCol && iRow >= 0 && Design->landmask[iRow][iCol]){
                            flagOnGrid = TRUE;
				            break;
                        }
					}
				}
    
                // .. Output for Valid Arc .. //
                if(flagOnGrid){
                    flagOnGrid = FALSE;

                    // .. Update Geometry .. //
                    calcGeometryInorbit(Rx->PosW, Tx->PosW, Sp);

                    // ECEF to ECI
                    MTxV(World[EARTH].CWN, Sp->PosW, Sp->PosN);

                    for(i=0;i<3;i++) {
                        vRx2TxN[i] = Tx->PosN[i] - Rx->PosN[i]; // for Sky-view antenna look angle & slant range
                        vRx2SpN[i] = Sp->PosN[i] - Rx->PosN[i]; // for Earth-view antenna look angle
                    }
                    UNITV(vRx2SpN);
                    UNITV(vRx2TxN);

                    // Earth-view (Reflected signal) antenna look angle
                    MxV(Rx->CLN, vRx2SpN, vRx2SpA); // opposite direction vector of reflected signal in Antenna body frame

                    Sp->lookAngElR = acos(VoV(vRx2SpA,vNadir));
                    Sp->lookAngAzR = atan2TwoPi(vRx2SpA[1],vRx2SpA[0]);

                    // Sky-view (Direct signal) antenna look angle
                    MxV(Rx->CLN, vRx2TxN, vRx2TxA); // opposite direction vector of direct signal in Antenna body frame

                    Sp->lookAngElD = acos(VoV(vRx2TxA,vNadir));
                    Sp->lookAngAzD = atan2TwoPi(vRx2TxA[1],vRx2TxA[0]);

                    // .. Update Spotbeam Footprint .. //
                    if(!strcmp(SoOp[iSoOp].TxName,"MUOS")){
                        Design->flagSpotbeam = checkMUOSspotbeam(Rx, Sp, &SpotBeam[Tx->ID - S->RefOrbFirst]);
                        writeFlagSpotbeam(iRx, iSoOp);
                    }

                    // Calculate figures of merit   
                    writeSpDataNc(iRx, iSoOp, Sp);

                    // .. Report Individual Receiver Data .. //
                    if(Design->flagReportSatData && OutFlag){
                        reportSpeculaPoint(Rx, S, Sp, Tx);
                    } 
                }
			}
			for(iSoOp=0; iSoOp<NSoOp; iSoOp++)
				S[iSoOp].SpCnt = SpCnt[iSoOp];
                
		}
        free(TxNum);
	}
}

void observationMultiThread(void)
{
    omp_set_num_threads(1);
    #pragma omp parallel default(none) firstprivate(RxSc, TxSc, SpotBeam) shared(SoOp, NTx, World, NSoOp, ExistGrid, Grid, Design, NGrid, OutFlag, SimTime, R2D)
    {
        struct ScType *Rx, *Tx;
        struct SoOpType *S;
        struct SpecularType *Sp;
        long iRx, iTx, iSoOp;
        long *TxNum = NULL;
        uint8_t i, flagOnGrid = FALSE, flagValidData = FALSE;

        long *TxCntTotal;
        long SpCnt[MAXNUM_TX];

        long iGrid;
        struct EASEgridType *G;
        long iRow, iCol;
        
        double vRx2TxN[3], vRx2TxA[3], vRx2SpN[3], vRx2SpA[3];
        double vNadir[3] = {0.0, 0.0, 1.0};
        
        TxCntTotal = (long *) calloc(1, sizeof(long));
        Sp = (struct SpecularType *) calloc(1,sizeof(struct SpecularType));

        for(iTx=0; iTx<NTx; iTx++){
            TxSc[iTx].flagUpdated = FALSE;
        }

        iRx = omp_get_thread_num();
    	Rx = &RxSc[iRx];
        
        double RxPosN[3], RxPosW[3], RxLat, RxLon, RxAlt;
        double TxPosN[3], TxPosW[3], TxLat, TxLon, TxAlt;

        copyV3(RxPosN, Rx->PosN);

    	// ECI to ECEF of Receiver's Position
    	MxV(World[EARTH].CWN, RxPosN, RxPosW);
    	// ECEF to GRS80
    	ECEFToGRS80(RxPosW, &RxLat, &RxLon, &RxAlt);
			
		if(RxAlt<100E3){
		//	EndFlag = TRUE;
			printf(">> Deorbit of Rx %ld. Rx[%ld].alt < 100km\n", iRx, iRx);
		}
		
		// .. Science Observation .. //
		// Select transmitters with the smallest off-boresight angle
	  	TxNum = FindVisibleTxDesignMultiThread(RxPosN, TxNum, TxCntTotal);

    	for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
    		SpCnt[iSoOp] = 0;
		}
        if(iRx==0){
            for(iTx=0; iTx<*TxCntTotal; iTx++){
                printf("%ld\n", TxNum[iTx]);
            }
        }
		if(*TxCntTotal>0){
			for(iTx=0; iTx<*TxCntTotal; iTx++){
				for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
					S = &SoOp[iSoOp];
					if(TxNum[iTx] > S->RefOrbFirst-1 && TxNum[iTx] < S->RefOrbLast){
						//Sp = &S->Sp[SpCnt[iSoOp]];
						Sp->SpNum = SpCnt[iSoOp]++;
						Sp->TxID = TxNum[iTx];
						break;
					}
				}

				Tx = &TxSc[Sp->TxID];
				copyV3(TxPosN, Tx->PosN);
                // ECI to ECEF of Transmitter's Position and Velocity
                MxV(World[EARTH].CWN, TxPosN, TxPosW);
                // ECEF to GRS80
                ECEFToGRS80(TxPosW, &TxLat, &TxLon, &TxAlt);
                                    
                // Update MUOS spotbeam
                if(!strcmp(S->TxName,"MUOS")){
                    TrackingSpotbeam(Tx, &SpotBeam[Tx->ID - S->RefOrbFirst]);
                }
                
				//* .. Determine Specular Reflection Point on GRS80 in ECEF coordinate system .. *//
                if(iRx == 0)
                    printf("%lf 1 Thread %ld, Rx = %lf %lf %lf, Tx %ld = %lf %lf %lf, Sp = %lf %lf %lf\n", SimTime, iRx, RxLat*R2D, RxLon*R2D, RxAlt/1e3, Sp->TxID, TxLat*R2D, TxLon*R2D, TxAlt/1e3, Sp->lat*R2D, Sp->lon*R2D, Sp->alt/1e3);
				GainAdjustUD(Tx, Sp);
				FindSP_UD(RxPosW, TxPosW, Sp);
				
				// ECEF to GRS80
				ECEFToGRS80(Sp->PosW, &Sp->lat, &Sp->lon, &Sp->alt);
                if(iRx == 0)
                    printf("%lf 2 Thread %ld, Rx = %lf %lf %lf, Tx %ld = %lf %lf %lf, Sp = %lf %lf %lf\n", SimTime, iRx, RxLat*R2D, RxLon*R2D, RxAlt/1e3, Sp->TxID, TxLat*R2D, TxLon*R2D, TxAlt/1e3, Sp->lat*R2D, Sp->lon*R2D, Sp->alt/1e3);
				
                // .. Discretize specular point onto grids .. //
				if(ExistGrid){
					for(iGrid=0; iGrid<NGrid; iGrid++){
						G = &Grid[iGrid];
                        // Mapping indices
                        ease2_LL2RC(G, Sp->lat, Sp->lon, &iRow, &iCol);
                        if(iRow < G->nRow && iCol < G->nCol && iRow >= 0 && Design->landmask[iRow][iCol]){
                            flagOnGrid = TRUE;
				            break;
                        }
					}
				}
    
                // .. Output for Valid Arc .. //
                if(flagOnGrid){
                    flagOnGrid = FALSE;
                    flagValidData = TRUE;

                    // .. Update Geometry .. //
                    calcGeometryInorbit(Rx->PosW, Tx->PosW, Sp);

                    // ECEF to ECI
                    MTxV(World[EARTH].CWN, Sp->PosW, Sp->PosN);

                    for(i=0;i<3;i++) {
                        vRx2TxN[i] = Tx->PosN[i] - Rx->PosN[i]; // for Sky-view antenna look angle & slant range
                        vRx2SpN[i] = Sp->PosN[i] - Rx->PosN[i]; // for Earth-view antenna look angle
                    }
                    UNITV(vRx2SpN);
                    UNITV(vRx2TxN);

                    // Earth-view (Reflected signal) antenna look angle
                    MxV(Rx->CLN, vRx2SpN, vRx2SpA); // opposite direction vector of reflected signal in Antenna body frame

                    Sp->lookAngElR = acos(VoV(vRx2SpA,vNadir));
                    Sp->lookAngAzR = atan2TwoPi(vRx2SpA[1],vRx2SpA[0]);

                    // Sky-view (Direct signal) antenna look angle
                    MxV(Rx->CLN, vRx2TxN, vRx2TxA); // opposite direction vector of direct signal in Antenna body frame

                    Sp->lookAngElD = acos(VoV(vRx2TxA,vNadir));
                    Sp->lookAngAzD = atan2TwoPi(vRx2TxA[1],vRx2TxA[0]);

                    // .. Update Spotbeam Footprint .. //
                    if(!strcmp(SoOp[iSoOp].TxName,"MUOS")){
                        flagValidData = checkMUOSspotbeam(Rx, Sp, &SpotBeam[Tx->ID - S->RefOrbFirst]);
                    }

                    // Calculate figures of merit   
                    if(flagValidData){
                        flagValidData = FALSE;
                        writeSpDataNc(iRx, iSoOp, Sp);

                        // .. Report Individual Receiver Data .. //
                        if(Design->flagReportSatData && OutFlag){
                            reportSpeculaPoint(Rx, S, Sp, Tx);
                        } 
                    }           
                }
			}
			//for(iSoOp=0; iSoOp<NSoOp; iSoOp++)
			//	S[iSoOp].SpCnt = SpCnt[iSoOp];
                
		}
        free(TxNum);
        free(Sp);
        free(TxCntTotal);
	}
}

void orbitMotion(void)
{
    struct OrbitType *O;
    struct SCType *S;
    long Iorb, i, j;

    // Receiver propagation
    for(Iorb=0; Iorb<NRx; Iorb++){
        S = &SC[Iorb];
        O = &Orb[Iorb];
        // Receiver orbit rectification
        /*for(i=0;i<3;i++) {
            O->PosN[i] += S->PosR[i];
            O->VelN[i] += S->VelR[i];
        }
        RV2Eph(DynTime,O->mu,O->PosN,O->VelN,
                &O->SMA,&O->ecc,&O->inc,
                &O->RAAN,&O->ArgP,&O->anom,
                &O->tp,&O->SLR,&O->alpha,&O->rmin,
                &O->MeanMotion,&O->Period);
        for(i=0;i<3;i++) {
            S->PosR[i] = 0;
            S->VelR[i] = 0;
        }*/
        if (O->J2DriftEnabled) MeanEph2RV(O,DynTime);
        else {
            Eph2RV(O->mu,O->SLR,O->ecc,
                    O->inc,O->RAAN,O->ArgP,
                    DynTime-O->tp,
                    O->PosN,O->VelN,&O->anom);
        }
        FindCLN(O->PosN,O->VelN,O->CLN,O->wln);

        // Update Position, Velocity, and CLN
        for(i=0; i<3; i++){
            RxSc[Iorb].PosN[i] = S->PosN[i];
            RxSc[Iorb].VelN[i] = S->VelN[i];
            for(j=0; j<3; j++){
                RxSc[Iorb].CLN[i][j] = O->CLN[i][j];
            }
        }
    }
    
    // Transmitter Propagation
    if(Design->flagTxPropagation){
        for(Iorb=NRx; Iorb<Norb; Iorb++){
            S = &SC[Iorb];
            O = &Orb[Iorb];
            // Propagation
            if (O->J2DriftEnabled) MeanEph2RV(O,DynTime);
            else {
                Eph2RV(O->mu,O->SLR,O->ecc,
                        O->inc,O->RAAN,O->ArgP,
                        DynTime-O->tp,
                        O->PosN,O->VelN,&O->anom);
            }
            /* Update CLN */
            FindCLN(O->PosN,O->VelN,O->CLN,O->wln);

            // Update Position, Velocity, and CLN
            for(i=0; i<3; i++){
                TxSc[Iorb-NRx].PosN[i] = S->PosN[i];
                TxSc[Iorb-NRx].VelN[i] = S->VelN[i];
                for(j=0; j<3; j++){
                    TxSc[Iorb-NRx].CLN[i][j] = O->CLN[i][j];
                }
            }
        }
    }
}

void orbitPropagator(void)
{
    struct SCType *S;
    struct CmdType *Cmd;
    long Isc, i, iRx, iSoOp;
    long SimComplete = FALSE;
    double TotalRunTime, Ang[3] = {90.0, 0.0, 0.0}, C[3][3];

    static int taskid = MASTER;

    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

    // .. Initialization .. //
    long iout = 1000000;
    long ProgressPercent = 0;
    long ProgressCtr = 0;
    double ProgressTime = 0.0;

    orbitMotion();
    // First call just initializes timer 
    RealRunTime(&TotalRunTime,DTSIM);

    Ephemerides(); // Sun, Moon, Planets, Spacecraft, Useful Auxiliary Frames 

    ZeroFrcTrq();

    for(Isc=0;Isc<Nsc;Isc++) {
        S = &SC[Isc];
        Cmd = &SC[Isc].AC.Cmd;
        Cmd->Parm = PARM_EULER_ANGLES;
        Cmd->Frame = FRAME_L;
        for(i=0;i<3;i++) Cmd->Ang[i] = Ang[i]*D2R;
        Cmd->RotSeq = 321;
        A2C(Cmd->RotSeq,Ang[0]*D2R,Ang[1]*D2R,Ang[2]*D2R,C);
        C2Q(C,Cmd->qrl);

        Environment(S);    // Magnetic Field, Atmospheric Density 
        Perturbations(S);  // Environmental Forces and Torques 
        Sensors(S);
        FlightSoftWare(S);
        Actuators(S);
        PartitionForces(S); // Orbit-affecting and "internal" 
    }

    // .. Orbit propagation and Specular point determination .. //
    while(!SimComplete){
        if(SimTime >= ProgressTime){
            ProgressCtr++;
            ProgressTime = (double) (ProgressCtr*5)/100.0*STOPTIME;
            printf("Task %d >>    42 Case %s is %3.1li%% Complete at Time = %12.3f\n", taskid, InOutPath, ProgressPercent, SimTime);
            ProgressPercent += 5;
            reopenSpDataNc();
        }
        manageOutFlag(iout);

        for(Isc=0;Isc<Nsc;Isc++) {
            Dynamics(&SC[Isc]);
        }

        SimComplete = AdvanceTime();

        orbitMotion();
        
        // Transmitter Propagation
        if(Design->flagTxPropagation){
            if(taskid == MASTER)
                reportTxOrbit(SimComplete); // Uncomment this to generate Tx trajectory files
        }
        else{
            readTxOrbit(SimComplete);
            //if(taskid == MASTER) // Read transmitter data
                //broadcastTxOrbit();
        }
        //reportRxOrbit(SimComplete);  // File Output 
        
        Ephemerides(); // Sun, Moon, Planets, Spacecraft, Useful Auxiliary Frames 
        ZeroFrcTrq();

        // SoOp-R observation
        observation();
        //observationMultiThread();
        /*
        for(Isc=0;Isc<Design->nRx;Isc++) {
            S = &SC[Isc];
            if (S->Exists) {
                Environment(S);    // Magnetic Field, Atmospheric Density
                Perturbations(S);  // Environmental Forces and Torques
                Sensors(S);
                FlightSoftWare(S);
                Actuators(S);
                PartitionForces(S); // Orbit-affecting and "internal"
            }
        }
        */
        //if(SimTime > ProgressTime/5){
        //    printf("Task %d >> Reopen NetCDF at Time = %12.3lf\n", taskid, SimTime);
        //}
        //syncSpDataNc();
    }

    if(SimComplete){
        if(!Design->flagTxPropagation)
            readTxOrbit(SimComplete);

        closeSpDataNc();    
        if(Design->flagReportSatData){
            for(iRx=0; iRx<NRx; iRx++){
                for(iSoOp=0; iSoOp<NSoOp; iSoOp++)
                    fclose(Design->outFileSp[iRx][iSoOp]);
            }
        }

        if(TimeMode == FAST_TIME){
            RealRunTime(&TotalRunTime,DTSIM);
            printf("Task %d >>     Total Run Time = %9.2lf sec\n", taskid, TotalRunTime);
            printf("Task %d >>     Sim Speed = %8.2lf x Real\n", taskid, STOPTIME/TotalRunTime);
        }
    }
}

void consPropagator(int taskID)
{
    // Update a structure variable "design" and "Sc" with new configuration
    update_design(taskID);
    reallocDesign();
    printf("Task %d >> Creating specular data files...\n", taskID);
    createSpDataNc();
    printf("Task %d >> Success!\n", taskID);
    // Initialize orbits and spacecrafts for 42 propagator
    printf("Task %d >> Initializing time, orbits, and spacecrafts for the 42 propagator...\n", taskID);
    resetTime();
    initOrbit();
    initSC();
    printf("Task %d >> Success!\n", taskID);
    printf("Task %d >> Propagating orbits...\n", taskID);
    orbitPropagator();
    printf("Task %d >> Success!\n", taskID);
}

void costAnalyzer(int taskID)
{
    double TotalRunTime;
    // First call just initializes timer 
    RealRunTime(&TotalRunTime,DTSIM);
    if(Design->consType == WALKER){
        printf("Task %d >> Start analyzing Walker constellation %ld_%d_%s_%ld\n", taskID, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
    }
    else if(Design->consType == HYBRID){
        printf("Task %d >> Start analyzing Hybrid constellation - %s\n", taskID, Design->consName);
    }
    printf("Task %d >> Creating Cost NetCDF file...\n", taskID);
    createCostNc();
    printf("Task %d >> Success!\n", taskID);
    printf("Task %d >> Calculating cost...\n", taskID);
    writeCostNc();
    printf("Task %d >> Success!\n", taskID);
    printf("Task %d >> Closing the file...\n", taskID);
    closeCostNc();
    printf("Task %d >> Success!\n", taskID);
    RealRunTime(&TotalRunTime,DTSIM);
    printf("     Total Run Time = %9.2lf sec\n", TotalRunTime);
}

void performAnalyzer(int taskID)
{
    double TotalRunTime;
    int i;
    // First call just initializes timer 
    RealRunTime(&TotalRunTime,DTSIM);
    if(Design->consType == WALKER){
        printf("Task %d >> Start analyzing Walker constellation %ld_%d_%s_%ld\n", taskID, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
    }
    else if(Design->consType == HYBRID){
        printf("Task %d >> Start analyzing Hybrid constellation - %s\n", taskID, Design->consName);
    }
    printf("Task %d >> Creating Performance NetCDF file...\n", taskID);
    resetPerformVars();
    createPerformNc();
    printf("Task %d >> Success!\n", taskID);
    for(i=0; i<Design->gridDivisor; i++){
        printf("Task %d >> (%d/%d) Importing specular points data into a grid...\n", taskID, i+1, Design->gridDivisor);
        readSpDataNc(i);
        printf("Task %d >> (%d/%d) Success!\n", taskID, i+1, Design->gridDivisor);
        printf("Task %d >> (%d/%d) Sorting data in each cell...\n", taskID, i+1, Design->gridDivisor);
        sortGriddata();
        printf("Task %d >> (%d/%d) Success!\n", taskID, i+1, Design->gridDivisor);
        printf("Task %d >> (%d/%d) Calculating revisit time...\n", taskID, i+1, Design->gridDivisor);
        writeRevisitTimeNc();
        printf("Task %d >> (%d/%d) Success!\n", taskID, i+1, Design->gridDivisor);
        printf("Task %d >> (%d/%d) Writing access times...\n", taskID, i+1, Design->gridDivisor);
        writeAccessTimeNc();
        printf("Task %d >> (%d/%d) Success!\n", taskID, i+1, Design->gridDivisor);
        printf("Task %d >> (%d/%d) Writing observable frequency combinations...\n", taskID, i+1, Design->gridDivisor);
        writeObsFreqNc(i);
        printf("Task %d >> (%d/%d) Success!\n", taskID, i+1, Design->gridDivisor);
    }
    printf("Task %d >> Writing the number of valid/invalid points...\n", taskID);        
    writeNspNc();
    printf("Task %d >> Success!\n", taskID);
    printf("Task %d >> Closing the file...\n", taskID);
    closePerformNc();
    printf("Task %d >> Success!\n", taskID);
    //freeGriddata();   
    //printf("Task %d >> Store the specular points data files... ", taskID); fflush(stdout);
    //storeSpDataNc();
    //deleteSpDataNc();
    //printf("Success!\n");
    RealRunTime(&TotalRunTime,DTSIM);
    printf("     Total Run Time = %9.2lf sec\n", TotalRunTime);
}

void runHybrid(int flagProc)
{
	// Variables for MPI
    static int taskid = MASTER;
    static int numtasks;

    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

    long flagExist, iPlane;

    sprintf(Design->largeFilePath, "%s%s/", Design->largeFileRoot, Design->consName);                    
    if(flagProc == DESIGN_ALL || flagProc == DESIGN_PROP){
        if(checkSpDataNc()){
            printf("Task %d >> Already exist %s. Skip it!\n", taskid, Design->largeFilePath);
            flagExist = TRUE;
        }
        else{
            printf("Task %d >> Including %s...\n", taskid, Design->largeFilePath);
            flagExist = FALSE;
        }
    }
    else if(flagProc == DESIGN_FOM){
        if(checkPerformNc() && checkCostNc()){
            printf("Task %d >> Already exist %s. Skip it!\n", taskid, Design->largeFilePath);
            flagExist = TRUE;
        }
        else{
            printf("Task %d >> Including %s...\n", taskid, Design->largeFilePath);
            flagExist = FALSE;
        }
    }  
    else if(flagProc == DESIGN_COST){
        if(checkCostNc()){
            printf("Task %d >> Already exist %s. Skip it!\n", taskid, Design->largeFilePath);
            flagExist = TRUE;
        }
        else{
            printf("Task %d >> Including %s...\n", taskid, Design->largeFilePath);
            flagExist = FALSE;
        }
    }   
    else if(flagProc == DESIGN_PERF){
        if(checkPerformNc()){
            printf("Task %d >> Already exist %s. Skip it!\n", taskid, Design->largeFilePath);
            flagExist = TRUE;
        }
        else{
            printf("Task %d >> Including %s...\n", taskid, Design->largeFilePath);
            flagExist = FALSE;
        }
    }

    // Step 2: All - Orbit propagation, Figures of merit calculation
    if(!flagExist){
        printf("Task %d >> Start simulation\n", taskid);
        Design->nRx = 0;
        for(iPlane=0; iPlane<Design->nPlane; iPlane++){
            Design->nRx += (long) Design->nRxPerPlane[iPlane];
        }
        switch(flagProc){
            case DESIGN_ALL:
                consPropagator(taskid);
                costAnalyzer(taskid);
                performAnalyzer(taskid);
                break;
            case DESIGN_PROP:
                consPropagator(taskid);
                break;
            case DESIGN_FOM:
                costAnalyzer(taskid);
                performAnalyzer(taskid);
                break;
            case DESIGN_COST:
                costAnalyzer(taskid);
                break;
            case DESIGN_PERF:
                performAnalyzer(taskid);
                break;
        }
    }
    printf("Task %d >> Simulation finished.\n", taskid);
}

void runWalker(int flagProc)
{
	// Variables for MPI
    static int taskid = MASTER;
    static int numtasks;

    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

    long nRx = Design->nCandi_nRx;
    long nAlt = Design->nCandi_alt;
    long nInc = Design->nCandi_inc;
    long nPlane = Design->nCandi_nPlane;
    long nVar = 4;
    long iRx, iAlt, iInc, iPlane, iComb = 0, iCycle;
    long flagExist[nRx][nAlt][nInc][nPlane];

    if(taskid == MASTER){
        // MASTER: Quick scan to drop out existing data and broadcast result
        for(iRx=0; iRx<nRx; iRx++){
            for(iPlane=0; iPlane<nPlane; iPlane++){
                for(iAlt=0; iAlt<nAlt; iAlt++){
                    for(iInc=0; iInc<nInc; iInc++){
                        Design->nRx = (long) Design->candi_nRx[iRx];
                        Design->alt = Design->candi_alt[iAlt];
                        strcpy(Design->inc_str, Design->candi_inc_str[iInc]); 
                        Design->nPlane = Design->candi_nPlane[iPlane];
                        sprintf(Design->largeFilePath, "%s%ld_%d_%s_%ld/", Design->largeFileRoot, Design->nRx, (int) Design->alt, Design->inc_str, Design->nPlane);
                        if(flagProc == DESIGN_ALL || flagProc == DESIGN_PROP){
                            if(checkSpDataNc()){
                                printf("Task %d >> Already exist %s. Skip it!\n", taskid, Design->largeFilePath);
                                flagExist[iRx][iAlt][iInc][iPlane] = TRUE;
                            }
                            else{
                                printf("Task %d >> Including %s...\n", taskid, Design->largeFilePath);
                                flagExist[iRx][iAlt][iInc][iPlane] = FALSE;
                                iComb++;
                            }
                        }
                        else if(flagProc == DESIGN_FOM){
                            if(checkPerformNc() && checkCostNc()){
                                printf("Task %d >> Already exist %s. Skip it!\n", taskid, Design->largeFilePath);
                                flagExist[iRx][iAlt][iInc][iPlane] = TRUE;
                            }
                            else{
                                printf("Task %d >> Including %s...\n", taskid, Design->largeFilePath);
                                flagExist[iRx][iAlt][iInc][iPlane] = FALSE;
                                iComb++;
                            }
                        }  
                        else if(flagProc == DESIGN_COST){
                            if(checkCostNc()){
                                printf("Task %d >> Already exist %s. Skip it!\n", taskid, Design->largeFilePath);
                                flagExist[iRx][iAlt][iInc][iPlane] = TRUE;
                            }
                            else{
                                printf("Task %d >> Including %s...\n", taskid, Design->largeFilePath);
                                flagExist[iRx][iAlt][iInc][iPlane] = FALSE;
                                iComb++;
                            }
                        }   
                        else if(flagProc == DESIGN_PERF){
                            if(checkPerformNc()){
                                printf("Task %d >> Already exist %s. Skip it!\n", taskid, Design->largeFilePath);
                                flagExist[iRx][iAlt][iInc][iPlane] = TRUE;
                            }
                            else{
                                printf("Task %d >> Including %s...\n", taskid, Design->largeFilePath);
                                flagExist[iRx][iAlt][iInc][iPlane] = FALSE;
                                iComb++;
                            }
                        }                        
                    }
                }
            }
        }
    }
    MPI_Bcast(&(flagExist[0][0][0][0]), nRx*nAlt*nInc*nPlane, MPI_LONG, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&iComb, 1, MPI_LONG, MASTER, MPI_COMM_WORLD);

    long nComb = iComb;
    long nCycle = floor(nComb / numtasks);
    long nRemainder = nComb % numtasks;
    double combinations[nComb][nVar];
    char combinations_inc_str[nComb][10];

    // Assign alternatives
    iComb = 0;
    for(iRx=0; iRx<nRx; iRx++){
        for(iPlane=0; iPlane<nPlane; iPlane++){
            for(iAlt=0; iAlt<nAlt; iAlt++){
                for(iInc=0; iInc<nInc; iInc++){
                    if(flagExist[iRx][iAlt][iInc][iPlane]){
                        // Do nothing
                    }
                    else{
                        if(strcmp(Design->candi_inc_str[iInc],"SSO")) // If not SSO
                            Design->inc = atof(Design->candi_inc_str[iInc]);
                        else{ // If SSO
                            Design->inc = calcSSOinc(Design->candi_alt[iAlt]);    
                        }
                        strcpy(combinations_inc_str[iComb], Design->candi_inc_str[iInc]);
                        combinations[iComb][0] = Design->candi_nRx[iRx];
                        combinations[iComb][1] = Design->candi_alt[iAlt];
                        combinations[iComb][2] = Design->inc;
                        combinations[iComb][3] = Design->candi_nPlane[iPlane];
                        iComb++;
                    }
                }
            }
        }
    }

    if(taskid == MASTER){
        printf("np = %d, nComb = %ld, nCycle = %ld, nRemainder = %ld, nSoOp= %ld\n", numtasks, nComb, nCycle, nRemainder, NSoOp);
    }

    // Step 2: All - Orbit propagation, Figures of merit calculation
    printf("Task %d >> Start simulation\n", taskid);
    for(iCycle=0; iCycle<nCycle; iCycle++){
        iComb = iCycle + taskid * nCycle;
        Design->nRx = (long) combinations[iComb][0];
        Design->alt = combinations[iComb][1];
        Design->inc = combinations[iComb][2];
        strcpy(Design->inc_str, combinations_inc_str[iComb]);
        Design->nPlane = combinations[iComb][3];
        if(iCycle>0)
            Design->flagTxPropagation = FALSE;
        
        switch(flagProc){
            case DESIGN_ALL:
                consPropagator(taskid);
                costAnalyzer(taskid);
                performAnalyzer(taskid);
                break;
            case DESIGN_PROP:
                consPropagator(taskid);
                break;
            case DESIGN_FOM:
                costAnalyzer(taskid);
                performAnalyzer(taskid);
                break;
            case DESIGN_COST:
                costAnalyzer(taskid);
                break;
            case DESIGN_PERF:
                performAnalyzer(taskid);
                break;
        }
        printf("Task %d >> Cycle %ld/%ld complete\n", taskid, iCycle+1, nCycle);
    }

    if(taskid < nRemainder){
        iComb = taskid + numtasks * nCycle;
        Design->nRx = (long) combinations[iComb][0];
        Design->alt = combinations[iComb][1];
        Design->inc = combinations[iComb][2];
        strcpy(Design->inc_str, combinations_inc_str[iComb]);
        Design->nPlane = combinations[iComb][3];
        if(iCycle>0)
            Design->flagTxPropagation = FALSE;
        
        switch(flagProc){
            case DESIGN_ALL:
                consPropagator(taskid);
                costAnalyzer(taskid);
                performAnalyzer(taskid);
                break;
            case DESIGN_PROP:
                consPropagator(taskid);
                break;
            case DESIGN_FOM:
                costAnalyzer(taskid);
                performAnalyzer(taskid);
                break;
            case DESIGN_COST:
                costAnalyzer(taskid);
                break;
            case DESIGN_PERF:
                performAnalyzer(taskid);
                break;
        }
        printf("Task %d >> Remainder %d/%ld complete\n", taskid, taskid+1, nRemainder);
    }

    printf("Task %d >> Simulation finished.\n", taskid);
}

void ConsDesign(int argc,char **argv)
{
    int flagProc;
    if(argc > 2){
        if(!strcmp(argv[2], "all"))
            flagProc = DESIGN_ALL;
        if(!strcmp(argv[2], "prop"))
            flagProc = DESIGN_PROP;
        if(!strcmp(argv[2], "fom"))
            flagProc = DESIGN_FOM;
        if(!strcmp(argv[2], "cost"))
            flagProc = DESIGN_COST;
        if(!strcmp(argv[2], "perf"))
            flagProc = DESIGN_PERF;
    }
    else{
        flagProc = DESIGN_ALL;
    }

    // Step 1: All - Initialization
    Design = (struct DesignType *) calloc(1, sizeof(struct DesignType));
    if (Design == NULL) {
        printf("Design calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    loadDesign();
    
    if(flagProc == DESIGN_ALL){
        initConsPropagator();
        initConsPerform();
    }
    else if(flagProc == DESIGN_PROP)
        initConsPropagator();
    else if(flagProc == DESIGN_FOM || flagProc == DESIGN_PERF)
        initConsPerform();
        
    InitReport();

    if(Design->consType == WALKER){
        runWalker(flagProc);
    }
    else if(Design->consType == HYBRID){
        runHybrid(flagProc);
    }
}