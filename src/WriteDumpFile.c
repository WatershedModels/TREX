/*---------------------------------------------------------------------
C-  Function:	WriteDumpFile.c
C-
C-	Purpose/	Write (dump) specified state variables and conditions
C-              for water (flow depths, rates, velocity, etc.), solids
C-              (total and individual particles concentrations), and
C-              chemicals (total concentrations, dissolved, bound, and
C-              particulate concentrations, etc.) in the water column
C-              and soil/sediment over time to a file.
C-
C-
C-	Inputs:
C-
C-	Outputs:
C-
C-	Controls:	ksim, chnopt
C-
C-	Calls:		None
C-
C-	Called by:	trex
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		09-FEB-2004
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C--------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void WriteDumpFile()
{
/*
	//local variable declarations/definitions
	int
		chanlink,	//channel link number
		channode;	//channel node number

	int uplink,		//link number of upstream link
		upnode,		//node number of upstream node
		downlink,	//link number of downstream link
		downnode,	//node number of downstream node
		row,		//row value of ichnrow[][] of a node
		col;		//col value of ichncol[][] of a node

	float
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf;	//surface area of channel (m)
*/

	//set the dump file pointer to null
	dmpfile_fp = NULL;

	//open the dump file (in append mode)
	dmpfile_fp = fopen(dmpfile, "a");

	//if file pointer is still null, then abort
	if(dmpfile_fp == NULL)
	{
        //write error message to file
		fprintf(echofile_fp, "Error! Can't open dump file %s\n", dmpfile);

		//write error message to screen
		printf("Error! Can't open dump file %s\n", dmpfile);
        exit(EXIT_FAILURE);	//exit with error
	}

	//close the dump file
	fclose(dmpfile_fp);

//End of function: Return to trex
}
