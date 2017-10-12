/*---------------------------------------------------------------------
C-  Function:   ReadInputFile.c
C-
C-  Purpose/    ReadInputFile is called at the start of the simulation
C-  Methods:    to the model input file.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   inputfile_fp, ksim
C-
C-  Calls:      ReadDataGroupA, ReadDataGroupB, ReadDataGroupC,
C-              ReadDataGroupD, ReadDataGroupE, ReadDataGroupF
C-
C-  Called by:  trex
C-
C-  Created:    Mark Velleux
C-              John England
C-              Aditya Maroo
C-
C-  Date:       15-JUL-2003
C-
C-  Revised:
C-
C-  Date:
C-
C-  Revisions:
C-
C----------------------------------------------------------------------*/

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

void ReadInputFile()
{
	//Set the input file pointer to null
	inputfile_fp = NULL;

	//Open the input file for reading
	inputfile_fp=fopen(inputfile,"r");

	//if the input file pointer is null, abort execution
	if(inputfile_fp==NULL)
	{
		printf("Can't open Input File : %s \n",inputfile);
		exit(EXIT_FAILURE);
	}

	// Write input file name to screen
	printf("Input File: %s \n",inputfile);

	//Read Data Group A: General Controls
	ReadDataGroupA();

	//Read Data Group B: Hydrologic Simulation Parameters
	ReadDataGroupB();

	//if simulation type is sediment (or chemical) transport...
	if(ksim >= 2)
	{
		//Read Data Group C: Sediment Transport Simulation Parameters
		ReadDataGroupC();

		//if simulation type is chemical transport...
		if(ksim >=3)
		{
			//Read Data Group D: Chemical Transport Parameters
			ReadDataGroupD();

		}	//endif ksim >= 3

	}	//end if ksim >= 2

	//Read Data Group E: Environmental Conditions
	ReadDataGroupE();

	//Read Data Group F: Output Specification Controls
	ReadDataGroupF();

//End of function: Return to trex.c
}
