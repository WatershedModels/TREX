/*---------------------------------------------------------------------
C-  Function:   ReadRestart.c
C-
C-  Purpose/    ReadRestart is called at the start of the simulation
C-  Methods:    to read the restart information file and read initial
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
C-  Calls:      Read2DIntegerGrid,
C-              Read3DIntegerGrid,
C-              Read2DFloatGrid,
C-              Read3DFloatGrid,
C-              Read3DDoubleGrid,
C-              ReadSedimentProperties,
C-              ReadWaterProperties,
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

void ReadRestart(int rstopt)
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
	printf(	   "*   Reading Restart Files  *\n");
	printf(	   "*                          *\n");
	printf(	   "****************************\n\n\n");

	//Allocate memory for restart info file name (files with names of restart files and headers)
	infofile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Allocate memory for restart file name
	restartfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Developer's Note:  The restart info file path and name is hardwired
	//                   to be "Restart/restart-info.txt".
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
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Restart Information File : %s \n",infofile);

		//Write message to screen
		printf("Error! Can't open Restart Information File : %s \n",infofile);
		exit(EXIT_FAILURE);	//abort
	}

	//Record 1
	fgets(header, MAXHEADERSIZE, infofile_fp);	//read header from restart info file

	/**********************************************/
	/*        Overland Soil Stack Variables       */
	/**********************************************/

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

	//read overland soil stack grid (nstackov[][])
	Read2DIntegerGrid(restartfile, nstackov);

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

        //read overland soil stack layer thickness grid
		Read3DFloatGrid(restartfile, hlayerov, ilayer);

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

		//read grid for current volume of stack layers
		Read3DDoubleGrid(restartfile, vlayerov, ilayer);

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

		//read grid for minimum volume of stack layers
		Read3DFloatGrid(restartfile, minvlayerov, ilayer);

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

		//read grid for maximum volume of stack layers
		Read3DFloatGrid(restartfile, maxvlayerov, ilayer);

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

		//read grid for current elevation of stack layers
		Read3DFloatGrid(restartfile, elevlayerov, ilayer);

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

		//Read grid for current soil types for this stack layers
		Read3DIntegerGrid(restartfile, soiltype, ilayer);

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

			//read grid for current solids concentration of stack layers
			Read3DFloatGrid(restartfile, csedov[isolid], ilayer);

		}	//end loop over solids

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

			//read grid for current chemical concentrations of stack layers
			Read3DFloatGrid(restartfile, cchemov[ichem], ilayer);

		}	//end loop over chemicals

	}	//end loop over soil stack layers

	/*************************************************/
	/*        Channel Sediment Stack Variables       */
	/*************************************************/

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

	//read channel sediment properties file for current conditions
	ReadSedimentProperties(restartfile);

	//Compute total solids concentrations in soil and sediment...
	//
	//Note:  The sum of all solids (TSS) is stored in the zeroeth element of
	//       the [isolids] dimension of the csedov and csedch arrays.
	//
	//       Only values for soil and sediment are computed here.  Values for
	//       the water column are computed below for rstopt = 2.
	//
	//loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//loop over number of columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (imask != nodatavalue)
			if(imask[i][j] != nodatavalue)
			{
				//loop over occupied layers
				for(k=1; k<=nstackov[i][j]; k++)
				{
					//initialize total solids concentration for this soil layer
					csedov[0][i][j][k] = 0.0;

					//loop over solids (don't include zero because it is computed inside the loop)
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//add present solids type to total concentration for this soil layer (g/m3)
						csedov[0][i][j][k] = csedov[0][i][j][k] + csedov[isolid][i][j][k];

					}	//end loop over solids

				}	//end loop over layers

			}	//end if imask != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//if channels are simulated
	if(chnopt == 1)
	{
		//loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//loop over number of nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//loop over occupied layers
				for(k=1; k<=nstackch[i][j]; k++)
				{
					//initialize total solids concentration for this sediment layer
					csedch[0][i][j][k] = 0.0;

					//loop over solids (don't include zero because it is computed inside the loop)
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//add present solids type to total concentration for this sediment layer (g/m3)
						csedch[0][i][j][k] = csedch[0][i][j][k] + csedch[isolid][i][j][k];

					}	//end loop over solids

				}	//end loop over layers

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt == 1

	//if the restart option = 2 (surface water variables are initialized)
	if(rstopt == 2)
	{
		/**************************************************/
		/*        Overland Surface Water Variables        */
		/**************************************************/

		//Record 24
		fgets(header, MAXHEADERSIZE, infofile_fp);	//read header from restart info file

		//Surface Water Depth...
		//
		//Record 25 (Part 1)
		fscanf(infofile_fp, "%s",		//read
			varname);					//dummy

		//Record 25 (Part 2)
		fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read overland water depth file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		restartfile = StripString(restartfile);

		//Record 26
		fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for water depth file

		//read overland water depth (hov[][])
		Read2DFloatGrid(restartfile, hov);

		//Surface Water Solids Concentration...
		//
		//loop over solids
		for(isolid=1;isolid<=nsolids;isolid++)
		{
			//Record 27 (Part 1)
			fscanf(infofile_fp, "%s",		//read
				varname);					//dummy

			//Record 27 (Part 2)
			fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read surface water solids concentration file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			restartfile = StripString(restartfile);

			//Record 28
			fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for solids concentration file

			//read grid for current solids concentration of surface water (ilayer = 0)
			Read3DFloatGrid(restartfile, csedov[isolid], 0);

		}	//end loop over solids

		//Surface Water Chemical Concentration...
		//
		//loop over chemicals
		for(ichem=1;ichem<=nchems;ichem++)
		{
			//Record 29 (Part 1)
			fscanf(infofile_fp, "%s",		//read
				varname);					//dummy

			//Record 29 (Part 2)
			fgets(restartfile, MAXNAMESIZE, infofile_fp);	//read surface water chemical concentration file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			restartfile = StripString(restartfile);

			//Record 30
			fgets(header, MAXHEADERSIZE, infofile_fp);	//read header for chemimical concentration file

			//read grid for current chemical concentrations of water column (ilayer = 0)
			Read3DFloatGrid(restartfile, cchemov[ichem], 0);

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

		//read channel surface water properties file for current conditions
		ReadWaterProperties(restartfile);

		//Compute total solids concentrations in surface water...
		//
		//Note:  The sum of all solids (TSS) is stored in the zeroeth element of
		//       the [isolids] dimension of the csedov and csedch arrays.
		//
		//       Only values for the water column are computed here (rstopt = 2).
		//
		//loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain (imask != nodatavalue)
				if(imask[i][j] != nodatavalue)
				{
					//initialize total solids concentration for water column (g/m3)
					csedov[0][i][j][0] = 0.0;

					//loop over solids (don't include zero because it is computed inside the loop)
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//add present solids type to total concentration for this cell (g/m3)
						csedov[0][i][j][0] = csedov[0][i][j][0] + csedov[isolid][i][j][0];

					}	//end loop over solids

				}	//end if imask != nodatavalue

			}	//end loop over columns

		}	//end loop over rows

		//if channels are simulated
		if(chnopt == 1)
		{
			//loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//initialize total solids concentration for water column (g/m3)
					csedch[0][i][j][0] = 0.0;

					//loop over solids (don't include zero because it is computed inside the loop)
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//add present solids type to total concentration for this node (g/m3)
						csedch[0][i][j][0] = csedch[0][i][j][0] + csedch[isolid][i][j][0];

					}	//end loop over solids

				}	//end loop over nodes

			}	//end loop over links

		}	//end if chnopt == 1

	}	//end if rstopt = 2

//End of function: Return to ReadInputFile
}


/*
write/read
	nstackov
	hlayerov
	vlayerov
	minvlayerov
	maxvlayerov
	elevlayerov
	csedov[isolid][i][j][ilayer]
	cchemov[ichem][i][j][ilayer]
*/
