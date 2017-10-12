/*---------------------------------------------------------------------
C-  Function:   WriteSedimentProperties.c
C-
C-  Purpose/    WriteSedimentProperties is called at the end of the
C-  Methods:    simulation to write initial condition channel properties
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
C-  Called by:  WriteRestart
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

void WriteSedimentProperties(char name[])
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

	//open the grid file for writing
	gridfile_fp = fopen(gridfile, "w");

	//if the local grid file pointer is null, abort execution
	if(gridfile_fp == NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't create local Grid Output File : %s \n",gridfile);

		//Write message to screen
		printf("Error! Can't create local Grid Output File : %s \n",gridfile);
		exit(EXIT_FAILURE);	//abort
	}

	//print header text
	fprintf(gridfile_fp, "%s", header);

	//loop over links
	for(i=1;i<=nlinks;i++)
	{
		//print link info to file
		fprintf(gridfile_fp, "%s %d  %s %d\n",
			"Link", i, "NumNodes", nnodes[i]);

		//loop over nodes
		for(j=1;j<=nnodes[i];j++)
		{
			//print node sediment stack and channel geometry info to file
			fprintf(gridfile_fp, "%s %d  %s %d  %s %f  %s %f  %s %f  %s %f\n",
				"  Node", j,
				"NSTACKCH", nstackch[i][j],
				"Hbank", hbank[i][j],
				"Bwidth", bwidth[i][j],
				"SideSlope", sideslope[i][j],
				"ElevationCh", elevationch[i][j]);

			//loop over maximum number of layers in soil stack (reverse order)
			for(k=maxstackov;k>=1;k--)
			{
				//Note:  use %lf conversion for a double (long float)
				//
				//print node sediment stack state and bed geometry info to file
				fprintf(gridfile_fp, "%s %d  %s %f  %s %f  %s %f  %s %lf  %s %f  %s %f  %s %f  %s %f\n",
					"    Layer", k,
					"hlayerch", hlayerch[i][j][k],
					"bwlayerch", bwlayerch[i][j][k],
					"achbed", achbed[i][j][k],
					"vlayerch", vlayerch[i][j][k],
					"minvlayerch", minvlayerch[i][j][k],
					"maxvlayerch", maxvlayerch[i][j][k],
					"elevlayerch", elevlayerch[i][j][k],
					"porositych", porositych[i][j][k]);

				//print label for node solids concentrations
				fprintf(gridfile_fp, "%s",
					"    Csedch");

				//loop over solids
				for(isolid=1;isolid<=nsolids;isolid++)
				{
					//print node solids concentrations
					fprintf(gridfile_fp, " %f", csedch[isolid][i][j][k]);

				}	//end loop over solids

				//print end of line for solids concentrations
				fprintf(gridfile_fp, "\n");

				//print label for node chemical concentrations
				fprintf(gridfile_fp, "%s",
					"    Cchemch");

				//loop over chemicals
				for(ichem=1;ichem<=nchems;ichem++)
				{
					//print node chemical concentrations
					fprintf(gridfile_fp, " %f", cchemch[ichem][i][j][k]);

				}	//end loop over chemicals

				//print end of line for chemical concentrations
				fprintf(gridfile_fp, "\n");

			}	//end loop over layers

		}	//end loop over nodes

	}	//end loop over links

	//Close local file pointer
	fclose(gridfile_fp);

	//deallocate memory for local variables
	free(gridfile);

//End of function: Return to WriteRestart
}
