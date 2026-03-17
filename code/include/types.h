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
	
#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>
#include <complex.h>
#include <stddef.h>

#undef I
#undef I1
#define I1 _Complex_I

struct CntType{
	long total;
	long land;
	long water;
};

struct CoverageType{
	double total;
	double land;
	double water;
};

struct RCGType{
	uint8_t flagRCGlevel;
	long cstr1_cnt;
	long cstr2_cnt;
	double cstr1_ratio;
	double cstr2_ratio;
};

struct CellType{
	uint8_t landMask;
	uint8_t obsStatus;
	uint8_t prevObsStatus;
	double lat; // deg
	double lon; // deg
	long *cnt; // cnt of access
	long *cntObs; // cnt of multifrequency observations
	long *cntRCGloss1; // cnt of missed cells due to RCG constraint 1
	long *cntRCGloss2; // cnt of missed cells due to RCG constraint 2
	double *prevAbsTime;
	double *currAbsTime;
	double *revisitMean;
	double *revisitMax;
	double *revisitMid;
	double *revisitMidLow;
	double *revisitMidUp;
	double *maxIncAng; // rad
	double *minIncAng; // rad
	double *sigma_gamma;
};

struct DesignSoOpType{
	char name[20];
	long nTx;
	char **TxName;
	double G_Rx; // Receive Antenna Gain [dB]
	double G_Rx_num; // numeric
	long antTag; // Antenna Type - USER-DEFINED, DIPOLE, PATCH etc.
	char antFileName[4][40]; // Antenna Pattern File Name
	char polRx; // Receiver Antenna Polarization
	double ***antPattern; // Antenna Pattern [dBi]
	double antPatRes; // Antenna Pattern Resolution [deg]
	double hpbw; // Receive Antenna Half-Power Bandwidth [deg]
	double SLL; // Side-lobe Level of Antenna Pattern [dB]
	double XPL; // Cross-Polarization Level of Antenna Pattern [dB]
};

struct DesignType{
	// Design Variables
	uint8_t consType;
	// Walker constellation
	long nRx; // Number of receivers per orbit
	long nPlane; // Number of orbital planes
	double alt; // Altitude in km
	double inc; // Inclination in degrees
	char inc_str[10];
	long nCandi_nRx;
	long nCandi_alt;
	long nCandi_inc;
	long nCandi_nPlane;
    double *candi_nRx;
    double *candi_alt;
	char **candi_inc_str;
    double *candi_nPlane;
	// Hybrid constellation
	char consName[20];
	int *nRxPerPlane;
	double *altPerPlane;
	char **incPerPlane_str;
	// Other Variables
	uint8_t flagTxPropagation; // if TRUE, propagate Tx, otherwise import existing data
	long timeWindow;
	double revisitThreshold;
	double minReflectRCG; // minimum reflected RCG (m^-4)
	double minDirectRCG; // minimum direct RCG (dB)
	double maxRCGdiff; // maximum RCG difference (dB) 
	// Internal Variables
	long nRxPrev;
	long nSCPrev;
	long *ID;
	double *RAAN;
	double argP;
	double *anom;
	double ecc;
	long iter;
	double costCons; // constellation cost
	double costDedicated; // launch cost - dedicated
	double costRideshare; // launch cost - ride share
	int launchOption; // 1: dedicated, 2: rideshare
	int nLaunch; // Number of launches
	uint8_t **landmask;
	struct CntType nCell;
	// Output
	uint8_t flagReportSatData;
	char outPath[200];
	char largeFileRoot[150];
	char largeFilePath[200];
	struct _IO_FILE ***outFileSp;
	// Variables for NetCDF for individual receiver data
	int *ncID;
	int *ncSuccessPropID;
	int **ncSecID;
	int **ncLatID;
	int **ncLonID;
	int **ncLookAngAzDID;
	int **ncLookAngElDID;
	int **ncLookAngAzRID;
	int **ncLookAngElRID;
	int **ncRangeTRID;
	int **ncRangeTSID;
	int **ncRangeSRID;
	int *ncFlagSpotbeamID; // 1: Valid, 0: Invalid
	int flagSpotbeam;
	size_t **ncCnt;
	int *accessSec;
	double *accessLat;
	double *accessLon;
	double *accessLookAngAzD;
	double *accessLookAngElD;
	double *accessLookAngAzR;
	double *accessLookAngElR;
	double *accessRangeTR;
	double *accessRangeTS;
	double *accessRangeSR;
	int *accessSpotbeam;
	// Variables for sorting Sp data into a grid
	int ****griddata;
	int ***griddataCnt;
	int gridDivisor;
	int nCol;
	int nWindow;
	// Variables for analyzing performance
	long nSoOp;
	struct DesignSoOpType *SoOp;
	int ncPerformID;
	int ncSuccessPerfID;
	int *ncRevisitTimeID;
	int *ncRevisitRowID;
	int *ncRevisitColID;
	int *ncAccessTime1stID;
	int *ncAccessTime2ndID;
	int *ncNspValidID;
	int *ncNspInvalidID;
	int ncObsFreqID;
	size_t *startpRevisit;
	size_t *startpAccessTime1st;
	size_t *startpAccessTime2nd;
	int *obsFreq;
	int *nSpValid;
	int *nSpInvalid;
	// Variables for analyzing cost
	int ncCostID;
	int ncCostVarID;	
};

struct MultiLayerType{
	// Parameters for EXPO SMP model (used in SCoBi)
	double zA; // Air Layer Thickness [m]
	double zB; // The Bottom-most Layer Thickness [m]
	double zS; // Total Layer Depth [m]
	long FitFcn; // Fitting Function for Dielectric Profile

	// Parameters for POME SMP model
	double sur; // surface effective SM for boundary condition
	double bott; // bottom-most effective SM for boundary condition
	double avg; // mean effective SM
	double D; // total depth [m]
	double z_infl; // inflection depth (cm)
	double infl_val; // inflection value
	double *SMP; // SM profile

	// Common Parameters
	double delZ; // Layer Discretization [m]
	double *z; // Layer Profile [m]
	long NSublayer; // Number of Discretized Layers (=zS/delZ)
	long NSublayer_extend;
	long NSublayer_POME;
};

struct NcDataType{
	char filename[30];
	long startYear;
	long startMonth;
	long endYear;
	long endMonth;

	// USCRN input
	int utcDateID;
	int utcTimeID;
	int soilDepthID;
	int soilTypeID;
	int soilMoistID;
	int soilTempID;
	int airTempMinID;
	int precID;
	int vegTypeID;
	int incMUOSID;
	int incORBCOMMID;
	int incGPSID;
	int VWCrefID;
	int vegVarID;
	size_t soilDepthDim;
	size_t soilTypeDim;
	size_t soilMoistDim;
	size_t soilSeparateDim;
	size_t sampleDim;
	size_t layerDim;
	size_t charDim;
	size_t vegDim;
	size_t vegVarDim;
	long *utcDate;
	long *utcTime;
	float *soilDepth;
	float soilType[3];
	float *soilMoist;
	float *soilTemp;
	float *airTempMin;
	float *prec;
	char *vegType;
	double *incMUOS;
	double *incORBCOMM;
	double *incGPS;
	double *VWCref;
	double *vegVar;

	// Forward Output
	int ncForwardID;
	int flagQcForwardID;
	int soilDepthPOMEID;
	int soilMoistPOMEID;
	int penDepthHID;
	int penDepthVID;
	int reflCoefCoR_R_B_ID;
	int reflCoefCoI_R_B_ID;
	int reflCoefXR_R_B_ID;
	int reflCoefXI_R_B_ID;
	int reflCoefCoR_X_B_ID;
	int reflCoefCoI_X_B_ID;
	int reflCoefXR_X_B_ID;
	int reflCoefXI_X_B_ID;
	int reflCoefCoR_R_V_ID;
	int reflCoefCoI_R_V_ID;
	int reflCoefXR_R_V_ID;
	int reflCoefXI_R_V_ID;
	int reflCoefCoR_X_V_ID;
	int reflCoefCoI_X_V_ID;
	int reflCoefXR_X_V_ID;
	int reflCoefXI_X_V_ID;
	int reflCo_R_B_ID;
	int reflX_R_B_ID;
	int reflCo_X_B_ID;
	int reflX_X_B_ID;
	int reflCo_R_V_ID;
	int reflX_R_V_ID;
	int reflCo_X_V_ID;
	int reflX_X_V_ID;
	int typePOMEID;
	int surPOMEID;
	int bottPOMEID;
	int avgPOMEID;
	int VODHID;
	int VODVID;
	int VWCrefmodelID;
	int argHrID;
	int argHiID;
	int argVrID;
	int argViID;
	size_t soopDim;
	size_t sublayerDim;
	int *flagQcForward;
	float *soilDepthPOME;
	double *soilMoistPOME;
	double *penDepthH;
	double *penDepthV;
	double *reflCoefCoR_R_B;
	double *reflCoefCoI_R_B;
	double *reflCoefXR_R_B;
	double *reflCoefXI_R_B;
	double *reflCoefCoR_X_B;
	double *reflCoefCoI_X_B;
	double *reflCoefXR_X_B;
	double *reflCoefXI_X_B;
	double *reflCoefCoR_R_V;
	double *reflCoefCoI_R_V;
	double *reflCoefXR_R_V;
	double *reflCoefXI_R_V;
	double *reflCoefCoR_X_V;
	double *reflCoefCoI_X_V;
	double *reflCoefXR_X_V;
	double *reflCoefXI_X_V;
	double *reflCo_R_B;
	double *reflX_R_B;
	double *reflCo_X_B;
	double *reflX_X_B;
	double *reflCo_R_V;
	double *reflX_R_V;
	double *reflCo_X_V;
	double *reflX_X_V;
	int *typePOME;
	double *surPOME;
	double *bottPOME;
	double *avgPOME;
	double *VODH;
	double *VODV;
	double *VWCrefmodel;
	double *argHr;
	double *argHi;
	double *argVr;
	double *argVi;	

	// Retrieval Output
	int flagQcRetID;
	int ncRetID;
	int iProductTimeID;
	int iObsTimeID;
	int reflX_R_B_RefID;
	int reflX_R_B_EstID;
	int reflX_R_B_ErrID;
	int reflX_X_B_RefID;
	int reflX_X_B_EstID;
	int reflX_X_B_ErrID;
	int reflCo_R_B_RefID;
	int reflCo_R_B_EstID;
	int reflCo_R_B_ErrID;
	int reflCo_X_B_RefID;
	int reflCo_X_B_EstID;
	int reflCo_X_B_ErrID;
	int reflX_R_V_RefID;
	int reflX_R_V_EstID;
	int reflX_R_V_ErrID;
	int reflX_X_V_RefID;
	int reflX_X_V_EstID;
	int reflX_X_V_ErrID;
	int reflCo_R_V_RefID;
	int reflCo_R_V_EstID;
	int reflCo_R_V_ErrID;
	int reflCo_X_V_RefID;
	int reflCo_X_V_EstID;
	int reflCo_X_V_ErrID;
	int reflErrID;
	int soilMoistRetID;
	int penDepthHRetID;
	int penDepthVRetID;
	int typePOMERetID;
	int surPOMERetID;
	int bottPOMERetID;
	int avgPOMERetID;
	int VWCRetID;
	int saTimeID;
	int saIterID;
	int saJumpID;
	int saCostMinID;
	int runtimeID;
	size_t retDim;
	int *flagQcRet;
	int *iProductTime;
	int *iObsTime;
	double *soilMoistRet;
	double *penDepthHRet;
	double *penDepthVRet;
	double *reflX_R_B_Ref;
	double *reflX_R_B_Est;
	double *reflX_R_B_Err;
	double *reflX_X_B_Ref;
	double *reflX_X_B_Est;
	double *reflX_X_B_Err;
	double *reflCo_R_B_Ref;
	double *reflCo_R_B_Est;
	double *reflCo_R_B_Err;
	double *reflCo_X_B_Ref;
	double *reflCo_X_B_Est;
	double *reflCo_X_B_Err;
	double *reflX_R_V_Ref;
	double *reflX_R_V_Est;
	double *reflX_R_V_Err;
	double *reflX_X_V_Ref;
	double *reflX_X_V_Est;
	double *reflX_X_V_Err;
	double *reflCo_R_V_Ref;
	double *reflCo_R_V_Est;
	double *reflCo_R_V_Err;
	double *reflCo_X_V_Ref;
	double *reflCo_X_V_Est;
	double *reflCo_X_V_Err;
	double reflErr;
	int *typePOMERet;
	double *surPOMERet;
	double *bottPOMERet;
	double *avgPOMERet;
	double *VWCRet;
	size_t nCycle;
	double *saTime;
	long *saIter;
	long *saJump;
	double *saCostMin;
	double runtime;
};

struct VegDataType{
	char label[20];
	long nLayer;
	double *thickness;
	double *typkndThick;
	double depth;
	long *nKind;
	char ***kind;
	int **typknd;
	int **typknd_new;
	long *part_type_idx;
	double ***dsty;
	double ***dim1;
	double ***dim2;
	double ***dim3;
	double complex ****e_c;
	double ***beginAng;
	double ***endAng;
	long ***shape;
	long nKindMax;
	long nTypeMax;
	double complex ****dKz; // Vegetation Effect
	double complex **dKzNc; // Vegetation Effect for NetCDF
	double ***VWC; // Vegetation Water Content of each type and kind, VWC = VWC[kg/m3]*depth in kg/m2
	double VWC_total; // Total vegetation water content, sum of all type and kind's
};

struct VegKindType{
	char ID[2];
	double density; // [particles/m^3]
	double dim[3]; // [m]
	double mg; // Gravimetric Water Content
	double mv; // Volumetric Water Content
	double complex *e_c;
	double beginAng; // [deg]
	double endAng; // [deg]
	double VWC; // Vegetation water content, VWC = density*volume*water content (*depth) in kg/m3
};

struct VegModelType{
	long shape;
	double theta1;
	double theta2;
	double tin;
	double pin;
	double phi;
	double freq;
	double complex epsilon;
	// Disk
	double semiMajorx;
	double semiMinorx;
	double thickness;
	// Cylinder
	double radius;
	double length;
};

struct GndDataType{
	long flagPenDep; // Flag for calculating penetration depth
	char filename[30];
	double depth; // Layer Depth [m]
	double layer_bottom; // Layer Bottom Depth [m]
	double layer_thickness; // Layer Thickness [m]
	float VSM[1624][3856]; // Volumetric Soil Moisture [m^3/m^3] in 9km EASE grid2
};

struct DynDataType{
	long ExistVSM, ExistSoilTemp, ExistFreezeThaw, ExistSnowCover;
	char filename_snowCover[12][30]; // For monthly files
	char filename_soilTemp1[30];
	uint8_t snowCover[3600][7200]; // 0.05 deg
	uint8_t freezeThaw[1624][3856]; // Freeze/Thaw State in 9km EASE grid2, 1: Frozen, 0: Thawed
	float soilTemp1[1624][3856]; // Soil Temperature Layer 1 [K] in 9km EASE grid2
};
/*
struct SttDataType{
	long ExistSoilTexture, ExistLandMask, ExistDEM;
	char filename_DEM[40];
	char filename_landMask[30];
	char filename_fixed[30]; // list of sand, clay, bulk density for fixed observation
	int16_t height[10800][21600]; // ellipsoid height [m]
	uint8_t landMask[14616][34704]; // 1km EASE grid2
};
*/

struct SttDataType{
	long ExistSoilTexture, ExistSoilPorosity, ExistLandMask, ExistLandCover, ExistDEM, ExistVeg;
	char filename_DEM[40];
	char filename_landMask[30];
	char filename_landType[30];
	char filename_sand[30];
	char filename_clay[30];
	char filename_rho_b[30];
	char filename_porosity[30];
	char filename_fixed[30]; // list of sand, clay, bulk density for fixed observation
	int16_t height[10800][21600]; // ellipsoid height [m]
	uint8_t landMask[14616][34704]; // 1km EASE grid2
	uint8_t landType[3600][7200]; // 0.05 deg
	float sand[14616][34704]; // Mass Fractions of Sand (0<=S<=1) in 1km EASE grid2
	float clay[14616][34704]; // Mass Fractions of Clay (0<=C<=1) in 1km EASE grid2
	float bulk[14616][34704]; // bulk density of soil sample in [g/cc] (grams/cubic centimeter)
	float porosity[600][1440]; // GLDAS Soil porosity 0.25 deg
};

struct LocalGndType{
	// Static Data
	float sand; // sand textural component of a soil by weight (0<=S<=1)
	float clay; // clay textural component of a soil by weight (0<=C<=1)
	float bulk; // bulk density of soil sample in [g/cc] (grams/cubic centimeter)
	float porosity; // soil porosity (GLDAS 0.25 deg resolution) (NOT USED)
	uint8_t landMask;
	uint8_t landType;

	// Look-up data
	float fc; // soil field capacity (m^3/m^3)
	float poro; // soil porosity (m^3/m^3)
	float resid; // residual soil moisture (m^3/m^3)
	float wp; // wilting point (m^3/m^3)
	uint8_t soilType;

	// Dynamic Data
	uint8_t snowCover;
	uint8_t freezeThaw;
	float soilTemp1; // temperature in [C]

	// Ground Data
	float *VSM; // volumetric water content in g*cm^-3 (NLayer)
	double VWC; // vegetation water content in kg/m2

	// Dielectric Profile Realization Method
	long model;

	// Dielectric Constant
	double complex *e_c; // Complex dielectric constant (NLayer)
	double complex *e_c_profile; // Dielectric Constant Profile Generated by Fitting Function (MultiLayer->NSublayer)

	// Soil Moisture Profile
	double *SMP;

	// Reflection
	double h; // Effective Roughness Parameter h = (2*RMSH*k0)^2, k0 = TwoPi/Wavelength[cm] in [cm]
	double complex FVV; // Standard Fresnel coefficient
	double complex FHH;
	double complex FLR;
	double complex FRR;
	double gammaV; // reflectivity (Vertical polarization)
	double gammaH; // reflectivity (Horizontal polarization)
	double complex RcV; // reflection coefficient
	double complex RcH;

	// Penetration depth
	double pdv;
	double pdh;
};

struct LocalGridType{
	double theta;
	double phi;
	double range;
	double delay;
	double doppler;
	double incAngL2Tx;
	double incAngL2Rx;
	double scaAng;
	double scaVec[3];
	double lat; // Geodetic latitude [rad]
	double lon;	// Geodetic longitude [rad]
	double alt; // Altitude normal to ellipsoid [m]
	double PosW[3];
	double PosN[3];
	double dArea;
	double lookAngElR;
	double lookAngAzR;
	// test
	double az;
	double elev;
	double incAng;
	double AngT2S_sf[2];
};

struct SpecularType{
	long Exists;
	long SpNum; // Label
	long TxID; // Transmitter SC[TxID]
	double CEW[3][3]; // Direction Cosine Matrix from ECEF to ENU
	double CSE[3][3]; // Direction Cosine Matrix from ENU to Specular frame
	double CLS[3][3]; // Direction Cosine Matrix from Specular to local surface frame
	double CSW[3][3]; // Direction Cosine Matrix from ECEF to Specular frame
	double PosN[3]; // Position of Specular Point expressed in ECI frame [m]
	double PosW[3]; // Position of Specular Point expressed in ECEF frame [m]
	double lat;
	double lon;
	double alt;
	double az; // Azimuth angle [rad]
	double elev; // Elevation angle [rad]
	double incAng; // Incidence angle [rad]
	double scaAng; // Scattering angle [rad]
	double h; // Effective roughness parameter h = (2*RMSH_cm*k0)^2
	double roll; // roll angle of 1-2-3 rotations from nadir to specular point in LVLH coordinates [rad]
	double pitch; // pitch angle of 1-2-3 rotations from nadir to specular point in LVLH coordinates [rad]
	double yaw; // yaw angle of 1-2-3 rotations from nadir to specular point in LVLH coordinates [rad]
	double lookAngAzR; // Earth-view Antenna Azimuth Look Angle [rad]
	double lookAngElR; // Earth-view Antenna Elevation Look Angle [rad]
	double lookAngAzD; // Sky-view Antenna Azimuth Look Angle [rad]
	double lookAngElD; // Sky-view Antenna Elevation Look Angle [rad]
	double AntGainD; // Antenna Gain at Direct Signal [dB]
	double AntGainR; // Antenna Gain at Reflected Signal [dB]
	double a; // 1st Fresnel Zone Semi-major Axis [m]
	double b; // 1st Fresnel Zone Semi-minor Axis [m]
	double area; // Area of 1st Fresnel zone area = pi*a*b [m^2]
	double K; // Gain of gradient function
	long cnt; // the number of iteration to find specular point
	double correction;
	double angErr;
	double gradient;
	double incAng2;
	double angBisNor;
	double AngT2S_sf[2];

	double VarSM; // Variance of Soil Moisture Estimate [dB]
	double VarERC; // Variance of Effective Reflection Coefficient [dB]
	double r;

	double depthP; // penetration depth [m]
	double depthS; // skin depth [m]

	long SpotbeamID[2]; // MUOS Spotbeam ID [0]: Rx, [1]: Sp
	struct LocalGridType **LocalGrid;
	long flagUpdatedSp;
};

struct SoOpType{
	long ID;
	char TxName[20];
	long RefOrbFirst;
	long RefOrbLast;
	long NTx; // number of transmitting satellites
	long NSp; // The Maximum Number of Specular Points
	long SpCnt; // The Number of Specular points
	long NValidSp; // Number of Valid Specular Points
	struct SpecularType *Sp;
	//char SpriteFileName[40];
	//long Nphi; // Number of rows (phi-angle Look-up) in Antenna Pattern File
	//long Ntheta; // Number of columns (theta-angle Look-up) in Antenna Pattern File
    //unsigned int SpriteTexTag;
	double Freq; // [Hz]
	double Wavelength; // [m]
	double Wavenumber;
	double h; // Effective Roughness Parameter h = (2*RMSH*k0)^2, k0 = TwoPi/Wavelength[cm] in [cm]
	double EIRP_Tx; // Transmitter EIRP [dB]
	double G_Rx; // Receive Antenna Gain [dB]
	double t_coh; // Coherent Integration Time [sec]
	long AntTag; // Antenna Type - USER-DEFINED, DIPOLE, PATCH etc.
	char AntFileName[4][40]; // Antenna Pattern File Name
	char polTx; // Transmitter Antenna Polarization 
	char polRx; // Receiver Antenna Polarization
	double ***AntPattern; // Antenna Pattern [dBi]
	double AntPatRes; // Antenna Pattern Resolution [deg]
	double hpbw; // Receive Antenna Half-Power Bandwidth [deg]
	double SLL; // Side-lobe Level of Antenna Pattern [dB]
	double XPL; // Cross-Polarization Level of Antenna Pattern [dB]
	double AntMaxGainS; // Antenna Max Gain of Sky-view Antenna [dB]
	double AntMaxGainE; // Antenna Max Gain of Earth-view Antenna [dB]
	double Bandwidth; // Noise bandwidth of the front-end [Hz]
	double NoiseTemS; // Noise Temperature of Sky-view Antenna [K]
	double NoiseTemE; // Noise Temperature of Earth-view Antenna [K]
	double sres_req; // Spatial resolution requirement [m]
	double t_inc; // Max incoherent integration time -set by  [sec]
	double N_inc; // Number of independent samples

	/* Local Surface Grid Input Parameters */
	double minSurfRes;
	long NthetaLocal;
	long NphiLocal;
};

struct ScType{
	long ID; // Assigned Spacecraft
	char OrbFilename[20];
	double PosN[3]; // [m]
	double PosW[3]; // [m]
	double VelN[3]; // [m/s]
	double VelW[3]; // [m/s]
	double lat; // [rad]
	double lat_prev; // [rad]
	double lon; // [rad]
	double alt; // [m]
	double CLN[3][3];
	long orbitNum;
	long flagUpdated; // TRUE if variables related to this Sc has been updated for this time step
};

struct TwrType{
	long ID;
	char label[20];
	double PosN[3]; // [m]
	double PosW[3]; // [m]
	double VelN[3]; // [m/s]
	double VelW[3]; // [m/s]
	double lat; // [rad]
	double lon; // [rad]
	double alt; // [m]
	double CLN[3][3];
	double elev;
};

struct BistaticType{
	double r_tr; // Slant Range from Transmitter to Receiver [m]
	double r_ts; // Slant Range from Transmitter to Specular Point [m]
	double r_sr; // Slant Range from Specular Point to Receiver [m]
	double r_tsr; // r_ts + r_sr
	double idn[3]; // propagation vector (i_d^-)
	double isn[3]; // propagation vector (i_s^-)
	double osp[3]; // propagation vector (o_s^+)
	double Tgs[3][3]; // Transformation matrix for transforming a vector from the ground frame to local (specular) ground system
	double Tgr[3][3]; // Transformation matrix for transforming a vector from the ground frame to receiver system
	double Tgt[3][3]; // Transformation matrix for transforming a vector from the ground frame to transmitters system
	double complex u_tr[2][2]; // Rotation Matrix (Transmitter to Receiver)
	double complex u_ts[2][2]; // Rotation Matrix (Transmitter to Specular Point)
	double complex u_sr[2][2]; // Rotation Matrix (Specular Point to Receiver)
	double complex g_rt[2][2]; // Normalized Voltage Antenna Pattern of Receiver in the transmitter direction
	double complex g_rs[2][2]; // Normalized Voltage Antenna Pattern of Receiver in the specular direction
	char polRx; // Receive Antenna Polarization
	char polTx; // Transmit Antenna Polarization
	double AngT2S_sf_th0;
	double complex ArgH;
	double complex ArgV;

	double Freq; // [Hz]
	double Wavelength; // [m]
	double Wavenumber;
	double th; // Incidence Angle [rad]
	double EIRP_Tx; // [dB]
	double EIRP_Tx_Lin; // Decibel to Natural
	double G_Rx; // [dB]
	double G_Rx_Lin; // Decibel to Natural
	double t_coh; // Coherent Integration Time [sec]
	double Bandwidth; // Noise bandwidth of the front-end [Hz]
	double Tn; // Noise temerature [K]
	long N_inc; // Number of Independent Samples
};

struct FixedObsType{
	long ID;

	double Freq; // [Hz]
	double Wavelength; // [m]
	double EIRP_Tx; // [dB]
	char polRx; // Receive Antenna Polarization
	char polTx; // Transmit Antenna Polarization
	double G_Rx; // [dB]
	double h; // Effective Roughness Parameter h = (2*RMSH*k0)^2, k0 = TwoPi/Wavelength[cm] in [cm]
	double t_coh; // Coherent Integration Time [sec]
	double Bandwidth; // Noise bandwidth of the front-end [Hz]
	double Tn; // Noise temerature [K]
	double sres_req; // Spatial Resolution Requirement [m]
	double t_inc; // Max Incoherent Integration Time [sec] - set by sres_req
	long N_inc; // Number of Independent Samples - floor(t_inc/t_coh)

	// Bistatic Geometry
	double rTx; // Transmitter range from Earth's center [m]
	double hTx; // Transmitter altitude [m]
	double elTx; // Tx Elevation = 90 - Incidence angle [rad]
	double thTx; // Tx Incidence angle [rad]
	double phTx; // Tx Azimuth angle [rad]
	double hRx; // Receiver altitude [m]
	double elRx; // Rx Elevation = 90 - Incidence angle [rad]
	double thRx; // Rx Antenna looking angle (angle of incidence) [rad]
	double phRx; // Rx Azimuth angle of receiver position [rad]
	double vcircRx; // Rx Circular Velocity [m/s] in ECEF frame
	double S0x; // Distance of specular point away from the receiver ground projection [m]
	double x1; // Distance to the center of the ellipse [m]
	long Nfz; // Number of Fresnel zones
	double ax1; // semi-major axis of Fresnel zone [m]
	double by1; // semi-minor axis of Fresnel zone [m]
	long AntTag; // Antenna Type - USER_DEFINED, IDEAL
	char AntFileName[4][40]; // Antenna Pattern File Name
	double ***AntPattern; // Antenna Pattern
	double AntPatRes; // Antenna Pattern Resolution [deg]
	double hpbw; // Receive Antenna Half-Power Bandwidth [deg]
	double SLL; // Side-lobe Level of Antenna Pattern [dB]
	double XPL; // Cross-Polarization Level of Antenna Pattern [dB]
	double AngT2R_rf[2];
	double AngS2R_rf[2];
	double AngT2S_sf[2];
	char orbitName[5];
};

struct InputType{
	struct FixedObsType *F;
	struct SoOpType *S;
	struct ScType *Sc;
	struct TwrType *Twr;
	double RMSH; // Root-mean-square height of roughness of the soil surface [cm]
};

struct OutputType{
	// Factor K
	double complex K;
	double complex Kd;
	double complex Kc;

	// Field
	// Direct
	double complex b_d1[2];
	double complex b_d2[2];
	// Specular
	double complex b_coh1[2];
	double complex b_coh2[2];
	double complex b_coh1b[2];
	double complex b_coh2b[2];
	double complex b_coh1v[2];
	double complex b_coh2v[2];

	// Power - Reflectivity
	// Direct
	double P_d1[4];
	double P_d2[4];
	// Specular
	double P_coh1[4];
	double P_coh2[4];
	double P_coh1_err[4]; // error included 
	double P_coh2_err[4]; // error included
	double P_coh1b[4];
	double P_coh2b[4];
	double P_coh1v[4];
	double P_coh2v[4];
	double Gamma_Std_mod_lin[2];
	double Gamma_Std_mod_fused[2];
	double Gamma_Std_mod_fused_avg[2];

	// Power
	double PWR_d1[2];
	double PWR_d2[2];
	double PWR_coh1[2];
	double PWR_coh2[2];

	// SNR
	double SNR0[2]; // for detectability
	double SNR_d1[2];
	double SNR_d2[2];
	double SNR_coh1[2];
	double SNR_coh2[2];

	// Delay and Doppler
	double delayD; // delay for direct signal [sec]
	double delayR; // delay for reflected signal [sec]
	double dopplerD; // doppler for direct signal [Hz]
	double dopplerR; // doppler for reflected signal [Hz]

	// Differential Phase
	double phaseRD;

	// Optical depth
	double tauH;
	double tauV;

	// Range Corrected Gain
	double RCG_R; // in dB
	double RCG_D; // in dB
	double Gamma_eff;
};

struct ReportType{
	// SoOp Observation
	uint8_t JulDay_Sp;
	uint8_t Date;
	uint8_t LLA_Rx;
	uint8_t PosW_Rx;
	uint8_t VelW_Rx;
	uint8_t TxID;
	uint8_t LLA_Tx;
	uint8_t PosW_Tx;
	uint8_t VelW_Tx;
	uint8_t LLA_Sp;
	uint8_t PosW_Sp;
	uint8_t LoS_D;
	uint8_t LoS_R;
	uint8_t DelayDoppler;
	uint8_t Gain_D;
	uint8_t Gain_R;
	uint8_t FresZone;
	uint8_t IncAng;
	uint8_t elev;
	uint8_t RPY_to_Sp;
	uint8_t LandMask;
	uint8_t VSM;
	uint8_t SoilTemp;
	uint8_t FreezeThaw;
	uint8_t SoilTexture;
	uint8_t SnowCover;
	uint8_t RcCO;
	uint8_t RcX;
	uint8_t GammaCO;
	uint8_t GammaX;
	uint8_t SNR_D;
	uint8_t SNR_R;
	uint8_t SbNum;
	// Comm
	uint8_t AccessTime;
	// ADCS
	uint8_t ADCS;
	uint8_t FirstRxID;
	uint8_t LastRxID;
	long nADCS;
	uint8_t TruthAtt;
	uint8_t JulDay;
	uint8_t PosN;
	uint8_t AngVel;
	uint8_t AngMom;
	uint8_t RPY;
	uint8_t MagField;
	uint8_t Eclipse;
	uint8_t Gyr;
	uint8_t Mtm;
	uint8_t Acc;
	uint8_t Whl;
	uint8_t Mtb;
};

struct SpotBeamType{
	// Variables for global coverage
	double x[16];
	double y[16];
	double r0sq; // Reference radius square
	double r[17]; // actual radius of a spotbeam
	double el[4]; // Beam spacing elevation - origin + 3 layers of spot beams
	double az[3][5]; // Beam spacing azimuth - each layer has 5 azimuth angles
	double scaleFactor;
	long visibleSb[16];
	long Exist[16][64]; // Existence
	double PosW[16][64][3];
	double lat[16][64]; // Latitude of spotbeam projection 
	double lon[16][64]; // Longitude of spotbeam projection
	double alt[16][64]; // Altitude of spotbeam projection
	uint8_t flagShifted[16];
	// Variables for spotbeam on Target
	double halfBW; // Half beamwidth of spotbeam
	double halfBW_whole;
	double altTx; // altitude of MUOS
	double r0Target; // radius of spotbeam
};

struct TargetAreaType{
	char name[30];
	double radius;
	double lat;
	double lon;
	double alt; // Ellipsoid height [m]
	double PosW[3];
	double PosN[3];
	double vTarget2SpW[3]; // Vector from Target to Specular point in ECEF frame
	double mTarget2Sp; // Distance between Target and Specular point
	double CTW[3][3]; // Direction Cosine Matrix from ECEF to Target frame
	// Soil Moisture Input
	double *jd; // Julian Day
	float **VSM;
};

struct PostProcType{
	uint16_t arc, TxID, site, visible, landMask;
	float m_v, Temp, sand, clay, rho_b;
	double t, lat, lon, alt, a, b, az, gammaLR, SnrR, SnrD, lookAngAzD, lookAngElD, lookAngAzR, lookAngElR;
};

struct MapType{
	double equatorial_radius_m; // m
	double eccentricity;
	double e2;
	double e4;
	double e6;
	double reference_latitude; // rad
	double second_reference_latitude; // rad
	double reference_longitude; // rad
	double sin_phi1;
	double cos_phi1;
	double kz;
};

struct EASEgridType{
	uint8_t type; // TRUE if EASE2, FALSE if user-defined
	char gpd[8];
	char projection;
	double map_scale_m;
	long nCol;
	long nRow;
	double r0;
	double s0;
	double x0; // For user-defined grid
	double y0; // For user-defined grid
	double lat0; // For user-defined grid
	double lon0; // For user-defined grid
	double *lat;
	double *lon;
	long *row;
	long *col;
	double latLimits[2]; // Grid boundary [min max]
	double lonLimits[2]; // Grid boundary [min max]
};

struct GridType{
	uint8_t type; // TRUE if EASE2, FALSE if user-defined
	double x0;
	double y0;
	double lat0; // rad
	double lon0; // rad
	long nRowEdge;
	long nColEdge;
	long nRow;
	long nCol;
	double pixelSize; // m
	double *latEdge; // rad
	double *lonEdge; // rad
	double *lat; // rad
	double *lon; // rad
	struct EASEgridType easeGrid;
};

struct SimAnnealType{
	// Inputs
	long nDim;
	long nIter;
	long nStep;
	long nTemp;
	long nTrap;
	long nGen; // for ASA
	long nAccepted; // for ASA
	double *c; // for ASA
	double c_cost; // for ASA
	double f_min;
	double f_err;
	double *f_list;
	double *lb;
	double *ub;
	double T0; // Initial temperature
	double Rt; // Temperature reduction coefficient
	double ci; // Varying criterion
	double k_accept;
	double *x_max;
	double *x_min;
	double *x_avg;
	// Outputs
	long iIter;
	long iTemp;
	long iStep;
	long cnt_feval;
	long cnt_trapped;
	long cnt_reanneal;
	uint8_t flag_stop;
	double *accepted_rate;
	double T;
	double *x;
	double *x_init;
	double *x_opt;
	double f;
	double f_init;
	double f_opt;
	double *T_gen;
	double T_accept;
	double *s;
};

struct RetrievalType{
	double *VSM_ref;
	double *VSM_est;
	double *VSM_err;
	double *R_CO_ref;
	double *R_CO_est;
	double *R_X_ref;
	double *R_X_est;
//	double *R_CO_B_ref;
//	double *R_CO_B_est;
//	double *R_X_B_ref;
//	double *R_X_B_est;
//	double *R_CO_V_ref;
//	double *R_CO_V_est;
//	double *R_X_V_ref;
//	double *R_X_V_est;
	double *R_X_noise;
	double stddev;
	long period;
	long window;
	double complex *ArgH;
	double complex *ArgV;
	double VWC_ref;
	// POME model
	double sur_ref;
	double avg_ref;
	double bott_ref;
	double sur_est;
	double avg_est;
	double bott_est;
	double sur_err;
	double avg_err;
	double bott_err;
};

struct GsType{
	char name[20];
	double lat; // [rad]
	double lon; // [rad]
	double alt; // Ellipsoid height [m]
	double PosW[3]; // Position in ECEF coordinates [m]
	double beamwidth_Gs; // Half antenna beamwidth of Ground station [rad]
	double beamwidth_Rx; // Half antenna beamwidth of Spacecraft [rad]
	long *flagAccessStart;
	long *flagAccessEnd;
	long *cntAccess;
	double *t_acc_start; // Starting time of access interval
	double *t_acc_end; // Ending time of access interval
};

typedef struct {
    double *F;
    double *X137;
    double *X255;
    double *X370;
    size_t n;
} tableCDF;

typedef struct{
	double S0;
	double alpha_threshold;
	double eta_bar_single;
	double N_coh;
	const double *X;

	double alpha;
	double beta;
	double SD;
	double ND;
	double C;
	double Gamma_upper;
	double Gamma_lower;

	double F_upper;
	double F_lower;
	double gamma;
} modelSAI;

#endif
