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

void Welcome(void)
{
	long ILayer;
	printf("/***************************************************************************/\n");
	printf("/*                                                                         */\n");
	printf("/*                                SOCRATES                                 */\n");
	printf("/*                                                                         */\n");
	printf("/*  SoOp Constellation and Remote sensing Analysis Tool for Earth Science  */\n");
	printf("/*                                                                         */\n");
	printf("/***************************************************************************/\n");
	printf(">> Number of Input Soil Layers is %ld\n", NLayer);
	if(GndModel == MULTI){
		if(SmpModel == EXPO){
			printf(">> Ground Multi-layered Structure: EXPO model\n");
			printf(">> |                              |\n");
			printf(">> |              Air             | %.3lf m\n", MultiLayer->zA);
			printf(">> |------------------------------|-- 0 m\n");
			for(ILayer=0; ILayer<NLayer; ILayer++){
				printf(">> |            Layer %ld           | %.3lf m\n", ILayer, GndData[ILayer].layer_thickness);
				printf(">> |------------------------------|-- %.3lf m\n", GndData[ILayer].layer_bottom);
			}
		}
		else if(SmpModel == POME){ 
			printf(">> Ground Multi-layered Structure: POME model, # sublayers = %ld + %ld\n", MultiLayer->NSublayer_POME, MultiLayer->NSublayer_extend);
		}
	}

	if(ObsMode == FIXED){
		printf(">> Initiate Fixed Geometry Observation ...\n");
	}
	else if(ObsMode == INORBIT){
		printf(">> Initiate In-Orbit Observation ...\n");
		printf(">> - Number of Receivers : %ld\n", NRx);
		printf(">> - Number of Transmitter Systems : %ld\n", NSoOp);
		/* .. Initialize Orbit Propagator .. */
		if(OrbModel == ORB_IMPORT){
			printf(">> - Orbit Propagator : IMPORT\n");
			printf(">> - Number of Data : %ld\n", NData);
		}
		else{
			if(OrbModel == ORB_42)
				printf(">> - Orbit Propagator : 42\n");
			else if(OrbModel == ORB_42_DECAY)
				printf(">> - Orbit Propagator : 42_DECAY\n");
		}
	}
}

void freeInputSoOp(void)
{
	long iSoOp;
	if(ObsMode == FIXED){
		free(InputSoOp->F);
	}
	else if(ObsMode == INORBIT){
		for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
			free(InputSoOp->S[iSoOp].Sp);
		}
		if(OrbModel == ORB_IMPORT){
			free(InputSoOp->Sc);
		}
		else if(OrbModel == ORB_42 || OrbModel == ORB_42_DECAY){
			if(ObsBase == TOWER){
				free(InputSoOp->Twr);
			}
		}
		free(InputSoOp->S);
	}
	free(InputSoOp);
}

void LoadFixed(void)
{
	FILE *infile;
    char junk[120], newline, response[120];
	char AntFileName[4][30], orbitName[5];
	long ISoOp;
	double hRx, G_Rx, elRx, phRx, thRx, t_coh, Tn, sres_req, hpbw, SLL, XPL, antPatRes;
	long AntTag;

	/* .. Read from file Inp_Fixed.txt */
    infile=FileOpen(SimPath,"Inp_Fixed.txt","r");

	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	// Receiver Altitude [km]
	fscanf(infile, "%lf %[^\n] %[\n]", &hRx, junk, &newline);
	hRx *= 1E3; // [km] to [m]
	// Receive Antenna Gain [dB] and Polarization (R,L,X,Y)
	fscanf(infile, "%lf %s %[^\n] %[\n]", &G_Rx, RxPol, junk, &newline);
	if(!strcmp(RxPol, "RX")){
		NPol = 2;
	}
	else{
		NPol = 1;
	}
	// Receive Antenna Pattern
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	AntTag = decodeStr(response);
	// (GAUSSIAN) HPBW[deg], Sidelobe[dB], X-pol[dB], Res[deg]
	fscanf(infile, "%lf %lf %lf %lf %[^\n] %[\n]", &hpbw, &SLL, &XPL, &antPatRes, junk, &newline);
	// (USER_DEFINED) File Name for Antenna Pattern
	fscanf(infile,"%s %[^\n] %[\n]",AntFileName[0],junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",AntFileName[1],junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",AntFileName[2],junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",AntFileName[3],junk,&newline);
	// (For retrieval only) Orbit type
	fscanf(infile,"%s %[^\n] %[\n]", orbitName,junk,&newline);
	// Receiver Elevation [deg], Azimuth [deg]
	fscanf(infile, "%lf %lf %[^\n] %[\n]", &elRx, &phRx, junk, &newline);
	elRx *= D2R;
	phRx *= D2R;
	thRx = HalfPi - elRx;
	// Coherent Integration Time [sec]
	fscanf(infile, "%lf %[^\n] %[\n]", &t_coh, junk, &newline);
	// Noise Temperature [K]
	fscanf(infile, "%lf %[^\n] %[\n]", &Tn, junk, &newline);
	// Spatial Resolution Requirement [m]
	fscanf(infile, "%lf %[^\n] %[\n]", &sres_req, junk, &newline);
	sres_req *= 1E3; // [km] -> [m]

	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);

	// Number of Transmitters
	fscanf(infile, "%ld %[^\n] %[\n]", &NSoOp, junk, &newline);

	InputSoOp->F = (struct FixedObsType *) calloc(NSoOp, sizeof(struct FixedObsType));
	if (InputSoOp->F == NULL) {
		printf("InputSoOp->F calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
		fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
		// Transmitter Frequency [Hz]
		fscanf(infile, "%lf %[^\n] %[\n]", &InputSoOp->F[ISoOp].Freq, junk, &newline);
		InputSoOp->F[ISoOp].Freq *= 1E6; // [MHz] to [Hz]
		// Transmitter Altitude [km]
		fscanf(infile, "%lf %[^\n] %[\n]", &InputSoOp->F[ISoOp].hTx, junk, &newline);
		InputSoOp->F[ISoOp].hTx *= 1E3; // [km] to [m]
		// Transmitter Range to Earth Center [m]
		InputSoOp->F[ISoOp].rTx = InputSoOp->F[ISoOp].hTx + World[EARTH].rad;
		// Transmitter EIRP [dB], Polarization (R,L,X,Y)
		fscanf(infile, "%lf %c %[^\n] %[\n]", &InputSoOp->F[ISoOp].EIRP_Tx, &InputSoOp->F[ISoOp].polTx, junk, &newline);
		// Transmitter Elevation [deg], Azimuth [deg]
		fscanf(infile, "%lf %lf %[^\n] %[\n]", &InputSoOp->F[ISoOp].elTx, &InputSoOp->F->phTx, junk, &newline);
		InputSoOp->F[ISoOp].elTx *= D2R;
		InputSoOp->F[ISoOp].phTx *= D2R;
		InputSoOp->F[ISoOp].thTx = HalfPi - InputSoOp->F[ISoOp].elTx; 
		// Bandwidth of Channel
		fscanf(infile,"%lf %[^\n] %[\n]",&InputSoOp->F[ISoOp].Bandwidth,junk,&newline); // read in [kHz]
		InputSoOp->F[ISoOp].Bandwidth *= 1E3; // [kHz] to [Hz]
	}

	for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
		// Receiver Altitude [km]
		InputSoOp->F[ISoOp].hRx = hRx;
		// Receive Antenna Gain [dB]
		InputSoOp->F[ISoOp].G_Rx = G_Rx;
		// Receive Antenna Pattern
		InputSoOp->F[ISoOp].AntTag = AntTag;
		// (GAUSSIAN) HPBW[deg], Sidelobe[dB], X-pol[dB], Res[deg]
		InputSoOp->F[ISoOp].hpbw = hpbw;
		InputSoOp->F[ISoOp].SLL = SLL;
		InputSoOp->F[ISoOp].XPL = XPL;
		InputSoOp->F[ISoOp].AntPatRes = antPatRes;
		// (USER_DEFINED) File Name for Antenna Pattern
		strcpy(InputSoOp->F[ISoOp].AntFileName[0], AntFileName[0]);
		strcpy(InputSoOp->F[ISoOp].AntFileName[1], AntFileName[1]);
		strcpy(InputSoOp->F[ISoOp].AntFileName[2], AntFileName[2]);
		strcpy(InputSoOp->F[ISoOp].AntFileName[3], AntFileName[3]);
		// (For retrieval only) Orbit Type
		strcpy(InputSoOp->F[ISoOp].orbitName, orbitName);
		// Receiver Elevation [deg], Azimuth [deg]
		InputSoOp->F[ISoOp].elRx = elRx;
		InputSoOp->F[ISoOp].phRx = phRx;
		InputSoOp->F[ISoOp].thRx = thRx;
		// Coherent Integration Time [sec]
		InputSoOp->F[ISoOp].t_coh = t_coh;
		// Noise Temperature [K]
		InputSoOp->F[ISoOp].Tn = Tn;
		// Spatial Resolution Requirement [m]
		InputSoOp->F[ISoOp].sres_req = sres_req;
	}
}

void LoadInorbit(void)
{
	FILE *infile;
    char junk[120], newline, response[120];
	long ISc, ISoOp, ISp, IRx;
	
	char AntFileName[4][30], Label[20], polTx, polRx;
	double Freq, EIRP_Tx, G_Rx, Bandwidth, t_coh, NoiseTemS, NoiseTemE, minSurfRes, hpbw, SLL, XPL, antPatRes;
	long NSp, AntTag, NthetaLocal, NphiLocal, RefOrbFirst, RefOrbLast;
	double lat, lon, alt;

	/* .. Read from file Inp_Inorbit.txt */
    infile=FileOpen(SimPath,"Inp_Inorbit.txt","r");
	
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	// Space-based or Ground-based Observation?
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ObsBase = decodeStr(response);
	// Number of Receivers
    fscanf(infile,"%ld %[^\n] %[\n]",&NRx,junk,&newline);
	// Number of SoOp Configurations
	fscanf(infile,"%ld %[^\n] %[\n]",&NSoOp,junk,&newline);
	InputSoOp->S = (struct SoOpType *) calloc(NSoOp,sizeof(struct SoOpType));
	if (InputSoOp->S == NULL) {
		printf("InputSoOp->S calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	// Orbit Model
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	OrbModel = decodeStr(response);
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);

	// (IMPORT) File Name to Import Orbit(Pos/Vel)
    if(OrbModel == ORB_IMPORT){
		// Number of Spacecrafts
		fscanf(infile,"%ld %[^\n] %[\n]",&NSc,junk,&newline);
		// Number of Data or Lines
		fscanf(infile,"%ld %[^\n] %[\n]",&NData,junk,&newline);
		NData = NData - 1; // Skip header
		InputSoOp->Sc = (struct ScType *) calloc(NSc,sizeof(struct ScType));
		if (InputSoOp->Sc == NULL) {
			printf("InputSoOp->Sc calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		for(ISc=0; ISc<NSc; ISc++){
			fscanf(infile,"%s %[^\n] %[\n]", InputSoOp->Sc[ISc].OrbFilename, junk, &newline);
		}
		
		InputSoOp->S[0].RefOrbFirst = NRx;
		InputSoOp->S[0].RefOrbLast = NSc + 1;

		// Skip 42 orbit 
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		fscanf(infile,"%s %ld %ld %[^\n] %[\n]", Label, &RxOrbFirst, &RxOrbLast, junk, &newline);
		strcpy(RxName, Label);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
			fscanf(infile,"%s %ld %ld %[^\n] %[\n]", Label, &RefOrbFirst, &RefOrbLast, junk, &newline);
			strcpy(InputSoOp->S[ISoOp].TxName, Label);

		}
    }
    else if(OrbModel == ORB_42 || OrbModel == ORB_42_DECAY){
		// Skip IMPORT
		fscanf(infile,"%ld %[^\n] %[\n]",&NSc,junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		for(ISc=0; ISc<NSc; ISc++){
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		}
		// (42) Rx/Tx Name, First RefOrb, Last RefOrb in Inp_Sim.txt
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		if(ObsBase == SPACE){
			fscanf(infile,"%s %ld %ld %[^\n] %[\n]", Label, &RxOrbFirst, &RxOrbLast, junk, &newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			strcpy(RxName, Label);
		}
		else if(ObsBase == TOWER){
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			InputSoOp->Twr = (struct TwrType *) calloc(NRx,sizeof(struct TwrType));
			if (InputSoOp->Twr == NULL) {
				printf("InputSoOp->Twr calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
			for(IRx=0; IRx<NRx; IRx++){
				fscanf(infile,"%s %lf %lf %lf %[^\n] %[\n]", Label, &lat, &lon, &alt, junk, &newline);
				strcpy(InputSoOp->Twr[IRx].label, Label);
				InputSoOp->Twr[IRx].lat = lat;
				InputSoOp->Twr[IRx].lon = lon;
				InputSoOp->Twr[IRx].alt = alt;
			}
		}

		// Tx Constellations
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
			fscanf(infile,"%s %ld %ld %[^\n] %[\n]", Label, &RefOrbFirst, &RefOrbLast, junk, &newline);
			strcpy(InputSoOp->S[ISoOp].TxName, Label);
			InputSoOp->S[ISoOp].RefOrbFirst = RefOrbFirst;
			InputSoOp->S[ISoOp].RefOrbLast = RefOrbLast + 1;
		}
    }

	for (ISoOp=0;ISoOp<NSoOp;ISoOp++){
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		// Number of Specular Points
		fscanf(infile,"%ld %[^\n] %[\n]",&NSp,junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		// Receive Antenna Gain, Polarization
		fscanf(infile,"%lf %c %[^\n] %[\n]", &G_Rx, &polRx, junk,&newline);
		NPol = 1;
		// Receive Antenna Pattern
		fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
		AntTag = decodeStr(response);
		// (GAUSSIAN) HPBW[deg], Sidelobe[dB], X-pol[dB], Res[deg]
		fscanf(infile, "%lf %lf %lf %lf %[^\n] %[\n]", &hpbw, &SLL, &XPL, &antPatRes, junk, &newline);
		// (USER_DEFINED) File Name for Antenna Pattern
		fscanf(infile,"%s %[^\n] %[\n]",AntFileName[0],junk,&newline);
		fscanf(infile,"%s %[^\n] %[\n]",AntFileName[1],junk,&newline);
		fscanf(infile,"%s %[^\n] %[\n]",AntFileName[2],junk,&newline);
		fscanf(infile,"%s %[^\n] %[\n]",AntFileName[3],junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		// Tx Frequency
		fscanf(infile,"%lf %[^\n] %[\n]",&Freq,junk,&newline); // read in [MHz]
		Freq *= 1E6; // [MHz] to [Hz]
		// Tx EIRP, Polarization
		fscanf(infile,"%lf %c %[^\n] %[\n]",&EIRP_Tx, &polTx, junk,&newline);
		// Bandwidth of Channel
		fscanf(infile,"%lf %[^\n] %[\n]",&Bandwidth,junk,&newline); // read in [kHz]
		Bandwidth *= 1E3; // [kHz] to [Hz]
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		// Coherent Integration Time
		fscanf(infile,"%lf %[^\n] %[\n]",&t_coh,junk,&newline);
		// Noise Temperature of Sky-view, Earth-view Channel
		fscanf(infile,"%lf %lf %[^\n] %[\n]",&NoiseTemS, &NoiseTemE, junk,&newline);
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		// Minimum Local Surface Resolution
		fscanf(infile,"%lf %[^\n] %[\n]",&minSurfRes,junk,&newline);
		// Number of Grid Points in theta, phi direction
		fscanf(infile,"%ld %ld %[^\n] %[\n]",&NthetaLocal, &NphiLocal, junk,&newline);

		InputSoOp->S[ISoOp].NSp = NSp;
		InputSoOp->S[ISoOp].Sp = (struct SpecularType *) calloc(NSp,sizeof(struct SpecularType));
		if (InputSoOp->S[ISoOp].Sp == NULL) {
			printf("InputSoOp->S[%ld].Sp calloc returned null pointer.  Bailing out!\n", ISoOp);
			exit(1);
		}

		for(ISp=0; ISp<NSp; ISp++){
			InputSoOp->S[ISoOp].Sp[ISp].K = 20;
		}
		// Rx Gain
		InputSoOp->S[ISoOp].G_Rx = G_Rx;
		// Rx Polarization
		InputSoOp->S[ISoOp].polRx = polRx;
		// Tx Frequency
		InputSoOp->S[ISoOp].Freq = Freq;
		// Tx EIRP
		InputSoOp->S[ISoOp].EIRP_Tx = EIRP_Tx;
		// Tx Polarization
		InputSoOp->S[ISoOp].polTx = polTx;
		// Bandwidth of Channel
		InputSoOp->S[ISoOp].Bandwidth = Bandwidth;
		// Coherent Integration Time
		InputSoOp->S[ISoOp].t_coh = t_coh;
		// Antenna Type
		InputSoOp->S[ISoOp].AntTag = AntTag;
		// Generalized-Gaussian Antenna Pattern Parameters
		InputSoOp->S[ISoOp].hpbw = hpbw;
		InputSoOp->S[ISoOp].SLL = SLL;
		InputSoOp->S[ISoOp].XPL = XPL;
		InputSoOp->S[ISoOp].AntPatRes = antPatRes;
		// Rx Antenna Pattern File
		strcpy(InputSoOp->S[ISoOp].AntFileName[0], AntFileName[0]);
		strcpy(InputSoOp->S[ISoOp].AntFileName[1], AntFileName[1]);
		strcpy(InputSoOp->S[ISoOp].AntFileName[2], AntFileName[2]);
		strcpy(InputSoOp->S[ISoOp].AntFileName[3], AntFileName[3]);
		// Noise Temperature of Sky-view Channel
		InputSoOp->S[ISoOp].NoiseTemS = NoiseTemS;
		// Noise Temperature of Earth-view Channel
		InputSoOp->S[ISoOp].NoiseTemE = NoiseTemE;
		// Minimum Local Surface Resolution
		InputSoOp->S[ISoOp].minSurfRes = minSurfRes;
		// Number of Grid Points in theta direction
		InputSoOp->S[ISoOp].NthetaLocal = NthetaLocal;
		// Number of Grid Points in phi direction
		InputSoOp->S[ISoOp].NphiLocal = NphiLocal;

		//InitAntGain(&SoOp[ISoOp]);
	}
	// Receivers and Transmitters
	if(OrbModel == ORB_42 || OrbModel == ORB_42_DECAY){
		NSc = RefOrbLast + 1; // Total Number of Receivers and Transmitters
	}
}

void LoadVegetation(void)
{
	FILE *infile;
    char junk[120], newline, response[120], kind[2];
	long iVeg, iLayer, iKind, iSoOp;
	double freq;// temp1, temp2;

	/* .. Read from file Inp_Fixed.txt */
    infile=FileOpen(SimPath,"Inp_Veg.txt","r");

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

	// Number of Vegetation Types
	fscanf(infile, "%ld %[^\n] %[\n]", &NVeg, junk, &newline);

	VegData = (struct VegDataType *) calloc(NVeg, sizeof(struct VegDataType));
	if (VegData == NULL) {
		printf("VegData calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	for(iVeg=0; iVeg<NVeg; iVeg++){
		fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
		// Vegetation Type (Label)
		fscanf(infile,"%s %[^\n] %[\n]",VegData[iVeg].label,junk,&newline);
		// Number of Layers
		fscanf(infile, "%ld %[^\n] %[\n]", &VegData[iVeg].nLayer, junk, &newline);
		VegData[iVeg].thickness = (double *) calloc(VegData[iVeg].nLayer, sizeof(double));
		if (VegData[iVeg].thickness == NULL) {
			printf("VegData[iVeg].thickness calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		VegData[iVeg].nKind = (long *) calloc(VegData[iVeg].nLayer, sizeof(long));
		if (VegData[iVeg].nKind == NULL) {
			printf("VegData[iVeg].nKind calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		VegData[iVeg].kind = (char ***) calloc(VegData[iVeg].nLayer, sizeof(char**));

		for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
			fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
			// Thickness [m]
			fscanf(infile, "%lf %[^\n] %[\n]", &VegData[iVeg].thickness[iLayer], junk, &newline);
			// Number of Included Kinds
			fscanf(infile, "%ld ", &VegData[iVeg].nKind[iLayer]);
			
			VegData[iVeg].kind[iLayer] = (char **) calloc(VegData[iVeg].nKind[iLayer], sizeof(char*));
			if (VegData[iVeg].kind[iLayer] == NULL) {
				printf("VegData[iVeg].kind calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
			// Particle ID
			for(iKind=0; iKind<VegData[iVeg].nKind[iLayer]; iKind++){
				VegData[iVeg].kind[iLayer][iKind] = (char *) calloc(2, sizeof(char));
			
				fscanf(infile,"%s ", kind);
				strcpy(VegData[iVeg].kind[iLayer][iKind], kind);
			}
			fscanf(infile,"%[^\n] %[\n]", junk, &newline);
		}
	}

	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	
	// Number of Kinds
	fscanf(infile, "%ld %[^\n] %[\n]", &NKind, junk, &newline);

	VegKind = (struct VegKindType *) calloc(NKind, sizeof(struct VegKindType));
	if (VegKind == NULL) {
		printf("VegKind calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	for(iKind=0; iKind<NKind; iKind++){
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		// Particle ID
		fscanf(infile,"%s %[^\n] %[\n]", VegKind[iKind].ID, junk,&newline);
		// Density [particles/m^3]
		fscanf(infile, "%lf %[^\n] %[\n]", &VegKind[iKind].density, junk, &newline);
		// Dimensions [m, m, m]
		fscanf(infile, "%lf %lf %lf %[^\n] %[\n]", &VegKind[iKind].dim[0], &VegKind[iKind].dim[1], &VegKind[iKind].dim[2], junk, &newline);
		// Dielectric Constant (real, imag)
		//fscanf(infile, "%lf %lf %[^\n] %[\n]", &temp1, &temp2, junk, &newline);
		//VegKind[iKind].e_c = temp1 + I1*temp2;
		// Gravimetric Water Content
		fscanf(infile, "%lf %[^\n] %[\n]", &VegKind[iKind].mv, junk, &newline);
		// Interval Angle of Orientation [deg] (min, max)
		fscanf(infile, "%lf %lf %[^\n] %[\n]", &VegKind[iKind].beginAng, &VegKind[iKind].endAng, junk, &newline);
		// degee to radian
		VegKind[iKind].beginAng *= D2R;
		VegKind[iKind].endAng *= D2R;

		// Vegetation Water Content
		VegKind[iKind].VWC = VegKind[iKind].density * VegKind[iKind].mv * 1000 * Pi * VegKind[iKind].dim[0] * VegKind[iKind].dim[1] * VegKind[iKind].dim[2];

		// Dielectric Constant
		VegKind[iKind].e_c = (double complex *) calloc(NSoOp, sizeof(double complex));

		for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
			if(ObsMode == FIXED){
				freq = Fixed[iSoOp].Freq;
			}
			else if(ObsMode == INORBIT){
				freq = SoOp[iSoOp].Freq;
			}
			
			switch(VegKind[iKind].ID[0]){
				case 'L':
					VegKind[iKind].e_c[iSoOp] = calcDielUlabyElRayesMv(freq, VegKind[iKind].mv);
					break;
				case 'B':
					VegKind[iKind].e_c[iSoOp] = calcDielUlabyElRayesMv(freq, VegKind[iKind].mv);
					break;
				case 'T':
					VegKind[iKind].e_c[iSoOp] = calcDielUlabyElRayesMv(freq, VegKind[iKind].mv);
					break;
				case 'N':
					VegKind[iKind].e_c[iSoOp] = calcDielUlabyElRayesMv(freq, VegKind[iKind].mv);
					break;
				case 'W':
					VegKind[iKind].e_c[iSoOp] = calcDielDebye(freq);
					break;
				default:
					printf(">> Wrong kind of vegetation...\n"); exit(1);	
			}
			//printf("e_c %c = %lf + j %lf\n", VegKind[iKind].ID[0], creal(VegKind[iKind].e_c), cimag(VegKind[iKind].e_c));
		}
	}
	//exit(1);
}

void InitDirectories(int argc, char **argv)
{
	int check;

	sprintf(SimPath, "./sim/Demo-space/");
	strcpy(OutPath, SimPath);
	strcat(OutPath, "out/");
	sprintf(SttDataPath, "./earthdata/static/");
	sprintf(DynDataPath, "./earthdata/dynamic/");
	sprintf(AntPath, "./ant_pat/");

	sprintf(OrbPathImport, "./orbit/import/");
	sprintf(InOutPath42, "./orbit/42input/Demo-space/");
	sprintf(ModelPath42, "./orbit/42/Model/");
	sprintf(SCPath42, "./orbit/42input/SC/");
	sprintf(OrbPath42, "./orbit/42input/Orb/");

	if (argc > 1){
		sprintf(SimPath, "./sim/%s/", argv[1]);
		strcpy(OutPath, SimPath);
		strcat(OutPath, "out/");
		strcpy(OutSubPath, OutPath);
		sprintf(InOutPath42, "./orbit/42input/%s/", argv[1]);
		
        // Make an output directory if not exist
        check = mkdir(OutPath, 0777);
        if(check != 0 && errno != EEXIST){
            printf("mkdir fail: %s\n", OutPath);
            exit(1);            
        }
	}
}

void InitConstants(void)
{
	// Physical constants
	World[EARTH].rad = 6.378145E6; // Earth radius
	World[EARTH].w = 7.292115E-5; // Earth spin rate
	World[EARTH].mu = 3.986004418E14; // Earth gravitational parameter

	// Math
	Pi = 4.0*atan(1.0);
	TwoPi = 2.0*Pi;
	HalfPi = 0.5*Pi;
	R2D = 180.0/Pi;
	D2R = Pi/180.0;
}

void InitVegetation(void)
{
	long iVeg, iKind, jKind, iLayer, iType;
	long sTypknd[NUM_VEGTYPE], num_types, num_kinds;
	long part_type, part_type_idx[NUM_VEGTYPE] = {0}, part_kind;

	FILE *infile;
    char newline, filename[60];
	long nAng, nLayer, iSoOp, iAng, i, flagCalcProp = FALSE;
	double freq, dKz0r, dKz0i, dKz1r, dKz1i;
	double ATTENH, ATTENV;
	
	for(iVeg=0; iVeg<NVeg; iVeg++){
		// Reset parameters
		for(iType=0; iType<NUM_VEGTYPE; iType++)
			sTypknd[iType] = 0;
		num_types = 0;
		num_kinds = 0;
		// Initialization
		VegData[iVeg].typknd = (int **) calloc(VegData[iVeg].nLayer, sizeof(int *));
		for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
			VegData[iVeg].typknd[iLayer] = (int *) calloc(NUM_VEGTYPE, sizeof(int));
			for(iKind=0; iKind<VegData[iVeg].nKind[iLayer]; iKind++){
				switch(VegData[iVeg].kind[iLayer][iKind][0]){
					case 'L':
						VegData[iVeg].typknd[iLayer][0]++;
						break;
					case 'B':
						VegData[iVeg].typknd[iLayer][1]++;
						break;
					case 'T':
						VegData[iVeg].typknd[iLayer][2]++;
						break;
					case 'N':
						VegData[iVeg].typknd[iLayer][3]++;
						break;
					case 'W':
						VegData[iVeg].typknd[iLayer][4]++;
						break;
					default:
						printf(">> Wrong kind of vegetation...\n"); exit(1);	
				}
			}
			for(iType=0; iType<NUM_VEGTYPE; iType++){
				sTypknd[iType] += VegData[iVeg].typknd[iLayer][iType];
				if(num_kinds < VegData[iVeg].typknd[iLayer][iType])
					num_kinds = VegData[iVeg].typknd[iLayer][iType];
			}
			VegData[iVeg].depth += VegData[iVeg].thickness[iLayer];
		}

		VegData[iVeg].part_type_idx = (long *) calloc(NUM_VEGTYPE, sizeof(long));
		for(iType=0; iType<NUM_VEGTYPE; iType++){
			if(sTypknd[iType] > 0){
				part_type_idx[iType] = num_types;
				VegData[iVeg].part_type_idx[iType] = num_types;
				num_types++;
			}
		}
		VegData[iVeg].nKindMax = num_kinds;
		VegData[iVeg].nTypeMax = num_types;
		VegData[iVeg].typknd_new = (int **) calloc(VegData[iVeg].nLayer, sizeof(int *));
		for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++)
			VegData[iVeg].typknd_new[iLayer] = (int *) calloc(VegData[iVeg].nTypeMax, sizeof(int));
		VegData[iVeg].dsty = (double ***) calloc(num_kinds, sizeof(double **));
		VegData[iVeg].dim1 = (double ***) calloc(num_kinds, sizeof(double **));
		VegData[iVeg].dim2 = (double ***) calloc(num_kinds, sizeof(double **));
		VegData[iVeg].dim3 = (double ***) calloc(num_kinds, sizeof(double **));
		VegData[iVeg].e_c = (double complex ****) calloc(num_kinds, sizeof(double complex ***));
		VegData[iVeg].beginAng = (double ***) calloc(num_kinds, sizeof(double **));
		VegData[iVeg].endAng = (double ***) calloc(num_kinds, sizeof(double **));
		VegData[iVeg].shape = (long ***) calloc(num_kinds, sizeof(long **));
		VegData[iVeg].VWC = (double ***) calloc(num_kinds, sizeof(double **));
		for(iKind=0; iKind<num_kinds; iKind++){
			VegData[iVeg].dsty[iKind] = (double **) calloc(num_types, sizeof(double *));
			VegData[iVeg].dim1[iKind] = (double **) calloc(num_types, sizeof(double *));
			VegData[iVeg].dim2[iKind] = (double **) calloc(num_types, sizeof(double *));
			VegData[iVeg].dim3[iKind] = (double **) calloc(num_types, sizeof(double *));
			VegData[iVeg].e_c[iKind] = (double complex ***) calloc(num_types, sizeof(double complex **));
			VegData[iVeg].beginAng[iKind] = (double **) calloc(num_types, sizeof(double *));
			VegData[iVeg].endAng[iKind] = (double **) calloc(num_types, sizeof(double *));
			VegData[iVeg].shape[iKind] = (long **) calloc(num_types, sizeof(long *));
			VegData[iVeg].VWC[iKind] = (double **) calloc(num_types, sizeof(double *));
			for(iType=0; iType<num_types; iType++){
				VegData[iVeg].dsty[iKind][iType] = (double *) calloc(VegData[iVeg].nLayer, sizeof(double));
				VegData[iVeg].dim1[iKind][iType] = (double *) calloc(VegData[iVeg].nLayer, sizeof(double));
				VegData[iVeg].dim2[iKind][iType] = (double *) calloc(VegData[iVeg].nLayer, sizeof(double));
				VegData[iVeg].dim3[iKind][iType] = (double *) calloc(VegData[iVeg].nLayer, sizeof(double));
				VegData[iVeg].e_c[iKind][iType] = (double complex **) calloc(VegData[iVeg].nLayer, sizeof(double complex *));
				VegData[iVeg].beginAng[iKind][iType] = (double *) calloc(VegData[iVeg].nLayer, sizeof(double));
				VegData[iVeg].endAng[iKind][iType] = (double *) calloc(VegData[iVeg].nLayer, sizeof(double));
				VegData[iVeg].shape[iKind][iType] = (long *) calloc(VegData[iVeg].nLayer, sizeof(long));
				VegData[iVeg].VWC[iKind][iType] = (double *) calloc(VegData[iVeg].nLayer, sizeof(double));
				for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
					VegData[iVeg].e_c[iKind][iType][iLayer] = (double complex *) calloc(NSoOp, sizeof(double complex));
				}
			}
		}

		// Find layer indices for each kind
		VegData[iVeg].typkndThick = (double *) calloc(NKind, sizeof(double));
		for(iKind=0; iKind<NKind; iKind++){
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
								part_type = part_type_idx[0];
								VegData[iVeg].shape[part_kind][part_type][iLayer] = DISK;
								VegData[iVeg].typknd_new[iLayer][part_type] = VegData[iVeg].typknd[iLayer][0];
								break;
							case 'B':
								part_type = part_type_idx[1];
								VegData[iVeg].shape[part_kind][part_type][iLayer] = CYLINDER;
								VegData[iVeg].typknd_new[iLayer][part_type] = VegData[iVeg].typknd[iLayer][1];
								break;
							case 'T':
								part_type = part_type_idx[2];
								VegData[iVeg].shape[part_kind][part_type][iLayer] = CYLINDER;
								VegData[iVeg].typknd_new[iLayer][part_type] = VegData[iVeg].typknd[iLayer][2];
								break;
							case 'N':
								part_type = part_type_idx[3];
								VegData[iVeg].shape[part_kind][part_type][iLayer] = CYLINDER;
								VegData[iVeg].typknd_new[iLayer][part_type] = VegData[iVeg].typknd[iLayer][3];
								break;
							case 'W':
								part_type = part_type_idx[4];
								VegData[iVeg].shape[part_kind][part_type][iLayer] = DISK;
								VegData[iVeg].typknd_new[iLayer][part_type] = VegData[iVeg].typknd[iLayer][4];
								break;
						}						
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

		VegData[iVeg].dKz = (double complex ****)calloc(NSoOp, sizeof(double complex ***));
		for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
			VegData[iVeg].dKz[iSoOp] = (double complex ***)calloc(2, sizeof(double complex **));
			for(i=0; i<2; i++){
				VegData[iVeg].dKz[iSoOp][i] = (double complex **)calloc(90, sizeof(double complex *));
				for(iAng=0; iAng<90; iAng++)
					VegData[iVeg].dKz[iSoOp][i][iAng] = (double complex *)calloc(VegData[iVeg].nLayer, sizeof(double complex));
			}
		}
#ifdef _USE_NETCDF_
		VegData[iVeg].dKzNc = (double complex **)calloc(2, sizeof(double complex *));
		for(i=0; i<2; i++){
			VegData[iVeg].dKzNc[i] = (double complex *)calloc(VegData[iVeg].nLayer, sizeof(double complex));
		}
		
		VegModel = (struct VegModelType *) calloc(1, sizeof(struct VegModelType));
		if (VegModel == NULL) {
			printf("VegModel calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
#endif
		for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
			if(ObsMode == FIXED){
				freq = Fixed[iSoOp].Freq;
			}
			else if(ObsMode == INORBIT){
				freq = SoOp[iSoOp].Freq;
			}

			sprintf(filename, "veg_%s_dKz_%ldMHz.out", VegData[iVeg].label, (long)(freq/1e6));
			infile = FileOpen(SttDataPath, filename, "r");
			if(infile != NULL){
				fscanf(infile, "%ld %ld %[\n]", &nAng, &nLayer, &newline);
				if((nAng == 90) & (nLayer == VegData[iVeg].nLayer)){
					for(iAng=0; iAng<nAng; iAng++){
						for(iLayer=0; iLayer<nLayer; iLayer++){
							fscanf(infile, "%le %le %le %le %le %le %[\n]", &dKz0r, &dKz0i, &dKz1r, &dKz1i, &ATTENH, &ATTENV, &newline);
							VegData[iVeg].dKz[iSoOp][0][iAng][iLayer] = dKz0r + I1 * dKz0i;
							VegData[iVeg].dKz[iSoOp][1][iAng][iLayer] = dKz1r + I1 * dKz1i;
						}
					}
				}
				else{
					flagCalcProp = TRUE;
				}
				fclose(infile);
			}
			else{
				flagCalcProp = TRUE;
			}
			if(flagCalcProp){
				flagCalcProp = FALSE;
				calcPropagation(freq, iSoOp, iVeg);
			}
		}

		for(iLayer=0; iLayer<VegData[iVeg].nLayer; iLayer++){
			for(iType=0; iType<num_types; iType++){
				for(iKind=0; iKind<num_kinds; iKind++){
					//printf("Veg %ld - Layer %ld - Type %ld - Kind %ld\n", iVeg, iLayer, iType, iKind);
					//printf("%lf %lf\n", VegData[iVeg].dsty[iKind][iType][iLayer], VegData[iVeg].VWC[iKind][iType][iLayer]);
					VegData[iVeg].VWC_total += VegData[iVeg].VWC[iKind][iType][iLayer];
				}
			}
		}
		//printf("VWC[%ld] = %lf\n", iVeg, VegData[iVeg].VWC_total);
	}
}

void InitFOVs(void)
{
      FILE *infile;
      char junk[120],newline;
      char response[120],response1[120],response2[120];
      double Ang1,Ang2,Ang3;
      long Seq;
      long i;

      infile = FileOpen(InOutPath,"Inp_FOV.txt","r");
      fscanf(infile,"%[^\n] %[\n]",junk,&newline);
      fscanf(infile,"%[^\n] %[\n]",junk,&newline);
      fscanf(infile,"%ld %[^\n] %[\n]",&Nfov,junk,&newline);
      FOV = (struct FovType *) calloc(Nfov,sizeof(struct FovType));
      for(i=0;i<Nfov;i++) {
         fscanf(infile,"%[^\n] %[\n]",junk,&newline);
         fscanf(infile,"\"%[^\"]\" %[^\n] %[\n]",
            FOV[i].Label,junk,&newline);
         fscanf(infile,"%ld %lf %[^\n] %[\n]",
            &FOV[i].Nv,&FOV[i].Length,junk,&newline);
         fscanf(infile,"%lf %lf %[^\n] %[\n]",
            &FOV[i].Width,&FOV[i].Height,junk,&newline);
         if (FOV[i].Width >= 180.0) {
            printf("FOV[%ld] Width >= 180 deg.  This is not allowed.  Bailing out.\n",i);
            exit(1);
         }
         if (FOV[i].Height >= 180.0) {
            printf("FOV[%ld] Width >= 180 deg.  This is not allowed.  Bailing out.\n",i);
            exit(1);
         }
         FOV[i].Width *= D2R;
         FOV[i].Height *= D2R;
         fscanf(infile,"%f %f %f %f %[^\n] %[\n]",
            &FOV[i].Color[0],&FOV[i].Color[1],&FOV[i].Color[2],&FOV[i].Color[3],
            junk,&newline);
         fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
         FOV[i].Type = decodeStr(response);
         fscanf(infile,"%s %s %[^\n] %[\n]",
            response1,response2,junk,&newline);
         FOV[i].NearExists = decodeStr(response1);
         FOV[i].FarExists = decodeStr(response2);
         fscanf(infile,"%ld %ld %[^\n] %[\n]",
            &FOV[i].SC,&FOV[i].Body,junk,&newline);
         if (FOV[i].SC >= Nsc) {
            printf("FOV[%ld].SC is out of range.\n",i);
            exit(1);
         }
         if (SC[FOV[i].SC].Exists && FOV[i].Body >= SC[FOV[i].SC].Nb) {
            printf("FOV[%ld].Body is out of range.\n",i);
            exit(1);
         }
         FOV[i].RefOrb = SC[FOV[i].SC].RefOrb;
         if (!SC[FOV[i].SC].Exists) {
            FOV[i].NearExists = FALSE;
            FOV[i].FarExists = FALSE;
         }

         fscanf(infile,"%lf %lf %lf %[^\n] %[\n]",
            &FOV[i].pb[0],&FOV[i].pb[1],&FOV[i].pb[2],junk,&newline);
         fscanf(infile,"%lf %lf %lf %ld %[^\n] %[\n]",
            &Ang1,&Ang2,&Ang3,&Seq,junk,&newline);
            A2C(Seq,Ang1*D2R,Ang2*D2R,Ang3*D2R,FOV[i].CB);
      }
      fclose(infile);
}

void InitCalvalSite(void) // for debug
{
	struct TargetAreaType *TARGET;
	struct ScType *Tx;
	uint8_t ITarget, ITx;
	long iSbSite;
	long *SbNumSite = NULL;
	static long *cntSbSite;
	
	cntSbSite = (long *) calloc(1, sizeof(long));
	for(ITarget=0; ITarget<NTarget; ITarget++){
		TARGET = &TargetArea[ITarget];
		for(ITx=SoOp[0].RefOrbFirst; ITx<SoOp[0].RefOrbLast; ITx++){
			Tx = &Sc[ITx];
			//* .. Update Spotbeam Footprint .. *//
			TrackingSpotbeam(Tx, &SpotBeam[Tx->ID - SoOp[0].RefOrbFirst]);
			SbNumSite = FindSpotbeam(&SpotBeam[Tx->ID - SoOp[0].RefOrbFirst], TARGET->lat, TARGET->lon, SbNumSite, cntSbSite);
			for(iSbSite=0; iSbSite<*cntSbSite; iSbSite++){
				printf("Tx %d, Site %d : Spotbeam %ld / %ld\n", ITx, ITarget, SbNumSite[iSbSite]+1, *cntSbSite);
			}
		}
	}
}

void InitSpotbeams(void)
{
	double r_beams, r0, rS1, rS2, rS3; // Beam spacing
	int i, iTx;
	double theta, temp;
	double a = 6378137.0;
    double f = 1.0/298.257223563;
    double b = a*(1.0-f);
	r_beams = 1+4*cos(36*D2R);
//	r_beams = 3+2*sin(18*D2R);
//	r_beams = sqrt(1+8*cos(36*D2R)*cos(36*D2R)+8*sin(18*D2R)*sin(18*D2R)+8*sin(18*D2R));
	r0 = b/r_beams;
	rS1 = 2*r0*cos(36*D2R);
	rS2 = (2+2*sin(18*D2R))*r0;
	rS3 = 2*rS1;

	for(iTx=0; iTx<5; iTx++){
		SpotBeam[iTx].el[0] = 0;
		SpotBeam[iTx].el[1] = atan2(rS1, SpotBeam[iTx].altTx);
		SpotBeam[iTx].el[2] = atan2(rS2, SpotBeam[iTx].altTx);
		SpotBeam[iTx].el[3] = atan2(rS3, SpotBeam[iTx].altTx);

		for(i=0; i<5; i++){
			SpotBeam[iTx].az[0][i] = (-18+72*i)*D2R;
			SpotBeam[iTx].az[1][i] = (18+72*i)*D2R;
			SpotBeam[iTx].az[2][i] = (-18+72*i)*D2R;
		}

		SpotBeam[iTx].r0sq = r0*r0*SpotBeam[iTx].scaleFactor*SpotBeam[iTx].scaleFactor;
		SpotBeam[iTx].x[0] = 0; SpotBeam[iTx].y[0] = 0;
		SpotBeam[iTx].x[1] = rS1; SpotBeam[iTx].y[1] = 0;
		SpotBeam[iTx].x[2] = rS1*cos(72*D2R); SpotBeam[iTx].y[2] = rS1*sin(72*D2R);
		SpotBeam[iTx].x[3] = rS1*cos(144*D2R); SpotBeam[iTx].y[3] = rS1*sin(144*D2R);
		SpotBeam[iTx].x[4] = rS1*cos(216*D2R); SpotBeam[iTx].y[4] = rS1*sin(216*D2R);
		SpotBeam[iTx].x[5] = rS1*cos(288*D2R); SpotBeam[iTx].y[5] = rS1*sin(288*D2R);
		SpotBeam[iTx].x[6] = rS3; SpotBeam[iTx].y[6] = 0;
		SpotBeam[iTx].x[7] = rS2*cos(36*D2R); SpotBeam[iTx].y[7] = rS2*sin(36*D2R);
		SpotBeam[iTx].x[8] = rS3*cos(72*D2R); SpotBeam[iTx].y[8] = rS3*sin(72*D2R);
		SpotBeam[iTx].x[9] = rS2*cos(108*D2R); SpotBeam[iTx].y[9] = rS2*sin(108*D2R);
		SpotBeam[iTx].x[10] = rS3*cos(144*D2R); SpotBeam[iTx].y[10] = rS3*sin(144*D2R);
		SpotBeam[iTx].x[11] = rS2*cos(180*D2R); SpotBeam[iTx].y[11] = rS2*sin(180*D2R);
		SpotBeam[iTx].x[12] = rS3*cos(216*D2R); SpotBeam[iTx].y[12] = rS3*sin(216*D2R);
		SpotBeam[iTx].x[13] = rS2*cos(252*D2R); SpotBeam[iTx].y[13] = rS2*sin(252*D2R);
		SpotBeam[iTx].x[14] = rS3*cos(288*D2R); SpotBeam[iTx].y[14] = rS3*sin(288*D2R);
		SpotBeam[iTx].x[15] = rS2*cos(324*D2R); SpotBeam[iTx].y[15] = rS2*sin(324*D2R);

		theta = 18*D2R;

		for(i=0; i<16; i++){
			temp = cos(theta)*SpotBeam[iTx].x[i] + sin(theta)*SpotBeam[iTx].y[i];
			SpotBeam[iTx].y[i] = cos(theta)*SpotBeam[iTx].y[i] - sin(theta)*SpotBeam[iTx].x[i];
			SpotBeam[iTx].x[i] = temp;
			SpotBeam[iTx].flagShifted[i] = FALSE;
		}

		SpotBeam[iTx].r0Target = SpotBeam[iTx].altTx * tan(SpotBeam[iTx].halfBW);
		SpotBeam[iTx].halfBW_whole = asin(b/(SpotBeam[iTx].altTx + b));
	}
}


void InitReport(void)
{
    FILE *infile;
    char junk[200],newline, response[120];

/* .. Read from file Inp_Report.txt */
    infile=FileOpen(SimPath,"Inp_Report.txt","r");

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

    ReportSoOp = (struct ReportType *) calloc(1,sizeof(struct ReportType));

	// .. Outputs for SoOp Observation .. //
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->JulDay_Sp = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->Date = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->LLA_Rx = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->PosW_Rx = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->VelW_Rx = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->TxID = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->LLA_Tx = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->PosW_Tx = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->VelW_Tx = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->LLA_Sp = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->PosW_Sp = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->LoS_D = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->LoS_R = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->DelayDoppler = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->Gain_D = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->Gain_R = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->FresZone = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->IncAng = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->elev = decodeStr(response);

	// SPACE only 
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->RPY_to_Sp = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->LandMask = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->VSM = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->SoilTemp = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->FreezeThaw = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->SoilTexture = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->SnowCover = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->RcCO = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->RcX = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->GammaCO = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->GammaX = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->SNR_D = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->SNR_R = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->SbNum = decodeStr(response);	
	// .. Outputs for Subsystems .. //
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	// Comm
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->AccessTime = decodeStr(response);
	// ADCS
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	fscanf(infile,"%hhd %hhd %[^\n] %[\n]", &ReportSoOp->FirstRxID, &ReportSoOp->LastRxID, junk, &newline);
	ReportSoOp->nADCS = ReportSoOp->LastRxID - ReportSoOp->FirstRxID + 1;
	// Truth data
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->JulDay = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->PosN = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->RPY = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->AngVel = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->AngMom = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->MagField = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->Eclipse = decodeStr(response);
	ReportSoOp->TruthAtt = ReportSoOp->JulDay || ReportSoOp->RPY || ReportSoOp->AngVel || ReportSoOp->AngMom || ReportSoOp->MagField || ReportSoOp->Eclipse;
	// Sensors
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->Gyr = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->Mtm = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->Acc = decodeStr(response);
	// Actuators
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->Whl = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ReportSoOp->Mtb = decodeStr(response);
	ReportSoOp->ADCS = ReportSoOp->TruthAtt || ReportSoOp->Gyr || ReportSoOp->Mtm || ReportSoOp->Acc || ReportSoOp->Whl || ReportSoOp->Mtb;

	fclose(infile);
}

void InitSubsys(void)
{
    FILE *infile;
    char junk[120], newline; // response[120];
	double beamwidth_Rx;
	long iGs;
	double rE, sinLat2;
    double a = 6378137.0;
//    double f = 1.0/298.257223563; // WGS84
	double f = 1.0/298.257222100882711; // GRS80
    double e2 = f*(2.0-f);
	int row, col;

	// .. Read from file Inp_Subsys.txt .. //
    infile=FileOpen(SimPath,"Inp_Subsys.txt","r");

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

	// Antenna beamwidth of spacecraft
	fscanf(infile,"%lf %[^\n] %[\n]",&beamwidth_Rx,junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	// Number of ground stations
	fscanf(infile,"%ld %[^\n] %[\n]",&NGs,junk,&newline);
	Gs = (struct GsType *) calloc(NGs, sizeof(struct GsType));
	if (Gs == NULL) {
		printf("Gs calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	for(iGs=0; iGs<NGs; iGs++){
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		fscanf(infile,"%s %lf %lf %[^\n] %[\n]", Gs[iGs].name, &Gs[iGs].lat, &Gs[iGs].lon, junk,&newline);
		fscanf(infile,"%lf %[^\n] %[\n]",&Gs[iGs].beamwidth_Gs,junk,&newline);
		Gs[iGs].beamwidth_Gs *= D2R/2;
		Gs[iGs].beamwidth_Rx = beamwidth_Rx*D2R/2;
		row = Gs[iGs].lat*60+5399.9999;
		col = Gs[iGs].lon*60+10799.9999;
		Gs[iGs].lat *= D2R;
		Gs[iGs].lon *= D2R;
		sinLat2 = sin(Gs[iGs].lat)*sin(Gs[iGs].lat);
		rE = a*sqrt((1-e2*(2-e2)*sinLat2) / (1-e2*sinLat2));
		Gs[iGs].alt = SttData->height[row][col] + 6371000 - rE;

		// GRS80 to ECEF
    	GRS80ToECEF(Gs[iGs].lat, Gs[iGs].lon, Gs[iGs].alt, Gs[iGs].PosW);

		// Memory allocation
		Gs[iGs].cntAccess = (long *) calloc(NRx, sizeof(long));
		Gs[iGs].flagAccessStart = (long *) calloc(NRx, sizeof(long));
		Gs[iGs].flagAccessEnd = (long *) calloc(NRx, sizeof(long));
		Gs[iGs].t_acc_start = (double *) calloc(NRx, sizeof(double));
		Gs[iGs].t_acc_end = (double *) calloc(NRx, sizeof(double));
	}
	fclose(infile);
}

void InitFixedObs(void)
{
	long ISoOp;
	
	Fixed = (struct FixedObsType *) calloc(NSoOp, sizeof(struct FixedObsType));
	if (Fixed == NULL) {
		printf("Fixed Observation calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
		Fixed[ISoOp].ID = ISoOp;
		Fixed[ISoOp].Freq = InputSoOp->F[ISoOp].Freq; // [Hz]
		Fixed[ISoOp].Wavelength = LIGHTSPEED / Fixed[ISoOp].Freq;
		Fixed[ISoOp].Bandwidth = InputSoOp->F[ISoOp].Bandwidth;
		Fixed[ISoOp].EIRP_Tx = InputSoOp->F[ISoOp].EIRP_Tx; // [dB]
		Fixed[ISoOp].polRx = InputSoOp->F[ISoOp].polRx; // Receive Antenna Polarization
		Fixed[ISoOp].polTx = InputSoOp->F[ISoOp].polTx; // Transmit Antenna Polarization
		Fixed[ISoOp].hpbw = InputSoOp->F[ISoOp].hpbw; // Half-power beanwidth of the receive antenna pattern [deg]
		Fixed[ISoOp].SLL = InputSoOp->F[ISoOp].SLL; // First Sidelobe level of the receive antenna pattern [dB]
		Fixed[ISoOp].XPL = InputSoOp->F[ISoOp].XPL; // Cross-polarization level of the receive antenna pattern [dB]
		Fixed[ISoOp].AntPatRes = InputSoOp->F[ISoOp].AntPatRes; // Receive antenna pattern resolution [deg]
		Fixed[ISoOp].G_Rx = InputSoOp->F[ISoOp].G_Rx; // [dB]
		Fixed[ISoOp].rTx = InputSoOp->F[ISoOp].rTx; // Transmitter range from Earth's center [m]
		Fixed[ISoOp].hTx = InputSoOp->F[ISoOp].hTx; // Transmitter altitude [m]
		Fixed[ISoOp].elTx = InputSoOp->F[ISoOp].elTx; // Tx Elevation = 90 - Incidence angle [rad]
		Fixed[ISoOp].thTx = InputSoOp->F[ISoOp].thTx; // Tx Incidence angle [rad]
		Fixed[ISoOp].phTx = InputSoOp->F[ISoOp].phTx; // Tx Azimuth angle [rad]
		Fixed[ISoOp].hRx = InputSoOp->F[ISoOp].hRx; // Receiver altitude [m]
		Fixed[ISoOp].elRx = InputSoOp->F[ISoOp].elRx; // Rx Elevation = 90 - Incidence angle [rad]
		Fixed[ISoOp].thRx = InputSoOp->F[ISoOp].thRx; // Rx Antenna looking angle (angle of incidence) [rad]
		Fixed[ISoOp].phRx = InputSoOp->F[ISoOp].phRx; // Rx Azimuth angle of receiver position [rad]
		Fixed[ISoOp].t_coh = InputSoOp->F[ISoOp].t_coh; // Coherent Integration Time [sec]
		Fixed[ISoOp].Tn = InputSoOp->F[ISoOp].Tn; // Noise Temperature [K]
		Fixed[ISoOp].sres_req = InputSoOp->F[ISoOp].sres_req; // Spatial Resolution Requirement [m]
		Fixed[ISoOp].t_inc = Fixed[ISoOp].sres_req / (sqrt(World[EARTH].mu/(Fixed[ISoOp].hRx + World[EARTH].rad)) - World[EARTH].rad*World[EARTH].w);
		Fixed[ISoOp].N_inc = floor(Fixed[ISoOp].t_inc / Fixed[ISoOp].t_coh);

		Fixed[ISoOp].AntTag = InputSoOp->F[ISoOp].AntTag;
		if(Fixed[ISoOp].AntTag == USER_DEFINED){
			strcpy(Fixed[ISoOp].AntFileName[0], InputSoOp->F[ISoOp].AntFileName[0]);
			strcpy(Fixed[ISoOp].AntFileName[1], InputSoOp->F[ISoOp].AntFileName[1]);
			strcpy(Fixed[ISoOp].AntFileName[2], InputSoOp->F[ISoOp].AntFileName[2]);
			strcpy(Fixed[ISoOp].AntFileName[3], InputSoOp->F[ISoOp].AntFileName[3]);
			Fixed[ISoOp].AntPattern = LoadAntPattern(Fixed[ISoOp].AntFileName, &Fixed[ISoOp].AntPatRes, Fixed[ISoOp].AntPattern);
		}
		else if(Fixed[ISoOp].AntTag == GAUSSIAN){
			Fixed[ISoOp].AntPattern = LoadAntPatternGG(Fixed[ISoOp].SLL, Fixed[ISoOp].XPL, Fixed[ISoOp].hpbw, Fixed[ISoOp].AntPatRes, Fixed[ISoOp].AntPattern);
		}
		//ReportAntPattern(Fixed[ISoOp].AntFileName, Fixed[ISoOp].AntPatRes, Fixed[ISoOp].AntPattern);
		Fixed[ISoOp].h = ( 2*InputSoOp->RMSH*TwoPi / (Fixed[ISoOp].Wavelength *100) ) * ( 2*InputSoOp->RMSH*TwoPi / (Fixed[ISoOp].Wavelength *100) );
		strcpy(Fixed[ISoOp].orbitName, InputSoOp->F[ISoOp].orbitName);
	}


		
	// Random Process
	RNG = CreateRandomProcess((long)time(NULL));
}

void InitInorbitObs(void)
{
	long ISc, ISoOp, ISp, Itheta, ITwr;

	/* .. Initialize Spacecrafts .. */
    Sc = (struct ScType *) calloc(NSc,sizeof(struct ScType));
	if (Sc == NULL) {
		printf("Sc calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	if(OrbModel == ORB_IMPORT){
		for (ISc=0; ISc<NSc; ISc++){
			Sc[ISc].ID = ISc;
			strcpy(Sc[ISc].OrbFilename, InputSoOp->Sc[ISc].OrbFilename);
			Sc[ISc].flagUpdated = FALSE;
		}
	}
	else if(OrbModel == ORB_42 || OrbModel == ORB_42_DECAY){
		for (ISc=0; ISc<NSc; ISc++){
			Sc[ISc].ID = ISc;
			Sc[ISc].flagUpdated = FALSE;
		}
	}

	/* .. Initialize SoOp Configurations .. */
	SoOp = (struct SoOpType *) calloc(NSoOp,sizeof(struct SoOpType));
	if (SoOp == NULL) {
		printf("SoOp calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	for(ISoOp=0; ISoOp<NSoOp; ISoOp++){
		SoOp[ISoOp].ID = ISoOp;
		strcpy(SoOp[ISoOp].TxName, InputSoOp->S[ISoOp].TxName);
		SoOp[ISoOp].RefOrbFirst = InputSoOp->S[ISoOp].RefOrbFirst;
		SoOp[ISoOp].RefOrbLast = InputSoOp->S[ISoOp].RefOrbLast;
		SoOp[ISoOp].NTx = SoOp[ISoOp].RefOrbLast - SoOp[ISoOp].RefOrbFirst;
		SoOp[ISoOp].NSp = InputSoOp->S[ISoOp].NSp; // The Maximum Number of Specular Points
		SoOp[ISoOp].Freq = InputSoOp->S[ISoOp].Freq; // [Hz]
		SoOp[ISoOp].Wavelength = LIGHTSPEED / SoOp[ISoOp].Freq;
		SoOp[ISoOp].Wavenumber = TwoPi * SoOp[ISoOp].Freq / LIGHTSPEED;
		SoOp[ISoOp].h = ( 2*InputSoOp->RMSH*TwoPi / (SoOp[ISoOp].Wavelength*100) ) * ( 2*InputSoOp->RMSH*TwoPi / (SoOp[ISoOp].Wavelength*100) );
		SoOp[ISoOp].EIRP_Tx = InputSoOp->S[ISoOp].EIRP_Tx; // Transmitter EIRP [dB]
		SoOp[ISoOp].G_Rx = InputSoOp->S[ISoOp].G_Rx; // Receive Antenna Gain [dB]
		SoOp[ISoOp].Bandwidth = InputSoOp->S[ISoOp].Bandwidth; // [Hz]
		SoOp[ISoOp].t_coh = InputSoOp->S[ISoOp].t_coh; // Coherent Integration Time [sec]
		SoOp[ISoOp].AntTag = InputSoOp->S[ISoOp].AntTag; // Antenna Type - USER-DEFINED, DIPOLE, PATCH etc.
		
		SoOp[ISoOp].polTx = InputSoOp->S[ISoOp].polTx; // Transmitter Antenna Polarization - POL_R, POL_L, POL_X, POL_Y
		SoOp[ISoOp].polRx = InputSoOp->S[ISoOp].polRx; // Receiver Antenna Polarization - POL_R, POL_L, POL_X, POL_Y
		SoOp[ISoOp].hpbw = InputSoOp->S[ISoOp].hpbw; // Half-power beanwidth of the receive antenna pattern [deg]
		SoOp[ISoOp].SLL = InputSoOp->S[ISoOp].SLL; // First Sidelobe level of the receive antenna pattern [dB]
		SoOp[ISoOp].XPL = InputSoOp->S[ISoOp].XPL; // Cross-polarization level of the receive antenna pattern [dB]
		SoOp[ISoOp].AntPatRes = InputSoOp->S[ISoOp].AntPatRes; // Receive antenna pattern resolution [deg]
		SoOp[ISoOp].NoiseTemS = InputSoOp->S[ISoOp].NoiseTemS; // Noise Temperature of Sky-view Antenna [K]
		SoOp[ISoOp].NoiseTemE = InputSoOp->S[ISoOp].NoiseTemE; // Noise Temperature of Earth-view Antenna [K]
		SoOp[ISoOp].minSurfRes = InputSoOp->S[ISoOp].minSurfRes;
		SoOp[ISoOp].NthetaLocal = InputSoOp->S[ISoOp].NthetaLocal;
		SoOp[ISoOp].NphiLocal = InputSoOp->S[ISoOp].NphiLocal;
		SoOp[ISoOp].sres_req = 9000; // Spatial Resolution Requirement [m]

		SoOp[ISoOp].Sp = (struct SpecularType *) calloc(SoOp[ISoOp].NSp,sizeof(struct SpecularType));
		if (SoOp[ISoOp].Sp == NULL) {
			printf("SoOp[%ld].Sp calloc returned null pointer.  Bailing out!\n", ISoOp);
			exit(1);
		}
		
		for(ISp=0; ISp<SoOp[ISoOp].NSp; ISp++){
			SoOp[ISoOp].Sp[ISp].K = InputSoOp->S[ISoOp].Sp[ISp].K;

			SoOp[ISoOp].Sp[ISp].LocalGrid = (struct LocalGridType **)calloc(SoOp[ISoOp].NthetaLocal, sizeof(struct LocalGridType));
			for(Itheta=0; Itheta<SoOp[ISoOp].NthetaLocal; Itheta++)
				SoOp[ISoOp].Sp[ISp].LocalGrid[Itheta] = (struct LocalGridType *)calloc(SoOp[ISoOp].NphiLocal, sizeof(struct LocalGridType));
		}

		if(SoOp[ISoOp].AntTag == USER_DEFINED){
			// Rx Antenna Pattern File
			strcpy(SoOp[ISoOp].AntFileName[0], InputSoOp->S[ISoOp].AntFileName[0]);
			strcpy(SoOp[ISoOp].AntFileName[1], InputSoOp->S[ISoOp].AntFileName[1]);
			strcpy(SoOp[ISoOp].AntFileName[2], InputSoOp->S[ISoOp].AntFileName[2]);
			strcpy(SoOp[ISoOp].AntFileName[3], InputSoOp->S[ISoOp].AntFileName[3]);
            SoOp[ISoOp].AntPattern = LoadAntPattern(SoOp[ISoOp].AntFileName, &SoOp[ISoOp].AntPatRes, SoOp[ISoOp].AntPattern);
		}
		else if(SoOp[ISoOp].AntTag == GAUSSIAN){
            SoOp[ISoOp].AntPattern = LoadAntPatternGG(SoOp[ISoOp].SLL, SoOp[ISoOp].XPL, SoOp[ISoOp].hpbw, SoOp[ISoOp].AntPatRes, SoOp[ISoOp].AntPattern);
		}
		//ReportAntPattern(SoOp[ISoOp].AntFileName, SoOp[ISoOp].AntPatRes, SoOp[ISoOp].AntPattern);
	}

	if(ObsBase == TOWER){
		int i;
		Twr = (struct TwrType *) calloc(NRx,sizeof(struct TwrType));
		if (Twr == NULL) {
			printf("Twr calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		for(ITwr=0; ITwr<NRx; ITwr++){
			strcpy(Twr[ITwr].label, InputSoOp->Twr[ITwr].label);
			Twr[ITwr].ID = ITwr;
			Twr[ITwr].lat = InputSoOp->Twr[ITwr].lat;
			Twr[ITwr].lon = InputSoOp->Twr[ITwr].lon;
			Twr[ITwr].alt = InputSoOp->Twr[ITwr].alt;
			Twr[ITwr].lat *= D2R;
			Twr[ITwr].lon *= D2R;

			GRS80ToECEF(Twr[ITwr].lat, Twr[ITwr].lon, Twr[ITwr].alt, Twr[ITwr].PosW);
			// ECEF to ECI
			MTxV(World[EARTH].CWN, Twr[ITwr].PosW, Twr[ITwr].PosN);
			for(i=0; i<3; i++){
				Twr[ITwr].VelN[i] = 0;
				Twr[ITwr].VelW[i] = 0;
			}
			newFindCLN(Twr[ITwr].PosN, Twr[ITwr].VelN, Twr[ITwr].CLN);
		}
	}

	Icalval = 0;
}

void InitSOCRATES(int argc,char **argv)
{
    FILE *infile;
    char junk[120], newline, response[120];
	long ILayer, ITarget, IGrid, temp, i;
	double rE, sinLat2;
    double a = 6378137.0;
    double f = 1.0/298.257223563; // WGS84
    double e2 = f*(2.0-f);
	int row, col;
	double RMSH;

	/* .. Initialize Math Constants .. */
	InitConstants();
	
	/* .. Set directories .. */
	InitDirectories(argc, argv);

	/* .. Initialize Input/Output Parameters Structure .. */
	InputSoOp = (struct InputType *) calloc(1, sizeof(struct InputType));
	if (InputSoOp == NULL) {
		printf("InputSoOp calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	Out = (struct OutputType *) calloc(1, sizeof(struct OutputType));
	if (Out == NULL) {
		printf("Output calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	
	/* .. Read from file Inp_Main.txt */
    infile=FileOpen(SimPath,"Inp_Main.txt","r");

	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	
	// Observation Mode
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	SimMode = decodeStr(response);

	/* .. Load SoOp Parameters .. */
	if(SimMode == FIXED || SimMode == RETRIEVAL){
		ObsMode = FIXED;
		LoadFixed();
	}
	else if(SimMode == INORBIT || SimMode == DESIGN){
		ObsMode = INORBIT;
		LoadInorbit();
	}

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

    /* .. Dynamic Data .. */
    DynData = (struct DynDataType *) calloc(1,sizeof(struct DynDataType));

    // Data On/off
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	DynData->ExistVSM = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	DynData->ExistSoilTemp = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	DynData->ExistFreezeThaw = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	DynData->ExistSnowCover = decodeStr(response);

	// Ground Structure (SINGLE, MULTI)
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	GndModel = decodeStr(response);

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

    // Single-Layered Soil
	if(GndModel == SINGLE){
	    NLayer = 1;

	    GndData = (struct GndDataType *) calloc(NLayer,sizeof(struct GndDataType));
		if (GndData == NULL) {
			printf("GndData calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
	    fscanf(infile,"%s %[^\n] %[\n]",GndData->filename,junk,&newline);
	    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

	    fscanf(infile,"%ld %[^\n] %[\n]",&temp,junk,&newline);
		for(i=0; i<16; i++)
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		for(ILayer=0; ILayer<temp; ILayer++){
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		}

	}
	// Multi-Layered Soil
	else if(GndModel == MULTI){
	    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

		/* .. Number of Input Soil Layers .. */
	    fscanf(infile,"%ld %[^\n] %[\n]",&NLayer,junk,&newline);
	    GndData = (struct GndDataType *) calloc(NLayer,sizeof(struct GndDataType));
		if (GndData == NULL) {
			printf("GndData calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		// .. Depth Calculation .. //
		fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
		GndData->flagPenDep = decodeStr(response);
		
		// .. Soil Moisture Profile Model .. //
		fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
		SmpModel = decodeStr(response);

		// .. Geophysical Data Source .. //
		fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
		DataSrc = decodeStr(response);

		MultiLayer = (struct MultiLayerType *) calloc(1,sizeof(struct MultiLayerType));
		if (MultiLayer == NULL) {
			printf("MultiLayer calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}

		if(SmpModel == EXPO){
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);

			fscanf(infile,"%lf %[^\n] %[\n]", &MultiLayer->zA,junk,&newline);
			MultiLayer->zA = MultiLayer->zA * 0.01; // [cm] -> [m]
			fscanf(infile,"%lf %[^\n] %[\n]", &MultiLayer->zB,junk,&newline);
			MultiLayer->zB = MultiLayer->zB * 0.01; // [cm] -> [m]
			fscanf(infile,"%lf %[^\n] %[\n]", &MultiLayer->delZ,junk,&newline);
			MultiLayer->delZ = MultiLayer->delZ * 0.001; // [mm] -> [m]
			fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
			MultiLayer->FitFcn = decodeStr(response);
			
			// Skip POME
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		}
		else if(SmpModel == POME){
			// Skip EXPO
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
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

			//MultiLayer->NSublayer = (long) round((MultiLayer->D - 0.05)/MultiLayer->delZ) + 1;
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
			for(ILayer=0; ILayer<MultiLayer->NSublayer; ILayer++){
				MultiLayer->z[ILayer] = ILayer*MultiLayer->delZ;
			}
		}

		// .. Data Source .. //
		if(DataSrc == USCRN){
			Nc = (struct NcDataType *) calloc(1, sizeof(struct NcDataType));
			if (Nc == NULL) {
				printf("Nc calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
			
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			// .. USCRN Station List File Name .. //
			fscanf(infile,"%s %[^\n] %[\n]",Nc->filename,junk,&newline);
	
			// .. Start Year and End Year .. //
			fscanf(infile,"%ld %ld %ld %ld %[^\n] %[\n]",&Nc->startYear, &Nc->startMonth, &Nc->endYear, &Nc->endMonth, junk,&newline);

			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			for(ILayer=0; ILayer<NLayer; ILayer++){
				fscanf(infile,"%[^\n] %[\n]",junk,&newline);
				fscanf(infile,"%[^\n] %[\n]",junk,&newline);
				fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			}
		}
		else if(DataSrc == OTHERS){		
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);
			fscanf(infile,"%[^\n] %[\n]",junk,&newline);

			for(ILayer=0; ILayer<NLayer; ILayer++){
				fscanf(infile,"%[^\n] %[\n]",junk,&newline);
				fscanf(infile,"%lf %[^\n] %[\n]",&GndData[ILayer].depth,junk,&newline);
				GndData[ILayer].depth *= 0.01; // [cm] -> [m]
				fscanf(infile,"%s %[^\n] %[\n]",GndData[ILayer].filename,junk,&newline);
			}
			if(SmpModel == EXPO){
				// Layer Bottom Depth and Thickness
				for(ILayer=1; ILayer<NLayer; ILayer++){
					GndData[ILayer-1].layer_bottom = (GndData[ILayer-1].depth + GndData[ILayer].depth)/2;
				}
				GndData[NLayer-1].layer_bottom = GndData[NLayer-2].layer_bottom + MultiLayer->zB;
				GndData[0].layer_thickness = GndData[0].layer_bottom;
				for(ILayer=1; ILayer<NLayer; ILayer++){
					GndData[ILayer].layer_thickness = GndData[ILayer].layer_bottom - GndData[ILayer-1].layer_bottom;
				}

				// Total Layers
				MultiLayer->zS = MultiLayer->zA + GndData[NLayer-1].layer_bottom;

				// Layer Profile
				MultiLayer->NSublayer = MultiLayer->zS/MultiLayer->delZ + 1;
				MultiLayer->z = (double *) calloc(MultiLayer->NSublayer,sizeof(double));
				if (MultiLayer->z == NULL) {
					printf("MultiLayer->z calloc returned null pointer.  Bailing out!\n");
					exit(1);
				}
				for(ILayer=0; ILayer<MultiLayer->NSublayer; ILayer++){
					MultiLayer->z[ILayer] = ILayer * MultiLayer->delZ;
				}
			}
		}
	}
	// .. Soil Temperature .. //
	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_soilTemp1,junk,&newline);

    // Snow
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[0],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[1],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[2],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[3],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[4],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[5],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[6],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[7],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[8],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[9],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[10],junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",DynData->filename_snowCover[11],junk,&newline);

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

    /* .. Static Data .. */
	SttData = (struct SttDataType *) calloc(1,sizeof(struct SttDataType));
	if (SttData == NULL) {
		printf("SttData calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

    // Data On/off
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	SttData->ExistDEM = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	SttData->ExistLandMask = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	SttData->ExistLandCover = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	SttData->ExistSoilTexture = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	SttData->ExistSoilPorosity = decodeStr(response);
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	SttData->ExistVeg = decodeStr(response);
	// Surface Roughness RMSH
	fscanf(infile,"%lf %[^\n] %[\n]",&RMSH,junk,&newline);
	InputSoOp->RMSH = RMSH;

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    // Digital Elevation Model
	fscanf(infile,"%s %[^\n] %[\n]",SttData->filename_DEM,junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

	// Land Mask
	fscanf(infile,"%s %[^\n] %[\n]",SttData->filename_landMask,junk,&newline);
    fscanf(infile,"%s %[^\n] %[\n]",SttData->filename_landType,junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

    // Soil Texture
	fscanf(infile,"%s %[^\n] %[\n]",SttData->filename_fixed, junk, &newline);
    fscanf(infile,"%s %[^\n] %[\n]",SttData->filename_sand, junk, &newline);
    fscanf(infile,"%s %[^\n] %[\n]",SttData->filename_clay, junk, &newline);
    fscanf(infile,"%s %[^\n] %[\n]",SttData->filename_rho_b, junk, &newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

	// Soil Porosity
    fscanf(infile,"%s %[^\n] %[\n]",SttData->filename_porosity, junk, &newline);

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

	// .. Grid .. //
	long gridType; // TRUE if EASE2, FALSE if user-defined
	double gridRes; // EASE2 grid resolution
	char gridProj; // EASE 2 grid projection
	char grid_name[8];
	// Use Grid?
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	ExistGrid = decodeStr(response);
	// EASE2 Grid or User-defined Grid
	fscanf(infile,"%s %[^\n] %[\n]",response,junk,&newline);
	gridType = decodeStr(response);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	// EASE2 Grid
	fscanf(infile,"%lf %s %[^\n] %[\n]",&gridRes, &gridProj,junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
	// User-defined Grid
	// Number of User-defined Grids
	fscanf(infile,"%ld %[^\n] %[\n]",&NGrid,junk,&newline);
	double pixelSize[NGrid], lat0[NGrid], lon0[NGrid];
	long nRow[NGrid], nCol[NGrid];
	for(IGrid=0; IGrid<NGrid; IGrid++){
		fscanf(infile,"%[^\n] %[\n]",junk,&newline);
		// Pixel Size
		fscanf(infile,"%lf %[^\n] %[\n]",&pixelSize[IGrid],junk,&newline);
		pixelSize[IGrid] *= 1E3;
		// Origin
		fscanf(infile,"%lf %lf %[^\n] %[\n]",&lat0[IGrid], &lon0[IGrid],junk,&newline);
		lat0[IGrid] *= D2R;
		lon0[IGrid] *= D2R;
		// Number of Pixels
		fscanf(infile,"%ld %ld %[^\n] %[\n]",&nRow[IGrid], &nCol[IGrid],junk,&newline);
	}

	if(gridType == EASE2){
		NGrid = 1;
	}
	if(ExistGrid && (ObsMode == INORBIT)){
		printf(">> Number of Grids = %ld\n", NGrid);
		Grid = (struct EASEgridType *) calloc(NGrid, sizeof(struct EASEgridType));
		if (Grid == NULL) {
			printf("Grid calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		Map = (struct MapType *) calloc(1, sizeof(struct MapType));
		if (Map == NULL) {
			printf("Map calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		if(gridType == EASE2){
			sprintf(grid_name, "%c%.2lf", gridProj, gridRes);
			ease2_grid_info(Grid, grid_name);
			Grid->lat = (double *)calloc(Grid->nRow, sizeof(double));
			if (Grid->lat == NULL) {
				printf("Grid->lat calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
			Grid->lon = (double *)calloc(Grid->nCol, sizeof(double));
			if (Grid->lon == NULL) {
				printf("Grid->lon calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
			Grid->row = (long *)calloc(Grid->nRow, sizeof(long));
			if (Grid->row == NULL) {
				printf("Grid->row calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
			Grid->col = (long *)calloc(Grid->nCol, sizeof(long));
			if (Grid->col == NULL) {
				printf("Grid->col calloc returned null pointer.  Bailing out!\n");
				exit(1);
			}
			ease2_map_info(Map, Grid->projection);
			ease2_inverseM(Grid);
			Grid->latLimits[0] = Grid->lat[Grid->nRow-1];
			Grid->latLimits[1] = Grid->lat[0];
			Grid->lonLimits[0] = Grid->lon[0];
			Grid->lonLimits[1] = Grid->lon[Grid->nCol-1];
		}
		else if(gridType == USER_DEFINED){
			for(IGrid=0; IGrid<NGrid; IGrid++){
				Grid[IGrid].projection = 'M';
				Grid[IGrid].map_scale_m = pixelSize[IGrid];
				Grid[IGrid].lat0 = lat0[IGrid];
				Grid[IGrid].lon0 = lon0[IGrid];
				Grid[IGrid].nRow = nRow[IGrid];
				Grid[IGrid].nCol = nCol[IGrid];
				Grid[IGrid].lat = (double *)calloc(Grid[IGrid].nRow, sizeof(double));
				if (Grid[IGrid].lat == NULL) {
					printf("Grid[IGrid].lat calloc returned null pointer.  Bailing out!\n");
					exit(1);
				}
				Grid[IGrid].lon = (double *)calloc(Grid[IGrid].nCol, sizeof(double));
				if (Grid[IGrid].lon == NULL) {
					printf("Grid[IGrid].lon calloc returned null pointer.  Bailing out!\n");
					exit(1);
				}
				Grid[IGrid].row = (long *)calloc(Grid[IGrid].nRow, sizeof(long));
				if (Grid[IGrid].row == NULL) {
					printf("Grid[IGrid].row calloc returned null pointer.  Bailing out!\n");
					exit(1);
				}
				Grid[IGrid].col = (long *)calloc(Grid[IGrid].nCol, sizeof(long));
				if (Grid[IGrid].col == NULL) {
					printf("Grid[IGrid].col calloc returned null pointer.  Bailing out!\n");
					exit(1);
				}
				ease2_map_info(Map, Grid[IGrid].projection);
				ease2_userdefined();
				Grid[IGrid].latLimits[0] = Grid[IGrid].lat[Grid[IGrid].nRow-1];
				Grid[IGrid].latLimits[1] = Grid[IGrid].lat[0];
				Grid[IGrid].lonLimits[0] = Grid[IGrid].lon[0];
				Grid[IGrid].lonLimits[1] = Grid[IGrid].lon[Grid[IGrid].nCol-1];
			}
		}
		// Report Grid vectors
		ReportGrid();
	}

    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

    /* .. Target .. */
	fscanf(infile,"%ld %[^\n] %[\n]",&NTarget,junk,&newline);
	TargetArea = (struct TargetAreaType *) calloc(NTarget, sizeof(struct TargetAreaType));
	if (TargetArea == NULL) {
		printf("TargetArea calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	for (ITarget=0; ITarget<NTarget; ITarget++){
		fscanf(infile,"%s %lf %lf %lf %[^\n] %[\n]",
				TargetArea[ITarget].name, &TargetArea[ITarget].radius, &TargetArea[ITarget].lat, &TargetArea[ITarget].lon, junk,&newline);
		row = TargetArea[ITarget].lat*60+5399.9999;
		col = TargetArea[ITarget].lon*60+10799.9999;
		TargetArea[ITarget].lat *= D2R;
		TargetArea[ITarget].lon *= D2R;
		sinLat2 = sin(TargetArea[ITarget].lat)*sin(TargetArea[ITarget].lat);
		rE = a*sqrt((1-e2*(2-e2)*sinLat2) / (1-e2*sinLat2));
		TargetArea[ITarget].alt = SttData->height[row][col] + 6371000 - rE;

		// GRS80 to ECEF
		TargetArea[ITarget].radius *= 1000;
    	GRS80ToECEF(TargetArea[ITarget].lat, TargetArea[ITarget].lon, TargetArea[ITarget].alt, TargetArea[ITarget].PosW);
    	/* ECEF to ECI */
    	MTxV(World[EARTH].CWN, TargetArea[ITarget].PosW, TargetArea[ITarget].PosN);
	}

	fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);
    fscanf(infile,"%[^\n] %[\n]",junk,&newline);

    /* .. MUOS spot beam radius .. */
    SpotBeam = (struct SpotBeamType *) calloc(5,sizeof(struct SpotBeamType));
	double scaleFactor, halfBW, altTx;
	long iTx;
	fscanf(infile,"%lf %[^\n] %[\n]",&scaleFactor,junk,&newline);
	fscanf(infile,"%lf %lf %[^\n] %[\n]",&halfBW, &altTx, junk,&newline);
	for(iTx=0; iTx<5; iTx++){
		SpotBeam[iTx].scaleFactor = scaleFactor;
		SpotBeam[iTx].halfBW = halfBW*D2R/2;
		SpotBeam[iTx].altTx = altTx*1000;
	}
	InitSpotbeams();

	fclose(infile);

	/* .. Initialize Local Ground Parameters .. */
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
	for(ILayer=0; ILayer<NLayer; ILayer++)
		Gnd->VSM[ILayer] = NAN;

	Gnd->e_c = (double complex *)calloc(NLayer, sizeof(double complex));
	if (Gnd->e_c == NULL) {
		printf("Gnd->e_c calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}
	if(GndModel == MULTI){
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
	}

	/* .. Initialize Bistatic Parameters .. */
	Bistatic = (struct BistaticType *)calloc(1, sizeof(struct BistaticType));
	if (Bistatic == NULL) {
		printf("Bistatic parameter calloc returned null pointer.  Bailing out!\n");
		exit(1);
	}

	/* .. Initialize Parameters .. */
	if(ObsMode == FIXED){
		InitFixedObs();
	}
	else if(ObsMode == INORBIT){
		/* .. Load Surface Static Data .. */
		LoadStaticData();
	    /* .. Load Soil Dynamic Data .. */
		LoadSoilData();
		InitInorbitObs();
		/* .. Initialize Orbit Propagator .. */
		if(OrbModel == ORB_IMPORT){
			OutFlag = TRUE;
			// import orbit
		}
		else{
			InitSim(argc, argv);
			CmdInterpreter();
		}
	}

	if(SttData->ExistVeg){
		LoadVegetation();
		InitVegetation();
	}

	freeInputSoOp();
}