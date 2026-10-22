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
	
#ifndef __DEFINES_H__
#define __DEFINES_H__

#define DEBUG 0
#define SA_TUNING 0

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef POSITIVE
	#define POSITIVE 1
#endif

#ifndef NEGATIVE
	#define NEGATIVE 0
#endif

#define EPS 1E-15
#define EL 0.5772156649015329
#define POS_ZERO 0

#define COMPLETE 110

//#define LIGHTSPEED 3e8 // speed of light [m/s]
#define LIGHTSPEED 299792458 // speed of light [m/s]
#define BOLTZMANN 1.380649E-23 // Boltzmann's constant [J/K]

#define FILLVALUE -9999
#define POME_FAILURE -1
#define POME_DRY 1
#define POME_WET 2
#define POME_DYNAMIC 3
#define SOILMOISTMAX_EFF 1.5 // 1.5
#define SOILMOISTMIN_EFF -0.15 // -0.15
//#define SOILMOISTMAX_EFF 1.33
//#define SOILMOISTMIN_EFF -0.06
#define VWCMAX 3.0
#define VWCMIN 0.0

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
#define ERRNO(e) {printf("Error: %s\n", strerror(errno)); exit(ERRCODE);}

// NetCDf
#define SINGLE_DIM_NANE "single"
#define SOILSEPARATES_DIM_NAME "soilSeparates"
#define SAMPLES_DIM_NAME "samples"
#define LAYERS_DIM_NAME "layers"
#define SUBLAYERS_DIM_NAME "sublayers"
#define SOOP_DIM_NAME "SoOps"
#define RETRIEVAL_DIM_NAME "retrievals"
#define CHAR_DIM_NAME "chars"
#define VEGS_DIM_NAME "vegs"
#define VEGVARS_DIM_NAME "vegVars"

#define LAT_VAR_NAME "lat"
#define LON_VAR_NAME "lon"
#define UTCDATE_VAR_NAME "utcDate"
#define UTCTIME_VAR_NAME "utcTime"
#define IND_PRODUCTTIME_VAR_NAME "productTimeIndex"
#define IND_OBSTIME_VAR_NAME "obsTimeIndex"
#define SOILDEPTH_VAR_NAME "soilDepth"
#define SOILDEPTHPOME_VAR_NAME "soilDepthPOME"
#define SOILTYPE_VAR_NAME "soilType"
#define SOILMOIST_VAR_NAME "soilMoist"
#define SOILTEMP_VAR_NAME "soilTemp"
#define AIRTEMPMIN_VAR_NAME "airTempMin"
#define PREC_VAR_NAME "prec"
#define VEGTYPE_VAR_NAME "vegType"
#define VWCREF_VAR_NAME "VWCref"
#define VEGVAR_VAR_NAME "vegVar"

#define FLAGQCFORWARD_VAR_NAME "flagQcForward"
#define FLAGQCRET_VAR_NAME "flagQcRet"
#define PENDEPTHH_VAR_NAME "penDepthH"
#define PENDEPTHV_VAR_NAME "penDepthV"
#define PENDEPTHHRET_VAR_NAME "penDepthHRet"
#define PENDEPTHVRET_VAR_NAME "penDepthVRet"
#define REFLCOEFCOR_R_B_VAR_NAME "reflCoefCoR_R_B"
#define REFLCOEFCOI_R_B_VAR_NAME "reflCoefCoI_R_B"
#define REFLCOEFXR_R_B_VAR_NAME "reflCoefXR_R_B"
#define REFLCOEFXI_R_B_VAR_NAME "reflCoefXI_R_B"
#define REFLCOEFCOR_X_B_VAR_NAME "reflCoefCoR_X_B"
#define REFLCOEFCOI_X_B_VAR_NAME "reflCoefCoI_X_B"
#define REFLCOEFXR_X_B_VAR_NAME "reflCoefXR_X_B"
#define REFLCOEFXI_X_B_VAR_NAME "reflCoefXI_X_B"
#define REFLCOEFCOR_R_V_VAR_NAME "reflCoefCoR_R_V"
#define REFLCOEFCOI_R_V_VAR_NAME "reflCoefCoI_R_V"
#define REFLCOEFXR_R_V_VAR_NAME "reflCoefXR_R_V"
#define REFLCOEFXI_R_V_VAR_NAME "reflCoefXI_R_V"
#define REFLCOEFCOR_X_V_VAR_NAME "reflCoefCoR_X_V"
#define REFLCOEFCOI_X_V_VAR_NAME "reflCoefCoI_X_V"
#define REFLCOEFXR_X_V_VAR_NAME "reflCoefXR_X_V"
#define REFLCOEFXI_X_V_VAR_NAME "reflCoefXI_X_V"

#define REFLCO_R_B_VAR_NAME "reflect_CO_R_B"
#define REFLX_R_B_VAR_NAME "reflect_X_R_B"
#define REFLCO_X_B_VAR_NAME "reflect_CO_X_B"
#define REFLX_X_B_VAR_NAME "reflect_X_X_B"

#define REFLCO_R_V_VAR_NAME "reflect_CO_R_V"
#define REFLX_R_V_VAR_NAME "reflect_X_R_V"
#define REFLCO_X_V_VAR_NAME "reflect_CO_X_V"
#define REFLX_X_V_VAR_NAME "reflect_X_X_V"

#define REFLX_R_B_REF_VAR_NAME "reflect_X_R_B_ref"
#define REFLX_R_B_EST_VAR_NAME "reflect_X_R_B_est"
#define REFLX_R_B_ERR_VAR_NAME "reflect_X_R_B_err"
#define REFLX_X_B_REF_VAR_NAME "reflect_X_X_B_ref"
#define REFLX_X_B_EST_VAR_NAME "reflect_X_X_B_est"
#define REFLX_X_B_ERR_VAR_NAME "reflect_X_X_B_err"
#define REFLCO_R_B_REF_VAR_NAME "reflect_CO_R_B_ref"
#define REFLCO_R_B_EST_VAR_NAME "reflect_CO_R_B_est"
#define REFLCO_R_B_ERR_VAR_NAME "reflect_CO_R_B_err"
#define REFLCO_X_B_REF_VAR_NAME "reflect_CO_X_B_ref"
#define REFLCO_X_B_EST_VAR_NAME "reflect_CO_X_B_est"
#define REFLCO_X_B_ERR_VAR_NAME "reflect_CO_X_B_err"

#define REFLX_R_V_REF_VAR_NAME "reflect_X_R_V_ref"
#define REFLX_R_V_EST_VAR_NAME "reflect_X_R_V_est"
#define REFLX_R_V_ERR_VAR_NAME "reflect_X_R_V_err"
#define REFLX_X_V_REF_VAR_NAME "reflect_X_X_V_ref"
#define REFLX_X_V_EST_VAR_NAME "reflect_X_X_V_est"
#define REFLX_X_V_ERR_VAR_NAME "reflect_X_X_V_err"
#define REFLCO_R_V_REF_VAR_NAME "reflect_CO_R_V_ref"
#define REFLCO_R_V_EST_VAR_NAME "reflect_CO_R_V_est"
#define REFLCO_R_V_ERR_VAR_NAME "reflect_CO_R_V_err"
#define REFLCO_X_V_REF_VAR_NAME "reflect_CO_X_V_ref"
#define REFLCO_X_V_EST_VAR_NAME "reflect_CO_X_V_est"
#define REFLCO_X_V_ERR_VAR_NAME "reflect_CO_X_V_err"

#define REFLERR_VAR_NAME "reflErr"
#define TYPEPOME_VAR_NAME "typePOME"
#define SURPOME_VAR_NAME "surPOME"
#define BOTTPOME_VAR_NAME "bottPOME"
#define AVGPOME_VAR_NAME "avgPOME"
#define VODH_VAR_NAME "VODH"
#define VODV_VAR_NAME "VODV"
#define VWCREFMODEL_VAR_NAME "VWCrefmodel"
#define ARGHR_VAR_NAME "argHr"
#define ARGHI_VAR_NAME "argHi"
#define ARGVR_VAR_NAME "argVr"
#define ARGVI_VAR_NAME "argVi"
#define E_C_CAN_R_VAR_NAME "e_c_can_r"
#define E_C_CAN_I_VAR_NAME "e_c_can_i"

#define TYPEPOMERET_VAR_NAME "typePOMERet"
#define SURPOMERET_VAR_NAME "surPOMERet"
#define BOTTPOMERET_VAR_NAME "bottPOMERet"
#define AVGPOMERET_VAR_NAME "avgPOMERet"
#define VWCINIT_VAR_NAME "VWCinit"
#define VWCRET_VAR_NAME "VWCRet"
#define SOILMOISTPOME_VAR_NAME "soilMoistPOME"
#define SOILMOISTRET_VAR_NAME "soilMoistRet"
#define SA_TIME_VAR_NAME "sa_time"
#define SA_ITER_VAR_NAME "sa_iter"
#define SA_JUMP_VAR_NAME "sa_jump"
#define SA_COSTMIN_VAR_NAME "sa_costMin"

#define INC_MUOS_VAR_NAME "incMUOS"
#define INC_ORBCOMM_VAR_NAME "incORBCOMM"
#define INC_GPS_VAR_NAME "incGPS"

#define RUNTIME "runtime"

#define QC_GOOD 0b0
#define QC_MISSING 0b1
#define QC_FROZEN 0b10

// Design
#define DESIGN_ALL 0
#define DESIGN_PROP 1
#define DESIGN_FOM 2
#define DESIGN_COST 3
#define DESIGN_PERF 4
#define WALKER 0
#define HYBRID 1
#define DEDICATED 1
#define RIDESHARE 2
#define MAX_NSP 7000
#define N_SOOPCOMBI 8
#define SAI_NCNT 5 // SAI counters per SoOp in the performance file
#define NO_OBS 0b0000
#define I_BAND 0b0001
#define P_BAND 0b0010
#define L_BAND 0b0100
#define I_BAND_SWARM 0b1000
#define IP_BAND 0b011
#define IL_BAND 0b101
#define PL_BAND 0b110
#define IPL_BAND 0b111
#define VISITED 1
#define VISIT_COUNTED 2
#define REVISITED 3
#define REVISIT_COUNTED 4
#define RCG_VALID 1
#define RCG_UNMATCHED -1
#define RCG_LOW -2
#define NOEXIST 0
#define EXIST 1

// MPI
#define MASTER 0

// Simulation mode
#define FIXED 0
#define INORBIT 1
#define DESIGN 2
#define RETRIEVAL 3

// Retrieval mode
#define FORWARD 0
#define INVERSE 1

// Space-based or Ground-based
#define SPACE 0
#define TOWER 1

// Grid Type
#define EASE2 1
#ifndef USER_DEFINED
	#define USER_DEFINED 2
#endif
// Ground Structure Model
#define SINGLE 0
#define MULTI 1
#define VEGETATION 2

// Geophysical Data Source
#define USCRN 0
#define OTHERS 1

// Soil Moisture Profile Model
#define EXPO 0
#define POME 1

// USDA Soil Texture
#define SAND 0
#define LOAMY_SAND 1
#define SANDY_LOAM 2
#define SILT_LOAM 3
#define SILT 4
#define LOAM 5
#define SANDY_CLAY_LOAM 6
#define SILTY_CLAY_LOAM 7
#define CLAY_LOAM 8
#define SANDY_CLAY 9
#define SILTY_CLAY 10
#define CLAY 11

// Number of Vegetation Types
#define NUM_VEGTYPE 5

// Vegetation Scatterer Type
#define DISK 0
#define CYLINDER 1
#define SPHERE 2

// Orbit Model
#define ORB_IMPORT 0
#define ORB_42 1
#define ORB_42_DECAY 2

#define MAXNUM_FIXED 4
#define MAXNUM_SC 300
#define MAXNUM_TX 11
#define MAXNUM_RX 120
#define MAXNUM_TARGET 15
#define ANTBODY 0 // Antenna body number

// Antenna Pattern
#define IDEAL 1
#ifndef USER_DEFINED
	#define USER_DEFINED 2
#endif
#define GAUSSIAN 3

#define ANT_PAT_TH_RANGE_DEG 180
#define ANT_PAT_PH_RANGE_DEG 360

// polarizations
//#define POL_H 0
//#define POL_V 1
//#define POL_X 2
//#define POL_Y 3
//#define POL_R 4
//#define POL_L 5
//#define POL_RY 6

// Air Dielectric Constant
#define EPS_DIEL_AIR 1.0

// Fitting Function for Dielectric Profile
#define POLY2ND 0
#define POLY3RD 1
#define DISCRETE_SLAB 2
#define LOGISTIC 3

// Solar Flux Model
#ifndef USER_DEFINED
	#define USER_DEFINED 2
#endif
#define SCHATTEN 3
#define NOMINAL_TIMING 0
#define EARLY_TIMING 1
#define LATE_TIMING 2

#define MEAN_F10 0
#define PLUSTWOSIG_F10 1
#define MINUSTWOSIG_F10 2

// Flight Software for 42
#define RX_FSW 11

#endif /* __DEFINES_H__ */
