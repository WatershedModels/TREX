/*---------------------------------------------------------------------
C-  Function:	ReadDataGroupA.c
C-
C-  Purpose/    ReadDataGroupA is called at the start of the simulation
C-  Methods:    to read Data Group A (general controls) from the model
C-              input file.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Controls:   dtopt (0,1,2,3) (time step option) (see also trex.c)
C-
C-              dtopt = 0: time steps entered by user (idt = 1,ndt)
C-                as sequence of paired values (dt, dttime) in a step
C-                function time series
C-              dtopt = 1-2: time steps calculated by model according
C-                to maximum and minimum Courant number conditions
C-                with an initial time step (dt) value entered by user
C-              dtopt = 3: time steps read from external file as a
C-                sequence of paired valies (dt, dttime) in a step
C-                function time series
C-
C-  Calls:      None
C-
C-  Called by:  ReadInputFile
C-
C-  Created:    Mark Velleux
C-              Aditya Maroo
C-
C-  Date:       15-JUL-2003
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       16-JAN-2007
C-
C-  Revisions:  Added input data field for Julian day of simulation
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       13-JUN-2008
C-
C-  Revisions:  Added input data field for starting value of gridcount
C-
C-  Revised:    Masa Takamatsu
C-              Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       29-JUL-2010
C-
C-  Revisions:  Added input data fields and controls for time step
C-              option (dtopt) implementation
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

void ReadDataGroupA()
{
	//write message to screen
	printf("\n\n***************************\n");
	printf(	   "*                         *\n");
	printf(	   "*   Reading Data Group A  *\n");
	printf(	   "*                         *\n");
	printf(	   "***************************\n\n\n");

	//Record 1
	fgets(header1, MAXHEADERSIZE, inputfile_fp);	//read header1 from the input file

	//Record 2
	fgets(header2, MAXHEADERSIZE, inputfile_fp);	//read header2 from the input file

	//Record 3
	fscanf(inputfile_fp, "%s %d %s %d %s %d %s %f %s %f %s %lf %s %lf",	//read
			varname,	//dummy 
			&ksim,		//simulation type
			varname,	//dummy
			&nrows,		//number of rows
			varname,	//dummy
			&ncols,		//number of columns
			varname,	//dummy
            &dx,		//grid cell length in east-west direction
			varname,	//dummy
            &dy,		//grid cell length in north-south direction
			varname,	//dummy
			&tzero,		//simulation time at time zero (decimal julian day/time) (day of year)
			varname,	//dummy
			&tstart);	//simulation start time (elapsed time past time zero) (hours)

	//Record 4 
	fscanf(inputfile_fp,"%s %d",	//read
			varname,				//dummy
			&dtopt);				//time step option

	//if time steps are entered by user (dtopt = 0)
	if(dtopt == 0)	//Explicit time steps input by user
	{
		//Record 5
		fscanf(inputfile_fp, "%s %d",	//read
			varname,	//dummy
			&ndt);		//number of time steps

		//Allocate memory for numerical integration time steps
		dt = (float *)malloc((ndt+1) * sizeof(float));		//allocate memory
		dttime = (float *)malloc((ndt+1) * sizeof(float));	//allocate memory

		//loop over number of time steps
		for(i=1; i<=ndt; i++)
		{
			//Record 6
			fscanf(inputfile_fp, "%f %f",	//read
				&dt[i],			//time step value (seconds)
				&dttime[i]);	//time break for time step (hours)

		}	//end loop over time step series
	}
	else if(dtopt == 1 || dtopt == 2)	//else if time steps are calculated by the model (dtopt = 1 or dtopt = 2)
	{
		//set number of time steps to zero
		ndt = 0;

		//set buffered array element counter to zero
		bdt = 0;

		//Allocate memory for numerical integration time steps
		dt = (float *)malloc((MAXBUFFERSIZE+1) * sizeof(float));		//allocate memory (maxbuffer+2 to allow us to use maxbuffer+ 1 elements_
		dttime = (float *)malloc((MAXBUFFERSIZE+1) * sizeof(float));	//allocate memory

		//Record 7
		fscanf(inputfile_fp, "%s %f %s %f %s %f %s %f", //read
			varname,		//dummy
			&dtmax,			//maximum time step (seconds)
			varname,		//dummy
			&maxcourant,	//maximum Courant number for numerical stability (dimensionless) (must be > 0 and <= 1.0)
			varname,		//dummy
			&relaxation,	//relaxation factor for time step estimation (dimensionless) 
			varname,		//dummy
			&dttime[ndt]);	//simulation end time (hours) (later assigned as tend, ndt = 0 as assigned above)

		//set time step to maximum value (seconds) (note: ndt = 0 as assigned above)
		dt[ndt] = dtmax;

		//if maxcourant > 1.0 or maxcourant <= 0.0
		if(maxcourant > 1.0 || maxcourant <= 0.0)
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nData Group Error:\n");
			fprintf(echofile_fp,       "  Maximum Courant number not valid!\n");
			fprintf(echofile_fp,       "  User selected maxcourant = %f\n", maxcourant);
			fprintf(echofile_fp,       "  Specify maxcourant > zero and maxcourant <= 1.0");

			//Write error message to screen
			printf("\n\n\nData Group Error:\n");
			printf(      "  Minimum Courant number not valid!\n");
			printf(      "  User selected maxcourant = %f\n", maxcourant);
			printf(      "  Specify maxcourant > zero and maxcourant <= 1.0");

			exit(EXIT_FAILURE);  //abort

		}	//end if maxcourant > 1.0

		//Record 8 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);	//dummy

		//Allocate memory for time step (dt) output file name
		dtoutputfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 8 (Part 2)
		fgets(dtoutputfile, MAXNAMESIZE, inputfile_fp);	//read name of dt output file (including any path)

		//strip leading blanks, trailing blanks, final carriage return from string
		dtoutputfile = StripString(dtoutputfile);
	}
	else if(dtopt == 3)	//time steps are calculated by model
	{
		//Record 9 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);	//dummy

		//Allocate memory for dt file name
		dtinputfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 9 (Part 2)
		fgets(dtinputfile, MAXNAMESIZE, inputfile_fp);	//read name of dt input file (including any path)

		//strip leading blanks, trailing blanks, final carriage return from string
		dtinputfile = StripString(dtinputfile);

		//Read External Time Step Input File;
		ReadDTFile();
	}
	else	//warn for invalid dtopt
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nData Group Error:\n");
		fprintf(echofile_fp,       "  Time stepping option < 0 or > 3 not valid!\n");
		fprintf(echofile_fp,       "  User selected dtopt = %5d\n", dtopt);
		fprintf(echofile_fp,       "  Select dtopt = 0, 1, 2, or 3");

		//Write error message to screen
		printf("\n\n\nData Group Error:\n");
		printf(      "  Time stepping option < 0 or > 3 not valid!\n");
		printf(      "  User selected dtopt = %5d\n", dtopt);
		printf(      "  Select dtopt = 0, 1, 2, or 3");

		exit(EXIT_FAILURE);  //abort

	}	//end if dtopt = 0, 1-2, 3

	//Record 10
	fscanf(inputfile_fp, "%s %d", varname, &nprintout);	//read number of output print intervals

	//Allocate memory for output print intervals
	printout = (float *)malloc((nprintout+1) * sizeof(float));	//allocate memory
	printouttime = (float *)malloc((nprintout+1) * sizeof(float));	//allocate memory

	//Loop over number of output print intervals
	for(i=1; i<=nprintout; i++)
	{
		//Record 11
		fscanf(inputfile_fp, "%f %f",	//read
			&printout[i],		//output print interval
			&printouttime[i]);	//time break for output print interval
	}	//end loop over print interval series

	//Record 12
	fscanf(inputfile_fp, "%s %d %s %d",	//read
		varname,		//dummy
		&nprintgrid,	//read number of grid print intervals
		varname,		//dummy
		&gridcount);	//read starting value for grid count (sequence number for grid outputs)

	//Allocate memory for grid print intervals
	printgrid = (float *)malloc((nprintgrid+1) * sizeof(float));	//allocate memory
	printgridtime = (float *)malloc((nprintgrid+1) * sizeof(float));	//allocate memory

	//Loop over number of grid print intervals
	for(i=1; i<=nprintgrid; i++)
	{
		//Record 13
		fscanf(inputfile_fp, "%f %f",	//read
			&printgrid[i],		//grid print interval
			&printgridtime[i]);	//time break for grid print interval
	}	//end loop over print interval series

	//Record 14 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);	//dummy

	//Allocate memory for echo file name
	echofile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 14 (Part 2)
	fgets(echofile, MAXNAMESIZE, inputfile_fp);	//read name of echo file (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	echofile = StripString(echofile);

	//set the echo file pointer to null
	echofile_fp = NULL;

	//open the echo file
	echofile_fp = fopen(echofile, "w");

	//if file pointer is still null, then abort
	if(echofile_fp == NULL)
	{
        printf("Error! Can't open Echo file %s\n", echofile);	//write error message to screen
        exit(EXIT_FAILURE);	//exit with error
    }

	//write welcome banner to echo file
	fprintf(echofile_fp, "\n\n   Welcome to TREX");
	fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~\n\n\n\n");

	//echo input file name
	fprintf(echofile_fp, "\n\nInput File Name:  %s\n\n\n", inputfile);

	//echo header lines to file
	fprintf(echofile_fp, "%s%s\n\n", header1, header2);

	//Echo Simulation Type, Number of Rows and Number of Columns to file
	fprintf(echofile_fp, "\nSimulation Type = %d\n", ksim);
	fprintf(echofile_fp, "\nSize of Spatial Domain:\n");
	fprintf(echofile_fp, "   Number of Rows = %d\n", nrows);
	fprintf(echofile_fp, "   Number of Columns = %d\n", ncols);
	fprintf(echofile_fp, "\nGrid Cell Size:\n");
	fprintf(echofile_fp, "   dx (east-west) = %f (m)\n", dx);
	fprintf(echofile_fp, "   dy (north-south) = %f (m)\n\n", dy);

	//Developer's Note:
	//Use of different values for dx and dy is for future development
	//and is not currently supported.  At present, the code assumes
	//that all grid cells are square so dx must equal dy.  The error
	//check below will need to be deleted if code development to
	//support different dx and dy values is implemented...  mlv 7/03
	if(dy != dx)	//delete when the code supports different dx and dy values...
	{
		//write error message to screen
        printf("Error! Can't have different values for dx and dy at present");

		//write error message to file to file
		fprintf(echofile_fp, "\nGrid must be square (dy = dx):\n");
		fprintf(echofile_fp, "   dx (east-west) = %f (m)\n", dx);
		fprintf(echofile_fp, "   dy (north-south) = %f (m)\n\n", dy);

		exit(EXIT_FAILURE);	//exit with error
	}
	else	//dx = dy, so set cell size w = dx (= dy)
	{
		//set cell size "w"
		w = dx;
	}	//end if(dy != dx)

	//Echo simulation start and end times to file
	fprintf(echofile_fp, "\nSimulation start and end times:");
	fprintf(echofile_fp, "\n   Start = %f (hours)", tstart);
	fprintf(echofile_fp, "\n   End = %f (hours)\n\n", dttime[ndt]);

	//Echo integration time step option to file
	fprintf(echofile_fp, "\nIntegration Time Step Option = %d\n\n", dtopt);

	//if dtopt = 0
	if(dtopt == 0)
	{
		//Echo number of time steps to file
		fprintf(echofile_fp, "\nNumber of Time Steps = %d\n", ndt);

		//Print label for time step series to file
		fprintf(echofile_fp, "\n    dt          time   ");
		fprintf(echofile_fp, "\n~~~~~~~~~~   ~~~~~~~~~~\n");

		//Loop over number of time steps
		for(i=1; i<=ndt; i++)
		{
			//Echo dt, dttime series to file
			fprintf(echofile_fp, "%10.4f   %10.4f\n", dt[i], dttime[i]);
		}
	}
	else if(dtopt == 1 || dtopt == 2)	//else if dtopt = 1 or dtopt = 2
	{
		//Echo maximum time step value to file
		fprintf(echofile_fp, "\nMaximum Time Step (dt) (seconds) = %f\n", dtmax);

		//Echo maximum Courant number to file
		fprintf(echofile_fp, "\nMaximum Courant Number = %f\n", maxcourant);

		//Echo relaxation factor to file
		fprintf(echofile_fp, "\nRelaxation Factor = %f\n", relaxation);

		//Echo time step output file name to file
		fprintf(echofile_fp, "\n\nCalculated Time Step Output File: %s\n", dtoutputfile);
	}
	else if(dtopt == 3)
	{
		//Echo external time step file name to file
		fprintf(echofile_fp, "\n\nExternal Time Step Input File: %s\n", dtinputfile);

		//Echo external time steps to file...
		//
		//Note:  External time step data is written to file here rather
		//       than when it is first read because the echo file is not
		//       open and available for output before this point.
		//
		//Echo dt input file header to file
		fprintf(echofile_fp, "\n%s\n", header);

		//Echo number of time steps to file
		fprintf(echofile_fp, "\nNumber of Time Steps = %d\n", ndt);

		//Print label for time step series to file
		fprintf(echofile_fp, "\n    dt          time   ");
		fprintf(echofile_fp, "\n~~~~~~~~~~   ~~~~~~~~~~\n");

		//Loop over number of time steps
		for(i=1; i<=ndt; i++)
		{
			//Echo dt, dttime series to file
			fprintf(echofile_fp, "%10.4f   %10.4f\n", dt[i], dttime[i]);
		}

	}	//end if dtopt = 0, 1-2, 3

	//Echo number of output print intervals to file
	fprintf(echofile_fp, "\n\nNumber of Print Intervals for Tabular Output = %d\n",
		nprintout);

	//Echo label for output print intervals to file
	fprintf(echofile_fp, "\n Interval (hours)    Time (hours) ");
	fprintf(echofile_fp, "\n~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~\n");

	//Echo output print interval and time breaks to file
	for(i=1; i<=nprintout; i++)
	{
		fprintf(echofile_fp, "%18.5f  %14.4f\n", printout[i], printouttime[i]);
	}

	//Echo number of grid print intervals to file
	fprintf(echofile_fp, "\n\nNumber of Print Intervals for Grid Output = %d\n",
		nprintgrid);
	//Echo label for grid print intervals to file
	fprintf(echofile_fp, "\n Interval (hours)    Time (hours) ");
	fprintf(echofile_fp, "\n~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~\n");

	//Echo grid print interval and time breaks to file
	for(i=1; i<=nprintgrid; i++)
	{
		fprintf(echofile_fp, "%18.5f  %14.4f\n", printgrid[i], printgridtime[i]);
	}

	//Echo grid start number to file
	fprintf(echofile_fp, "\n\nStarting Number for Grid Output = %d\n",
		gridcount);

	//Echo the name of the echo file name to file
	fprintf(echofile_fp, "\n\nEcho File Name: %s\n\n\n", echofile);

	//Close the Echo File
	fclose(echofile_fp);

//End of function: Return to ReadInputFile
}
