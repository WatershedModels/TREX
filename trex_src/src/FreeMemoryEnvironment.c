/*----------------------------------------------------------------------
C-  Function:	FreeMemoryEnvironment.c                                              
C-
C-	Purpose/    Free allocated memory for environmental variables
C-  Methods     at end of model run.
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:   npropg, npropov, npropch, chnopt
C-
C-	Calls:		None
C-
C-	Called by:	FreeMemory
C-
C-  Created:	Mark Velleux
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
C-	Date:		14-SEP-2004
C-
C-	Revisions:
C-
C-	Date:
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

void FreeMemoryEnvironment()
{
	//local variable declarations/definitions
	int
		iprop,			//loop index for environmental property
		itf,			//loop index for time function
		isolid;			//loop index for solids type

	/**********************************************/
	/*        Free ReadDataGroupE Variables       */
	/**********************************************/

//mlv	//Deallocate memory for environmental property file names
//mlv	free(envpropfile);	//deallocate memory

	//loop over number of general properties
	for(iprop=1; iprop<=npropg; iprop++)
	{
		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//Deallocate memory (cols)
			free(envg[iprop][i]);	//general property cell value
			free(envgtfid[iprop][i]);	//general property time function id (pointer)

		}	//end loop over rows

		//if time functions are specified
		if(nenvgtf[iprop] > 0)
		{
			//Loop over number of time functions
			for(itf=1; itf<=nenvgtf[iprop]; itf++)
			{
				//Deallocate memory for general environmental time functions
				free(envgtf[iprop][itf]);		//deallocate memory
				free(envgtftime[iprop][itf]);	//deallocate memory

			}	//end loop over number of time functions

		}	//end if nenvgtf[iprop] > 0

		//Deallocate memory for general environmental time functions
		free(nenvgtfpairs[iprop]);		//number of overland time functions pairs
		free(envgtf[iprop]);		//general time function value
		free(envgtftime[iprop]);	//general time function time break

		//Deallocate memory for general environmental properties cell values...
		//
		//Deallocate memory (rows)
		free(envg[iprop]);		//general property cell value
		free(envgtfid[iprop]);		//general property time function id (pointer)

	}	//end loop over general properties

	//Deallocate memory for general environmental properties and time functions
	free(pidg);					//general property identification number
	free(envg);		//general property cell value
	free(envgtfid);		//general property time function id (pointer)
	free(nenvgtf);				//number of general property time functions
	free(nenvgtfpairs);		//number of general time functions pairs
	free(envgtf);		//general time function value
	free(envgtftime);	//general time function time break

	//loop over number of overland properties
	for(iprop=1; iprop<=npropov; iprop++)
	{
		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//loop over columns
			for(i=1; i<=ncols; i++)
			{
				//Deallocate memory (layers)
				free(envov[iprop][i][j]);	//overland property cell value
				free(envovtfid[iprop][i][j]);	//overland property time function id (pointer)

			}	//end loop over columns

			//Deallocate memory (cols)
			free(envov[iprop][i]);	//overland property cell value
			free(envovtfid[iprop][i]);	//overland property time function id (pointer)

		}	//end loop over rows

		//Deallocate memory (rows)
		free(envov[iprop]);		//overland property cell value
		free(envovtfid[iprop]);		//overland property time function id (pointer)

		//if time functions are specified
		if(nenvovtf[iprop] > 0)
		{
			//Loop over number of time functions
			for(itf=1; itf<=nenvovtf[iprop]; itf++)
			{
				//Deallocate memory for overland environmental time functions
				free(envovtf[iprop][itf]);	//deallocate memory
				free(envovtftime[iprop][itf]);	//deallocate memory

			}	//end loop over number of time functions

		}	//end if nenvovtf[iprop] > 0

		//Deallocate memory for overland environmental time functions
		free(nenvovtfpairs[iprop]);		//number of overland time functions pairs
		free(envovtf[iprop]);		//overland time function value
		free(envovtftime[iprop]);	//overland time function time break

	}	//end loop over overland properties

	//Deallocate memory for overland environmental properties and time functions
	free(pidov);					//overland property identification number
	free(envov);		//overland property cell value
	free(envovtfid);		//overland property time function id (pointer)
	free(nenvovtf);				//number of overland property time functions
	free(nenvovtfpairs);		//number of overland time functions pairs
	free(envovtf);		//overland time function value
	free(envovtftime);	//overland time function time break

	//if channel are simulated
	if(chnopt > 0)
	{
		//loop over number of properties
		for(iprop=1; iprop<=npropch; iprop++)
		{
			//loop over links
			for(i=1; i<=nlinks; i++)
			{
				//Deallocate memory (cols)
				free(envch[iprop][i]);	//channel property node value
				free(envchtfid[iprop][i]);	//channel property time function id (pointer)

				//loop over nodes
				for(i=1; i<=nnodes[i]; i++)
				{
					//Deallocate memory (layers)
					free(envch[iprop][i][j]);	//channel property node value
					free(envchtfid[iprop][i][j]);	//channel property time function id (pointer)

				}	//end loop over nodes

			}	//end loop over links

			//Deallocate memory (links)
			free(envch[iprop]);		//channel property node value
			free(envchtfid[iprop]);		//channel property time function id (pointer)

			//if time functions are specified
			if(nenvchtf[iprop] > 0)
			{
				//Loop over number of time functions
				for(itf=1; itf<=nenvchtf[iprop]; itf++)
				{
					//Deallocate memory for channel environmental time functions
					free(envchtf[iprop][itf]);		//deallocate memory
					free(envchtftime[iprop][itf]);	//deallocate memory

				}	//end loop over number of time functions

			}	//end if nenvchtf[iprop] > 0

			//Deallocate memory for channel environmental time functions
			free(nenvchtfpairs[iprop]);		//number of channel time functions pairs
			free(envchtf[iprop]);		//channel time function value
			free(envchtftime[iprop]);	//channel time function time break

		}	//end loop over channel properties

		//Deallocate memory for channel environmental properties and time functions
		free(pidch);					//channel property identification number
		free(envch);		//channel property cell value
		free(envchtfid);		//channel property time function id (pointer)
		free(nenvchtf);				//number of channel property time functions
		free(nenvchtfpairs);		//number of channel time functions pairs
		free(envchtf);			//channel time function value
		free(envchtftime);		//channel time function time break

	}	//end if chanopt > 0

	//Overland particle foc
	//
	//Loop over solids
	for(isolid=1; isolid<=nsolids; isolid++)
	{
		//Loop over rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over columns
			for(j=1; j<=ncols; j++)
			{
				//Deallocate memory for overland particle properties
				free(fpocov[isolid][i][j]);		//overland paricle fraction organic carbon (dimensionless) [isolid][row][col][layer]
				free(fpocovtfid[isolid][i][j]);		//overland particle property time function id (pointer) [isolid][row][col][ilayer]

			}	//end loop over columns

			//Deallocate memory for overland particle properties
			free(fpocov[isolid][i]);		//overland paricle fraction organic carbon (dimensionless) [isolid][row][col][layer]
			free(fpocovtfid[isolid][i]);		//overland particle property time function id (pointer) [isolid][row][col][ilayer]

		}	//end loop over rows

		//Deallocate memory for overland particle properties
		free(fpocov[isolid]);		//overland paricle fraction organic carbon (dimensionless) [isolid][row][col][layer]
		free(fpocovtfid[isolid]);	//overland particle property time function id (pointer) [isolid][row][col][ilayer]

	}	//end loop over solids

	//Deallocate memory for overland particle properties
	free(fpocov);		//overland paricle fraction organic carbon (dimensionless) [isolid][row][col][layer]
	free(fpocovtfid);		//overland particle property time function id (pointer) [isolid][row][col][ilayer]
	free(nfpocovtf);				//number of overland particle time functions [isolid]
	free(nfpocovtfpairs);		//number of time series pairs in overland particle time function [isolid][itf]
	free(fpocovtf);		//overland particle time function value [isolid][itf][ipairs]
	free(fpocovtftime);	//overland partcle time function time break [isolid][itf][ipairs]

	//Read overland particle properties
	//
	//if fpocovopt > 0 (overland fpoc value are entered for each solids type)
	if(fpocovopt > 0)
	{
		//loop over number of solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//Deallocate memory for overland particle properties
			free(nfpocovtfpairs[isolid]);		//number of time series pairs in overland particle time function [isolid][itf]
			free(fpocovtf[isolid]);		//overland particle time function value [isolid][itf][ipairs]
			free(fpocovtftime[isolid]);	//overland partcle time function time break [isolid][itf][ipairs]

			//if time functions are specified
			if(nfpocovtf[isolid] > 0)
			{
				//Loop over number of time functions
				for(itf=1; itf<=nfpocovtf[isolid]; itf++)
				{
					//Deallocate memory for overland fpoc time functions
					free(fpocovtf[isolid][itf]);	//deallocate memory
					free(fpocovtftime[isolid][itf]);	//deallocate memory

				}	//end loop over number of time functions

			}	//end if nfpocovtf[isolid] > 0

		}	//end loop over solids

	}	//end if fpocovopt > 0

	//Channel particle foc
	//
	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Loop over solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//Loop over links
			for(i=1; i<=nlinks; i++)
			{
				//Loop over nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Deallocate memory for channel particle properties
					free(fpocch[isolid][i][j]);			//channel paricle fraction organic carbon (dimensionless) [isolid][link][node][layer]
					free(fpocchtfid[isolid][i][j]);			//channel particle property time function id (pointer) [isolid][row][col][ilayer]

				}	//end loop over nodes

				//Deallocate memory for channel particle properties
				free(fpocch[isolid][i]);		//channel paricle fraction organic carbon (dimensionless) [isolid][link][node][layer]
				free(fpocchtfid[isolid][i]);		//channel particle property time function id (pointer) [isolid][row][col][ilayer]

			}	//end loop over links

			//Deallocate memory for channel particle properties
			free(fpocch[isolid]);		//channel paricle fraction organic carbon (dimensionless) [isolid][link][node][layer]
			free(fpocchtfid[isolid]);		//channel particle property time function id (pointer) [isolid][row][col][ilayer]

		}	//end loop over solids

		//if fpocchopt > 0 (channel particle fpoc is specified)
		if(fpocchopt > 0)
		{
			//loop over number of solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//if time functions are specified
				if(nfpocchtf[isolid] > 0)
				{
					//Loop over number of time functions
					for(itf=1; itf<=nfpocchtf[isolid]; itf++)
					{
						//Deallocate memory for channel fpoc time functions
						free(fpocchtf[isolid][itf]);	//deallocate memory
						free(fpocchtftime[isolid][itf]);	//deallocate memory

					}	//end loop over number of time functions

				}	//end if nfpocchtf[isolid] > 0

				//Deallocate memory for overland particle properties
				free(nfpocchtfpairs[isolid]);		//number of time series pairs in channel particle time function [isolid][itf]
				free(fpocchtf[isolid]);		//channel particle time function value [isolid][itf][ipairs]
				free(fpocchtftime[isolid]);	//channel partcle time function time break [isolid][itf][ipairs]

			}	//end loop over solids

		}	//end if fpocchopt > 0

		//Deallocate memory for channel particle properties
		free(fpocch);		//channel paricle fraction organic carbon (dimensionless) [isolid][link][node][layer]
		free(fpocchtfid);		//channel particle property time function id (pointer) [isolid][row][col][ilayer]
		free(nfpocchtf);				//number of channel particle time functions [isolid]
		free(nfpocchtfpairs);		//number of time series pairs in channel particle time function [isolid][itf]
		free(fpocchtf);		//channel particle time function value [isolid][itf][ipairs]
		free(fpocchtftime);	//channel partcle time function time break [isolid][itf][ipairs]

	}	//end if chanopt > 0

	/*****************************************************/
	/*        Free InitializeEnvironment Variables       */
	/*****************************************************/



	/***********************************************************************/
	/*        Free Global General Environmental Property Variables         */
	/***********************************************************************/

	//Free arrays related to general environmental time functions
	//
	//Loop over number of general properties
	for(iprop=1; iprop<=npropg; iprop++)
	{
		//Deallocate memory for general property time functions
		free(nenvgtft[iprop]);		//next (upper) interval time for general property time series
		free(penvgtft[iprop]);		//prior (lower) interval time for general property time series
		free(benvgtf[iprop]);		///intercept for general property interpolation
		free(menvgtf[iprop]);		//slope for general property interpolation
		free(envgtfpoint[iprop]);		//pointer (to position in time series) for general function
		free(envgtfinterp[iprop]);	//general property function interpolated in time

	}	//end loop over number of general properties

	//Deallocate memory for general property time functions
	free(nenvgtft);		//next (upper) interval time for general property time series
	free(penvgtft);		//prior (lower) interval time for general property time series
	free(benvgtf);		//intercept for general property interpolation
	free(menvgtf);		//slope for general property interpolation
	free(envgtfpoint);		//pointer (to position in time series) for general function
	free(envgtfinterp);	//general property function interpolated in time
	free(ntenvgtf);			//next time to update any general function for property [i]

	/************************************************************************/
	/*        Free Global Overland Environmental Property Variables         */
	/************************************************************************/

	//Loop over number of overland properties
	for(iprop=1; iprop<=npropov; iprop++)
	{
		//Deallocate memory for overland property time functions
		free(nenvovtft[iprop]);		//next (upper) interval time for overland property time series
		free(penvovtft[iprop]);		//prior (lower) interval time for overland property time series
		free(benvovtf[iprop]);			///intercept for overland property interpolation
		free(menvovtf[iprop]);			//slope for overland property interpolation
		free(envovtfpoint[iprop]);			//pointer (to position in time series) for overland function
		free(envovtfinterp[iprop]);	//overland property function interpolated in time

	}	//end loop over number of overland properties

	//Deallocate memory for overland property time functions
	free(nenvovtft);		//next (upper) interval time for overland property time series
	free(penvovtft);		//prior (lower) interval time for overland property time series
	free(benvovtf);			//intercept for overland property interpolation
	free(menvovtf);			//slope for overland property interpolation
	free(envovtfpoint);			//pointer (to position in time series) for overland function
	free(envovtfinterp);	//overland property function interpolated in time
	free(ntenvovtf);			//next time to update any overland function for property [i]

	/***********************************************************************/
	/*        Free Global Channel Environmental Property Variables         */
	/***********************************************************************/

	//if channels are simulated
	if(chnopt > 0)
	{
		//Loop over number of channel properties
		for(iprop=1; iprop<=npropch; iprop++)
		{
			//Deallocate memory for channel property time functions
			free(nenvchtft[iprop]);		//next (upper) interval time for channel property time series
			free(penvchtft[iprop]);		//prior (lower) interval time for channel property time series
			free(benvchtf[iprop]);			//intercept for channel property interpolation
			free(menvchtf[iprop]);			//slope for channel property interpolation
			free(envchtfpoint[iprop]);			//pointer (to position in time series) for channel function
			free(envchtfinterp[iprop]);	//channel property function interpolated in time

		}	//end loop over number of channel properties

		//Deallocate memory for channel property time functions
		free(nenvchtft);		//next (upper) interval time for channel property time series
		free(penvchtft);		//prior (lower) interval time for channel property time series
		free(benvchtf);		//intercept for channel property interpolation
		free(menvchtf);		//slope for channel property interpolation
		free(envchtfpoint);		//pointer (to position in time series) for channel function
		free(envchtfinterp);	//channel property function interpolated in time
		free(ntenvchtf);			//next time to update any channel function for property [i]

	}	//end if chhnopt > 0

	/**************************************************************/
	/*        Free Global Overland Particle Foc Variables         */
	/**************************************************************/

	//if fpocovopt > 0 (overland particle fpoc is specified)
	if(fpocovopt > 0)
	{
		//Loop over number of solids
		for(i=1; i<=nsolids; i++)
		{
			//Deallocate memory for overland particle time functions
			free(nfpocovtft[i]);		//next (upper) interval time for overland particle time series
			free(pfpocovtft[i]);		//prior (lower) interval time for overland particle time series
			free(bfpocovtf[i]);		//intercept for overland particle function interpolation
			free(mfpocovtf[i]);		//slope for overland particle interpolation
			free(fpocovtfpoint[i]);		//pointer (to position in time series) for overland particlefunction
			free(fpocovtfinterp[i]);	//overland particle function interpolated in time

		}	//end loop over number of solids

		//Deallocate memory for overland particle time functions
		free(nfpocovtft);		//next (upper) interval time for overland particle time series
		free(pfpocovtft);		//prior (lower) interval time for overland particle time series
		free(bfpocovtf);		//intercept for overland particle function interpolation
		free(mfpocovtf);		//slope for overland property interpolation
		free(fpocovtfpoint);		//pointer (to position in time series) for overland particle function
		free(fpocovtfinterp);	//overland particle function interpolated in time
		free(ntfpocovtf);			//next time to update any overland particle function for solid[i]

	}	//end if fpocovopt > 0

	/**************************************************************/
	/*        Free Global Overland Particle Foc Variables         */
	/**************************************************************/

	//if channels are simulated
	if(chnopt > 0)
	{
		//if fpocchopt > 0 (channel particle fpoc is specified)
		if(fpocchopt > 0)
		{
			//Loop over number of solids
			for(i=1; i<=nsolids; i++)
			{
				//Deallocate memory for channel property time functions
				free(nfpocchtft[i]);		//next (upper) interval time for channel particle time series
				free(pfpocchtft[i]);		//prior (lower) interval time for channel particle time series
				free(bfpocchtf[i]);		//intercept for channel particle interpolation
				free(mfpocchtf[i]);		//slope for channel particle function interpolation
				free(fpocchtfpoint[i]);		//pointer (to position in time series) for channel particle function
				free(fpocchtfinterp[i]);	//channel particle function interpolated in time

			}	//end loop over number of solids

			//Deallocate memory for channel property time functions
			free(nfpocchtft);		//next (upper) interval time for channel property time series
			free(pfpocchtft);		//prior (lower) interval time for channel property time series
			free(bfpocchtf);		//intercept for channel property interpolation
			free(mfpocchtf);		//slope for channel particle function interpolation
			free(fpocchtfpoint);		//pointer (to position in time series) for channel function
			free(fpocchtfinterp);	//channel property function interpolated in time
			free(ntfpocchtf);			//next time to update any channel function for solid[i]

		}	//end if fpocchopt > 0

	}	//end if chhnopt > 0

	/************************************************************/
	/*        Free Named General Environmental Properties       */
	/************************************************************/

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Deallocate memory for general properties
		free(windspeed[i]);		//wind speed (m/s) [row][col]
		free(airtemp[i]);		//air temperature (C) [row][col]
		free(solarrad[i]);		//solar radiation (W/m2) [row][col]

	}	//end loop over rows

	//Deallocate memory for general properties
	free(windspeed);		//wind speed (m/s) [row][col]
	free(airtemp);		//air temperature (C) [row][col]
	free(solarrad);		//solar radiation (W/m2) [row][col]

	/*************************************************************/
	/*        Free Named Overland Environmental Properties       */
	/*************************************************************/

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//Deallocate memory for overland properties
			free(cdocov[i][j]);			//overland doc concentration (g/m3) [row][col][layer]
			free(fdocov[i][j]);			//overland effective fraction doc (dimensionless) [row][col][layer]
			free(hardnessov[i][j]);	//overland hardness concentration (g/m3) [row][col][layer]
			free(phov[i][j]);			//overland pH (s.u.) [row][col][layer]
			free(temperatureov[i][j]);	//overland water/soil temperature (C) [row][col][layer]
			free(oxradov[i][j]);		//overland oxidant/radical concentration (g/m3) [row][col][layer]
			free(bacteriaov[i][j]);		//overland bacterial concentration (cells/100 mL) [row][col][layer]
			free(extinctionov[i][j]);	//overland light extinction coefficient (1/m) [row][col][layer[i][j])

		}	//end loop over columns

		//Deallocate memory for overland properties
		free(cdocov[i]);			//overland doc concentration (g/m3) [row][col][layer]
		free(fdocov[i]);			//overland effective fraction doc (dimensionless) [row][col][layer]
		free(hardnessov[i]);		//overland hardness concentration (g/m3) [row][col][layer]
		free(phov[i]);			//overland pH (s.u.) [row][col][layer]
		free(temperatureov[i]);		//overland water/soil temperature (C) [row][col][layer]
		free(oxradov[i]);			//overland oxidant/radical concentration (g/m3) [row][col][layer]
		free(bacteriaov[i]);		//overland bacterial concentration (cells/100 mL) [row][col][layer]
		free(extinctionov[i]);	//overland light extinction coefficient (1/m) [row][col][layer[i])

	}	//end loop over rows

	//Deallocate memory for overland properties
	free(cdocov);			//overland doc concentration (g/m3) [row][col][layer]
	free(fdocov);			//overland effective fraction doc (dimensionless) [row][col][layer]
	free(hardnessov);		//overland hardness concentration (g/m3) [row][col][layer]
	free(phov);				//overland pH (s.u.) [row][col][layer]
	free(temperatureov);		//overland water/soil temperature (C) [row][col][layer]
	free(oxradov);			//overland oxidant/radical concentration (g/m3) [row][col][layer]
	free(bacteriaov);		//overland bacterial concentration (cells/100 mL) [row][col][layer]
	free(extinctionov);		//overland light extinction coefficient (1/m) [row][col][layer)

	/************************************************************/
	/*        Free Named Channel Environmental Properties       */
	/************************************************************/

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Loop over links
		for(i=1; i<=nlinks; i++)
		{
			//Loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Deallocate memory for channel properties
				free(cdocch[i][j]);			//channel doc concentration (g/m3) [link][node][layer]
				free(fdocch[i][j]);			//channel effective fraction doc (dimensionless) [link][node][layer]
				free(hardnessch[i][j]);		//channel hardness concentration (g/m3) [link][node][layer]
				free(phch[i][j]);			//channel pH (s.u.) [link][node][layer]
				free(temperaturech[i][j]);	//channel water/sediment temperature (C) [link][node][layer]
				free(oxradch[i][j]);		//channel oxidant/radical concentration (g/m3) [link][node][layer]
				free(bacteriach[i][j]);		//channel bacterial concentration (cells/100 mL) [link][node][layer]
				free(extinctionch[i][j]);	//channel light extinction coefficient (1/m) [link][node][layer[i][j])

			}	//end loop over nodes

			//Deallocate memory for channel properties
			free(cdocch[i]);			//channel doc concentration (g/m3) [link][node][layer]
			free(fdocch[i]);			//channel effective fraction doc (dimensionless) [link][node][layer]
			free(hardnessch[i]);		//channel hardness concentration (g/m3) [link][node][layer]
			free(phch[i]);				//channel pH (s.u.) [link][node][layer]
			free(temperaturech[i]);		//channel water/sediment temperature (C) [link][node][layer]
			free(oxradch[i]);			//channel oxidant/radical concentration (g/m3) [link][node][layer]
			free(bacteriach[i]);		//channel bacterial concentration (cells/100 mL) [link][node][layer]
			free(extinctionch[i]);		//channel light extinction coefficient (1/m) [link][node][layer[i])

		}	//end loop over links

		//Deallocate memory for channel properties
		free(cdocch);			//channel doc concentration (g/m3) [link][node][layer]
		free(fdocch);			//channel effective fraction doc (dimensionless) [link][node][layer]
		free(hardnessch);		//channel hardness concentration (g/m3) [link][node][layer]
		free(phch);			//channel pH (s.u.) [link][node][layer]
		free(temperaturech);		//channel water/sediment temperature (C) [link][node][layer]
		free(oxradch);			//channel oxidant/radical concentration (g/m3) [link][node][layer]
		free(bacteriach);		//channel bacterial concentration (cells/100 mL) [link][node][layer]
		free(extinctionch);	//channel light extinction coefficient (1/m) [link][node][layer)

	}	//end if chnopt > 0

//End of function: Return to FreeMemory
}
