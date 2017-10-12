/*----------------------------------------------------------------------
C-  Function:	Initialize.c
C-
C-	Purpose/	Initialize is called at the start of the simulation to
C-  Methods:    allocate memory and initialize values for variables used
C-              in computations but not read from input files.
C-
C-	Inputs:		None
C-
C-	Outputs:	ksim0
C-
C-	Controls:   ksim, dtopt
C-
C-	Calls:		InitializeWater, InitializeSolids, InitializeChemical
C-
C-	Called by:	trex
C-
C-	Created:	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-				
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		18-MAR-2004
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		31-JUL-2009
C-
C-  Revisions:	Added code so that errors caused by bad file names
C-              and paths are are reported to the simulation error
C-              file in addition to the screen.
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C---------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void Initialize()
{
	/***************************************************/
	/*        Initialize Simulation Error File         */
	/***************************************************/

	//Allocate memory for simulation error file name
	errorfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Set simulation error file name = inputfile name
	strcpy(errorfile, inputfile);

	//Add .err extension to simulation error file name
	errorfile = strcat(errorfile,".err");

	//Set the simulation error file pointer to null
	errorfile_fp = NULL;

	//Open simulation error file and write header line
	errorfile_fp = fopen(errorfile,"w");

	//if the simulation error file pointer is null, abort execution
	if(errorfile_fp==NULL)
	{
		printf("Can't create Simulation Error File:  %s\n",errorfile);	//Write message to screen
		exit(EXIT_FAILURE);	//abort

	}	//End if simulation error file is null

	//Echo simulation error file name to error file
	fprintf(errorfile_fp, "\n\nSimulation Error File:  %s\n", errorfile);

	//Close simulation error file
	fclose(errorfile_fp);

	//Open simulation echo file in append mode
	echofile_fp = fopen(echofile,"a");

	//Echo simulation error file name to echo file
	fprintf(echofile_fp, "\n\nSimulation Error File:  %s\n", errorfile);

	//Close echo file
	fclose(echofile_fp);

	/***********************************************************************/
	/*        Initialize Summary Statistics and Mass Balance Files         */
	/***********************************************************************/

	//The summary statistics file is opened here and then closed
	//as a check to make sure it can be created to minimize the
	//chance that the program aborts at the end of a simulation
	//when the statistics file is in used in the WriteSummary*
	//routines...
	//
	//set the summary statistics file pointer to null
	statsfile_fp = NULL;

	//open the summary statistics file
	statsfile_fp = fopen(statsfile, "w");

	//if file pointer is still null, then abort
	if(statsfile_fp == NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "\n\nError!  Can't open Summary Statistics file %s\n", statsfile);

		//Write message to screen
		printf("Error! Can't open Summary Statistics file %s\n", statsfile);
        exit(EXIT_FAILURE);	//exit with error
    }

	//Close the Summary Statistics File
	fclose(statsfile_fp);

	//if the mass balance file name is not null
	if(*msbfile != '\0')
	{
		//The mass balance file is opened here and then closed as a
		//check to make sure it can be created to minimize the chance
		//that the program aborts at the end of a simulation when the
		//mass balance file is in used in the WriteMassBalance routine.
		//
		//set the summary statistics file pointer to null
		msbfile_fp = NULL;

		//open the mass balance file
		msbfile_fp = fopen(msbfile, "w");

		//if file pointer is still null, then abort
		if(msbfile_fp == NULL)
		{
			//Write message to file
			fprintf(echofile_fp, "\n\nError!  Can't open Mass Balance file %s\n", msbfile);

			//Write message to screen
			printf("Error! Can't open Mass Balance file %s\n", msbfile);
			exit(EXIT_FAILURE);	//exit with error
		}

		//Close the Mass Balance File
		fclose(msbfile_fp);

	}	//end if msbfile is not blank

	//Initialize water transport variables
	InitializeWater();

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//Initialize sediment transport variables
		InitializeSolids();

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Initialize chemical transport variables
			InitializeChemical();

		}	//end if ksim > 2

	}	//end if ksim > 1

	//Initialize environmental condition variables
	InitializeEnvironment();

	//Initializations for automated time step option
	//
	//if automated time stepping is selected (if dtopt = 1 or 2)
	if(dtopt == 1 || dtopt == 2)
	{
		//store the initial value of ksim
		ksim0 = ksim;

		//reset the simulation type to hydrology only (ksim = 1)
		ksim = 1;

		//set the initial time step value
		dt[1] = dtmax;

		/***************************************************/
		/*        Initialize Time Step Buffer File         */
		/***************************************************/

		//Allocate memory for time step buffer file name
		dtbufferfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Set time step buffer file name = time step output file name
		strcpy(dtbufferfile, dtoutputfile);

		//Add .buf extension to time step buffer file name
		dtbufferfile = strcat(dtbufferfile,".buf");

		//Set the time step buffer file pointer to null
		dtbufferfile_fp = NULL;

		//Open the time step buffer file for writing
		dtbufferfile_fp=fopen(dtbufferfile,"w");

		//if the time step buffer file pointer is null, abort execution
		if(dtbufferfile_fp==NULL)
		{
			//Write message to file
			fprintf(echofile_fp, "Error! Can't open Temporary Time Step Buffer File (dtbufferfile) : %s \n",dtbufferfile);

			//Write message to screen
			printf("Error! Can't open Temporary Time Step Buffer File (dtbufferfile) : %s \n",dtbufferfile);
			exit(EXIT_FAILURE);	//abort
		}

		//close temporary time step buffer file
		fclose(dtbufferfile_fp);

		/***************************************************/
		/*        Initialize Time Step Output File         */
		/***************************************************/

		//Set the time step output file pointer to null
		dtoutputfile_fp = NULL;

		//Open the time step output file for writing
		dtoutputfile_fp=fopen(dtoutputfile,"w");

		//if the time step output file pointer is null, abort execution
		if(dtoutputfile_fp==NULL)
		{
			//Write message to file
			fprintf(echofile_fp, "Error! Can't open External Time Step Output File (dtoutputfile) : %s \n",dtoutputfile);

			//Write message to screen
			printf("Error! Can't open External Time Step Output File (dtoutputfile) : %s \n",dtoutputfile);
			exit(EXIT_FAILURE);	//abort
		}

	}	//end if dtopt = 1 or 2

//End of function: Return to trex_main
}
