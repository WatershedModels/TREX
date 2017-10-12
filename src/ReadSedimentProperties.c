/*---------------------------------------------------------------------
C-  Function:   ReadSedimentProperties.c
C-
C-  Purpose/    ReadSedimentProperties is called at the start of the
C-  Methods:    simulation to read initial condition channel properties
C-              needed to restart simulations for storms in sequence.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadRestart
C-
C-  Created:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       03-JUN-2008
C-
C-  Revised:    
C-
C-  Date:       
C-
C-  Revisions:  
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

void ReadSedimentProperties(char name[])
{
	//local variable declarations/definitions
	int
		isolid,			//loop index for solids type
		ichem;			//loop index for chemical type

	char
		*gridfile;	//file name for grid including extension

	//Declare local grid file pointer;
	FILE *gridfile_fp;

	//Allocate memory for simulation error file name
	gridfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Print restart file name file and extension to local string gridfile
	sprintf(gridfile,"%s",name);

	//Set the local grid file pointer to null
	gridfile_fp = NULL;

	//open the grid file for reading
	gridfile_fp = fopen(gridfile, "r");

	//if the local grid file pointer is null, abort execution
	if(gridfile_fp == NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't create local Grid Output File : %s \n",gridfile);

		//Write message to screen
		printf("Error! Can't create local Grid Output File : %s \n",gridfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Record 1
	fgets(header, MAXHEADERSIZE, gridfile_fp);	//read header

	//loop over links
	for(i=1;i<=nlinks;i++)
	{
		//Record 2
		fscanf(gridfile_fp, "%s %s  %s %s",		//read
			varname,							//dummy
			varname,							//link number (dummy)
			varname,							//dummy
			varname);							//number of nodes (dummy)

		//loop over nodes
		for(j=1;j<=nnodes[i];j++)
		{
			//Record 3
			fscanf(gridfile_fp, "%s %s %s %d %s %f %s %f %s %f %s %f",	//read
				varname,						//dummy
				varname,						//node number (dummy)
				varname,						//dummy
				&nstackch[i][j],				//number of layers in sediment stack
				varname,						//dummy
				&hbank[i][j],					//channel bank height (m)
				varname,						//dummy
				&bwidth[i][j],					//channel bottom width (m)
				varname,						//dummy
				&sideslope[i][j],				//channel side slope (dimensionless)
				varname,						//dummy
				&elevationch[i][j]);			//channel bed elevation (m)

			//loop over maximum number of layers in soil stack (reverse order)
			for(k=maxstackov;k>=1;k--)
			{
				//Note:  use %lf conversion for a double (long float)
				//
				//Record 4
				fscanf(gridfile_fp, "%s %s %s %f %s %f %s %f %s %lf %s %f %s %f %s %f %s %f",	//read
					varname,						//dummy
					varname,						//layer number (dummy)
					varname,						//dummy
					&hlayerch[i][j][k],				//layer thickness (m)
					varname,						//dummy
					&bwlayerch[i][j][k],			//layer bottom width (m)
					varname,						//dummy
					&achbed[i][j][k],				//layer surface area (m2)
					varname,						//dummy
					&vlayerch[i][j][k],				//layer volume (m3)
					varname,						//dummy
					&minvlayerch[i][j][k],			//layer minimum volume (m3)
					varname,						//dummy
					&maxvlayerch[i][j][k],			//layer maximum volume (m3)
					varname,						//dummy
					&elevlayerch[i][j][k],			//layer elevation (m)
					varname,						//dummy
					&porositych[i][j][k]);			//layer porosity (dimensionless)

				//Record 5 (Part 1)
				fscanf(gridfile_fp, "%s", varname);		//dummy

				//loop over solids
				for(isolid=1;isolid<=nsolids;isolid++)
				{
					//Record 5 (Part 2)
					fscanf(gridfile_fp, "%f", &csedch[isolid][i][j][k]);

				}	//end loop over solids

				//Record 6 (Part 1)
				fscanf(gridfile_fp, "%s", varname);

				//loop over chemicals
				for(ichem=1;ichem<=nchems;ichem++)
				{
					//Record 6 (Part 2)
					fscanf(gridfile_fp, "%f", &cchemch[ichem][i][j][k]);

				}	//end loop over chemicals

			}	//end loop over layers

		}	//end loop over nodes

	}	//end loop over links

	//Close local file pointer
	fclose(gridfile_fp);

	//deallocate memory for local variables
	free(gridfile);

//End of function: Return to ReadRestart
}
