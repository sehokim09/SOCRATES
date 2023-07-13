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

#ifndef __SOCRATES_H__
#define __SOCRATES_H__

/* Disable extern keyword to declare globals */
#ifdef DECLARE_GLOBALS
   #define EXTERN
#else
   #define EXTERN extern
#endif
#include "defines.h"
#include "types.h"
#include "util.h"
#include <time.h>
#include "mpi.h"
#include "netcdf.h"
#include "sys/stat.h"
#include <float.h>
#include "omp.h"
//#include "fftw3.h"

EXTERN char SimPath[30];
EXTERN char OutPath[40];
EXTERN char OutSubPath[50];
EXTERN char SttDataPath[30];
EXTERN char DynDataPath[30];
EXTERN char AntPath[30];

EXTERN char OrbPathImport[40];
EXTERN char InOutPath42[40];
EXTERN char ModelPath42[40];
EXTERN char OrbPath42[30];
EXTERN char SCPath42[30];

EXTERN long SimMode, ObsMode, RetMode, ObsBase, OrbModel, GndModel, SmpModel;
EXTERN long FlagOrbDecay;
EXTERN double ModJulDay;
EXTERN char EndFlag;
EXTERN char OrbFileName[20];
EXTERN char RxName[20];
EXTERN long RxOrbFirst, RxOrbLast;
EXTERN struct InputType *InputSoOp;
EXTERN struct FixedObsType *Fixed;
EXTERN struct SoOpType *SoOp;
EXTERN struct ScType *Sc;
EXTERN struct ScType *RxSc, *TxSc;
EXTERN struct TwrType *Twr;
EXTERN struct BistaticType *Bistatic;
EXTERN struct DynDataType *DynData;
EXTERN struct SttDataType *SttData;
EXTERN struct GndDataType *GndData;
EXTERN struct VegDataType *VegData;
EXTERN struct VegKindType *VegKind;
EXTERN struct VegModelType *VegModel;
EXTERN struct MultiLayerType *MultiLayer;
EXTERN struct LocalGndType *Gnd;
EXTERN struct OutputType *Out;
EXTERN struct ReportType *ReportSoOp;
EXTERN long NSc, NRx, NData, NTx;
EXTERN long NSoOp, NPol, flagMixPol;
EXTERN char RxPol[2];
EXTERN long NTarget;
EXTERN long NLayer; // Number of Soil Layers
EXTERN long Icalval; // Index of Cal/Val Data
EXTERN long NVeg, NKind; // Number of Vegetation Types, Kinds
EXTERN long ExistGrid, NGrid; // Number of Grids
EXTERN struct TargetAreaType *TargetArea;
EXTERN struct SpotBeamType *SpotBeam;
EXTERN struct PostProcType *Data;
EXTERN struct EASEgridType *Grid;
EXTERN struct MapType *Map;
EXTERN uint8_t SchattenTimingModel; // NOMIAL_TIMING, EARLY_TIMING, LATE_TIMING - added
EXTERN uint8_t SchattenErrorModel; // MEAN_F10, PLUSTWOSIG_F10, MINUSTWOSIG_F10 - added

EXTERN struct SimAnnealType *SimAnneal;
EXTERN struct RetrievalType *Ret;
EXTERN struct NcDataType *Nc;
EXTERN long DataSrc;
EXTERN double **b_LUT; // Look-up table for VOD = b * VWC

// Subsystems
EXTERN long NGs;
EXTERN struct GsType *Gs;

// For Constellation Design
EXTERN struct DesignType *Design;

/* Math Basics */
//EXTERN double R2Dx60, R2Dx20;

/* Functions from 42 */
void LoadSun(void);
void LoadPlanets(void);
long LoadDE430(char DE430Path[80],double JD);
void LoadMoonOfEarth(void);
void ManageFlags(void);
void ZeroFrcTrq(void);
void ReportProgress(void);
long AdvanceTime(void);

/* Functions for Signals of Opportunity */
// obs.c
void updateProgress(long cnt);
void ObsFixed(void);
void ObsSpace(void);
void ObsTower(void);
void ObsDDM(void);
void GainAdjustUD(struct ScType *Tx, struct SpecularType *Sp);

// geometry.c
void updateBistaticFixed(struct FixedObsType *F);
void updateBistaticInorbit(struct SoOpType *S, double incAng);
void calcTxGeometry(struct FixedObsType *F);
void calcGeometryFixed(struct FixedObsType *F);
void calcGeometryInorbit(double *PosWR, double *PosWT, struct SpecularType *Sp);
void calcGeometryInorbitLocal(double *PosWR, double *PosWT, struct LocalGridType *Lg);
void FindSP_MPL(struct ScType *Rx, struct SpecularType *Sp, struct ScType *Tx);
void FindSP_UD(double *PosWR, double *PosWT, struct SpecularType *Sp);
long *FindVisibleTx(struct ScType *Rx, long *TxNum, long *TxCnt);
long *FindVisibleTxTwr(double *PosN, long *TxNum, long *TxCnt);
uint8_t FindVisibleSpotbeamTarget(struct ScType *Rx, struct SpecularType *Sp, struct SpotBeamType *MuosSb, struct TargetAreaType *TARGET);
void TrackingSpotbeam(struct ScType *Tx, struct SpotBeamType *MuosSb);
long *FindSpotbeam(struct SpotBeamType *MuosSb, double lat, double lon, long *SbNum, long *SbCnt);
uint8_t checkMUOSspotbeam(struct ScType *Rx, struct SpecularType *Sp, struct SpotBeamType *MuosSb);
void reportSpotbeam(void);
void LocalGeometry(double vSp2RxW[3], struct ScType *Rx, struct SoOpType *S, struct SpecularType *Sp, struct ScType *Tx);
void LocalGeometryDEM(double *PosWR, double *PosWT, struct SoOpType *S, struct SpecularType *Sp);
void tanLineLengAng(double *pos, double *length, double *angle);

// antenna.c
void calcAntPolRot(void);
void calcAntPattern(struct SoOpType *S, struct SpecularType *Sp);
void calcMueller(double complex u[2][2], double complex U[4][4]);
double*** LoadAntPattern(char antFileName[4][40], double *antPatRes, double ***antPattern);
double*** LoadAntPatternGG(double SLL, double XPL, double hpbw, double antPatRes, double*** antPattern);
void LookupAntPattern(double ***antPat, double res, double th, double ph, double complex g[2][2]);
void ReportAntPattern(char name[], double antPatRes, double*** antPattern);

// ground.c
void updateLocalGnd(double lat, double lon, double h);
void calcRcSingle(void);
void calcRcMulti(void);
double complex calcDielDebye(double freq);
void calcDielMironov(void);
double complex calcDielUlabyElRayesMg(double freq, double mv);
double complex calcDielUlabyElRayesMv(double freq, double mv);
double complex calcTwoPhaseMixture(long type, double complex e_i, double vi);
void calcMLDielProfile(void);
int SMP_POME_main(double sur, double bott, double avg);
void LookupSoiltype(void);
void qsimp_ThAvg(double a, double b, double eps, struct VegModelType *V, double complex s[4]);
void qsimp_pxf(double phi, double tol, struct VegModelType *V, double complex s[4]);
void calcPropagation(double freq, long iSoOp, long iVeg);
void calcPropagationNetCDF(long iSoOp, long iVeg, long iSample);
void calcVegT(long iSoOp, long iVeg);

// product.c
void calcDelayDoppler(double PosN[3], double VelN[3], struct SpecularType *Sp, struct ScType *Tx);
void error_gaussian(double std_dev);
void error_acf(void);
void error_acf_CN(double CN);
void error_acf_test(long ISoOp);
void calcPower(void);
void calcSNR(void);
void calcRCGcoh(struct SpecularType *Sp);
void calcRCGincoh(struct SpecularType *Sp);
void calcDirectTerm(void);
void calcSpecularTerm(void);
void calcSpecularTermForward(void);
void calcSpecularTermSimple(struct FixedObsType *F, double VWC);

void ReportGrid(void);
void ReportGridLandmask(void);
void ReportFixedObs(struct FixedObsType *F, long iData);
void ReportSpaceObs(struct ScType *Rx, struct SoOpType *S, struct SpecularType *Sp, struct ScType *Tx);
void ReportTowerObs(struct TwrType *T, struct SoOpType *S, struct SpecularType *Sp, struct ScType *Tx);
void ReportOrbit(void);
void ReportOrbElements(void);
void ReportSMP(void);
void ReportNoSignal(struct ScType *Rx);
void ReportError1D(long iSoOp);
void ReportError2D(void);
void ReportTrajectory(void);

// interface.c
void Propagator(void);

// earthdata.c
void SchattenAtmoParam(void);
void UpdateOrbitCycle(void);
void UpdateMonthlyData(void);
void UpdateCalvalData(void);
void LoadCalvalSoilData(long ITarget);
void LoadSnowCover(void);
void LoadFreezeThawState(void);
void LoadStaticData(void);
void LoadSoilData(void);
void LoadSnowCover(void);
void LL2EzRC1km(double lat, double lon, uint16_t *row, uint16_t *col);
void LL2EzRC9km(double lat, double lon, uint16_t *row, uint16_t *col);
void LL2EzRC(struct GridType *G, double lat, double lon, uint16_t *row, uint16_t *col);

// init.c
void Welcome(void);
void InitFOVs(void);
void InitCalvalSite(void);
void InitSpotbeams(void);
void InitSOCRATES(int argc,char **argv);
void InitReport(void);
void InitSubsys(void);

void postProcessCalVal(uint8_t RxNum, uint8_t TxNum);

// attitude.c
void calcRPY(struct SpecularType *Sp);
void CmdReport(void);
void ReportAttitude(struct ScType *Rx);

// comm.c
void ReportAccessTime(struct ScType *Rx);

// design.c
void ConsDesign(int argc,char **argv);
void orbitPropagator(void);

// retrieval.c
void initRetMode(int argc, char **argv);
void writeNcRet(void);
void writeNcForward(void);
void createNcRetPeriod(long year, char *stationName, long period, long window);
void createNcForward(long year, char *stationName);
void updateNcVeg(long iSample);
void readNcForward(long year, char *stationName);
void readNcUSCRN(long year, char *stationName);
void ObsFixedNetCDF(void);
void InitRetrieval(void);
void InitSA(void);
void RetrievalNetCDF(void);

/////////////////////////////////////////////

#endif /* __SOCRATES_H__ */
