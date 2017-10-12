/*---------------------------------------------------------------------
C-  Function:   ReadDesignRainGrid.c                                              
C-
C-  Purpose/    Reads the design rain gage number that is applied
C-  Methods:    to each cell within the spatial domain
C-              for Rainfall Option 2.
C-
C-  Inputs:     none
C-
C-  Outputs:    designrainindex[][]
C-
C-  Controls:   none
C-
C-  Calls:      none
C-
C-  Called by:  ReadDataGroupB
C-
C-  Created:    John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-  Date:       29-MAR-2005
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
C--------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void ReadDesignRainGrid()
{
  //Local Variable Declarations
  int
    gridrows,  //number of rows in grid
    gridcols;  //number of columns in grid

  float
    cellsize;  //length of grid cell (m) (this means dx must equal dy)

  //write message to screen
  printf("\n\n*******************************\n");
  printf(    "*                             *\n");
  printf(    "*   Reading Design Rain Grid  *\n");
  printf(    "*                             *\n");
  printf(    "*******************************\n\n\n");

  //Set the design rain grid file pointer to null
  designraingridfile_fp = NULL;

  //Open the design rain grid file for reading
  designraingridfile_fp=fopen(designraingridfile,"r");
 
  //if the design rain grid file pointer is null, abort execution
  if(designraingridfile_fp==NULL)
    {
      //Write message to file
      fprintf(echofile_fp, "Error! Can't open Design Rain Grid File : %s \n",designraingridfile);

      //Write message to screen
      printf("Error! Can't open Design Rain Grid File : %s \n",designraingridfile);
      exit(EXIT_FAILURE);  //abort
    }

  //Write label for design rain grid to echo file
  fprintf(echofile_fp, "\n\n\n  Design Rain Grid File: Grid Cell Rain Gage Location  \n");
  fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  //Record 1
  fgets(header, MAXHEADERSIZE, designraingridfile_fp);	//read header

  //Echo header to file
  fprintf(echofile_fp, "\n%s\n", header);
 
  //Record 2
  fscanf(designraingridfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
	 varname,  //dummy
	 &gridcols,  //number of columns in grid
	 varname,  //dummy
	 &gridrows,  //number of rows in grid
	 varname,  //dummy
	 &xllcorner,  //x location of grid lower left corner (m) (GIS projection)
	 varname,  //dummy
	 &yllcorner,  //y location projection of grid lower left corner (m) (GIS projection)
	 varname,  //dummy
	 &cellsize,  //length of grid cell (m) (this means dx must equal dy)
	 varname,  //dummy
	 &nodatavalue);  //no data value (null value)
  
  //If number of grid rows, grid columns, or cell size do not equal global values, abort...
  if(gridrows != nrows || gridcols != ncols || cellsize != dx)
    {
      //Write error message to file
      fprintf(echofile_fp, "\n\n\nDesign Rain Grid File Error:\n");
      fprintf(echofile_fp, "  nrows = %5d grid rows = %5d\n", nrows, gridrows);
      fprintf(echofile_fp, "  ncols = %5d grid cols = %5d\n", ncols, gridcols);
      fprintf(echofile_fp, "  dx = %12.4f dy = %12.4f cell size = %12.4f\n", dx, dy, cellsize);

      //Write error message to screen
      printf("Design Rain Grid File Error:\n");
      printf("  nrows = %5d grid rows = %5d\n", nrows, gridrows);
      printf("  ncols = %5d grid cols = %5d\n", ncols, gridcols);
      printf("  dx = %12.4f dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);
      
      exit(EXIT_FAILURE);  //abort
    }  //end check of grid dimensions

  //Echo design rain grid header to file
  fprintf(echofile_fp, "\nDesign Rain Grid Header Information:\n");
  fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
  fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
  fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
  fprintf(echofile_fp, "   No Data Value = %6d\n\n\n", nodatavalue);

  //Allocate memory for design rain index grid
  designrainindex = (int **)malloc((gridrows+1) * sizeof(int *));

  //Loop over number of rows
  for(i=1; i<=gridrows; i++)
    {
      //Allocate remaining memory for design rain index grid
      designrainindex[i] = (int *)malloc((gridcols+1) * sizeof(int));

      //Loop over number of columns
      for(j=1; j<=gridcols; j++)
	{
	  //Record 3 - read design rain index grid values
	  fscanf(designraingridfile_fp, "%d", &designrainindex[i][j]);
	  
	  //Echo design rain index grid value to file
	  fprintf(echofile_fp, "  %5d", designrainindex[i][j]);

	}  //end loop over grid columns

      //Start a new line for the next row of data in the echo file 
	fprintf(echofile_fp, "\n");

    } //end loop over grid rows

  //Close Design Rain Grid File
  fclose(designraingridfile_fp);

//End of Function: Return to ReadDataGroupB
}
