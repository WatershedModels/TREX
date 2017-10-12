/*----------------------------------------------------------------------
C-  Function:	UpdateEnvironment.c
C-
C-	Purpose/	UpdateEnvironment is called to compute environmental
C-  Methods:    conditions for each cell and node in the model domain.
C-              Values are computed for each property as the product
C-              of cell/node constant values and interpolated time-
C-              dependent functions for that cell/node. The convention
C-              used is:
C-
C-              E(x,y,t) = Econst(x,y) * Etime(t)
C-
C-              where:  E(x,y,t) = spatially and temporally variable
C-                                 environmental property value
C-
C-                      Econst(x,y) = spatially variable environmental
C-                                    property value for a cell/node
C-
C-                      Etime(t) = temporally variable environmental
C-                                 time function value for a cell/node
C-
C-
C-	Inputs:		envgtf[][][], envovtf[][][], envchtf[][][],
C-              focovtf[][][], focchtf[][][]
C-              (at some time t in time series)
C-
C-	Outputs:	envgtfinterp[][], envovtfinterp[][], envchtfinterp[][],
C-              focovtfinterp[][], focchtfinterp[][]
C-              (at current simulation time)
C-
C-	Controls:	chnopt, npropg, npropov, npropch, nsolids,
C-              nenvgtf, nenvovtf, nenvchtf, nfpocovtf, nfpocchtf,
C-              nenvgtfpairs, nenvovtfpairs, nenvchtfpairs,
C-              nfpocovpairs, nfpocchpairs
C-
C-	Calls:		ComputeSolarRadiation
C-
C-	Called by:	trex
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-				Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		13-SEP-2004
C-
C-	Revised:	Mark Velleux
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		20-JAN-2007
C-
C-	Revisions:	Added update for meteorology (solar radiation)
C-
C-	Revised:	Mark Velleux
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		20-JAN-2009
C-
C-	Revisions:	Added update for air temperature lapse with elevation
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

void UpdateEnvironment()
{
	//local variable declarations/definitions
	int
		iprop,		//loop index for properties
		ip,			//pointer to current position in time series
		ncycle;		//number of completed cycles for a time series

	int
		pid,		//index for property identification
		tfid;		//index for time function identification

	double
		endtime,	//end time of a time function (last time break in series)
		mtime;		//modulo end time

	float
		nt,			//next (upper) interval time for a time function
		pt;			//prior (lower) interval time for a time function

//mlv  This module should be split into parts for overland and channel properties.
//mlv  UpdateEnvironment should be a shell to control entry to UpdateEnvironmentOverland
//mlv  and UpdateEnvironmentChannel.  The logic for the overland properties could be
//mlv  restructured so that it loops over the grid once with a loop for each process ID
//mlv  occuring for each cell.  This could shorten the code and make it less repetative.
//mlv  In fact, it might be even be easier to eliminate the loop over properties for each
//mlv  call and just to have one assignment for each property.  That would be faster...
//mlv  The same is true for channel processes too.

	//Developer's Note:  To implement a modified temperature index snowmelt model,
	//                   meteorlogical conditions such as incident solar radiation
	//                   must be computed and updated on a periodic basis.  This
	//                   is performed following the approach of Liston and Elder
	//                   (2006).  However, this is really just a placeholder until
	//                   a more complete treatment of meteorology is integrated
	//                   into the TREX code.
	//
	//Case-Specific Patch (mlv)...
	//
	//if snowmelt is simulated and it is time to update meteorology
	if(meltopt > 0 && simtime > timemeteorology)
	{
		//Compute solar radiation
		ComputeSolarRadiation();

		//set next meteorology update time (updated every 15 minutes = 0.25 hours)
		timemeteorology = timemeteorology + 0.25;

	}	//end if meltopt > 0 and simtine > timemeteorology 

	//Update general environmental conditions...
	//
	//loop over number of general functions
	for(iprop=1; iprop<=npropg; iprop++)
	{
		//set property identification
		pid = pidg[iprop];

		//if the property is wind speed (pid == 1)
		if(pid == 1)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//set the time function id for this cell
						tfid = envgtfid[iprop][i][j];

						//if a time function exists for this cell (tfid > 0)
						if(tfid > 0)
						{
							//set the wind speed value (m/s) f(x,y,t)
							windspeed[i][j] = envg[iprop][i][j] * envgtfinterp[iprop][tfid];
						}
						else	//else, no time function exists
						{
							//set the wind speed value (m/s) f(x,y)
							windspeed[i][j] = envg[iprop][i][j];

						}	//end if envgtfid > 0

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is air temperature (pid == 2)
		else if(pid == 2)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//set the time function id for this cell
						tfid = envgtfid[iprop][i][j];

						//if a time function exists for this cell (tfid > 0)
						if(tfid > 0)
						{
							//set the air temperature value (C) f(x,y,t)
							airtemp[i][j] = envg[iprop][i][j] * envgtfinterp[iprop][tfid];
						}
						else	//else, no time function exists
						{
							//set the air temperature value (C) f(x,y)
							airtemp[i][j] = envg[iprop][i][j];

						}	//end if envgtfid > 0

						//copmute air temperature lapse adjustments for elevations
						airtemp[i][j] = airtemp[i][j]
							- atlapse[tfid] * (elevationov[i][j] - stnelevenvg[2][1]);

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is solar radiation (pid == 3)
		else if(pid == 3)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//set the time function id for this cell
						tfid = envgtfid[iprop][i][j];

						//if a time function exists for this cell (tfid > 0)
						if(tfid > 0)
						{
							//set the solar radiation value (W/m2) f(x,y,t)
							solarrad[i][j] = envg[iprop][i][j] * envgtfinterp[iprop][tfid];
						}
						else	//else, no time function exists
						{
							//set the solar radiation value (W/m2) f(x,y)
							solarrad[i][j] = envg[iprop][i][j];

						}	//end if envgtfid > 0

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is cloud cover (pid == 4)
		else if(pid == 4)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//set the time function id for this cell
						tfid = envgtfid[iprop][i][j];

						//if a time function exists for this cell (tfid > 0)
						if(tfid > 0)
						{
							//set the cloud cover value (fraction, 0-1) f(x,y,t)
							cloudcover[i][j] = envg[iprop][i][j] * envgtfinterp[iprop][tfid];
						}
						else	//else, no time function exists
						{
							//set the cloud cover value (fraction, 0-1) f(x,y)
							cloudcover[i][j] = envg[iprop][i][j];

						}	//end if envgtfid > 0

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is albedo (pid == 5)
		else if(pid == 5)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//set the time function id for this cell
						tfid = envgtfid[iprop][i][j];

						//if a time function exists for this cell (tfid > 0)
						if(tfid > 0)
						{
							//set the albedo value (fraction, 0-1) f(x,y,t)
							albedo[i][j] = envg[iprop][i][j] * envgtfinterp[iprop][tfid];
						}
						else	//else, no time function exists
						{
							//set the albedo value (fraction, 0-1) f(x,y)
							albedo[i][j] = envg[iprop][i][j];

						}	//end if envgtfid > 0

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//else, the property is undefined
		else
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nEnvironmental Property Error:\n");
			fprintf(echofile_fp,       "  Undefined environmental property\n");
			fprintf(echofile_fp,       "  General Property ID = %d\n", pid);
			fprintf(echofile_fp,       "  No property for this identifier");

			//Write error message to screen
			printf(              "\n\n\nEnvironmental Property Error:\n");
			printf(                    "  Undefined environmental property\n");
			printf(                    "  General Property ID = %d\n", pid);
			printf(                    "  No property for this identifier");

			exit(EXIT_FAILURE);					//abort

		}	//end if pid == 1 etc...

	}	//end loop over number of general properties

	//Update overland environmental functions...
	//
	//loop over number of overland functions
	for(iprop=1; iprop<=npropov; iprop++)
	{
		//set property identification
		pid = pidov[iprop];

		//if the property is doc concentration (cdoc) (pid == 1)
		if(pid == 1)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackov; k++)
						{
							//set the time function id for this cell
							tfid = envovtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the doc concentration value (g/m3) f(x,y,z,t)
								cdocov[i][j][k] = envov[iprop][i][j][k] * envovtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the doc concentration value (g/m3) f(x,y,z)
								cdocov[i][j][k] = envov[iprop][i][j][k];

							}	//end if envovtfid > 0

						}	//end loop over layers

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is effection fraction of doc (fdoc) (pid == 2)
		else if(pid == 2)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackov; k++)
						{
							//set the time function id for this cell
							tfid = envovtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the fdoc value (dimensionless) f(x,y,z,t)
								fdocov[i][j][k] = envov[iprop][i][j][k] * envovtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the fdoc value (dimensionless) f(x,y,z)
								fdocov[i][j][k] = envov[iprop][i][j][k];

							}	//end if envovtfid > 0

						}	//end loop over layers

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is hardness (pid == 3)
		else if(pid == 3)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackov; k++)
						{
							//set the time function id for this cell
							tfid = envovtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the hardness value (g/m3) f(x,y,z,t)
								hardnessov[i][j][k] = envov[iprop][i][j][k] * envovtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the hardness value (g/m3) f(x,y,z)
								hardnessov[i][j][k] = envov[iprop][i][j][k];

							}	//end if envovtfid > 0

						}	//end loop over layers

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is pH (pid == 4)
		else if(pid == 4)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackov; k++)
						{
							//set the time function id for this cell
							tfid = envovtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the pH value (s.u.) f(x,y,z,t)
								phov[i][j][k] = envov[iprop][i][j][k] * envovtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the pH value (s.u.) f(x,y,z)
								phov[i][j][k] = envov[iprop][i][j][k];

							}	//end if envovtfid > 0

						}	//end loop over layers

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is water/soil temperature (pid == 5)
		else if(pid == 5)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackov; k++)
						{
							//set the time function id for this cell
							tfid = envovtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the water/soil temperature value (C) f(x,y,z,t)
								temperatureov[i][j][k] = envov[iprop][i][j][k] * envovtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the water/soil temperature value (C) f(x,y,z)
								temperatureov[i][j][k] = envov[iprop][i][j][k];

							}	//end if envovtfid > 0

						}	//end loop over layers

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is oxidant/radical concentration (pid == 6)
		else if(pid == 6)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackov; k++)
						{
							//set the time function id for this cell
							tfid = envovtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the oxidant/radical concentration value (g/m3) f(x,y,z,t)
								oxradov[i][j][k] = envov[iprop][i][j][k] * envovtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the oxidant/radical concentration value (g/m3) f(x,y,z)
								oxradov[i][j][k] = envov[iprop][i][j][k];

							}	//end if envovtfid > 0

						}	//end loop over layers

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is bacterial concentration (pid == 7)
		else if(pid == 7)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackov; k++)
						{
							//set the time function id for this cell
							tfid = envovtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the bacteria concentration value (cells/100 mL) f(x,y,z,t)
								bacteriaov[i][j][k] = envov[iprop][i][j][k] * envovtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the bacteria concentration value (cells/100 mL) f(x,y,z)
								bacteriaov[i][j][k] = envov[iprop][i][j][k];

							}	//end if envovtfid > 0

						}	//end loop over layers

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is light extinction coefficient (pid == 8)
		else if(pid == 8)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//set layer to water column
						k=0;

						//set the time function id for this cell
						tfid = envovtfid[iprop][i][j][k];

						//if a time function exists for this cell (tfid > 0)
						if(tfid > 0)
						{
							//set the light extinction coefficient value (1/m) f(x,y,t)
							extinctionov[i][j][k] = envov[iprop][i][j][k] * envovtfinterp[iprop][tfid];
						}
						else	//else, no time function exists
						{
							//set the light extinction coefficient value (1/m) f(x,y)
							extinctionov[i][j][k] = envov[iprop][i][j][k];

						}	//end if envovtfid > 0

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//if the property is user-defined reaction property (pid == 9)
		else if(pid == 9)
		{
			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//if the cell is in the domain (is not null)
					if(imask[i][j] != nodatavalue)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackov; k++)
						{
							//set the time function id for this cell
							tfid = envovtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the user-defined reaction value f(x,y,z,t)
								udrpropov[i][j][k] = envov[iprop][i][j][k] * envovtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the user-defined reaction value f(x,y,z)
								udrpropov[i][j][k] = envov[iprop][i][j][k];

							}	//end if envovtfid > 0

						}	//end loop over layers

					}	//end if imask[][] != nodatavalue

				}	//end loop over columns

			}	//end loop over rows
		}
		//else, the property is undefined
		else
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nEnvironmental Property Error:\n");
			fprintf(echofile_fp,       "  Undefined environmental property\n");
			fprintf(echofile_fp,       "  Overland Property ID = %d\n", pid);
			fprintf(echofile_fp,       "  No property for this identifier");

			//Write error message to screen
			printf(              "\n\n\nEnvironmental Property Error:\n");
			printf(                    "  Undefined environmental property\n");
			printf(                    "  Overland Property ID = %d\n", pid);
			printf(                    "  No property for this identifier");

			exit(EXIT_FAILURE);					//abort

		}	//end if pid == 1 etc...

	}	//end loop over number of overland properties

	//if channels are simulated
	if(chnopt > 0)
	{
		//Update channel environmental functions...
		//
		//loop over number of channel functions
		for(iprop=1; iprop<=npropov; iprop++)
		{
			//set property identification
			pid = pidch[iprop];

			//if the property is doc concentration (cdoc) (pid == 1)
			if(pid == 1)
			{
				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackch; k++)
						{
							//set the time function id for this cell
							tfid = envchtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the doc concentration value (g/m3) f(x,y,z,t)
								cdocch[i][j][k] = envch[iprop][i][j][k] * envchtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the doc concentration value (g/m3) f(x,y,z)
								cdocch[i][j][k] = envch[iprop][i][j][k];

							}	//end if envchtfid > 0

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links
			}
			//if the property is effective fraction of doc (fdoc) (pid == 2)
			else if(pid == 2)
			{
				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackch; k++)
						{
							//set the time function id for this cell
							tfid = envchtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the fdoc value (dimensionless) f(x,y,z,t)
								fdocch[i][j][k] = envch[iprop][i][j][k] * envchtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the fdoc value (dimensionless) f(x,y,z)
								fdocch[i][j][k] = envch[iprop][i][j][k];

							}	//end if envchtfid > 0

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links
			}
			//if the property is hardness (pid == 3)
			else if(pid == 3)
			{
				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackch; k++)
						{
							//set the time function id for this cell
							tfid = envchtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the hardness value (g/m3) f(x,y,z,t)
								hardnessch[i][j][k] = envch[iprop][i][j][k] * envchtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the hardness value (g/m3) f(x,y,z)
								hardnessch[i][j][k] = envch[iprop][i][j][k];

							}	//end if envchtfid > 0

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links
			}
			//if the property is pH (pid == 4)
			else if(pid == 4)
			{
				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackch; k++)
						{
							//set the time function id for this cell
							tfid = envchtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the pH value (s.u.) f(x,y,z,t)
								phch[i][j][k] = envch[iprop][i][j][k] * envchtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the pH value (s.u.) f(x,y,z)
								phch[i][j][k] = envch[iprop][i][j][k];

							}	//end if envchtfid > 0

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links
			}
			//if the property is water/sediment temperature (pid == 5)
			else if(pid == 5)
			{
				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackch; k++)
						{
							//set the time function id for this cell
							tfid = envchtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the water/sediment temperature value (C) f(x,y,z,t)
								temperaturech[i][j][k] = envch[iprop][i][j][k] * envchtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the water/sediment temperature value (C) f(x,y,z)
								temperaturech[i][j][k] = envch[iprop][i][j][k];

							}	//end if envchtfid > 0

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links
			}
			//if the property is oxidant/radical concentration (pid == 6)
			else if(pid == 6)
			{
				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackch; k++)
						{
							//set the time function id for this cell
							tfid = envchtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the oxidant/radical concentration value (g/m3) f(x,y,z,t)
								oxradch[i][j][k] = envch[iprop][i][j][k] * envchtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the oxidant/radical concentration value (g/m3) f(x,y,z)
								oxradch[i][j][k] = envch[iprop][i][j][k];

							}	//end if envchtfid > 0

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links
			}
			//if the property is bacterial concentration (pid == 7)
			else if(pid == 7)
			{
				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackch; k++)
						{
							//set the time function id for this cell
							tfid = envchtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the bacteria concentration value (cells/100 mL) f(x,y,z,t)
								bacteriach[i][j][k] = envch[iprop][i][j][k] * envchtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the bacteria concentration value (cells/100 mL) f(x,y,z)
								bacteriach[i][j][k] = envch[iprop][i][j][k];

							}	//end if envchtfid > 0

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links
			}
			//if the property is light extinction coefficient (pid == 8)
			else if(pid == 8)
			{
				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//set layer to water column
						k=0;

						//set the time function id for this cell
						tfid = envchtfid[iprop][i][j][k];

						//if a time function exists for this cell (tfid > 0)
						if(tfid > 0)
						{
							//set the light extinction coefficient value (1/m) f(x,y,z,t)
							extinctionch[i][j][k] = envch[iprop][i][j][k] * envchtfinterp[iprop][tfid];
						}
						else	//else, no time function exists
						{
							//set the light extinction coefficient value (1/m) f(x,y,z)
							extinctionch[i][j][k] = envch[iprop][i][j][k];

						}	//end if envchtfid > 0

					}	//end loop over nodes

				}	//end loop over links
			}
			//if the property is user-defined reaction property (pid == 9)
			else if(pid == 9)
			{
				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//loop over layers (include zero for water column)
						for(k=0; k<=maxstackch; k++)
						{
							//set the time function id for this cell
							tfid = envchtfid[iprop][i][j][k];

							//if a time function exists for this cell (tfid > 0)
							if(tfid > 0)
							{
								//set the user-defined reaction value f(x,y,z,t)
								udrpropch[i][j][k] = envch[iprop][i][j][k] * envchtfinterp[iprop][tfid];
							}
							else	//else, no time function exists
							{
								//set the user-defined reaction value f(x,y,z)
								udrpropch[i][j][k] = envch[iprop][i][j][k];

							}	//end if envchtfid > 0

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links
			}
			//else, the property is undefined
			else
			{
				//Write error message to file
				fprintf(echofile_fp, "\n\n\nEnvironmental Property Error:\n");
				fprintf(echofile_fp,       "  Undefined environmental property\n");
				fprintf(echofile_fp,       "  Channel Property ID = %d\n", pid);
				fprintf(echofile_fp,       "  No property for this identifier");

				//Write error message to screen
				printf(              "\n\n\nEnvironmental Property Error:\n");
				printf(                    "  Undefined environmental property\n");
				printf(                    "  Channel Property ID = %d\n", pid);
				printf(                    "  No property for this identifier");

				exit(EXIT_FAILURE);					//abort

			}	//end if pid == 1 etc...

		}	//end loop over number of channel properties

	}	//end if chnopt > 0

	//if fpocovopt > 0 (overland particle fpoc is specified)
	if(fpocovopt > 0)
	{
		//Update overland particle functions...
		//
		//loop over number of overland particle functions
		for(i=1; i<=nsolids; i++)
		{
			//If it is time to update any time function for this property (simtime >= ntfpocov[])
			if(simtime >= ntfpocovtf[i])
			{
				//Initialize the lower and upper bounds of the _next_ time window
				//in which the functions are to be updated.
				nt = 1.0e+6;	//lower bound for next time window
				pt = 0.0;		//upper bound for next time window

				//Loop over number of functions
				for(j=1; j<=nfpocovtf[i]; j++)
				{
					//if it is time to update values for this function...
					//
					//nfpocovtft = next time value in property time series
					//pfpocovtft = prior time value in property time series
					//
					if(simtime >= nfpocovtft[i][j] || simtime < pfpocovtft[i][j])
					{
						//If the simulation time is greater than the last time specified,
						//start again (cyclically) with the first value in the array.  So
						//work with the current time, modulo the last entry in the array.
						endtime = fpocovtftime[i][j][nfpocovtfpairs[i][j]];

						//mtime = mod(simtime,endtime)	(original code from ipx)
						//
						//mtime is remaining part of time series
						mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

						//Pointer to the last "current" time interval index for this 
						//function.
						ip = fpocovtfpoint[i][j];
							
						//Search upward or downward to get the index corresponding
						//to the current simulation time.
						while(mtime >= fpocovtftime[i][j][ip+1])
						{
							ip = ip + 1;

						}	//end while
								
						while((mtime < fpocovtftime[i][j][ip]) && (ip != 1))
						{
							ip = ip - 1;

						}	//end while

						//Linear interpolation is perfomed following the form:
						//
						//  y = m x + b
						//
						//Compute intercept (b) for new interval
						bfpocovtf[i][j] = fpocovtf[i][j][ip+1];

						//Compute slope (m) for new interval
						mfpocovtf[i][j] = (fpocovtf[i][j][ip] - fpocovtf[i][j][ip+1])
							/ (fpocovtftime[i][j][ip] - fpocovtftime[i][j][ip+1]);
						
						//Number of times cycled through the function time interval array for the
						//current value of the simulation time.
						ncycle = (int)(simtime / endtime);

						//Next time interval upper bound to recalculate this function
						nfpocovtft[i][j] = (float)(ncycle * endtime) + fpocovtftime[i][j][ip+1];

						//Next time interval lower bound to recalculate this function
						pfpocovtft[i][j] = (float)(ncycle * endtime) + fpocovtftime[i][j][ip];

						//Save the pointer to the current time interval for this function
						fpocovtfpoint[i][j] = ip;

					}	//End if simtime >= fpocovtft or simtime < fpocovtft

					//Get the necessary time window bounds to update any function
					if (nfpocovtft[i][j] < nt) nt = nfpocovtft[i][j];
					if (pfpocovtft[i][j] > pt) pt = pfpocovtft[i][j];
						
				}	//end loop over number functions for the current property

				//Store the lower time window to update functions for this property (function with lowest time break)
				ntfpocovtf[i] = nt;

			}	//end if simtime >= ntfpocovtf[i]

 			//Loop over number of functions to update interpolated value
 			//each time step using current slope and intercept
 			for(j=1; j<=nfpocovtf[i]; j++)
 			{
 				//Compute linearly interpolated values for this time step
 				fpocovtfinterp[i][j] = (float)(mfpocovtf[i][j] * (simtime - nfpocovtft[i][j]) + bfpocovtf[i][j]);
		
 			}	//end loop over number of functions

		}	//end loop over number of solids

	}	//end if fpocovopt > 0

	//if channels are simulated
	if(chnopt > 0)
	{
		//if fpocchopt > 0 (channel particle fpoc is specified)
		if(fpocchopt > 0)
		{
			//Update channel particle functions...
			//
			//loop over number of channel particle functions
			for(i=1; i<=nsolids; i++)
			{
				//If it is time to update any time function for this property (simtime >= ntfpocch[])
				if(simtime >= ntfpocchtf[i])
				{
					//Initialize the lower and upper bounds of the _next_ time window
					//in which the functions are to be updated.
					nt = 1.0e+6;	//lower bound for next time window
					pt = 0.0;		//upper bound for next time window

					//Loop over number of functions
					for(j=1; j<=nfpocchtf[i]; j++)
					{
						//if it is time to update values for this function...
						//
						//nfpocchtft = next time value in property time series
						//pfpocchtft = prior time value in property time series
						//
						if(simtime >= nfpocchtft[i][j] || simtime < pfpocchtft[i][j])
						{
							//If the simulation time is greater than the last time specified,
							//start again (cyclically) with the first value in the array.  So
							//work with the current time, modulo the last entry in the array.
							endtime = fpocchtftime[i][j][nfpocchtfpairs[i][j]];

							//mtime = mod(simtime,endtime)	(original code from ipx)
							//
							//mtime is remaining part of time series
							mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

							//Pointer to the last "current" time interval index for this 
							//wind function.
							ip = fpocchtfpoint[i][j];
								
							//Search upward or downward to get the index corresponding
							//to the current simulation time.
							while(mtime >= fpocchtftime[i][j][ip+1])
							{
								ip = ip + 1;

							}	//end while
									
							while((mtime < fpocchtftime[i][j][ip]) && (ip != 1))
							{
								ip = ip - 1;

							}	//end while

							//Linear interpolation is perfomed following the form:
							//
							//  y = m x + b
							//
							//Compute intercept (b) for new interval
							bfpocchtf[i][j] = fpocchtf[i][j][ip+1];

							//Compute slope (m) for new interval
							mfpocchtf[i][j] = (fpocchtf[i][j][ip] - fpocchtf[i][j][ip+1])
								/ (fpocchtftime[i][j][ip] - fpocchtftime[i][j][ip+1]);
							
							//Number of times cycled through the function time interval array for the
							//current value of the simulation time.
							ncycle = (int)(simtime / endtime);

							//Next time interval upper bound to recalculate this function
							nfpocchtft[i][j] = (float)(ncycle * endtime) + fpocchtftime[i][j][ip+1];

							//Next time interval lower bound to recalculate this function
							pfpocchtft[i][j] = (float)(ncycle * endtime) + fpocchtftime[i][j][ip];

							//Save the pointer to the current time interval for this function
							fpocchtfpoint[i][j] = ip;

						}	//End if simtime >= fpocchtft or simtime < fpocchtft

						//Get the necessary time window bounds to update any function
						if (nfpocchtft[i][j] < nt) nt = nfpocchtft[i][j];
						if (pfpocchtft[i][j] > pt) pt = pfpocchtft[i][j];
							
					}	//end loop over number functions for the current property

					//Store the lower time window to update functions for this property (function with lowest time break)
					ntfpocchtf[i] = nt;

				}	//end if simtime >= ntfpocchtf[i]

 				//Loop over number of functions to update interpolated value
 				//each time step using current slope and intercept
 				for(j=1; j<=nfpocchtf[i]; j++)
 				{
 					//Compute linearly interpolated values for this time step
 					fpocchtfinterp[i][j] = (float)(mfpocchtf[i][j] * (simtime - nfpocchtft[i][j]) + bfpocchtf[i][j]);
			
 				}	//end loop over number of functions

			}	//end loop over number of solids

		}	//end if fpocchopt > 0

	}	//end if chnopt > 0

//End of function: Return to trex
}
