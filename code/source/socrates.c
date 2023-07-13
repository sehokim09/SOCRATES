/***************************************************************************/
/*                                                                         */
/*                                SOCRATES                                 */
/*                                                                         */
/*  SoOp Constellation and Remote sensing Analysis Tool for Earth Science  */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/*   Written by Seho Kim @ Purdue University                               */
/*                                                                         */
/***************************************************************************/
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

#define DECLARE_GLOBALS
#include "socrates.h"
#undef DECLARE_GLOBALS
#include "42.h"
#include <stdio.h>

int main(int argc,char **argv)
{
	int provided, taskid;

	MPI_Init_thread(&argc,&argv, MPI_THREAD_SINGLE, &provided);
//	MPI_Init_thread(&argc,&argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

	// .. Initialization .. //
	InitSOCRATES(argc, argv);
	if(taskid == MASTER)
		Welcome();

	// .. Observation Mode .. //
	switch(SimMode){
		case FIXED:
			ObsFixed();
			break;
		case INORBIT:
			InitReport();
			if(ObsBase == SPACE){
				InitSubsys();
				while(!EndFlag){
					// .. Orbit Propagation .. //
					Propagator();
					// .. Observation .. //
					if(OutFlag){
						ObsSpace();
					}
				}
			}
			else if(ObsBase == TOWER){
				while(!EndFlag){
					// .. Orbit Propagation .. //
					Propagator();
					// .. Observation .. //
					if(OutFlag)
						ObsTower();
				}
			}
			break;
		case DESIGN:
			//orbitPropagator(); // Uncomment this to generate Tx trajectory files
			ConsDesign(argc, argv);
			break;
		case RETRIEVAL:
			/* .. Run forward/inverse model .. */
			initRetMode(argc, argv);
			switch(RetMode){
				case FORWARD:
					ObsFixedNetCDF();
					break;
				case INVERSE:
					RetrievalNetCDF();
					break;
				default:
					break;
			}
			break;
		default:
			break;		
	}

	/* .. Finalize Simulator .. */
	if(taskid == MASTER)
		printf(">> Simulation End.\n");

	/* .. Finalize MPI .. */
	MPI_Finalize();

	return 0;
}

