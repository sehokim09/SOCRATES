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

void ImportOrb(void)
{
	static FILE **inFile;
	static long IData = 0;
	static long First = TRUE;
	long ISc, i;
	char junk[240], newline;
    double GMST, ZAxis[3] = {0.0,0.0,1.0};
	double C_TEME_TETE[3][3], C_TETE_J2000[3][3], C_Z_Rot[3][3], C_TEME_J2000[3][3];
	double PosN_km[3], VelN_km_s[3];

	//* .. Open Trajectory Files .. *//
	if(First){
		First = FALSE;
		inFile = (FILE **)calloc(NSc, sizeof(FILE *));
		if (inFile == NULL) {
			printf("inFile calloc returned null pointer.  Bailing out!\n");
			exit(1);
		}
		for(ISc=0; ISc<NSc; ISc++){
			inFile[ISc] = FileOpen(OrbPathImport, Sc[ISc].OrbFilename, "r");
			fscanf(inFile[ISc],"%[^\n] %[\n]",junk,&newline); // Skip Header
		}	
	}

	//* .. Update Position, Velocity, and CLN .. *//
	for(ISc=0; ISc<NSc; ISc++){
		fscanf(inFile[ISc],"%lf %lf %lf %lf %lf %lf %lf %[\n]", &ModJulDay, &PosN_km[0], &PosN_km[1], &PosN_km[2],
																 		&VelN_km_s[0], &VelN_km_s[1], &VelN_km_s[2], &newline);
		for(i=0; i<3; i++){
			Sc[ISc].PosN[i] = PosN_km[i] * 1E3;
			Sc[ISc].VelN[i] = VelN_km_s[i] * 1E3;
		}
		newFindCLN(Sc[ISc].PosN, Sc[ISc].VelN, Sc[ISc].CLN);
	}

	//* .. Update Epoch .. *//
//	JulDay = ModJulDay + 2430000; // Only for GMAT
	UTC.JulDay = ModJulDay + 2400000.5; // Common conversion
	JD2YMDHMS(UTC.JulDay, &UTC.Year, &UTC.Month, &UTC.Day, &UTC.Hour, &UTC.Minute, &UTC.Second);
	UTC.doy = MD2DOY(UTC.Year, UTC.Month, UTC.Day);

	/* .. Earth rotation is a special case */
	GMST = JD2GMST(UTC.JulDay);
	World[EARTH].PriMerAng = TwoPi*GMST;
//	SimpRot(ZAxis,World[EARTH].PriMerAng,World[EARTH].CWN);
	HiFiEarthPrecNute(UTC.JulDay,C_TEME_TETE,C_TETE_J2000);
	SimpRot(ZAxis,World[EARTH].PriMerAng,C_Z_Rot);
	MxM(C_TETE_J2000,C_TEME_TETE,C_TEME_J2000);
	MxM(C_Z_Rot,C_TEME_J2000,World[EARTH].CWN);
	
	//* .. Report Progress .. *//
	updateProgress(IData);

	//* .. Close Files .. *//
	if(EndFlag){
		for(ISc=0; ISc<NSc; ISc++)
			fclose(inFile[ISc]);
	}

	IData++;
}

void Import42(void)
{
	struct ScType *ScSoOp;
	struct SCType *Sc42;
	struct OrbitType *Orb42;
	long iSc, i, j;

	for(iSc=0; iSc<NSc; iSc++){
		ScSoOp = &Sc[iSc];
		Sc42 = &SC[iSc];
		Orb42 = &Orb[iSc];
		for(i=0; i<3; i++){
			ScSoOp->PosN[i] = Sc42->PosN[i];
			ScSoOp->VelN[i] = Sc42->VelN[i];
			for(j=0; j<3; j++){
				ScSoOp->CLN[i][j] = Orb42->CLN[i][j];
			}
		}
	}
}

void Propagator(void)
{
    if(OrbModel == ORB_IMPORT){
    	ImportOrb();
    }
    else if(OrbModel == ORB_42 || OrbModel == ORB_42_DECAY){
    	EndFlag = SimStep();
    	Import42();
    }
}
