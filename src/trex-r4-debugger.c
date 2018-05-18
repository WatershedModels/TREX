/*----------------------------------------------------------------------
C-  Function:   trex.c (main module)
C-
C-  Purpose/    trex is the main module of the program.  It
C-  Methods:    obtains the input file name from the initial
C-              argument (argv), calls ReadInputFile to read
C-              all program inputs, calls Initialize to set
C-              initial values for variables.  Euler's method
C-              is used to generate solutions over space and
C-              time.  During integration, derivative terms
C-              are computed for transport processes for the
C-              current time step, output written, and mass
C-              balances computed to get water depths and the
C-              concentrations of solids and chemicals for
C-              the next time step.
C-
C-  Inputs:     argv
C-
C-  Outputs:    argc
C-
C-  Controls:   ksim, rstopt, relaunch, simtime, tend, dtopt,
C-              timeprintout, timeprintgrid
C-
C-  Calls:      ReadInputFile, Initialize, ReadRestart,
C-              TimeFunctionInit, ComputeInitialState,
C-              UpdateTimeFunction, UpdateEnvironment,
C-              WaterTransport, SolidsTransport,
C-              ChemicalTransport, WaterBalance,
C-              SolidsBalance, ChemicalBalance,
C-              WriteTimeSeries, WriteDumpFile,
C-              WriteGrids, NewState, Reinitialize,
C-              WriteEndGrids, ComputeFinalState,
C-              WriteMassBalance, WriteSummary,
C-              FreeMemory, RunTime, WriteRestart
C-
C-  Called by:  None
C-
C-  Created:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-  Date:       17-MAR-2004
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       03-JUN-2008
C-
C-  Revisions:  Added simulation restart options
C-
C-  Revised:    Mark Velleux
C-              Masa Takamatsu
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       29-JUL-2010
C-
C-  Revisions:  Added controls for time step option (dtopt)
C-              and simulation relaunch.
C-
C-  Revised:
C-
C-  Date:
C-
C-  Revisions:
C-
C---------------------------------------------------------------------*/
//trex global general variable declarations/definitions
#include "trex_general_declarations.h"
#include "trex_general_definitions.h"

//trex global variable declarations/definitions for water transport
#include "trex_water_declarations.h"
#include "trex_water_definitions.h"

//trex global variable declarations/definitions for sediment transport
#include "trex_solids_declarations.h"
#include "trex_solids_definitions.h"

//trex global variable declarations/definitions for chemical transport
#include "trex_chemical_declarations.h"
#include "trex_chemical_definitions.h"

//trex global variable declarations/definitions for environmental conditions
#include "trex_environmental_declarations.h"
#include "trex_environmental_definitions.h"

int main(int argc, char *argv[])
{
	//Local variable declarations/definitions
	char
		*prog;				//name of program (defined from argv[0])

	double
		timeprintout,		//tabular output print time
		timeprintgrid;		//grid output print time

	int
		rstopt,				//restart option for surface water initializations (1 = do not read, 2 = read) 
		relaunch;			//simulation relaunch control (0 = do not relaunch, 1 = relaunch)

	time_t clockstart;
	time_t clockstop;

	//set the floating point exception masks
//mlv	_control87( 0, _MCW_EM);
//mlv	_control87( _EM_INVALID + _EM_DENORMAL + _EM_ZERODIVIDE + _EM_OVERFLOW + _EM_UNDERFLOW, _MCW_EM);
	_control87( _EM_DENORMAL + _EM_UNDERFLOW + _EM_INEXACT, _MCW_EM);
//mlv	_control87( _EM_INEXACT, _MCW_EM);
//mlv	_control87( _EM_ZERODIVIDE, _MCW_EM);
//mlv	_control87( _EM_DENORMAL, _MCW_EM);
//mlv	_control87( _EM_INVALID, _MCW_EM);
//mlv	_control87( _EM_OVERFLOW, _MCW_EM);
//mlv	_control87( _EM_UNDERFLOW, _MCW_EM);
//mlv	_control87( _DN_FLUSH, _MCW_DN);

	//CPU clock time (in seconds since epoch) at start of simulation
	time(&clockstart);

	//Assign program name to define stderr file for error reporting
	prog = argv[0];

	//Assign program name to define stderr file for error reporting
	strcpy(prog, argv[0]);

	//Allocate memory for input file name
	inputfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Assign inputfile name from argv initial argument
	strcpy(inputfile,argv[1]);

	/*********************************************/
	/*         Read and initialize data          */
	/*********************************************/

	//Read input file
	ReadInputFile();

	//Initialize Variables
	Initialize();

	//Note:  Three restart options are available: (1) restart0; (2) restart1; and
	//       (3) restart2.  For restart0, no initializations are read at the start
	//       of the simulation (restart information is written at the end).  For
	//       restart1, the simulation is initialized with conditions for soils and
	//       sediments but not the water column. This option assumes that the water
	//       column starts with baseflow conditions for water depth in channels, zero
	//       water depth on the overland plane, and that initial solids and chemical
	//       concentrations in surface water are zero.  For restart2, the simulation
	//       also includes all the initializations for restart1 plus initializations
	//       for water depth and solids and chemicals in surface water (channels and
	//       overland plane).  Neither restart option initializes the wetting front
	//       or other properties for infiltration or transmission loss or rainfall
	//       interception depth, etc.  All three restart options (0, 1, and 2) write
	//       final conditions at the end of the simulation.
	//
	//       If the count of initial arguments (argc) > 2, the user has specified a
	//       a restart option.  If the specified restart option is 1 or 2, then
	//       restart information is read...
	//
	//if the count of initial arguments > 2 (if a restart option is specified
	if(argc > 2)
	{
		//if the restart argument = restart2 (strcmp returns zero when the strings are identical)
		if(strcmp(argv[2],"restart2") == 0)
		{
			//set the restart option to 2 (read restart info with surface water initializations)
			rstopt = 2;
		}
		//if the restart argument = restart1 (strcmp returns zero when the strings are identical)
		else if(strcmp(argv[2],"restart1") == 0)
		{
			//set the restart option to 1 (read restart info without surface water initializations)
			rstopt = 1;
		}
		//if the restart option is restart0 (strcmp returns zero when the strings are identical)
		else if(strcmp(argv[2],"restart0") == 0)
		{
			//set the restart option to 0 (do not read restart info)
			rstopt = 0;
		}
		else	//else the argument is not defined
		{
			//set the restart option to -1 (restart option not defined, do not read or write restart info)
			rstopt = -1;

		}	//end if the restart argument = restart2, restart1, restart0...
	}
	else	//else, no restart option specified by user
	{
		//set the restart option to -1 (no restart option specified)
		rstopt = -1;

	}	//end if argc > 2

	//if the rstopt > 0 (if restart info and initializations are read)
	if(rstopt > 0)
	{
		//Read initial condition (restart) files for storms in sequence according to restart option
		ReadRestart(rstopt);

	}	//end if rstopt > 0

	/*******************************************/
	/*        Simulation Relaunch Loop         */
	/*******************************************/

	//set simulation relauch condition to on (relaunch = 1)
	relaunch = 1;

	//while the simulation is set to relaunch itself
	while(relaunch == 1)
	{
		//Set simulation start time: tstart specified in Data Group A
		simtime = tstart;	//simulation time (hours)

		//Set simulation end time: end time = time of last break in dt time series
		tend = dttime[ndt];		//simulation end time (hours)

		//Determine starting index of all time functions for any simulation start time
		TimeFunctionInit();

		//set time series and grid print output start times to simulation start time
		timeprintout = simtime;
		timeprintgrid = simtime;

		//Compute initial volumes and masses for overland plane and channels
		ComputeInitialState();

		/*********************************************/
		/*        Numerical Integration Loop         */
		/*********************************************/

		//write simulation start message to screen
		printf("\n\n*********************************\n");
		printf(	   "*                               *\n");
		printf(	   "*   Beginning TREX Simulation   *\n");
		printf(	   "*                               *\n");
		printf(	   "*********************************\n\n\n");

		//Main loop over time...
		//
		//while the current simulation time <= the simulation end time
		while(simtime <= tend)
		{
			//if time steps are specified (dtopt = 0 or dtopt = 3)
			if(dtopt == 0 || dtopt == 3)
			{
				//if it is time to use a new dt, increment the time step index (idt)
				if((simtime > dttime[idt]) && (idt < ndt))
				{
					//increment time step index
					idt = idt + 1;	//increment time step index
				}
			}
			else if(dtopt == 1 || dtopt == 2)	//else if automated time stepping is selected
			{
				//Note:  A new time step is added to the series of dt, dttime
				//       values whenever the number of stored time step values
				//       is equal to the number of values in the series.  This
				//       occurs when idt = bdt.
				//
				//if a new time step value is needed (if idt = bdt)
				if(idt == bdt)
				{
					//increment time step index
					idt = idt + 1;

				}	//end if dt[idt] not equal to dtmax

				//set time step for this time level to the maximum value (seconds)
				dt[idt] = dtmax;

			}	//end if dtopt = 1 or 2

			//Update time functions
			UpdateTimeFunction();

			//Update environmental conditions
			UpdateEnvironment();

			//Compute water transport/forcing functions (rainfall, infiltration, flows)
			WaterTransport();

			//Compute water mass balance (obtain flow depths and floodplain transfers)
			WaterBalance();

			//if sediment transport is simulated
			if(ksim > 1)
			{
				//Compute sediment transport/forcing functions (transport, erosion, deposition, loads)
				SolidsTransport();

				//Compute solids mass balance (obtain particle concentrations)
				SolidsBalance();

				//if chemical transport is simulated
				if(ksim > 2)
				{
					//Compute chemical transport/forcing functions (mass transfer and transformations etc.)
					ChemicalTransport();

					//Compute chemical mass balance (obtain chemical concentrations)
					ChemicalBalance();

				}	//end if ksim > 2

			}	//end if ksim > 1

			//if simulation time >= time to report time series output
			if(simtime >= timeprintout)
			{
				//Write simtime and time series printout status to screen

				//jfe need to use stderr here as in K&R p. 163 to insure msg goes to screen....
				//jfe????? or should we allow user to redirect all to file??????? if so, which one???

				fprintf(stderr, "  Time Series printout time = %9.6f\tSimulation Time (hours) = %9.6f \n"
							, timeprintout,simtime);
//jfe				fprintf("  Time Series printout time = %f\tSimulation Time (hours) = %f \n"
//jfe							,timeprintout,simtime);

				//Write times series output to text files
				WriteTimeSeries();

				//if the dump file name is not blank
				if(*dmpfile != '\0')
				{
					//Write detailed model results to dump file (*.dmp)
					WriteDumpFile();

				}	//end if dmpfile is not blank

				//if it is time to use a new output print interval
				if((timeprintout >= printouttime[pdt]) && (pdt < nprintout))
				{
					//increment the printout index (pdt)
					pdt = pdt + 1;
				}

				//Set the new output print time
				timeprintout = timeprintout + printout[pdt];

			}	//end if simtime >= timeprintout

			//if simulation time >= time to report grid output
			if(simtime >= timeprintgrid)
			{
				//Write simtime and grid printout status to screen via stderr

//mlv				fprintf(stderr, "  Grid printout time = %9.6f\tSimulation Time (hours) = %9.6f \n"
//mlv							,prog, timeprintgrid,simtime);
//jfe				printf(stderr, "  Grid printout time = %f\tSimulation Time (hours) = %f \n"
//jfe							,timeprintgrid,simtime);

				//Write output grids to files
				WriteGrids(gridcount);

				//increment sequential count of grids print events for file extension
				gridcount = gridcount + 1;

				//if it is time to use a new grid print interval
				if((timeprintgrid >= printgridtime[gdt]) && (gdt < nprintgrid))
				{
					//increment the printgrid index (gdt)
					gdt = gdt + 1;
				}

				//Set the new grid print time
				timeprintgrid = timeprintgrid + printgrid[pdt];

			}	//end if simtime >= timeprintgrid

			//Assign new state (flow depths, concentrations) for next time step
			NewState();

			//Increment simulation time (t = t + dt) (for upcoming iteration)
			simtime = simtime + dt[idt] / 3600.0; //simtime (hours), dt (seconds)

		}	// End Numerical Integration Loop

		//if automated time-stepping is selected (dtopt = 1 or dtopt = 2)
		if(dtopt == 1 || dtopt == 2)
		{
			//store simulation time for last time step time (dttime)
			dttime[idt-1] = (float)(tend);

			//increment total number of time steps in series
			ndt = ndt + 1;

			//increment timestep buffer index
			bdt = bdt + 1;

			//Note:  If ndt <= MAXBUFFERSIZE at the end of a simulation,
			//       no dt and dttime pairs were written to file and all
			//       are in memory.  Otherwise, if ndt > MAXBUFFERSIZE at
			//       the simulation end, the buffer must be flushed.
			//
			//       For simplicity, all dt and dttime values are always
			//       written to the buffer file to simply how values are
			//       retrieved and used to repopulate the dt and dttime
			//       arrays for final storage to external output file.
			//
			//Flush the buffer to write any remaing timestep pairs to
			//file...
			//
			//write time step series (dt, dttime pairs) to buffer file
			WriteDTBuffer();

			//write time step series (dt, dttime pairs) to external output file
			WriteDTFile();

		}	//end if dtopt = 1 or 2

		//Check for simulation relaunch conditions...
		//
		//Note:  Simulation relaunch occurs if dtopt = 2 and the initial
		//       ksim value is greater than 1 (ksim0 > 1).
		//
		//if dtopt = 2 and ksim0 > 1 (if automated time-stepping with relaunch is selected)
		if(dtopt == 2 && ksim0 > 1)
		{
			//reset the timestep option to read timesteps from file
			dtopt = 3;

			//Reinitialize simulation for relaunch...
			//
			//Note:  Mass balance and other variables must be are reset
			//       to initial values for simulation relaunch...
			//
			//Reinitialize
			Reinitialize();
		}
		else	//else dtopt is not 2
		{
			//set simulation relauch condition to off (relaunch = 0)
			relaunch = 0;

		}	//end if dtopt = 2

	}	//end loop while relaunch = 1

	/*******************************************/
	/* Compute final volumes & write results   */
	/*******************************************/

	//Write times series output to text files (ensure output of final iteration)
	WriteTimeSeries();

	//if the dump file name is not blank
	if(*dmpfile != '\0')
	{
		//Write detailed model results to dump file (*.dmp) (ensure output of final iteration)
		WriteDumpFile();

	}	//end if dmpfile is not blank

	//Write output grids to files (ensure output of final iteration)
	WriteGrids(gridcount);

	//Write end of run (single) grids to files (net elevation change, gross erosion, etc.)
	WriteEndGrids(gridcount);

	//Compute final volumes and masses for the overland and channels
	ComputeFinalState();

	//if the mass balance file name is not null
	if(*msbfile != '\0')
	{
		//Write summary mass balance file
		WriteMassBalance();

	}	//end if msbfile is not blank

	//Write summary file
	WriteSummary();

	//Developer's Note:  The memory deallocation routines are "stale"
	//                   and cause segmentation faults when executed.
	//                   For now, the call to FreeMemory is commented
	//                   out.  However, the set of FreeMemory modules
	//                   are still part of the active set of modules
	//                   for the TREX code and are subject to further
	//                   development.  We'll fix this at some point...
	//
	//Deallocate memory
//mlv	FreeMemory();

	//CPU clock time (in seconds since epoch) at end of simulation
	time(&clockstop);

	//Compute trex elapsed running time for this simulation
	RunTime(clockstart, clockstop);

	//if the restart option is >= 0 (if final conditions for a restart are written)
	if(rstopt >= 0)
	{
		//Write initial condition (restart) files for storms in sequence accoprding to restart option
		WriteRestart();

	}	//end if rstopt >= 0

	//End of Main Program
	return 0;
}
