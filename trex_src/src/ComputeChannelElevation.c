/*----------------------------------------------------------------------
C-  Function:	ComputeChannelElevation.c
C-
C-	Purpose/	
C-	Methods:	Computes channel bed elevation from overland elevation
C-              and channel bank height.
C-
C-	Inputs:		elevationov[][], hbank[][]  (Globals)
C-
C-	Outputs:	elevationch[][]  (Globals)
C-
C-	Controls:   nlinks, nnodes[]
C-
C-	Calls:	    none
C-
C-	Called by:	ReadDataGroupB
C-
C-	Created:	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80521
C-
C-	Date:		04-FEB-2004
C-
C-	Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       13-JUN-2008
C-
C-	Revisions:  Added assignments for elevationch0 so the starting bed
C-              elevation is stored for later use in other modules.
C-
C-  Revised:
C-
C-  Date:
C-
C-	Revisions:
C-
C--------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void ComputeChannelElevation()
{
	//local variable declarations, initialization and description
	int row,			//row value of ichnrow[][] for present node
		col;			//col value of ichncol[][] for present node

	//Write label for channel bed elevations to file
	fprintf(echofile_fp,"\n\n\n  Channel Bed Elevations  \n");
	fprintf(echofile_fp,      "--------------------------\n\n");

	fprintf(echofile_fp, "\n\n  Link    ");
	fprintf(echofile_fp,     "  Node    ");
	fprintf(echofile_fp,     "  Overland Elevation (m)    ");
	fprintf(echofile_fp,     "  Bank Height (m)    ");
	fprintf(echofile_fp,     "  Channel Bed Elevation (m)  \n");
	fprintf(echofile_fp,     "~~~~~~~~  ");
	fprintf(echofile_fp,     "~~~~~~~~  ");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~~~~~~~~~~  ");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~~~  ");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Allocate memory for channel bed elevations
	elevationch0 = (float **)malloc((nlinks+1) * sizeof(float *));	//allocate memory
	elevationch = (float **)malloc((nlinks+1) * sizeof(float *));	//allocate memory

	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Allocate remaining memory for channel bed elevations
		elevationch0[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));	//allocate memory
		elevationch[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));	//allocate memory

		//Loop over number of nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//determine row and collumn of present link/node
			row = ichnrow[i][j];
			col = ichncol[i][j];

			//compute initial channel bed elevation of present link/node (m)
			elevationch0[i][j] = elevationov[row][col] - hbank[i][j];

			//Echo bed elevation to file
			fprintf(echofile_fp,"%8d  %8d  %26.3f  %19.3f  %27.3f\n",
				i, j, elevationov0[row][col], hbank[i][j], elevationch0[i][j]);

			//set present channel bed elevation to initial elevation (m)
			elevationch[i][j] = elevationch0[i][j];

		}	//end loop over nodes

		//add blank line between links to echo file
		fprintf(echofile_fp, "\n");

	}	//end loop over links

//End function: Return to ReadDataGroupB
}
