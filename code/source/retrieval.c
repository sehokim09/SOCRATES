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

void initRetMode(int argc, char **argv)
{
	if (argc > 2){
		if(!strcmp(argv[2], "f")){
			RetMode = FORWARD;
		}
		else if(!strcmp(argv[2], "i")){
			RetMode = INVERSE;
			if (argc > 3) sprintf(OutSubPath, "%s%s/", OutPath, argv[3]);
			else printf(">> Error: Missing third argument. Specify your output directory.\n");
		}
		else{
			printf(">> Error: Second argument is for retrieval simulation and must be either 'f' for forward mode or 'i' for inverse mode.\n");
		}
	}
}

void writeNcRet(void)
{
	int retval;

	// Quality Control Flag
	if ((retval = nc_put_var_int(Nc->ncRetID, Nc->flagQcRetID, &Nc->flagQcRet[0])))
		ERR(retval);
	// Product Time Index
	if ((retval = nc_put_var_int(Nc->ncRetID, Nc->iProductTimeID, &Nc->iProductTime[0])))
		ERR(retval);
	// Observation Time Index
	if ((retval = nc_put_var_int(Nc->ncRetID, Nc->iObsTimeID, &Nc->iObsTime[0])))
		ERR(retval);
	// Reflectivity Error Parameter
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflErrID, &Nc->reflErr)))
		ERR(retval);
	// Penetration Depth
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->penDepthHRetID, &Nc->penDepthHRet[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->penDepthVRetID, &Nc->penDepthVRet[0])))
		ERR(retval);
	// Reference Reflectivity
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_R_B_RefID, &Nc->reflX_R_B_Ref[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_X_B_RefID, &Nc->reflX_X_B_Ref[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_R_V_RefID, &Nc->reflX_R_V_Ref[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_X_V_RefID, &Nc->reflX_X_V_Ref[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_R_B_RefID, &Nc->reflCo_R_B_Ref[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_X_B_RefID, &Nc->reflCo_X_B_Ref[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_R_V_RefID, &Nc->reflCo_R_V_Ref[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_X_V_RefID, &Nc->reflCo_X_V_Ref[0])))
		ERR(retval);
	// Estimated Reflectivity
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_R_B_EstID, &Nc->reflX_R_B_Est[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_X_B_EstID, &Nc->reflX_X_B_Est[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_R_V_EstID, &Nc->reflX_R_V_Est[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_X_V_EstID, &Nc->reflX_X_V_Est[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_R_B_EstID, &Nc->reflCo_R_B_Est[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_X_B_EstID, &Nc->reflCo_X_B_Est[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_R_V_EstID, &Nc->reflCo_R_V_Est[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_X_V_EstID, &Nc->reflCo_X_V_Est[0])))
		ERR(retval);
	// Inserted Error
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_R_B_ErrID, &Nc->reflX_R_B_Err[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_X_B_ErrID, &Nc->reflX_X_B_Err[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_R_V_ErrID, &Nc->reflX_R_V_Err[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflX_X_V_ErrID, &Nc->reflX_X_V_Err[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_R_B_ErrID, &Nc->reflCo_R_B_Err[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_X_B_ErrID, &Nc->reflCo_X_B_Err[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_R_V_ErrID, &Nc->reflCo_R_V_Err[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->reflCo_X_V_ErrID, &Nc->reflCo_X_V_Err[0])))
		ERR(retval);
	// Soil Moisture Retrieval
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->soilMoistRetID, &Nc->soilMoistRet[0])))
		ERR(retval);
	// Profile Type
	if ((retval = nc_put_var_int(Nc->ncRetID, Nc->typePOMERetID, &Nc->typePOMERet[0])))
		ERR(retval);
	// POME input - surface SM
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->surPOMERetID, &Nc->surPOMERet[0])))
		ERR(retval);
	// POME input - bottom SM
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->bottPOMERetID, &Nc->bottPOMERet[0])))
		ERR(retval);
	// POME input - average SM
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->avgPOMERetID, &Nc->avgPOMERet[0])))
		ERR(retval);
	// Vegetation Water Content - retrieval
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->VWCRetID, &Nc->VWCRet[0])))
		ERR(retval);
	// Simulated Annealing Computational Time
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->saTimeID, &Nc->saTime[0])))
		ERR(retval);
	// Simulated Annealing Number of Iterations
	if ((retval = nc_put_var_long(Nc->ncRetID, Nc->saIterID, &Nc->saIter[0])))
		ERR(retval);
	// Simulated Annealing Number of Jumps
	if ((retval = nc_put_var_long(Nc->ncRetID, Nc->saJumpID, &Nc->saJump[0])))
		ERR(retval);
	// Simulated Annealing Minimum Cost
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->saCostMinID, &Nc->saCostMin[0])))
		ERR(retval);
	// Total runtime
	if ((retval = nc_put_var_double(Nc->ncRetID, Nc->runtimeID, &Nc->runtime)))
		ERR(retval);
	// Close the file
    if ((retval = nc_close(Nc->ncRetID)))
       ERR(retval);
}

void writeNcForward(void)
{
	int retval;

	// Quality Control Flag
	if ((retval = nc_put_var_int(Nc->ncForwardID, Nc->flagQcForwardID, &Nc->flagQcForward[0])))
		ERR(retval)
	// Profile Type
	if ((retval = nc_put_var_int(Nc->ncForwardID, Nc->typePOMEID, &Nc->typePOME[0])))
		ERR(retval)
	// Surface SM Input
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->surPOMEID, &Nc->surPOME[0])))
		ERR(retval)
	// Bottom-most SM Input
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->bottPOMEID, &Nc->bottPOME[0])))
		ERR(retval)
	// Mean SM Input
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->avgPOMEID, &Nc->avgPOME[0])))
		ERR(retval)
	// Vegetation Optical Depth
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->VODHID, &Nc->VODH[0])))
		ERR(retval)
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->VODVID, &Nc->VODV[0])))
		ERR(retval)
	// Soil Depth POME model
	if ((retval = nc_put_var_float(Nc->ncForwardID, Nc->soilDepthPOMEID, &Nc->soilDepthPOME[0])))
		ERR(retval)
	// Soil Moisture Profile POME model
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->soilMoistPOMEID, &Nc->soilMoistPOME[0])))
		ERR(retval);
	// Penetration Depth
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->penDepthHID, &Nc->penDepthH[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->penDepthVID, &Nc->penDepthV[0])))
		ERR(retval);
	// Reflection Coefficient Co-pol
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefCoR_R_B_ID, &Nc->reflCoefCoR_R_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefCoI_R_B_ID, &Nc->reflCoefCoI_R_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefCoR_X_B_ID, &Nc->reflCoefCoR_X_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefCoI_X_B_ID, &Nc->reflCoefCoI_X_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefCoR_R_V_ID, &Nc->reflCoefCoR_R_V[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefCoI_R_V_ID, &Nc->reflCoefCoI_R_V[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefCoR_X_V_ID, &Nc->reflCoefCoR_X_V[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefCoI_X_V_ID, &Nc->reflCoefCoI_X_V[0])))
		ERR(retval);
	// Reflectivity Co-pol
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCo_R_B_ID, &Nc->reflCo_R_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCo_X_B_ID, &Nc->reflCo_X_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCo_R_V_ID, &Nc->reflCo_R_V[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCo_X_V_ID, &Nc->reflCo_X_V[0])))
		ERR(retval);

	// Reflection Coefficient X-pol
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefXR_R_B_ID, &Nc->reflCoefXR_R_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefXI_R_B_ID, &Nc->reflCoefXI_R_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefXR_X_B_ID, &Nc->reflCoefXR_X_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefXI_X_B_ID, &Nc->reflCoefXI_X_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefXR_R_V_ID, &Nc->reflCoefXR_R_V[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefXI_R_V_ID, &Nc->reflCoefXI_R_V[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefXR_X_V_ID, &Nc->reflCoefXR_X_V[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflCoefXI_X_V_ID, &Nc->reflCoefXI_X_V[0])))
		ERR(retval);
	// Reflectivity X-pol
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflX_R_B_ID, &Nc->reflX_R_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflX_X_B_ID, &Nc->reflX_X_B[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflX_R_V_ID, &Nc->reflX_R_V[0])))
		ERR(retval);
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->reflX_X_V_ID, &Nc->reflX_X_V[0])))
		ERR(retval);
	// Vegetation
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->argHrID, &Nc->argHr[0])))
		ERR(retval)
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->argHiID, &Nc->argHi[0])))
		ERR(retval)
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->argVrID, &Nc->argVr[0])))
		ERR(retval)
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->argViID, &Nc->argVi[0])))
		ERR(retval)
	if ((retval = nc_put_var_double(Nc->ncForwardID, Nc->VWCrefmodelID, &Nc->VWCrefmodel[0])))
		ERR(retval)
	/* Close the file. */
    if ((retval = nc_close(Nc->ncForwardID)))
       ERR(retval);
}

void createNcRetPeriod(long year, char *stationName, long period, long window)
{
	char fullpath[120];
	/* This will be the netCDF ID for the file and data variable. */
	int dimid1, dimid2, dimid3, dimids1[2], dimids2[2], dimidsingle;

	/* Loop indexes, and error handling. */
	int retval;

	int fillvalue_int = -9999;
	long fillvalue_long = -9999.0;
	double fillvalue_double = -9999.0;

	// Output file name
	sprintf(fullpath, "%sUSCRN_%ldM%ldM%ld_p%ldw%ldstd%.0lf_%s_%s_ret.nc", OutSubPath, year, Nc->startMonth, Nc->endMonth, period, window, Ret->stddev*100, Fixed[0].orbitName, stationName);

	/* Create the file. */
	if ((retval = nc_create(fullpath, NC_NETCDF4 | NC_SHARE, &Nc->ncRetID)))
		ERR(retval);

	/* Define the dimensions. */
	Nc->soopDim = NSoOp;
	Nc->sublayerDim = MultiLayer->NSublayer;

	if ((retval = nc_def_dim(Nc->ncRetID, SINGLE_DIM_NANE, 1, &dimidsingle)))
		ERR(retval);	

	if ((retval = nc_def_dim(Nc->ncRetID, SOOP_DIM_NAME, Nc->soopDim, &dimid1)))
		ERR(retval);
	
	if ((retval = nc_def_dim(Nc->ncRetID, RETRIEVAL_DIM_NAME, Nc->retDim, &dimid2)))
		ERR(retval);

	if ((retval = nc_def_dim(Nc->ncRetID, SUBLAYERS_DIM_NAME, Nc->sublayerDim, &dimid3)))
		ERR(retval);

	dimids1[0] = dimid2;
	dimids1[1] = dimid1;

	dimids2[0] = dimid2;
	dimids2[1] = dimid3;

	/* Define netCDF variables.*/
	if ((retval = nc_def_var(Nc->ncRetID, FLAGQCRET_VAR_NAME, NC_INT, 1, &dimid2, &Nc->flagQcRetID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->flagQcRetID, NC_FILL, &fillvalue_int)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, IND_PRODUCTTIME_VAR_NAME, NC_INT, 1, &dimid2, &Nc->iProductTimeID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->iProductTimeID, NC_FILL, &fillvalue_int)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, IND_OBSTIME_VAR_NAME, NC_INT, 2, dimids1, &Nc->iObsTimeID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->iObsTimeID, NC_FILL, &fillvalue_int)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, PENDEPTHHRET_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->penDepthHRetID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->penDepthHRetID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, PENDEPTHVRET_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->penDepthVRetID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->penDepthVRetID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	// Reflectivity X-pol, RHCP, Bare soil
	if ((retval = nc_def_var(Nc->ncRetID, REFLX_R_B_REF_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_R_B_RefID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_R_B_RefID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLX_R_B_EST_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_R_B_EstID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_R_B_EstID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLX_R_B_ERR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_R_B_ErrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_R_B_ErrID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity X-pol, Linear, Bare soil
	if ((retval = nc_def_var(Nc->ncRetID, REFLX_X_B_REF_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_X_B_RefID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_X_B_RefID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLX_X_B_EST_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_X_B_EstID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_X_B_EstID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLX_X_B_ERR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_X_B_ErrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_X_B_ErrID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity Co-pol, RHCP, Bare soil
	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_R_B_REF_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_R_B_RefID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_R_B_RefID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_R_B_EST_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_R_B_EstID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_R_B_EstID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_R_B_ERR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_R_B_ErrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_R_B_ErrID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity Co-pol, Linear, Bare soil
	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_X_B_REF_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_X_B_RefID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_X_B_RefID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_X_B_EST_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_X_B_EstID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_X_B_EstID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_X_B_ERR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_X_B_ErrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_X_B_ErrID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	// Refleictivty X-pol, RHCP, Vegetation
	if ((retval = nc_def_var(Nc->ncRetID, REFLX_R_V_REF_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_R_V_RefID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_R_V_RefID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLX_R_V_EST_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_R_V_EstID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_R_V_EstID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLX_R_V_ERR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_R_V_ErrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_R_V_ErrID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflecitvity X-pol, Linear, Vegetation
	if ((retval = nc_def_var(Nc->ncRetID, REFLX_X_V_REF_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_X_V_RefID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_X_V_RefID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLX_X_V_EST_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_X_V_EstID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_X_V_EstID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLX_X_V_ERR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_X_V_ErrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflX_X_V_ErrID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity Co-pol, RHCP, Vegetation
	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_R_V_REF_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_R_V_RefID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_R_V_RefID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_R_V_EST_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_R_V_EstID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_R_V_EstID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_R_V_ERR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_R_V_ErrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_R_V_ErrID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflecitivity Co-pol, Linear, Vegetation
	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_X_V_REF_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_X_V_RefID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_X_V_RefID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_X_V_EST_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_X_V_EstID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_X_V_EstID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLCO_X_V_ERR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_X_V_ErrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflCo_X_V_ErrID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	// Retrieved Soil Moisture
	if ((retval = nc_def_var(Nc->ncRetID, SOILMOISTRET_VAR_NAME, NC_DOUBLE, 2, dimids2, &Nc->soilMoistRetID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->soilMoistRetID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, TYPEPOME_VAR_NAME, NC_INT, 1, &dimid2, &Nc->typePOMERetID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->typePOMERetID, NC_FILL, &fillvalue_int)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, SURPOMERET_VAR_NAME, NC_DOUBLE, 1, &dimid2, &Nc->surPOMERetID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->surPOMERetID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, BOTTPOMERET_VAR_NAME, NC_DOUBLE, 1, &dimid2, &Nc->bottPOMERetID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->bottPOMERetID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, AVGPOMERET_VAR_NAME, NC_DOUBLE, 1, &dimid2, &Nc->avgPOMERetID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->avgPOMERetID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, VWCRET_VAR_NAME, NC_DOUBLE, 1, &dimid2, &Nc->VWCRetID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->VWCRetID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, SA_TIME_VAR_NAME, NC_DOUBLE, 1, &dimid2, &Nc->saTimeID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->saTimeID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, SA_ITER_VAR_NAME, NC_LONG, 1, &dimid2, &Nc->saIterID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->saIterID, NC_FILL, &fillvalue_long)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, SA_JUMP_VAR_NAME, NC_LONG, 1, &dimid2, &Nc->saJumpID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->saJumpID, NC_FILL, &fillvalue_long)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, SA_COSTMIN_VAR_NAME, NC_DOUBLE, 1, &dimid2, &Nc->saCostMinID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->saCostMinID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, RUNTIME, NC_DOUBLE, 1, &dimidsingle, &Nc->runtimeID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->runtimeID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncRetID, REFLERR_VAR_NAME, NC_DOUBLE, 1, &dimidsingle, &Nc->reflErrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncRetID, Nc->reflErrID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	/* End define mode. */
	if ((retval = nc_enddef(Nc->ncRetID)))
		ERR(retval);

	Nc->flagQcRet = (int *) realloc(Nc->flagQcRet, sizeof(int)*Nc->retDim);
	if (Nc->flagQcRet == NULL) {
		printf("Nc->flagQcRet realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->iProductTime = (int *) realloc(Nc->iProductTime, sizeof(int)*Nc->retDim);
	if (Nc->iProductTime == NULL) {
		printf("Nc->iProductTime realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->iObsTime = (int *) realloc(Nc->iObsTime, sizeof(int)*Nc->retDim*Nc->soopDim);
	if (Nc->iObsTime == NULL) {
		printf("Nc->iObsTime realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->penDepthHRet = (double *) realloc(Nc->penDepthHRet, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->penDepthHRet == NULL) {
		printf("Nc->penDepthHRet realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->penDepthVRet = (double *) realloc(Nc->penDepthVRet, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->penDepthVRet == NULL) {
		printf("Nc->penDepthVRet realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	// Reflectivity X-pol, RHCP, Bare soil
	Nc->reflX_R_B_Ref = (double *) realloc(Nc->reflX_R_B_Ref, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_R_B_Ref == NULL) {
		printf("Nc->reflX_R_B_Ref realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_R_B_Est = (double *) realloc(Nc->reflX_R_B_Est, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_R_B_Est == NULL) {
		printf("Nc->reflX_R_B_Est realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_R_B_Err = (double *) realloc(Nc->reflX_R_B_Err, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_R_B_Err == NULL) {
		printf("Nc->reflX_R_B_Err realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity Co-pol, RHCP, Bare soil
	Nc->reflCo_R_B_Ref = (double *) realloc(Nc->reflCo_R_B_Ref, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_R_B_Ref == NULL) {
		printf("Nc->reflCo_R_B_Ref realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_R_B_Est = (double *) realloc(Nc->reflCo_R_B_Est, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_R_B_Est == NULL) {
		printf("Nc->reflCo_R_B_Est realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_R_B_Err = (double *) realloc(Nc->reflCo_R_B_Err, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_R_B_Err == NULL) {
		printf("Nc->reflCo_R_B_Err realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	// Reflectivity X-pol, Linear, Bare soil
	Nc->reflX_X_B_Ref = (double *) realloc(Nc->reflX_X_B_Ref, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_X_B_Ref == NULL) {
		printf("Nc->reflX_X_B_Ref realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_X_B_Est = (double *) realloc(Nc->reflX_X_B_Est, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_X_B_Est == NULL) {
		printf("Nc->reflX_X_B_Est realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_X_B_Err = (double *) realloc(Nc->reflX_X_B_Err, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_X_B_Err == NULL) {
		printf("Nc->reflX_X_B_Err realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity Co-pol, Linear, Bare soil
	Nc->reflCo_X_B_Ref = (double *) realloc(Nc->reflCo_X_B_Ref, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_X_B_Ref == NULL) {
		printf("Nc->reflCo_X_B_Ref realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_X_B_Est = (double *) realloc(Nc->reflCo_X_B_Est, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_X_B_Est == NULL) {
		printf("Nc->reflCo_X_B_Est realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_X_B_Err = (double *) realloc(Nc->reflCo_X_B_Err, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_X_B_Err == NULL) {
		printf("Nc->reflCo_X_B_Err realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	// Reflectivity X-pol, RHCP, Vegetation
	Nc->reflX_R_V_Ref = (double *) realloc(Nc->reflX_R_V_Ref, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_R_V_Ref == NULL) {
		printf("Nc->reflX_R_V_Ref realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_R_V_Est = (double *) realloc(Nc->reflX_R_V_Est, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_R_V_Est == NULL) {
		printf("Nc->reflX_R_V_Est realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_R_V_Err = (double *) realloc(Nc->reflX_R_V_Err, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_R_V_Err == NULL) {
		printf("Nc->reflX_R_V_Err realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity Co-pol, RHCP, Vegetation
	Nc->reflCo_R_V_Ref = (double *) realloc(Nc->reflCo_R_V_Ref, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_R_V_Ref == NULL) {
		printf("Nc->reflCo_R_V_Ref realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_R_V_Est = (double *) realloc(Nc->reflCo_R_V_Est, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_R_V_Est == NULL) {
		printf("Nc->reflCo_R_V_Est realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_R_V_Err = (double *) realloc(Nc->reflCo_R_V_Err, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_R_V_Err == NULL) {
		printf("Nc->reflCo_R_V_Err realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	// Reflectivity X-pol, Linear, Vegetation
	Nc->reflX_X_V_Ref = (double *) realloc(Nc->reflX_X_V_Ref, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_X_V_Ref == NULL) {
		printf("Nc->reflX_X_V_Ref realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_X_V_Est = (double *) realloc(Nc->reflX_X_V_Est, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_X_V_Est == NULL) {
		printf("Nc->reflX_X_V_Est realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_X_V_Err = (double *) realloc(Nc->reflX_X_V_Err, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflX_X_V_Err == NULL) {
		printf("Nc->reflX_X_V_Err realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity Co-pol, Linear, Vegetation
	Nc->reflCo_X_V_Ref = (double *) realloc(Nc->reflCo_X_V_Ref, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_X_V_Ref == NULL) {
		printf("Nc->reflCo_X_V_Ref realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_X_V_Est = (double *) realloc(Nc->reflCo_X_V_Est, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_X_V_Est == NULL) {
		printf("Nc->reflCo_X_V_Est realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_X_V_Err = (double *) realloc(Nc->reflCo_X_V_Err, sizeof(double)*Nc->retDim*Nc->soopDim);
	if (Nc->reflCo_X_V_Err == NULL) {
		printf("Nc->reflCo_X_V_Err realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	// Soil Moisture
	Nc->soilMoistRet = (double *) realloc(Nc->soilMoistRet, sizeof(double)*Nc->retDim*Nc->sublayerDim);
	if (Nc->soilMoistRet == NULL) {
		printf("Nc->soilMoistRet realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	
	Nc->typePOMERet = (int *) realloc(Nc->typePOMERet, sizeof(int)*Nc->retDim);
	if (Nc->typePOMERet == NULL) {
		printf("Nc->typePOMERet realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->surPOMERet = (double *) realloc(Nc->surPOMERet, sizeof(double)*Nc->retDim);
	if (Nc->surPOMERet == NULL) {
		printf("Nc->surPOMERet realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->bottPOMERet = (double *) realloc(Nc->bottPOMERet, sizeof(double)*Nc->retDim);
	if (Nc->bottPOMERet == NULL) {
		printf("Nc->bottPOMERet realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->avgPOMERet = (double *) realloc(Nc->avgPOMERet, sizeof(double)*Nc->retDim);
	if (Nc->avgPOMERet == NULL) {
		printf("Nc->avgPOMERet realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->VWCRet = (double *) realloc(Nc->VWCRet, sizeof(double)*Nc->retDim);
	if (Nc->VWCRet == NULL) {
		printf("Nc->VWCRet realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->saTime = (double *) realloc(Nc->saTime, sizeof(double)*Nc->retDim);
	if (Nc->saTime == NULL) {
		printf("Nc->saTime realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->saIter = (long *) realloc(Nc->saIter, sizeof(long)*Nc->retDim);
	if (Nc->saIter == NULL) {
		printf("Nc->saIter realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->saJump = (long *) realloc(Nc->saJump, sizeof(long)*Nc->retDim);
	if (Nc->saJump == NULL) {
		printf("Nc->saJump realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->saCostMin = (double *) realloc(Nc->saCostMin, sizeof(double)*Nc->retDim);
	if (Nc->saCostMin == NULL) {
		printf("Nc->saCostMin realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
}

void createNcForward(long year, char *stationName)
{
	char fullpath[90];
	/* This will be the netCDF ID for the file and data variable. */
	int dimid1, dimid2, dimid3, dimid4, dimids1[2], dimids2[2], dimids3[2];

	/* Loop indexes, and error handling. */
	int retval;

	int fillvalue_int = -9999;
	float fillvalue_float = -9999.0;
	double fillvalue_double = -9999.0;

	// Output file name
	sprintf(fullpath, "%sUSCRN_hourly_%ldM%ldM%ld_%s_%s_forward.nc", OutPath, year, Nc->startMonth, Nc->endMonth, Fixed[0].orbitName, stationName);

	/* Create the file. */
	if ((retval = nc_create(fullpath, NC_NETCDF4, &Nc->ncForwardID)))
		ERR(retval);

	/* Define the dimensions. */	
	Nc->soopDim = NSoOp;
	Nc->sublayerDim = MultiLayer->NSublayer;

	if ((retval = nc_def_dim(Nc->ncForwardID, SOOP_DIM_NAME, Nc->soopDim, &dimid1)))
		ERR(retval);
	
	if ((retval = nc_def_dim(Nc->ncForwardID, SAMPLES_DIM_NAME, Nc->sampleDim, &dimid2)))
		ERR(retval);

	if ((retval = nc_def_dim(Nc->ncForwardID, SUBLAYERS_DIM_NAME, Nc->sublayerDim, &dimid3)))
		ERR(retval);

	if ((retval = nc_def_dim(Nc->ncForwardID, VEGS_DIM_NAME, Nc->vegDim, &dimid4)))
		ERR(retval);


	dimids1[0] = dimid1;
	dimids1[1] = dimid2;

	dimids2[0] = dimid3;
	dimids2[1] = dimid2;
		
	dimids3[0] = dimid4; // veg
	dimids3[1] = dimid2; // sample

	/* Define netCDF variables.*/
	if ((retval = nc_def_var(Nc->ncForwardID, FLAGQCFORWARD_VAR_NAME, NC_INT, 1, &dimid2, &Nc->flagQcForwardID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->flagQcForwardID, NC_FILL, &fillvalue_int)))
		ERR(retval);
	
	if ((retval = nc_def_var(Nc->ncForwardID, TYPEPOME_VAR_NAME, NC_INT, 1, &dimid2, &Nc->typePOMEID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->typePOMEID, NC_FILL, &fillvalue_int))) 
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, SURPOME_VAR_NAME, NC_DOUBLE, 1, &dimid2, &Nc->surPOMEID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->surPOMEID, NC_FILL, &fillvalue_double))) 
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, BOTTPOME_VAR_NAME, NC_DOUBLE, 1, &dimid2, &Nc->bottPOMEID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->bottPOMEID, NC_FILL, &fillvalue_double))) 
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, AVGPOME_VAR_NAME, NC_DOUBLE, 1, &dimid2, &Nc->avgPOMEID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->avgPOMEID, NC_FILL, &fillvalue_double))) 
		ERR(retval);;

	if ((retval = nc_def_var(Nc->ncForwardID, VODH_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->VODHID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->VODHID, NC_FILL, &fillvalue_double))) 
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, VODV_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->VODVID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->VODVID, NC_FILL, &fillvalue_double))) 
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, PENDEPTHH_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->penDepthHID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->penDepthHID, NC_FILL, &fillvalue_double))) 
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, PENDEPTHV_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->penDepthVID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->penDepthVID, NC_FILL, &fillvalue_double))) 
		ERR(retval);
	// Reflection Coefficient Bare soil
	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFCOR_R_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefCoR_R_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefCoR_R_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFCOI_R_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefCoI_R_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefCoI_R_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFXR_R_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefXR_R_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefXR_R_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFXI_R_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefXI_R_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefXI_R_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);
		
	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFCOR_X_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefCoR_X_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefCoR_X_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFCOI_X_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefCoI_X_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefCoI_X_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFXR_X_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefXR_X_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefXR_X_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFXI_X_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefXI_X_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefXI_X_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	
	// Reflection Coefficient, Vegetation
	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFCOR_R_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefCoR_R_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefCoR_R_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFCOI_R_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefCoI_R_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefCoI_R_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFXR_R_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefXR_R_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefXR_R_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFXI_R_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefXI_R_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefXI_R_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFCOR_X_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefCoR_X_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefCoR_X_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFCOI_X_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefCoI_X_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefCoI_X_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFXR_X_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefXR_X_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefXR_X_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, REFLCOEFXI_X_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCoefXI_X_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCoefXI_X_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	// Reflectivity X-pol, RHCP, Bare soil
	if ((retval = nc_def_var(Nc->ncForwardID, REFLX_R_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_R_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflX_R_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity Co-pol, RHCP, Bare soil
	if ((retval = nc_def_var(Nc->ncForwardID, REFLCO_R_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_R_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCo_R_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity X-pol, Linear, Bare soil
	if ((retval = nc_def_var(Nc->ncForwardID, REFLX_X_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_X_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflX_X_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity Co-pol, Linear, Bare soil
	if ((retval = nc_def_var(Nc->ncForwardID, REFLCO_X_B_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_X_B_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCo_X_B_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity X-pol, RHCP, Vegetation
	if ((retval = nc_def_var(Nc->ncForwardID, REFLX_R_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_R_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflX_R_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity Co-pol, RHCP, Vegetation
	if ((retval = nc_def_var(Nc->ncForwardID, REFLCO_R_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_R_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCo_R_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity X-pol, Linear, Vegetation
	if ((retval = nc_def_var(Nc->ncForwardID, REFLX_X_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflX_X_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflX_X_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// Reflectivity Co-pol, Linear, Vegetation
	if ((retval = nc_def_var(Nc->ncForwardID, REFLCO_X_V_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->reflCo_X_V_ID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->reflCo_X_V_ID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, SOILDEPTHPOME_VAR_NAME, NC_FLOAT, 1, &dimid3, &Nc->soilDepthPOMEID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->soilDepthPOMEID, NC_FILL, &fillvalue_float)))
		ERR(retval);
			
	if ((retval = nc_def_var(Nc->ncForwardID, SOILMOISTPOME_VAR_NAME, NC_DOUBLE, 2, dimids2, &Nc->soilMoistPOMEID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->soilMoistPOMEID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	// Vegetation arguments
	if ((retval = nc_def_var(Nc->ncForwardID, ARGHR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->argHrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->argHrID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, ARGHI_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->argHiID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->argHiID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, ARGVR_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->argVrID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->argVrID, NC_FILL, &fillvalue_double)))
		ERR(retval);

	if ((retval = nc_def_var(Nc->ncForwardID, ARGVI_VAR_NAME, NC_DOUBLE, 2, dimids1, &Nc->argViID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->argViID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	// VWC
	if ((retval = nc_def_var(Nc->ncForwardID, VWCREFMODEL_VAR_NAME, NC_DOUBLE, 2, dimids3, &Nc->VWCrefmodelID)))
		ERR(retval);
	if ((retval = nc_def_var_fill(Nc->ncForwardID, Nc->VWCrefmodelID, NC_FILL, &fillvalue_double)))
		ERR(retval);
	/* End define mode. */
	if ((retval = nc_enddef(Nc->ncForwardID)))
		ERR(retval);

	Nc->flagQcForward = (int *) realloc(Nc->flagQcForward, sizeof(int)*Nc->sampleDim);
	if (Nc->flagQcForward == NULL) {
		printf("Nc->flagQcForward realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}	
	Nc->typePOME = (int *) realloc(Nc->typePOME, sizeof(int)*Nc->sampleDim);
	if (Nc->typePOME == NULL) {
		printf("Nc->typePOME realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->surPOME = (double *) realloc(Nc->surPOME, sizeof(double)*Nc->sampleDim);
	if (Nc->surPOME == NULL) {
		printf("Nc->surPOME realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->bottPOME = (double *) realloc(Nc->bottPOME, sizeof(double)*Nc->sampleDim);
	if (Nc->bottPOME == NULL) {
		printf("Nc->bottPOME realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->avgPOME = (double *) realloc(Nc->avgPOME, sizeof(double)*Nc->sampleDim);
	if (Nc->avgPOME == NULL) {
		printf("Nc->avgPOME realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->VODH = (double *) realloc(Nc->VODH, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->VODH == NULL) {
		printf("Nc->VODH realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->VODV = (double *) realloc(Nc->VODV, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->VODV == NULL) {
		printf("Nc->VODV realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->soilDepthPOME = (float *) realloc(Nc->soilDepthPOME, sizeof(float)*Nc->sublayerDim);
	if (Nc->soilDepthPOME == NULL) {
		printf("Nc->soilDepthPOME realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->penDepthH = (double *) realloc(Nc->penDepthH, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->penDepthH == NULL) {
		printf("Nc->penDepthH realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->penDepthV = (double *) realloc(Nc->penDepthV, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->penDepthV == NULL) {
		printf("Nc->penDepthV realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflection Coefficient, Bare soil
	Nc->reflCoefCoR_R_B = (double *) realloc(Nc->reflCoefCoR_R_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefCoR_R_B == NULL) {
		printf("Nc->reflCoefCoR_R_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefCoI_R_B = (double *) realloc(Nc->reflCoefCoI_R_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefCoI_R_B == NULL) {
		printf("Nc->reflCoefCoI_R_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefXR_R_B = (double *) realloc(Nc->reflCoefXR_R_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefXR_R_B == NULL) {
		printf("Nc->reflCoefXR_R_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefXI_R_B = (double *) realloc(Nc->reflCoefXI_R_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefXI_R_B == NULL) {
		printf("Nc->reflCoefXI_R_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefCoR_X_B = (double *) realloc(Nc->reflCoefCoR_X_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefCoR_X_B == NULL) {
		printf("Nc->reflCoefCoR_X_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefCoI_X_B = (double *) realloc(Nc->reflCoefCoI_X_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefCoI_X_B == NULL) {
		printf("Nc->reflCoefCoI_X_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefXR_X_B = (double *) realloc(Nc->reflCoefXR_X_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefXR_X_B == NULL) {
		printf("Nc->reflCoefXR_X_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefXI_X_B = (double *) realloc(Nc->reflCoefXI_X_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefXI_X_B == NULL) {
		printf("Nc->reflCoefXI_X_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflection Coefficient, Vegetation
	Nc->reflCoefCoR_R_V = (double *) realloc(Nc->reflCoefCoR_R_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefCoR_R_V == NULL) {
		printf("Nc->reflCoefCoR_R_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefCoI_R_V = (double *) realloc(Nc->reflCoefCoI_R_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefCoI_R_V == NULL) {
		printf("Nc->reflCoefCoI_R_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefXR_R_V = (double *) realloc(Nc->reflCoefXR_R_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefXR_R_V == NULL) {
		printf("Nc->reflCoefXR_R_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefXI_R_V = (double *) realloc(Nc->reflCoefXI_R_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefXI_R_V == NULL) {
		printf("Nc->reflCoefXI_R_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefCoR_X_V = (double *) realloc(Nc->reflCoefCoR_X_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefCoR_X_V == NULL) {
		printf("Nc->reflCoefCoR_X_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefCoI_X_V = (double *) realloc(Nc->reflCoefCoI_X_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefCoI_X_V == NULL) {
		printf("Nc->reflCoefCoI_X_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefXR_X_V = (double *) realloc(Nc->reflCoefXR_X_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefXR_X_V == NULL) {
		printf("Nc->reflCoefXR_X_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Nc->reflCoefXI_X_V = (double *) realloc(Nc->reflCoefXI_X_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCoefXI_X_V == NULL) {
		printf("Nc->reflCoefXI_X_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity X-pol, RHCP, Bare soil
	Nc->reflX_R_B = (double *) realloc(Nc->reflX_R_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflX_R_B == NULL) {
		printf("Nc->reflX_R_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity Co-pol, RHCP, Bare soil
	Nc->reflCo_R_B = (double *) realloc(Nc->reflCo_R_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCo_R_B == NULL) {
		printf("Nc->reflCo_R_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity X-pol, Linear, Bare soil
	Nc->reflX_X_B = (double *) realloc(Nc->reflX_X_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflX_X_B == NULL) {
		printf("Nc->reflX_X_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity Co-pol, Linear, Bare soil
	Nc->reflCo_X_B = (double *) realloc(Nc->reflCo_X_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCo_X_B == NULL) {
		printf("Nc->reflCo_X_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity X-pol, RHCP, Vegetation
	Nc->reflX_R_V = (double *) realloc(Nc->reflX_R_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflX_R_V == NULL) {
		printf("Nc->reflX_R_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity Co-pol, RHCP, Vegetation
	Nc->reflCo_R_V = (double *) realloc(Nc->reflCo_R_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCo_R_V == NULL) {
		printf("Nc->reflCo_R_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity X-pol, Linear, Vegetation
	Nc->reflX_X_V = (double *) realloc(Nc->reflX_X_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflX_X_V == NULL) {
		printf("Nc->reflX_X_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	// Reflectivity Co-pol, Linear, Vegetation
	Nc->reflCo_X_V = (double *) realloc(Nc->reflCo_X_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCo_X_V == NULL) {
		printf("Nc->reflCo_X_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->soilMoistPOME = (double *) realloc(Nc->soilMoistPOME, sizeof(double)*Nc->sampleDim*Nc->sublayerDim);
	if (Nc->soilMoistPOME == NULL) {
		printf("Nc->soilMoistPOME realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->argHr = (double *) realloc(Nc->argHr, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->argHr == NULL) {
		printf("Nc->argHr realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->argHi = (double *) realloc(Nc->argHi, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->argHi == NULL) {
		printf("Nc->argHi realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->argVr = (double *) realloc(Nc->argVr, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->argVr == NULL) {
		printf("Nc->argVr realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->argVi = (double *) realloc(Nc->argVi, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->argVi == NULL) {
		printf("Nc->argVi realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->VWCrefmodel = (double *) realloc(Nc->VWCrefmodel, sizeof(double)*Nc->sampleDim*Nc->vegDim);
	if (Nc->VWCrefmodel == NULL) {
		printf("Nc->VWCrefmodel realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
}

void updateNcVeg(long iSample)
{
	long iVeg, iKind, jKind, iLayer, iType, iSoOp;
	long part_type, part_kind;
	double freq, VWC_layer;

	VegData[0].thickness[0] = Nc->vegVar[iSample + Nc->sampleDim*5];

	for(iVeg=0; iVeg<NVeg; iVeg++){
		// Reset parameters
		VegData[iVeg].depth = 0;

		for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
			VegData[iVeg].depth += VegData[iVeg].thickness[iLayer];
		}

		// Find layer indices for each kind
		for(iKind=0; iKind<NKind; iKind++){
			VegData[iVeg].typkndThick[iKind] = 0;
			for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
				for(jKind=0; jKind<VegData[iVeg].nKind[iLayer]; jKind++){
					if(!strcmp(VegKind[iKind].ID, VegData[iVeg].kind[iLayer][jKind])){
						VegData[iVeg].typkndThick[iKind] += VegData[iVeg].thickness[iLayer];
					}
				}
			}
		}

		for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
			part_kind = 0;
			for(jKind=0; jKind<NKind; jKind++){
				if(jKind>0 && (VegKind[jKind].ID[0] != VegKind[jKind-1].ID[0]))
					part_kind = 0;
				for(iKind=0; iKind<VegData[iVeg].nKind[iLayer]; iKind++){
					if(!strcmp(VegKind[jKind].ID, VegData[iVeg].kind[iLayer][iKind])){	
						switch(VegData[iVeg].kind[iLayer][iKind][0]){
							case 'L':
								part_type = VegData[iVeg].part_type_idx[0];
								break;
							case 'B':
								part_type = VegData[iVeg].part_type_idx[1];
								break;
							case 'T':
								part_type = VegData[iVeg].part_type_idx[2];
								break;
							case 'N':
								part_type = VegData[iVeg].part_type_idx[3];
								break;
							case 'W':
								part_type = VegData[iVeg].part_type_idx[4];
								break;
						}
						if(jKind == 0){
							// grass variable : density, dim1, dim2, dim3, m_g, + d
							VegKind[jKind].density = Nc->vegVar[iSample];
							VegKind[jKind].dim[0] = Nc->vegVar[iSample + Nc->sampleDim];
							VegKind[jKind].dim[1] = Nc->vegVar[iSample + Nc->sampleDim*2];
							VegKind[jKind].dim[2] = Nc->vegVar[iSample + Nc->sampleDim*3];
							VegKind[jKind].mv = Nc->vegVar[iSample + Nc->sampleDim*4];
							for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
                                freq = Fixed[iSoOp].Freq;
								VegKind[jKind].e_c[iSoOp] = calcDielUlabyElRayesMv(freq, VegKind[jKind].mv);
							}		
						}
						else if(jKind == 1){
							// thatch variable : none
							VegKind[jKind].density = Nc->vegVar[iSample + Nc->sampleDim*6];
							VegKind[jKind].dim[0] = Nc->vegVar[iSample + Nc->sampleDim*7];
							VegKind[jKind].dim[1] = Nc->vegVar[iSample + Nc->sampleDim*8];
							VegKind[jKind].dim[2] = Nc->vegVar[iSample + Nc->sampleDim*9];
						} 
						VegKind[jKind].VWC = VegKind[jKind].density * VegKind[jKind].mv * 1000 * Pi * VegKind[jKind].dim[0] * VegKind[jKind].dim[1] * VegKind[jKind].dim[2];
					
						VegData[iVeg].dsty[part_kind][part_type][iLayer] = VegKind[jKind].density * VegData[iVeg].depth / VegData[iVeg].typkndThick[jKind];	
						VegData[iVeg].dim1[part_kind][part_type][iLayer] = VegKind[jKind].dim[0];	
						VegData[iVeg].dim2[part_kind][part_type][iLayer] = VegKind[jKind].dim[1];	
						VegData[iVeg].dim3[part_kind][part_type][iLayer] = VegKind[jKind].dim[2];
						VegData[iVeg].beginAng[part_kind][part_type][iLayer] = VegKind[jKind].beginAng;
						VegData[iVeg].endAng[part_kind][part_type][iLayer] = VegKind[jKind].endAng;
						VegData[iVeg].VWC[part_kind][part_type][iLayer] = VegKind[jKind].VWC * VegData[iVeg].typkndThick[jKind];
						for(iSoOp=0; iSoOp<NSoOp; iSoOp++)
							VegData[iVeg].e_c[part_kind][part_type][iLayer][iSoOp] = VegKind[jKind].e_c[iSoOp];
						part_kind++;
						break;
					}
				}	
			}
		}		

		for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
			calcPropagationNetCDF(iSoOp, iVeg, iSample);
		}
		VegData[iVeg].VWC_total = 0;
		for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
			VWC_layer = 0;
			for(iType=0; iType<VegData[iVeg].nTypeMax; iType++){
				for(iKind=0; iKind<VegData[iVeg].nKindMax; iKind++){
					VWC_layer += VegData[iVeg].VWC[iKind][iType][iLayer];
					VegData[iVeg].VWC_total += VegData[iVeg].VWC[iKind][iType][iLayer];
				}
			}
			Nc->VWCrefmodel[iLayer*Nc->sampleDim + iSample] = VWC_layer;
		}
	}
}

void readNcForward(long year, char *stationName)
{
	char fullpath[80];
	/* This will be the netCDF ID for the file and data variable. */
	int ncid, dimid;

	/* Loop indexes, and error handling. */
	int retval;
				
	// Input file name
	sprintf(fullpath, "%sUSCRN_hourly_%ldM%ldM%ld_%s_%s_forward.nc", OutPath, year, Nc->startMonth, Nc->endMonth, Fixed[0].orbitName, stationName);

	/* Open the file. NC_NOWRITE tells netCDF we want read-only access
	* to the file.*/
	if ((retval = nc_open(fullpath, NC_NOWRITE, &ncid)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, SAMPLES_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->sampleDim)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, SOOP_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->soopDim)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, SUBLAYERS_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->sublayerDim)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, VEGS_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->vegDim)))
		ERR(retval);

	Nc->flagQcForward = (int *) realloc(Nc->flagQcForward, sizeof(int)*Nc->sampleDim);
	if (Nc->flagQcForward == NULL) {
		printf("Nc->flagQcForward realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->surPOME = (double *) realloc(Nc->surPOME, sizeof(double)*Nc->sampleDim);
	if (Nc->surPOME == NULL) {
		printf("Nc->surPOME realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->bottPOME = (double *) realloc(Nc->bottPOME, sizeof(double)*Nc->sampleDim);
	if (Nc->bottPOME == NULL) {
		printf("Nc->bottPOME realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->avgPOME = (double *) realloc(Nc->avgPOME, sizeof(double)*Nc->sampleDim);
	if (Nc->avgPOME == NULL) {
		printf("Nc->avgPOME realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->VODH = (double *) realloc(Nc->VODH, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->VODH == NULL) {
		printf("Nc->VODH realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->VODV = (double *) realloc(Nc->VODV, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->VODV == NULL) {
		printf("Nc->VODV realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_R_B = (double *) realloc(Nc->reflX_R_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflX_R_B == NULL) {
		printf("Nc->reflX_R_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_R_B = (double *) realloc(Nc->reflCo_R_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCo_R_B == NULL) {
		printf("Nc->reflCo_R_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_X_B = (double *) realloc(Nc->reflX_X_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflX_X_B == NULL) {
		printf("Nc->reflX_X_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_X_B = (double *) realloc(Nc->reflCo_X_B, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCo_X_B == NULL) {
		printf("Nc->reflCo_X_B realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_R_V = (double *) realloc(Nc->reflX_R_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflX_R_V == NULL) {
		printf("Nc->reflX_R_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_R_V = (double *) realloc(Nc->reflCo_R_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCo_R_V == NULL) {
		printf("Nc->reflCo_R_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflX_X_V = (double *) realloc(Nc->reflX_X_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflX_X_V == NULL) {
		printf("Nc->reflX_X_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->reflCo_X_V = (double *) realloc(Nc->reflCo_X_V, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->reflCo_X_V == NULL) {
		printf("Nc->reflCo_X_V realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->argHr = (double *) realloc(Nc->argHr, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->argHr == NULL) {
		printf("Nc->argHr realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->argHi = (double *) realloc(Nc->argHi, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->argHi == NULL) {
		printf("Nc->argHi realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->argVr = (double *) realloc(Nc->argVr, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->argVr == NULL) {
		printf("Nc->argVr realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->argVi = (double *) realloc(Nc->argVi, sizeof(double)*Nc->sampleDim*Nc->soopDim);
	if (Nc->argVi == NULL) {
		printf("Nc->argVi realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->VWCrefmodel = (double *) realloc(Nc->VWCrefmodel, sizeof(double)*Nc->sampleDim*Nc->vegDim);
	if (Nc->VWCrefmodel == NULL) {
		printf("Nc->VWCrefmodel realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	// Quality Control Flag
	// Get the varid of the data variable, based on its name and read the data 
	if ((retval = nc_inq_varid(ncid, FLAGQCFORWARD_VAR_NAME, &Nc->flagQcForwardID)))
		ERR(retval);
	if ((retval = nc_get_var_int(ncid, Nc->flagQcForwardID, &Nc->flagQcForward[0])))
		ERR(retval);
	
	// Inputs for POME model
	if ((retval = nc_inq_varid(ncid, SURPOME_VAR_NAME, &Nc->surPOMEID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->surPOMEID, &Nc->surPOME[0])))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, BOTTPOME_VAR_NAME, &Nc->bottPOMEID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->bottPOMEID, &Nc->bottPOME[0])))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, AVGPOME_VAR_NAME, &Nc->avgPOMEID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->avgPOMEID, &Nc->avgPOME[0])))
		ERR(retval);
	// Vegetation Optical Depth
	if ((retval = nc_inq_varid(ncid, VODH_VAR_NAME, &Nc->VODHID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->VODHID, &Nc->VODH[0])))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, VODV_VAR_NAME, &Nc->VODVID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->VODVID, &Nc->VODV[0])))
		ERR(retval);
	// R-pol
	// Get the varid of the data variable, based on its name and read the data 
	if ((retval = nc_inq_varid(ncid, REFLX_R_B_VAR_NAME, &Nc->reflX_R_B_ID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->reflX_R_B_ID, &Nc->reflX_R_B[0])))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, REFLCO_R_B_VAR_NAME, &Nc->reflCo_R_B_ID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->reflCo_R_B_ID, &Nc->reflCo_R_B[0])))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, REFLX_R_V_VAR_NAME, &Nc->reflX_R_V_ID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->reflX_R_V_ID, &Nc->reflX_R_V[0])))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, REFLCO_R_V_VAR_NAME, &Nc->reflCo_R_V_ID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->reflCo_R_V_ID, &Nc->reflCo_R_V[0])))
		ERR(retval);

	// X-pol
	// Get the varid of the data variable, based on its name and read the data 
	if ((retval = nc_inq_varid(ncid, REFLX_X_B_VAR_NAME, &Nc->reflX_X_B_ID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->reflX_X_B_ID, &Nc->reflX_X_B[0])))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, REFLCO_X_B_VAR_NAME, &Nc->reflCo_X_B_ID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->reflCo_X_B_ID, &Nc->reflCo_X_B[0])))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, REFLX_X_V_VAR_NAME, &Nc->reflX_X_V_ID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->reflX_X_V_ID, &Nc->reflX_X_V[0])))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, REFLCO_X_V_VAR_NAME, &Nc->reflCo_X_V_ID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->reflCo_X_V_ID, &Nc->reflCo_X_V[0])))
		ERR(retval);

	// Vegetation
	if ((retval = nc_inq_varid(ncid, ARGHR_VAR_NAME, &Nc->argHrID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->argHrID, &Nc->argHr[0])))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, ARGHI_VAR_NAME, &Nc->argHiID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->argHiID, &Nc->argHi[0])))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, ARGVR_VAR_NAME, &Nc->argVrID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->argVrID, &Nc->argVr[0])))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, ARGVI_VAR_NAME, &Nc->argViID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->argViID, &Nc->argVi[0])))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, VWCREFMODEL_VAR_NAME, &Nc->VWCrefmodelID)))
		ERR(retval);
	if ((retval = nc_get_var_double(ncid, Nc->VWCrefmodelID, &Nc->VWCrefmodel[0])))
		ERR(retval);

	/* Close the file, freeing all resources. */
	if ((retval = nc_close(ncid)))
		ERR(retval);	
}

void readNcUSCRN(long year, char *stationName)
{
	char fullpath[100];
	/* This will be the netCDF ID for the file and data variable. */
	int ncid, dimid;

	/* Loop indexes, and error handling. */
	int retval;
					
	Nc->soopDim = NSoOp;

	// Input file name
	sprintf(fullpath, "./data/processed/%ld/USCRN_hourly_%ldM%ldM%ld_%s_%s.nc", year, year, Nc->startMonth, Nc->endMonth, Fixed[0].orbitName, stationName);
	
	/* Open the file. NC_NOWRITE tells netCDF we want read-only access
	* to the file.*/
	if ((retval = nc_open(fullpath, NC_NOWRITE, &ncid)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, SAMPLES_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->sampleDim)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, LAYERS_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->layerDim)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, SOILSEPARATES_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->soilSeparateDim)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, CHAR_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->charDim)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, VEGS_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->vegDim)))
		ERR(retval);

	if ((retval = nc_inq_dimid(ncid, VEGVARS_DIM_NAME, &dimid)))
		ERR(retval);

	if ((retval = nc_inq_dimlen(ncid, dimid, &Nc->vegVarDim)))
		ERR(retval);

	Nc->utcDate = (long *) realloc(Nc->utcDate, sizeof(long)*Nc->sampleDim);
	if (Nc->utcDate == NULL) {
		printf("Nc->utcDate realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->utcTime = (long *) realloc(Nc->utcTime, sizeof(long)*Nc->sampleDim);
	if (Nc->utcTime == NULL) {
		printf("Nc->utcTime realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->soilDepth = (float *) realloc(Nc->soilDepth, sizeof(float)*Nc->layerDim);
	if (Nc->soilDepth == NULL) {
		printf("Nc->soilDepth realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->soilMoist = (float *) realloc(Nc->soilMoist, sizeof(float)*Nc->sampleDim*Nc->layerDim);
	if (Nc->soilMoist == NULL) {
		printf("Nc->soilMoist realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->soilTemp = (float *) realloc(Nc->soilTemp, sizeof(float)*Nc->sampleDim*Nc->layerDim);
	if (Nc->soilTemp == NULL) {
		printf("Nc->soilTemp realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->airTempMin = (float *) realloc(Nc->airTempMin, sizeof(float)*Nc->sampleDim);
	if (Nc->airTempMin == NULL) {
		printf("Nc->airTempMin realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->prec = (float *) realloc(Nc->prec, sizeof(float)*Nc->sampleDim);
	if (Nc->prec == NULL) {
		printf("Nc->prec realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->vegType = (char *) realloc(Nc->vegType, sizeof(char)*Nc->charDim);
	if (Nc->vegType == NULL) {
		printf("Nc->vegType realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->incMUOS = (double *) realloc(Nc->incMUOS, sizeof(double)*Nc->sampleDim);
	if (Nc->incMUOS == NULL) {
		printf("Nc->incMUOS realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->incORBCOMM = (double *) realloc(Nc->incORBCOMM, sizeof(double)*Nc->sampleDim);
	if (Nc->incORBCOMM == NULL) {
		printf("Nc->incORBCOMM realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->incGPS = (double *) realloc(Nc->incGPS, sizeof(double)*Nc->sampleDim);
	if (Nc->incGPS == NULL) {
		printf("Nc->incGPS realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->VWCref = (double *) realloc(Nc->VWCref, sizeof(double)*Nc->sampleDim*Nc->vegDim);
	if (Nc->VWCref == NULL) {
		printf("Nc->VWCref realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Nc->vegVar = (double *) realloc(Nc->vegVar, sizeof(double)*Nc->sampleDim*Nc->vegVarDim);
	if (Nc->vegVar == NULL) {
		printf("Nc->vegVar realloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	
	/* Get the varid of the data variable, based on its name. */
	if ((retval = nc_inq_varid(ncid, UTCDATE_VAR_NAME, &Nc->utcDateID)))
		ERR(retval);
	
	/* Read the data. */
	if ((retval = nc_get_var_long(ncid, Nc->utcDateID, &Nc->utcDate[0])))
		ERR(retval);

	/* Get the varid of the data variable, based on its name. */
	if ((retval = nc_inq_varid(ncid, UTCTIME_VAR_NAME, &Nc->utcTimeID)))
		ERR(retval);
	
	/* Read the data. */
	if ((retval = nc_get_var_long(ncid, Nc->utcTimeID, &Nc->utcTime[0])))
		ERR(retval);

	/* Get the varid of the data variable, based on its name. */
	if ((retval = nc_inq_varid(ncid, SOILDEPTH_VAR_NAME, &Nc->soilDepthID)))
		ERR(retval);
	
	/* Read the data. */
	if ((retval = nc_get_var_float(ncid, Nc->soilDepthID, &Nc->soilDepth[0])))
		ERR(retval);

	/* Get the varid of the data variable, based on its name. */
	if ((retval = nc_inq_varid(ncid, SOILTYPE_VAR_NAME, &Nc->soilTypeID)))
		ERR(retval);
	
	/* Read the data. */
	if ((retval = nc_get_var_float(ncid, Nc->soilTypeID, &Nc->soilType[0])))
		ERR(retval);

	/* Get the varid of the data variable, based on its name. */
	if ((retval = nc_inq_varid(ncid, SOILMOIST_VAR_NAME, &Nc->soilMoistID)))
		ERR(retval);
	
	/* Read the data. */
	if ((retval = nc_get_var_float(ncid, Nc->soilMoistID, &Nc->soilMoist[0])))
		ERR(retval);

	/* Get the varid of the data variable, based on its name. */
	if ((retval = nc_inq_varid(ncid, SOILTEMP_VAR_NAME, &Nc->soilTempID)))
		ERR(retval);
	
	/* Read the data. */
	if ((retval = nc_get_var_float(ncid, Nc->soilTempID, &Nc->soilTemp[0])))
		ERR(retval);

	/* Get the varid of the data variable, based on its name. */
	if ((retval = nc_inq_varid(ncid, AIRTEMPMIN_VAR_NAME, &Nc->airTempMinID)))
		ERR(retval);
	
	/* Read the data. */
	if ((retval = nc_get_var_float(ncid, Nc->airTempMinID, &Nc->airTempMin[0])))
		ERR(retval);

	/* Get the varid of the data variable, based on its name. */
	if ((retval = nc_inq_varid(ncid, PREC_VAR_NAME, &Nc->precID)))
		ERR(retval);
	
	/* Read the data. */
	if ((retval = nc_get_var_float(ncid, Nc->precID, &Nc->prec[0])))
		ERR(retval);

	/* Get the varid of the data variable, based on its name. */
	if ((retval = nc_inq_varid(ncid, VEGTYPE_VAR_NAME, &Nc->vegTypeID)))
		ERR(retval);
	
	/* Read the data. */
	if ((retval = nc_get_var_text(ncid, Nc->vegTypeID, &Nc->vegType[0])))
		ERR(retval);

	// Get the varid of the data variable, based on its name. 
	if ((retval = nc_inq_varid(ncid, INC_MUOS_VAR_NAME, &Nc->incMUOSID)))
		ERR(retval);
	
	// Read the data. 
	if ((retval = nc_get_var_double(ncid, Nc->incMUOSID, &Nc->incMUOS[0])))
		ERR(retval);

	// Get the varid of the data variable, based on its name. 
	if ((retval = nc_inq_varid(ncid, INC_ORBCOMM_VAR_NAME, &Nc->incORBCOMMID)))
		ERR(retval);
	
	// Read the data. 
	if ((retval = nc_get_var_double(ncid, Nc->incORBCOMMID, &Nc->incORBCOMM[0])))
		ERR(retval);

	// Get the varid of the data variable, based on its name. 
	if ((retval = nc_inq_varid(ncid, INC_GPS_VAR_NAME, &Nc->incGPSID)))
		ERR(retval);
	
	// Read the data. 
	if ((retval = nc_get_var_double(ncid, Nc->incGPSID, &Nc->incGPS[0])))
		ERR(retval);

	// Get the varid of the data variable, based on its name. 
	if ((retval = nc_inq_varid(ncid, VWCREF_VAR_NAME, &Nc->VWCrefID)))
		ERR(retval);
	
	// Read the data. 
	if ((retval = nc_get_var_double(ncid, Nc->VWCrefID, &Nc->VWCref[0])))
		ERR(retval);

	// Get the varid of the data variable, based on its name. 
	if ((retval = nc_inq_varid(ncid, VEGVAR_VAR_NAME, &Nc->vegVarID)))
		ERR(retval);
	
	// Read the data. 
	if ((retval = nc_get_var_double(ncid, Nc->vegVarID, &Nc->vegVar[0])))
		ERR(retval);

	/* Close the file, freeing all resources. */
	if ((retval = nc_close(ncid)))
		ERR(retval);	
}

// Generate synthetic reflectivity observations from processed USCRN netCDF data
void ObsFixedNetCDF(void)
{
	long iYear, iStation, iLayer, iSample, nStation, iSoOp, iOut, iPol, iVeg;
	FILE *stationFile;
	char stationName[30], newline;
	double sum, avg;
	struct FixedObsType *F;

    stationFile=FileOpen(SimPath, Nc->filename,"r");

    printf(">> Start generating synthetic observations...\n");
	printf(">> #SoOp = %ld, #Pol = %ld\n", NSoOp, NPol);
	for(iYear=Nc->startYear; iYear<Nc->endYear+1; iYear++){
		printf(">> Year %ld\n", iYear);
		fscanf(stationFile,"%ld %[\n]",&nStation, &newline);
		for(iStation=0; iStation<nStation; iStation++){
			// Read station name
			fscanf(stationFile, "%s %[\n]", stationName, &newline);
			printf(">> Station %ld : %s\n", iStation+1, stationName);
			
			// Read USCRN NetCDF
			readNcUSCRN(iYear, stationName);
			
			// Create Forward NetCDF
			createNcForward(iYear, stationName);

            // Total number of samples
			NData = Nc->sampleDim;

            // Soil depths
			for(iLayer=0; iLayer<Nc->sublayerDim; iLayer++){
				Nc->soilDepthPOME[iLayer] = iLayer*MultiLayer->delZ;
			}

			// Load Soil Texture
			Gnd->clay = Nc->soilType[0];
			Gnd->sand = Nc->soilType[2];

			// Get soil water characteristic based on soil fractions
			LookupSoiltype();

			// Run forward model
			for(iSample=0; iSample<Nc->sampleDim; iSample++){
				// Progress
				EndFlag = FALSE;
				updateProgress(iSample);

				// Load data and quality control
				Nc->flagQcForward[iSample] = QC_GOOD;
				for(iLayer=0; iLayer<NLayer; iLayer++){
					Gnd->VSM[iLayer] = Nc->soilMoist[iLayer*Nc->sampleDim + iSample];
					
					if(Gnd->VSM[iLayer] == FILLVALUE || Nc->soilTemp[iLayer*Nc->sampleDim + iSample] == FILLVALUE){
						Nc->flagQcForward[iSample] |= QC_MISSING;
						break;
					}
					else if(Nc->soilTemp[iLayer*Nc->sampleDim + iSample] <= 0){
						Nc->flagQcForward[iSample] |= QC_FROZEN;
						break;
					}
				}
				if(Nc->airTempMin[iSample] == FILLVALUE){
					Nc->flagQcForward[iSample] |= QC_MISSING;
				}
				else if(Nc->airTempMin[iSample] <= 0){
					Nc->flagQcForward[iSample] |= QC_FROZEN;
				}
				if(Nc->VWCref[iSample] == FILLVALUE){
					Nc->flagQcForward[iSample] |= QC_MISSING;
				}

                // Continue with good data
				if(Nc->flagQcForward[iSample] == QC_GOOD){
					// Ground Structure
					// Calculate mean SM
					sum = 0;
					for(iLayer=0; iLayer<NLayer-1; iLayer++)
						sum += (Gnd->VSM[iLayer] + Gnd->VSM[iLayer+1]) * (Nc->soilDepth[iLayer+1] - Nc->soilDepth[iLayer]);
					avg = sum / 2 / (Nc->soilDepth[NLayer-1] - Nc->soilDepth[0]);
					// VSM to effective SM
					Nc->surPOME[iSample] = (Gnd->VSM[0] - Gnd->resid) / (Gnd->poro - Gnd->resid);
					Nc->bottPOME[iSample] = (Gnd->VSM[NLayer-1] - Gnd->resid) / (Gnd->poro - Gnd->resid);
					Nc->avgPOME[iSample] = (avg - Gnd->resid) / (Gnd->poro - Gnd->resid);
                    // POME type
					Nc->typePOME[iSample] = SMP_POME_main(Nc->surPOME[iSample], Nc->bottPOME[iSample], Nc->avgPOME[iSample]);

					// Effective SM to VSM
					for(iLayer=0; iLayer<MultiLayer->NSublayer; iLayer++){
						Gnd->SMP[iLayer+MultiLayer->NSublayer_extend] = MultiLayer->SMP[iLayer] * (Gnd->poro - Gnd->resid) + Gnd->resid;
					}
					for(iLayer=0; iLayer<MultiLayer->NSublayer_extend; iLayer++){
						Gnd->SMP[iLayer] = Gnd->SMP[MultiLayer->NSublayer_extend];
					}

					// Update vegetation if exists
                    if(SttData->ExistVeg){
					    updateNcVeg(iSample);
                    }

					// Loop for multiple SoOp
					for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
						iOut = iSoOp*Nc->sampleDim + iSample;
						F = &Fixed[iSoOp];
						// Update Geometry						
						if(F->Freq == 137E6){
							F->thTx = Nc->incORBCOMM[iSample]*D2R;
							F->elTx = HalfPi - F->thTx;
							F->thRx = F->thTx;
							F->elRx = HalfPi - F->thRx;
						}
						else if(F->Freq == 255E6 || F->Freq == 370E6){
							F->thTx = Nc->incMUOS[iSample]*D2R;
							F->elTx = HalfPi - F->thTx;
							F->thRx = F->thTx;
							F->elRx = HalfPi - F->thRx;
						}
						else if(F->Freq == 1575.42E6){
							F->thTx = Nc->incGPS[iSample]*D2R;
							F->elTx = HalfPi - F->thTx;
							F->thRx = F->thTx;
							F->elRx = HalfPi - F->thRx;
						}
						F->elTx = HalfPi - F->thTx;
						F->thRx = F->thTx;
						F->elRx = HalfPi - F->thRx;
						//* .. Bistatic Geometry .. *//
						calcTxGeometry(F);
						calcGeometryFixed(F);

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
							LookupAntPattern(F->AntPattern, F->AntPatRes, F->AngT2R_rf[0], F->AngT2R_rf[1], Bistatic->g_rt);
							// Specular direction
							LookupAntPattern(F->AntPattern, F->AntPatRes, F->AngS2R_rf[0], F->AngS2R_rf[1], Bistatic->g_rs);
						}

						// Load vegetation arguments if exist
                        if(SttData->ExistVeg){
                            Bistatic->ArgH = Nc->argHr[iOut] + I1 * Nc->argHi[iOut];
                            Bistatic->ArgV = Nc->argVr[iOut] + I1 * Nc->argVi[iOut];
                        }

						// Effective Roughness Paramter
						Gnd->h = F->h;

                        // Update the bistatic configuration
						updateBistaticFixed(F);
						
						// Construct dielectric constant structure
						calcDielMironov();

						// Loop for polarizations
						for(iPol=0; iPol<NPol; iPol++){
							F->polRx = RxPol[iPol];
							Bistatic->polRx = F->polRx; // Receive Antenna Polarization
							calcAntPolRot();

							/*if(SttData->ExistVeg){
								for(iVeg=0; iVeg<NVeg; iVeg++){
									if(!strcmp(Nc->vegType, VegData[iVeg].label)){
										break;
									}
								}
								calcVegT(iSoOp, iVeg);
								//printf("iSoOp = %ld, AngT2S_sf_th0 = %lf\n", iSoOp, Bistatic->AngT2S_sf_th0*R2D);
								//printf("ArgH = %lf + i %lf, ArgV = %lf + i %lf\n",
								//		creal(Bistatic->ArgH), cimag(Bistatic->ArgH), creal(Bistatic->ArgV), cimag(Bistatic->ArgV));
							}
							*/
							
							// Specular Term
							calcSpecularTermForward();

							if(RxPol[iPol] == 'R'){
								// Reflection Coefficient Co-pol
								Nc->reflCoefCoR_R_B[iOut] = creal(Out->b_coh1b[0]);
								Nc->reflCoefCoI_R_B[iOut] = cimag(Out->b_coh1b[0]);
								// Reflectivity Co-pol
								Nc->reflCo_R_B[iOut] = Out->P_coh1b[0];
							}
							else if(RxPol[iPol] == 'X'){
								// Reflection Coefficient Co-pol
								Nc->reflCoefCoR_X_B[iOut] = creal(Out->b_coh1b[0]);
								Nc->reflCoefCoI_X_B[iOut] = cimag(Out->b_coh1b[0]);
								// Reflectivity Co-pol
								Nc->reflCo_X_B[iOut] = Out->P_coh1b[0];
							}

							if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
								// Reflection Coefficient X-pol (YY or HH)
								Nc->reflCoefXR_X_B[iOut] = creal(Out->b_coh2b[1]);
								Nc->reflCoefXI_X_B[iOut] = cimag(Out->b_coh2b[1]);
							}
							else{
								if(RxPol[iPol] == 'R'){
									// Reflection Coefficient X-pol
									Nc->reflCoefXR_R_B[iOut] = creal(Out->b_coh1b[1]);
									Nc->reflCoefXI_R_B[iOut] = cimag(Out->b_coh1b[1]);
									// Reflectivity X-pol
									Nc->reflX_R_B[iOut] = Out->P_coh1b[1];
								}
								else if(RxPol[iPol] == 'X'){
									// Reflection Coefficient X-pol
									Nc->reflCoefXR_X_B[iOut] = creal(Out->b_coh1b[1]);
									Nc->reflCoefXI_X_B[iOut] = cimag(Out->b_coh1b[1]);
									// Reflectivity X-pol
									Nc->reflX_X_B[iOut] = Out->P_coh1b[1];
								}
							}

							if(SttData->ExistVeg){
								
								Nc->VODH[iOut] = Out->tauH;
								Nc->VODV[iOut] = Out->tauV;

								if(RxPol[iPol] == 'R'){
									// Reflection Coefficient Co-pol
									Nc->reflCoefCoR_R_V[iOut] = creal(Out->b_coh1v[0]);
									Nc->reflCoefCoI_R_V[iOut] = cimag(Out->b_coh1v[0]);
									// Reflectivity Co-pol
									
									Nc->reflCo_R_V[iOut] = Out->P_coh1v[0];
								}
								else if(RxPol[iPol] == 'X'){
									// Reflection Coefficient Co-pol
									Nc->reflCoefCoR_X_V[iOut] = creal(Out->b_coh1v[0]);
									Nc->reflCoefCoI_X_V[iOut] = cimag(Out->b_coh1v[0]);
									// Reflectivity Co-pol
									Nc->reflCo_X_V[iOut] = Out->P_coh1v[0];
								}
								if(Bistatic->polTx == 'X' && Bistatic->polRx == 'X'){
									// Reflection Coefficient X-pol (YY or HH)
									Nc->reflCoefXR_X_V[iOut] = creal(Out->b_coh2v[1]);
									Nc->reflCoefXI_X_V[iOut] = cimag(Out->b_coh2v[1]);
								}
								else{
									if(RxPol[iPol] == 'R'){
										// Reflection Coefficient X-pol
										Nc->reflCoefXR_R_V[iOut] = creal(Out->b_coh1v[1]);
										Nc->reflCoefXI_R_V[iOut] = cimag(Out->b_coh1v[1]);
										// Reflectivity X-pol
										Nc->reflX_R_V[iOut] = Out->P_coh1v[1];
									}
									else if(RxPol[iPol] == 'X'){
										// Reflection Coefficient X-pol
										Nc->reflCoefXR_X_V[iOut] = creal(Out->b_coh1v[1]);
										Nc->reflCoefXI_X_V[iOut] = cimag(Out->b_coh1v[1]);
										// Reflectivity X-pol
										Nc->reflX_X_V[iOut] = Out->P_coh1v[1];
									}
								}
							}
						}
						if(iSoOp == 0){
							for(iLayer=0; iLayer<Nc->sublayerDim; iLayer++){
								Nc->soilMoistPOME[iLayer*Nc->sampleDim + iSample] = Gnd->SMP[iLayer];
							}
						}
					}
				}
				else
				{
					for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
						iOut = iSoOp*Nc->sampleDim + iSample;
						Nc->typePOME[iSample] = FILLVALUE;
						// Penetration Depth
						Nc->penDepthH[iOut] = FILLVALUE;
						Nc->penDepthV[iOut] = FILLVALUE;
						// Reflection Coefficient Co-pol (XX or RR or RX)
						Nc->reflCoefCoR_R_B[iOut] = FILLVALUE;
						Nc->reflCoefCoI_R_B[iOut] = FILLVALUE;
						Nc->reflCoefCoR_X_B[iOut] = FILLVALUE;
						Nc->reflCoefCoI_X_B[iOut] = FILLVALUE;
						Nc->reflCoefCoR_R_V[iOut] = FILLVALUE;
						Nc->reflCoefCoI_R_V[iOut] = FILLVALUE;
						Nc->reflCoefCoR_X_V[iOut] = FILLVALUE;
						Nc->reflCoefCoI_X_V[iOut] = FILLVALUE;
						// Reflectivty Co-pol (XX or RR or RX)
						Nc->reflCo_R_B[iOut] = FILLVALUE;
						Nc->reflCo_X_B[iOut] = FILLVALUE;
						Nc->reflCo_R_V[iOut] = FILLVALUE;
						Nc->reflCo_X_V[iOut] = FILLVALUE;
						// Reflection Coefficient X-pol (RL or RY)
						Nc->reflCoefXR_R_B[iOut] = FILLVALUE;
						Nc->reflCoefXI_R_B[iOut] = FILLVALUE;
						Nc->reflCoefXR_X_B[iOut] = FILLVALUE;
						Nc->reflCoefXI_X_B[iOut] = FILLVALUE;
						Nc->reflCoefXR_R_V[iOut] = FILLVALUE;
						Nc->reflCoefXI_R_V[iOut] = FILLVALUE;
						Nc->reflCoefXR_X_V[iOut] = FILLVALUE;
						Nc->reflCoefXI_X_V[iOut] = FILLVALUE;
						// Reflectivity X-pol (RL or RY)
						Nc->reflX_R_B[iOut] = FILLVALUE;
						Nc->reflX_X_B[iOut] = FILLVALUE;
						Nc->reflX_R_V[iOut] = FILLVALUE;
						Nc->reflX_X_V[iOut] = FILLVALUE;
						// Vegetation
						Nc->VODH[iOut] = FILLVALUE;
						Nc->VODV[iOut] = FILLVALUE;
						Nc->argHr[iOut] = FILLVALUE;
						Nc->argHi[iOut] = FILLVALUE;
						Nc->argVr[iOut] = FILLVALUE;
						Nc->argVi[iOut] = FILLVALUE;
					}
					for(iLayer=0; iLayer<Nc->sublayerDim; iLayer++){
						Nc->soilMoistPOME[iLayer*Nc->sampleDim + iSample] = FILLVALUE;
					}
					for(iVeg=0; iVeg<NVeg; iVeg++){
						for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
							Nc->VWCrefmodel[iLayer*Nc->sampleDim + iSample] = FILLVALUE;
						}
					}
				}	
			}
			// Write to Forward NetCDF
			writeNcForward();
		}
		rewind(stationFile);
	}
	fclose(stationFile);
}

void ForwardScobi_VWC(struct FixedObsType *F, double VWC)
{   
    long iLayer;

    //updateBistaticFixed(F);
	Bistatic->Freq = F->Freq; // Transmitter Frequency [Hz]
	Bistatic->Wavelength = F->Wavelength; // [m]
	Bistatic->polTx = F->polTx; // Transmit Antenna Polarization
    Bistatic->polRx = F->polRx; // Receive Antenna Polarization
	Bistatic->th = F->thTx;
    calcAntPolRot();

    // Effective SM to VSM
    for(iLayer=0; iLayer<MultiLayer->NSublayer_POME; iLayer++){
        Gnd->SMP[iLayer+MultiLayer->NSublayer_extend] = MultiLayer->SMP[iLayer] * (Gnd->poro - Gnd->resid) + Gnd->resid;
    }
    for(iLayer=0; iLayer<MultiLayer->NSublayer_extend; iLayer++){
        Gnd->SMP[iLayer] = Gnd->SMP[MultiLayer->NSublayer_extend];
    }
    calcDielMironov();

    // Specular Term
    calcSpecularTermSimple(F, VWC);
}

double Cost_SM_VWC(double sur, double bott, double avg, double VWC)
{
    int res;

    res = SMP_POME_main(sur, bott, avg);
    if(res == POME_FAILURE)
        return POME_FAILURE;

    long iSoOp, iPol;
    double R_ref, R_est, R_err_norm;
    double cost = 0.0;
 
    for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
        // .. Effective Roughness Paramter .. //
        Gnd->h = Fixed[iSoOp].h;
        
        // .. Bistatic Geometry .. //
        calcTxGeometry(&Fixed[iSoOp]);
        calcGeometryFixed(&Fixed[iSoOp]);

        for(iPol=0; iPol<NPol; iPol++){     
            if(iPol==0 || flagMixPol){
                if(RxPol[iPol] == 'R' || RxPol[iPol] == 'L')
                    Fixed[iSoOp].polRx = 'R';
                else
                    Fixed[iSoOp].polRx = 'X';
                // Forward funciton
                ForwardScobi_VWC(&Fixed[iSoOp], VWC);
            }  
            // Cross-pol
            if(RxPol[iPol] == 'L' || RxPol[iPol] == 'Y'){
                R_ref = Ret->R_X_ref[iSoOp*NPol+iPol];
                R_est = Out->P_coh1[1];
            }
            // Co-pol
            else{
                R_ref = Ret->R_CO_ref[iSoOp*NPol+iPol];
                R_est = Out->P_coh1[0];
            }
            R_err_norm = (R_est - R_ref) / R_ref;
            cost += R_err_norm*R_err_norm;
        }
    }
    cost /= NSoOp*NPol;

    return cost;
}

void InitSA(void)
{
    long iDim;
    
    /* .. Initialize Simulated Annealing Parameters Structure .. */
	SimAnneal = (struct SimAnnealType *) calloc(1, sizeof(struct SimAnnealType));
	if (SimAnneal == NULL) {
		printf("SimAnneal calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

    // Inputs
    SimAnneal->nDim = 4;
    SimAnneal->nIter = 100;
    SimAnneal->nTemp = 100;
	SimAnneal->nTrap = 3;
    SimAnneal->f_min = 1E-8;
    SimAnneal->f_err = 1E-7;
    SimAnneal->c_cost = 1;
    SimAnneal->nAccepted = 50;
    SimAnneal->nGen = 500;

    SimAnneal->c = (double *) calloc(SimAnneal->nDim,sizeof(double));
    if (SimAnneal->c == NULL) {
        printf("SimAnneal->c calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    SimAnneal->c[0] = 1;
    SimAnneal->c[1] = 1;
    SimAnneal->c[2] = 1;
    SimAnneal->c[3] = 1;

    SimAnneal->s = (double *) calloc(SimAnneal->nDim,sizeof(double));
    if (SimAnneal->s == NULL) {
        printf("SimAnneal->s calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    SimAnneal->T_gen = (double *) calloc(SimAnneal->nDim,sizeof(double));
    if (SimAnneal->T_gen == NULL) {
        printf("SimAnneal->T_gen calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    SimAnneal->lb = (double *) calloc(SimAnneal->nDim,sizeof(double));
    if (SimAnneal->lb == NULL) {
        printf("SimAnneal->lb calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    SimAnneal->ub = (double *) calloc(SimAnneal->nDim,sizeof(double));
    if (SimAnneal->ub == NULL) {
        printf("SimAnneal->ub calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    
    // Effective bounds    
    for(iDim=0; iDim<SimAnneal->nDim-1; iDim++){
        SimAnneal->lb[iDim] = SOILMOISTMIN_EFF;
        SimAnneal->ub[iDim] = SOILMOISTMAX_EFF;
    }
    SimAnneal->lb[3] = VWCMIN;
    SimAnneal->ub[3] = VWCMAX;

	// Outputs
    SimAnneal->x_opt = (double *) calloc(SimAnneal->nDim,sizeof(double));
    if (SimAnneal->x_opt == NULL) {
        printf("SimAnneal->x_opt calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
}

void InitRetrieval(void)
{
    // Read Inp_Retrieval.txt
    FILE *infile;
    char junk[120], newline, filename[30];
	long iLayer, period, window, iSoOp;
    double stddev;
	
    /* .. Read from file Inp_Retrieval.txt */
    infile=FileOpen(SimPath,"Inp_Retrieval.txt","r");
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

	// Receive Polarization (R,L,X,Y)
	fscanf(infile, "%s %[^\n] %[\n]", RxPol, junk, &newline);
	if(!strcmp(RxPol, "RL") || !strcmp(RxPol, "XY")){
		NPol = 2;
        flagMixPol = FALSE;
	}
    else if(!strcmp(RxPol, "RX") || !strcmp(RxPol, "RY") || !strcmp(RxPol, "LX") || !strcmp(RxPol, "LY")){
        NPol = 2;
        flagMixPol = TRUE;
    }
	else{
		NPol = 1;
        flagMixPol = FALSE;
	}
    // Stand deviation of reflectivity error ratio
    fscanf(infile,"%lf %[^\n] %[\n]", &stddev,junk,&newline);
    // Measurement period and time window
    fscanf(infile,"%ld %ld %[^\n] %[\n]", &period, &window,junk,&newline);
    // Number of Input Soil Layers
    NLayer = 5;
    // The b parameter input file name
    fscanf(infile,"%s %[^\n] %[\n]",filename,junk,&newline);
    
    free(MultiLayer);

    MultiLayer = (struct MultiLayerType *) calloc(1,sizeof(struct MultiLayerType));
    if (MultiLayer == NULL) {
        printf("MultiLayer calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    // POME model
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    // Total depths [cm]
    fscanf(infile,"%lf %[^\n] %[\n]", &MultiLayer->D,junk,&newline);
    MultiLayer->D *= 1E-2; // [cm] -> [m]
    MultiLayer->D -= 0.05;
    // Layer discretization [mm]
    fscanf(infile,"%lf %[^\n] %[\n]", &MultiLayer->delZ,junk,&newline);
    MultiLayer->delZ *= 1E-3; // [mm] -> [m]
    // Initial inflection point depth [cm]
    fscanf(infile,"%lf %[^\n] %[\n]", &MultiLayer->z_infl,junk,&newline);
    MultiLayer->z_infl *= 1E-2; // [cm] -> [m]
    MultiLayer->z_infl -= 0.05;

    MultiLayer->NSublayer_POME = (long) round(MultiLayer->D/MultiLayer->delZ) + 1;
    MultiLayer->NSublayer_extend = (long) round(0.05/MultiLayer->delZ);
    MultiLayer->NSublayer = MultiLayer->NSublayer_POME + MultiLayer->NSublayer_extend;

    MultiLayer->z = (double *) calloc(MultiLayer->NSublayer, sizeof(double));
    if (MultiLayer->z == NULL) {
        printf("MultiLayer->z calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    MultiLayer->SMP = (double *) calloc(MultiLayer->NSublayer_POME, sizeof(double));
    if (MultiLayer->SMP == NULL) {
        printf("MultiLayer->SMP calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    for(iLayer=0; iLayer<MultiLayer->NSublayer; iLayer++){
        MultiLayer->z[iLayer] = iLayer*MultiLayer->delZ;
    }
        
    fclose(infile);
 
 	/* .. Initialize Local Ground Parameters .. */
    free(Gnd);
   
	Gnd = (struct LocalGndType *) calloc(1, sizeof(struct LocalGndType));
	if (Gnd == NULL) {
		printf("Local Gnd parameter calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Gnd->VSM = (float *)calloc(NLayer, sizeof(float));
	if (Gnd->VSM == NULL) {
		printf("Gnd->VSM calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	Gnd->e_c = (double complex *)calloc(NLayer, sizeof(double complex));
	if (Gnd->e_c == NULL) {
		printf("Gnd->e_c calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
    Gnd->e_c_profile = (double complex *)calloc(MultiLayer->NSublayer, sizeof(double complex));
    if (Gnd->e_c_profile == NULL) {
        printf("Gnd->e_c_profile calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Gnd->SMP = (double *)calloc(MultiLayer->NSublayer, sizeof(double));
    if (Gnd->SMP == NULL) {
        printf("Gnd->SMP calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Ret = (struct RetrievalType *) calloc(1,sizeof(struct RetrievalType));
    if (Ret == NULL) {
        printf("Ret calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Ret->VSM_ref = (double *) calloc(NLayer,sizeof(double));
    if (Ret->VSM_ref == NULL) {
        printf("Ret->VSM_ref calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Ret->VSM_est = (double *) calloc(NLayer,sizeof(double));
    if (Ret->VSM_est == NULL) {
        printf("Ret->VSM_est calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Ret->VSM_err = (double *) calloc(NLayer,sizeof(double));
    if (Ret->VSM_err == NULL) {
        printf("Ret->VSM_err calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Ret->R_CO_ref = (double *) calloc(NSoOp*NPol,sizeof(double));
    if (Ret->R_CO_ref == NULL) {
        printf("Ret->R_CO_ref calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Ret->R_CO_est = (double *) calloc(NSoOp*NPol,sizeof(double));
    if (Ret->R_CO_est == NULL) {
        printf("Ret->R_CO_est calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Ret->R_X_ref = (double *) calloc(NSoOp*NPol,sizeof(double));
    if (Ret->R_X_ref == NULL) {
        printf("Ret->R_X_ref calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Ret->R_X_est = (double *) calloc(NSoOp*NPol,sizeof(double));
    if (Ret->R_X_est == NULL) {
        printf("Ret->R_X_est calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Ret->ArgH = (double complex *) calloc(NSoOp,sizeof(double complex));
    if (Ret->ArgH == NULL) {
        printf("Ret->ArgH calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }
    Ret->ArgV = (double complex *) calloc(NSoOp,sizeof(double complex));
    if (Ret->ArgV == NULL) {
        printf("Ret->ArgV calloc returned null pointer.  Bailing out!\n");
        exit(1);
    }

    Ret->stddev = stddev;
    Ret->period = period;
    Ret->window = window;

    // .. Read the b parameter .. //
    double temp[14];
    char delimiter[2] = "\t";
    int iPol;

    infile=FileOpen(SimPath, filename,"r");
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

	b_LUT = (double **) calloc(4, sizeof(double *));
	for(iSoOp=0; iSoOp<4; iSoOp++){
		b_LUT[iSoOp] = (double *) calloc(4, sizeof(double));
	}

    for(iSoOp=0; iSoOp<4; iSoOp++){
        readLineParsor(infile, temp, delimiter);
        for(iPol=0; iPol<4; iPol++){
            b_LUT[iSoOp][iPol] = temp[iPol+1]; // [][0]: R-pol, [][1]: L-pol, [][2]: X(V)-pol, [][3]: Y(H)-pol
        }
    }

    // Transmitter direction
    Bistatic->g_rt[0][0] = 1; Bistatic->g_rt[0][1] = 0;
    Bistatic->g_rt[1][0] = 0; Bistatic->g_rt[1][1] = 1;
    // Specular direction
    Bistatic->g_rs[0][0] = 1; Bistatic->g_rs[0][1] = 0;
    Bistatic->g_rs[1][0] = 0; Bistatic->g_rs[1][1] = 1;
}

void asa_SM_VWC(double (*costFunc)(double, double, double, double), long stationID)
{
    //static long prev_stationID = -1;

    struct SimAnnealType *SA;
    SA = &SimAnneal[0];

    //* ..  Set up variables .. *//
    // Random starting point mapped into solution space
    long iIter, iDim, iGen;
    long nIter = SA->nIter, nDim = SA->nDim, nGen = SA->nGen, nAccepted = SA->nAccepted;
    long cnt_accepted = 0, cnt_accepted_prev = 0;
    double x_old[nDim], f_old, y;
    double x_new[nDim], f_new;
    double x_opt[nDim], f_opt;
    double lb[nDim], ub[nDim];
    double f_min = SA->f_min;
    double alpha, delE;
    double T_gen[nDim], T0_gen[nDim];
    double T_accept, T0_accept, T_accept_init;
    double k_gen[nDim], k_accept = 0.0, c[nDim], c_cost = SA->c_cost;
    double s[nDim], del = 0.01, temp, s_max = 0.0; // for reannealing
    
    long flagTrapped = 1, nTrap = SA->nTrap;
    double f_err = SA->f_err;
    long cnt_trapped = 0, cnt_reanneal = 0;

    struct RandomProcessType *randproc;
    
    int taskid;
    
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

    // Create random process
    randproc = CreateRandomProcess((long)time(NULL));

    // Generate some random samples within the optimization space and calculate the cost
    for(iDim=0; iDim<nDim; iDim++){
        T0_gen[iDim] = 1.0;
        T_gen[iDim] = T0_gen[iDim];
        k_gen[iDim] = 0.0;
        c[iDim] = SA->c[iDim];
        lb[iDim] = SA->lb[iDim];
        ub[iDim] = SA->ub[iDim];
    }
    do{
        for(iDim=0; iDim<nDim; iDim++){
            do{
                x_old[iDim] = lb[iDim] + UniformRandom(randproc) * (ub[iDim] - lb[iDim])/2;
            }while((x_old[iDim] < lb[iDim]) || (x_old[iDim] > ub[iDim]) || isnan(x_old[iDim]));
        }
        f_old = costFunc(x_old[0], x_old[1], x_old[2], x_old[3]);
    }while(f_old == POME_FAILURE);

    f_opt = f_old;
    // Initial temperature of the acceptance probability function
    T_accept_init = f_old;
    T0_accept = T_accept_init;
    T_accept = T_accept_init;

    //* .. Run Simulated Annealing .. *//
    for(iIter=0; iIter<nIter; iIter++){
        for(iGen=0; iGen<nGen; iGen++){ // Number of point generation
            // New point generation
            do{
                for(iDim=0; iDim<nDim; iDim++){
                    do{
                        alpha = UniformRandom(randproc);
                        y = signum(alpha-0.5)*T_gen[iDim]*(pow(1.0+1.0/T_gen[iDim],fabs(2.0*alpha-1.0))-1.0);
                        x_new[iDim] = x_old[iDim] + y*(ub[iDim] - lb[iDim]);
                    }while((x_new[iDim] < lb[iDim]) || (x_new[iDim] > ub[iDim])); 
                }
                f_new = costFunc(x_new[0], x_new[1], x_new[2], x_new[3]);
            }while(f_new == POME_FAILURE); // Compute cost f_new

            delE = f_new - f_old;

            // If the new point is better accept it. Otherwise, accept it randomly based on a Boltzmann probability density (Metropolis move)
            //if (exp(-delE/T_accept)>UniformRandom(randproc)){
            if ((delE < 0) || (1/(1+exp(delE/T_accept))>=UniformRandom(randproc))){
                f_old = f_new;
                //printf("P = %lf\n", 1/(1+exp(delE/T_accept)));
                for(iDim=0; iDim<nDim; iDim++){
                    x_old[iDim] = x_new[iDim];
                }
                cnt_accepted++;

                if(f_old < f_opt){
                    f_opt = f_old;
                    for(iDim=0; iDim<nDim; iDim++)
                        x_opt[iDim] = x_old[iDim];
                }
            }

            // Reanneal
            if(cnt_accepted > nAccepted){
                cnt_accepted = 0;
                cnt_reanneal++;
                s_max = 0.0;
                // Copy the optimal point
                for(iDim=0; iDim<nDim; iDim++){
                    x_new[iDim] = x_opt[iDim];
                }
                for(iDim=0; iDim<nDim; iDim++){
                    temp = x_new[iDim];
                    x_new[iDim] += del;
                    f_new = costFunc(x_new[0], x_new[1], x_new[2], x_new[3]);
                    // Sensitivity calculation
                    if(f_new != POME_FAILURE){
                        s[iDim] = fabs((f_new - f_opt)/del);
                        if(s[iDim] < EPS)
                            s[iDim] = EPS;
                        if(s[iDim] > s_max)
                            s_max = s[iDim];
                    }
                    else{
                        s[iDim] = POME_FAILURE;
                    }                
                    x_new[iDim] = temp;
                }
                // Update generating temperature
                for(iDim=0; iDim<nDim; iDim++){
                    if(s[iDim] != POME_FAILURE){
                        T_gen[iDim] = s_max/s[iDim] * T_gen[iDim];
                        k_gen[iDim] = fabs(pow(log(T0_gen[iDim]/T_gen[iDim])/c[iDim], (double) nDim));
                    }
                    else{
                        T_gen[iDim] = 1.0;
                        k_gen[iDim] = 0.0;
                    }
                }

                // Update acceptance temperature
                T0_accept = f_old;
                T_accept = f_opt;
                k_accept = pow(log(T0_accept/T_accept)/c_cost, (double) nDim);
            }
        }
        // Update generating temperature
        for(iDim=0; iDim<nDim; iDim++){
            k_gen[iDim] += 1.0;
            T_gen[iDim] = T0_gen[iDim] * exp(-c[iDim]*pow(k_gen[iDim], (double) 1/nDim));
        }
        // Update acceptance temperature
        k_accept += 1.0;
        T_accept = T0_accept * exp(-c_cost*pow(k_accept, (double) 1/nDim));
        
        // Terminating criterion
        if(f_opt < f_min){
            break;
        }

        // Decide whether the algorithm has been trapped in a local minimum
        if((cnt_accepted == cnt_accepted_prev) || (fabs(f_old - f_opt) < f_err)){
            flagTrapped += 1;
        }
        else{
            flagTrapped = 1;
        }
        cnt_accepted_prev = cnt_accepted;
        if(flagTrapped > nTrap){
            flagTrapped = 1;
            // Initial temperature of the acceptance probability function
            T0_accept = T_accept_init;
            T_accept = T_accept_init;
            k_accept = 0.0;
            for(iDim=0; iDim<nDim; iDim++){
                T0_gen[iDim] = 1.0;
                T_gen[iDim] = 1.0;
                k_gen[iDim] = 0.0;
            }
            cnt_accepted = 0;
            cnt_trapped++;
        }
    }
    
    for(iDim=0; iDim<nDim; iDim++){
        SA->x_opt[iDim] = x_opt[iDim];
    }
    SA->f_opt = f_opt;
    SA->iIter = iIter;
    SA->cnt_trapped = cnt_trapped;

    DestroyRandomProcess(randproc);
}

void RetrievalNetCDF(void)
{
    clock_t startSingle, endSingle; // CPU time
    time_t startTotal, endTotal; // Wall time
    double timeSingle;
	long iYear, iStation, iLayer, iSample, nStation, iSoOp, iPol, iCycle, iTask, iFreq, iVeg;
	FILE *stationFile;
	char stationName[30], newline;
    struct RandomProcessType *randproc;
    
	// Variables for MPI
    int inmsg, outmsg = 1, tag = 1, source, dest;
    static int taskid;
    static int numtasks;
    MPI_Status status;

    // MPI Tasks
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

    int flagQcRetResi = FILLVALUE;
    int iProductTimeResi = FILLVALUE;
    int typePOMERetResi = FILLVALUE;
    double surPOMERetResi = FILLVALUE;
    double bottPOMERetResi = FILLVALUE;
    double avgPOMERetResi = FILLVALUE;
    double VWCRetResi = FILLVALUE;
    double saTimeResi = FILLVALUE;
    long saIterResi = FILLVALUE;
    long saJumpResi = FILLVALUE;
    double saCostMinResi = FILLVALUE;

    int recvcount1[numtasks], displs1[numtasks];
    int recvcount2[numtasks], displs2[numtasks];
    int recvcount3[numtasks], displs3[numtasks];

    int dt_lb, dt_ub, iObs, iRefl, flagAssigned, iSampleMUOS;
    long period;
    long window;

    InitRetrieval();
    InitSA();
    period = Ret->period;
    window = Ret->window;
    
    dt_lb = ceil(-window/2);
    dt_ub = floor(window/2);

    if(taskid == MASTER){
        printf(">> Number of Retrieved Soil Layers is %ld\n", NLayer);
        printf(">> Ground Multi-layered Structure: POME model\n");
        printf(">> Start Retrieval.\n");
    }	
	
	stationFile = FileOpen(SimPath, Nc->filename, "r");
	
	for(iYear=Nc->startYear; iYear<Nc->endYear+1; iYear++){
        fscanf(stationFile,"%ld %[\n]",&nStation, &newline);
		for(iStation=0; iStation<nStation; iStation++){
			// Read station name
			fscanf(stationFile, "%s %[\n]", stationName, &newline);

	        // Read USCRN NetCDF
			readNcUSCRN(iYear, stationName);
	
	        // Read Forward NetCDF
            readNcForward(iYear, stationName);

	        Nc->retDim = floor(Nc->sampleDim / period);   
            Nc->nCycle = floor(Nc->retDim / numtasks);

            Nc->soopDim = NSoOp;
            Nc->sublayerDim = MultiLayer->NSublayer;
	        
            // Create Retrieval NetCDF            
            if(taskid == MASTER){
    			createNcRetPeriod(iYear, stationName, period, window);    
                printf(">> %ld %s, sampling period = %ld, samples = %ld, processors = %d, cycle = %ld, window = %ld, sublayers= %ld + %ld\n",
                    iYear, stationName, period, Nc->sampleDim, numtasks, Nc->nCycle, window, MultiLayer->NSublayer_POME, MultiLayer->NSublayer_extend);
                for (dest=1; dest<numtasks; dest++) {
                    MPI_Send(&outmsg, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
                }
                startTotal = time(NULL);
                Nc->reflErr = Ret->stddev;
            }
            // Non-master tasks only
            if (taskid > MASTER) {
                source = MASTER;
                MPI_Recv(&inmsg, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
            }// end of non-master
            // Load Soil Texture
            Gnd->clay = Nc->soilType[0];
            Gnd->sand = Nc->soilType[2];
            // Get soil water characteristic based on soil fractions
            LookupSoiltype();

            int flagQcRet[Nc->nCycle];
            int iProductTime[Nc->nCycle];
            int iObsTime[Nc->nCycle*Nc->soopDim];
            double reflX_R_B_Ref[Nc->nCycle*Nc->soopDim];
            double reflX_X_B_Ref[Nc->nCycle*Nc->soopDim];
            double reflX_R_B_Est[Nc->nCycle*Nc->soopDim];
            double reflX_X_B_Est[Nc->nCycle*Nc->soopDim];
            double reflX_R_B_Err[Nc->nCycle*Nc->soopDim];
            double reflX_X_B_Err[Nc->nCycle*Nc->soopDim];

            double reflX_R_V_Ref[Nc->nCycle*Nc->soopDim];
            double reflX_X_V_Ref[Nc->nCycle*Nc->soopDim];
            double reflX_R_V_Est[Nc->nCycle*Nc->soopDim];
            double reflX_X_V_Est[Nc->nCycle*Nc->soopDim];
            double reflX_R_V_Err[Nc->nCycle*Nc->soopDim];
            double reflX_X_V_Err[Nc->nCycle*Nc->soopDim];

            double reflCo_R_B_Ref[Nc->nCycle*Nc->soopDim];
            double reflCo_X_B_Ref[Nc->nCycle*Nc->soopDim];
            double reflCo_R_B_Est[Nc->nCycle*Nc->soopDim];
            double reflCo_X_B_Est[Nc->nCycle*Nc->soopDim];
            double reflCo_R_B_Err[Nc->nCycle*Nc->soopDim];
            double reflCo_X_B_Err[Nc->nCycle*Nc->soopDim];

            double reflCo_R_V_Ref[Nc->nCycle*Nc->soopDim];
            double reflCo_X_V_Ref[Nc->nCycle*Nc->soopDim];
            double reflCo_R_V_Est[Nc->nCycle*Nc->soopDim];
            double reflCo_X_V_Est[Nc->nCycle*Nc->soopDim];
            double reflCo_R_V_Err[Nc->nCycle*Nc->soopDim];
            double reflCo_X_V_Err[Nc->nCycle*Nc->soopDim];

            double penDepthHRet[Nc->nCycle*Nc->soopDim];
            double penDepthVRet[Nc->nCycle*Nc->soopDim];

            double soilMoistRet[Nc->nCycle*Nc->sublayerDim];
            int typePOMERet[Nc->nCycle];
            double surPOMERet[Nc->nCycle];
            double bottPOMERet[Nc->nCycle];
            double avgPOMERet[Nc->nCycle];
            double VWCRet[Nc->nCycle];
            double saTime[Nc->nCycle];
            long saIter[Nc->nCycle];
            long saJump[Nc->nCycle];
            double saCostMin[Nc->nCycle];

            int iObsTimeResi[Nc->soopDim];
            double reflX_R_B_RefResi[Nc->soopDim];
            double reflX_X_B_RefResi[Nc->soopDim];
            double reflX_R_B_EstResi[Nc->soopDim];
            double reflX_X_B_EstResi[Nc->soopDim];
            double reflX_R_B_ErrResi[Nc->soopDim];
            double reflX_X_B_ErrResi[Nc->soopDim];

            double reflX_R_V_RefResi[Nc->soopDim];
            double reflX_X_V_RefResi[Nc->soopDim];
            double reflX_R_V_EstResi[Nc->soopDim];
            double reflX_X_V_EstResi[Nc->soopDim];
            double reflX_R_V_ErrResi[Nc->soopDim];
            double reflX_X_V_ErrResi[Nc->soopDim];

            double reflCo_R_B_RefResi[Nc->soopDim];
            double reflCo_X_B_RefResi[Nc->soopDim];
            double reflCo_R_B_EstResi[Nc->soopDim];
            double reflCo_X_B_EstResi[Nc->soopDim];
            double reflCo_R_B_ErrResi[Nc->soopDim];
            double reflCo_X_B_ErrResi[Nc->soopDim];

            double reflCo_R_V_RefResi[Nc->soopDim];
            double reflCo_X_V_RefResi[Nc->soopDim];
            double reflCo_R_V_EstResi[Nc->soopDim];
            double reflCo_X_V_EstResi[Nc->soopDim];
            double reflCo_R_V_ErrResi[Nc->soopDim];
            double reflCo_X_V_ErrResi[Nc->soopDim];

            double penDepthHRetResi[Nc->soopDim];
            double penDepthVRetResi[Nc->soopDim];
            double soilMoistRetResi[Nc->sublayerDim];
            
            for(iTask=0; iTask<numtasks; iTask++){
                recvcount1[iTask] = 0;
                recvcount2[iTask] = 0;
                recvcount3[iTask] = 0;
                displs1[iTask] = 0;
                displs2[iTask] = 0;
                displs3[iTask] = 0;
            }

            // Create random process
            randproc = CreateRandomProcess((long)time(NULL) + taskid);
            
            // Start retrieval cycle
            for(iCycle=0; iCycle<Nc->nCycle; iCycle++){
                // Product time
                iProductTime[iCycle] = period * (iCycle + taskid*Nc->nCycle) + floor(period/2);
                // Quality check for the data on the product time
                flagQcRet[iCycle] = Nc->flagQcForward[iProductTime[iCycle]];
                if(flagQcRet[iCycle] == QC_GOOD){
                    // Random selection of observations (reflectivities)
                    flagAssigned = FALSE;
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){  
                        // Index for observation time          
                        iObs = iCycle*Nc->soopDim + iSoOp;
                        if(Fixed[iSoOp].Freq == 255E6 || Fixed[iSoOp].Freq == 370E6){
                            // P-band observations occur simultaneously
                            if(flagAssigned){
                                iSample = iSampleMUOS;
                            }
                            else{
                                iSample = iProductTime[iCycle] + UniformRandomDiscrete(randproc, dt_lb, dt_ub);
                                iSampleMUOS = iSample;
                                flagAssigned = TRUE;
                            }
                        }
                        else{
                            iSample = iProductTime[iCycle] + UniformRandomDiscrete(randproc, dt_lb, dt_ub);
                        }
                        // Store observation time
                        iObsTime[iObs] = iSample;
                        // Quality check for the data on the observation time. If any observatoin is bad, skip this retrieval.
                        flagQcRet[iCycle] = Nc->flagQcForward[iSample];
                        if(flagQcRet[iCycle] == QC_MISSING){
                            printf("Task %d >> Sample %ld : QC_MISSING\n", taskid, iSample);  
                            break;
                        } 
                        else if(flagQcRet[iCycle] == QC_FROZEN){
                            printf("Task %d >> Sample %ld : QC_FROZEN\n", taskid, iSample);
                            break;
                        }
                        else if(flagQcRet[iCycle] == QC_MISSING + QC_FROZEN){
                            printf("Task %d >> Sample %ld : QC_MISSING + QC_FROZEN\n", taskid, iSample);  
                            break;
                        }
                    }
                }
                if(flagQcRet[iCycle] == QC_GOOD){
                    // Load observations
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                        iObs = iCycle*Nc->soopDim + iSoOp;
                        iSample = iObsTime[iObs];

                        // Update Geometry
                        if(Fixed[iSoOp].Freq == 137E6){	
                            Fixed[iSoOp].thTx = Nc->incORBCOMM[iSample]*D2R;
                            Fixed[iSoOp].ID = 0;
                            iFreq = 0;
                        }
                        else if(Fixed[iSoOp].Freq == 255E6){
                            Fixed[iSoOp].thTx = Nc->incMUOS[iSample]*D2R;
                            Fixed[iSoOp].ID = 1;
                            iFreq = 1;
                        }
                        else if(Fixed[iSoOp].Freq == 370E6){
                            Fixed[iSoOp].thTx = Nc->incMUOS[iSample]*D2R;
                            Fixed[iSoOp].ID = 2;
                            iFreq = 2;
                        }
                        else if(Fixed[iSoOp].Freq == 1575.42E6){
                            Fixed[iSoOp].thTx = Nc->incGPS[iSample]*D2R;
                            Fixed[iSoOp].ID = 3;
                            iFreq = 3;
                        }
                        Fixed[iSoOp].elTx = HalfPi - Fixed[iSoOp].thTx;
                        Fixed[iSoOp].thRx = Fixed[iSoOp].thTx;
                        Fixed[iSoOp].elRx = HalfPi - Fixed[iSoOp].thRx;

                        // Vegetation
                        if(SttData->ExistVeg){
                            Ret->ArgH[iSoOp] = Nc->argHr[iFreq*Nc->sampleDim + iSample] + I1 * Nc->argHi[iFreq*Nc->sampleDim + iSample];
                            Ret->ArgV[iSoOp] = Nc->argVr[iFreq*Nc->sampleDim + iSample] + I1 * Nc->argVi[iFreq*Nc->sampleDim + iSample];

                            for(iPol=0; iPol<NPol; iPol++){
                                iRefl = iSoOp*NPol+iPol;
                                switch (RxPol[iPol]){
                                    case 'L':
                                        Ret->R_X_ref[iRefl] = Nc->reflX_R_V[iFreq*Nc->sampleDim + iSample];
                                        reflX_R_V_Ref[iObs] = Ret->R_X_ref[iRefl];
                                        reflX_R_V_Err[iObs] = GaussianRandom(randproc) * reflX_R_V_Ref[iObs] * Ret->stddev;
                                        Ret->R_X_ref[iRefl] += reflX_R_V_Err[iObs];
                                        break;
                                    case 'R':
                                        Ret->R_CO_ref[iRefl] = Nc->reflCo_R_V[iFreq*Nc->sampleDim + iSample];
                                        reflCo_R_V_Ref[iObs] = Ret->R_CO_ref[iRefl];
                                        reflCo_R_V_Err[iObs] = GaussianRandom(randproc) * reflCo_R_V_Ref[iObs] * Ret->stddev;
                                        Ret->R_CO_ref[iRefl] += reflCo_R_V_Err[iObs];
                                        break;
                                    case 'Y':
                                        Ret->R_X_ref[iRefl] = Nc->reflX_X_V[iFreq*Nc->sampleDim + iSample];
                                        reflX_X_V_Ref[iObs] = Ret->R_X_ref[iRefl];
                                        reflX_X_V_Err[iObs] = GaussianRandom(randproc) * reflX_X_V_Ref[iObs] * Ret->stddev;
                                        Ret->R_X_ref[iRefl] += reflX_X_V_Err[iObs];
                                        break;
                                    case 'X':
                                        Ret->R_CO_ref[iRefl] = Nc->reflCo_X_V[iFreq*Nc->sampleDim + iSample];
                                        reflCo_X_V_Ref[iObs] = Ret->R_CO_ref[iRefl];
                                        reflCo_X_V_Err[iObs] = GaussianRandom(randproc) * reflCo_X_V_Ref[iObs] * Ret->stddev;
                                        Ret->R_CO_ref[iRefl] += reflCo_X_V_Err[iObs];  
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                        // Bare soil
                        else{
                            for(iPol=0; iPol<NPol; iPol++){
                                iRefl = iSoOp*NPol+iPol;
                                switch (RxPol[iPol]){
                                    case 'L':
                                        Ret->R_X_ref[iRefl] = Nc->reflX_R_B[iFreq*Nc->sampleDim + iSample];
                                        reflX_R_B_Ref[iObs] = Ret->R_X_ref[iRefl];
                                        reflX_R_B_Err[iObs] = GaussianRandom(randproc) * reflX_R_B_Ref[iObs] * Ret->stddev;
                                        Ret->R_X_ref[iRefl] += reflX_R_B_Err[iObs];
                                        break;
                                    case 'R':
                                        Ret->R_CO_ref[iRefl] = Nc->reflCo_R_B[iFreq*Nc->sampleDim + iSample];
                                        reflCo_R_B_Ref[iObs] = Ret->R_CO_ref[iRefl];
                                        reflCo_R_B_Err[iObs] = GaussianRandom(randproc) * reflCo_R_B_Ref[iObs] * Ret->stddev;
                                        Ret->R_CO_ref[iRefl] += reflCo_R_B_Err[iObs];
                                        break;
                                    case 'Y':
                                        Ret->R_X_ref[iRefl] = Nc->reflX_X_B[iFreq*Nc->sampleDim + iSample];
                                        reflX_X_B_Ref[iObs] = Ret->R_X_ref[iRefl];
                                        reflX_X_B_Err[iObs] = GaussianRandom(randproc) * reflX_X_B_Ref[iObs] * Ret->stddev;
                                        Ret->R_X_ref[iRefl] += reflX_X_B_Err[iObs];
                                        break;
                                    case 'X':
                                        Ret->R_CO_ref[iRefl] = Nc->reflCo_X_B[iFreq*Nc->sampleDim + iSample];
                                        reflCo_X_B_Ref[iObs] = Ret->R_CO_ref[iRefl];
                                        reflCo_X_B_Err[iObs] = GaussianRandom(randproc) * reflCo_X_B_Ref[iObs] * Ret->stddev;
                                        Ret->R_CO_ref[iRefl] += reflCo_X_B_Err[iObs]; 
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    // Truth data
                    Ret->sur_ref = Nc->surPOME[iProductTime[iCycle]];
                    Ret->bott_ref = Nc->bottPOME[iProductTime[iCycle]];
                    Ret->avg_ref = Nc->avgPOME[iProductTime[iCycle]];
                    if(SttData->ExistVeg){
                        Ret->VWC_ref = 0;
                        for(iVeg=0; iVeg<NVeg; iVeg++){
                            for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
                                Ret->VWC_ref += Nc->VWCrefmodel[iLayer*Nc->sampleDim + iSample];
                            }
                        }
                    }

                    // Print product time and observation times
                    printf("Task %d >> h_product = %ld-%ld / h_obs = ", taskid, Nc->utcDate[iProductTime[iCycle]], Nc->utcTime[iProductTime[iCycle]]);
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                        iObs = iCycle*Nc->soopDim + iSoOp;
                        printf("%ld-%ld ", Nc->utcDate[iObsTime[iObs]], Nc->utcTime[iObsTime[iObs]]);
                    }
                    printf("\n");
                    // Print truth data
                    printf("Task %d >> Truth: POME = %lf %lf %lf / VWC = %lf\n",
                        taskid, Ret->sur_ref, Ret->bott_ref, Ret->avg_ref, Ret->VWC_ref);
                    // Print observation data
                    printf("Task %d >> Obs: Gamma(Err) = ", taskid);
                    if(SttData->ExistVeg){
                        for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                            iObs = iCycle*Nc->soopDim + iSoOp;
                            for(iPol=0; iPol<NPol; iPol++){
                                switch (RxPol[iPol]){
                                    case 'L':
                                        printf("L %lf(%lf) ", reflX_R_V_Ref[iObs], reflX_R_V_Err[iObs]);
                                        break;
                                    case 'R':
                                        printf("R %lf(%lf) ", reflCo_R_V_Ref[iObs], reflCo_R_V_Err[iObs]);
                                        break;
                                    case 'Y':
                                        printf("H %lf(%lf) ", reflX_X_V_Ref[iObs], reflX_X_V_Err[iObs]);
                                        break;
                                    case 'X':
                                        printf("V %lf(%lf) ", reflCo_X_V_Ref[iObs], reflCo_X_V_Err[iObs]);
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    else{
                        for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                            iObs = iCycle*Nc->soopDim + iSoOp;
                            for(iPol=0; iPol<NPol; iPol++){
                                switch (RxPol[iPol]){
                                    case 'L':
                                        printf("L %lf(%lf) ", reflX_R_B_Ref[iObs], reflX_R_B_Err[iObs]);
                                        break;
                                    case 'R':
                                        printf("R %lf(%lf) ", reflCo_R_B_Ref[iObs], reflCo_R_B_Err[iObs]);
                                        break;
                                    case 'Y':
                                        printf("H %lf(%lf) ", reflX_X_B_Ref[iObs], reflX_X_B_Err[iObs]);
                                        break;
                                    case 'X':
                                        printf("V %lf(%lf) ", reflCo_X_B_Ref[iObs], reflCo_X_B_Err[iObs]);
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    printf("/ theta = ");
                    for(iSoOp=0; iSoOp<NSoOp; iSoOp++)
                        printf("%lf ", Fixed[iSoOp].thTx*R2D);
                    printf("\n");

                    startSingle = clock();
                    // Adaptive simulated annealing (ASA)
                    asa_SM_VWC(Cost_SM_VWC, iStation);
                    endSingle = clock();
                    // CPU time for ASA
                    timeSingle = ((double) (endSingle - startSingle)) / CLOCKS_PER_SEC;
                    // Outputs for ASA
                    saTime[iCycle] = timeSingle;
                    saIter[iCycle] = SimAnneal->iIter;
                    saJump[iCycle] = SimAnneal->cnt_trapped;
                    saCostMin[iCycle] = SimAnneal->f_opt;   
                    // Retrieved data
                    surPOMERet[iCycle] = SimAnneal->x_opt[0];
                    bottPOMERet[iCycle] = SimAnneal->x_opt[1];
                    avgPOMERet[iCycle] = SimAnneal->x_opt[2];
                    typePOMERet[iCycle] = SMP_POME_main(surPOMERet[iCycle], bottPOMERet[iCycle], avgPOMERet[iCycle]);
                    VWCRet[iCycle] = SimAnneal->x_opt[3];

                    // Retrieved Soil Moisture
                    // Below the top layer in-situ data              
                    for(iLayer=0; iLayer<MultiLayer->NSublayer_POME; iLayer++){
                        soilMoistRet[iCycle*Nc->sublayerDim + iLayer + MultiLayer->NSublayer_extend] = MultiLayer->SMP[iLayer] * (Gnd->poro - Gnd->resid) + Gnd->resid;
                    }
                    // Above the top layer in-situ data
                    for(iLayer=0; iLayer<MultiLayer->NSublayer_extend; iLayer++){
                        soilMoistRet[iCycle*Nc->sublayerDim + iLayer] = soilMoistRet[iCycle*Nc->sublayerDim + MultiLayer->NSublayer_extend];
                    }
                    // Estimated Reflectivity
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                        iObs = iCycle*Nc->soopDim + iSoOp;
                        Bistatic->ArgH = Ret->ArgH[iSoOp];
                        Bistatic->ArgV = Ret->ArgV[iSoOp];

                        // .. Effective Roughness Paramter .. //
                        Gnd->h = Fixed[iSoOp].h;

                        // .. Bistatic Geometry .. //
                        calcTxGeometry(&Fixed[iSoOp]);
                        calcGeometryFixed(&Fixed[iSoOp]);

                        for(iPol=0; iPol<NPol; iPol++){
                            if(iPol==0 || flagMixPol){
                                if(RxPol[iPol] == 'R' || RxPol[iPol] == 'L')
                                    Fixed[iSoOp].polRx = 'R';
                                else if(RxPol[iPol] == 'X' || RxPol[iPol] == 'Y')
                                    Fixed[iSoOp].polRx = 'X';
                                // Forward funciton
                                ForwardScobi_VWC(&Fixed[iSoOp], VWCRet[iCycle]);
                            }
                            if(SttData->ExistVeg){
                                switch (RxPol[iPol]){
                                    case 'L':
                                        reflX_R_V_Est[iObs] = Out->P_coh1[1];
                                        break;
                                    case 'R':
                                        reflCo_R_V_Est[iObs] = Out->P_coh1[0];
                                        break;
                                    case 'Y':
                                        reflX_X_V_Est[iObs] = Out->P_coh1[1];
                                        break;
                                    case 'X':
                                        reflCo_X_V_Est[iObs] = Out->P_coh1[0];
                                        break;
                                    default:
                                        break;
                                }
                            }
                            else{
                                switch (RxPol[iPol]){
                                    case 'L':
                                        reflX_R_B_Est[iObs] = Out->P_coh1[1];
                                        break;
                                    case 'R':
                                        reflCo_R_B_Est[iObs] = Out->P_coh1[0];
                                        break;
                                    case 'Y':
                                        reflX_X_B_Est[iObs] = Out->P_coh1[1];
                                        break;
                                    case 'X':
                                        reflCo_X_B_Est[iObs] = Out->P_coh1[0];
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }   
                    printf("Task %d >> Cycle %ld/%ld done (%d sec)\n", taskid, iCycle+1, Nc->nCycle, (int) timeSingle);
                }
                if(flagQcRet[iCycle] != QC_GOOD){
                    saTime[iCycle] = FILLVALUE;
                    saIter[iCycle] = FILLVALUE;
                    saJump[iCycle] = FILLVALUE;
                    saCostMin[iCycle] = FILLVALUE;  
                    typePOMERet[iCycle] = FILLVALUE;
                    surPOMERet[iCycle] = FILLVALUE;
                    bottPOMERet[iCycle] = FILLVALUE;
                    avgPOMERet[iCycle] = FILLVALUE;
                    VWCRet[iCycle] = FILLVALUE;
                    for(iLayer=0; iLayer<Nc->sublayerDim; iLayer++){
                        soilMoistRet[iCycle*Nc->sublayerDim + iLayer] = FILLVALUE;
                    }
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                        iObs = iCycle*Nc->soopDim + iSoOp;
                        iObsTime[iObs] = FILLVALUE;
                        reflX_R_B_Ref[iObs] = FILLVALUE;
                        reflX_X_B_Ref[iObs] = FILLVALUE;
                        reflX_R_B_Err[iObs] = FILLVALUE;
                        reflX_X_B_Err[iObs] = FILLVALUE;
                        reflX_R_B_Est[iObs] = FILLVALUE;
                        reflX_X_B_Est[iObs] = FILLVALUE;

                        reflX_R_V_Ref[iObs] = FILLVALUE;
                        reflX_X_V_Ref[iObs] = FILLVALUE;
                        reflX_R_V_Err[iObs] = FILLVALUE;
                        reflX_X_V_Err[iObs] = FILLVALUE;
                        reflX_R_V_Est[iObs] = FILLVALUE;
                        reflX_X_V_Est[iObs] = FILLVALUE;
                        
                        reflCo_R_B_Ref[iObs] = FILLVALUE;
                        reflCo_X_B_Ref[iObs] = FILLVALUE;
                        reflCo_R_B_Err[iObs] = FILLVALUE;
                        reflCo_X_B_Err[iObs] = FILLVALUE;
                        reflCo_R_B_Est[iObs] = FILLVALUE;
                        reflCo_X_B_Est[iObs] = FILLVALUE;

                        reflCo_R_V_Ref[iObs] = FILLVALUE;
                        reflCo_X_V_Ref[iObs] = FILLVALUE;
                        reflCo_R_V_Err[iObs] = FILLVALUE;
                        reflCo_X_V_Err[iObs] = FILLVALUE;
                        reflCo_R_V_Est[iObs] = FILLVALUE;
                        reflCo_X_V_Est[iObs] = FILLVALUE;
                        
                        penDepthHRet[iObs] = FILLVALUE;
                        penDepthVRet[iObs] = FILLVALUE;
                    }
                    printf("Task %d >> Cycle %ld/%ld done\n", taskid, iCycle+1, Nc->nCycle);
                }
            }

            // Process residual samples
            if(taskid < Nc->retDim%numtasks){
                // Product time
                iProductTimeResi = period * (taskid + numtasks * Nc->nCycle) + floor(period/2);
                // Quality check for data on the product time
                flagQcRetResi = Nc->flagQcForward[iProductTimeResi];
                if(flagQcRetResi == QC_GOOD){
                    // Random selection of observations (reflectivities)
                    flagAssigned = FALSE;
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){  
                        if(Fixed[iSoOp].Freq == 255E6 || Fixed[iSoOp].Freq == 370E6){
                            // P-band observations occur simultaneously
                            if(flagAssigned){
                                iSample = iSampleMUOS;
                            }
                            else{
                                iSample = iProductTimeResi + UniformRandomDiscrete(randproc, dt_lb, dt_ub);
                                iSampleMUOS = iSample;
                                flagAssigned = TRUE;
                            }
                        }
                        else{
                            iSample = iProductTimeResi + UniformRandomDiscrete(randproc, dt_lb, dt_ub);
                        }
                        // Store observation time
                        iObsTimeResi[iSoOp] = iSample;
                        // Quality check for data on the observation time. If any data is bad, skip this retrieval
                        flagQcRetResi = Nc->flagQcForward[iSample];
                        if(flagQcRetResi == QC_MISSING){
                            printf("Task %d >> Sample %ld : QC_MISSING\n", taskid, iSample);  
                            break;
                        }      
                        else if(flagQcRetResi == QC_FROZEN){
                            printf("Task %d >> Sample %ld : QC_FROZEN\n", taskid, iSample);
                            break;
                        }
                        else if(flagQcRetResi == QC_MISSING + QC_FROZEN){
                            printf("Task %d >> Sample %ld : QC_MISSING + QC_FROZEN\n", taskid, iSample);  
                            break;
                        }                
                    }   
                }
                if(flagQcRetResi == QC_GOOD){
                    // Load observations
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                        iSample = iObsTimeResi[iSoOp];
                        // Update Geometry
                        if(Fixed[iSoOp].Freq == 137E6){	
                            Fixed[iSoOp].thTx = Nc->incORBCOMM[iSample]*D2R;
                            Fixed[iSoOp].ID = 0;
                            iFreq = 0;
                        }
                        else if(Fixed[iSoOp].Freq == 255E6){
                            Fixed[iSoOp].thTx = Nc->incMUOS[iSample]*D2R;
                            Fixed[iSoOp].ID = 1;
                            iFreq = 1;
                        }
                        else if(Fixed[iSoOp].Freq == 370E6){
                            Fixed[iSoOp].thTx = Nc->incMUOS[iSample]*D2R;
                            Fixed[iSoOp].ID = 2;
                            iFreq = 2;
                        }
                        else if(Fixed[iSoOp].Freq == 1575.42E6){
                            Fixed[iSoOp].thTx = Nc->incGPS[iSample]*D2R;
                            Fixed[iSoOp].ID = 3;
                            iFreq = 3;
                        }
                        Fixed[iSoOp].elTx = HalfPi - Fixed[iSoOp].thTx;
                        Fixed[iSoOp].thRx = Fixed[iSoOp].thTx;
                        Fixed[iSoOp].elRx = HalfPi - Fixed[iSoOp].thRx;

                        // Vegetation
                        if(SttData->ExistVeg){
                            Ret->ArgH[iSoOp] = Nc->argHr[iFreq*Nc->sampleDim + iSample] + I1 * Nc->argHi[iFreq*Nc->sampleDim + iSample];
                            Ret->ArgV[iSoOp] = Nc->argVr[iFreq*Nc->sampleDim + iSample] + I1 * Nc->argVi[iFreq*Nc->sampleDim + iSample];

                            for(iPol=0; iPol<NPol; iPol++){
                                iRefl = iSoOp*NPol+iPol;
                                switch (RxPol[iPol]){
                                    case 'L':
                                        Ret->R_X_ref[iRefl] = Nc->reflX_R_V[iFreq*Nc->sampleDim + iSample];
                                        reflX_R_V_RefResi[iSoOp] = Ret->R_X_ref[iRefl];
                                        reflX_R_V_ErrResi[iSoOp] = GaussianRandom(randproc) * reflX_R_V_RefResi[iSoOp] * Ret->stddev;
                                        Ret->R_X_ref[iRefl] += reflX_R_V_ErrResi[iSoOp];
                                        break;
                                    case 'R':
                                        Ret->R_CO_ref[iRefl] = Nc->reflCo_R_V[iFreq*Nc->sampleDim + iSample];
                                        reflCo_R_V_RefResi[iSoOp] = Ret->R_CO_ref[iRefl];
                                        reflCo_R_V_ErrResi[iSoOp] = GaussianRandom(randproc) * reflCo_R_V_RefResi[iSoOp] * Ret->stddev;
                                        Ret->R_CO_ref[iRefl] += reflCo_R_V_ErrResi[iSoOp];
                                        break;
                                    case 'Y':                      
                                        Ret->R_X_ref[iRefl] = Nc->reflX_X_V[iFreq*Nc->sampleDim + iSample];
                                        reflX_X_V_RefResi[iSoOp] = Ret->R_X_ref[iRefl];
                                        reflX_X_V_ErrResi[iSoOp] = GaussianRandom(randproc) * reflX_X_V_RefResi[iSoOp] * Ret->stddev;
                                        Ret->R_X_ref[iRefl] += reflX_X_V_ErrResi[iSoOp];
                                        break;
                                    case 'X':
                                        Ret->R_CO_ref[iRefl] = Nc->reflCo_X_V[iFreq*Nc->sampleDim + iSample];
                                        reflCo_X_V_RefResi[iSoOp] = Ret->R_CO_ref[iRefl];
                                        reflCo_X_V_ErrResi[iSoOp] = GaussianRandom(randproc) * reflCo_X_V_RefResi[iSoOp] * Ret->stddev;
                                        Ret->R_CO_ref[iRefl] += reflCo_X_V_ErrResi[iSoOp]; 
                                        break;
                                    default:
                                        break;
                                }                  
                            }
                        }
                        // Bare soil
                        else{
                            for(iPol=0; iPol<NPol; iPol++){
                                iRefl = iSoOp*NPol+iPol;
                                switch (RxPol[iPol]){
                                    case 'L':
                                        Ret->R_X_ref[iRefl] = Nc->reflX_R_B[iFreq*Nc->sampleDim + iSample];
                                        reflX_R_B_RefResi[iSoOp] = Ret->R_X_ref[iRefl];
                                        reflX_R_B_ErrResi[iSoOp] = GaussianRandom(randproc) * reflX_R_B_RefResi[iSoOp] * Ret->stddev;
                                        Ret->R_X_ref[iRefl] += reflX_R_B_ErrResi[iSoOp];
                                        break;
                                    case 'R':
                                        Ret->R_CO_ref[iRefl] = Nc->reflCo_R_B[iFreq*Nc->sampleDim + iSample];
                                        reflCo_R_B_RefResi[iSoOp] = Ret->R_CO_ref[iRefl];
                                        reflCo_R_B_ErrResi[iSoOp] = GaussianRandom(randproc) * reflCo_R_B_RefResi[iSoOp] * Ret->stddev;
                                        Ret->R_CO_ref[iRefl] += reflCo_R_B_ErrResi[iSoOp];
                                        break;
                                    case 'Y':
                                        Ret->R_X_ref[iRefl] = Nc->reflX_X_B[iFreq*Nc->sampleDim + iSample];
                                        reflX_X_B_RefResi[iSoOp] = Ret->R_X_ref[iRefl];
                                        reflX_X_B_ErrResi[iSoOp] = GaussianRandom(randproc) * reflX_X_B_RefResi[iSoOp] * Ret->stddev;
                                        Ret->R_X_ref[iRefl] += reflX_X_B_ErrResi[iSoOp];
                                        break;
                                    case 'X':
                                        Ret->R_CO_ref[iRefl] = Nc->reflCo_X_B[iFreq*Nc->sampleDim + iSample];
                                        reflCo_X_B_RefResi[iSoOp] = Ret->R_CO_ref[iRefl];
                                        reflCo_X_B_ErrResi[iSoOp] = GaussianRandom(randproc) * reflCo_X_B_RefResi[iSoOp] * Ret->stddev;
                                        Ret->R_CO_ref[iRefl] += reflCo_X_B_ErrResi[iSoOp];
                                        break;
                                    default:
                                        break;
                                }                  
                            }
                        }
                    }

                    // Truth data
                    Ret->sur_ref = Nc->surPOME[iProductTimeResi];
                    Ret->bott_ref = Nc->bottPOME[iProductTimeResi];
                    Ret->avg_ref = Nc->avgPOME[iProductTimeResi];

                    // Load Vegetation Water Content
                    if(SttData->ExistVeg){
                        Ret->VWC_ref = 0;
                        for(iVeg=0; iVeg<NVeg; iVeg++){
                            for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
                                Ret->VWC_ref += Nc->VWCrefmodel[iLayer*Nc->sampleDim + iProductTimeResi];
                            }
                        }
                    }

                    // Print product time and observation times
                    printf("Task %d >> h_product = %ld-%ld / h_obs = ", taskid, Nc->utcDate[iProductTimeResi], Nc->utcTime[iProductTimeResi]);
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                        iObs = iCycle*Nc->soopDim + iSoOp;
                        printf("%ld-%ld ", Nc->utcDate[iObsTimeResi[iSoOp]], Nc->utcTime[iObsTimeResi[iSoOp]]);
                    }
                    printf("\n");
                    // Print truth data
                    printf("Task %d >> Truth: POME = %lf %lf %lf / VWC = %lf\n",
                        taskid, Ret->sur_ref, Ret->bott_ref, Ret->avg_ref, Ret->VWC_ref);
                    // Print observation data
                    printf("Task %d >> Obs: Gamma(Err) = ", taskid);
                    if(SttData->ExistVeg){
                        for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                            for(iPol=0; iPol<NPol; iPol++){
                                switch (RxPol[iPol]){
                                    case 'L':
                                        printf("L %lf(%lf) ", reflX_R_V_RefResi[iSoOp], reflX_R_V_ErrResi[iSoOp]);
                                        break;
                                    case 'R':
                                        printf("R %lf(%lf) ", reflCo_R_V_RefResi[iSoOp], reflCo_R_V_ErrResi[iSoOp]);
                                        break;
                                    case 'Y':
                                        printf("H %lf(%lf) ", reflX_X_V_RefResi[iSoOp], reflX_X_V_ErrResi[iSoOp]);
                                        break;
                                    case 'X':
                                        printf("V %lf(%lf) ", reflCo_X_V_RefResi[iSoOp], reflCo_X_V_ErrResi[iSoOp]);
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    else{
                        for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                            for(iPol=0; iPol<NPol; iPol++){
                                switch (RxPol[iPol]){
                                    case 'L':
                                        printf("L %lf(%lf) ", reflX_R_B_RefResi[iSoOp], reflX_R_B_ErrResi[iSoOp]);
                                        break;
                                    case 'R':
                                        printf("R %lf(%lf) ", reflCo_R_B_RefResi[iSoOp], reflCo_R_B_ErrResi[iSoOp]);
                                        break;
                                    case 'Y':
                                        printf("H %lf(%lf) ", reflX_X_B_RefResi[iSoOp], reflX_X_B_ErrResi[iSoOp]);
                                        break;
                                    case 'X':
                                        printf("V %lf(%lf) ", reflCo_X_B_RefResi[iSoOp], reflCo_X_B_ErrResi[iSoOp]);
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    printf("/ theta = ");
                    for(iSoOp=0; iSoOp<NSoOp; iSoOp++)
                        printf("%lf ", Fixed[iSoOp].thTx*R2D);
                    printf("\n");

                    startSingle = clock();
                    // Adaptive simulated annealing (ASA)
                    asa_SM_VWC(Cost_SM_VWC, iStation);
                    endSingle = clock();
                    // CPU time for ASA
                    timeSingle = ((double) (endSingle - startSingle)) / CLOCKS_PER_SEC;
                    // Outputs for ASA
                    saTimeResi = timeSingle;
                    saIterResi = SimAnneal->iIter;
                    saJumpResi = SimAnneal->cnt_trapped;
                    saCostMinResi = SimAnneal->f_opt;
                    // Retrieved data
                    surPOMERetResi = SimAnneal->x_opt[0];
                    bottPOMERetResi = SimAnneal->x_opt[1];
                    avgPOMERetResi = SimAnneal->x_opt[2];
                    typePOMERetResi = SMP_POME_main(surPOMERetResi, bottPOMERetResi, avgPOMERetResi);
                    VWCRetResi = SimAnneal->x_opt[3];
                    
                    // Retrieved soil moisture
                    // Below the top layer in-situ data
                    for(iLayer=0; iLayer<MultiLayer->NSublayer_POME; iLayer++){
                        soilMoistRetResi[iLayer + MultiLayer->NSublayer_extend] = MultiLayer->SMP[iLayer] * (Gnd->poro - Gnd->resid) + Gnd->resid;
                    }
                    // Above the top layer in-situ data
                    for(iLayer=0; iLayer<MultiLayer->NSublayer_extend; iLayer++){
                        soilMoistRetResi[iLayer] = soilMoistRetResi[MultiLayer->NSublayer_extend];
                    }
                    //GndData->flagPenDep = TRUE;
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                        Bistatic->ArgH = Ret->ArgH[iSoOp];
                        Bistatic->ArgV = Ret->ArgV[iSoOp];
                        // .. Effective Roughness Paramter .. //
                        Gnd->h = Fixed[iSoOp].h;
                        
                        // .. Bistatic Geometry .. //
                        calcTxGeometry(&Fixed[iSoOp]);
                        calcGeometryFixed(&Fixed[iSoOp]);

                        for(iPol=0; iPol<NPol; iPol++){
                            if(iPol==0 || flagMixPol){
                                if(RxPol[iPol] == 'R' || RxPol[iPol] == 'L')
                                    Fixed[iSoOp].polRx = 'R';
                                else if(RxPol[iPol] == 'X' || RxPol[iPol] == 'Y')
                                    Fixed[iSoOp].polRx = 'X';
                                // Forward funciton
                                ForwardScobi_VWC(&Fixed[iSoOp], VWCRetResi);
                            } 
                            if(SttData->ExistVeg){
                                switch (RxPol[iPol]){
                                    case 'L':
                                        reflX_R_V_EstResi[iSoOp] = Out->P_coh1[1];
                                        break;
                                    case 'R':
                                        reflCo_R_V_EstResi[iSoOp] = Out->P_coh1[0];
                                        break;
                                    case 'Y':
                                        reflX_X_V_EstResi[iSoOp] = Out->P_coh1[1];
                                        break;
                                    case 'X':
                                        reflCo_X_V_EstResi[iSoOp] = Out->P_coh1[0];
                                        break;
                                    default:
                                        break;
                                }
                            }
                            else{
                                switch (RxPol[iPol]){
                                    case 'L':
                                        reflX_R_B_EstResi[iSoOp] = Out->P_coh1[1];
                                        break;
                                    case 'R':
                                        reflCo_R_B_EstResi[iSoOp] = Out->P_coh1[0];
                                        break;
                                    case 'Y':
                                        reflX_X_B_EstResi[iSoOp] = Out->P_coh1[1];
                                        break;
                                    case 'X':
                                        reflCo_X_B_EstResi[iSoOp] = Out->P_coh1[0];
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    printf("Task %d >> Residual sample %d/%ld done (%d sec)\n", taskid, taskid+1, Nc->retDim%numtasks, (int) timeSingle);
                }
                else{
                    printf("Task %d >> Residual sample %d/%ld done\n", taskid, taskid+1, Nc->retDim%numtasks);
                    saTimeResi = FILLVALUE;
                    saIterResi = FILLVALUE;
                    saJumpResi = FILLVALUE;
                    saCostMinResi = FILLVALUE;
                    typePOMERetResi = FILLVALUE;
                    surPOMERetResi = FILLVALUE;
                    bottPOMERetResi = FILLVALUE;
                    avgPOMERetResi = FILLVALUE;
                    VWCRetResi = FILLVALUE;
                    for(iLayer=0; iLayer<Nc->sublayerDim; iLayer++){
                        soilMoistRetResi[iLayer] = FILLVALUE;
                    }
                    for(iSoOp=0; iSoOp<Nc->soopDim; iSoOp++){
                        iObsTimeResi[iSoOp] = FILLVALUE;
                        reflX_R_B_RefResi[iSoOp] = FILLVALUE;
                        reflX_X_B_RefResi[iSoOp] = FILLVALUE;
                        reflX_R_B_EstResi[iSoOp] = FILLVALUE;
                        reflX_X_B_EstResi[iSoOp] = FILLVALUE;
                        reflX_R_B_ErrResi[iSoOp] = FILLVALUE;
                        reflX_X_B_ErrResi[iSoOp] = FILLVALUE;

                        reflX_R_V_RefResi[iSoOp] = FILLVALUE;
                        reflX_X_V_RefResi[iSoOp] = FILLVALUE;
                        reflX_R_V_EstResi[iSoOp] = FILLVALUE;
                        reflX_X_V_EstResi[iSoOp] = FILLVALUE;
                        reflX_R_V_ErrResi[iSoOp] = FILLVALUE;
                        reflX_X_V_ErrResi[iSoOp] = FILLVALUE;
                        
                        reflCo_R_B_RefResi[iSoOp] = FILLVALUE;
                        reflCo_X_B_RefResi[iSoOp] = FILLVALUE;
                        reflCo_R_B_EstResi[iSoOp] = FILLVALUE;
                        reflCo_X_B_EstResi[iSoOp] = FILLVALUE;
                        reflCo_R_B_ErrResi[iSoOp] = FILLVALUE;
                        reflCo_X_B_ErrResi[iSoOp] = FILLVALUE;

                        reflCo_R_V_RefResi[iSoOp] = FILLVALUE;
                        reflCo_X_V_RefResi[iSoOp] = FILLVALUE;
                        reflCo_R_V_EstResi[iSoOp] = FILLVALUE;
                        reflCo_X_V_EstResi[iSoOp] = FILLVALUE;
                        reflCo_R_V_ErrResi[iSoOp] = FILLVALUE;
                        reflCo_X_V_ErrResi[iSoOp] = FILLVALUE;
                        
                        penDepthHRetResi[iSoOp] = FILLVALUE;
                        penDepthVRetResi[iSoOp] = FILLVALUE;
                    }
                }
            }
            MPI_Gather(flagQcRet, Nc->nCycle, MPI_INT, Nc->flagQcRet, Nc->nCycle, MPI_INT, MASTER, MPI_COMM_WORLD);
            MPI_Gather(iProductTime, Nc->nCycle, MPI_INT, Nc->iProductTime, Nc->nCycle, MPI_INT, MASTER, MPI_COMM_WORLD);
            MPI_Gather(iObsTime, Nc->nCycle*Nc->soopDim, MPI_INT, Nc->iObsTime, Nc->nCycle*Nc->soopDim, MPI_INT, MASTER, MPI_COMM_WORLD);

            MPI_Gather(reflX_R_B_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_R_B_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_X_B_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_X_B_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_R_B_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_R_B_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_X_B_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_X_B_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_R_B_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_R_B_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_X_B_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_X_B_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

            MPI_Gather(reflX_R_V_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_R_V_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_X_V_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_X_V_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_R_V_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_R_V_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_X_V_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_X_V_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_R_V_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_R_V_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflX_X_V_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflX_X_V_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            
            MPI_Gather(reflCo_R_B_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_R_B_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_X_B_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_X_B_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_R_B_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_R_B_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_X_B_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_X_B_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_R_B_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_R_B_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_X_B_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_X_B_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

            MPI_Gather(reflCo_R_V_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_R_V_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_X_V_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_X_V_Ref, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_R_V_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_R_V_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_X_V_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_X_V_Est, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_R_V_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_R_V_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(reflCo_X_V_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->reflCo_X_V_Err, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            
            MPI_Gather(penDepthHRet, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->penDepthHRet, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(penDepthVRet, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, Nc->penDepthVRet, Nc->nCycle*Nc->soopDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(soilMoistRet, Nc->nCycle*Nc->sublayerDim, MPI_DOUBLE, Nc->soilMoistRet, Nc->nCycle*Nc->sublayerDim, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(typePOMERet, Nc->nCycle, MPI_INT, Nc->typePOMERet, Nc->nCycle, MPI_INT, MASTER, MPI_COMM_WORLD);
            MPI_Gather(surPOMERet, Nc->nCycle, MPI_DOUBLE, Nc->surPOMERet, Nc->nCycle, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(bottPOMERet, Nc->nCycle, MPI_DOUBLE, Nc->bottPOMERet, Nc->nCycle, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(avgPOMERet, Nc->nCycle, MPI_DOUBLE, Nc->avgPOMERet, Nc->nCycle, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(VWCRet, Nc->nCycle, MPI_DOUBLE, Nc->VWCRet, Nc->nCycle, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(saTime, Nc->nCycle, MPI_DOUBLE, Nc->saTime, Nc->nCycle, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            MPI_Gather(saIter, Nc->nCycle, MPI_LONG, Nc->saIter, Nc->nCycle, MPI_LONG, MASTER, MPI_COMM_WORLD);
            MPI_Gather(saJump, Nc->nCycle, MPI_LONG, Nc->saJump, Nc->nCycle, MPI_LONG, MASTER, MPI_COMM_WORLD);
            MPI_Gather(saCostMin, Nc->nCycle, MPI_DOUBLE, Nc->saCostMin, Nc->nCycle, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);


            if(taskid < Nc->retDim%numtasks){   
                for(iTask=0; iTask< Nc->retDim%numtasks; iTask++){
                    recvcount1[iTask] = Nc->soopDim;
                    recvcount2[iTask] = Nc->sublayerDim;
                    recvcount3[iTask] = 1;
                }
                for(iTask=1; iTask<numtasks; iTask++){
                    displs1[iTask] = displs1[iTask-1] + recvcount1[iTask];
                    displs2[iTask] = displs2[iTask-1] + recvcount2[iTask];
                    displs3[iTask] = displs3[iTask-1] + recvcount3[iTask];
                }
                MPI_Gatherv(&flagQcRetResi, 1, MPI_INT, &Nc->flagQcRet[Nc->nCycle*numtasks], recvcount3, displs3, MPI_INT, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&iProductTimeResi, 1, MPI_INT, &Nc->iProductTime[Nc->nCycle*numtasks], recvcount3, displs3, MPI_INT, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(iObsTimeResi, Nc->soopDim, MPI_INT, &Nc->iObsTime[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_INT, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_R_B_RefResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_R_B_Ref[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_X_B_RefResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_X_B_Ref[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_R_B_EstResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_R_B_Est[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_X_B_EstResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_X_B_Est[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_R_B_ErrResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_R_B_Err[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_X_B_ErrResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_X_B_Err[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

                MPI_Gatherv(reflX_R_V_RefResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_R_V_Ref[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_X_V_RefResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_X_V_Ref[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_R_V_EstResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_R_V_Est[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_X_V_EstResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_X_V_Est[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_R_V_ErrResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_R_V_Err[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflX_X_V_ErrResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflX_X_V_Err[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                
                MPI_Gatherv(reflCo_R_B_RefResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_R_B_Ref[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_X_B_RefResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_X_B_Ref[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_R_B_EstResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_R_B_Est[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_X_B_EstResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_X_B_Est[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_R_B_ErrResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_R_B_Err[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_X_B_ErrResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_X_B_Err[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

                MPI_Gatherv(reflCo_R_V_RefResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_R_V_Ref[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_X_V_RefResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_X_V_Ref[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_R_V_EstResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_R_V_Est[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_X_V_EstResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_X_V_Est[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_R_V_ErrResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_R_V_Err[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(reflCo_X_V_ErrResi, Nc->soopDim, MPI_DOUBLE, &Nc->reflCo_X_V_Err[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                
                MPI_Gatherv(penDepthHRetResi, Nc->soopDim, MPI_DOUBLE, &Nc->penDepthHRet[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(penDepthVRetResi, Nc->soopDim, MPI_DOUBLE, &Nc->penDepthVRet[Nc->nCycle*Nc->soopDim*numtasks], recvcount1, displs1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(soilMoistRetResi, Nc->sublayerDim, MPI_DOUBLE, &Nc->soilMoistRet[Nc->nCycle*Nc->sublayerDim*numtasks], recvcount2, displs2, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&typePOMERetResi, 1, MPI_INT, &Nc->typePOMERet[Nc->nCycle*numtasks], recvcount3, displs3, MPI_INT, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&surPOMERetResi, 1, MPI_DOUBLE, &Nc->surPOMERet[Nc->nCycle*numtasks], recvcount3, displs3, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&bottPOMERetResi, 1, MPI_DOUBLE, &Nc->bottPOMERet[Nc->nCycle*numtasks], recvcount3, displs3, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&avgPOMERetResi, 1, MPI_DOUBLE, &Nc->avgPOMERet[Nc->nCycle*numtasks], recvcount3, displs3, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&VWCRetResi, 1, MPI_DOUBLE, &Nc->VWCRet[Nc->nCycle*numtasks], recvcount3, displs3, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&saTimeResi, 1, MPI_DOUBLE, &Nc->saTime[Nc->nCycle*numtasks], recvcount3, displs3, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&saIterResi, 1, MPI_LONG, &Nc->saIter[Nc->nCycle*numtasks], recvcount3, displs3, MPI_LONG, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&saJumpResi, 1, MPI_LONG, &Nc->saJump[Nc->nCycle*numtasks], recvcount3, displs3, MPI_LONG, MASTER, MPI_COMM_WORLD);
                MPI_Gatherv(&saCostMinResi, 1, MPI_DOUBLE, &Nc->saCostMin[Nc->nCycle*numtasks], recvcount3, displs3, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            }
            if(taskid == MASTER){        
                endTotal = time(NULL);
                Nc->runtime = difftime(endTotal, startTotal);
                printf(">> Writing retrieval results to NetCDF file... ");
                writeNcRet();
                printf("Simulation Complete!\n");
            }
        }
        rewind(stationFile);
    }
    fclose(stationFile);
}