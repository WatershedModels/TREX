/*---------------------------------------------------------------------
C-  Function:   ReadRadarRainRates.c                                              
C-
C-  Purpose/    Reads the rain rates (mm/hr) (typical units, can convert)
C-  Methods:    derived from radar data for each (x,y) UTM cell center
C-              locations associated with Rainfall Option 3.
C-
C-  Inputs:     convunits, convtime, scale (from ReadDataGroupB),
C-              nrg (from ReadRadarRainLocations)
C-
C-  Outputs:    rfintensity[][], rftime[][], nrpairs[]
C-
C-  Controls:   dt[ndt], radarrainendtime
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
C-  Date:       11-APR-2005
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

void ReadRadarRainRates(float convunits, float convtime, float scale)
{
  //local variable declarations/definitions

  float
    timeincrement,  //incremental radar rain rate duration in minutes
    cumradartime,  //cumulative radar time in hours (to match base time for rftime)
    radarrainendtime;  //arbitrary very long end time to prevent time wrapping
  
  int
    stormduration;  //total storm duration in hours - temp input is number of rows (nrpairs)
    //(temporary for nrpairs, this is number of rows)
    //    radarrows,  //number of rows in radar rain rate file (nrpairs)
    //    radarcols;  //number of columns in radar rain rate file (local for input nrg)

  //Initialize locals
  cumradartime = 0.0;  //cumulative time in hours, start at 0.
  //may have alignment problem with cumradartime start at 0 and different tstart for model
  radarrainendtime = 100000;  //end time in hours

  //write message to screen
  printf("\n\n*******************************\n");
  printf(    "*                             *\n");
  printf(    "*   Reading Radar Rain Rates  *\n");
  printf(    "*                             *\n");
  printf(    "*******************************\n\n\n");

  //Set the radar rain verification file pointer to null
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

  //Set the radar rain rate file pointer to null
  radarrainfile_fp = NULL;

  //Open the radar rain rate file for reading
  radarrainfile_fp=fopen(radarrainfile,"r");
 
  //if the radar rain rate file pointer is null, abort execution
  if(radarrainfile_fp==NULL)
    {
      //Write message to file
      fprintf(echofile_fp, "Error! Can't open Radar Rain Rate File : %s \n",radarrainfile);

      //Write message to screen
	  printf("Error! Can't open Radar Rain Rate File : %s \n",radarrainfile);
      exit(EXIT_FAILURE);  //abort
    }

  //Write label for radar rain rate file to radarverify file
  fprintf(radarverifyfile_fp, "\n\n\n  Radar Rainfall: Rain Rates  \n");
  fprintf(radarverifyfile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  //Record 1
  fgets(header, MAXHEADERSIZE, radarrainfile_fp);  //read header

  //Echo header to file
  fprintf(radarverifyfile_fp, "\n%s\n", header);

  //Record 2
  fscanf(radarrainfile_fp, "%s %f %s %d",  //read
	 varname,  //dummy
	 &timeincrement,  //rain rate duration increment
	 varname,  //dummy
	 &stormduration);  //total storm duration - temporary input is nrows

  //Echo rain rate time increment and total storm duration file
  fprintf(radarverifyfile_fp, "Radar Rain Rate Duration Increment (minutes) = %f\n",
	  timeincrement);
  fprintf(radarverifyfile_fp, "Radar Total Storm Duration - number of rows in file = %d\n", 
	  stormduration);

  //Echo Conversion units, time and scale 
  fprintf(radarverifyfile_fp, "Radar Conversion Units = %f\n", convunits);
  fprintf(radarverifyfile_fp, "Radar Conversion Time = %f\n", convtime);
  fprintf(radarverifyfile_fp, "Radar Conversion Scale = %f\n", scale);

  //Calculate number of rows and columns to read in radar rainfall field file
  //
  //jfe first time temporarily fix both of these variables
  //directly in input file, then later determine on read-in

  //number of rows in radar file set equal to stormduration
  //one row represents rain rate at one time in all space

  //number of columns in radar rate file
  //where column represents a particular location,
  //this is fixed from ReadRadarLocations as nrg

  //Allocate memory for radar rain cell parameters
  //Note this is different than ReadDataGroupB
  //
  //first we allocate memory, then read data in separate loop
  //
  //number of pairs in rainfall time series
  nrpairs = (int *)malloc((nrg+1) * sizeof(int));

  //value of rainfall intensity (m/s) (final units for computations)
  rfintensity = (float **)malloc((nrg+1) * sizeof(float *));

  //time break in rainfall intensity time series (hours)
  rftime = (float **)malloc((nrg+1) * sizeof(float *));

  //Base Units on rftime is hours (see casc2d.c)
  //base units on rfintensity is m/s
  //user needs to convert to these base units via input conversions
  //
  //base units on simtime is hours
  //base units on dt is seconds

  //Set values of nrpairs
  //and Allocate remaining memory for rainfall parameters
  //
  //Loop over number of radar cell locations
  for(i=1; i<=nrg; i++)
    {
      //scalar - fixed for rainfall field!!
      //used same temporary variable fix here...
      //extend pair by 2 to prevent autowrapping on storms
      nrpairs[i] = stormduration + 2;

      //here we pad the intensity and time arrays by two (2) places
      //we usually pad arrays by one (+1), here we add +3
      //to automatically disable wrapping (recycling of values) of time functions

      rfintensity[i] = (float *)malloc((nrpairs[i]+3) * sizeof(float));
      rftime[i] = (float *)malloc((nrpairs[i]+3) * sizeof(float));  
      
    }  //end loop over number of radar cell locations

  //Print header for rainfall rates
  fprintf(radarverifyfile_fp, "\n User-Input Radar Rainfall Rate Field, mm/hr\n");

  //Read in rainfall intensity from file
  //
  //Loop over number of pairs to read (except last two) (rows)
  for(j=1; j<=stormduration; j++)
    {
      //Loop over number of radar cell locations (columns)
      for(i=1; i<= nrg; i++)
	{
	  //Record 3
	  fscanf(radarrainfile_fp, "%f",  //read
		 &rfintensity[i][j]);  //Rainfall Intensity

	  //Echo Rainfall Intensity to file in same format as input
	  //format of f is to match current Princeton input format
	   fprintf(radarverifyfile_fp, "%6.2f", rfintensity[i][j]);

	  //set value for rainfall time
	  rftime[i][j] = cumradartime;  //Rainfall Intensity Time Break

	  //Apply conversion and scale factors
	  rfintensity[i][j] = rfintensity[i][j]
	    * convunits * convtime * scale;

	}  //end loop over radar cell locations

      //print hard return after each line to match input format
      fprintf(radarverifyfile_fp, "\n");

      //Increment cumulative radar time (hours)
      cumradartime = (float)(cumradartime + timeincrement/60.0);

    }  //end loop over number of pairs

  //Loop over all locations and set last two time pairs
  for(i=1; i<=nrg; i++)
    {
      //we set last two array elements
      //rainrates to 0.0 m/s
      //and set times to last time + 1.5*dt[ndt]
      //and large value initialized in top of routine

      //set values directly
      rfintensity[i][stormduration+1] = 0.0;
      rfintensity[i][stormduration+2] = 0.0;

      //set next time just after the radar file end time
      //this assumes the last time step is much shorter than radar time
      //this works when the radar time increment is O~(minutes)
      //and time step is O~(seconds)
      //rftime (hours), dt (seconds)
      rftime[i][stormduration+1] = (float)(cumradartime + 1.5*dt[ndt]/3600.0);
 
      //last time is arbitrary long
      rftime[i][stormduration+2] = radarrainendtime;

    }  //end loop over rainfall locations

  //Time series pairs - send to verify file
  //first location and last location, complete time pairs
  //
  //Write label for rainfall time series first location pairs to file
  //Note mm/hr is default input units!
  fprintf(radarverifyfile_fp, "\n\n  Rainfall Pairs at FIRST Location\n");
  fprintf(radarverifyfile_fp, "  Rainfall Intensity (mm/hr)     ");
  fprintf(radarverifyfile_fp, "  Time (hours)  \n");
  fprintf(radarverifyfile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ");
  fprintf(radarverifyfile_fp, "~~~~~~~~~~~~~~~~\n");

  //print first location (i=1)

  //Loop over all rainfall pairs to print (rows)
  for(j=1; j<=stormduration+2; j++)
    {
      //Echo Rainfall Intensity, Rainfall Intensity Time Step to file in input units
      fprintf(radarverifyfile_fp, "%28.8f %19.5f\n", rfintensity[1][j]/
	      (convunits*convtime*scale), rftime[1][j]);

    }  //end loop over number of pairs

  //Write label for rainfall time series last location pairs to file
  //Note mm/hr is default input units!
  fprintf(radarverifyfile_fp, "\n\n  Rainfall Pairs at LAST Location\n");
  fprintf(radarverifyfile_fp, "  Rainfall Intensity (mm/hr)     ");
  fprintf(radarverifyfile_fp, "  Time (hours)  \n");
  fprintf(radarverifyfile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ");
  fprintf(radarverifyfile_fp, "~~~~~~~~~~~~~~~~\n");

  //print last location (i=nrg)
  //Loop over all rainfall pairs to print (rows)
  for(j=1; j<=stormduration+2; j++)
    {
      //Echo Rainfall Intensity, Rainfall Intensity Time Step to file in input units
      fprintf(radarverifyfile_fp, "%28.8f %19.5f\n", rfintensity[nrg][j]/
	      (convunits*convtime*scale), rftime[nrg][j]);

    }  //end loop over number of pairs

  //Close the radar verification file
  fclose(radarverifyfile_fp);

  //Close Radar Rain Rates File
  fclose(radarrainfile_fp);
}

/*

SCRAPS


  //Write label for rainfall time series to file
  fprintf(radarverifyfile_fp, "  Rainfall Intensity (m/s)     ");  //Defacto units!!!
  fprintf(radarverifyfile_fp, "  Time (hours)  \n");
  fprintf(radarverifyfile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ");
  fprintf(radarverifyfile_fp, "~~~~~~~~~~~~~~~~\n");


  //Echo simulation error file name to error file
  fprintf(errorfile_fp, "\n\n Simulation Error File: ReadRadarRainRates Input\n");
  fprintf(errorfile_fp, "\n Radar Rainfall Rate mm/hr\n");


  //NOTE printing of time series is suppressed
  //Echo Rainfall Intensity, Rainfall Intensity Time Step to file
  fprintf(radarverifyfile_fp, "%28.8f %18.3f\n", rfintensity[i][j], rftime[i][j]);

  //specific array locations

      //Echo Rainfall Intensity at first location, to file
      fprintf(radarverifyfile_fp, "rfintensity 1,1;  1,124 %28.8f %28.8f\n", rfintensity[1][1]/
	      (convunits*convtime*scale),rfintensity[1][124]/(convunits*convtime*scale) );

      //Echo Rainfall Intensity at last location, to file
      fprintf(radarverifyfile_fp, "rfintensity 40000,1;  40000,124 %28.8f %28.8f\n", rfintensity[40000][1]/(convunits*convtime*scale),
	      rfintensity[40000][124]/(convunits*convtime*scale) );

      //Echo Rainfall Intensity at last location, to file
      fprintf(radarverifyfile_fp, "rfintensity 39999,1;  39999,124 %28.8f %28.8f\n", rfintensity[39999][1]/(convunits*convtime*scale),
	      rfintensity[39999][124]/(convunits*convtime*scale) );

*/
