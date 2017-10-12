/*----------------------------------------------------------------------
C-  Header:     trex_general_definitions.h
C-
C-	Purpose/	Global definitions header file for trex.  Used in
C-	Methods:	concert with trex_general_declarations.h
C-
C-	Created:	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-				
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO  80225
C-
C-	Date:		16-MAR-2004
C-
C-	Revised:	Mark Velleux
C-              Masa Takamatsu
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		29-Jul-2010
C-
C-	Revisions:	Added declarations for time step option (dtopt)
C-              implementation
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C---------------------------------------------------------------------*/

/*************************/
/*   FILE DECLARATIONS   */
/*************************/

//trex main routine
FILE *inputfile_fp = NULL;	//input file pointer

//Data Group A
FILE *echofile_fp = NULL;	//echo file pointer
FILE *dtinputfile_fp;		//time step (dt) input file pointer
FILE *dtbufferfile_fp;		//time step (dt) buffer file pointer
FILE *dtoutputfile_fp;		//time step (dt) output file pointer

//Data Group E

FILE *dmpfile_fp = NULL;		//dump file pointer
FILE *msbfile_fp = NULL;		//mass balance file pointer
FILE *statsfile_fp = NULL;		//statistics file pointer
FILE *errorfile_fp = NULL;		//simulation error file pointer

/************************************/
/*   GLOBAL VARIABLE DEFINITIONS    */
/************************************/


	/*****************************/
	//  general controls and i/o
	/*****************************/
char 
	header1[MAXHEADERSIZE],		//header1 in input file
	header2[MAXHEADERSIZE],		//header2 in input file
	header[MAXHEADERSIZE],		//general header line in input file
	varname[MAXNAMESIZE],		//dummy variable name in input file
	loadname[MAXNAMESIZE],		//load name
	bcname[MAXNAMESIZE];		//boundary condition name

char
	*inputfile,			//input file name (specified by the argv function)
	*dtbufferfile,		//time step (dt) temporary buffer file name (used for dtopt = 1-2)
	*dtoutputfile,		//time step (dt) output file name (used for dtopt = 1)
	*dtinputfile,		//time step (dt) input file name (used for dtopt = 2)
	*echofile,			//echo (summary) file name
	*dmpfile,			//dump (detailed results) file
	*msbfile,			//mass balance file
	*statsfile,			//statistics file
	*errorfile;			//simulation error file

int 
	i,		//loop index
	j,		//loop index
	k,		//loop index
	ksim,	//simulation type index:
				//	1 = hydrology,
				//	2 = sediment transport,
				//	3 = chemical transport
	ksim0,	//initial value of ksim (simulation type index)
	dtopt,	//time step (dt) option
				//	0 = user-defined time step series
				//	1 = model-calculated time step series, no simulation relaunch (overrides ksim)
				//  2 = model-calculated time step series plus simulation relaunch (if ksim > 1)
				//	3 = time step series read from external file
	idt,	//time step index (counter). Used in main code.
	bdt,	//time step buffer index (counter). Used in main code (dtopt = 1-2).
	pdt,	//tabular output print interval index (counter)
	gdt,	//grid output print interval index (counter)
	nrows,	//Number of Rows
	ncols,	//Number of Columns
	ndt,	//Number of Time Steps
	nprintout,		//Number of print intervals for tabular output
	nprintgrid,		//Number of print intervals for grid output
	gridcount;		//running count of the number of time grids are printed (grid sequence number)
	
float
	w,			//width of square cell in overland plane (grid cell size) (m) (to be eventually replaced by dx, dy)
	dx,			//width of cell (x-direction) in overland plane (grid cell width) (m) (currently fixed at w)
	dy,			//width of cell (y-direction) in overland plane (grid cell width) (m) (currently fixed at w)
	dtmax,		//maximum time step (dt) value (used for dtopt = 1-2)
	maxcourant,	//maximum courant number for numerical stability (used for dtopt = 1-2)
	relaxation,	//relaxation factor for time step calculations (used for dtopt = 1-2)
	*dt,				//numerical integration time step
	*dttime,			//Time up to which the time step is used
	*printout,			//Print interval for tabular output
	*printouttime,		//Time up to which the output print interval is used
	*printgrid,			//Print interval for grid output
	*printgridtime;		//Time up to which the grid print interval is used

double	
	simtime,		//current time of simulation
	tzero,			//simulation time at time zero (decimal julian day/time) (day of year)
	tstart,			//simulation start time (elapsed time past time zero) (hours)
	tend;			//simulation end time
