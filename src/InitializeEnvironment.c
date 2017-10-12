/*----------------------------------------------------------------------
C-  Function:	InitializeEnvironment.c
C-
C-	Purpose/	Allocate memory for and initialize environmental
C-  Methods:    variables used in computations but not read from
C-              input files.
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
C-	Created:	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-				
C-	Date:		07-SEP-2004
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

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void InitializeEnvironment()
{
	//Local variable declarations/definitions
	int
		iprop;		//index for number of properties specified

	/*****************************************************************************/
	/*        Initialize Global General Environmental Property Variables         */
	/*****************************************************************************/

	//set meteorology time function update time to simulation start time
	timemeteorology = simtime;

	//Initialize arrays related to general environmental time functions
	//
	//Allocate initial memory for general property time functions
	nenvgtft = (float **)malloc((npropg+1) * sizeof(float *));		//next (upper) interval time for general property time series
	penvgtft = (float **)malloc((npropg+1) * sizeof(float *));		//prior (lower) interval time for general property time series
	benvgtf = (float **)malloc((npropg+1) * sizeof(float *));		//intercept for general property interpolation
	menvgtf = (float **)malloc((npropg+1) * sizeof(float *));		//slope for general property interpolation
	envgtfpoint = (int **)malloc((npropg+1) * sizeof(int *));		//pointer (to position in time series) for general function
	envgtfinterp = (float **)malloc((npropg+1) * sizeof(float *));	//general property function interpolated in time
	ntenvgtf = (float *)malloc((npropg+1) * sizeof(float));			//next time to update any general function for property [i]

	//Loop over number of general properties
	for(iprop=1; iprop<=npropg; iprop++)
	{
		//Allocate remaining memory for general property time functions
		nenvgtft[iprop] = (float *)malloc((nenvgtf[iprop]+1) * sizeof(float));		//next (upper) interval time for general property time series
		penvgtft[iprop] = (float *)malloc((nenvgtf[iprop]+1) * sizeof(float));		//prior (lower) interval time for general property time series
		benvgtf[iprop] = (float *)malloc((nenvgtf[iprop]+1) * sizeof(float));		//intercept for general property interpolation
		menvgtf[iprop] = (float *)malloc((nenvgtf[iprop]+1) * sizeof(float));		//slope for general property interpolation
		envgtfpoint[iprop] = (int *)malloc((nenvgtf[iprop]+1) * sizeof(int));		//pointer (to position in time series) for general function
		envgtfinterp[iprop] = (float *)malloc((nenvgtf[iprop]+1) * sizeof(float));	//general property function interpolated in time

	}	//end loop over number of general properties

	/******************************************************************************/
	/*        Initialize Global Overland Environmental Property Variables         */
	/******************************************************************************/

	//Initialize arrays related to overland environmental time functions
	//
	//Allocate initial memory for overland property time functions
	nenvovtft = (float **)malloc((npropov+1) * sizeof(float *));		//next (upper) interval time for overland property time series
	penvovtft = (float **)malloc((npropov+1) * sizeof(float *));		//prior (lower) interval time for overland property time series
	benvovtf = (float **)malloc((npropov+1) * sizeof(float *));			//intercept for overland property interpolation
	menvovtf = (float **)malloc((npropov+1) * sizeof(float *));			//slope for overland property interpolation
	envovtfpoint = (int **)malloc((npropov+1) * sizeof(int *));			//pointer (to position in time series) for overland function
	envovtfinterp = (float **)malloc((npropov+1) * sizeof(float *));	//overland property function interpolated in time
	ntenvovtf = (float *)malloc((npropov+1) * sizeof(float));			//next time to update any overland function for property [i]

	//Loop over number of overland properties
	for(iprop=1; iprop<=npropov; iprop++)
	{
		//Allocate remaining memory for overland property time functions
		nenvovtft[iprop] = (float *)malloc((nenvovtf[iprop]+1) * sizeof(float));		//next (upper) interval time for overland property time series
		penvovtft[iprop] = (float *)malloc((nenvovtf[iprop]+1) * sizeof(float));		//prior (lower) interval time for overland property time series
		benvovtf[iprop] = (float *)malloc((nenvovtf[iprop]+1) * sizeof(float));			//intercept for overland property interpolation
		menvovtf[iprop] = (float *)malloc((nenvovtf[iprop]+1) * sizeof(float));			//slope for overland property interpolation
		envovtfpoint[iprop] = (int *)malloc((nenvovtf[iprop]+1) * sizeof(int));			//pointer (to position in time series) for overland function
		envovtfinterp[iprop] = (float *)malloc((nenvovtf[iprop]+1) * sizeof(float));	//overland property function interpolated in time

	}	//end loop over number of overland properties

	/*****************************************************************************/
	/*        Initialize Global Channel Environmental Property Variables         */
	/*****************************************************************************/

	//if channels are simulated
	if(chnopt > 0)
	{
		//Initialize arrays related to channel environmental time functions
		//
		//Allocate initial memory for channel property time functions
		nenvchtft = (float **)malloc((npropch+1) * sizeof(float *));		//next (upper) interval time for channel property time series
		penvchtft = (float **)malloc((npropch+1) * sizeof(float *));		//prior (lower) interval time for channel property time series
		benvchtf = (float **)malloc((npropch+1) * sizeof(float *));		//intercept for channel property interpolation
		menvchtf = (float **)malloc((npropch+1) * sizeof(float *));		//slope for channel property interpolation
		envchtfpoint = (int **)malloc((npropch+1) * sizeof(int *));		//pointer (to position in time series) for channel function
		envchtfinterp = (float **)malloc((npropch+1) * sizeof(float *));	//channel property function interpolated in time
		ntenvchtf = (float *)malloc((npropch+1) * sizeof(float));			//next time to update any channel function for property [i]

		//Loop over number of channel properties
		for(iprop=1; iprop<=npropch; iprop++)
		{
			//Allocate remaining memory for channel property time functions
			nenvchtft[iprop] = (float *)malloc((nenvchtf[iprop]+1) * sizeof(float));		//next (upper) interval time for channel property time series
			penvchtft[iprop] = (float *)malloc((nenvchtf[iprop]+1) * sizeof(float));		//prior (lower) interval time for channel property time series
			benvchtf[iprop] = (float *)malloc((nenvchtf[iprop]+1) * sizeof(float));			//intercept for channel property interpolation
			menvchtf[iprop] = (float *)malloc((nenvchtf[iprop]+1) * sizeof(float));			//slope for channel property interpolation
			envchtfpoint[iprop] = (int *)malloc((nenvchtf[iprop]+1) * sizeof(int));			//pointer (to position in time series) for channel function
			envchtfinterp[iprop] = (float *)malloc((nenvchtf[iprop]+1) * sizeof(float));	//channel property function interpolated in time

		}	//end loop over number of channel properties

	}	//end if chhnopt > 0

	/********************************************************************/
	/*        Initialize Global Overland Particle Foc Variables         */
	/********************************************************************/

	//if fpocovopt > 0 (overland particle fpoc is specified)
	if(fpocovopt > 0)
	{
		//Initialize arrays related to overland particle time functions
		//
		//Allocate initial memory for overland particle time functions
		nfpocovtft = (float **)malloc((nsolids+1) * sizeof(float *));		//next (upper) interval time for overland particle time series
		pfpocovtft = (float **)malloc((nsolids+1) * sizeof(float *));		//prior (lower) interval time for overland particle time series
		bfpocovtf = (float **)malloc((nsolids+1) * sizeof(float *));		//intercept for overland particle function interpolation
		mfpocovtf = (float **)malloc((nsolids+1) * sizeof(float *));		//slope for overland property interpolation
		fpocovtfpoint = (int **)malloc((nsolids+1) * sizeof(int *));		//pointer (to position in time series) for overland particle function
		fpocovtfinterp = (float **)malloc((nsolids+1) * sizeof(float *));	//overland particle function interpolated in time
		ntfpocovtf = (float *)malloc((nsolids+1) * sizeof(float));			//next time to update any overland particle function for solid[i]

		//Loop over number of solids
		for(i=1; i<=nsolids; i++)
		{
			//Allocate remaining memory for overland particle time functions
			nfpocovtft[i] = (float *)malloc((nfpocovtf[i]+1) * sizeof(float));		//next (upper) interval time for overland particle time series
			pfpocovtft[i] = (float *)malloc((nfpocovtf[i]+1) * sizeof(float));		//prior (lower) interval time for overland particle time series
			bfpocovtf[i] = (float *)malloc((nfpocovtf[i]+1) * sizeof(float));		//intercept for overland particle function interpolation
			mfpocovtf[i] = (float *)malloc((nfpocovtf[i]+1) * sizeof(float));		//slope for overland particle interpolation
			fpocovtfpoint[i] = (int *)malloc((nfpocovtf[i]+1) * sizeof(int));		//pointer (to position in time series) for overland particlefunction
			fpocovtfinterp[i] = (float *)malloc((nfpocovtf[i]+1) * sizeof(float));	//overland particle function interpolated in time

		}	//end loop over number of solids

	}	//end if fpocovopt > 0

	/********************************************************************/
	/*        Initialize Global Overland Particle Foc Variables         */
	/********************************************************************/

	//if channels are simulated
	if(chnopt > 0)
	{
		//if fpocchopt > 0 (channel particle fpoc is specified)
		if(fpocchopt > 0)
		{
			//Initialize arrays related to channel particle time functions
			//
			//Allocate initial memory for channel property time functions
			nfpocchtft = (float **)malloc((nsolids+1) * sizeof(float *));		//next (upper) interval time for channel property time series
			pfpocchtft = (float **)malloc((nsolids+1) * sizeof(float *));		//prior (lower) interval time for channel property time series
			bfpocchtf = (float **)malloc((nsolids+1) * sizeof(float *));		//intercept for channel property interpolation
			mfpocchtf = (float **)malloc((nsolids+1) * sizeof(float *));		//slope for channel particle function interpolation
			fpocchtfpoint = (int **)malloc((nsolids+1) * sizeof(int *));		//pointer (to position in time series) for channel function
			fpocchtfinterp = (float **)malloc((nsolids+1) * sizeof(float *));	//channel property function interpolated in time
			ntfpocchtf = (float *)malloc((nsolids+1) * sizeof(float));			//next time to update any channel function for solid[i]

			//Loop over number of solids
			for(i=1; i<=nsolids; i++)
			{
				//Allocate remaining memory for channel property time functions
				nfpocchtft[i] = (float *)malloc((nfpocchtf[i]+1) * sizeof(float));		//next (upper) interval time for channel particle time series
				pfpocchtft[i] = (float *)malloc((nfpocchtf[i]+1) * sizeof(float));		//prior (lower) interval time for channel particle time series
				bfpocchtf[i] = (float *)malloc((nfpocchtf[i]+1) * sizeof(float));		//intercept for channel particle interpolation
				mfpocchtf[i] = (float *)malloc((nfpocchtf[i]+1) * sizeof(float));		//slope for channel particle function interpolation
				fpocchtfpoint[i] = (int *)malloc((nfpocchtf[i]+1) * sizeof(int));		//pointer (to position in time series) for channel particle function
				fpocchtfinterp[i] = (float *)malloc((nfpocchtf[i]+1) * sizeof(float));	//channel particle function interpolated in time

			}	//end loop over number of solids

		}	//end if fpocchopt > 0

	}	//end if chhnopt > 0

	/******************************************************************/
	/*        Initialize Named General Environmental Properties       */
	/******************************************************************/

	//Allocate initial memory for general properties
	windspeed = (float **)malloc((nrows+1) * sizeof(float *));		//wind speed (m/s) [row][col]
	airtemp = (float **)malloc((nrows+1) * sizeof(float *));		//air temperature (C) [row][col]
	solarrad = (float **)malloc((nrows+1) * sizeof(float *));		//solar radiation (W/m2) [row][col]
	
	//Developer's Note:  This is a temporary patch for snowmelt simulations
	cloudcover = (float **)malloc((nrows+1) * sizeof(float *));		//cloud cover (fraction)(0-1) [row][col]
	albedo = (float **)malloc((nrows+1) * sizeof(float *));		//albedo (fraction) (0-1) [row][col]

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate remaining memory for general properties
		windspeed[i] = (float *)malloc((ncols+1) * sizeof(float));		//wind speed (m/s) [row][col]
		airtemp[i] = (float *)malloc((ncols+1) * sizeof(float));		//air temperature (C) [row][col]
		solarrad[i] = (float *)malloc((ncols+1) * sizeof(float));		//solar radiation (W/m2) [row][col]

		//Developer's Note:  This is a temporary patch for snowmelt simulations
		cloudcover[i] = (float *)malloc((ncols+1) * sizeof(float));		//cloud cover (fraction)(0-1) [row][col]
		albedo[i] = (float *)malloc((ncols+1) * sizeof(float));		//albedo (fraction) (0-1) [row][col]

		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//set default values to zero (0 = process not simulated)
			windspeed[i][j] = 0.0;		//wind speed (m/s) [row][col]
			airtemp[i][j] = 0.0;		//air temperature (C) [row][col]
			solarrad[i][j] = 0.0;		//solar radiation (W/m2) [row][col]

			//Developer's Note:  This is a temporary patch for snowmelt simulations
			cloudcover[i][j] = 0.0;		//cloud cover (fraction)(0-1) [row][col]
			albedo[i][j] = 0.0;		//albedo (fraction) (0-1) [row][col]

		}	//end loop over columns

	}	//end loop over rows

	/*******************************************************************/
	/*        Initialize Named Overland Environmental Properties       */
	/*******************************************************************/

	//Allocate initial memory for overland properties
	cdocov = (float ***)malloc((nrows+1) * sizeof(float **));			//overland doc concentration (g/m3) [row][col][layer]
	fdocov = (float ***)malloc((nrows+1) * sizeof(float **));			//overland effective fraction doc (dimensionless) [row][col][layer]
	hardnessov = (float ***)malloc((nrows+1) * sizeof(float **));		//overland hardness concentration (g/m3) [row][col][layer]
	phov = (float ***)malloc((nrows+1) * sizeof(float **));				//overland pH (s.u.) [row][col][layer]
	temperatureov = (float ***)malloc((nrows+1) * sizeof(float **));		//overland water/soil temperature (C) [row][col][layer]
	oxradov = (float ***)malloc((nrows+1) * sizeof(float **));			//overland oxidant/radical concentration (g/m3) [row][col][layer]
	bacteriaov = (float ***)malloc((nrows+1) * sizeof(float **));		//overland bacterial concentration (cells/100 mL) [row][col][layer]
	extinctionov = (float ***)malloc((nrows+1) * sizeof(float **));		//overland light extinction coefficient (1/m) [row][col][layer = 0]
	udrpropov = (float ***)malloc((nrows+1) * sizeof(float **));		//overland user-defined reaction property

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate additional memory for overland properties
		cdocov[i] = (float **)malloc((ncols+1) * sizeof(float *));			//overland doc concentration (g/m3) [row][col][layer]
		fdocov[i] = (float **)malloc((ncols+1) * sizeof(float *));			//overland effective fraction doc (dimensionless) [row][col][layer]
		hardnessov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//overland hardness concentration (g/m3) [row][col][layer]
		phov[i] = (float **)malloc((ncols+1) * sizeof(float *));			//overland pH (s.u.) [row][col][layer]
		temperatureov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//overland water/soil temperature (C) [row][col][layer]
		oxradov[i] = (float **)malloc((ncols+1) * sizeof(float *));			//overland oxidant/radical concentration (g/m3) [row][col][layer]
		bacteriaov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//overland bacterial concentration (cells/100 mL) [row][col][layer]
		extinctionov[i] = (float **)malloc((ncols+1) * sizeof(float *));	//overland light extinction coefficient (1/m) [row][col][layer[i] = 0]
		udrpropov[i] = (float **)malloc((ncols+1) * sizeof(float *));		//overland user-defined reaction property

		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//Allocate remaining memory for overland properties
			cdocov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));			//overland doc concentration (g/m3) [row][col][layer]
			fdocov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));			//overland effective fraction doc (dimensionless) [row][col][layer]
			hardnessov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//overland hardness concentration (g/m3) [row][col][layer]
			phov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));			//overland pH (s.u.) [row][col][layer]
			temperatureov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//overland water/soil temperature (C) [row][col][layer]
			oxradov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//overland oxidant/radical concentration (g/m3) [row][col][layer]
			bacteriaov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//overland bacterial concentration (cells/100 mL) [row][col][layer]
			extinctionov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));	//overland light extinction coefficient (1/m) [row][col][layer[i][j] = 0]
			udrpropov[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//overland user-defined reaction property

			//loop over layers (include 0 for water column)
			for(k=0; k<=maxstackov; k++)
			{
				//set default values to zero (fdoc = 1.0, pH = 7.0, water/soil temp = 10)
				cdocov[i][j][k] = 0.0;			//overland doc concentration (g/m3) [row][col][layer]
				fdocov[i][j][k] = 1.0;			//overland effective fraction doc (dimensionless) [row][col][layer]
				hardnessov[i][j][k] = 0.0;		//overland hardness concentration (g/m3) [row][col][layer]
				phov[i][j][k] = 7.0;			//overland pH (s.u.) [row][col][layer]
				temperatureov[i][j][k] = 10.0;	//overland water/soil temperature (C) [row][col][layer]
				oxradov[i][j][k] = 0.0;			//overland oxidant/radical concentration (g/m3) [row][col][layer]
				bacteriaov[i][j][k] = 0.0;		//overland bacterial concentration (cells/100 mL) [row][col][layer]
				extinctionov[i][j][k] = 0.0;	//overland light extinction coefficient (1/m) [row][col][layer[i][j] = 0]
				udrpropov[i][j][k] = 0.0;		//overland user-defined reaction property

			}	//end loop over layers

		}	//end loop over columns

	}	//end loop over rows

	/******************************************************************/
	/*        Initialize Named Channel Environmental Properties       */
	/******************************************************************/

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Allocate initial memory for channel properties
		cdocch = (float ***)malloc((nlinks+1) * sizeof(float **));			//channel doc concentration (g/m3) [link][node][layer]
		fdocch = (float ***)malloc((nlinks+1) * sizeof(float **));			//channel effective fraction doc (dimensionless) [link][node][layer]
		hardnessch = (float ***)malloc((nlinks+1) * sizeof(float **));		//channel hardness concentration (g/m3) [link][node][layer]
		phch = (float ***)malloc((nlinks+1) * sizeof(float **));			//channel pH (s.u.) [link][node][layer]
		temperaturech = (float ***)malloc((nlinks+1) * sizeof(float **));		//channel water/sediment temperature (C) [link][node][layer]
		oxradch = (float ***)malloc((nlinks+1) * sizeof(float **));			//channel oxidant/radical concentration (g/m3) [link][node][layer]
		bacteriach = (float ***)malloc((nlinks+1) * sizeof(float **));		//channel bacterial concentration (cells/100 mL) [link][node][layer]
		extinctionch = (float ***)malloc((nlinks+1) * sizeof(float **));	//channel light extinction coefficient (1/m) [link][node][layer = 0]
		udrpropch = (float ***)malloc((nlinks+1) * sizeof(float **));		//channel user-defined reaction property

		//Loop over links
		for(i=1; i<=nlinks; i++)
		{
			//Allocate additional memory for channel properties
			cdocch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));			//channel doc concentration (g/m3) [link][node][layer]
			fdocch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));			//channel effective fraction doc (dimensionless) [link][node][layer]
			hardnessch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//channel hardness concentration (g/m3) [link][node][layer]
			phch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));			//channel pH (s.u.) [link][node][layer]
			temperaturech[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//channel water/sediment temperature (C) [link][node][layer]
			oxradch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));			//channel oxidant/radical concentration (g/m3) [link][node][layer]
			bacteriach[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//channel bacterial concentration (cells/100 mL) [link][node][layer]
			extinctionch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//channel light extinction coefficient (1/m) [link][node][layer[i] = 0]
			udrpropch[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//channel user-defined reaction property

			//Loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Allocate remaining memory for channel properties
				cdocch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));			//channel doc concentration (g/m3) [link][node][layer]
				fdocch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));			//channel effective fraction doc (dimensionless) [link][node][layer]
				hardnessch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//channel hardness concentration (g/m3) [link][node][layer]
				phch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));			//channel pH (s.u.) [link][node][layer]
				temperaturech[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//channel water/sediment temperature (C) [link][node][layer]
				oxradch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//channel oxidant/radical concentration (g/m3) [link][node][layer]
				bacteriach[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//channel bacterial concentration (cells/100 mL) [link][node][layer]
				extinctionch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//channel light extinction coefficient (1/m) [link][node][layer[i][j] = 0]
				udrpropch[i][j] = (float *)malloc((maxstackch+1) * sizeof(float));		//channel user-defined reaction property

				//loop over layers (include 0 for water column)
				for(k=0; k<=maxstackch; k++)
				{
					//set default values to zero (fdoc = 1.0, pH = 7.0, water/sediment temp = 10)
					cdocch[i][j][k] = 0.0;			//channel doc concentration (g/m3) [link][node][layer]
					fdocch[i][j][k] = 1.0;			//channel effective fraction doc (dimensionless) [link][node][layer]
					hardnessch[i][j][k] = 0.0;		//channel hardness concentration (g/m3) [link][node][layer]
					phch[i][j][k] = 7.0;			//channel pH (s.u.) [link][node][layer]
					temperaturech[i][j][k] = 10.0;	//channel water/sediment temperature (C) [link][node][layer]
					oxradch[i][j][k] = 0.0;			//channel oxidant/radical concentration (g/m3) [link][node][layer]
					bacteriach[i][j][k] = 0.0;		//channel bacterial concentration (cells/100 mL) [link][node][layer]
					extinctionch[i][j][k] = 0.0;	//channel light extinction coefficient (1/m) [link][node][layer[i][j] = 0]
					udrpropch[i][j][k] = 0.0;		//channel user-defined reaction property

				}	//end loop over layers

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt > 0

//End of function: Return to Initialize
}
