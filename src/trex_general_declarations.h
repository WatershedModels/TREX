/*----------------------------------------------------------------------
C-  Header:     trex_general_declarations.h
C-
C-	Purpose/	Global declarations header file for trex.  Used in
C-	Methods:	concert with trex_general_definitions.h
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

//general order of header:
//	1. include files
//	2. global constant definitions
//	3. file declarations (organized by DataGroups A-E)
//	4. function prototypes (organized by order of appearance/DataGroups A-E)
//	5. variable declarations by state variables, general controls, then ksim
//		(1=water, 2=solids, 3=chemicals)

/******************************************/
/*      INCLUDED LIBRARY HEADER FILES     */
/******************************************/

#include <stdio.h>		//C library for standard I/O functions
#include <stdlib.h>		//C library for utility functions
#include <string.h>		//C library for string functions
#include <math.h>		//C library for math functions
#include <float.h>		//C library for floating point arithmetic
#include <time.h>		//C library for date and time functions

/***********************************/
/*   GLOBAL CONSTANT DEFINITIONS   */
/***********************************/

#define MAXHEADERSIZE 132	//Maximum size of character string for headers in input file
#define MAXNAMESIZE 256		//Maximum size of character string for names in input file and path name
#define MAXBUFFERSIZE 1000	//Maximum size for allocating mamory for buffered arrays
#define TOLERANCE 1.0e-7	//Error tolerance for single precision math

/*************************/
/*   FILE DECLARATIONS   */
/*************************/

extern FILE *inputfile_fp;	//input file pointer

//Data Group A
extern FILE *echofile_fp;		//echo file pointer
extern FILE *dtinputfile_fp;	//time step (dt) input file pointer
extern FILE *dtbufferfile_fp;	//time step (dt) buffer file pointer
extern FILE *dtoutputfile_fp;	//time step (dt) output file pointer

//Data Group F
extern FILE *dmpfile_fp;		//dump file pointer
extern FILE *msbfile_fp;		//mass balance file pointer
extern FILE *statsfile_fp;		//statistics file pointer
extern FILE *errorfile_fp;		//simulation error file pointer

/***************************/
/*   FUNCTION PROTOTYPES   */
/***************************/

extern void ReadInputFile(void);		//Overall control for read of input file

extern void ReadDataGroupA(void);		//Reads Data Group A of input file

extern void ReadDataGroupF(void);		//Reads Data Group F of input file

extern void Initialize(void);			//Initializes variables (sets initial values)

extern void TimeFunctionInit(void);		//Initializes piecewise linear time functions (Loads, BCs)

extern void ComputeInitialState(void);	//Computes initial states of variables at beginning of simulation

extern void UpdateTimeFunction(void);	//Updates piecewise linear time functions (Loads, BCs)

extern void WriteTimeSeries(void);		//Writes time series (export) files

extern void WriteGrids(int);		//Writes grid files

extern void WriteEndGrids(int);		//Writes end of simulation grid files

extern void Grid(char *, float **, float, int);		//Writes a particular grid based on WriteGrids

extern void WriteDumpFile(void);		//Writes (dumps) details of model computations to dump file

extern void NewState(void);	//Set new state (conditions) (flow depths, concentrations) for next time step

extern void Reinitialize(void);	//Reinitializes conditions for simulation relaunch (flow depths, volumnes, etc.)

extern void ComputeFinalState(void);	//Computes final states of variables at end of simulation

extern void WriteMassBalance(void);		//Writes end of simulation mass balance file

extern void WriteSummary(void);			//Writes summary files (water, solids and chemical) statistics at end of simulation

extern void FreeMemory(void);			//Frees all allocated memory at end of simulation

extern void	RunTime(time_t, time_t);		//Computes total run time of simulation

extern void SimulationError(int, int, int, int);	//Reports errors and terminates simulation

extern double Max(double, double);	//Maximum of some value

extern double Min(double, double);	//Minimum of some value

extern char *StripString(char *);	//strip blanks/carriage returns from char strings read by fgets

//Simulation Restart Modules
extern void ReadRestart(int);		//read initial conditions (restart) files for storms in sequence
extern void Read2DIntegerGrid(char *, int **);		//Read integer 2D (row, col) overland grid files for storms in sequence
extern void Read3DIntegerGrid(char *, int ***, int);		//Read integer 3D (row, col, layer) overland grid files for storms in sequence
extern void Read2DFloatGrid(char *, float **);		//Read real (float) overland grid files for storms in sequence
extern void Read3DFloatGrid(char *, float ***, int);		//Read real (float) overland grid files for storms in sequence
extern void Read3DDoubleGrid(char *, double ***, int);		//Read real (double) overland grid files for storms in sequence
extern void ReadSedimentProperties(char *);		//Read channel bed property files for storms in sequence
extern void ReadWaterProperties(char *);		//Read channel water property files for storms in sequence
extern void WriteRestart();			//Prepare initial condition (restart) files for storms in sequence
extern void Write2DIntegerGrid(char *, int **);		//Write integer 2D (row, col) overland grid files for storms in sequence
extern void Write3DIntegerGrid(char *, int ***, int);		//Write integer 3D (row, col, layer) overland grid files for storms in sequence
extern void Write2DFloatGrid(char *, float **);		//Write real (float) overland grid files for storms in sequence
extern void Write3DFloatGrid(char *, float ***, int);		//Write real (float) overland grid files for storms in sequence
extern void Write3DDoubleGrid(char *, double ***, int);		//Write real (double) overland grid files for storms in sequence
extern void WriteSedimentProperties(char *);		//Write channel bed property files for storms in sequence
extern void WriteWaterProperties(char *);		//Write channel water property files for storms in sequence

//Time-Stepping Modules
extern void ReadDTFile(void);				//Read file with sequence of numerical integration time steps
extern void TruncateDT(void);				//Truncate time step values to a fixed number of significant digits
extern void WriteDTBuffer(void);			//Write sequence of numerical integration time steps to temporary buffer file
extern void WriteDTFile(void);				//Write sequence of numerical integration time steps to file



/************************************/
/*   GLOBAL VARIABLE DECLARATIONS   */
/************************************/

	/*****************************/
	//  general controls and i/o
	/*****************************/
extern char
	header1[MAXHEADERSIZE],		//header1 in input file
	header2[MAXHEADERSIZE],		//header2 in input file
	header[MAXHEADERSIZE],		//general header line in input file
	varname[MAXNAMESIZE];		//dummy variable name in input file

extern char
	*inputfile,			//input file name (specified by the argv function)
	*dtbufferfile,		//time step (dt) temporary buffer file name (used for dtopt = 1-2)
	*dtoutputfile,		//time step (dt) output file name (used for dtopt = 1)
	*dtinputfile,		//time step (dt) input file name (used for dtopt = 2)
	*echofile,			//echo (summary) file name
	*dmpfile,			//dump (detailed results) file
	*msbfile,			//mass balance file
	*statsfile,			//statistics file
	*errorfile;			//simulation error file

extern int
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

extern float
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

extern double
	simtime,		//current time of simulation
	tzero,			//simulation time at time zero (decimal julian day/time) (day of year)
	tstart,			//simulation start time (elapsed time past time zero) (hours)
	tend;			//simulation end time
