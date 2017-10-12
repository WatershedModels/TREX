/*---------------------------------------------------------------------
C-  Function:   ReadRadarRainLocations.c                                              
C-
C-  Purpose/    Reads the (x,y) UTM cell center locations for
C-  Methods:    radar rainfall associated with Rainfall Option 3.
C-
C-  Inputs:     none
C-
C-  Outputs:    radarcellw, nrg, rgx[], rgy[]
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
C-  Date:       08-APR-2005
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

void ReadRadarRainLocations()
{

  //write message to screen
  printf("\n\n***********************************\n");
  printf(    "*                                 *\n");
  printf(    "*   Reading Radar Rain Locations  *\n");
  printf(    "*                                 *\n");
  printf(    "***********************************\n\n\n");

  //Set the simulation error file pointer to null
  radarverifyfile_fp = NULL;

  //Open radar rain verification file and write header line
  radarverifyfile_fp = fopen(radarverifyfile,"a");

  //if the radar rain verification file pointer is null, abort execution
  if(radarverifyfile_fp==NULL)
    {
      //Write message to file
      fprintf(echofile_fp, "Error! Can't create Radar Verification File:  %s\n",radarverifyfile);

	  //Write message to screen
      printf("Error! Can't create Radar Verification File:  %s\n",radarverifyfile);
      exit(EXIT_FAILURE);  //abort

    }  //End if radar verification file is null

  //Set the radar rain location file pointer to null
  radarlocationfile_fp = NULL;

  //Open the radar rain location file for reading
  radarlocationfile_fp=fopen(radarlocationfile,"r");
 
  //if the radar rain location file pointer is null, abort execution
  if(radarlocationfile_fp==NULL)
    {
      //Write message to file
      fprintf(echofile_fp, "Error! Can't open Radar Location File : %s \n",radarlocationfile);

	  //Write message to screen
      printf("Error! Can't open Radar Location File : %s \n",radarlocationfile);
      exit(EXIT_FAILURE);  //abort
    }

  //Write label for radar location file to radarverify file
  fprintf(radarverifyfile_fp,
	  "\n\n\n  Radar Rainfall: Grid Cell Center Locations  \n");
  fprintf(radarverifyfile_fp,
	  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  //Record 1
  fgets(header, MAXHEADERSIZE, radarlocationfile_fp);  //read header

  //Echo header to file
  fprintf(radarverifyfile_fp, "\n%s\n", header);
 
  //Record 2
  fscanf(radarlocationfile_fp, "%s %f %s %d",	//read
	 varname,  //dummy
	 &radarcellw,  //uniform spacing between radar cell centers
	 varname,  //dummy
	 &nrg);  //number of radar rain cells ("gages")

  //Echo radar cell center spacing to file
  fprintf(radarverifyfile_fp, "Radar Cell Spacing = %f\n\n", radarcellw);

  //Echo number of rain gages to file
  fprintf(radarverifyfile_fp, "Number of Radar Rain Cells = %d\n\n", nrg);

  //Allocate memory for rain gage parameters
  //
  //Gage x coordinate (m)
  rgx = (float *)malloc((nrg+1) * sizeof(float));
  //Gage y coordinate (m)
  rgy = (float *)malloc((nrg+1) * sizeof(float));

  //Routine Assumes that rgx, rgy are in same coordinate
  //system as other GIS input data/grids (mask, elevation, etc.)

  //Write label to output file
  fprintf(radarverifyfile_fp, "  X Coordinate (m)   Y Coordinate (m) \n");

  //Loop over number of radar rain cell locations
  for(i=1; i<=nrg; i++)
    {
      //Record 3
      fscanf(radarlocationfile_fp, "%f %f",  //read
	     &rgx[i],  //Gage x coordinate (m)
	     &rgy[i]);  //Gage y coordinate (m)

      //Echo Radar X-Coordinate, Y-Coordinate to file
      fprintf(radarverifyfile_fp, "%f %f\n", rgx[i], rgy[i]);

    }  //end loop over number of radar cell locations

  //Close the radar verification file
  fclose(radarverifyfile_fp);

  //Close Radar Rain Location File
  fclose(radarlocationfile_fp);

  /*Eventual Ideas to Implement
    skip headers and just read x, y as locals
    from x, y determine spacing and number of points
    set those variables as local
    then allocate memory
    then read file again and assign values
  */


//End of Function: Return to ReadDataGroupB
}
