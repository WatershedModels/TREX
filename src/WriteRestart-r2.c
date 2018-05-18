/*---------------------------------------------------------------------
C-  Function:   WriteRestart.c
C-
C-  Purpose/    WriteRestart is called at the end of the simulation
C-  Methods:    to read the restart information file and write initial
C-              condition files needed to restart simulations for
C-              storms in sequence.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Calls:      Write2DIntegerGrid,
C-              Write3DIntegerGrid,
C-              Write2DFloatGrid,
C-              Write3DFloatGrid,
C-              Write3DDoubleGrid,
C-              WriteSedimentProperties,
C-              WriteWaterProperties,
C-              StripString
C-
C-  Called by:  trex
C-
C-  Created:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       03-JUN-2008
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
C-  Revised:	Mark Velleux
C-              HDR, Inc.
C-              1 International Boulevard, 10th Floor, Suite 1000
C-              Mahwah, NJ  07495
C-
C-  Date:		30-MAY-2017
C-
C-  Revisions:	Added code so information read from restart-info.txt
C               file conforms to the simulation type defined by ksim.
C               When ksim < 3, chemical transport information is not
C               written.
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void WriteRestart()
{
	//local variable declarations/definitions
	int
		isolid,			//loop index for solids type
		ichem,			//loop index for chemical type
		ilayer;			//layer in the soil/sediment stack

	char
		*infofile,		//file name for restart information file
		*restartfile;	//generic file name for restart files

	//Declare local info file pointer;
	FILE *infofile_fp;

	//write message to screen
	printf("\n\n****************************\n");
	printf(	   "*                          *\n");
	printf(	   "*   Writing Restart Files  *\n");
	printf(	   "*                          *\n");
	printf(	   "****************************\n\n\n");

	//Allocate memory for restart info file name (files with names of restart files and headers)
	infofile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Allocate memory for restart file name
	restartfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Developer's Note:  The name of the restart info file is hardwired
	//                   to be "restart-info.txt".
	//
	//Assign the restart info file name
	sprintf(infofile,"%s","Restart/restart-info.txt");

	//Set the local restart info file pointer to null
	infofile_fp = NULL;

	//open the restart info file for reading
	infofile_fp = fopen(infofile, "r");

	//if the local info file pointer is null, abort execution
	if(infofile_fp == NULL)
	{
		//write error message to file
		fprintf(echofile_fp, "Error! Can't open Restart Information File : %s \n",infofile);

		//Write message to screen
		printf("Error! Can't open Restart Information File : %s \n",infofile);
		exit(EXIT_FAILURE);	//abort
	}

	//if sediment transport is simulated
	if(ksim > 1)
	{

		//Record 1
		fgets(header, MAXHEADERSIZE, infofile_fp);	//read header from restart info file

		/**********************************************/
		/*        Overland Soil Stack Variables       */
		/**********************************************/

		//Soil stack layers (nstackov)...
		//
		//Record 2
		fgets(header, MAXHEADERSIZE, infofile_fp);	//read header from restart info file

		//Record 3 (Part 1)
		fscanf(infofile_fp, "%s",		//read
			varname);					//dummy

		//Record 3 (Part 2)
		fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read soil stack file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		restartfile = StripString(restartfile);

		//Record 4
		fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for soil stack file

		//write grid for current number of soil stack elements
		Write2DIntegerGrid(restartfile, nstackov);

		//loop over maximum number of layers in soil stack (reverse order)
		for(ilayer=maxstackov;ilayer>=1;ilayer--)
		{
			//Soil Layer Thickness...
			//
			//Record 5 (Part 1)
			fscanf(infofile_fp, "%s",		//read
				varname);					//dummy

			//Record 5 (Part 2)
			fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read soil layer thickness file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			restartfile = StripString(restartfile);

			//Record 6
			fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for soil layer thickness file

			//write grid for current thickness of stack layers
			Write3DFloatGrid(restartfile, hlayerov, ilayer);

			//Soil Layer Volume...
			//
			//Record 7 (Part 1)
			fscanf(infofile_fp, "%s",		//read
				varname);					//dummy

			//Record 7 (Part 2)
			fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read soil layer volume file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			restartfile = StripString(restartfile);

			//Record 8
			fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for soil layer volume file

			//write grid for current volume of stack layers
			Write3DDoubleGrid(restartfile, vlayerov, ilayer);

			//Soil Layer Minimum Volume...
			//
			//Record 9 (Part 1)
			fscanf(infofile_fp, "%s",		//read
				varname);					//dummy

			//Record 9 (Part 2)
			fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read soil layer minimum volume file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			restartfile = StripString(restartfile);

			//Record 10
			fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for soil layer minimum volume file

			//write grid for minimum volume of stack layers
			Write3DFloatGrid(restartfile, minvlayerov, ilayer);

			//Soil Layer Maximum Volume...
			//
			//Record 11 (Part 1)
			fscanf(infofile_fp, "%s",		//read
				varname);					//dummy

			//Record 11 (Part 2)
			fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read soil layer maximum volume file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			restartfile = StripString(restartfile);

			//Record 12
			fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for soil layer maximum volume file

			//write grid for maximum volume of stack layers
			Write3DFloatGrid(restartfile, maxvlayerov, ilayer);

			//Soil Layer Elevation...
			//
			//Record 13 (Part 1)
			fscanf(infofile_fp, "%s",		//read
				varname);					//dummy

			//Record 13 (Part 2)
			fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read soil layer elevation file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			restartfile = StripString(restartfile);

			//Record 14
			fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for soil layer elevation file

			//write grid for current elevation of stack layers
			Write3DFloatGrid(restartfile, elevlayerov, ilayer);

			//Soil Layer Soil Types...
			//
			//Record 15 (Part 1)
			fscanf(infofile_fp, "%s",		//read
				varname);					//dummy

			//Record 15 (Part 2)
			fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read soil layer soil types file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			restartfile = StripString(restartfile);

			//Record 16
			fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for soil layer soil types file

			//write grid for current soil types for this stack layers
			Write3DIntegerGrid(restartfile, soiltype, ilayer);

			//Soil Layer Solids Concentration...
			//
			//loop over solids
			for(isolid=1;isolid<=nsolids;isolid++)
			{
				//Record 17 (Part 1)
				fscanf(infofile_fp, "%s",		//read
				varname);					//dummy

				//Record 17 (Part 2)
				fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read soil layer solids concentration file name

				//strip leading blanks, trailing blanks, and final carriage return from string
				restartfile = StripString(restartfile);

				//Record 18
				fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for solids concentration file

				//write grid for current solids concentration of stack layers
				Write3DFloatGrid(restartfile, csedov[isolid], ilayer);

			}	//end loop over solids

			//if chemical transport is simulated
			if(ksim > 2)
			{
				//Soil Layer Chemical Concentration...
				//
				//loop over chemicals
				for(ichem=1;ichem<=nchems;ichem++)
				{
					//Record 19 (Part 1)
					fscanf(infofile_fp, "%s",		//read
						varname);					//dummy

					//Record 19 (Part 2)
					fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read soil chemical concentration file name

					//strip leading blanks, trailing blanks, and final carriage return from string
					restartfile = StripString(restartfile);

					//Record 20
					fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for chemimical concentration file

					//write grid for current chemical concentrations of stack layers
					Write3DFloatGrid(restartfile, cchemov[ichem], ilayer);

				}	//end loop over chemicals

			}	//end if ksim > 2

		}	//end loop over soil stack layers

	}	//end if ksim > 1

	/**********************************************************************/
	/*        Channel Channel Geometry and Sediment Stack Variables       */
	/**********************************************************************/

	//Record 21
	fgets(header, MAXHEADERSIZE, infofile_fp);	//read header from restart info file

	//Record 22 (Part 1)
	fscanf(infofile_fp, "%s",		//read
		varname);					//dummy

	//Record 22 (Part 2)
	fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read sediment stack file name

	//strip leading blanks, trailing blanks, and final carriage return from string
	restartfile = StripString(restartfile);

	//Record 23
	fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for sediment stack file

	//write channel sediment properties file for current conditions
	WriteSedimentProperties(restartfile);

	/************************************************/
	/*        Overland Water Column Variables       */
	/************************************************/

	//Record 24
	fgets(header, MAXHEADERSIZE, infofile_fp);	//read header from restart info file

	//set the layer number to zero (surface water is layer zero)
	ilayer = 0;

	//Surface Water Depth...
	//
	//Record 25 (Part 1)
	fscanf(infofile_fp, "%s",		//read
		varname);					//dummy

	//Record 25 (Part 2)
	fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read surface water depth file name

	//strip leading blanks, trailing blanks, and final carriage return from string
	restartfile = StripString(restartfile);

	//Record 26
	fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for surface water depth file

	//write grid for current overland surface water depth
	Write2DFloatGrid(restartfile, hov);

	//Water Column (Runoff) Solids Concentration...
	//
	//loop over solids
	for(isolid=1;isolid<=nsolids;isolid++)
	{
		//Record 27 (Part 1)
		fscanf(infofile_fp, "%s",		//read
			varname);					//dummy

		//Record 27 (Part 2)
		fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read surface water concentration file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		restartfile = StripString(restartfile);

		//Record 28
		fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for surface water concentration file

		//write grid for current solids concentrations in overland surface water
		Write3DFloatGrid(restartfile, csedov[isolid], ilayer);

	}	//end loop over solids

	//Water Column (Runoff) Chemical Concentration...
	//
	//loop over chemicals
	for(ichem=1;ichem<=nchems;ichem++)
	{
		//Record 29 (Part 1)
		fscanf(infofile_fp, "%s",		//read
			varname);					//dummy

		//Record 29 (Part 2)
		fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read surface water concentration file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		restartfile = StripString(restartfile);

		//Record 30
		fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for surface water concentration file

		//write grid for current chemical concentrations in overland surface water
		Write3DFloatGrid(restartfile, cchemov[ichem], ilayer);

	}	//end loop over chemicals

	/*************************************************/
	/*        Channel Surface Water Variables        */
	/*************************************************/

	//Record 31
	fgets(header, MAXHEADERSIZE, infofile_fp);	//read header from restart info file

	//Record 32 (Part 1)
	fscanf(infofile_fp, "%s",		//read
		varname);					//dummy

	//Record 32 (Part 2)
	fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read channel surface water file name

	//strip leading blanks, trailing blanks, and final carriage return from string
	restartfile = StripString(restartfile);

	//Record 33
	fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for channel surface water file

	//write channel surface water properties file for current conditions
	WriteWaterProperties(restartfile);

//End of function: Return to ReadInputFile
}

/*
Overland restart: write/read

	interception depth/volume
	depth to infiltration wetting front?

	nstackov
	hlayerov
	vlayerov
	minvlayerov
	maxvlayerov
	elevlayerov
	csedov[isolid][i][j][ilayer]
	cchemov[ichem][i][j][ilayer]

	cumulative "mass" variables...
	peak and time to peak

	min/max
	
*/

/*
Channel restart: write/read

	depth to transmission loss wetting front?

	nstackch
	hlayerch
	blayerch
	vlayerch
	minvlayerch
	maxvlayerch
	elevlayerch
	csedch[isolid][i][j][ilayer]
	cchemch[ichem][i][j][ilayer]
*/
