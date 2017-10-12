/*---------------------------------------------------------------------
C-  Function:   ReadSoilLayerChemicalFile.c
C-
C-  Purpose/    ReadSoilLayerChemicalFile is called to read soil layer chemical
C-  Methods:    initial condition files that specify the concentration
C-              of a chemical in each layer of the soil stack (in the
C-              overland plane) within the spatial domain of the
C-              simulation.
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupD
C-
C-  Created:    Mark Velleux (CSU)
C-
C-  Date:       23-JUL-2003
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

void ReadSoilLayerChemicalFile(int ilayer, int ichem)
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize,	//length of grid cell (m) (this means dx must equal dy)
		conc;		//chemical concentration (ug/g = mg/kg)

	//write message to screen
	printf("\n\n***************************************************\n");
	printf(	   "*                                                 *\n");
	printf(	   "*   Reading Soil Chemical Initial Condition File  *\n");
	printf(	   "*                                                 *\n");
	printf(	   "***************************************************\n\n\n");

	//Set the soil chemical file pointer to null
	soilchemicfile_fp = NULL;

	//Open the soil chemical file for reading
	soilchemicfile_fp=fopen(soilchemicfile,"r");

	//if the soil chemical file pointer is null, abort execution
	if(soilchemicfile_fp==NULL)
	{
		//Write message to screen
		fprintf(echofile_fp, "Error! Can't open Soil Chemical IC File : %s \n",soilchemicfile);

		//Write message to screen
		printf("Error! Can't open Soil Chemical IC File : %s \n",soilchemicfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for soil chemical file to file
	fprintf(echofile_fp,
		"\n\n\n  Soil Chemical Initial Condition File: Chemical Concentrations (mg/kg)  \n");
	fprintf(echofile_fp,
		      "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, soilchemicfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(soilchemicfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
		varname,		//dummy
		&gridcols,		//number of columns in grid
		varname,		//dummy
		&gridrows,		//number of rows in grid
		varname,		//dummy
		&xllcorner,		//x location of grid lower left corner (m) (GIS projection)
		varname,		//dummy
		&yllcorner,		//y location projection of grid lower left corner (m) (GIS projection)
		varname,		//dummy
		&cellsize,		//length of grid cell (m) (this means dx must equal dy)
		varname,		//dummy
		&nodatavalue);	//no data value (null value)

	//If number of grid rows, grid columns, or cell size do not equal global values, abort...
	if(gridrows != nrows || gridcols != ncols || cellsize != dx)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nSoil Chemical IC File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Soil Chemical IC File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n\n\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo soil initial condition characteristics to file
	fprintf(echofile_fp, "\nSoil Chemical Initial Condition Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(soilchemicfile_fp, "%f", &conc);	//read chemical concentration

			//Echo concentrations to file
			fprintf(echofile_fp, "  %10.4f", conc);

			//if the cell is in the domain
			if(imask[i][j] > 0)
			{
				//Note:  Concentration values are input as ug/g.  These values
				//       are converted from ug/g to g/m3 in InitializeChemical
				//       (which occurs after solids grain size distributions
				//       are converted to concentrations in InitializeSolids).
				//
				//store concentrations as ug/g (mg/kg)
				cchemov[ichem][i][j][ilayer] = conc;

			}	//end if imask[]] > 0

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Soil Cehmical File
	fclose(soilchemicfile_fp);

//End of function: Return to ReadDataGroupD
}
