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

void updateProgress(long cnt)
{
#define PROGRESSPERCENT 10

	static long ProgressPercent = 0;
	static long ProgressCtr = 0;
	static double ProgressTime = 0.0;

	if (cnt >= ProgressTime) {
		ProgressCtr++;
		ProgressTime = (double) (ProgressCtr*PROGRESSPERCENT)/100.0*NData - 1;
		printf(">> SoOp %s is %3.1li%% Complete\n", SimPath, ProgressPercent);
		ProgressPercent += PROGRESSPERCENT;
		if(ProgressPercent == COMPLETE){
			EndFlag = TRUE;
			ProgressPercent = 0;
			ProgressCtr = 0;
			ProgressTime = 0.0;	
		}
	}
}

void GainAdjustUD(struct ScType *Tx, struct SpecularType *Sp)
{
	if(Tx->alt>20000E3) // MEO and GEO
	{
		Sp->K = 1e5;
	}
	else // LEO
	{	
		Sp->K = 5e4;
	}
}

void ObsFixed(void)
{
	long iLayer, iData, NDataCheck, iSoOp, iPol;
	FILE *inFile[NLayer], *sttFile;
	char newline;
	struct FixedObsType *F;
	double VSM;

	//* .. Total Number of Data .. *//
	for(iLayer=0; iLayer<NLayer; iLayer++){
		inFile[iLayer] = FileOpen(DynDataPath, GndData[iLayer].filename, "r");
		fscanf(inFile[iLayer],"%ld %[\n]", &NData, &newline);
		if(iLayer==0)
			NDataCheck = NData;
		if(NDataCheck != NData){
			printf(">> Error: The number of soil moisture data at each layer must be the same\n");
			exit(1);
		}
	}
	printf(">> - Total # of Data = %ld\n\n", NData);

	//* .. Open Soil Texture File .. *//
	sttFile = FileOpen(SttDataPath, SttData->filename_fixed, "r");
	fscanf(sttFile,"%ld %[\n]", &NData, &newline);
	if(NDataCheck != NData){
		printf(">> Error: The number of soil texture data is not equal to the number of soil moisture data\n");
		exit(1);
	}

	//* .. Start Simulations .. *//
	printf(">> START SIMULATIONS\n");
	
	for(iSoOp=0; iSoOp<NSoOp; iSoOp++){
		for(iPol=0; iPol<NPol; iPol++){
			EndFlag = FALSE;
			F = &Fixed[iSoOp];
			F->polRx = RxPol[iPol];
			printf("\n>> SoOp Configuration #%ld: Tx Freq = %lf MHz, Rx Pol = %c\n", iSoOp+1, F->Freq*1E-6, F->polRx);
			
			//* .. Bistatic Geometry .. *//
			calcTxGeometry(F);
			calcGeometryFixed(F);
			updateBistaticFixed(F);
			calcAntPolRot();
			
			// .. Effective Roughness Paramter .. //
			Gnd->h = F->h;

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
				LookupAntPattern(F->AntPattern, F->AntPatRes, Pi - F->AngT2R_rf[0], Pi - F->AngT2R_rf[1], Bistatic->g_rt);
				// Specular direction
				LookupAntPattern(F->AntPattern, F->AntPatRes, F->AngS2R_rf[0], F->AngS2R_rf[1], Bistatic->g_rs);
			}
			
			// Vegetation
			if(SttData->ExistVeg){
				printf("iSoOp = %ld, th = %lf, AngT2S_sf_th0 = %lf\n", iSoOp, Bistatic->th*R2D, Bistatic->AngT2S_sf_th0*R2D);
				calcVegT(iSoOp, 0);
			}

			// Direct Term
			calcDirectTerm();
			
			for(iData=0; iData<NData; iData++){
				// Progress
				updateProgress(iData);

				// Load Soil Moisture
				for(iLayer=0; iLayer<NLayer; iLayer++){
					fscanf(inFile[iLayer],"%lf %[\n]", &VSM, &newline);
					Gnd->VSM[iLayer] = VSM;
				}

				// Load Soil Texture
				fscanf(sttFile,"%f %f %f %[\n]", &Gnd->sand, &Gnd->clay, &Gnd->bulk, &newline);
				
				// Ground Structure
				if(GndModel == MULTI){
					calcMLDielProfile();
					if(iSoOp == 0 && iPol == 0 && SmpModel == POME){
						ReportSMP();
					}
				}
				else if(GndModel == SINGLE){
					calcDielMironov();
				}

				// Specular Term
				calcSpecularTermForward();
				// Power
				calcPower();
				
				// Report
				ReportFixedObs(F, iData);
			}
			
			for(iLayer=0; iLayer<NLayer; iLayer++){
				rewind(inFile[iLayer]);
				fscanf(inFile[iLayer],"%ld %[\n]", &NData, &newline);
			}
			rewind(sttFile);
			fscanf(sttFile,"%ld %[\n]", &NData, &newline);
		}
	}

	for(iLayer=0; iLayer<NLayer; iLayer++)
		fclose(inFile[iLayer]);
	fclose(sttFile);
}

void ObsSpace(void)
{
    static uint8_t First = TRUE;
    struct ScType *Rx, *Tx;
	struct SoOpType *S;
	struct SpecularType *Sp;
	long iRx, iTx, iSoOp;
	long *TxNum = NULL;
	uint8_t i;

	double vRx2TxN[3], vRx2TxA[3], vRx2SpN[3], vRx2SpA[3];
	double vNadir[3] = {0.0, 0.0, 1.0};

	static long *TxCntTotal;
	long SpCnt[MAXNUM_TX];

    if (First) {
    	TxCntTotal = (long *) calloc(1, sizeof(long));
        First = FALSE;
		//reportSpotbeam();
    }

	for(iTx=NRx; iTx<NSc; iTx++){
		Sc[iTx].flagUpdated = FALSE;
	}

    for(iRx=0; iRx<NRx; iRx++){
    	Rx = &Sc[iRx];
    	// ECI to ECEF of Receiver's Position
    	MxV(World[EARTH].CWN, Rx->PosN, Rx->PosW);
       	// ECI to ECEF of Receiver's Velocity
    	MxV(World[EARTH].CWN, Rx->VelN, Rx->VelW);
    	// ECEF to GRS80
		Rx->lat_prev = Rx->lat;
    	ECEFToGRS80(Rx->PosW, &Rx->lat, &Rx->lon, &Rx->alt);
		if(Rx->lat_prev<0 && Rx->lat>= 0){
			Rx->orbitNum++;
		}

		if(Rx->alt<100E3){
			EndFlag = TRUE;
			printf(">> Deorbit of Rx %ld. Rx[%ld].alt < 100km\n", iRx, iRx);
		}
		
		// .. Housekeeping .. //
		if(ReportSoOp->AccessTime)
			ReportAccessTime(Rx);
		if(OrbModel != ORB_IMPORT && ReportSoOp->ADCS && iRx >= ReportSoOp->FirstRxID && iRx <= ReportSoOp->LastRxID)
			ReportAttitude(Rx);

		// .. Science Observation .. //
		// Select transmitters with the smallest off-boresight angle
	  	TxNum = FindVisibleTx(Rx, TxNum, TxCntTotal);

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

				Tx = &Sc[Sp->TxID];
				
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

				//* .. Update Specular Point Integrating DEM .. */.
				if(SttData->ExistDEM){
					LocalGeometryDEM(Rx->PosW, Tx->PosW, S, Sp);
					// GRS80 to ECEF
					GRS80ToECEF(Sp->lat, Sp->lon, Sp->alt, Sp->PosW);
				}

				if(ExistGrid){
					long iGrid;
					struct EASEgridType *G;
					for(iGrid=0; iGrid<NGrid; iGrid++){
						G = &Grid[iGrid];
						if(Sp->lon > G->lonLimits[0] && Sp->lon < G->lonLimits[1]){
							if(Sp->lat > G->latLimits[0] && Sp->lat < G->latLimits[1]){
								OutFlag = TRUE;
								break;
							}
							else{
								OutFlag = FALSE;
							}
						}
						else{
							OutFlag = FALSE;
						}
					}
				}
				// .. Update Spotbeam Footprint .. //
				if(!strcmp(S->TxName,"MUOS")){
					OutFlag = checkMUOSspotbeam(Rx, Sp, &SpotBeam[Tx->ID - S->RefOrbFirst]);
				}
				
				if(OutFlag){
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

					//* .. Load Geophysical Data .. *//
					updateLocalGnd(Sp->lat, Sp->lon, S->h);
		
					//* .. Update Geometry .. *//
					calcGeometryInorbit(Rx->PosW, Tx->PosW, Sp);

					//* .. Update Bistatic Radar Parameters .. *//
					updateBistaticInorbit(S, Sp->incAng);

					//* .. Antenna Rotation Matrices .. *//
					calcAntPolRot();

					//* .. Antenna Pattern .. * //
					if(S->AntTag == IDEAL){
						// Transmitter direction
						Bistatic->g_rt[0][0] = 1; Bistatic->g_rt[0][1] = 0;
						Bistatic->g_rt[1][0] = 0; Bistatic->g_rt[1][1] = 1;
						// Specular direction
						Bistatic->g_rs[0][0] = 1; Bistatic->g_rs[0][1] = 0;
						Bistatic->g_rs[1][0] = 0; Bistatic->g_rs[1][1] = 1;
					}
					else if(S->AntTag == GAUSSIAN){
						// Transmitter direction
						LookupAntPattern(S->AntPattern, S->AntPatRes, Pi - Sp->lookAngElD, Pi - Sp->lookAngAzD, Bistatic->g_rt);
						// Specular direction
						LookupAntPattern(S->AntPattern, S->AntPatRes, Sp->lookAngElR, Sp->lookAngAzR, Bistatic->g_rs);
					}
					else if(S->AntTag == USER_DEFINED){
						// Transmitter direction
						LookupAntPattern(S->AntPattern, S->AntPatRes, Sp->lookAngElD, Sp->lookAngAzD, Bistatic->g_rt);
						// Specular direction
						LookupAntPattern(S->AntPattern, S->AntPatRes, Sp->lookAngElR, Sp->lookAngAzR, Bistatic->g_rs);
					}
                    Sp->AntGainD = 10*log10(cabs(Bistatic->g_rt[0][0])*cabs(Bistatic->g_rt[0][0])) + S->G_Rx;
                    Sp->AntGainR = 10*log10(cabs(Bistatic->g_rs[0][1])*cabs(Bistatic->g_rs[0][1])) + S->G_Rx;

					//* .. Report .. *//
					ReportSpaceObs(Rx, S, Sp, Tx);
				}
			}
			for(iSoOp=0; iSoOp<NSoOp; iSoOp++)
				S[iSoOp].SpCnt = SpCnt[iSoOp];

			free(TxNum);
		}
		else{
			//ReportNoSignal(Rx);
		}
	}
}

void ObsTower(void)
{
    static uint8_t First = TRUE;
    struct ScType *Tx;
	struct TwrType *Rx;
	struct SoOpType *S;
	struct SpecularType *Sp;
	long iRx, iTx, iSoOp;
	long *TxNum = NULL;
	uint8_t i;

	double vRx2TxN[3], vRx2TxA[3], vRx2SpN[3], vRx2SpA[3];
	double vNadir[3] = {0.0, 0.0, 1.0};
	double RelPosN[3], Rhat[3];

	static long *TxCntTotal;
	long SpCnt[MAXNUM_TX];

    if (First) {
    	TxCntTotal = (long *) calloc(1, sizeof(long));
        First = FALSE;
    }

    for(iRx=0; iRx<NRx; iRx++){
    	Rx = &Twr[iRx];
		// ECEF to ECI
		MTxV(World[EARTH].CWN, Rx->PosW, Rx->PosN);

		// .. Science Observation .. //
		// Select transmitters with the smallest off-boresight angle
	  	TxNum = FindVisibleTxTwr(Rx->PosN, TxNum, TxCntTotal);

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

				Tx = &Sc[Sp->TxID];
				
				// ECI to ECEF of Transmitter's Position and Velocity
				MxV(World[EARTH].CWN, Tx->PosN, Tx->PosW);
				MxV(World[EARTH].CWN, Tx->VelN, Tx->VelW);
				
				// ECEF to GRS80
				ECEFToGRS80(Tx->PosW, &Tx->lat, &Tx->lon, &Tx->alt);

				//* .. Determine Specular Reflection Point on GRS80 in ECEF coordinate system .. *//
				RelPosN[0] = Tx->PosN[0] - Rx->PosN[0];
				RelPosN[1] = Tx->PosN[1] - Rx->PosN[1];
				RelPosN[2] = Tx->PosN[2] - Rx->PosN[2];
				UNITV(RelPosN);
				CopyUnitV(Rx->PosN,Rhat);
				Rx->elev = HalfPi - acos(VoV(RelPosN,Rhat));
				if(Rx->elev > 1*D2R){
					FindSP_UD(Rx->PosW, Tx->PosW, Sp);
					// ECEF to GRS80
					ECEFToGRS80(Sp->PosW, &Sp->lat, &Sp->lon, &Sp->alt);

					//* .. Update Specular Point Integrating DEM .. */.
					if(SttData->ExistDEM){
						LocalGeometryDEM(Rx->PosW, Tx->PosW, S, Sp);
						// GRS80 to ECEF
						GRS80ToECEF(Sp->lat, Sp->lon, Sp->alt, Sp->PosW);
					}

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

					//* .. Update Geometry .. *//
					calcGeometryInorbit(Rx->PosW, Tx->PosW, Sp);

					//* .. Antenna Pattern .. * //
					if(S->AntTag == IDEAL){
						// Transmitter direction
						Bistatic->g_rt[0][0] = 1; Bistatic->g_rt[0][1] = 0;
						Bistatic->g_rt[1][0] = 0; Bistatic->g_rt[1][1] = 1;
						// Specular direction
						Bistatic->g_rs[0][0] = 1; Bistatic->g_rs[0][1] = 0;
						Bistatic->g_rs[1][0] = 0; Bistatic->g_rs[1][1] = 1;
					}
					else{
						// Transmitter direction
						LookupAntPattern(S->AntPattern, S->AntPatRes, Sp->lookAngElD, Sp->lookAngAzD, Bistatic->g_rt);
						// Specular direction
						LookupAntPattern(S->AntPattern, S->AntPatRes, Sp->lookAngElR, Sp->lookAngAzR, Bistatic->g_rs);
					}
                    Sp->AntGainD = 10*log10(cabs(Bistatic->g_rt[0][0])*cabs(Bistatic->g_rt[0][0])) + S->G_Rx;
                    Sp->AntGainR = 10*log10(cabs(Bistatic->g_rs[0][1])*cabs(Bistatic->g_rs[0][1])) + S->G_Rx;
				}
				else{
					Sp->lat = NAN;
					Sp->lon = NAN;
					Sp->alt = NAN;
					Sp->PosW[0] = NAN;
					Sp->PosW[1] = NAN;
					Sp->PosW[2] = NAN;
					Sp->PosN[0] = NAN;
					Sp->PosN[1] = NAN;
					Sp->PosN[2] = NAN;
					Sp->lookAngElR = NAN;
					Sp->lookAngAzR = NAN;
					Sp->lookAngElD = NAN;
					Sp->lookAngAzD = NAN;
					Sp->elev = NAN;
					Sp->az = NAN;
					Sp->incAng = NAN;
				}
				
				//* .. Report .. *//
				ReportTowerObs(Rx, S, Sp, Tx);
			}
			for(iSoOp=0; iSoOp<NSoOp; iSoOp++)
				S[iSoOp].SpCnt = SpCnt[iSoOp];

			free(TxNum);
		}
		else{
			// No signal
		}
	}
}

void ObsDDM(void)
{
    static uint8_t First = TRUE;
	long nTx = NSc - NRx;
	static FILE *outFile[1][4][2];
    struct ScType *Rx, *Tx;
	struct SoOpType *S;
	struct SpecularType *Sp;
	long iRx, iTx, iSoOp;
	long *TxNum = NULL;
	uint8_t i;
	long iTh, iPh;

	double vRx2TxN[3], vRx2TxA[3], vRx2SpN[3], vRx2SpA[3], vSp2RxW[3];
	double vNadir[3] = {0.0, 0.0, 1.0};

	static long *TxCntTotal;
	long SpCnt[MAXNUM_TX];

    if (First) {
		char filename[30];
    	TxCntTotal = (long *) calloc(1, sizeof(long));
        First = FALSE;
        for(iRx=0; iRx<NRx; iRx++){
			for(iTx=0; iTx<nTx; iTx++){
				sprintf(filename,"Rx%ld_Tx%ld_Pos.out", iRx+1, iTx+1);
				outFile[iRx][iTx][0] = FileOpen(OutPath,filename,"w");
				sprintf(filename,"Rx%ld_Tx%ld_DelayDoppler.out", iRx+1, iTx+1);
				outFile[iRx][iTx][1] = FileOpen(OutPath,filename,"w");
			}
        }
    }

    for(iRx=0; iRx<NRx; iRx++){
    	Rx = &Sc[iRx];
    	// ECI to ECEF of Receiver's Position
    	MxV(World[EARTH].CWN, Rx->PosN, Rx->PosW);
       	// ECI to ECEF of Receiver's Velocity
    	MxV(World[EARTH].CWN, Rx->VelN, Rx->VelW);
    	// ECEF to GRS80
    	ECEFToGRS80(Rx->PosW, &Rx->lat, &Rx->lon, &Rx->alt);
		
		// .. Housekeeping .. //
		//if(ReportSoOp->AccessTime)
		//	ReportAccessTime(Rx);
		//if(OrbModel != ORB_IMPORT && ReportSoOp->ADCS && iRx >= ReportSoOp->FirstRxID && iRx <= ReportSoOp->LastRxID)
		//	ReportAttitude(Rx);

		// .. Science Observation .. //
		// Select transmitters with the smallest off-boresight angle
	  	TxNum = FindVisibleTx(Rx, TxNum, TxCntTotal);

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

				Tx = &Sc[Sp->TxID];
				
				// ECI to ECEF of Transmitter's Position and Velocity
				MxV(World[EARTH].CWN, Tx->PosN, Tx->PosW);
				MxV(World[EARTH].CWN, Tx->VelN, Tx->VelW);
				
				// ECEF to GRS80
				ECEFToGRS80(Tx->PosW, &Tx->lat, &Tx->lon, &Tx->alt);
				
				//* .. Determine Specular Reflection Point on GRS80 in ECEF coordinate system .. *//
				printf("Rx LLA: %lf %lf %lf, Tx %ld LLA: %lf %lf %lf\n", Rx->lat*R2D, Rx->lon*R2D, Rx->alt, Tx->ID, Tx->lat*R2D, Tx->lon*R2D, Tx->alt);
				//GainAdjustUD(Tx, Sp);				
				FindSP_UD(Rx->PosW, Tx->PosW, Sp);
				
				// ECEF to GRS80
				ECEFToGRS80(Sp->PosW, &Sp->lat, &Sp->lon, &Sp->alt);

				vSp2RxW[0] = Rx->PosW[0] - Sp->PosW[0];
				vSp2RxW[1] = Rx->PosW[1] - Sp->PosW[1];
				vSp2RxW[2] = Rx->PosW[2] - Sp->PosW[2];

				//* .. Update Bistatic Radar Parameters .. *//
				updateBistaticInorbit(S, Sp->incAng);

				// Compute delay and doppler at specular point
				calcDelayDoppler(Rx->PosN, Rx->VelN, Sp, Tx);
				
				// Compute delay and doppler around the initial specular point
				LocalGeometry(vSp2RxW, Rx, S, Sp, Tx);

				if(OutFlag){
					// Position and Velocity
					fprintf(outFile[iRx][Sp->TxID-NRx][0], "%lf %ld %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
							TT.JulDay, Sp->TxID, Tx->lat, Tx->lon, Tx->alt, Rx->lat, Rx->lon, Rx->alt,
							Sp->lat, Sp->lon, Sp->alt);
					for(i=0;i<3;i++) {
						vRx2TxN[i] = Tx->PosN[i] - Rx->PosN[i]; // for Sky-view antenna look angle & slant range
					}
					UNITV(vRx2TxN);

					// Sky-view (Direct signal) antenna look angle
					MxV(Rx->CLN, vRx2TxN, vRx2TxA); // opposite direction vector of direct signal in Antenna body frame

					Sp->lookAngElD = acos(VoV(vRx2TxA,vNadir));
					Sp->lookAngAzD = atan2TwoPi(vRx2TxA[1],vRx2TxA[0]);
					
					// Delay and Doppler
					for(iTh=0; iTh<S->NthetaLocal; iTh++){
						for(iPh=0; iPh<S->NphiLocal; iPh++){
							// ECEF to ECI
							MTxV(World[EARTH].CWN, Sp->LocalGrid[iTh][iPh].PosW, Sp->LocalGrid[iTh][iPh].PosN);

							for(i=0;i<3;i++) {
								vRx2SpN[i] = Sp->LocalGrid[iTh][iPh].PosN[i] - Rx->PosN[i]; // for Earth-view antenna look angle
							}
							UNITV(vRx2SpN);

							// Earth-view (Reflected signal) antenna look angle
							MxV(Rx->CLN, vRx2SpN, vRx2SpA); // opposite direction vector of reflected signal in Antenna body frame

							Sp->LocalGrid[iTh][iPh].lookAngElR = acos(VoV(vRx2SpA,vNadir));
							Sp->LocalGrid[iTh][iPh].lookAngAzR = atan2TwoPi(vRx2SpA[1],vRx2SpA[0]);

							//* .. Load Geophysical Data .. *//
							updateLocalGnd(Sp->LocalGrid[iTh][iPh].lat, Sp->LocalGrid[iTh][iPh].lon, S->h);
				
							//* .. Update Geometry .. *//
							calcGeometryInorbitLocal(Rx->PosW, Tx->PosW, &Sp->LocalGrid[iTh][iPh]);

							//* .. Update Bistatic Radar Parameters .. *//
							updateBistaticInorbit(S, Sp->LocalGrid[iTh][iPh].incAngL2Tx);

							//* .. Antenna Rotation Matrices .. *//
							calcAntPolRot();

							//* .. Antenna Pattern .. * //
							if(S->AntTag == IDEAL){
								// Transmitter direction
								Bistatic->g_rt[0][0] = 1; Bistatic->g_rt[0][1] = 0;
								Bistatic->g_rt[1][0] = 0; Bistatic->g_rt[1][1] = 1;
								// Specular direction
								Bistatic->g_rs[0][0] = 1; Bistatic->g_rs[0][1] = 0;
								Bistatic->g_rs[1][0] = 0; Bistatic->g_rs[1][1] = 1;
							}
							else{
								// Transmitter direction
								LookupAntPattern(S->AntPattern, S->AntPatRes, Sp->lookAngElD, Sp->lookAngAzD, Bistatic->g_rt);
								// Specular direction
								LookupAntPattern(S->AntPattern, S->AntPatRes, Sp->LocalGrid[iTh][iPh].lookAngElR, Sp->LocalGrid[iTh][iPh].lookAngAzR, Bistatic->g_rs);
							}
							// Ground Structure
							if(isnan(Gnd->VSM[0])){
								for(i=0; i<2; i++){
									Out->b_coh1[i] = NAN;
									Out->b_coh2[i] = NAN;
									Out->P_coh1[i] = NAN;
									Out->P_coh2[i] = NAN;
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
							//calcDirectTerm();
							calcPower();
							//calcSNR(S);
							
							fprintf(outFile[iRx][Sp->TxID-NRx][1], "%lf %ld %lf %lf %lf %le %le %lf %lf %lf %ld %ld %le %le\n",
									TT.JulDay, Sp->TxID, Sp->r, Sp->LocalGrid[iTh][iPh].theta, Sp->LocalGrid[iTh][iPh].phi, Sp->LocalGrid[iTh][iPh].delay, Sp->LocalGrid[iTh][iPh].doppler,
									Sp->LocalGrid[iTh][iPh].lat, Sp->LocalGrid[iTh][iPh].lon, Sp->LocalGrid[iTh][iPh].alt,
									S->NthetaLocal, S->NphiLocal,
									Out->PWR_coh1[0], Out->PWR_coh1[1]);
						}
					}
				}
			}
			for(iSoOp=0; iSoOp<NSoOp; iSoOp++)
				S[iSoOp].SpCnt = SpCnt[iSoOp];

			free(TxNum);
		}
//		else{
//			ReportNoSignal(Rx);
//		}
	}
}