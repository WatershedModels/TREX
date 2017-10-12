/*----------------------------------------------------------------------
C-  Function:	InitializeSolids.c
C-
C-	Purpose/	Allocate memory for and initialize solids variables
C-  Methods:    used in computations but not read from input files.
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:   chnopt, infopt
C-
C-	Calls:		None
C-
C-	Called by:	Initialize
C-
C-	Based on:	Coding by Rosalia Rojas-Sanchez (and others)
C-              Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		19-JUN-2003 (last version)
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
C-	Revisions:  Added calculation of total solids concentrations.
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
C---------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void InitializeSolids()
{
	//Local variable declarations/definitions
	int
		isoil,		//index for soil type [i][j][ilayer]
		isolid,		//loop index for number of solids types
		nsources,	//number of transport sources/directions
		chanlink,	//channel link number
		channode;	//channel node number

	float
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf,	//surface area of channel (m)
		hsum,		//sum of layer thicknesses (m)
		densityp,	//particle density of a solids type (kg/m3)
		g,			//gravitational acceleration (9.81) (m/s2)
		nu;			//kinimatic viscosity of a sediment mixture (1.14e-6) (m2/s)

	//Define local constants...
	//
	//initialize number of transport directions/sources
	nsources = 10;

	//set gravitational acceleration = 9.81 m/s2
	g = (float)(9.81);

	//set kinimatic viscosity of dilute sediment mixture (assumed = viscosity of water) = 1.14e-6 m2/s
	nu = (float)(1.14e-6);

	//set the coefficient

	/**************************************************************/
	/*        Initialize Particle Characteristics Variables       */
	/**************************************************************/

	dstar = (float *)malloc((nsolids+1) * sizeof(float));	//particle dimensionless diameter (d*)

	//loop over number of solids
	for(isolid=1; isolid<=nsolids; isolid++)
	{
		//compute dimensionless particle diameter (d*)
		dstar[isolid] = (float)(ds[isolid] *
			pow((((spgravity[isolid] - 1.0) * g)/pow(nu, 2.0)), 0.333));

	}	//end loop over solids

	/************************************************************************/
	/*        Initialize Total Solids Concentrations for Water Column       */
	/************************************************************************/

	//Note:  The sum of all solids (TSS) is stored in the zeroeth element of
	//       the [isolids] dimension of the csedov and csedch arrays.  The
	//       total solids concentration is used in several places throughout
	//       the code.  Storing the total solids in csedov[0][i][j][ilayer]
	//       eliminates the need for repetative calculations elsewhere.
	//
	//       Only values for the water column are computed here.  Values for
	//       the soil and sediment stack are computed below along with other
	//       soil and sediment stack initializations.
	//
	//loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//loop over number of columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (imask != nodatavalue)
			if(imask[i][j] != nodatavalue)
			{
				//initialize total solids concentration for water column (g/m3)
				csedov[0][i][j][0] = 0.0;

				//loop over solids (don't include zero because it is computed inside the loop)
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//add present solids type to total concentration for this cell (g/m3)
					csedov[0][i][j][0] = csedov[0][i][j][0] + csedov[isolid][i][j][0];

				}	//end loop over solids

			}	//end if imask != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//if channels are simulated
	if(chnopt == 1)
	{
		//loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//loop over number of nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//initialize total solids concentration for water column (g/m3)
				csedch[0][i][j][0] = 0.0;

				//loop over solids (don't include zero because it is computed inside the loop)
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//add present solids type to total concentration for this node (g/m3)
					csedch[0][i][j][0] = csedch[0][i][j][0] + csedch[isolid][i][j][0];

				}	//end loop over solids

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt == 1

	/*******************************************************/
	/*        Initialize Total Solids Mass Variables       */
	/*******************************************************/

	//Allocate memory for overland solids mass variables
	initialsolidsmassov = (double **)malloc((nsolids+1) * sizeof(double *));	//initial solids mass by layer overland (kg)
	finalsolidsmassov = (double **)malloc((nsolids+1) * sizeof(double *));		//final solids mass by layer overland (kg)
	initialsolidsov = (double ****)malloc((nsolids+1) * sizeof(double ***));	//initial solids mass in an overland cell by layer (kg)
	finalsolidsov = (double ****)malloc((nsolids+1) * sizeof(double ***));		//final solids mass in an overland cell by layer (kg)

	//loop over number of solids (include zero for sum of all solids)
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Allocate additional memory for total mass by cell variables
		initialsolidsov[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//initial solids mass in an overland cell by layer (kg)
		finalsolidsov[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));		//final solids mass in an overland cell by layer (kg)

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate additional memory for overland solids mass by cell (values are initialized in ComputeInitialStateSolids)
			initialsolidsov[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//initial solids mass in an overland cell by layer (kg)
			finalsolidsov[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));		//final solids mass in an overland cell by layer (kg)

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain (imask != nodatavalue)
				if(imask[i][j] != nodatavalue)
				{
					//Allocate remaining memory for overland solids mass by cell (values are initialized in ComputeInitialStateSolids)
					initialsolidsov[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));		//initial solids mass in an overland cell by layer (kg)
					finalsolidsov[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));		//final solids mass in an overland cell by layer (kg)

					//loop over layers (start with Layer 0 for water column)
					for(k=0; k<=maxstackov; k++)
					{
						//initialize initial solids mass in each overland cell by layer (kg)
						initialsolidsov[isolid][i][j][k] = 0.0;

						//initialize final solids mass in each overland cell by layer (kg)
						finalsolidsov[isolid][i][j][k] = 0.0;

					}	//end loop over layers

				}	//end if imask != nodatavalue

			}	//end loop over columns

		}	//end loop over rows

		//Allocate remaining memory for cumulative overland solids mass variables
		initialsolidsmassov[isolid] = (double *)malloc((maxstackov+1) * sizeof(double));	//initial solids mass by layer overland (kg)
		finalsolidsmassov[isolid] = (double *)malloc((maxstackov+1) * sizeof(double));	//final solids mass by layer overland (kg)

		//loop over layers (start with Layer 0 for water column)
		for(k=0; k<=maxstackov; k++)
		{
			//initialize initial cumulative solids mass in the overland plane by layer (kg)
			initialsolidsmassov[isolid][k] = 0.0;

			//initialize final cumulative solids mass in the overland plane by layer (kg)
			finalsolidsmassov[isolid][k] = 0.0;

		}	//end loop over layers

	}	//end loop over solids

	//if channels are simulated
	if(chnopt == 1)
	{
		//Allocate memory for channel solids mass variables
		initialsolidsmassch = (double **)malloc((nsolids+1) * sizeof(double *));		//initial solids mass in layers of channels (kg)
		finalsolidsmassch = (double **)malloc((nsolids+1) * sizeof(double *));		//final solids mass in layers of channel (kg)
		initialsolidsch = (double ****)malloc((nsolids+1) * sizeof(double ***));		//initial solids mass in a channel node by layer (kg)
		finalsolidsch = (double ****)malloc((nsolids+1) * sizeof(double ***));		//final solids mass in a channel node by layer (kg)

		//loop over number of solids (include zero for sum of all solids)
		for(isolid=0; isolid<=nsolids; isolid++)
		{
			//Allocate additional memory for channel solids mass by node variables
			initialsolidsch[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//initial solids mass in a channel node by layer (kg)
			finalsolidsch[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));		//final solids mass in a channel node by layer (kg)

			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//Allocate remaining memory for channel solids mass by node (values are initialized in ComputeInitialVolume)
				initialsolidsch[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//initial solids mass in a channel node by layer (kg)
				finalsolidsch[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));		//final solids mass in a channel node by layer (kg)

				//loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Allocate remaining memory for channel solids mass by node (values are initialized in ComputeInitialVolume)
					initialsolidsch[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//initial solids mass in a channel node by layer (kg)
					finalsolidsch[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//final solids mass in a channel node by layer (kg)

					//loop over layers (start with Layer 0 for water column)
					for(k=0; k<=maxstackch; k++)
					{
						//initial solids mass in each channel node by layer (kg)
						initialsolidsch[isolid][i][j][k] = 0.0;

						//final solids mass in each channel node by layer (kg)
						finalsolidsch[isolid][i][j][k] = 0.0;

					}	//end loop over layers

				}	//end loop over nodes

			}	//end loop over links

			//Allocate remaining memory for cumulative channel solids mass variables
			initialsolidsmassch[isolid] = (double *)malloc((maxstackch+1) * sizeof(double));	//initial solids mass by layer in channels (kg)
			finalsolidsmassch[isolid] = (double *)malloc((maxstackch+1) * sizeof(double));		//final solids mass by layer in channels (kg)

			//loop over layers (start with Layer 0 for water column)
			for(k=0; k<=maxstackch; k++)
			{
				//initialize cumulative initial solids mass in the channel network by layer (kg)
				initialsolidsmassch[isolid][k] = 0.0;

				//initialize cumulative final solids mass in the channel network by layer (kg)
				finalsolidsmassch[isolid][k] = 0.0;

			}	//end loop over layers

		}	//end loop over solids

	}	//end if chnopt == 1

	/************************************************************************/
	/*        Initialize Global Overland Solids Transport Variables         */
	/************************************************************************/

	//Allocate memory for solids transport parameters indexed by solids type, row, column, and source/layer
	csedovnew = (float ****)malloc((nsolids+1) * sizeof(float ***));		//new overland solids concentration (for next time t+dt) (g/m3)
	advsedovinflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported into a cell by advection (g/s)
	advsedovoutflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported out of a cell by advection (g/s)
	dspsedovinflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported into a cell by dispersion (g/s)
	dspsedovoutflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported out of a cell by dispersion (g/s)
	depsedovinflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported into a cell by deposition (g/s)
	depsedovoutflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported out of a cell by deposition (g/s)
	erssedovinflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported into a cell by erosion (g/s)
	erssedovoutflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported out of a cell by erosion (g/s)

	advsedovinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a cell by advection (kg)
	advsedovoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a cell by advection (kg)
	dspsedovinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a cell by dispersion (kg)
	dspsedovoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a cell by dispersion (kg)
	depsedovinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a cell by deposition (kg)
	depsedovoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a cell by deposition (kg)
	erssedovinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a cell by erosion (kg)
	erssedovoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a cell by erosion (kg)
	bursedovinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a cell by burial (kg)
	bursedovoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a cell by burial (kg)
	scrsedovinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a cell by scour (kg)
	scrsedovoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a cell by scour (kg)

	transcapov = (float ***)malloc((nsolids+1) * sizeof(float **));			//transport capacity of solids in the overland plane (g/s)
	transrateov = (float ***)malloc((nsolids+1) * sizeof(float **));		//transport rate of solids in the overland plane (g/s)
	depflowov = (float ***)malloc((nsolids+1) * sizeof(float **));			//flow by deposition in the overland plane (m3/s) [row][col][direction]
	ersflowov = (float ***)malloc((nsolids+1) * sizeof(float **));			//flow by erosion in the overland plane (m3/s) [row][col][direction]

	//Allocate memory for overland mass tracking parameters
	totalerssedinov = (double *)malloc((nsolids+1) * sizeof(double));		//cumulative gross solids mass transported into the overland water column by erosion (kg)
	totaldepsedoutov = (double *)malloc((nsolids+1) * sizeof(double));	//cumulative gross solids mass transported out of the overland water column by deposition (kg)
	totalswov = (double *)malloc((nsolids+1) * sizeof(double));			//cumulative net solids mass entering the channel water column from an external source (kg)

	//loop over number of solids (include zeroeth element for total solids)
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Initialize cumulative sum of external solids mass entering overland plane (kg)
		totalswov[isolid] = 0.0;

		//Initialize cumulative gross solids mass entering overland water column by erosion (kg)
		totalerssedinov[isolid] = 0.0;

		//Initialize cumulative gross solids mass exiting overland water column by deposition (kg)
		totaldepsedoutov[isolid] = 0.0;

		//Allocate additional memory for solids transport parameters indexed by solids type, row, column, and source/layer
		csedovnew[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));		//new overland solids concentration (for next time t+dt) (g/m3)
		advsedovinflux[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross solids flux transported into a cell by advection (g/s)
		advsedovoutflux[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross solids flux transported out of a cell by advection (g/s)
		dspsedovinflux[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross solids flux transported into a cell by dispersion (g/s)
		dspsedovoutflux[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross solids flux transported out of a cell by dispersion (g/s)
		depsedovinflux[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross solids flux transported into a cell by deposition (g/s)
		depsedovoutflux[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross solids flux transported out of a cell by deposition (g/s)
		erssedovinflux[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross solids flux transported into a cell by erosion (g/s)
		erssedovoutflux[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));	//gross solids flux transported out of a cell by erosion (g/s)

		advsedovinmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported into a cell by advection (kg)
		advsedovoutmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported out of a cell by advection (kg)
		dspsedovinmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported into a cell by dispersion (kg)
		dspsedovoutmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported out of a cell by dispersion (kg)
		depsedovinmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported into a cell by deposition (kg)
		depsedovoutmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported out of a cell by deposition (kg)
		erssedovinmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported into a cell by erosion (kg)
		erssedovoutmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported out of a cell by erosion (kg)
		bursedovinmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported into a cell by burial (kg)
		bursedovoutmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported out of a cell by burial (kg)
		scrsedovinmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported into a cell by scour (kg)
		scrsedovoutmass[isolid] = (double ***)malloc((nrows+1) * sizeof(double **));	//cumulative gross solids mass transported out of a cell by scour (kg)

		transcapov[isolid] = (float **)malloc((nrows+1) * sizeof(float *));			//transport capacity of solids in the overland plane (g/s)
		transrateov[isolid] = (float **)malloc((nrows+1) * sizeof(float *));		//transport rate of solids in the overland plane (g/s)
		depflowov[isolid] = (float **)malloc((nrows+1) * sizeof(float *));			//flow by deposition in the overland plane (m3/s) [isolid][row][col]
		ersflowov[isolid] = (float **)malloc((nrows+1) * sizeof(float *));			//flow by erosion in the overland plane (m3/s) [isolid][row][col]

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate additional memory for overland transport parameters
			csedovnew[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));		//new overland solids concentration (for next time t+dt) (g/m3)
			advsedovinflux[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross solids flux transported into a cell by advection (g/s)
			advsedovoutflux[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross solids flux transported out of a cell by advection (g/s)
			dspsedovinflux[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross solids flux transported into a cell by dispersion (g/s)
			dspsedovoutflux[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross solids flux transported out of a cell by dispersion (g/s)
			depsedovinflux[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross solids flux transported into a cell by deposition (g/s)
			depsedovoutflux[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross solids flux transported out of a cell by deposition (g/s)
			erssedovinflux[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross solids flux transported into a cell by erosion (g/s)
			erssedovoutflux[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));	//gross solids flux transported out of a cell by erosion (g/s)

			advsedovinmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported into a cell by advection (kg)
			advsedovoutmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported out of a cell by advection (kg)
			dspsedovinmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported into a cell by dispersion (kg)
			dspsedovoutmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported out of a cell by dispersion (kg)
			depsedovinmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported into a cell by deposition (kg)
			depsedovoutmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported out of a cell by deposition (kg)
			erssedovinmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported into a cell by erosion (kg)
			erssedovoutmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported out of a cell by erosion (kg)
			bursedovinmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported into a cell by burial (kg)
			bursedovoutmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported out of a cell by burial (kg)
			scrsedovinmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported into a cell by scour (kg)
			scrsedovoutmass[isolid][i] = (double **)malloc((ncols+1) * sizeof(double *));	//cumulative gross solids mass transported out of a cell by scour (kg)

			transcapov[isolid][i] = (float *)malloc((ncols+1) * sizeof(float));			//transport capacity of solids in the overland plane (g/s)
			transrateov[isolid][i] = (float *)malloc((ncols+1) * sizeof(float));		//transport rate of solids in the overland plane (g/s)
			depflowov[isolid][i] = (float *)malloc((ncols+1) * sizeof(float));			//flow by deposition in the overland plane (m3/s) [isolid][row][col]
			ersflowov[isolid][i] = (float *)malloc((ncols+1) * sizeof(float));			//flow by erosion in the overland plane (m3/s) [isolid][row][col]

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] != nodatavalue)
				{
					//Initialize transport parameters
					transcapov[isolid][i][j] = 0.0;		//transport capacity of solids in the overland plane (g/s)
					transrateov[isolid][i][j] = 0.0;	//transport rate of solids in the overland plane (g/s)
					depflowov[isolid][i][j] = 0.0;		//flow by deposition in the overland plane (m3/s) [isolid][row][col]
					ersflowov[isolid][i][j] = 0.0;		//flow by erosion in the overland plane (m3/s) [isolid][row][col]

					//Allocate additional memory for overland transport parameters
					csedovnew[isolid][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//new overland solids concentration (for next time t+dt) (g/m3)
					advsedovinflux[isolid][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross solids flux transported into a cell by advection (g/s)
					advsedovoutflux[isolid][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross solids flux transported out of a cell by advection (g/s)
					dspsedovinflux[isolid][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross solids flux transported into a cell by dispersion (g/s)
					dspsedovoutflux[isolid][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross solids flux transported out of a cell by dispersion (g/s)
					depsedovinflux[isolid][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross solids flux transported into a cell by deposition (g/s)
					depsedovoutflux[isolid][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross solids flux transported out of a cell by deposition (g/s)
					erssedovinflux[isolid][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross solids flux transported into a cell by erosion (g/s)
					erssedovoutflux[isolid][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//gross solids flux transported out of a cell by erosion (g/s)

					advsedovinmass[isolid][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross solids mass transported into a cell by advection (kg)
					advsedovoutmass[isolid][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross solids mass transported out of a cell by advection (kg)
					dspsedovinmass[isolid][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross solids mass transported into a cell by dispersion (kg)
					dspsedovoutmass[isolid][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross solids mass transported out of a cell by dispersion (kg)
					depsedovinmass[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross solids mass transported into a cell by deposition (kg)
					depsedovoutmass[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross solids mass transported out of a cell by deposition (kg)
					erssedovinmass[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross solids mass transported into a cell by erosion (kg)
					erssedovoutmass[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross solids mass transported out of a cell by erosion (kg)
					bursedovinmass[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross solids mass transported into a cell by burial (kg)
					bursedovoutmass[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross solids mass transported out of a cell by burial (kg)
					scrsedovinmass[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross solids mass transported into a cell by scour (kg)
					scrsedovoutmass[isolid][i][j] = (double *)malloc((maxstackov+1) * sizeof(double));	//cumulative gross solids mass transported out of a cell by scour (kg)

					//loop over flow sources
					for(k=0; k<=nsources; k++)
					{
						//Initialize cumulative overland transport flux and mass variables
						advsedovinflux[isolid][i][j][k] = 0.0;	//gross solids flux transported into a cell by advection (g/s)
						advsedovinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a cell by advection (kg)
						advsedovoutflux[isolid][i][j][k] = 0.0;	//gross solids flux transported out of a cell by advection (g/s)
						advsedovoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a cell by advection (kg)
						dspsedovinflux[isolid][i][j][k] = 0.0;	//gross solids flux transported into a cell by dispersion (g/s)
						dspsedovinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a cell by dispersion (kg)
						dspsedovoutflux[isolid][i][j][k] = 0.0;	//gross solids flux transported out of a cell by dispersion (g/s)
						dspsedovoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a cell by dispersion (kg)

					}	//end loop over flow sources

					//loop over stack layers
					for(k=0; k<=maxstackov; k++)
					{
						//Initialize cumulative overland transport flux and mass variables
						depsedovinflux[isolid][i][j][k] = 0.0;	//gross solids flux transported into a cell by deposition (g/s)
						depsedovinmass[isolid][i][j][k] =  0.0;	//cumulative gross solids mass transported into a cell by deposition (kg)
						depsedovoutflux[isolid][i][j][k] = 0.0;	//gross solids flux transported out of a cell by deposition (g/s)
						depsedovoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a cell by deposition (kg)
						erssedovinflux[isolid][i][j][k] = 0.0;	//gross solids flux transported into a cell by erosion (g/s)
						erssedovinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a cell by erosion (kg)
						erssedovoutflux[isolid][i][j][k] = 0.0;	//gross solids flux transported out of a cell by erosion (g/s)
						erssedovoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a cell by erosion (kg)
						bursedovinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a cell by burial (kg)
						bursedovoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a cell by burial (kg)
						scrsedovinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a cell by scour (kg)
						scrsedovoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a cell by scour (kg)

					}	//end loop over stack layers

				}	//end if imask[i][j] is not nodatavalue (cell is is domain)

			}	//end loop over columns

		}	//end loop over rows

	}	//end loop over solids

	//Allocate memory for solids transport parameters indexed by row, column, and source/layer
	advinflowov = (float ***)malloc((nrows+1) * sizeof(float **));		//inflow by advection in the overland plane (m3/s) [row][col][direction]
	advoutflowov = (float ***)malloc((nrows+1) * sizeof(float **));		//outflow by advection in the overland plane (m3/s) [row][col][direction]
	dspflowov = (float ***)malloc((nrows+1) * sizeof(float **));		//flow by dispersion in the overland plane (m3/s) [row][col][direction]

	//loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate memory for solids transport parameters indexed by row, column, and source/layer
		advinflowov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//inflow by advection in the overland plane (m3/s) [row][col][direction]
		advoutflowov[i] = (float **)malloc((ncols+1) * sizeof(float *));	//outflow by advection in the overland plane (m3/s) [row][col][direction]
		dspflowov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//flow by dispersion in the overland plane (m3/s) [row][col][direction]

		//loop over columns
		for(j=1; j<=ncols; j++)
		{
			//Allocate remaining memory for solids transport parameters indexed by row, column, and source/layer
			advinflowov[i][j] = (float *)malloc((nsources+1) * sizeof(float));	//inflow by advection in the overland plane (m3/s) [row][col][direction]
			advoutflowov[i][j] = (float *)malloc((nsources+1) * sizeof(float));	//outflow by advection in the overland plane (m3/s) [row][col][direction]
			dspflowov[i][j] = (float *)malloc((nsources+1) * sizeof(float));	//flow by dispersion in the overland plane (m3/s) [row][col][direction]

			//loop over sources
			for(k=0; k<=nsources; k++)
			{
				//initialize values
				advinflowov[i][j][k] = 0.0;		//inflow by advection in the overland plane (m3/s) [row][col][direction]
				advoutflowov[i][j][k] = 0.0;	//outflow by advection in the overland plane (m3/s) [row][col][direction]
				dspflowov[i][j][k] = 0.0;		//flow by dispersion in the overland plane (m3/s) [row][col][direction]

			}	//end loop over sources

		}	//end loop over columns

	}	//end loop over rows

	/***********************************************************************/
	/*        Initialize Global Channel Solids Transport Variables         */
	/***********************************************************************/

	//if channels are simulated
	if(chnopt == 1)
	{
		//Allocate memory for channel solids transport parameters indexed by link and node (and solid type and source/layer)
		csedchnew = (float ****)malloc((nsolids+1) * sizeof(float ***));		//new channel solids concentration (for next time t+dt) (g/m3)
		advsedchinflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported into a node by advection (g/s)
		advsedchoutflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported out of a node by advection (g/s)
		dspsedchinflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported into a node by dispersion (g/s)
		dspsedchoutflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported out of a node by dispersion (g/s)
		depsedchinflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported into a node by deposition (g/s)
		depsedchoutflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported out of a node by deposition (g/s)
		erssedchinflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported into a node by erosion (g/s)
		erssedchoutflux = (float ****)malloc((nsolids+1) * sizeof(float ***));	//gross solids flux transported out of a node by erosion (g/s)

		advsedchinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a node by advection (kg)
		advsedchoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a node by advection (kg)
		dspsedchinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a node by dispersion (kg)
		dspsedchoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a node by dispersion (kg)
		depsedchinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a node by deposition (kg)
		depsedchoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a node by deposition (kg)
		erssedchinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a node by erosion (kg)
		erssedchoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a node by erosion (kg)
		bursedchinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a node by burial (kg)
		bursedchoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a node by burial (kg)
		scrsedchinmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported into a node by scour (kg)
		scrsedchoutmass = (double ****)malloc((nsolids+1) * sizeof(double ***));	//cumulative gross solids mass transported out of a node by scour (kg)

		transcapch = (float ***)malloc((nsolids+1) * sizeof(float **));			//transport capacity of solids in the channel network (g/s)
		transratech = (float ***)malloc((nsolids+1) * sizeof(float **));		//transport rate of solids in the channel network (g/s)
		depflowch = (float ***)malloc((nsolids+1) * sizeof(float **));			//flow by deposition in the channel network (m3/s) [solid][link][node]
		ersflowch = (float ***)malloc((nsolids+1) * sizeof(float **));			//flow by erosion in the channel network (m3/s) [solid][link][node]

		//Allocate memory for channel mass tracking parameters
		totalerssedinch = (double *)malloc((nsolids+1) * sizeof(double));		//cumulative gross solids mass transported into the channel water column by erosion (kg)
		totaldepsedoutch = (double *)malloc((nsolids+1) * sizeof(double));	//cumulative gross solids mass transported out of the channel water column by deposition (kg)
		totalswch = (double *)malloc((nsolids+1) * sizeof(double));			//cumulative net solids mass entering the channel water column from an external source (kg)

		//loop over number of solids (include zeroeth element for total solids)
		for(isolid=0; isolid<=nsolids; isolid++)
		{
			//Initialize cumulative sum of external solids mass entering channel network (kg)
			totalswch[isolid] = 0.0;

			//Initialize cumulative gross solids mass entering channel water column by erosion (kg)
			totalerssedinch[isolid] = 0.0;

			//Initialize cumulative gross solids mass exiting channel water column by deposition (kg)
			totaldepsedoutch[isolid] = 0.0;

			//Allocate additional memory for channel solids transport parameters indexed by link and node (and solid type and source/layer)
			csedchnew[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));		//new channel solids concentration (for next time t+dt) (g/m3)
			advsedchinflux[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross solids flux transported into a node by advection (g/s)
			advsedchoutflux[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross solids flux transported out of a node by advection (g/s)
			dspsedchinflux[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross solids flux transported into a node by dispersion (g/s)
			dspsedchoutflux[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross solids flux transported out of a node by dispersion (g/s)
			depsedchinflux[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross solids flux transported into a node by deposition (g/s)
			depsedchoutflux[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross solids flux transported out of a node by deposition (g/s)
			erssedchinflux[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross solids flux transported into a node by erosion (g/s)
			erssedchoutflux[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//gross solids flux transported out of a node by erosion (g/s)

			advsedchinmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported into a node by advection (kg)
			advsedchoutmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported out of a node by advection (kg)
			dspsedchinmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported into a node by dispersion (kg)
			dspsedchoutmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported out of a node by dispersion (kg)
			depsedchinmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported into a node by deposition (kg)
			depsedchoutmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported out of a node by deposition (kg)
			erssedchinmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported into a node by erosion (kg)
			erssedchoutmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported out of a node by erosion (kg)
			bursedchinmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported into a node by burial (kg)
			bursedchoutmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported out of a node by burial (kg)
			scrsedchinmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported into a node by scour (kg)
			scrsedchoutmass[isolid] = (double ***)malloc((nlinks+1) * sizeof(double **));	//cumulative gross solids mass transported out of a node by scour (kg)

			transcapch[isolid] = (float **)malloc((nlinks+1) * sizeof(float *));		//transport capacity of solids in the channel network (g/s)
			transratech[isolid] = (float **)malloc((nlinks+1) * sizeof(float *));		//transport rate of solids in the channel network (g/s)
			depflowch[isolid] = (float **)malloc((nlinks+1) * sizeof(float *));			//flow by deposition in the channel network (m3/s) [solid][link][node]
			ersflowch[isolid] = (float **)malloc((nlinks+1) * sizeof(float *));			//flow by erosion in the channel network (m3/s) [solid][link][node]

			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//Allocate additional memory for channel solids transport parameters
				csedchnew[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//new channel solids concentration (for next time t+dt) (g/m3)
				advsedchinflux[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross solids flux transported into a node by advection (g/s)
				advsedchoutflux[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross solids flux transported out of a node by advection (g/s)
				dspsedchinflux[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross solids flux transported into a node by dispersion (g/s)
				dspsedchoutflux[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross solids flux transported out of a node by dispersion (g/s)
				depsedchinflux[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross solids flux transported into a node by deposition (g/s)
				depsedchoutflux[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross solids flux transported out of a node by deposition (g/s)
				erssedchinflux[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross solids flux transported into a node by erosion (g/s)
				erssedchoutflux[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//gross solids flux transported out of a node by erosion (g/s)

				advsedchinmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported into a node by advection (kg)
				advsedchoutmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported out of a node by advection (kg)
				dspsedchinmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported into a node by dispersion (kg)
				dspsedchoutmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported out of a node by dispersion (kg)
				depsedchinmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported into a node by deposition (kg)
				depsedchoutmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported out of a node by deposition (kg)
				erssedchinmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported into a node by erosion (kg)
				erssedchoutmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported out of a node by erosion (kg)
				bursedchinmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported into a node by burial (kg)
				bursedchoutmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported out of a node by burial (kg)
				scrsedchinmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported into a node by scour (kg)
				scrsedchoutmass[isolid][i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//cumulative gross solids mass transported out of a node by scour (kg)

				transcapch[isolid][i] = (float *)malloc((nnodes[i]+1) * sizeof(float));			//transport capacity of solids in the channel network (g/s)
				transratech[isolid][i] = (float *)malloc((nnodes[i]+1) * sizeof(float));		//transport rate of solids in the channel network (g/s)
				depflowch[isolid][i] = (float *)malloc((nnodes[i]+1) * sizeof(float));			//flow by deposition in the channel network (m3/s) [solid][link][node]
				ersflowch[isolid][i] = (float *)malloc((nnodes[i]+1) * sizeof(float));			//flow by erosion in the channel network (m3/s) [solid][link][node]

				//Loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Initialize transport parameters
					transcapch[isolid][i][j] = 0.0;		//transport capacity of solids in the channel network (g/s)
					transratech[isolid][i][j] = 0.0;	//transport rate of solids in the channel network (g/s)
					depflowch[isolid][i][j] = 0.0;		//flow by deposition in the channel network (m3/s) [isolid][link][node]
					ersflowch[isolid][i][j] = 0.0;		//flow by erosion in the channel network (m3/s) [isolid][link][node]

					//Allocate additional memory for channel solids transport parameters
					csedchnew[isolid][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//new channel solids concentration (for next time t+dt) (g/m3)
					advsedchinflux[isolid][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross solids flux transported into a node by advection (g/s)
					advsedchoutflux[isolid][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross solids flux transported out of a node by advection (g/s)
					dspsedchinflux[isolid][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross solids flux transported into a node by dispersion (g/s)
					dspsedchoutflux[isolid][i][j] = (float *)malloc((nsources+1) * sizeof(float));	//gross solids flux transported out of a node by dispersion (g/s)
					depsedchinflux[isolid][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross solids flux transported into a node by deposition (g/s)
					depsedchoutflux[isolid][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross solids flux transported out of a node by deposition (g/s)
					erssedchinflux[isolid][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross solids flux transported into a node by erosion (g/s)
					erssedchoutflux[isolid][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//gross solids flux transported out of a node by erosion (g/s)

					advsedchinmass[isolid][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross solids mass transported into a node by advection (kg)
					advsedchoutmass[isolid][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross solids mass transported out of a node by advection (kg)
					dspsedchinmass[isolid][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross solids mass transported into a node by dispersion (kg)
					dspsedchoutmass[isolid][i][j] = (double *)malloc((nsources+1) * sizeof(double));	//cumulative gross solids mass transported out of a node by dispersion (kg)
					depsedchinmass[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross solids mass transported into a node by deposition (kg)
					depsedchoutmass[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross solids mass transported out of a node by deposition (kg)
					erssedchinmass[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross solids mass transported into a node by erosion (kg)
					erssedchoutmass[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross solids mass transported out of a node by erosion (kg)
					bursedchinmass[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross solids mass transported into a node by burial (kg)
					bursedchoutmass[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross solids mass transported out of a node by burial (kg)
					scrsedchinmass[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross solids mass transported into a node by scour (kg)
					scrsedchoutmass[isolid][i][j] = (double *)malloc((maxstackch+1) * sizeof(double));	//cumulative gross solids mass transported out of a node by scour (kg)

					//loop over flow sources
					for(k=0; k<=nsources; k++)
					{
						//Initialize cumulative channel transport flux and mass variables
						advsedchinflux[isolid][i][j][k] = 0.0;	//gross solids flux transported into a node by advection (g/s)
						advsedchinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a node by advection (kg)
						advsedchoutflux[isolid][i][j][k] = 0.0;	//gross solids flux transported out of a node by advection (g/s)
						advsedchoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a node by advection (kg)
						dspsedchinflux[isolid][i][j][k] = 0.0;	//gross solids flux transported into a node by dispersion (g/s)
						dspsedchinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a node by dispersion (kg)
						dspsedchoutflux[isolid][i][j][k] = 0.0;	//gross solids flux transported out of a node by dispersion (g/s)
						dspsedchoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a node by dispersion (kg)

					}	//end loop over flow sources

					//loop over stack layers
					for(k=0; k<=maxstackch; k++)
					{
						//Initialize cumulative channel transport flux and mass variables
						depsedchinflux[isolid][i][j][k] = 0.0;	//gross solids flux transported into a node by deposition (g/s)
						depsedchinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a node by deposition (kg)
						depsedchoutflux[isolid][i][j][k] = 0.0;	//gross solids flux transported out of a node by deposition (g/s)
						depsedchoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a node by deposition (kg)
						erssedchinflux[isolid][i][j][k] = 0.0;	//gross solids flux transported into a node by erosion (g/s)
						erssedchinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a node by erosion (kg)
						erssedchoutflux[isolid][i][j][k] = 0.0;	//gross solids flux transported out of a node by erosion (g/s)
						erssedchoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a node by erosion (kg)
						bursedchinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a node by burial (kg)
						bursedchoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a node by burial (kg)
						scrsedchinmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported into a node by scour (kg)
						scrsedchoutmass[isolid][i][j][k] = 0.0;	//cumulative gross solids mass transported out of a node by scour (kg)

					}	//end loop over stack layers

				}	//end loop over nodes

			}	//end loop over links

		}	//end loop over solids

		//Allocate memory for solids transport parameters indexed by link, node, and source
		advinflowch = (float ***)malloc((nrows+1) * sizeof(float **));		//inflow by advection in the channel network (m3/s) [link][node][direction]
		advoutflowch = (float ***)malloc((nrows+1) * sizeof(float **));		//outflow by advection in the channel network (m3/s) [link][node][direction]
		dspflowch = (float ***)malloc((nrows+1) * sizeof(float **));		//inflow by dispersion in the channel network (m3/s) [link][node][direction]

		//loop over links
		for(i=1; i<=nlinks; i++)
		{
			//Allocate memory for solids transport parameters indexed by link, node, and source
			advinflowch[i] = (float **)malloc((ncols+1) * sizeof(float *));		//inflow by advection in the channel network (m3/s) [link][node][direction]
			advoutflowch[i] = (float **)malloc((ncols+1) * sizeof(float *));	//outflow by advection in the channel network (m3/s) [link][node][direction]
			dspflowch[i] = (float **)malloc((ncols+1) * sizeof(float *));		//flow by dispersion in the channel network (m3/s) [link][node][direction]

			//loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Allocate remaining memory for solids transport parameters indexed by link, node, and source
				advinflowch[i][j] = (float *)malloc((nsources+1) * sizeof(float));	//inflow by advection in the channel network (m3/s) [link][node][direction]
				advoutflowch[i][j] = (float *)malloc((nsources+1) * sizeof(float));	//outflow by advection in the channel network (m3/s) [link][node][direction]
				dspflowch[i][j] = (float *)malloc((nsources+1) * sizeof(float));	//flow by dispersion in the channel network (m3/s) [link][node][direction]

				//loop over sources
				for(k=0; k<=nsources; k++)
				{
					//initialize values
					advinflowch[i][j][k] = 0.0;		//inflow by advection in the channel network (m3/s) [link][node][direction]
					advoutflowch[i][j][k] = 0.0;	//outflow by advection in the channel network (m3/s) [link][node][direction]
					dspflowch[i][j][k] = 0.0;		//flow by dispersion in the channel network (m3/s) [link][node][direction]

				}	//end loop over sources

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt = 1

	/**********************************************************************/
	/*        Initialize Global Soil and Sediment Stack Variables         */
	/**********************************************************************/

	//Allocate memory for number soil stack layers
	nstackov = (int **)malloc((nrows+1) * sizeof(int *));				//present number of occupied soil layers

	//Allocate memory for soil stack parameters that are constant for all layers
	aovground = (float **)malloc((nrows+1) * sizeof(float *));			//ground surface area of a soil layer (m2)

	//Allocate memory for soil stack parameters that vary with each layer
	hlayerov = (float ***)malloc((nrows+1) * sizeof(float **));			//present thickness of soil layer (m)
	minvlayerov = (float ***)malloc((nrows+1) * sizeof(float **));		//minimum soil layer volume for a pop (m3)
	maxvlayerov = (float ***)malloc((nrows+1) * sizeof(float **));		//maximum soil layer volume for a push (m3)
	elevlayerov = (float ***)malloc((nrows+1) * sizeof(float **));		//elevation of a layer in the soil stack (m)

	//Allocate memory for soil stack parameters that vary with each layer (double precision)
	vlayerov0 = (double ***)malloc((nrows+1) * sizeof(double **));		//initial volume of a soil layer (m3)
	vlayerov = (double ***)malloc((nrows+1) * sizeof(double **));		//present volume of a soil layer (m3)
	vlayerovnew = (double ***)malloc((nrows+1) * sizeof(double **));	//new volume of a soil layer (m3)

	//loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate memory for number soil stack layers
		nstackov[i] = (int *)malloc((ncols+1) * sizeof(int));				//present number of occupied soil layers

		//Allocate additional memory for soil stack parameters that are constant for all layers
		aovground[i] = (float *)malloc((ncols+1) * sizeof(float));			//ground surface area for a soil layer (m2)

		//Allocate additional memory for soil stack parameters that vary with each layer
		hlayerov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//present thickness of soil layer (m)
		minvlayerov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//minimum soil layer volume for a pop (m3)
		maxvlayerov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//maximum soil layer volume for a push (m3)
		elevlayerov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//elevation of a layer in the soil stack (m)

		//Allocate additional memory for soil stack parameters that vary with each layer (double precision)
		vlayerov0[i] = (double **)malloc((ncols+1) * sizeof(double *));		//initial volume of a soil layer (m3)
		vlayerov[i] = (double **)malloc((ncols+1) * sizeof(double *));		//present volume of a soil layer (m3)
		vlayerovnew[i] = (double **)malloc((ncols+1) * sizeof(double *));	//new volume of a soil layer (m3)

		//loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (imask > 0)
			if(imask[i][j] > 0)
			{
				//Allocate remaining memory for soil stack parameters that vary with each layer
				hlayerov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//present thickness of soil layer (m)
				minvlayerov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//minimum soil layer volume for a pop (m3)
				maxvlayerov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//maximum soil layer volume for a push (m3)
				elevlayerov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//elevation of a layer in the soil stack (m)

				//Allocate remaining memory for soil stack parameters that vary with each layer (double precision)
				vlayerov0[i][j] = (double *)malloc((nstackov0[i][j]+1) * sizeof(double));	//initial volume of a soil layer (m3)
				vlayerov[i][j] = (double *)malloc((maxstackov+1) * sizeof(double));			//present volume of a soil layer (m3)
				vlayerovnew[i][j] = (double *)malloc((maxstackov+1) * sizeof(double));		//new volume of a soil layer (m3)

				//set the present number of soil layers to the initial number of layers
				nstackov[i][j] = nstackov0[i][j];

				//Assign ground surface area...
				//
				//if the cell is a channel cell
				if(imask[i][j] > 1)
				{
					//Get channel link and node
					chanlink = link[i][j];
					channode = node[i][j];

					//Compute area of channel within cell...
					//
					//Assign channel characteristics
					lch = chanlength[chanlink][channode];	//channel length (m) (includes sinuosity) (m)
					twch = twidth[chanlink][channode];		//channel top width (m)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;
				}
				else	//cell is not a channel cell (overland only)
				{
					//no channel present, surface area is zero
					achsurf = 0.0;

				}	//end if imask > 1

				//compute ground surface area of cell (m2)
				aovground[i][j] = w * w - achsurf;

				//Compute elevation of hardpan under the soil stack...
				//
				//initialize sum of layer thicknesses
				hsum = 0.0;

				//loop over occupied layers
				for(k=1; k<=nstackov0[i][j]; k++)
				{
					//Compute sum of layer thicknesses (m)
					hsum = hsum + hlayerov0[i][j][k];

				}	//end loop over occupied layers

				//elevation of hardpan under the soil stack (m)
				elevlayerov[i][j][0] = elevationov[i][j] - hsum;

				//loop over layers
				for(k=1; k<=maxstackov; k++)
				{
					//if the layer is initially occupied (k <= nstackov0)
					if(k <= nstackov0[i][j])
					{
						//set the present thickness to initial thickness (m)
						hlayerov[i][j][k] = hlayerov0[i][j][k];

						//compute the initial layer volume (m3)
						vlayerov0[i][j][k] = hlayerov0[i][j][k]
							* aovground[i][j];

						//set the present layer volume to initial layer volume (m3)
						vlayerov[i][j][k] = vlayerov0[i][j][k];

						//Note:  New sediment layer volumes are initialized here
						//       to prevent unitialized values from being used
						//       when subsurface layer volumes are updated in
						//       NewStateStack...
						//
						//set the new layer volume to initial layer volume (m3)
						vlayerovnew[i][j][k] = vlayerov0[i][j][k];

						//Compute the minimum volume limit for layer pop (m3)
						minvlayerov[i][j][k] = minvolov * (float)(vlayerov0[i][j][k]);

						//Compute the maximum volume limit for layer push (m3)
						maxvlayerov[i][j][k] = maxvolov * (float)(vlayerov0[i][j][k]);

						//Compute the elevation at the top of this layer (m)
						elevlayerov[i][j][k] = elevlayerov[i][j][k-1] +
							hlayerov0[i][j][k];

						//initialize solids concentrations in the stack
						//
						//assign the soil layer
						isoil = soiltype[i][j][k];

						//initialize total solids concentration for this soil layer
						csedov[0][i][j][k] = 0.0;

						//loop over solids (don't include zero because it is computed inside the loop)
						for(isolid=1; isolid<=nsolids; isolid++)
						{
							//compute particle density (kg/m3)
							densityp = (float)(spgravity[isolid] * 1000.0);

							//convert layer properties to solids concentrations
							//
							//initial solids concentration (g/m3)
							csedov[isolid][i][j][k] = (float)(gsdov[isoil][isolid]
								* densityp * (1.0 - porosityov[isoil])
								* 1000.0);

							//add present solids type to total concentration for this soil layer (g/m3)
							csedov[0][i][j][k] = csedov[0][i][j][k] + csedov[isolid][i][j][k];

						}	//end loop over solids
					}
					else	//else the layer is not occcupied (k > nstackov0)
					{
						//set the present thickness to zero
						hlayerov[i][j][k] = 0.0;

						//set the present layer volume to zero
						vlayerov[i][j][k] = 0.0;

						//set the minimum volume limit to zero
						minvlayerov[i][j][k] = 0.0;

						//set the maximum volume limit to zero
						maxvlayerov[i][j][k] = 0.0;

						//Compute the elevation at the top of this layer (m)
						elevlayerov[i][j][k] = elevationov[i][j];

						//initialize solids concentrations in the stack
						//
						//loop over solids (include zero for sum of all solids)
						for(isolid=0; isolid<=nsolids; isolid++)
						{
							//set initial solids concentration to zero (g/m3)
							csedov[isolid][i][j][k] = 0.0;

						}	//end loop over solids

					}	//end if k <= nstackov0

				}	//end loop over layers

			}	//end if imask > 0

		}	//end loop over columns

	}	//end loop over rows

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Allocate memory for number sediment stack layers
		nstackch = (int **)malloc((nlinks+1) * sizeof(int *));				//present number of occupied sediment layers

		//Allocate memory for sediment stack parameters that vary with each layer
		hlayerch = (float ***)malloc((nlinks+1) * sizeof(float **));		//present thickness of a sediment layer (m)
		bwlayerch = (float ***)malloc((nlinks+1) * sizeof(float **));		//present bottom width of a sediment layer (m)
		achbed0 = (float ***)malloc((nlinks+1) * sizeof(float **));			//initial ground surface area for a sediment layer (m2)
		achbed = (float ***)malloc((nlinks+1) * sizeof(float **));			//present ground surface area for a sediment layer (m2)
		minvlayerch = (float ***)malloc((nlinks+1) * sizeof(float **));		//minimum sediment layer volume for a pop (m3)
		maxvlayerch = (float ***)malloc((nlinks+1) * sizeof(float **));		//maximum sediment layer volume for a pop (m3)
		elevlayerch = (float ***)malloc((nlinks+1) * sizeof(float **));		//elevation of a layer in the sediment stack (m)

		//Allocate memory for sediment stack parameters that vary with each layer (double precision)
		vlayerch0 = (double ***)malloc((nlinks+1) * sizeof(float **));		//initial volume of a sediment layer (m3)
		vlayerch = (double ***)malloc((nlinks+1) * sizeof(float **));		//present volume of a sediment layer (m3)
		vlayerchnew = (double ***)malloc((nlinks+1) * sizeof(double **));	//new volume of a sediment layer (m3)

		//loop over links
		for(i=1; i<=nlinks; i++)
		{
			//Allocate memory for number sediment stack layers
			nstackch[i] = (int *)malloc((nnodes[i]+1) * sizeof(int));		//present number of occupied sediment layers

			//Allocate additional memory for sediment stack parameters that vary with each layer
			hlayerch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//present thickness of a sediment layer (m)
			bwlayerch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//present bottom width of a sediment layer (m)
			achbed0[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));			//initial ground surface area for a sediment layer (m2)
			achbed[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));			//present ground surface area for a sediment layer (m2)
			minvlayerch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//minimum sediment layer volume for a pop (m3)
			maxvlayerch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//maximum sediment layer volume for a pop (m3)
			elevlayerch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//elevation of a layer in the sediment stack (m)

			//Allocate memory for sediment stack parameters that vary with each layer (double precision)
			vlayerch0[i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));		//initial volume of a sediment layer (m3)
			vlayerch[i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));		//present volume of a sediment layer (m3)
			vlayerchnew[i] = (double **)malloc((nnodes[i]+1) * sizeof(double *));	//new volume of a sediment layer (m3)

			//loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Allocate remaining memory for sediment stack parameters that vary with each layer
				hlayerch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//present thickness of a sediment layer (m)
				bwlayerch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//present bottom width of a sediment layer (m)
				achbed0[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//initial ground surface area for a sediment layer (m2)
				achbed[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));			//present ground surface area for a sediment layer (m2)
				minvlayerch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//minimum sediment layer volume for a pop (m3)
				maxvlayerch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//maximum sediment layer volume for a pop (m3)
				elevlayerch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//elevation of a layer in the sediment stack (m)

				//Allocate memory for sediment stack parameters that vary with each layer (double precision)
				vlayerch0[i][j] = (double *)malloc((nstackch0[i][j]+1) * sizeof(double));	//initial volume of a sediment layer (m3)
				vlayerch[i][j] = (double *)malloc((maxstackch+1) * sizeof(double));			//present volume of a sediment layer (m3)
				vlayerchnew[i][j] = (double *)malloc((maxstackch+1) * sizeof(double));		//new volume of a sediment layer (m3)

				//set the present number of sediment layers to the initial number of layers
				nstackch[i][j] = nstackch0[i][j];

				//Compute elevation of hardpan under the sediment stack...
				//
				//initialize sum of layer thicknesses
				hsum = 0.0;

				//loop over occupied layers
				for(k=1; k<=nstackch0[i][j]; k++)
				{
					//Compute sum of layer thicknesses (m)
					hsum = hsum + hlayerch0[i][j][k];

				}	//end loop over occupied layers

				//elevation of hardpan under the sediment stack (m)
				elevlayerch[i][j][0] = elevationch[i][j] - hsum;

				//loop over layers
				for(k=1; k<=maxstackch; k++)
				{
					//if the layer is initially occupied (k <= nstackch0)
					if(k <= nstackch0[i][j])
					{
						//set the present thickness to the initial thickness (m)
						hlayerch[i][j][k] = hlayerch0[i][j][k];

						//set the present width to the initial width (m)
						bwlayerch[i][j][k] = bwlayerch0[i][j][k];

						//compute the present layer area (m2)
						achbed[i][j][k] = chanlength[i][j]
							* bwlayerch[i][j][k];

						//set the initial layer area (m2)
						achbed0[i][j][k] = achbed[i][j][k];

						//compute the initial layer volume (m3)
						vlayerch0[i][j][k] = hlayerch0[i][j][k]
							* achbed[i][j][k];

						//set the present layer volume to initial layer volume (m3)
						vlayerch[i][j][k] = vlayerch0[i][j][k];

						//Note:  New sediment layer volumes are initialized here
						//       to prevent unitialized values from being used
						//       when subsurface layer volumes are updated in
						//       NewStateStack...
						//
						//set the new layer volume to initial layer volume (m3)
						vlayerchnew[i][j][k] = vlayerch0[i][j][k];

						//Compute the minimum volume limit for layer pop (m3)
						minvlayerch[i][j][k] = minvolch * (float)(vlayerch0[i][j][k]);

						//Compute the maximum volume limit for layer push (m3)
						maxvlayerch[i][j][k] = maxvolch * (float)(vlayerch0[i][j][k]);

						//Compute the elevation at the top of this layer (m)
						elevlayerch[i][j][k] = elevlayerch[i][j][k-1] +
							hlayerch0[i][j][k];

						//initialize solids concentrations in the stack
						//
						//initialize total solids concentration for this sediment layer
						csedch[0][i][j][k] = 0.0;

						//loop over solids (don't include zero because it is computed inside the loop)
						for(isolid=1; isolid<=nsolids; isolid++)
						{
							//compute particle density (kg/m3)
							densityp = (float)(spgravity[isolid] * 1000.0);

							//convert layer properties to solids concentrations
							//
							//initial solids concentration (g/m3)
							csedch[isolid][i][j][k] = (float)(gsdch[isolid][i][j][k]
								* densityp * (1.0 - porositych[i][j][k])
								* 1000.0);

							//add present solids type to total concentration for this sediment layer (g/m3)
							csedch[0][i][j][k] = csedch[0][i][j][k] + csedch[isolid][i][j][k];

						}	//end loop over solids
					}
					else	//else the layer is not occcupied (k > nstackov0)
					{
						//set the present thickness to zero
						hlayerch[i][j][k] = 0.0;

						//set the present width to zero
						bwlayerch[i][j][k] = 0.0;

						//set the present area to zero
						achbed[i][j][k] = 0.0;

						//set the present layer volume to zero
						vlayerch[i][j][k] = 0.0;

						//set the minimum volume limit to zero
						minvlayerch[i][j][k] = 0.0;

						//set the maximum volume limit to zero
						maxvlayerch[i][j][k] = 0.0;

						//Compute the elevation at the top of this layer (m)
						elevlayerch[i][j][k] = elevationch[i][j];

						//set the porosity to zero (unoccupied layer)
						porositych[i][j][k]  = 0.0;

						//initialize solids concentrations in the stack
						//
						//loop over solids (include zero for sum of all solids)
						for(isolid=0; isolid<=nsolids; isolid++)
						{
							//set initial solids concentration to zero (g/m3)
							csedch[isolid][i][j][k] = 0.0;

						}	//end loop over solids

					}	//end if k <= nstackch0

				}	//end loop over layers

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt > 0

	/********************************************************************/
	/*        Initialize Global Solids Reporting Station Arrays         */
	/********************************************************************/

	//Allocate memory for reported sediment discharges (export)
	sedreportov = (float **)malloc((nsolids+1) * sizeof(float *));	//reported sediment discharge (units vary)
	sedreportch = (float **)malloc((nsolids+1) * sizeof(float *));	//reported sediment discharge (units vary)
	sedconvert = (float *)malloc((nsedreports+1) * sizeof(float));		//sediment export conversion factor

	//Loop over number of solids reporting groups (include zero for sum of solids)
	for(i=0; i<=nsolids; i++)
	{
		//Note:  Memory for solids reports is allocated for all particle
		//       types.  Sums of particle types for group reports are
		//       are computed in WriteTimeSeriesSolids.
		//
		//Allocate remaining memory for reported sediment discharges (export)
		sedreportov[i] = (float *)malloc((nsedreports+1) * sizeof(float));	//reported sediment discharge (units vary)
		sedreportch[i] = (float *)malloc((nsedreports+1) * sizeof(float));	//reported sediment discharge (units vary)

		//Loop over number of sediment reporting stations
		for(j=1; j<=nsedreports; j++)
		{
			//Initialize sediment reporting variables
			sedreportov[i][j] = 0.0;
			sedreportch[i][j] = 0.0;

		}	//end loop over sediment reporting stations

	}	//end loop over solids

	/**********************************************************/
	/*        Initialize Outlet Cell Solids Variables         */
	/**********************************************************/

	//Allocate memory for overland outlet mass tracking variables
	totaladvsedinov = (double **)malloc((nsolids+1) * sizeof(double *));		//cumulative solids mass entering domain by overland advection at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
	totaladvsedoutov = (double **)malloc((nsolids+1) * sizeof(double *));		//cumulative solids mass exiting domain by overland advection at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
	totaldspsedinov = (double **)malloc((nsolids+1) * sizeof(double *));		//cumulative solids mass entering domain by overland dispersion at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
	totaldspsedoutov = (double **)malloc((nsolids+1) * sizeof(double *));		//cumulative solids mass exiting domain by overland dispersion at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
	sedflowpeakov = (float **)malloc((nsolids+1) * sizeof(float *));		//peak sediment discharge leaving an outlet (kg/s) [isolid][outlet]
	sedtimepeakov = (float **)malloc((nsolids+1) * sizeof(float *));		//time of peak sediment discharge leaving an outlet (hrs) [isolid][outlet]

	//loop over solids (use zeroeth element for sum of all solids)
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Allocate remaining memory for overland outlet mass tracking variables
		totaladvsedinov[isolid] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative solids mass entering domain by overland advection at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
		totaladvsedoutov[isolid] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative solids mass exiting domain by overland advection at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
		totaldspsedinov[isolid] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative solids mass entering domain by overland dispersion at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
		totaldspsedoutov[isolid] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative solids mass exiting domain by overland dispersion at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
		sedflowpeakov[isolid] = (float *)malloc((noutlets+1) * sizeof(float));		//peak sediment discharge leaving an outlet (kg/s) [isolid][oulet]
		sedtimepeakov[isolid] = (float *)malloc((noutlets+1) * sizeof(float));		//time of peak sediment discharge leaving an outlet (hrs) [isolid][outlet]

		//Loop over number of outlet cells (zeroeth element to sum over all outlets)
		for(i=0; i<=noutlets; i++)
		{
			//initialize cumulative outlet solids mass entering by overland advection to zero (kg)
			totaladvsedinov[isolid][i] = 0.0;

			//initialize cumulative outlet solids mass exiting by overland advection to zero (kg)
			totaladvsedoutov[isolid][i] = 0.0;

			//initialize cumulative outlet solids mass entering by overland dispersion to zero (kg)
			totaldspsedinov[isolid][i] = 0.0;

			//initialize cumulative outlet solids mass exiting by overland dispersion to zero (kg)
			totaldspsedoutov[isolid][i] = 0.0;

			//initialize peak overland sediment discharge to zero (kg/s)
			sedflowpeakov[isolid][i] = 0.0;

			//initialize time of peak overland sediment discharge to zero (hrs)
			sedtimepeakov[isolid][i] = 0.0;

		}	//end loop over outlets

	}	//end loop over solids

	//if channels are simulated
	if(chnopt > 0)
	{
		//Allocate memory for channel outlet mass tracking variables
		totaladvsedinch = (double **)malloc((nsolids+1) * sizeof(double *));		//cumulative solids mass entering domain by channel advection at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
		totaladvsedoutch = (double **)malloc((nsolids+1) * sizeof(double *));		//cumulative solids mass exiting domain by channel advection at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
		totaldspsedinch = (double **)malloc((nsolids+1) * sizeof(double *));		//cumulative solids mass entering domain by channel dispersion at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
		totaldspsedoutch = (double **)malloc((nsolids+1) * sizeof(double *));		//cumulative solids mass exiting domain by channel dispersion at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
		sedflowpeakch = (float **)malloc((nsolids+1) * sizeof(float *));		//peak channel sediment discharge leaving an outlet (kg/s) [isolid][outlet]
		sedtimepeakch = (float **)malloc((nsolids+1) * sizeof(float *));		//time of peak channel sediment discharge leaving an outlet (hrs) [isolid][outlet]

		//loop over solids (include zeroeth element for sum of all solids)
		for(isolid=0; isolid<=nsolids; isolid++)
		{
			//Allocate remaining memory for channel outlet mass tracking variables
			totaladvsedinch[isolid] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative solids mass entering domain by channel advection at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
			totaladvsedoutch[isolid] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative solids mass exiting domain by channel advection at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
			totaldspsedinch[isolid] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative solids mass entering domain by channel dispersion at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
			totaldspsedoutch[isolid] = (double *)malloc((noutlets+1) * sizeof(double));	//cumulative solids mass exiting domain by channel dispersion at an outlet (kg) [isolid][outlet] (zeroeth element used to sum all outlets)
			sedflowpeakch[isolid] = (float *)malloc((noutlets+1) * sizeof(float));		//peak channel sediment discharge leaving an outlet (kg/s) [isolid][outlet]
			sedtimepeakch[isolid] = (float *)malloc((noutlets+1) * sizeof(float));		//time of peak channel sediment discharge leaving an outlet (hrs) [isolid][outlet]

			//Loop over number of outlet cells (zeroeth element to sum over all outlets)
			for(i=0; i<=noutlets; i++)
			{
				//initialize cumulative outlet solids mass entering by channel advection to zero (kg)
				totaladvsedinch[isolid][i] = 0.0;

				//initialize cumulative outlet solids mass exiting by channel advection to zero (kg)
				totaladvsedoutch[isolid][i] = 0.0;

				//initialize cumulative outlet solids mass entering by channel dispersion to zero (kg)
				totaldspsedinch[isolid][i] = 0.0;

				//initialize cumulative outlet solids mass exiting by channel dispersion to zero (kg)
				totaldspsedoutch[isolid][i] = 0.0;

				//initialize peak channel sediment discharge to zero (kg/s)
				sedflowpeakch[isolid][i] = 0.0;

				//initialize time of peak channel sediment discharge to zero (hrs)
				sedtimepeakch[isolid][i] = 0.0;

			}	//end loop over outlets

		}	//end loop over solids

	}	//end if chnopt > 0

	/**********************************************************************/
	/*        Initialize Solids Forcing Function and BC Variables         */
	/**********************************************************************/

	//Include forcing functions (loads) and boundary conditions (outlets)
	//
	//Initialize arrays related to solids loads and boundary conditions
	//
	//Allocate initial memory for overland point source solids (soil) loads
	nswpovt = (float **)malloc((nsolids+1) * sizeof(float *));		//next (upper) interval time for overland point source solids load time series
	pswpovt = (float **)malloc((nsolids+1) * sizeof(float *));		//prior (lower) interval time for overland point source solids load time series
	bswpov = (float **)malloc((nsolids+1) * sizeof(float *));		//intercept for overland point source solids load interpolation
	mswpov = (float **)malloc((nsolids+1) * sizeof(float *));		//slope for overland point source solids load interpolation
	swpovpoint = (int **)malloc((nsolids+1) * sizeof(int *));		//pointer (to position in time series) for overland point source solids loads
	swpovinterp = (float **)malloc((nsolids+1) * sizeof(float *));	//external overland point source solids load interpolated in time for solids type [i] source [j]
	ntswpov = (float *)malloc((nsolids+1) * sizeof(float));			//next time to update any overland point source solids load function for solids type [i]

	//Allocate initial memory for overland distributed solids (soil) loads
	nswdovt = (float **)malloc((nsolids+1) * sizeof(float *));		//next (upper) interval time for overland distributed load time series
	pswdovt = (float **)malloc((nsolids+1) * sizeof(float *));		//prior (lower) interval time for overland distributed load time series
	bswdov = (float **)malloc((nsolids+1) * sizeof(float *));		//intercept for overland distributed load interpolation
	mswdov = (float **)malloc((nsolids+1) * sizeof(float *));		//slope for overland distributed load interpolation
	swdovpoint = (int **)malloc((nsolids+1) * sizeof(int *));		//pointer (to position in time series) for overland distributed loads
	swdovinterp = (float **)malloc((nsolids+1) * sizeof(float *));	//external overland distributed load interpolated in time for solids type [i] source [j]
	ntswdov = (float *)malloc((nsolids+1) * sizeof(float));			//next time to update any overland distributed load function for solids type [i]

	//Allocate initial memory for solids boundary conditions
	nsbct = (float **)malloc((nsolids+1) * sizeof(float *));		//next (upper) interval time for sediment bc time series
	psbct = (float **)malloc((nsolids+1) * sizeof(float *));		//prior (lower) interval time for sediment bc time series
	bsbc = (float **)malloc((nsolids+1) * sizeof(float *));			//intercept for sediment bc interpolation
	msbc = (float **)malloc((nsolids+1) * sizeof(float *));			//slope for sediment bc interpolation
	sbcpoint = (int **)malloc((nsolids+1) * sizeof(int *));			//pointer (to position in time series) for sediment BCs
	sbcinterp = (float **)malloc((nsolids+1) * sizeof(float *));	//external sediment BC interpolated in time for outlet[i]
	ntsbc = (float *)malloc((nsolids+1) * sizeof(float));			//next time to update any bc function for solids type [i]

	//Loop over number of solids types
	for(i=1; i<=nsolids; i++)
	{
		//Allocate remaining memory for overland point source solids (soil) loads
		nswpovt[i] = (float *)malloc((nswpov[i]+1) * sizeof(float));		//next (upper) interval time for overland point source solids load time series
		pswpovt[i] = (float *)malloc((nswpov[i]+1) * sizeof(float));		//prior (lower) interval time for overland point source solids load time series
		bswpov[i] = (float *)malloc((nswpov[i]+1) * sizeof(float));		//intercept for overland point source solids load interpolation
		mswpov[i] = (float *)malloc((nswpov[i]+1) * sizeof(float));		//slope for overland point source solids load interpolation
		swpovpoint[i] = (int *)malloc((nswpov[i]+1) * sizeof(int));		//pointer (to position in time series) for overland point source solids loads
		swpovinterp[i] = (float *)malloc((nswpov[i]+1) * sizeof(float));	//external overland point source solids load interpolated in time for source[i]

		//Allocate remaining memory for overland distributed solids (soil) loads
		nswdovt[i] = (float *)malloc((nswdov[i]+1) * sizeof(float));		//next (upper) interval time for overland distributed solids load time series
		pswdovt[i] = (float *)malloc((nswdov[i]+1) * sizeof(float));		//prior (lower) interval time for overland distributed solids load time series
		bswdov[i] = (float *)malloc((nswdov[i]+1) * sizeof(float));			//intercept for overland distributed solids load interpolation
		mswdov[i] = (float *)malloc((nswdov[i]+1) * sizeof(float));			//slope for overland distributed solids load interpolation
		swdovpoint[i] = (int *)malloc((nswdov[i]+1) * sizeof(int));			//pointer (to position in time series) for overland distributed solids loads
		swdovinterp[i] = (float *)malloc((nswdov[i]+1) * sizeof(float));	//external overland distributed solids load interpolated in time for source[i]

		//Allocate remaining memory for solids boundary conditions
		nsbct[i] = (float *)malloc((noutlets+1) * sizeof(float));	//next (upper) interval time for sediment bc time series
		psbct[i] = (float *)malloc((noutlets+1) * sizeof(float));	//prior (lower) interval time for sediment bc time series
		bsbc[i] = (float *)malloc((noutlets+1) * sizeof(float));		//intercept for sediment bc interpolation
		msbc[i] = (float *)malloc((noutlets+1) * sizeof(float));		//slope for sediment bc interpolation
		sbcpoint[i] = (int *)malloc((noutlets+1) * sizeof(int));		//pointer (to position in time series) for sediment BCs
		sbcinterp[i] = (float *)malloc((noutlets+1) * sizeof(float));	//external sediment BC interpolated in time for outlet[i]

	}	//end loop over number of solids

	//if channels are simulated
	if(chnopt == 1)
	{
		//Initialize arrays related to sediment loads
		//
		//Allocate initial memory for channel solids (sediment) loads
		nswcht = (float **)malloc((nsolids+1) * sizeof(float *));		//next (upper) interval time for sediment load time series
		pswcht = (float **)malloc((nsolids+1) * sizeof(float *));		//prior (lower) interval time for sediment load time series
		bswch = (float **)malloc((nsolids+1) * sizeof(float *));		//intercept for sediment load interpolation
		mswch = (float **)malloc((nsolids+1) * sizeof(float *));		//slope for sediment load interpolation
		swchpoint = (int **)malloc((nsolids+1) * sizeof(int *));		//pointer (to position in time series) for sediment loads
		swchinterp = (float **)malloc((nsolids+1) * sizeof(float *));	//external sediment load interpolated in time for source[i]
		ntswch = (float *)malloc((nsolids+1) * sizeof(float));			//next time to update any load function for solids type [i]

		//Loop over number of solids types
		for(i=1; i<=nsolids; i++)
		{
			//Allocate remaining memory for channel solids (sediment) loads
			nswcht[i] = (float *)malloc((nswch[i]+1) * sizeof(float));	//next (upper) interval time for sediment load time series
			pswcht[i] = (float *)malloc((nswch[i]+1) * sizeof(float));	//prior (lower) interval time for sediment load time series
			bswch[i] = (float *)malloc((nswch[i]+1) * sizeof(float));	//intercept for sediment load interpolation
			mswch[i] = (float *)malloc((nswch[i]+1) * sizeof(float));	//slope for sediment load interpolation
			swchpoint[i] = (int *)malloc((nswch[i]+1) * sizeof(int));	//pointer (to position in time series) for sediment loads
			swchinterp[i] = (float *)malloc((nswch[i]+1) * sizeof(float));	//external sediment load interpolated in time for source[i]

		}	//end loop over number of solids

	}	//end if chhnopt = 1

	/**********************************************************/
	/*        Initialize Solids Export And Grid Files         */
	/**********************************************************/

	//Initialize export files...
	//
	//Note:  Memory for solids export files is allocated for each
	//       reporting group.  Sums of particle types for each group
	//       are computed in ComputeSolidsGroupConcSums.
	//
	//if nsedreports > 0 (if there are solids reporting stations...)
	if(nsedreports > 0)
	{
		//Allocate memory for solids export file names
		sedexpfile = (char **)malloc((nsgroups+1) * sizeof(char *));

		//loop over number of solids reports (include zero for sum of solids)
		for(i=0; i<=nsgroups; i++)
		{
			//Allocate remaining memory for sediment export file
			sedexpfile[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));

			//Set sediment export file name = sediment export file root name
			strcpy(sedexpfile[i], sedexpfileroot);

			//Add underscore to sediment export file name
			sedexpfile[i] = strcat(sedexpfile[i], "_");

			//if the group number is zero
			if(i == 0)
			{
				//Add reporting group name to sediment export file name
				sedexpfile[i] = strcat(sedexpfile[i], "total");
			}
			else	//else the group number > 0
			{
				//Add reporting group name to sediment export file name
				sedexpfile[i] = strcat(sedexpfile[i], sgroupname[i]);

			}	//end if i == 0
			
			//Add extension to sediment export file name
			sedexpfile[i] = strcat(sedexpfile[i], sedextension);

			//Set the sediment export file pointer to null
			sedexpfile_fp = NULL;

			//Set sediment export file pointer
			sedexpfile_fp = fopen(sedexpfile[i], "w");

			//if the sediment export file pointer is null, abort execution
			if(sedexpfile_fp == NULL)
			{
				//Write message to file
				fprintf(echofile_fp, "Solids Type = %d  Can't create Solids Export File: %s \n",
					i, sedexpfile[i]);

				//Write message to screen
				printf("Solids Type = %d  Can't create Solids Export File: %s \n",
					i, sedexpfile[i]);
				exit(EXIT_FAILURE);	//abort
			}

			//Write header labels to sediment export file...
			//
			//Write initial part of label
			fprintf(sedexpfile_fp, "Time (hours)");

			//Loop over number of sediment reporting stations
			for(j=1; j<=nsedreports; j++)
			{
				//Check sediment reporting units option (in reverse order)...
				//
				//if sedunitsopt = 2, sediment export reported in MT/day (metric tons/day)
				if(sedunitsopt[j] == 2)	
				{
					//Write label
					fprintf(sedexpfile_fp, ",Qsedov%d (MT/day)", j);

					//Write label
					fprintf(sedexpfile_fp, ",Qsedch%d (MT/day)", j);

					//Write label
					fprintf(sedexpfile_fp, ",Qsedsum%d (MT/day)", j);

					//Compute units conversion factor
					//
					//Note: this value will need to be multiplied by discharge to
					//      complete the conversion...
					//
					//Set conversion factor to report sediment export in desired units
					sedconvert[j] = (float) (86400.0/1.0e6);	//g/m3 * Q * sedconvert gives MT/day
				}
				else	//else sediment export reported in g/m3 (mg/l)
				{
					//Write label
					fprintf(sedexpfile_fp, ",Csedov%d (g/m3)", j);

					//Write label
					fprintf(sedexpfile_fp, ",Csedch%d (g/m3)", j);

					//Set conversion factor to report sediment export in desired units
					sedconvert[j] = 1.0;

				}	//end if sedunitsopt = 2

			}	//end loop over sediment discharge reporting stations

			//Write final part of label
			fprintf(sedexpfile_fp, "\n");

			//Close sediment export file
			fclose(sedexpfile_fp);

		}	//end loop over number of solids reports

    }  //end if nsedreports > 0

	//Initialize grid file names...
	//
	//Note:  Memory for solids grid files is allocated for each
	//       reporting group.  Sums of particle types for each group
	//       are computed in ComputeSolidsGroupConcSums, etc.
	//
	//Allocate memory for solids grid file names
	solidsconcwatergrid = (char **)malloc((nsgroups+1) * sizeof(char *));
	solidsconcsurfgrid = (char **)malloc((nsgroups+1) * sizeof(char *));
	solidserosiongrid = (char **)malloc((nsgroups+1) * sizeof(char *));
	solidsdepositiongrid = (char **)malloc((nsgroups+1) * sizeof(char *));
	solidsnetaccumgrid = (char **)malloc((nsgroups+1) * sizeof(char *));

	//loop over number of solids reports
	for(i=0; i<=nsgroups; i++)
	{
		//Allocate remaining memory for solids grid files
		solidsconcwatergrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		solidsconcsurfgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		solidserosiongrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		solidsdepositiongrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		solidsnetaccumgrid[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Set grid file names = grid file root names
		strcpy(solidsconcwatergrid[i], solidsconcwatergridroot);
		strcpy(solidsconcsurfgrid[i], solidsconcsurfgridroot);
		strcpy(solidserosiongrid[i], solidserosiongridroot);
		strcpy(solidsdepositiongrid[i], solidsdepositiongridroot);
		strcpy(solidsnetaccumgrid[i], solidsnetaccumgridroot);

		//Add underscore to grid file names
		solidsconcwatergrid[i] = strcat(solidsconcwatergrid[i], "_");
		solidsconcsurfgrid[i] = strcat(solidsconcsurfgrid[i], "_");
		solidserosiongrid[i] = strcat(solidserosiongrid[i], "_");
		solidsdepositiongrid[i] = strcat(solidsdepositiongrid[i], "_");
		solidsnetaccumgrid[i] = strcat(solidsnetaccumgrid[i], "_");

		//if the group number is zero
		if(i == 0)
		{
			//Add reporting group name to grid file names
			solidsconcwatergrid[i] = strcat(solidsconcwatergrid[i], "total");
			solidsconcsurfgrid[i] = strcat(solidsconcsurfgrid[i], "total");
			solidserosiongrid[i] = strcat(solidserosiongrid[i], "total");
			solidsdepositiongrid[i] = strcat(solidsdepositiongrid[i], "total");
			solidsnetaccumgrid[i] = strcat(solidsnetaccumgrid[i], "total");
		}
		else	//else the group number > 0
		{
			//Add reporting group name to grid file names
			solidsconcwatergrid[i] = strcat(solidsconcwatergrid[i], sgroupname[i]);
			solidsconcsurfgrid[i] = strcat(solidsconcsurfgrid[i], sgroupname[i]);
			solidserosiongrid[i] = strcat(solidserosiongrid[i], sgroupname[i]);
			solidsdepositiongrid[i] = strcat(solidsdepositiongrid[i], sgroupname[i]);
			solidsnetaccumgrid[i] = strcat(solidsnetaccumgrid[i], sgroupname[i]);

		}	//end if i == 0
		
	}	//end loop over number of reporting groups

	/************************************************************/
	/*        Initialize Min and Max Statistics Variables       */
	/************************************************************/

	//Allocate memory for minimum and maximum overland concentrations
	maxcsedov0 = (double *)malloc((nsolids+1) * sizeof(double));	//maximum solids concentration in the overland plane water column (g/m3)
	mincsedov0 = (double *)malloc((nsolids+1) * sizeof(double));	//minimum solids concentration in the overland plane water column (g/m3)
	maxcsedov1 = (double *)malloc((nsolids+1) * sizeof(double));	//maximum solids concentration in the overland plane surface soil layer (g/m3)
	mincsedov1 = (double *)malloc((nsolids+1) * sizeof(double));	//minimum solids concentration in the overland plane surface soil layer (g/m3)

	//loop over solids
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Initialize values
		maxcsedov0[isolid] = -9e30;	//maximum solids concentration in the overland plane water column (g/m3)
		mincsedov0[isolid] = 9e30;	//minimum solids concentration in the overland plane water column (g/m3)
		maxcsedov1[isolid] = -9e30;	//maximum solids concentration in the overland plane surface soil layer (g/m3)
		mincsedov1[isolid] = 9e30;	//minimum solids concentration in the overland plane surface soil layer (g/m3)

	}	//end loop over solids

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Allocate memory for minimum and maximum channel concentrations
		maxcsedch0 = (double *)malloc((nsolids+1) * sizeof(double));	//maximum solids concentration in the channel network water column (g/m3)
		mincsedch0 = (double *)malloc((nsolids+1) * sizeof(double));	//minimum solids concentration in the channel network water column (g/m3)
		maxcsedch1 = (double *)malloc((nsolids+1) * sizeof(double));	//maximum solids concentration in the channel network surface soil layer (g/m3)
		mincsedch1 = (double *)malloc((nsolids+1) * sizeof(double));	//minimum solids concentration in the channel network surface soil layer (g/m3)

		//loop over solids
		for(isolid=0; isolid<=nsolids; isolid++)
		{
			//Initialize values
			maxcsedch0[isolid] = -9e30;	//maximum solids concentration in the channel network water column (g/m3)
			mincsedch0[isolid] = 9e30;	//minimum solids concentration in the channel network water column (g/m3)
			maxcsedch1[isolid] = -9e30;	//maximum solids concentration in the channel network surface sediment layer (g/m3)
			mincsedch1[isolid] = 9e30;	//minimum solids concentration in the channel network surface sediment layer (g/m3)

		}	//end loop over solids

	}	//end if chnopt > 0

//End of function: Return to Initialize
}
