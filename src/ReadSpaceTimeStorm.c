/*---------------------------------------------------------------------
C-  Function:   ReadSpaceTimeStorm.c                                              
C-
C-  Purpose/    Reads the space-time storm parameters, storm Depths (in.),
C-  Methods:    storm Areas (mi2), and storm Durations (hrs.), 
C-              for design storms (standard D-A-D tables) from
C-              USACE/USBR storm catalog.
C-              Converts depth-duration data to rates by difference
C-              over entered duration.
C-              
C-              This routine uses individual storms in storm DAD files.
C-              These user-input files MUST be edited for each storm.
C-              The user must specify the storm duration and maximum
C-              area size within the storm file. TREX then determines
C-              the storm area and duration based on this user input.
C-              If the user wishes to restrict (expand) the storm duration
C-              and/or storm area, the DAD file must be edited to reflect this.
C-              For example, if the storm area is 20,000 mi^2 and the 
C-              desired input is 5,000 mi^2 (restrict area), simply edit
C-              the DAD file to reflect the desired area size.
C-
C-              This routine uses the storm area sizes in the DAD files
C-              to determine pointers [i] to rain gage id rgid[i].
C-              Rainfall Pointer Grid developed in InitializeWater
C-
C-              Eventually inplement option to rearrange rates
C-              based on user-input hyetograph pattern.
C-              Eventually modify for stochastic storm transposition.
C-
C-  Inputs:     convunits, convtime, scale (from ReadDataGroupB)
C-
C-  Outputs:    nrg, rgid[], rfintensity[][], rftime[][], nrpairs[],
C-              stormorient, stormelong, raincenterx, raincentery,
C-              rainarea[] (m^2), cumdaddepth[] (inches)
C-
C-  Controls:   dt[ndt]
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
C-  Date:       08-SEP-2005
C-
C-  Revised:    John England, USBR
C-
C-  Date:       14-SEP-2008
C-
C-  Revisions:  rainopt integration 
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

void ReadSpaceTimeStorm(float convunits, float convtime, float scale)
{
  //Local Variable Declarations

int
  ndurations,  //number of duration increments of the total storm
  nareas;  //number of area sizes of the total storm

float
  maxdur,    //maximum duration to limit DAD storm duration
  maxarea,  //maximum area size to limit DAD storm area
  *dadtime,  //storm rainfall time increments (hours)
  **daddepth,  //cumulative rainfall depth for particular area (inches)
  dadrainendtime;  //arbitrary very long end time to prevent time wrapping

  //Initialize locals
  dadrainendtime = 100000;  //end time in hours

//Note: Echo file is opened and closed in ReadDataGroupB
//so we can write directly here using echofile_fp

  //write message to screen
  printf("\n\n*************************************\n");
  printf(    "*                                   *\n");
  printf(    "*   Reading Space-Time Storm File   *\n");
  printf(    "*                                   *\n");
  printf(    "*************************************\n\n\n");

  //Set the space-time storm file pointer to null
  spacetimestormfile_fp = NULL;

  //Open the space-time storm file for reading
  spacetimestormfile_fp=fopen(spacetimestormfile,"r");
 
  //if the space-time storm file pointer is null, abort execution
  if(spacetimestormfile_fp==NULL)
    {
      //Write message file
      fprintf(echofile_fp, "Error! Can't open Space-Time Storm File : %s \n",spacetimestormfile);

      //Write message screen
	  printf("Can't open Space-Time Storm File : %s \n",spacetimestormfile);
      exit(EXIT_FAILURE);  //abort
    }

  //Write label for space-time storm file to echo file
  fprintf(echofile_fp, "\n\n\n  D-A-D Space-Time Storm File Parameters and Data  \n");
  fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  //Record 1
  fgets(header, MAXHEADERSIZE, spacetimestormfile_fp);  //read header

  //Echo header to file
  fprintf(echofile_fp, "\n%s\n", header);
 
  //Record 2
  fscanf(spacetimestormfile_fp, "%s %d %s %f",  //read
	 varname,  //dummy
	 &ndurations,  //number of duration increments of the total storm
	 varname,  //dummy
	 &maxdur);  //maximum duration to limit DAD storm duration

  //Echo number of storm duration increments to file
  fprintf(echofile_fp, "Number of Storm Duration Increments = %d\n", ndurations);

  //Echo maximum storm duration to file
  fprintf(echofile_fp, "User-Entered Maximum of Storm Duration (hours) = %f\n", maxdur);

  //Record 3
  fscanf(spacetimestormfile_fp, "%s %d %s %f",  //read
	 varname,  //dummy
	 &nareas,  //number of area sizes of the total storm
	 varname,  //dummy
	 &maxarea);  //maximum area size to limit DAD storm area

  //Echo number of storm areas to file
  fprintf(echofile_fp, "Number of Storm Areas = %d\n", nareas);

  //Echo maximum storm area size to file
  fprintf(echofile_fp, "User-Entered Maximum of Storm Area Size (mi2) = %f\n", maxarea);

  //Record 4
  fscanf(spacetimestormfile_fp, "%s %f %f",  //read
	 varname,  //dummy
	 &raincenterx,  //storm center X coordinate (UTM)
	 &raincentery);  //storm center X coordinate (UTM)

  //Echo storm center X coordinate to file
  fprintf(echofile_fp, "Storm Center X Coordinate (UTM) = %f\n", raincenterx);

  //Echo storm center Y coordinate to file
  fprintf(echofile_fp, "Storm Center Y Coordinate (UTM) = %f\n", raincentery);

  //Record 5
  fscanf(spacetimestormfile_fp, "%s %f %s %f",  //read
	 varname,  //dummy
	 &stormelong,  //storm shape ratio
	 varname,  //dummy
	 &stormorient);  //storm orientation angle (degrees from North, clockwise direction)

  //Echo storm shape ratio (elongation parameter for ellipse
  fprintf(echofile_fp, "Storm Shape Ratio (ellipse elongation) = %f :1\n", stormelong);

  //Echo user-entered storm orientation to file
  fprintf(echofile_fp, "Storm Orientation (decimal degrees) from North = %f\n", stormorient);

  //Adjust storm orientation to start at horizontal (x=0 line, East)
  //this is a shift to right from vertical (y=0) by 90 degrees
  if(stormorient <=90.0)
    {
      stormorient = (float)(90.0) - stormorient;
    }
  else
    {
      stormorient = (float)(450.0) - stormorient;
    }  //end if stormorient <=90 degrees

  fprintf(echofile_fp, "Adjusted Storm Orientation (decimal degrees) from East = %f\n", stormorient);

  //convert storm orientation to radians
  //to determine ellipses in InitializeWater
  stormorient = (float)(3.14159265358/180.0) * stormorient;

  fprintf(echofile_fp, "Adjusted Storm Orientation (radians) from East = %f\n", stormorient);

  //Set nrg and nrpairs global values from locals
  //later need to modify how we do this in order to read entire table
  //but truncate based on user-entered max values

  //number of "rain" gages set equal to number of isohyets
  nrg = nareas;

  //Allocate memory for DAD storm rainfall
  //Note this is different than ReadDataGroupB
  //
  //first we allocate memory, then read data in separate loop
  //
  //DAD storm rainfall area
  rainarea = (float *)malloc((nareas+1) * sizeof(float ));  

  //storm rainfall time increments (hours)
  dadtime = (float *)malloc((ndurations+2) * sizeof(float));

  //cumulative rainfall depth for particular area (inches)
  daddepth = (float **)malloc((nrg+1) * sizeof(float *));

  //number of pairs in rainfall time series
  nrpairs = (int *)malloc((nrg+1) * sizeof(int));

  //value of rainfall intensity (m/s) (final units for computations)
  rfintensity = (float **)malloc((nrg+1) * sizeof(float *));

  //time break in rainfall intensity time series (hours)
  rftime = (float **)malloc((nrg+1) * sizeof(float *));

  //Base Units on rftime is hours (see trex.c)
  //base units on rfintensity is m/s
  //user needs to convert to these base units via input conversions
  //
  //base units on simtime is hours
  //base units on dt is seconds

  //Set values of nrpairs
  //and Allocate remaining memory for rainfall parameters
  //
  //Loop over number of DAD isohyetal areas
  for(i=1; i<=nrg; i++)
    {
      //scalar - fixed for DAD storm rainfall field!!
      //used same temporary variable fix here...
      //extend pair by one at beginning as storm data start at non-zero hour,
      //and 2 to prevent autowrapping on storms, so extend by 3

      //this should take care of array lengths
      //to automatically disable wrapping (recycling of values) of time functions

      nrpairs[i] = ndurations + 3;

      //here we pad the intensity and time arrays by two (2) places
      //we usually pad arrays by one (+1), here we add +3
      //to automatically disable wrapping (recycling of values) of time functions

      daddepth[i] = (float *)malloc((nrpairs[i]+1) * sizeof(float ));
      //      daddepth[i] = (float *)malloc((nrpairs[i]+3) * sizeof(float ));

      //Initialize first value of depth (boundary) to zero
      daddepth [i][1] = 0.0;

      rfintensity[i] = (float *)malloc((nrpairs[i]+1) * sizeof(float));
      rftime[i] = (float *)malloc((nrpairs[i]+1) * sizeof(float));  
      //      rfintensity[i] = (float *)malloc((nrpairs[i]+3) * sizeof(float));
      //rftime[i] = (float *)malloc((nrpairs[i]+3) * sizeof(float));  
      
    }  //end loop over number of DAD isohyetal areas

  //Initialize first time (boundary) to zero
  //Note D-A-D tables start typically at hour 1, 3, or 6
  dadtime[1] = 0.0;

  //echo to file
  fprintf(echofile_fp, "\n    Read-In Standard D-A-D Storm Duration Area Depth Table \n");
  fprintf(echofile_fp, "        Duration (hours), Area (mi^2), and Depth (inches)\n");
  fprintf(echofile_fp, "        ");

  //Loop over number of durations
  for(i=2; i<=ndurations+1; i++)
   {
     //Record 6
     fscanf(spacetimestormfile_fp, "%f",  //read
	    &dadtime[i]);  //storm rainfall time

     //echo to file
     fprintf(echofile_fp, "%6.1f",dadtime[i]);

   }  //end loop over durations

  //print hard return to match input format
  fprintf(echofile_fp, "\n");

  //loop over number of areas
  for(i=1; i<=nareas; i++)
   {
     //Record 7
     fscanf(spacetimestormfile_fp, "%f",  //read
	    &rainarea[i]);  //DAD storm rainfall area

     //echo to file
     fprintf(echofile_fp, "%8.1f",rainarea[i]);

     for(j=2; j<=ndurations+1; j++)
       {
	 //Record 8
	 fscanf(spacetimestormfile_fp, "%f",  //read
		&daddepth[i][j]);  //cumulative rainfall depth

	 //echo to file
	 fprintf(echofile_fp, "%6.1f",daddepth[i][j]);

       }  //end loop over durations

     //print hard return to match input format
     fprintf(echofile_fp, "\n");

   }  //end loop over areas

  //Allocate memory for cumulative rainfall depth for each "rain gage"
  //this is used later in Initialize Water
  //and also in stochastic storm transposition
  //to determine depth over watershed

  //cumulative dad depth for rain gage
  cumdaddepth = (float *)malloc((nrg+1) * sizeof(float));

  //Initialize
 //Loop over number of DAD isohyetal areas (all)
  for(i=0; i<=nrg; i++)
    {
      //Initialize to zero
      cumdaddepth[i] = 0.0;

    }  //end loop over number of DAD isohyetal areas

  //Print out header to echo file
  fprintf(echofile_fp, "\n    Cumulative DAD Value for Each Rain Gage \n");
  //  fprintf(echofile_fp, "      Rain Gage ID  Cumulative DAD Value\n");
  fprintf(echofile_fp, "      Rain Gage ID  Cumulative DAD Value Scaled Cumulative DAD Value\n");

 //Loop over number of DAD isohyetal areas
  for(i=1; i<=nrg; i++)
    {
      //set value from ndurations
      cumdaddepth[i] = daddepth[i][ndurations+1];

      //Print out to echo file
      //      fprintf(echofile_fp, "%12d %18.1f\n", i,cumdaddepth[i]);
      fprintf(echofile_fp, "%12d %18.1f %18.1f\n", i,cumdaddepth[i],
	      cumdaddepth[i]*scale);

      //scale the cumulative depth for later basin-average calculations
      cumdaddepth[i] = scale*cumdaddepth[i];

    }  //end loop over number of DAD isohyetal areas

  //Allocate memory for rgid,
  //this is used to initialize elliptical pointer grid in InitializeWater
  //convention is smallest area =1 and largest area =narea,
  //there is one "rain gage" per area

  //rain gage id
  rgid = (int *)malloc((nrg+1) * sizeof(int));

  //Assign rgid[], rfintensity[][], rftime[][], and write to echo file
 //Loop over number of DAD isohyetal areas
  for(i=1; i<=nrg; i++)
    {
      rgid[i] = i;
      //convert rainfall area from square miles to m^2
      rainarea[i] = (float)(rainarea[i]*2590000.0);

      //Echo Gage number, Gage X-Coord, Gage Y-Coord and Number of Pairs to file
      fprintf(echofile_fp, "\n\nRain Gage Number = %d\n", rgid[i]);
      fprintf(echofile_fp, "\n   Location:\n");
      fprintf(echofile_fp, "      x coordinate = %f\n", raincenterx);
      fprintf(echofile_fp, "      y coordinate = %f\n", raincentery);
      //print rain area in km^2 but base units in m^2
      fprintf(echofile_fp, "      rainfall area (m^2) = %f\n", rainarea[i]);
      fprintf(echofile_fp, "      rainfall area (km^2) = %f\n", rainarea[i]/1000000.0);
      fprintf(echofile_fp, "\n   Number of Pairs in Time Series = %d\n\n", nrpairs[i]);

      //Write label for rainfall time series to file
      fprintf(echofile_fp, "  Rainfall Intensity (m/s)     ");  //Defacto units!!!
      fprintf(echofile_fp, "  Time (hours)  \n");
      fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ");
      fprintf(echofile_fp, "~~~~~~~~~~~~~~~~\n");

      //Set first value of rainfall intensity and time at boundary to zero
      rfintensity[i][1] = 0.0;
      rftime[i][1] = 0.0;

      //Set second to last value of intensity and time
      rfintensity[i][nrpairs[i]-1] = 0.0;

      //this assumes the last time step is much shorter than storm time
      //this works easily when the storm time increment is O~(hours)
      //and time step is O~(seconds)
      //rftime (hours), dt (seconds)
      rftime[i][nrpairs[i]-1] = (float)(dadtime[ndurations +1] + 1.5*dt[ndt]/3600.0);

      //Set ending (last) value of intensity and time
      rfintensity[i][nrpairs[i]] = 0.0;
      rftime[i][nrpairs[i]] = dadrainendtime;

      //Loop over input DAD depth values,
      //skipping last two (calculated) for wrapping
      for(j=2; j<=nrpairs[i]-2; j++)
	{
	  //Compute rainfall intensity by difference
	  rfintensity[i][j] = (daddepth[i][j] - daddepth[i][j-1])/
	    (dadtime[j] - dadtime[j-1]);

	  //Apply conversion and scale factors
	  rfintensity[i][j] = rfintensity[i][j]
	    * convunits * convtime * scale;

	  //set rainfall time
	  rftime[i][j] = dadtime[j];

	}  //end loop over rainfall intensity time series

      //Print all values at this location
      //Loop over rainfall intensity time series
      for(j=1; j<=nrpairs[i]; j++)
	{
	  //Echo Rainfall Intensity, Rainfall Intensity Time Step to file
	  fprintf(echofile_fp, "%28.8f %18.3f\n", rfintensity[i][j], rftime[i][j]);

	}  //end loop over rainfall intensity time series

    }  //end loop over number of DAD isohyetal areas

  //Free local memory

  //Loop over number of gages
  for(i=1; i<=nrg; i++)
    {
      //Free memory for daddepth
      free(daddepth[i]);

    }  //End loop over gages

  //Free remaining memory for daddepth
  free(daddepth);

  //Free dadtime
  free(dadtime);

  //To Do Later... implement for Storm Generation
  //Read non-dimensional hyetograph

  //Read nordinates
  //loop
  //Read x (t/tp), y(d/dp)


  //Close Space Time Storm File
  fclose(spacetimestormfile_fp);

//End of Function: Return to ReadDataGroupB
}
