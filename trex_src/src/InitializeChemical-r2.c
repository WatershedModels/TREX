/*----------------------------------------------------------------------
C-  Function:	InitializeChemical.c
C-
C-	Purpose/
C-	Methods:	Allocate memory for and initialize chemical variables
C-              used in computations but not read from input files.
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:   chnopt
C-
C-	Calls:		None
C-
C-	Called by:	Initialize
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
C-              Denver, CO 80225
C-
C-	Date:		03-JAN-2004
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       29-APR-2008
C-
C-  Revisions:  Added overland distributed loads.
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		13-JUN-2008
C-
C-	Revisions:  Removed calculation of total solids concentrations and
C-              replaced with csedov[0][][][] and csedch[0][][][].
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
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		23-AUG-2010
C-
C-  Revisions:	Added initializations for subsurface soil/sediment
C-              grid file names.
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C---------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void InitializeChemical()
{
	//Local variable declarations/definitions
	int
		ichem,		//loop index for number of chemical types
		isolid,		//loop index for number of solids types
		nsources;	//number of transport sources/directions

	//initialize number of transport directions/sources
	nsources = 10;

	/*********************************************************/
	/*        Initialize Total Chemical Mass Variables       */
	/*********************************************************/

	//Allocate memory for overland chemical mass variables
	initialchemmassov = (double **)malloc((nchems+1) * sizeof(double *));	//initial chemical mass by layer overland (kg)
	finalchemmassov = (double **)malloc((nchems+1) * sizeof(double *));	//final chemical mass by layer overland (kg)
	initialchemov = (double ****)malloc((nchems+1) * sizeof(double ***));	//initial chemical mass in an overland cell by layer (kg)
	finalchemov = (double ****)malloc((nchems+1) * sizeof(double ***));	//final chemical mass in an overland cell by layer (kg)

	//loop over number of chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Allocate additional memory for total mass by cell variables
		initialchemov[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//initial chemical mass in an overland cell by layer (kg)
		finalchemov[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//final chemical mass in an overland cell by layer (kg)

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate additional memory for overland chemical mass by cell (values are initialized in ComputeInitialStateChemical)
			initialchemov[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//initial chemical mass in an overland cell by layer (kg)
			finalchemov[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));		//final chemical mass in an overland cell by layer (kg)

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain (imask != nodatavalue)
				if(imask[i][j] != nodatavalue)
				{
					//Allocate remaining memory for overland chemical mass by cell (values are initialized in ComputeInitialStateChemical)
					initialchemov[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//initial chemical mass in an overland cell by layer (kg)
					finalchemov[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));		//final chemical mass in an overland cell by layer (kg)

					//loop over layers (start with Layer 0 for water column)
					for(k=0; k<=maxstackov; k++)
					{
						//initialize initial chemical mass in each overland cell by layer (kg)
						initialchemov[ichem][i][j][k] = 0.0;

						//initialize final chemical mass in each overland cell by layer (kg)
						finalchemov[ichem][i][j][k] = 0.0;

					}	//end loop over layers

				}	//end if imask != nodatavalue

			}	//end loop over columns

		}	//end loop over rows

		//Allocate remaining memory for cumulative overland chemical mass variables
		initialchemmassov[ichem] = (double *)malloc((maxstackov+1) * sizeof(double));	//initial chemical mass by layer overland (kg)
		finalchemmassov[ichem] = (double *)malloc((maxstackov+1) * sizeof(double));	//final chemical mass by layer overland (kg)

		//loop over layers (start with Layer 0 for water column)
		for(k=0; k<=maxstackov; k++)
		{
			//initialize initial cumulative chemical mass in the overland plane by layer (kg)
			initialchemmassov[ichem][k] = 0.0;

			//initialize final cumulative chemical mass in the overland plane by layer (kg)
			finalchemmassov[ichem][k] = 0.0;

		}	//end loop over layers

	}	//end loop over chemicals

	//if channels are simulated
	if(chnopt == 1)
	{
		//Allocate memory for channel chemical mass variables
		initialchemmassch = (double **)malloc((nchems+1) * sizeof(double *));		//initial chemical mass in layers of channels (kg)
		finalchemmassch = (double **)malloc((nchems+1) * sizeof(double *));		//final chemical mass in layers of channel (kg)
		initialchemch = (double ****)malloc((nchems+1) * sizeof(double ***));		//initial chemical mass in a channel node by layer (kg)
		finalchemch = (double ****)malloc((nchems+1) * sizeof(double ***));		//final chemical mass in a channel node by layer (kg)

		//loop over number of chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Allocate additional memory for channel chemical mass by node variables
			initialchemch[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//initial chemical mass in a channel node by layer (kg)
			finalchemch[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//final chemical mass in a channel node by layer (kg)

			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//Allocate remaining memory for channel chemical mass by node (values are initialized in ComputeInitialVolume)
				initialchemch[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));		//initial chemical mass in a channel node by layer (kg)
				finalchemch[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));		//final chemical mass in a channel node by layer (kg)

				//loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Allocate remaining memory for channel chemical mass by node (values are initialized in ComputeInitialVolume)
					initialchemch[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//initial chemical mass in a channel node by layer (kg)
					finalchemch[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));		//final chemical mass in a channel node by layer (kg)

					//loop over layers (start with Layer 0 for water column)
					for(k=0; k<=maxstackch; k++)
					{
						//initial chemical mass in each channel node by layer (kg)
						initialchemch[ichem][i][j][k] = 0.0;

						//final chemical mass in each channel node by layer (kg)
						finalchemch[ichem][i][j][k] = 0.0;

					}	//end loop over layers

				}	//end loop over nodes

			}	//end loop over links

			//Allocate remaining memory for cumulative channel chemical mass variables
			initialchemmassch[ichem] = (double *)malloc((maxstackch+1) * sizeof(double));	//initial chemical mass by layer in channels (kg)
			finalchemmassch[ichem] = (double *)malloc((maxstackch+1) * sizeof(double));	//final chemical mass by layer in channels (kg)

			//loop over layers (start with Layer 0 for water column)
			for(k=0; k<=maxstackch; k++)
			{
				//initialize cumulative initial chemical mass in the channel network by layer (kg)
				initialchemmassch[ichem][k] = 0.0;

				//initialize cumulative final chemical mass in the channel network by layer (kg)
				finalchemmassch[ichem][k] = 0.0;

			}	//end loop over layers

		}	//end loop over chemicals

	}	//end if chnopt == 1

	/**************************************************************************/
	/*        Initialize Global Overland Chemical Transport Variables         */
	/**************************************************************************/

	//Allocate memory for chemical transport parameters indexed by chemical type, row, column, and source/layer
	cchemovnew = (float ****)malloc((nchems+1) * sizeof(float ***));		//new overland chemical concentration (for next time t+dt) (g/m3)
	advchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a cell by advection (g/s)
	advchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a cell by advection (g/s)
	dspchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a cell by dispersion (g/s)
	dspchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a cell by dispersion (g/s)
	depchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a cell by deposition (g/s)
	depchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a cell by deposition (g/s)
	erschemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a cell by erosion (g/s)
	erschemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a cell by erosion (g/s)
	pwrchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a cell by porewater (g/s)
	pwrchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a cell by porewater (g/s)
	infchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into of a cell by infiltration (g/s)
	infchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a cell by infiltration (g/s)

	biochemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//biodegredation influx of chemical in overland plane [ichem][row][col][layer]
	biochemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//biodegredation outflux of chemical in overland plane [ichem][row][col][layer]
	hydchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//hydrolysis influx of chemical in overland plane [ichem][row][col][layer]
	hydchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//hydrolysis outflux of chemical in overland plane [ichem][row][col][layer]
	oxichemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//oxidation influx of chemical in overland plane [ichem][row][col][layer]
	oxichemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//oxidation outflux of chemical in overland plane [ichem][row][col][layer]
	phtchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//photolysis influx of chemical in overland plane [ichem][row][col][layer]
	phtchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//photolysis outflux of chemical in overland plane [ichem][row][col][layer]
	radchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//radioactive decay influx of chemical in overland plane [ichem][row][col][layer]
	radchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//radioactive decay outflux of chemical in overland plane [ichem][row][col][layer]
	vltchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//volatilization influx of chemical in overland plane [ichem][row][col][layer]
	vltchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//volatilization outflux of chemical in overland plane [ichem][row][col][layer]
	udrchemovinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//user-defined reaction influx of chemical in overland plane [ichem][row][col][layer]
	udrchemovoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//user-defined reaction outflux of chemical in overland plane [ichem][row][col][layer]

	advchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//cumulative gross chemical mass transported into a cell by advection (kg)
	advchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a cell by advection (kg)
	dspchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//cumulative gross chemical mass transported into a cell by dispersion (kg)
	dspchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a cell by dispersion (kg)
	depchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//cumulative gross chemical mass transported into a cell by deposition (kg)
	depchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a cell by deposition (kg)
	erschemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//cumulative gross chemical mass transported into a cell by erosion (kg)
	erschemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a cell by erosion (kg)
	pwrchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//cumulative gross chemical mass transported into a cell by porewater (kg)
	pwrchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a cell by porewater (kg)
	burchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//cumulative gross chemical mass transported into a cell by burial (kg)
	burchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a cell by burial (kg)
	scrchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//cumulative gross chemical mass transported into a cell by scour (kg)
	scrchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a cell by scour (kg)
	infchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//cumulative gross chemical mass transported into of a cell by infiltration (kg)
	infchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into of a cell by infiltration (kg)

	biochemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//biodegredation inflowing mass of chemical in overland plane [ichem][row][col][layer]
	biochemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//biodegredation outflowing mass of chemical in overland plane [ichem][row][col][layer]
	hydchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//hydrolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
	hydchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//hydrolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
	oxichemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//oxidation inflowing mass of chemical in overland plane [ichem][row][col][layer]
	oxichemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//oxidation outflowing mass of chemical in overland plane [ichem][row][col][layer]
	phtchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//photolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
	phtchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//photolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
	radchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//radioactive decay inflowing mass of chemical in overland plane [ichem][row][col][layer]
	radchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//radioactive decay outflowing mass of chemical in overland plane [ichem][row][col][layer]
	vltchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//volatilization inflowing mass of chemical in overland plane [ichem][row][col][layer]
	vltchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//volatilization outflowing mass of chemical in overland plane [ichem][row][col][layer]
	udrchemovinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//user-defined reaction inflowing mass of chemical in overland plane [ichem][row][col][layer]
	udrchemovoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//user-defined reaction outflowing mass of chemical in overland plane [ichem][row][col][layer]

	//Allocate memory for overland mass tracking parameters
	totalerscheminov = (double *)malloc((nchems+1) * sizeof(double));	//cumulative gross chemical mass transported into the overland water column by erosion (kg)
	totalpwrcheminov = (double *)malloc((nchems+1) * sizeof(double));	//cumulative gross chemical mass transported into the overland water column by porewater release (kg)
	totaldepchemoutov = (double *)malloc((nchems+1) * sizeof(double));	//cumulative gross chemical mass transported out of the overland water column by deposition (kg)
	totalinfchemoutov = (double **)malloc((nchems+1) * sizeof(double *));	//cumulative gross chemical mass transported out of the overland water column or sediment bed by infiltration (kg)
	totaltnscheminov = (double *)malloc((nchems+1) * sizeof(double));	//cumulative gross chemical mass transported into the overland water column by chemical kinetics (kg)
	totaltnschemoutov = (double *)malloc((nchems+1) * sizeof(double));	//cumulative gross chemical mass transported out of the overland water column by chemical kinetics (kg)
	totalcwov = (double *)malloc((nchems+1) * sizeof(double));			//cumulative net chemical mass entering the channel water column from an external source (kg)

	//loop over number of chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Allocate remaining memory for overland mass tracking parameters indexed by layer
		totalinfchemoutov[ichem] = (double *)malloc(2 * sizeof(double));	//cumulative gross chemical mass transported out of the overland water column or sediment bed by infiltration (kg)

		//Initialize cumulative sum of external chemical mass entering overland plane (kg)
		totalcwov[ichem] = 0.0;

		//Initialize cumulative gross chemical mass entering overland water column by erosion (kg)
		totalerscheminov[ichem] = 0.0;

		//Initialize cumulative gross chemical mass entering overland water column by porewater release (kg)
		totalpwrcheminov[ichem] = 0.0;

		//Initialize cumulative gross chemical mass exiting overland water column by deposition (kg)
		totaldepchemoutov[ichem] = 0.0;

		//Initialize cumulative gross chemical mass exiting overland water column by infiltration (kg)
		totalinfchemoutov[ichem][0] = 0.0;

		//Initialize cumulative gross chemical mass exiting surface soil layer by infiltration (kg)
		totalinfchemoutov[ichem][1] = 0.0;

		//Initialize cumulative gross chemical mass entering overland water column by chemical kinetics (kg)
		totaltnscheminov[ichem] = 0.0;

		//Initialize cumulative gross chemical mass exiting overland water column by chemical kinetics (kg)
		totaltnschemoutov[ichem] = 0.0;

		//Allocate additional memory for chemical transport parameters indexed by chemical type, row, column, and source/layer
		cchemovnew[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));		//new overland chemical concentration (for next time t+dt) (g/m3)
		advchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported into a cell by advection (g/s)
		advchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported out of a cell by advection (g/s)
		dspchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported into a cell by dispersion (g/s)
		dspchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported out of a cell by dispersion (g/s)
		depchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported into a cell by deposition (g/s)
		depchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported out of a cell by deposition (g/s)
		erschemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported into a cell by erosion (g/s)
		erschemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported out of a cell by erosion (g/s)
		pwrchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported into a cell by porewater (g/s)
		pwrchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported out of a cell by porewater (g/s)
		infchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported into of a cell by infiltration (g/s)
		infchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross chemical flux transported out of a cell by infiltration (g/s)

		biochemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//biodegradation influx of chemical in overland plane [ichem][row][col][layer]
		biochemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//biodegradation outflux of chemical in overland plane [ichem][row][col][layer]
		hydchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//hydrolysis influx of chemical in overland plane [ichem][row][col][layer]
		hydchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//hydrolysis outflux of chemical in overland plane [ichem][row][col][layer]
		oxichemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//oxidation influx of chemical in overland plane [ichem][row][col][layer]
		oxichemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//oxidation outflux of chemical in overland plane [ichem][row][col][layer]
		phtchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//photolysis influx of chemical in overland plane [ichem][row][col][layer]
		phtchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//photolysis outflux of chemical in overland plane [ichem][row][col][layer]
		radchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//radioactive decay influx of chemical in overland plane [ichem][row][col][layer]
		radchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//radioactive decay outflux of chemical in overland plane [ichem][row][col][layer]
		vltchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//volatilization influx of chemical in overland plane [ichem][row][col][layer]
		vltchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//volatilization outflux of chemical in overland plane [ichem][row][col][layer]
		udrchemovinflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//user-defined reaction influx of chemical in overland plane [ichem][row][col][layer]
		udrchemovoutflux[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));	//user-defined reaction outflux of chemical in overland plane [ichem][row][col][layer]

		advchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//cumulative gross chemical mass transported into a cell by advection (kg)
		advchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a cell by advection (kg)
		dspchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//cumulative gross chemical mass transported into a cell by dispersion (kg)
		dspchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a cell by dispersion (kg)
		depchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//cumulative gross chemical mass transported into a cell by deposition (kg)
		depchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a cell by deposition (kg)
		erschemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//cumulative gross chemical mass transported into a cell by erosion (kg)
		erschemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a cell by erosion (kg)
		pwrchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//cumulative gross chemical mass transported into a cell by porewater (kg)
		pwrchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a cell by porewater (kg)
		burchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//cumulative gross chemical mass transported into a cell by burial (kg)
		burchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a cell by burial (kg)
		scrchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//cumulative gross chemical mass transported into a cell by scour (kg)
		scrchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a cell by scour (kg)
		infchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//cumulative gross chemical mass transported into of a cell by infiltration (kg)
		infchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross chemical mass transported into of a cell by infiltration (kg)

		biochemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//biodegradation inflowing mass of chemical in overland plane [ichem][row][col][layer]
		biochemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//biodegradation outflowing mass of chemical in overland plane [ichem][row][col][layer]
		hydchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//hydrolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
		hydchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//hydrolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
		oxichemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//oxidation inflowing mass of chemical in overland plane [ichem][row][col][layer]
		oxichemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//oxidation outflowing mass of chemical in overland plane [ichem][row][col][layer]
		phtchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//photolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
		phtchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//photolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
		radchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//radioactive decay inflowing mass of chemical in overland plane [ichem][row][col][layer]
		radchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//radioactive decay outflowing mass of chemical in overland plane [ichem][row][col][layer]
		vltchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//volatilization inflowing mass of chemical in overland plane [ichem][row][col][layer]
		vltchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//volatilization outflowing mass of chemical in overland plane [ichem][row][col][layer]
		udrchemovinmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));		//user-defined reaction inflowing mass of chemical in overland plane [ichem][row][col][layer]
		udrchemovoutmass[ichem] = (double ***)malloc((nrows+1) * sizeof(double **));	//user-defined reaction outflowing mass of chemical in overland plane [ichem][row][col][layer]

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate additional memory for overland transport parameters
			cchemovnew[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));		//new overland chemical concentration (for next time t+dt) (g/m3)
			advchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported into a cell by advection (g/s)
			advchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported out of a cell by advection (g/s)
			dspchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported into a cell by dispersion (g/s)
			dspchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported out of a cell by dispersion (g/s)
			depchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported into a cell by deposition (g/s)
			depchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported out of a cell by deposition (g/s)
			erschemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported into a cell by erosion (g/s)
			erschemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported out of a cell by erosion (g/s)
			pwrchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported into a cell by porewater (g/s)
			pwrchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported out of a cell by porewater (g/s)
			infchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported into of a cell by infiltration (g/s)
			infchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross chemical flux transported out of a cell by infiltration (g/s)

			biochemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//biodegredation influx of chemical in overland plane [ichem][row][col][layer]
			biochemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//biodegredation outflux of chemical in overland plane [ichem][row][col][layer]
			hydchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//hydrolysis influx of chemical in overland plane [ichem][row][col][layer]
			hydchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//hydrolysis outflux of chemical in overland plane [ichem][row][col][layer]
			oxichemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//oxidation influx of chemical in overland plane [ichem][row][col][layer]
			oxichemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//oxidation outflux of chemical in overland plane [ichem][row][col][layer]
			phtchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//photolysis influx of chemical in overland plane [ichem][row][col][layer]
			phtchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//photolysis outflux of chemical in overland plane [ichem][row][col][layer]
			radchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//radioactive decay influx of chemical in overland plane [ichem][row][col][layer]
			radchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//radioactive decay outflux of chemical in overland plane [ichem][row][col][layer]
			vltchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//volatilization influx of chemical in overland plane [ichem][row][col][layer]
			vltchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//volatilization outflux of chemical in overland plane [ichem][row][col][layer]
			udrchemovinflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//user-defined reaction influx of chemical in overland plane [ichem][row][col][layer]
			udrchemovoutflux[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));	//user-defined reaction outflux of chemical in overland plane [ichem][row][col][layer]

			advchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported into a cell by advection (kg)
			advchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a cell by advection (kg)
			dspchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported into a cell by dispersion (kg)
			dspchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a cell by dispersion (kg)
			depchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported into a cell by deposition (kg)
			depchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a cell by deposition (kg)
			erschemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported into a cell by erosion (kg)
			erschemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a cell by erosion (kg)
			pwrchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported into a cell by porewater (kg)
			pwrchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a cell by porewater (kg)
			burchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported into a cell by burial (kg)
			burchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a cell by burial (kg)
			scrchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported into a cell by scour (kg)
			scrchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a cell by scour (kg)
			infchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported into of a cell by infiltration (kg)
			infchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross chemical mass transported into of a cell by infiltration (kg)

			biochemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//biodegradation inflowing mass of chemical in overland plane [ichem][row][col][layer]
			biochemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//biodegradation outflowing mass of chemical in overland plane [ichem][row][col][layer]
			hydchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//hydrolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
			hydchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//hydrolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
			oxichemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//oxidation inflowing mass of chemical in overland plane [ichem][row][col][layer]
			oxichemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//oxidation outflowing mass of chemical in overland plane [ichem][row][col][layer]
			phtchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//photolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
			phtchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//photolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
			radchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//radioactive decay inflowing mass of chemical in overland plane [ichem][row][col][layer]
			radchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//radioactive decay outflowing mass of chemical in overland plane [ichem][row][col][layer]
			vltchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//volatilization inflowing mass of chemical in overland plane [ichem][row][col][layer]
			vltchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//volatilization outflowing mass of chemical in overland plane [ichem][row][col][layer]
			udrchemovinmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//user-defined reaction inflowing mass of chemical in overland plane [ichem][row][col][layer]
			udrchemovoutmass[ichem][i] = (double **)malloc((ncols+1) * sizeof(double *));	//user-defined reaction outflowing mass of chemical in overland plane [ichem][row][col][layer]

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] != nodatavalue)
				{
					//Allocate additional memory for overland transport parameters
					cchemovnew[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//new overland chemical concentration (for next time t+dt) (g/m3)
					advchemovinflux[ichem][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross chemical flux transported into a cell by advection (g/s)
					advchemovoutflux[ichem][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross chemical flux transported out of a cell by advection (g/s)
					dspchemovinflux[ichem][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross chemical flux transported into a cell by dispersion (g/s)
					dspchemovoutflux[ichem][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross chemical flux transported out of a cell by dispersion (g/s)
					depchemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross chemical flux transported into a cell by deposition (g/s)
					depchemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross chemical flux transported out of a cell by deposition (g/s)
					erschemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross chemical flux transported into a cell by erosion (g/s)
					erschemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross chemical flux transported out of a cell by erosion (g/s)
					pwrchemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross chemical flux transported into a cell by porewater (g/s)
					pwrchemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross chemical flux transported out of a cell by porewater (g/s)
					infchemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross chemical flux transported into of a cell by infiltration (g/s)
					infchemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross chemical flux transported out of a cell by infiltration (g/s)

					biochemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//biodegradation influx of chemical in overland plane [ichem][row][col][layer]
					biochemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//biodegradation outflux of chemical in overland plane [ichem][row][col][layer]
					hydchemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//hydrolysis influx of chemical in overland plane [ichem][row][col][layer]
					hydchemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//hydrolysis outflux of chemical in overland plane [ichem][row][col][layer]
					oxichemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//oxidation influx of chemical in overland plane [ichem][row][col][layer]
					oxichemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//oxidation outflux of chemical in overland plane [ichem][row][col][layer]
					phtchemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//photolysis influx of chemical in overland plane [ichem][row][col][layer]
					phtchemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//photolysis outflux of chemical in overland plane [ichem][row][col][layer]
					radchemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//radioactive decay influx of chemical in overland plane [ichem][row][col][layer]
					radchemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//radioactive decay outflux of chemical in overland plane [ichem][row][col][layer]
					vltchemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//volatilization influx of chemical in overland plane [ichem][row][col][layer]
					vltchemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//volatilization outflux of chemical in overland plane [ichem][row][col][layer]
					udrchemovinflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//user-defined reaction influx of chemical in overland plane [ichem][row][col][layer]
					udrchemovoutflux[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//user-defined reaction outflux of chemical in overland plane [ichem][row][col][layer]

					advchemovinmass[ichem][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross chemical mass transported into a cell by advection (kg)
					advchemovoutmass[ichem][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross chemical mass transported out of a cell by advection (kg)
					dspchemovinmass[ichem][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross chemical mass transported into a cell by dispersion (kg)
					dspchemovoutmass[ichem][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross chemical mass transported out of a cell by dispersion (kg)
					depchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported into a cell by deposition (kg)
					depchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported out of a cell by deposition (kg)
					erschemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported into a cell by erosion (kg)
					erschemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported out of a cell by erosion (kg)
					pwrchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported into a cell by porewater (kg)
					pwrchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported out of a cell by porewater (kg)
					burchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported into a cell by burial (kg)
					burchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported out of a cell by burial (kg)
					scrchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported into a cell by scour (kg)
					scrchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported out of a cell by scour (kg)
					infchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported into of a cell by infiltration (kg)
					infchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross chemical mass transported into of a cell by infiltration (kg)

					biochemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
					biochemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
					hydchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
					hydchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
					oxichemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
					oxichemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
					phtchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
					phtchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
					radchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
					radchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
					vltchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
					vltchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
					udrchemovinmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
					udrchemovoutmass[ichem][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]

					//loop over flow sources
					for(k=0; k<=nsources; k++)
					{
						//Initialize cumulative overland transport flux and mass variables
						advchemovinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a cell by advection (g/s)
						advchemovinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a cell by advection (kg)
						advchemovoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a cell by advection (g/s)
						advchemovoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a cell by advection (kg)
						dspchemovinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a cell by dispersion (g/s)
						dspchemovinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a cell by dispersion (kg)
						dspchemovoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a cell by dispersion (g/s)
						dspchemovoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a cell by dispersion (kg)

					}	//end loop over flow sources

					//loop over stack layers
					for(k=0; k<=maxstackov; k++)
					{
						//Initialize cumulative overland transport flux and mass variables
						depchemovinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a cell by deposition (g/s)
						depchemovinmass[ichem][i][j][k] =  0.0;	//cumulative gross chemical mass transported into a cell by deposition (kg)
						depchemovoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a cell by deposition (g/s)
						depchemovoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a cell by deposition (kg)
						erschemovinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a cell by erosion (g/s)
						erschemovinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a cell by erosion (kg)
						erschemovoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a cell by erosion (g/s)
						erschemovoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a cell by erosion (kg)
						pwrchemovinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a cell by porewater (g/s)
						pwrchemovinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a cell by porewater (kg)
						pwrchemovoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a cell by porewater (g/s)
						pwrchemovoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a cell by porewater (kg)
						infchemovinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into of a cell by infiltration (g/s)
						infchemovinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into of a cell by infiltration (kg)
						infchemovoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a cell by infiltration (g/s)
						infchemovoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a cell by infiltration (kg)
						burchemovinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a cell by burial (kg)
						burchemovoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a cell by burial (kg)
						scrchemovinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a cell by scour (kg)
						scrchemovoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a cell by scour (kg)

						biochemovinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in overland plane [ichem][row][col][layer]
						biochemovinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
						biochemovoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in overland plane [ichem][row][col][layer]
						biochemovoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
						hydchemovinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in overland plane [ichem][row][col][layer]
						hydchemovinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
						hydchemovoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in overland plane [ichem][row][col][layer]
						hydchemovoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
						oxichemovinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in overland plane [ichem][row][col][layer]
						oxichemovinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
						oxichemovoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in overland plane [ichem][row][col][layer]
						oxichemovoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
						phtchemovinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in overland plane [ichem][row][col][layer]
						phtchemovinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
						phtchemovoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in overland plane [ichem][row][col][layer]
						phtchemovoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
						radchemovinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in overland plane [ichem][row][col][layer]
						radchemovinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
						radchemovoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in overland plane [ichem][row][col][layer]
						radchemovoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
						vltchemovinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in overland plane [ichem][row][col][layer]
						vltchemovinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
						vltchemovoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in overland plane [ichem][row][col][layer]
						vltchemovoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]
						udrchemovinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in overland plane [ichem][row][col][layer]
						udrchemovinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in overland plane [ichem][row][col][layer]
						udrchemovoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in overland plane [ichem][row][col][layer]
						udrchemovoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in overland plane [ichem][row][col][layer]

					}	//end loop over stack layers

				}	//end if imask[i][j] is not nodatavalue (cell is is domain)

			}	//end loop over columns

		}	//end loop over rows

	}	//end loop over chemicals

	/*************************************************************************/
	/*        Initialize Global Channel Chemical Transport Variables         */
	/*************************************************************************/

	//if channels are simulated
	if(chnopt == 1)
	{
		//Allocate memory for channel chemical transport parameters indexed by link and node (and chemical type and source/layer)
		cchemchnew = (float ****)malloc((nchems+1) * sizeof(float ***));		//new channel chemical concentration (for next time t+dt) (g/m3)
		advchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a node by advection (g/s)
		advchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a node by advection (g/s)
		dspchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a node by dispersion (g/s)
		dspchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a node by dispersion (g/s)
		depchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a node by deposition (g/s)
		depchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a node by deposition (g/s)
		erschemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a node by erosion (g/s)
		erschemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a node by erosion (g/s)
		pwrchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into a node by porewater (g/s)
		pwrchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a node by porewater (g/s)
		infchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported into of a node by infiltration (g/s)
		infchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//gross chemical flux transported out of a node by infiltration (g/s)

		biochemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//biodegredation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
		biochemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//biodegredation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
		hydchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//hydrolysis influx of chemical in the channel network (g/s) [ichem][link][node][layer]
		hydchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//hydrolysis outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
		oxichemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//oxidation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
		oxichemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//oxidation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
		phtchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//photolysis influx of chemical in the channel network (g/s) [ichem][link][node][layer]
		phtchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//photolysis outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
		radchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//radioactive decay influx of chemical in the channel network (g/s) [ichem][link][node][layer]
		radchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//radioactive decay outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
		vltchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//volatilization influx of chemical in the channel network (g/s) [ichem][link][node][layer]
		vltchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//volatilization outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
		udrchemchinflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//user-defined reaction influx of chemical in the channel network (g/s) [ichem][link][node][layer]
		udrchemchoutflux = (float ****)malloc((nchems+1) * sizeof(float ***));	//user-defined reaction outflux of chemical in the channel network (g/s) [ichem][link][node][layer]

		advchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into a node by advection (kg)
		advchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a node by advection (kg)
		dspchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into a node by dispersion (kg)
		dspchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a node by dispersion (kg)
		depchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into a node by deposition (kg)
		depchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a node by deposition (kg)
		erschemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into a node by erosion (kg)
		erschemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a node by erosion (kg)
		pwrchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into a node by porewater (kg)
		pwrchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a node by porewater (kg)
		burchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into a node by burial (kg)
		burchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a node by burial (kg)
		scrchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into a node by scour (kg)
		scrchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported out of a node by scour (kg)
		infchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into of a node by infiltration (kg)
		infchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//cumulative gross chemical mass transported into of a node by infiltration (kg)

		biochemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//biodegredation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		biochemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//biodegredation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		hydchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//hydrolysis inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		hydchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//hydrolysis outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		oxichemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//oxidation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		oxichemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//oxidation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		phtchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//photolysis inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		phtchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//photolysis outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		radchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//radioactive decay inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		radchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//radioactive decay outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		vltchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//volatilization inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		vltchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//volatilization outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		udrchemchinmass = (double ****)malloc((nchems+1) * sizeof(double ***));		//user-defined reaction inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
		udrchemchoutmass = (double ****)malloc((nchems+1) * sizeof(double ***));	//user-defined reaction outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]

		//Allocate memory for channel mass tracking parameters
		totalerscheminch = (double *)malloc((nchems+1) * sizeof(double));		//cumulative gross chemical mass transported into the channel water column by erosion (kg)
		totalpwrcheminch = (double *)malloc((nchems+1) * sizeof(double));		//cumulative gross chemical mass transported into the channel water column by porewater release (kg)
		totaldepchemoutch = (double *)malloc((nchems+1) * sizeof(double));	//cumulative gross chemical mass transported out of the channel water column by deposition (kg)
		totalinfchemoutch = (double **)malloc((nchems+1) * sizeof(double *));	//cumulative gross chemical mass transported out of the channel water column or surface sediment by infiltration (kg)
		totaltnscheminch = (double *)malloc((nchems+1) * sizeof(double));	//cumulative gross chemical mass transported into the channel water column by chemical kinetics (kg)
		totaltnschemoutch = (double *)malloc((nchems+1) * sizeof(double));	//cumulative gross chemical mass transported out of the channel water column by chemical kinetics (kg)
		totalcwch = (double *)malloc((nchems+1) * sizeof(double));			//cumulative net chemical mass entering the channel water column from an external source (kg)

		//loop over number of chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Allocate remaining memory for channel mass tracking parameters indexed by layer
			totalinfchemoutch[ichem] = (double *)malloc(2 * sizeof(double));	//cumulative gross chemical mass transported out of the channel water column or surface sediment by infiltration (kg)

			//Initialize cumulative sum of external chemical mass entering channel network (kg)
			totalcwch[ichem] = 0.0;

			//Initialize cumulative gross chemical mass entering channel water column by erosion (kg)
			totalerscheminch[ichem] = 0.0;

			//Initialize cumulative gross chemical mass entering channel water column by porewater release (kg)
			totalpwrcheminch[ichem] = 0.0;

			//Initialize cumulative gross chemical mass exiting channel water column by deposition (kg)
			totaldepchemoutch[ichem] = 0.0;

			//Initialize cumulative gross chemical mass exiting channel water column by infiltration (kg)
			totalinfchemoutch[ichem][0] = 0.0;

			//Initialize cumulative gross chemical mass exiting surface sediment layer by infiltration (kg)
			totalinfchemoutch[ichem][1] = 0.0;

			//Initialize cumulative gross chemical mass entering channel water column by chemical kinetics (kg)
			totaltnscheminch[ichem] = 0.0;

			//Initialize cumulative gross chemical mass exiting channel water column by chemical kinetics (kg)
			totaltnschemoutch[ichem] = 0.0;

			//Allocate additional memory for channel chemical transport parameters indexed by link and node (and chemical type and source/layer)
			cchemchnew[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));		//new channel chemical concentration (for next time t+dt) (g/m3)
			advchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported into a node by advection (g/s)
			advchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported out of a node by advection (g/s)
			dspchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported into a node by dispersion (g/s)
			dspchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported out of a node by dispersion (g/s)
			depchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported into a node by deposition (g/s)
			depchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported out of a node by deposition (g/s)
			erschemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported into a node by erosion (g/s)
			erschemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported out of a node by erosion (g/s)
			pwrchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported into a node by porewater (g/s)
			pwrchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported out of a node by porewater (g/s)
			infchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported into of a node by infiltration (g/s)
			infchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross chemical flux transported out of a node by infiltration (g/s)

			biochemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
			biochemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
			hydchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
			hydchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
			oxichemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
			oxichemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
			phtchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
			phtchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
			radchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
			radchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
			vltchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
			vltchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
			udrchemchinflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
			udrchemchoutflux[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]

			advchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported into a node by advection (kg)
			advchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a node by advection (kg)
			dspchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported into a node by dispersion (kg)
			dspchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a node by dispersion (kg)
			depchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported into a node by deposition (kg)
			depchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a node by deposition (kg)
			erschemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported into a node by erosion (kg)
			erschemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a node by erosion (kg)
			pwrchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported into a node by porewater (kg)
			pwrchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a node by porewater (kg)
			burchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported into a node by burial (kg)
			burchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a node by burial (kg)
			scrchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported into a node by scour (kg)
			scrchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported out of a node by scour (kg)
			infchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported into of a node by infiltration (kg)
			infchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross chemical mass transported into of a node by infiltration (kg)

			biochemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			biochemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			hydchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			hydchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			oxichemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			oxichemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			phtchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			phtchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			radchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			radchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			vltchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			vltchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			udrchemchinmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
			udrchemchoutmass[ichem] = (double ***)malloc((nlinks+1) * sizeof(double **));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]

			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//Allocate additional memory for channel chemical transport parameters
				cchemchnew[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//new channel chemical concentration (for next time t+dt) (g/m3)
				advchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported into a node by advection (g/s)
				advchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported out of a node by advection (g/s)
				dspchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported into a node by dispersion (g/s)
				dspchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported out of a node by dispersion (g/s)
				depchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported into a node by deposition (g/s)
				depchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported out of a node by deposition (g/s)
				erschemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported into a node by erosion (g/s)
				erschemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported out of a node by erosion (g/s)
				pwrchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported into a node by porewater (g/s)
				pwrchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported out of a node by porewater (g/s)
				infchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported into of a node by infiltration (g/s)
				infchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross chemical flux transported out of a node by infiltration (g/s)

				biochemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
				biochemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
				hydchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
				hydchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
				oxichemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
				oxichemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
				phtchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
				phtchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
				radchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
				radchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
				vltchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
				vltchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
				udrchemchinflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
				udrchemchoutflux[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]

				advchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported into a node by advection (kg)
				advchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a node by advection (kg)
				dspchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported into a node by dispersion (kg)
				dspchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a node by dispersion (kg)
				depchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported into a node by deposition (kg)
				depchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a node by deposition (kg)
				erschemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported into a node by erosion (kg)
				erschemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a node by erosion (kg)
				pwrchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported into a node by porewater (kg)
				pwrchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a node by porewater (kg)
				burchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported into a node by burial (kg)
				burchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a node by burial (kg)
				scrchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported into a node by scour (kg)
				scrchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported out of a node by scour (kg)
				infchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported into of a node by infiltration (kg)
				infchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross chemical mass transported into of a node by infiltration (kg)

				biochemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				biochemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				hydchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				hydchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				oxichemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				oxichemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				phtchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				phtchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				radchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				radchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				vltchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				vltchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				udrchemchinmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
				udrchemchoutmass[ichem][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]

				//Loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Allocate additional memory for channel chemical transport parameters
					cchemchnew[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//new channel chemical concentration (for next time t+dt) (g/m3)
					advchemchinflux[ichem][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross chemical flux transported into a node by advection (g/s)
					advchemchoutflux[ichem][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross chemical flux transported out of a node by advection (g/s)
					dspchemchinflux[ichem][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross chemical flux transported into a node by dispersion (g/s)
					dspchemchoutflux[ichem][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross chemical flux transported out of a node by dispersion (g/s)
					depchemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross chemical flux transported into a node by deposition (g/s)
					depchemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross chemical flux transported out of a node by deposition (g/s)
					erschemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross chemical flux transported into a node by erosion (g/s)
					erschemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross chemical flux transported out of a node by erosion (g/s)
					pwrchemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//gross chemical flux transported into a node by porewater (g/s)
					pwrchemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross chemical flux transported out of a node by porewater (g/s)
					infchemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross chemical flux transported into of a node by infiltration (g/s)
					infchemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross chemical flux transported out of a node by infiltration (g/s)

					biochemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
					biochemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
					hydchemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
					hydchemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
					oxichemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
					oxichemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
					phtchemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
					phtchemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
					radchemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
					radchemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
					vltchemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
					vltchemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
					udrchemchinflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
					udrchemchoutflux[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]

					advchemchinmass[ichem][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross chemical mass transported into a node by advection (kg)
					advchemchoutmass[ichem][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross chemical mass transported out of a node by advection (kg)
					dspchemchinmass[ichem][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross chemical mass transported into a node by dispersion (kg)
					dspchemchoutmass[ichem][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross chemical mass transported out of a node by dispersion (kg)
					depchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported into a node by deposition (kg)
					depchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported out of a node by deposition (kg)
					erschemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported into a node by erosion (kg)
					erschemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported out of a node by erosion (kg)
					pwrchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));		//cumulative gross chemical mass transported into a node by porewater (kg)
					pwrchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported out of a node by porewater (kg)
					burchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported into a node by burial (kg)
					burchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported out of a node by burial (kg)
					scrchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported into a node by scour (kg)
					scrchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported out of a node by scour (kg)
					infchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported into of a node by infiltration (kg)
					infchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross chemical mass transported into of a node by infiltration (kg)

					biochemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					biochemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					hydchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					hydchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					oxichemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					oxichemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					phtchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					phtchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					radchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					radchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					vltchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					vltchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					udrchemchinmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
					udrchemchoutmass[ichem][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]

					//loop over flow sources
					for(k=0; k<=nsources; k++)
					{
						//Initialize cumulative channel transport flux and mass variables
						advchemchinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a node by advection (g/s)
						advchemchinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a node by advection (kg)
						advchemchoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a node by advection (g/s)
						advchemchoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a node by advection (kg)
						dspchemchinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a node by dispersion (g/s)
						dspchemchinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a node by dispersion (kg)
						dspchemchoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a node by dispersion (g/s)
						dspchemchoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a node by dispersion (kg)

					}	//end loop over flow sources

					//loop over stack layers
					for(k=0; k<=maxstackch; k++)
					{
						//Initialize cumulative channel transport flux and mass variables
						depchemchinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a node by deposition (g/s)
						depchemchinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a node by deposition (kg)
						depchemchoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a node by deposition (g/s)
						depchemchoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a node by deposition (kg)
						erschemchinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a node by erosion (g/s)
						erschemchinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a node by erosion (kg)
						erschemchoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a node by erosion (g/s)
						erschemchoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a node by erosion (kg)
						pwrchemchinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into a node by porewater (g/s)
						pwrchemchinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a node by porewater (kg)
						pwrchemchoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a node by porewater (g/s)
						pwrchemchoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a node by porewater (kg)
						burchemchinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a node by burial (kg)
						burchemchoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a node by burial (kg)
						scrchemchinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into a node by scour (kg)
						scrchemchoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported out of a node by scour (kg)
						infchemchinflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported into of a node by infiltration (g/s)
						infchemchinmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into of a node by infiltration (kg)
						infchemchoutflux[ichem][i][j][k] = 0.0;	//gross chemical flux transported out of a node by infiltration (g/s)
						infchemchoutmass[ichem][i][j][k] = 0.0;	//cumulative gross chemical mass transported into of a node by infiltration (kg)

						biochemchinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
						biochemchinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						biochemchoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
						biochemchoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						hydchemchinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
						hydchemchinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						hydchemchoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
						hydchemchoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						oxichemchinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
						oxichemchinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						oxichemchoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
						oxichemchoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						phtchemchinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
						phtchemchinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						phtchemchoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
						phtchemchoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						radchemchinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
						radchemchinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						radchemchoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
						radchemchoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						vltchemchinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
						vltchemchinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						vltchemchoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
						vltchemchoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						udrchemchinflux[ichem][i][j][k] = 0.0;	//transformation influx of chemical in the channel network (g/s) [ichem][link][node][layer]
						udrchemchinmass[ichem][i][j][k] = 0.0;	//transformation inflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]
						udrchemchoutflux[ichem][i][j][k] = 0.0;	//transformation outflux of chemical in the channel network (g/s) [ichem][link][node][layer]
						udrchemchoutmass[ichem][i][j][k] = 0.0;	//transformation outflowing mass of chemical in the channel network (kg) [ichem][link][node][layer]

					}	//end loop over stack layers

				}	//end loop over nodes

			}	//end loop over links

		}	//end loop over chemical

	}	//end if chnopt = 1

	/**********************************************************************/
	/*        Initialize Global Chemical Reporting Station Arrays         */
	/**********************************************************************/

	//Allocate memory for reported chemical discharges (export)
	totchemreportov = (float **)malloc((nchems+1) * sizeof(float *));	//reported overland total chemical discharge (units vary)
	dischemreportov = (float **)malloc((nchems+1) * sizeof(float *));	//reported overland dissolved chemical discharge (units vary)
	bndchemreportov = (float **)malloc((nchems+1) * sizeof(float *));	//reported overland bound chemical discharge (units vary)
	prtchemreportov = (float **)malloc((nchems+1) * sizeof(float *));	//reported overland particulate chemical discharge (units vary)
	totchemreportch = (float **)malloc((nchems+1) * sizeof(float *));	//reported channel total chemical discharge (units vary)
	dischemreportch = (float **)malloc((nchems+1) * sizeof(float *));	//reported channel dissolved chemical discharge (units vary)
	bndchemreportch = (float **)malloc((nchems+1) * sizeof(float *));	//reported channel bound chemical discharge (units vary)
	prtchemreportch = (float **)malloc((nchems+1) * sizeof(float *));	//reported channel particulate chemical discharge (units vary)
	chemconvert = (float *)malloc((nchemreports+1) * sizeof(float));		//chemical export conversion factor

	//Loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//Allocate memory for reported chemical discharges (export)
		totchemreportov[i] = (float *)malloc((nchemreports+1) * sizeof(float));	//reported overland total chemical discharge (units vary)
		dischemreportov[i] = (float *)malloc((nchemreports+1) * sizeof(float));	//reported overland dissolved chemical discharge (units vary)
		bndchemreportov[i] = (float *)malloc((nchemreports+1) * sizeof(float));	//reported overland bound chemical discharge (units vary)
		prtchemreportov[i] = (float *)malloc((nchemreports+1) * sizeof(float));	//reported overland particulate chemical discharge (units vary)
		totchemreportch[i] = (float *)malloc((nchemreports+1) * sizeof(float));	//reported channel total chemical discharge (units vary)
		dischemreportch[i] = (float *)malloc((nchemreports+1) * sizeof(float));	//reported channel dissolved chemical discharge (units vary)
		bndchemreportch[i] = (float *)malloc((nchemreports+1) * sizeof(float));	//reported channel bound chemical discharge (units vary)
		prtchemreportch[i] = (float *)malloc((nchemreports+1) * sizeof(float));	//reported channel particulate chemical discharge (units vary)

		//Loop over number of chemical reporting stations
		for(j=1; j<=nchemreports; j++)
		{
			//Initialize chemical reporting variables
			totchemreportov[i][j] = 0.0;
			dischemreportov[i][j] = 0.0;
			bndchemreportov[i][j] = 0.0;
			prtchemreportov[i][j] = 0.0;
			totchemreportch[i][j] = 0.0;
			dischemreportch[i][j] = 0.0;
			bndchemreportch[i][j] = 0.0;
			prtchemreportch[i][j] = 0.0;

		}	//end loop over chemical reports

	}	//end loop over chemicals

	/************************************************************/
	/*        Initialize Outlet Cell Chemical Variables         */
	/************************************************************/

	//Allocate memory for overland outlet mass tracking variables
	totaladvcheminov = (double **)malloc((nchems+1) * sizeof(double *));		//cumulative chemical mass entering domain by overland advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
	totaladvchemoutov = (double **)malloc((nchems+1) * sizeof(double *));		//cumulative chemical mass exiting domain by overland advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
	totaldspcheminov = (double **)malloc((nchems+1) * sizeof(double *));		//cumulative chemical mass entering domain by overland dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
	totaldspchemoutov = (double **)malloc((nchems+1) * sizeof(double *));		//cumulative chemical mass exiting domain by overland dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
	chemflowpeakov = (float **)malloc((nchems+1) * sizeof(float *));		//peak chemical discharge leaving an outlet (kg/s) [ichem][outlet]
	chemtimepeakov = (float **)malloc((nchems+1) * sizeof(float *));		//time of peak chemical discharge leaving an outlet (hrs) [ichem][outlet]

	//loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Allocate remaining memory for overland outlet mass tracking variables
		totaladvcheminov[ichem] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative chemical mass entering domain by overland advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		totaladvchemoutov[ichem] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative chemical mass exiting domain by overland advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		totaldspcheminov[ichem] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative chemical mass entering domain by overland dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		totaldspchemoutov[ichem] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative chemical mass exiting domain by overland dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		chemflowpeakov[ichem] = (float *)malloc((noutlets+1) * sizeof(float));		//peak chemical discharge leaving an outlet (kg/s) [ichem][oulet]
		chemtimepeakov[ichem] = (float *)malloc((noutlets+1) * sizeof(float));		//time of peak chemical discharge leaving an outlet (hrs) [ichem][outlet]

		//Loop over number of outlet cells (zeroeth element to sum over all outlets)
		for(i=0; i<=noutlets; i++)
		{
			//initialize cumulative outlet chemical mass entering by overland advection to zero (kg)
			totaladvcheminov[ichem][i] = 0.0;

			//initialize cumulative outlet chemical mass exiting by overland advection to zero (kg)
			totaladvchemoutov[ichem][i] = 0.0;

			//initialize cumulative outlet chemical mass entering by overland dispersion to zero (kg)
			totaldspcheminov[ichem][i] = 0.0;

			//initialize cumulative outlet chemical mass exiting by overland dispersion to zero (kg)
			totaldspchemoutov[ichem][i] = 0.0;

			//initialize peak overland chemical discharge to zero (kg/s)
			chemflowpeakov[ichem][i] = 0.0;

			//initialize time of peak overland chemical discharge to zero (hrs)
			chemtimepeakov[ichem][i] = 0.0;

		}	//end loop over outlets

	}	//end loop over chemicals

	//if channels are simulated
	if(chnopt > 0)
	{
		//Allocate memory for channel outlet mass tracking variables
		totaladvcheminch = (double **)malloc((nchems+1) * sizeof(double *));		//cumulative chemical mass entering domain by channel advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		totaladvchemoutch = (double **)malloc((nchems+1) * sizeof(double *));		//cumulative chemical mass exiting domain by channel advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		totaldspcheminch = (double **)malloc((nchems+1) * sizeof(double *));		//cumulative chemical mass entering domain by channel dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		totaldspchemoutch = (double **)malloc((nchems+1) * sizeof(double *));		//cumulative chemical mass exiting domain by channel dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		chemflowpeakch = (float **)malloc((nchems+1) * sizeof(float *));		//peak channel chemical discharge leaving an outlet (kg/s) [ichem][outlet]
		chemtimepeakch = (float **)malloc((nchems+1) * sizeof(float *));		//time of peak channel chemical discharge leaving an outlet (hrs) [ichem][outlet]

		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Allocate remaining memory for channel outlet mass tracking variables
			totaladvcheminch[ichem] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative chemical mass entering domain by channel advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
			totaladvchemoutch[ichem] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative chemical mass exiting domain by channel advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
			totaldspcheminch[ichem] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative chemical mass entering domain by channel dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
			totaldspchemoutch[ichem] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative chemical mass exiting domain by channel dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
			chemflowpeakch[ichem] = (float *)malloc((noutlets+1) * sizeof(float));		//peak channel chemical discharge leaving an outlet (kg/s) [ichem][outlet]
			chemtimepeakch[ichem] = (float *)malloc((noutlets+1) * sizeof(float));		//time of peak channel chemical discharge leaving an outlet (hrs) [ichem][outlet]

			//Loop over number of outlet cells (zeroeth element to sum over all outlets)
			for(i=0; i<=noutlets; i++)
			{
				//initialize cumulative outlet chemical mass entering by channel advection to zero (kg)
				totaladvcheminch[ichem][i] = 0.0;

				//initialize cumulative outlet chemical mass exiting by channel advection to zero (kg)
				totaladvchemoutch[ichem][i] = 0.0;

				//initialize cumulative outlet chemical mass entering by channel dispersion to zero (kg)
				totaldspcheminch[ichem][i] = 0.0;

				//initialize cumulative outlet chemical mass exiting by channel dispersion to zero (kg)
				totaldspchemoutch[ichem][i] = 0.0;

				//initialize peak channel chemical discharge to zero (kg/s)
				chemflowpeakch[ichem][i] = 0.0;

				//initialize time of peak channel chemical discharge to zero (hrs)
				chemtimepeakch[ichem][i] = 0.0;

			}	//end loop over outlets

		}	//end loop over chemicals

	}	//end if chnopt > 0

	/************************************************************************/
	/*        Initialize Chemical Forcing Function and BC Variables         */
	/************************************************************************/

	//Include forcing functions (loads) and boundary conditions (outlets)
	//
	//Initialize arrays related to chemical loads and boundary conditions
	//
	//Allocate initial memory for overland point source chemical loads
	ncwpovt = (float **)malloc((nchems+1) * sizeof(float *));		//next (upper) interval time for overland point source chemical load time series
	pcwpovt = (float **)malloc((nchems+1) * sizeof(float *));		//prior (lower) interval time for overland point source chemical load time series
	bcwpov = (float **)malloc((nchems+1) * sizeof(float *));		//intercept for overland point source chemical load interpolation
	mcwpov = (float **)malloc((nchems+1) * sizeof(float *));		//slope for point source chemical load interpolation
	cwpovpoint = (int **)malloc((nchems+1) * sizeof(int *));		//pointer (to position in time series) for overland point source chemical loads
	cwpovinterp = (float **)malloc((nchems+1) * sizeof(float *));	//external overland point source chemical load interpolated in time for source[i]
	ntcwpov = (float *)malloc((nchems+1) * sizeof(float));			//next time to update any overland point source load function for chemical type [i]

	//Allocate initial memory for distributed chemical loads
	ncwdovt = (float **)malloc((nchems+1) * sizeof(float *));		//next (upper) interval time for overland distributed chemical load time series
	pcwdovt = (float **)malloc((nchems+1) * sizeof(float *));		//prior (lower) interval time for overland distributed chemical load time series
	bcwdov = (float **)malloc((nchems+1) * sizeof(float *));		//intercept for overland distributed chemical load interpolation
	mcwdov = (float **)malloc((nchems+1) * sizeof(float *));		//slope for overland distributed chemical load interpolation
	cwdovpoint = (int **)malloc((nchems+1) * sizeof(int *));		//pointer (to position in time series) for overland distributed chemical loads
	cwdovinterp = (float **)malloc((nchems+1) * sizeof(float *));	//external overland distributed chemical load interpolated in time for source[i]
	ntcwdov = (float *)malloc((nchems+1) * sizeof(float));			//next time to update any overland distributed load function for chemical type [i]

	//Allocate initial memory for chemical boundary conditions
	ncbct = (float **)malloc((nchems+1) * sizeof(float *));		//next (upper) interval time for chemical bc time series
	pcbct = (float **)malloc((nchems+1) * sizeof(float *));		//prior (lower) interval time for chemical bc time series
	bcbc = (float **)malloc((nchems+1) * sizeof(float *));		//intercept for chemical bc interpolation
	mcbc = (float **)malloc((nchems+1) * sizeof(float *));		//slope for chemical bc interpolation
	cbcpoint = (int **)malloc((nchems+1) * sizeof(int *));		//pointer (to position in time series) for chemical BCs
	cbcinterp = (float **)malloc((nchems+1) * sizeof(float *));	//external chemical BC interpolated in time for outlet[i]
	ntcbc = (float *)malloc((nchems+1) * sizeof(float));		//next time to update any bc function for chemical type [i]

	//Loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//Allocate remaining memory for overland point source chemical loads
		ncwpovt[i] = (float *)malloc((ncwpov[i]+1) * sizeof(float));	//next (upper) interval time for overland point source chemical load time series
		pcwpovt[i] = (float *)malloc((ncwpov[i]+1) * sizeof(float));	//prior (lower) interval time for overland point source chemical load time series
		bcwpov[i] = (float *)malloc((ncwpov[i]+1) * sizeof(float));	//intercept for overland point source chemical load interpolation
		mcwpov[i] = (float *)malloc((ncwpov[i]+1) * sizeof(float));	//slope for overland point source chemical load interpolation
		cwpovpoint[i] = (int *)malloc((ncwpov[i]+1) * sizeof(int));	//pointer (to position in time series) for overland point source chemical loads
		cwpovinterp[i] = (float *)malloc((ncwpov[i]+1) * sizeof(float));	//external overland point source chemical load interpolated in time for source[i]

		//Allocate remaining memory for overland distributedchemical loads
		ncwdovt[i] = (float *)malloc((ncwdov[i]+1) * sizeof(float));	//next (upper) interval time for overland distributedchemical load time series
		pcwdovt[i] = (float *)malloc((ncwdov[i]+1) * sizeof(float));	//prior (lower) interval time for overland distributedchemical load time series
		bcwdov[i] = (float *)malloc((ncwdov[i]+1) * sizeof(float));	//intercept for overland distributedchemical load interpolation
		mcwdov[i] = (float *)malloc((ncwdov[i]+1) * sizeof(float));	//slope for overland distributedchemical load interpolation
		cwdovpoint[i] = (int *)malloc((ncwdov[i]+1) * sizeof(int));	//pointer (to position in time series) for overland distributedchemical loads
		cwdovinterp[i] = (float *)malloc((ncwdov[i]+1) * sizeof(float));	//external overland distributedchemical load interpolated in time for source[i]

		//Allocate remaining memory for chemical boundary
		ncbct[i] = (float *)malloc((noutlets+1) * sizeof(float));	//next (upper) interval time for chemical bc time series
		pcbct[i] = (float *)malloc((noutlets+1) * sizeof(float));	//prior (lower) interval time for chemical bc time series
		bcbc[i] = (float *)malloc((noutlets+1) * sizeof(float));	//intercept for chemical bc interpolation
		mcbc[i] = (float *)malloc((noutlets+1) * sizeof(float));	//slope for chemical bc interpolation
		cbcpoint[i] = (int *)malloc((noutlets+1) * sizeof(int));	//pointer (to position in time series) for chemical BCs
		cbcinterp[i] = (float *)malloc((noutlets+1) * sizeof(float));	//external chemical BC interpolated in time for outlet[i]

	}	//end loop over number of chemicals

	//if channels are simulated
	if(chnopt == 1)
	{
		//Initialize arrays related to chemical loads and boundary conditions
		//
		//Allocate initial memory for chemical loads
		ncwcht = (float **)malloc((nchems+1) * sizeof(float *));		//next (upper) interval time for chemical load time series
		pcwcht = (float **)malloc((nchems+1) * sizeof(float *));		//prior (lower) interval time for chemical load time series
		bcwch = (float **)malloc((nchems+1) * sizeof(float *));			//intercept for chemical load interpolation
		mcwch = (float **)malloc((nchems+1) * sizeof(float *));			//slope for chemical load interpolation
		cwchpoint = (int **)malloc((nchems+1) * sizeof(int *));			//pointer (to position in time series) for chemical loads
		cwchinterp = (float **)malloc((nchems+1) * sizeof(float *));	//external chemical load interpolated in time for source[i]
		ntcwch = (float *)malloc((nchems+1) * sizeof(float));			//next time to update any load function for chemical type [i]

		//Loop over number of chemicals
		for(i=1; i<=nchems; i++)
		{
			//Allocate remaining memory for chemical loads
			ncwcht[i] = (float *)malloc((ncwch[i]+1) * sizeof(float));	//next (upper) interval time for chemical load time series
			pcwcht[i] = (float *)malloc((ncwch[i]+1) * sizeof(float));	//prior (lower) interval time for chemical load time series
			bcwch[i] = (float *)malloc((ncwch[i]+1) * sizeof(float));	//intercept for chemical load interpolation
			mcwch[i] = (float *)malloc((ncwch[i]+1) * sizeof(float));	//slope for chemical load interpolation
			cwchpoint[i] = (int *)malloc((ncwch[i]+1) * sizeof(int));	//pointer (to position in time series) for chemical loads
			cwchinterp[i] = (float *)malloc((ncwch[i]+1) * sizeof(float));	//external chemical load interpolated in time for source[i]

		}	//end loop over number of chemicals

	}	//end if chhnopt = 1

	/************************************************************/
	/*        Initialize Chemical Export And Grid Files         */
	/************************************************************/

	//Initialize export files...
	//
	//Note:  Memory for chemical export files is allocated for each
	//       reporting group.  Sums of chemical types for each group
	//       are computed in ComputeChemicalGroupConcSums.
	//
	//if nchemreports > 0 (if there are chemical reporting stations...)
	if(nchemreports > 0)
	{
		//Allocate memory for chemical export file names
		chemexpfile = (char **)malloc(MAXNAMESIZE * sizeof(char *));

		//loop over number of chemicals
		for(i=1; i<=nchems; i++)
		{
			//Allocate remaining memory for chemical export file
			chemexpfile[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));

			//Set chemical export file name = chemical export file root name
			strcpy(chemexpfile[i], chemexprootfile);

			//Add undersore to chemical export file name
			chemexpfile[i] = strcat(chemexpfile[i], "_");

			//Add chemical name to chemical export file name
			chemexpfile[i] = strcat(chemexpfile[i], chemname[i]);

			//Add extension to chemical export file name
			chemexpfile[i] = strcat(chemexpfile[i], chemextension);

			//Set the chemical export file pointer to null
			chemexpfile_fp = NULL;

			//Set chemical export file pointer
			chemexpfile_fp = fopen(chemexpfile[i], "w");

			//if the chemical export file pointer is null, abort execution
			if(chemexpfile_fp == NULL)
			{
				//Write message to file
				fprintf(echofile_fp, "Chemical Type = %d  Can't create Chemical Export File: %s \n",
					i, chemexpfile[i]);

				//Write message to screen
				printf("Chemical Type = %d  Can't create Chemical Export File: %s \n",
					i, chemexpfile[i]);
				exit(EXIT_FAILURE);	//abort
			}

			//Write header labels to chemical export file...
			//
			//Write initial part of label
			fprintf(chemexpfile_fp, "Time (hours)");

			//Loop over number of chemical reporting stations
			for(j=1; j<=nchemreports; j++)
			{
				//Check chemical reporting units option (in reverse order)...
				//
				//if chemunitsopt = 2, chemical export reported in kg/day
				if(chemunitsopt[j] == 2)
				{
					//Compute units conversion factor
					//
					//Note: this value will need to be multiplied by discharge to
					//      complete the conversion...
					//
					//Set conversion factor to report chemical export in desired units
					chemconvert[j] = (float) (86400.0/1000.0);	//g/m3 * Q * chemconvert gives kg/day

					//Write intermediate label: total chemical
					fprintf(chemexpfile_fp, ",Qtotchemov%d (kg/day)", j);
					fprintf(chemexpfile_fp, ",Qtotchemch%d (kg/day)", j);
					fprintf(chemexpfile_fp, ",Qtotchemsum%d (kg/day)", j);

					//Write intermediate label: dissolved chemical
					fprintf(chemexpfile_fp, ",Qdischemov%d (kg/day)", j);
					fprintf(chemexpfile_fp, ",Qdischemch%d (kg/day)", j);
					fprintf(chemexpfile_fp, ",Qdischemsum%d (kg/day)", j);

					//Write intermediate label: bound chemical
					fprintf(chemexpfile_fp, ",Qbndchemov%d (kg/day)", j);
					fprintf(chemexpfile_fp, ",Qbndchemch%d (kg/day)", j);
					fprintf(chemexpfile_fp, ",Qbndchemsum%d (kg/day)", j);

					//Write intermediate label: particulate chemical
					fprintf(chemexpfile_fp, ",Qpartchemov%d (kg/day)", j);
					fprintf(chemexpfile_fp, ",Qpartchemch%d (kg/day)", j);
					fprintf(chemexpfile_fp, ",Qpartchemsum%d (kg/day)", j);
				}
				else	//else chemical export reported in g/m3 (mg/l)
				{
					//Compute units conversion factor
					//
					//Set conversion factor to report chemical export in desired units
					chemconvert[j] = 1.0;

					//Write label: total chemical
					fprintf(chemexpfile_fp, ",Ctotchemov%d (g/m3)", j);
					fprintf(chemexpfile_fp, ",Ctotchemch%d (g/m3)", j);

					//Write label: dissolved chemical
					fprintf(chemexpfile_fp, ",Cdischemov%d (g/m3)", j);
					fprintf(chemexpfile_fp, ",Cdischemch%d (g/m3)", j);

					//Write label: bound chemical
					fprintf(chemexpfile_fp, ",Cbndchemov%d (g/m3)", j);
					fprintf(chemexpfile_fp, ",Cbndchemch%d (g/m3)", j);

					//Write label: particulate chemical
					fprintf(chemexpfile_fp, ",Cpartchemov%d (g/m3)", j);
					fprintf(chemexpfile_fp, ",Cpartchemch%d (g/m3)", j);

				}	//end if chemunitsopt = 2

			}	//end loop over chemical reporting stations

			//Write final part of label
			fprintf(chemexpfile_fp,   "\n");

			//Close chemical export file
			fclose(chemexpfile_fp);

		}	//end loop over number of chemicals

	}	//end if nchemreports > 0

	//Initialize grid file names...
	//
	//Note:  Memory for chemical grid files is allocated for each
	//       reporting group.  Sums of particle types for each group
	//       are computed in ComputeChemicalGroupConcSums, etc.
	//
	//Allocate memory for chemical grid file names (total, dissolved, bound, particulate, and sorbed)
	totchemconcwatergrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	dischemconcwatergrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	bndchemconcwatergrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	prtchemconcwatergrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	srbchemconcwatergrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	totchemconcsurfgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	dischemconcsurfgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	bndchemconcsurfgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	prtchemconcsurfgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	srbchemconcsurfgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	totchemconcsubgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	dischemconcsubgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	bndchemconcsubgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	prtchemconcsubgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	srbchemconcsubgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	dischemfracwatergrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	bndchemfracwatergrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	mblchemfracwatergrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	prtchemfracwatergrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	dischemfracsurfgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	bndchemfracsurfgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	mblchemfracsurfgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	prtchemfracsurfgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	infchemfluxgrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	chemerosiongrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	chemdepositiongrid = (char **)malloc((ncgroups+1) * sizeof(char *));
	chemnetaccumgrid = (char **)malloc((ncgroups+1) * sizeof(char *));

	//loop over number of chemical reports
	for(i=1; i<=ncgroups; i++)
	{
		//Allocate remaining memory for chemical grid files
		totchemconcwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		dischemconcwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		bndchemconcwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		prtchemconcwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		srbchemconcwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		totchemconcsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		dischemconcsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		bndchemconcsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		prtchemconcsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		srbchemconcsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		totchemconcsubgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		dischemconcsubgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		bndchemconcsubgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		prtchemconcsubgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		srbchemconcsubgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		dischemfracwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		bndchemfracwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		mblchemfracwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		prtchemfracwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		dischemfracsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		bndchemfracsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		mblchemfracsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		prtchemfracsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		infchemfluxgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		chemerosiongrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		chemdepositiongrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		chemnetaccumgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Set grid file names = grid file root names
		strcpy(totchemconcwatergrid[i], totchemconcwatergridroot);
		strcpy(dischemconcwatergrid[i], dischemconcwatergridroot);
		strcpy(bndchemconcwatergrid[i], bndchemconcwatergridroot);
		strcpy(prtchemconcwatergrid[i], prtchemconcwatergridroot);
		strcpy(srbchemconcwatergrid[i], srbchemconcwatergridroot);
		strcpy(totchemconcsurfgrid[i], totchemconcsurfgridroot);
		strcpy(dischemconcsurfgrid[i], dischemconcsurfgridroot);
		strcpy(bndchemconcsurfgrid[i], bndchemconcsurfgridroot);
		strcpy(prtchemconcsurfgrid[i], prtchemconcsurfgridroot);
		strcpy(srbchemconcsurfgrid[i], srbchemconcsurfgridroot);
		strcpy(totchemconcsubgrid[i], totchemconcsubgridroot);
		strcpy(dischemconcsubgrid[i], dischemconcsubgridroot);
		strcpy(bndchemconcsubgrid[i], bndchemconcsubgridroot);
		strcpy(prtchemconcsubgrid[i], prtchemconcsubgridroot);
		strcpy(srbchemconcsubgrid[i], srbchemconcsubgridroot);
		strcpy(dischemfracwatergrid[i], dischemfracwatergridroot);
		strcpy(bndchemfracwatergrid[i], bndchemfracwatergridroot);
		strcpy(mblchemfracwatergrid[i], mblchemfracwatergridroot);
		strcpy(prtchemfracwatergrid[i], prtchemfracwatergridroot);
		strcpy(dischemfracsurfgrid[i], dischemfracsurfgridroot);
		strcpy(bndchemfracsurfgrid[i], bndchemfracsurfgridroot);
		strcpy(mblchemfracsurfgrid[i], mblchemfracsurfgridroot);
		strcpy(prtchemfracsurfgrid[i], prtchemfracsurfgridroot);
		strcpy(infchemfluxgrid[i], infchemfluxgridroot);
		strcpy(chemerosiongrid[i], chemerosiongridroot);
		strcpy(chemdepositiongrid[i], chemdepositiongridroot);
		strcpy(chemnetaccumgrid[i], chemnetaccumgridroot);

		//Add underscore to grid file names
		totchemconcwatergrid[i] = strcat(totchemconcwatergrid[i], "_");
		dischemconcwatergrid[i] = strcat(dischemconcwatergrid[i], "_");
		bndchemconcwatergrid[i] = strcat(bndchemconcwatergrid[i], "_");
		prtchemconcwatergrid[i] = strcat(prtchemconcwatergrid[i], "_");
		srbchemconcwatergrid[i] = strcat(srbchemconcwatergrid[i], "_");
		totchemconcsurfgrid[i] = strcat(totchemconcsurfgrid[i], "_");
		dischemconcsurfgrid[i] = strcat(dischemconcsurfgrid[i], "_");
		bndchemconcsurfgrid[i] = strcat(bndchemconcsurfgrid[i], "_");
		prtchemconcsurfgrid[i] = strcat(prtchemconcsurfgrid[i], "_");
		srbchemconcsurfgrid[i] = strcat(srbchemconcsurfgrid[i], "_");
		totchemconcsubgrid[i] = strcat(totchemconcsubgrid[i], "_");
		dischemconcsubgrid[i] = strcat(dischemconcsubgrid[i], "_");
		bndchemconcsubgrid[i] = strcat(bndchemconcsubgrid[i], "_");
		prtchemconcsubgrid[i] = strcat(prtchemconcsubgrid[i], "_");
		srbchemconcsubgrid[i] = strcat(srbchemconcsubgrid[i], "_");
		dischemfracwatergrid[i] = strcat(dischemfracwatergrid[i], "_");
		bndchemfracwatergrid[i] = strcat(bndchemfracwatergrid[i], "_");
		mblchemfracwatergrid[i] = strcat(mblchemfracwatergrid[i], "_");
		prtchemfracwatergrid[i] = strcat(prtchemfracwatergrid[i], "_");
		dischemfracsurfgrid[i] = strcat(dischemfracsurfgrid[i], "_");
		bndchemfracsurfgrid[i] = strcat(bndchemfracsurfgrid[i], "_");
		mblchemfracsurfgrid[i] = strcat(mblchemfracsurfgrid[i], "_");
		prtchemfracsurfgrid[i] = strcat(prtchemfracsurfgrid[i], "_");
		infchemfluxgrid[i] = strcat(infchemfluxgrid[i], "_");
		chemerosiongrid[i] = strcat(chemerosiongrid[i], "_");
		chemdepositiongrid[i] = strcat(chemdepositiongrid[i], "_");
		chemnetaccumgrid[i] = strcat(chemnetaccumgrid[i], "_");

		//Add reporting group name to grid file names
		totchemconcwatergrid[i] = strcat(totchemconcwatergrid[i], cgroupname[i]);
		dischemconcwatergrid[i] = strcat(dischemconcwatergrid[i], cgroupname[i]);
		bndchemconcwatergrid[i] = strcat(bndchemconcwatergrid[i], cgroupname[i]);
		prtchemconcwatergrid[i] = strcat(prtchemconcwatergrid[i], cgroupname[i]);
		srbchemconcwatergrid[i] = strcat(srbchemconcwatergrid[i], cgroupname[i]);
		totchemconcsurfgrid[i] = strcat(totchemconcsurfgrid[i], cgroupname[i]);
		dischemconcsurfgrid[i] = strcat(dischemconcsurfgrid[i], cgroupname[i]);
		bndchemconcsurfgrid[i] = strcat(bndchemconcsurfgrid[i], cgroupname[i]);
		prtchemconcsurfgrid[i] = strcat(prtchemconcsurfgrid[i], cgroupname[i]);
		srbchemconcsurfgrid[i] = strcat(srbchemconcsurfgrid[i], cgroupname[i]);
		totchemconcsubgrid[i] = strcat(totchemconcsubgrid[i], cgroupname[i]);
		dischemconcsubgrid[i] = strcat(dischemconcsubgrid[i], cgroupname[i]);
		bndchemconcsubgrid[i] = strcat(bndchemconcsubgrid[i], cgroupname[i]);
		prtchemconcsubgrid[i] = strcat(prtchemconcsubgrid[i], cgroupname[i]);
		srbchemconcsubgrid[i] = strcat(srbchemconcsubgrid[i], cgroupname[i]);
		dischemfracwatergrid[i] = strcat(dischemfracwatergrid[i], cgroupname[i]);
		bndchemfracwatergrid[i] = strcat(bndchemfracwatergrid[i], cgroupname[i]);
		mblchemfracwatergrid[i] = strcat(mblchemfracwatergrid[i], cgroupname[i]);
		prtchemfracwatergrid[i] = strcat(prtchemfracwatergrid[i], cgroupname[i]);
		dischemfracsurfgrid[i] = strcat(dischemfracsurfgrid[i], cgroupname[i]);
		bndchemfracsurfgrid[i] = strcat(bndchemfracsurfgrid[i], cgroupname[i]);
		mblchemfracsurfgrid[i] = strcat(mblchemfracsurfgrid[i], cgroupname[i]);
		prtchemfracsurfgrid[i] = strcat(prtchemfracsurfgrid[i], cgroupname[i]);
		infchemfluxgrid[i] = strcat(infchemfluxgrid[i], cgroupname[i]);
		chemerosiongrid[i] = strcat(chemerosiongrid[i], cgroupname[i]);
		chemdepositiongrid[i] = strcat(chemdepositiongrid[i], cgroupname[i]);
		chemnetaccumgrid[i] = strcat(chemnetaccumgrid[i], cgroupname[i]);

	}	//end loop over number of reporting groups

	/************************************************************/
	/*        Initialize Min and Max Statistics Variables       */
	/************************************************************/

	//Allocate memory for minimum and maximum overland concentrations
	maxcchemov0 = (double *)malloc((nchems+1) * sizeof(double));	//maximum chemical concentration in the overland plane water column (g/m3)
	mincchemov0 = (double *)malloc((nchems+1) * sizeof(double));	//minimum chemical concentration in the overland plane water column (g/m3)
	maxcchemov1 = (double *)malloc((nchems+1) * sizeof(double));	//maximum chemical concentration in the overland plane surface soil layer (g/m3)
	mincchemov1 = (double *)malloc((nchems+1) * sizeof(double));	//minimum chemical concentration in the overland plane surface soil layer (g/m3)

	//loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Initialize values
		maxcchemov0[ichem] = -9e30;	//maximum chemical concentration in the overland plane water column (g/m3)
		mincchemov0[ichem] = 9e30;	//minimum chemical concentration in the overland plane water column (g/m3)
		maxcchemov1[ichem] = -9e30;	//maximum chemical concentration in the overland plane surface soil layer (g/m3)
		mincchemov1[ichem] = 9e30;	//minimum chemical concentration in the overland plane surface soil layer (g/m3)

	}	//end loop over chemicals

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Allocate memory for minimum and maximum channel concentrations
		maxcchemch0 = (double *)malloc((nchems+1) * sizeof(double));	//maximum chemical concentration in the channel network water column (g/m3)
		mincchemch0 = (double *)malloc((nchems+1) * sizeof(double));	//minimum chemical concentration in the channel network water column (g/m3)
		maxcchemch1 = (double *)malloc((nchems+1) * sizeof(double));	//maximum chemical concentration in the channel network surface soil layer (g/m3)
		mincchemch1 = (double *)malloc((nchems+1) * sizeof(double));	//minimum chemical concentration in the channel network surface soil layer (g/m3)

		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Initialize values
			maxcchemch0[ichem] = -9e30;	//maximum chemical concentration in the channel network water column (g/m3)
			mincchemch0[ichem] = 9e30;	//minimum chemical concentration in the channel network water column (g/m3)
			maxcchemch1[ichem] = -9e30;	//maximum chemical concentration in the channel network surface sediment layer (g/m3)
			mincchemch1[ichem] = 9e30;	//minimum chemical concentration in the channel network surface sediment layer (g/m3)

		}	//end loop over chemicals

	}	//end if chnopt > 0

	/******************************************************/
	/*        Initialize Chemical Property Arrays         */
	/******************************************************/

	//Allocate initial memory for chemical properties
	fdissolvedov = (float ****)malloc((nchems+1) * sizeof(float ***));		//fraction dissolved of a chemical overland [ichem][row][col][layer]
	fboundov = (float ****)malloc((nchems+1) * sizeof(float ***));			//fraction bound of a chemical overland [ichem][row][col][layer]
	fparticulateov = (float *****)malloc((nchems+1) * sizeof(float ****));	//fraction particulate of a chemical overland [ichem][isolid][row][col][layer]

	//Loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Allocate additional memory for chemical properties
		fdissolvedov[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));		//fraction dissolved of a chemical overland [ichem][row][col][layer]
		fboundov[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));			//fraction bound of a chemical overland [ichem][row][col][layer]

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate additional memory for chemical properties
			fdissolvedov[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));		//fraction dissolved of a chemical overland [ichem][row][col][layer]
			fboundov[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));			//fraction bound of a chemical overland [ichem][row][col][layer]

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] != nodatavalue)
				{
					//Allocate additional memory for chemical properties
					fdissolvedov[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//fraction dissolved of a chemical overland [ichem][row][col][layer]
					fboundov[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));			//fraction bound of a chemical overland [ichem][row][col][layer]

					//loop over stack layers
					for(k=0; k<=maxstackov; k++)
					{
						//Initialize chemical properties
						fdissolvedov[ichem][i][j][k] = 1.0;
						fboundov[ichem][i][j][k] = 0.0;

					}	//end loop over stack layers

				}	//end if imask[i][j] is not nodatavalue (cell is is domain)

			}	//end loop over columns

		}	//end loop over rows

		//Allocate additional memory for chemical properties
		fparticulateov[ichem] = (float ****)malloc((nsolids+1) * sizeof(float ***));	//fraction particulate of a chemical overland [ichem][row][col][layer]

		//Loop over number of solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//Allocate additional memory for chemical properties
			fparticulateov[ichem][isolid] = (float ***)malloc((nrows+1) * sizeof(float **));	//fraction particulate of a chemical overland [ichem][row][col][layer]

			//Loop over number of rows
			for(i=1; i<=nrows; i++)
			{
				//Allocate additional memory for chemical properties
				fparticulateov[ichem][isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));	//fraction dissolved of a chemical overland [ichem][row][col][layer]

				//Loop over number of columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain
					if(imask[i][j] != nodatavalue)
					{
						//Allocate additional memory for chemical properties
						fparticulateov[ichem][isolid][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//fraction particulate of a chemical overland [ichem][row][col][layer]

						//loop over stack layers
						for(k=0; k<=maxstackov; k++)
						{
							//Initialize chemical properties
							fparticulateov[ichem][isolid][i][j][k] = 0.0;

						}	//end loop over stack layers

					}	//end if imask[i][j] is not nodatavalue (cell is is domain)

				}	//end loop over columns

			}	//end loop over rows

		}	//end loop over solids

	}	//end loop over chemicals

	//if channels are simulated
	if(chnopt == 1)
	{
		//Allocate initial memory for chemical properties
		fdissolvedch = (float ****)malloc((nchems+1) * sizeof(float ***));		//fraction dissolved of a chemical in channels [ichem][link][node][layer]
		fboundch = (float ****)malloc((nchems+1) * sizeof(float ***));			//fraction bound of a chemical in channels [ichem][link][node][layer]
		fparticulatech = (float *****)malloc((nchems+1) * sizeof(float ****));	//fraction particulate of a chemical in channels [ichem][isolid][link][node][layer]

		//Loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Allocate additional memory for chemical properties
			fdissolvedch[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));		//fraction dissolved of a chemical in channels [ichem][link][node][layer]
			fboundch[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));			//fraction bound of a chemical in channels [ichem][link][node][layer]

			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//Allocate additional memory for chemical properties
				fdissolvedch[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//fraction dissolved of a chemical in channels [ichem][link][node][layer]
				fboundch[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));			//fraction bound of a chemical in channels [ichem][link][node][layer]

				//Loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Allocate additional memory for chemical properties
					fdissolvedch[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//fraction dissolved of a chemical in channels [ichem][link][node][layer]
					fboundch[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));			//fraction bound of a chemical in channels [ichem][link][node][layer]

					//loop over stack layers
					for(k=0; k<=maxstackch; k++)
					{
						//Initialize chemical properties
						fdissolvedch[ichem][i][j][k] = 1.0;
						fboundch[ichem][i][j][k] = 0.0;

					}	//end loop over stack layers

				}	//end loop over nodes

			}	//end loop over links

			//Allocate additional memory for chemical properties
			fparticulatech[ichem] = (float ****)malloc((nsolids+1) * sizeof(float ***));	//fraction particulate of a chemical in channels [ichem][link][node][layer]

			//Loop over number of solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//Allocate additional memory for chemical properties
				fparticulatech[ichem][isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//fraction particulate of a chemical in channels [ichem][link][node][layer]

				//Loop over number of links
				for(i=1; i<=nlinks; i++)
				{
					//Allocate additional memory for chemical properties
					fparticulatech[ichem][isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//fraction dissolved of a chemical in channels [ichem][link][node][layer]

					//Loop over number of nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//Allocate additional memory for chemical properties
						fparticulatech[ichem][isolid][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//fraction particulate of a chemical in channels [ichem][link][node][layer]

						//loop over stack layers
						for(k=0; k<=maxstackch; k++)
						{
							//Initialize chemical properties
							fparticulatech[ichem][isolid][i][j][k] = 0.0;

						}	//end loop over stack layers

					}	//end loop over nodes

				}	//end loop over links

			}	//end loop over solids

		}	//end loop over chemicals

	}	//end if chnopt > 0

	/******************************************************************/
	/*        Initialize Soil/Sediment Chemical Concentrations        */
	/******************************************************************/

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain
			if(imask[i][j] > 0)
			{
				//Note:  To insure initial chemical concentrations are specified
				//       for all possible layers of the soil stack
				//Loop over soil layers
				for(k=1; k<=maxstackov; k++)
				{
					//if this layer is occupied
					if(k <= nstackov[i][j])
					{
						//Loop over chemicals
						for(ichem=1; ichem<=nchems; ichem++)
						{
							//Note:  csedov[0][][][] is the total solids concentration
							//       (sum of all solids types) for this cell/layer.
							//
							//convert from ug/g (mg/kg) to g/m3
							cchemov[ichem][i][j][k] = (float)(cchemov[ichem][i][j][k]
								* csedov[0][i][j][k] * 1.0e-6);

						}	//end loop over chemicals
					}
					else	//else the layer is not occupued (k > nstackov[][])
					{
						//Loop over chemicals
						for(ichem=1; ichem<=nchems; ichem++)
						{
							//initialize the chemical concentration to zero (g/m3)
							cchemov[ichem][i][j][k] = 0.0;

						}	//end loop over chemicals

					}	//end if k <= nstackov[i][j]

				}	//end loop over soil layers

			}	//end if imask[][] > 0

		}	//end loop over columns

	}	//end loop over rows

	//if channels are simulated
	if(chnopt > 0)
	{
		//Loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//Loop over number of nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Loop over sediment layers
				for(k=1; k<=maxstackch; k++)
				{
					//if this layer is occupied
					if(k <= nstackch[i][j])
					{
						//Loop over chemicals
						for(ichem=1; ichem<=nchems; ichem++)
						{
							//Note:  csedch[0][][][] is the total solids concentration
							//       (sum of all solids types) for this node/layer.
							//
							//convert from ug/g (mg/kg) to g/m3
							cchemch[ichem][i][j][k] = (float)(cchemch[ichem][i][j][k]
								* csedch[0][i][j][k] * 1.0e-6);

						}	//end loop over chemicals
					}
					else	//else the layer is not occupued (k > nstackch[][])
					{
						//Loop over chemicals
						for(ichem=1; ichem<=nchems; ichem++)
						{
							//initialize the chemical concentration to zero (g/m3)
							cchemch[ichem][i][j][k] = 0.0;

						}	//end loop over chemicals

					}	//end if k <= nstackch[i][j]

				}	//end loop over sediment layers

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt > 0

//End of function: Return to Initialize
}
