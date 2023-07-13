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

/**********************************************************************/
/*  Schatten Model Solar Flux KP and AP data   */
void SchattenAtmoParam(void)
{
	static long First=1;
	static double JD[323], F10[323], Ap[323];

	FILE *Schattenfile;
    char junk[120],newline;

    double normMeanF10[323], normPlusTwoSigF10[323], normMinusTwoSigF10[323], normAp[323];
    double earlyMeanF10[323], earlyPlusTwoSigF10[323], earlyMinusTwoSigF10[323], earlyAp[323];
    double lateMeanF10[323], latePlusTwoSigF10[323], lateMinusTwoSigF10[323], lateAp[323];
    double fileyear,filemonth;
    long n;

	/* Get F10.7 and AP data from SchattenPredict.txt */
	if (First){
		First=0;
		Schattenfile = FileOpen(ModelPath,"SchattenPredict.txt","r");
		if (Schattenfile==NULL){
			fprintf(Schattenfile,"Unable to open Schatten file.");
		}
		else{
			/* Save Date and KP info for use in interpolation */
			fscanf(Schattenfile,"%[^\n] %[\n]",junk,&newline);
			fscanf(Schattenfile,"%[^\n] %[\n]",junk,&newline);
			fscanf(Schattenfile,"%[^\n] %[\n]",junk,&newline);
			for(n=0;n<323;n++){
				fscanf(Schattenfile,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %[\n]",
					   &filemonth,&fileyear,&normMeanF10[n],&normPlusTwoSigF10[n],&normMinusTwoSigF10[n],&normAp[n],
					   &earlyMeanF10[n],&earlyPlusTwoSigF10[n],&earlyMinusTwoSigF10[n],&earlyAp[n],
					   &lateMeanF10[n],&latePlusTwoSigF10[n],&lateMinusTwoSigF10[n],&lateAp[n],&newline);
				JD[n] = YMDHMS2JD(fileyear, filemonth, 01, 12, 00, 00);
				/* .. Use of Schatten error model for Solar Flux and AP values */
				if (SchattenTimingModel == NOMINAL_TIMING){
					Ap[n] = normAp[n];
					if(SchattenErrorModel == MEAN_F10)
					   F10[n] = normMeanF10[n];
					else if(SchattenErrorModel == PLUSTWOSIG_F10)
					   F10[n] = normPlusTwoSigF10[n];
					else if(SchattenErrorModel == MINUSTWOSIG_F10)
					   F10[n] = normMinusTwoSigF10[n];
				}
				else if (SchattenTimingModel == EARLY_TIMING){
					Ap[n] = earlyAp[n];
					if(SchattenErrorModel == MEAN_F10)
					   F10[n] = earlyMeanF10[n];
					else if(SchattenErrorModel == PLUSTWOSIG_F10)
					   F10[n] = earlyPlusTwoSigF10[n];
					else if(SchattenErrorModel == MINUSTWOSIG_F10)
					   F10[n] = earlyMinusTwoSigF10[n];
				}
				else if (SchattenTimingModel == LATE_TIMING){
					Ap[n] = lateAp[n];
					if(SchattenErrorModel == MEAN_F10)
					   F10[n] = lateMeanF10[n];
					else if(SchattenErrorModel == PLUSTWOSIG_F10)
					   F10[n] = latePlusTwoSigF10[n];
					else if(SchattenErrorModel == MINUSTWOSIG_F10)
					   F10[n] = lateMinusTwoSigF10[n];
				}
			}
		}
		fclose(Schattenfile);
	}

	GeomagIndex=round(LinInterp(JD, Ap, TT.JulDay, 323));

	/*Interpolate date and solar flux info to find current F10.7 value*/
	Flux10p7=LinInterp(JD, F10, TT.JulDay, 323);
}

void LoadFreezeThawState(void)
{
	FILE * pFile;
	long lSize;
	size_t result;
	char filename[12][60];

	sprintf(filename[0], "SMAP_L3_FT_P_E_20190115_R16022_001_FT_AM.uint8");
	sprintf(filename[1], "SMAP_L3_FT_P_E_20190215_R16022_001_FT_AM.uint8");
	sprintf(filename[2], "SMAP_L3_FT_P_E_20190315_R16022_001_FT_AM.uint8");
	sprintf(filename[3], "SMAP_L3_FT_P_E_20190415_R16022_001_FT_AM.uint8");
	sprintf(filename[4], "SMAP_L3_FT_P_E_20190515_R16022_001_FT_AM.uint8");
	sprintf(filename[5], "SMAP_L3_FT_P_E_20190615_R16022_002_FT_AM.uint8");
	sprintf(filename[6], "SMAP_L3_FT_P_E_20190726_R16022_001_FT_AM.uint8");
	sprintf(filename[7], "SMAP_L3_FT_P_E_20190815_R16510_001_FT_AM.uint8");
	sprintf(filename[8], "SMAP_L3_FT_P_E_20190915_R16510_001_FT_AM.uint8");
	sprintf(filename[9], "SMAP_L3_FT_P_E_20191015_R16510_001_FT_AM.uint8");
	sprintf(filename[10], "SMAP_L3_FT_P_E_20191115_R16510_001_FT_AM.uint8");
	sprintf(filename[11], "SMAP_L3_FT_P_E_20191215_R16510_001_FT_AM.uint8");

	/*.. Freeze/thaw State ..*/
	printf(">> Loading freeze/thaw state %ld ...", UTC.Month);
	pFile = FileOpen(DynDataPath, filename[UTC.Month-1], "r" );
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);
	// copy the file into the buffer:
	result = fread (DynData->freezeThaw, sizeof(uint8_t), lSize/sizeof(uint8_t), pFile);
	if (result != lSize/sizeof(uint8_t)) {fputs ("Reading error",stderr); exit (3);}
	fclose (pFile);
	printf("Success!\n");
}

/* Load static data of surface */
void LoadStaticData(void)
{
	FILE * pFile;
	long lSize;
	size_t result;
	double lat, lon;
	uint16_t row, col;

	/* DEM EARTH2014 1 arc-min resolution */
	if(SttData->ExistDEM){
		printf(">> Loading digital elevation model ..."); fflush(stdout);
		pFile = FileOpen(SttDataPath, SttData->filename_DEM, "r" );
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		// copy the file into the buffer:
		result = fread (SttData->height,2,lSize/2,pFile);
		if (result != lSize/2) {fputs ("DEM file reading error",stderr); exit (3);}
		fclose (pFile);
		// Sanity Check
		lat = 35.1917;
		lon = 172.3417;
		row = (uint16_t) floor(lat*60 + 5400);
		col = (uint16_t) floor(lon*60 + 10800);
		//row = 5399.9999 + lat*60;//(90+lat)*60-0.001;
		//col = 10799.9999 + lon*60;//(lon+180)*60-0.001;
		if(SttData->height[row][col]!=65) {
			fputs ("Fail",stderr); exit (1);
		}
		else
			printf("Success!\n");
	}

	/* Land Mask - 1km EASE grid 2 */
	if(SttData->ExistLandMask){
		printf(">> Loading land mask ..."); fflush(stdout);
		pFile = FileOpen(SttDataPath, SttData->filename_landMask, "r" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		// copy the file into the buffer:
		result = fread (SttData->landMask, 1, lSize, pFile);
		if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
		fclose (pFile);
		// Sanity Check
		lat = 65.415679931640620*D2R;
		lon = 22.432573318481445*D2R;
		LL2EzRC1km(lat, lon, &row, &col);
		if(SttData->landMask[row][col]!=1){
			printf("Fail\n");
			exit(1);
		}
		else
			printf("Success!\n");
	}

	/*.. Land Type ..*/
	if(SttData->ExistLandCover){
		printf(">> Loading land cover ..."); fflush(stdout);
		pFile = FileOpen(SttDataPath, SttData->filename_landType, "r" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		// copy the file into the buffer:
		result = fread (SttData->landType, 1, lSize, pFile);
		if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
		fclose (pFile);
		// Sanity Check
		lat = 11.4499;
		lon = 36.7001;
		row = 1799.9999 - lat*20;//(90-lat)*20;
		col = 3599.9999 + lon*20;//(lon+180)*20;
		if(SttData->landType[row][col]!=9){
			printf("Fail\n");
			exit(1);
		}
		else
			printf("Success!\n");
	}

	// .. Soil Texture .. 
	if(SttData->ExistSoilTexture){
		printf(">> Loading soil texture ...\n");
		// .. Soil - Sand ..
		pFile = FileOpen(SttDataPath, SttData->filename_sand, "r" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		// copy the file into the buffer:
		result = fread(SttData->sand, sizeof(float), lSize/sizeof(float), pFile);
		if (result != lSize/sizeof(float)) {fputs ("Reading error",stderr); exit (3);}
		fclose (pFile);
		printf(">> - sand ratio - Success!\n");

		// .. Soil - Clay ..
		pFile = FileOpen(SttDataPath, SttData->filename_clay, "r" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		// copy the file into the buffer:
		result = fread(SttData->clay, sizeof(float), lSize/sizeof(float), pFile);
		if (result != lSize/sizeof(float)) {fputs ("Reading error",stderr); exit (3);}
		fclose (pFile);
		printf(">> - clay ratio - Success!\n");

		// .. Soil - Bulk Density ..
		pFile = FileOpen(SttDataPath, SttData->filename_rho_b, "r" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		// copy the file into the buffer:
		result = fread(SttData->bulk, sizeof(float), lSize/sizeof(float), pFile);
		if (result != lSize/sizeof(float)) {fputs ("Reading error",stderr); exit (3);}
		fclose (pFile);
		printf(">> - bulk density - Success!\n");
	}

	// .. Soil Porosity .. ///
	if(SttData->ExistSoilPorosity){
		printf(">> Loading soil porosity ...\n");
		pFile = FileOpen(SttDataPath, SttData->filename_porosity, "r" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		// copy the file into the buffer:
		result = fread(SttData->porosity, sizeof(float), lSize/sizeof(float), pFile);
		if (result != lSize/sizeof(float)) {fputs ("Reading error",stderr); exit (3);}
		fclose (pFile);
		printf(">> - sand porosity - Success!\n");
	}
}

void LoadSoilData(void)
{
	FILE *pFile;
	long lSize, ILayer;
	size_t result;

	/*.. Soil Moisture ..*/
	if(DynData->ExistVSM){
		printf(">> Loading soil moisture ...\n");
		for(ILayer = 0; ILayer<NLayer; ILayer++){
			pFile = FileOpen(DynDataPath, GndData[ILayer].filename, "r" );
			if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
			// obtain file size:
			fseek (pFile , 0 , SEEK_END);
			lSize = ftell (pFile);
			rewind (pFile);
			// copy the file into the buffer:
			result = fread (GndData[ILayer].VSM, sizeof(float), lSize/sizeof(float), pFile);
			if (result != lSize/sizeof(float)) {fputs ("Reading error",stderr); exit (3);}
			fclose (pFile);
			printf(">> - Layer %ld - Success!\n", ILayer+1);
		}
	}

	/*.. Soil Temperature ..*/
	if(DynData->ExistSoilTemp){
		printf(">> Loading soil temperature ..."); fflush(stdout);
		pFile = FileOpen(DynDataPath, DynData->filename_soilTemp1, "r" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		// copy the file into the buffer:
		result = fread (DynData->soilTemp1, sizeof(float), lSize/sizeof(float), pFile);
		if (result != lSize/sizeof(float)) {fputs ("Reading error",stderr); exit (3);}
		fclose (pFile);
		printf("Success!\n");
	}
}

void LoadSnowCover(void)
{
	FILE * pFile;
	long lSize;
	size_t result;
//	double lat, lon;
//	uint16_t row, col;

	/*.. Snow Cover ..*/
	printf(">> Loading snow cover %ld ...", UTC.Month); fflush(stdout);
	pFile = FileOpen(DynDataPath, DynData->filename_snowCover[UTC.Month-1], "r" );
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// copy the file into the buffer:
	result = fread (DynData->snowCover, 1, lSize, pFile);
	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

	// terminate
	fclose (pFile);

	// Sanity Check
//	lat = 70.525;
//	lon = 56.475;
//	row = 1799.9999 - lat*20;//(90-lat)*20;
//	col = 3599.9999 + lon*20;//(lon+180)*20;

//	if(Surface->snow[row][col]!=6){
//		printf("Error loading snow mask\n");
//		exit(1);
//	}
//	else
		//printf("(%d,%d) = %d\n", row, col, LandMask[row][col]);
		printf("Success!\n");
}

void UpdateOrbitCycle(void)
{
/*	SC[0].Ncycle = floor(SimTime/Orb[0].Period)+1;
	SC[1].Ncycle = floor(SimTime/Orb[1].Period)+1;
	SC[2].Ncycle = floor(SimTime/Orb[2].Period)+1;
	SC[3].Ncycle = floor(SimTime/Orb[3].Period)+1;
	SC[4].Ncycle = floor(SimTime/Orb[4].Period)+1;
	SC[5].Ncycle = floor(SimTime/Orb[5].Period)+1;
	SC[6].Ncycle = floor(SimTime/Orb[6].Period)+1;
	SC[7].Ncycle = floor(SimTime/Orb[7].Period)+1;
	SC[8].Ncycle = floor(SimTime/Orb[8].Period)+1;
	SC[9].Ncycle = floor(SimTime/Orb[9].Period)+1;
	SC[10].Ncycle = floor(SimTime/Orb[10].Period)+1;
	SC[11].Ncycle = floor(SimTime/Orb[11].Period)+1;
	SC[12].Ncycle = floor(SimTime/Orb[12].Period)+1;
	SC[13].Ncycle = floor(SimTime/Orb[13].Period)+1;
	SC[14].Ncycle = floor(SimTime/Orb[14].Period)+1;
	SC[15].Ncycle = floor(SimTime/Orb[15].Period)+1;
*/
}

void UpdateMonthlyData(void)
{
	if(UTC.Day==1 && UTC.Hour==0 && UTC.Minute==0 && UTC.Second==0){
        if(DynData->ExistSnowCover){
        	LoadSnowCover();
		}
        if(DynData->ExistFreezeThaw){
        	LoadFreezeThawState();
		}
	}
}

void UpdateCalvalData(void)
{
	double jd;

	jd = YMDHMS2JD(2016, UTC.Month, UTC.Day, UTC.Hour+6, UTC.Minute, UTC.Second);
	
	if(TargetArea[0].jd[Icalval+1] == jd){
		Icalval++;
	}
}

void LoadCalvalSoilData(long ITarget)
{
	FILE *pFile;
	long iVSM;
	char newline, filename[50], junk[120];

	sprintf(filename, "SM_%s.txt", TargetArea[ITarget].name);
	pFile = FileOpen(DynDataPath, filename, "r");
	fscanf(pFile,"%[^\n] %[\n]",junk,&newline); // Skip first line

	for(iVSM=0; iVSM<Icalval; iVSM++){
		fscanf(pFile,"%[^\n] %[\n]",junk,&newline); // Skip lines
	}
	fscanf(pFile,"%lf %f %f %[\n]",
				&TargetArea[ITarget].jd[iVSM], &Gnd->VSM[0], &Gnd->VSM[1], &newline);	
	fclose(pFile);
	
	printf("ITarget = %ld iVSM = %ld, jd = %lf, %f, %f\n", ITarget, iVSM, TargetArea[ITarget].jd[iVSM], Gnd->VSM[0], Gnd->VSM[1]);
}

/* Convert WGS84 geodetic coordinates to row, column index for 1km resolution EASE grid 2.0
   Input: lat, lon in radian */
void LL2EzRC1km(double lat, double lon, uint16_t *row, uint16_t *col)
{
	double e = 0.081819190842621; // eccentricity sqrt(2f-f^2)
	double c = 1000.90; // interdistance pixel
	//double nc = 34704; // Number of columns
	//double nr = 14616; // Number of lines
	double s0 = 7307.5; //(nr-1)/2;
	double r0 = 17351.5; //(nc-1)/2;
	double q, x, y;
	double aXk0 = 5528256.639292833; // a*k0
	double e2 = 0.006694379990141; // e*e
	double oneMinuse2 = 0.993305620009859; // 1-e2
	double oneOver2e = 6.111035746634866; // 1/(2*e)
	double aOver2k0 = 3679336.384424151; // a/(2*k0)
	double sinLat = sin(lat);
//	double a = 6378137;
//	double phi1 = 30*D2R;
//	double k0 = cos(phi1) / sqrt(1 - (e*e*sin(phi1)*sin(phi1)));

//	q = (1-e*e) * ( (sin(lat)/(1-e*e*sin(lat)*sin(lat))) - (1/(2*e))*log((1-e*sin(lat))/(1+e*sin(lat))) );
//	x= a*k0 * lon;
//	y= q * a/(2*k0);

	q = oneMinuse2 * ( (sinLat/(1-e2*sinLat*sinLat)) - oneOver2e*log((1-e*sinLat)/(1+e*sinLat)) );
	x= aXk0 * lon;
	y= q * aOver2k0;

	*col = round(r0 + (x/c)); // as Brodzik et al formula => r [0 1387]
	*row = round(s0 - (y/c)); // as Brodzik et al. formula => s [0 583] +1 included in nl
}

/* Convert WGS84 geodetic coordinates to row, column index for 9km resolution EASE grid 2.0
   Input: lat, lon in radian */
void LL2EzRC9km(double lat, double lon, uint16_t *row, uint16_t *col)
{
	double e = 0.081819190842621; // eccentricity sqrt(2f-f^2)
	double c = 9008.05; // interdistance pixel
	//double nc = 3856; // Number of columns
	//double nr = 1624; // Number of lines
	double s0 = 811.5; //(nr-1)/2;
	double r0 = 1927.5; //(nc-1)/2;
	double q, x, y;
	double aXk0 = 5528256.639292833; // a*k0
	double e2 = 0.006694379990141; // e*e
	double oneMinuse2 = 0.993305620009859; // 1-e2
	double oneOver2e = 6.111035746634866; // 1/(2*e)
	double aOver2k0 = 3679336.384424151; // a/(2*k0)
	double sinLat = sin(lat);
//	double a = 6378137;
//	double phi1 = 30*D2R;
//	double k0 = cos(phi1) / sqrt(1 - (e*e*sin(phi1)*sin(phi1)));

//	q = (1-e*e) * ( (sin(lat)/(1-e*e*sin(lat)*sin(lat))) - (1/(2*e))*log((1-e*sin(lat))/(1+e*sin(lat))) );
//	x= a*k0 * lon;
//	y= q * a/(2*k0);

	q = oneMinuse2 * ( (sinLat/(1-e2*sinLat*sinLat)) - oneOver2e*log((1-e*sinLat)/(1+e*sinLat)) );
	x= aXk0 * lon;
	y= q * aOver2k0;

	*col = round(r0 + (x/c)); // as Brodzik et al formula => r [0 1387]
	*row = round(s0 - (y/c)); // as Brodzik et al. formula => s [0 583] +1 included in nl
}


/* Convert WGS84 geodetic coordinates to row, column index for 9km resolution EASE grid 2.0
   Input: lat, lon in radian */
void LL2EzRC(struct GridType *G, double lat, double lon, uint16_t *row, uint16_t *col)
{
	double e = 0.081819190842621; // eccentricity sqrt(2f-f^2)
	double q, x, y;
	double aXk0 = 5528256.639292833; // a*k0
	double e2 = 0.006694379990141; // e*e
	double oneMinuse2 = 0.993305620009859; // 1-e2
	double oneOver2e = 6.111035746634866; // 1/(2*e)
	double aOver2k0 = 3679336.384424151; // a/(2*k0)
	double sinLat = sin(lat);

	q = oneMinuse2 * ( (sinLat/(1-e2*sinLat*sinLat)) - oneOver2e*log((1-e*sinLat)/(1+e*sinLat)) );
	x = aXk0 * lon;
	y = q * aOver2k0;

	*col = round((x-G->x0)/G->pixelSize);
	*row = round((G->y0-y)/G->pixelSize);
}
