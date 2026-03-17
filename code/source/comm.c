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

void ReportAccessTime(struct ScType *Rx)
{
	static FILE *outFile1[MAXNUM_RX], *outFile2[MAXNUM_RX];
	char filename[20];
	static char First = TRUE;
    struct GsType *G;
	double Rhat[3], RelPosW[3];
	long iGs, iRx;
    long flagWrite = FALSE;
	double angLoS_Gs, angLoS_Rx;
    
	if(First){
		First = FALSE;
		for(iRx=0; iRx<NRx; iRx++){
			sprintf(filename, "AccessTime_Rx%ld.out", iRx);
			outFile1[iRx] = FileOpen(OutPath, filename, "w");
			sprintf(filename, "AccessArc_Rx%ld.out", iRx);
			outFile2[iRx] = FileOpen(OutPath, filename, "w");
		}
	}

    iRx = Rx->ID;
    
    for(iGs=0; iGs<NGs; iGs++){
        G = &Gs[iGs];
        CopyUnitV(Rx->PosW,Rhat);
        Rhat[0] = -Rhat[0];
        Rhat[1] = -Rhat[1];
        Rhat[2] = -Rhat[2];
        RelPosW[0] = G->PosW[0] - Rx->PosW[0];
        RelPosW[1] = G->PosW[1] - Rx->PosW[1];
        RelPosW[2] = G->PosW[2] - Rx->PosW[2];
        UNITV(RelPosW);
        angLoS_Rx = acos(VoV(RelPosW,Rhat));
        if(angLoS_Rx < G->beamwidth_Rx) {
            CopyUnitV(G->PosW,Rhat);
            RelPosW[0] = -RelPosW[0];
            RelPosW[1] = -RelPosW[1];
            RelPosW[2] = -RelPosW[2];
            angLoS_Gs = acos(VoV(RelPosW,Rhat));
            if(angLoS_Gs < G->beamwidth_Gs){
                if(G->flagAccessStart[iRx] == FALSE && G->flagAccessEnd[iRx] == FALSE){
                    G->cntAccess[iRx]++;
                    G->t_acc_start[iRx] = UTC.JulDay;
                }
                G->flagAccessStart[iRx] = TRUE;
                G->flagAccessEnd[iRx] = FALSE;
                fprintf(outFile2[iRx], "%ld %ld %lf %lf %lf\n", iGs, G->cntAccess[iRx], Rx->lat*R2D, Rx->lon*R2D, Rx->alt);
            }
            else{
                if(G->flagAccessStart[iRx] == TRUE && G->flagAccessEnd[iRx] == FALSE){
                    G->t_acc_end[iRx] = UTC.JulDay;
                    flagWrite = TRUE;
                }
                G->flagAccessStart[iRx] = FALSE;
                G->flagAccessEnd[iRx] = FALSE;
            }
        }
        else
        {
            if(G->flagAccessStart[iRx] == TRUE && G->flagAccessEnd[iRx] == FALSE){
                G->t_acc_end[iRx] = UTC.JulDay;
                flagWrite = TRUE;
            }
            G->flagAccessStart[iRx] = FALSE;
            G->flagAccessEnd[iRx] = FALSE;
        }
        if(flagWrite){
            fprintf(outFile1[iRx], "%ld %ld %lf %lf\n", iGs, G->cntAccess[iRx], G->t_acc_start[iRx], G->t_acc_end[iRx]);
            flagWrite = FALSE;
        }
    }
    if(EndFlag){
        fclose(outFile1[iRx]);
        fclose(outFile2[iRx]);
    }
}

