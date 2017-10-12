/*----------------------------------------------------------------------
C-  Function:   FreeMemoryWater.c                                              
C-
C-	Purpose/    Free allocated memory for water variables
C-  Methods:    from ReadDataGroupB, InitializeWater,
C-              and functions called within ReadDataGroupB
C-
C-	Inputs:	    None
C-
C-	Outputs:    None
C-
C-	Controls:   None
C-
C-	Calls:	    None
C-
C-	Called by:  FreeMemory 
C-
C-	Created:    Rosalia Rojas-Sanchez
C-              Department of Civil Engineering
C-              Colorado State University
C-		        Fort Collins, CO 80523
C-
C-	Date:       19-JUN-2003 (assumed date)
C-
C-	Revised:    John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO 80523
C-
C-	Date:       23-MAY-2004
C-
C-	Revisions:  Complete Rewrite
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

void FreeMemoryWater()
{
	//jfe NEED TO move to Initalize:WriteGrids

	/**********************************************/
	/*        Free ReadDataGroupB Variables       */
	/**********************************************/

	//Free memory for major input grids (mandatory inputs)

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Free memory for mask
		free(imask[i]);

		//Free memory for overland elevation grid
		free(elevationov[i]);

		//Free memory for land use grid
		free(landuse[i]);

		//Free memory for storage depth grid
		free(storagedepth[i]);

		//Free memory for overland depth grid
		free(hov[i]);

	}	//End loop over rows

	//Free remaining memory for mask
	free(imask);

	//Free remaining memory for overland elevation grid
	free(elevationov);

	//Free remaining memory for land use grid
	free(landuse);

	//Free remaining memory for overland depth grid
	free(hov);

	//Free memory for major input grids (optional inputs)
	//
	//If infiltration is simulated
	if(infopt == 1)
	{
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Free memory for soil type
			free(soiltype[i]);

            //Free memory for location (layer) in the overland soil stack
            free(nstackov[i]);  //soil stack layer

		}	//End loop over rows

		//Free remaining memory for soil type
		free(soiltype);

        //Free memory for location (layer) in the overland soil stack
        free(nstackov);  //soil stack layer

	}	//end if infiltration is simulated

	//If channels are simulated
	if(chnopt == 1)
	{
		//Loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//Free memory for links
			free(link[i]);

			//Free memory for nodes
			free(node[i]);

			//Free memory for channel depth
			free(hch[i]);

			//Free memory for channel properties
			free(bwidth[i]);
			free(sideslope[i]);
			free(hbank[i]);
			free(nmanningch[i]);
			free(sinuosity[i]);
			free(deadstoragedepth[i]);
			free(twidth[i]);

			//Free memory for channel topology variables
			free(nupbranches[i]);
			free(ndownbranches[i]);
			free(ichnrow[i]);
			free(ichncol[i]);

			//loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Free memory for directions between nodes
				free(updirection[i][j]);
				free(downdirection[i][j]);

			}	//end loop over nodes

			//Free momory for directions between nodes
			free(updirection[i]);
			free(downdirection[i]);

			//Free memory for channel elevation
			free(elevationch[i]);

			//Free memory for channel length variables
			free(chanlength[i]);
			free(lengthup[i]);
			free(lengthdown[i]);

		}	//end loop over links

		//Free remaining memory for links
		free(link);

		//Free remaining memory for node and nnodes
		free(node);
		free(nnodes);

		//Free remaining memory for channel depth
		free(hch);

		//Free remaining memory for channel properties
		free(bwidth);
		free(sideslope);
		free(hbank);
		free(nmanningch);
		free(sinuosity);
		free(deadstoragedepth);
		free(twidth);

		//Free remaining memory for channel topology variables
		free(nupbranches);
		free(ndownbranches);
		free(ichnrow);
		free(ichncol);
		free(updirection);
		free(downdirection);

		//Free memory for channel elevation
		free(elevationch);

		//Free memory for channel length variables
		free(chanlength);
		free(lengthup);
		free(lengthdown);

	}     //End if channels are simulated

    //Check value of ksim
    if(ksim == 1)
	{
      //Check value of infopt
      if(infopt == 1)
      {
        //jfe comment out  NEED TO CHECK ALL FREE OF CHAR

/*        //Loop over the number of soil types
        for(i=1; i<=nsoils; i++)
        {
          //Free memory for soilname
          free(soilname[i]);

        }  //End loop over soil types
*/

        //Free memory for infitration parameters
        free(khsoil);
        free(capshsoil);
        free(soilmd);

//jfe      //Free remaining memory for soilname
//jfe      free(soilname);

      }  //End check of infopt

      //Loop over number of land use classes to free memory
      for(i=1; i<=nlands; i++)
      {
        free(landname[i]);

      }  //End loop over land use classes

      //Free memory to land use class parameters
      free(nmanningov);
      free(interceptionclass);

      //Free remaining memory for landname
      free(landname);

	}  //end if ksim == 1

	//if rainopt <= 1 (uniform or distributed gage data)
	if(rainopt <= 1)
	{
		//if raingages are specified (nrg > 0)
		if(nrg > 0)
		{
			//Loop over number of rain gages (rainfall functions)
			for(i=1; i<=nrg; i++)
			{
				//Free memory for rainfall parameters
				free(rfintensity[i]);
				free(rftime[i]);

			}     //end loop over rain gages (rainfall functions)

			//Free remaining memory for rain gage parameters
			free(rgid);		//rain gage id
			free(rgx);		//Gage x coordinate (m)
			free(rgy);		//Gage y coordinate (m)
			free(nrpairs);	//number of pairs in rainfall time series
			free(rfintensity);	//value of rainfall intentsity (m/s)
			free(rftime);	//time break in rainfall intensity time series (days)

		}     //end if nrg > 0

	}	//end if rainopt <= 1

//mlv: remove this if here or add to InitializeWater
	//if external overland flows are specified (nqwov > 0)
	if(nqwov > 0)
	{
		//Free memory for overland flow parameters
		//Loop over number of overland flow functions
		for(i=1; i<=nqwov; i++)
		{
			free(qwovdescription[i]);	//overland flow description
			free(qwov[i]);		//external flow (m3/s)
			free(qwovtime[i]);		//time break (hrs)

		}	//end loop over number of overland flow functions

		//Free remaining memory for external overland flow parameters
		free(qwovrow);		//flow source row number
		free(qwovcol);		//flow source column number
		free(nqwovpairs);	//number of pairs
		free(qwovdescription);	//flow description
		free(qwov);		//external flow (m3/s)
		free(qwovtime);		//time break in flow function (days)

	}	//end if nqwov > 0

	//Check value of chnopt (channel option)
	if(chnopt == 1)
	{
//mlv: remove this if here or add to InitializeWater
		//if external channel flows are specified (nqwch > 0)
		if(nqwch > 0)
		{
			//Free memory for channel flow parameters
			//Loop over number of channel flow functions
			for(i=1; i<=nqwch; i++)
			{
				free(qwchdescription[i]);	//channel flow description
				free(qwch[i]);		//external flow (m3/s)
				free(qwchtime[i]);		//time break (hrs)

			}	//end loop over number of flow functions

			//Free remaining memory for external channel flow parameters
			free(qwchlink);		//flow source link number
			free(qwchnode);		//flow source node number
			free(nqwchpairs);		//number of pairs
			free(qwchdescription);	//flow description
			free(qwch);			//external flow (m3/s)
			free(qwchtime);		//time break in flow function (days)

		}	//end if nqwch > 0

	}	//end check of chnopt

	/***********************************************/
	/*        Free InitializeWater Variables       */
	/***********************************************/

	/*********************************/
	//  Free Total Volume Variables  */
	/*********************************/

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//free memory for overland volume parameters (values are initialized in ComputeInitialState)
		free(initialwaterov[i]);  //initial water volume in an overland cell (m3)
		free(finalwaterov[i]);	//final water volume in an overland cell (m3)

	}	//end loop over rows

	//Free remaining memory for total volume by cell variables
	free(initialwaterov);  //initial water volume in an overland cell (m3)
	free(finalwaterov);    //final water volume in an overland cell (m3)

	//if channels are simulated
	if(chnopt == 1)
	{
		//Loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//Free memory for overland volume parameters (values are initialized in ComputeInitialState)
			free(initialwaterch[i]);  //initial water volume in a channel node (m3)
			free(finalwaterch[i]);   //final water volume in a channel node (m3)

		}	//end loop over links

		//Free remaining memory for total volume by node variables
		free(initialwaterch);  //initial water volume in a channel node (m3)
		free(finalwaterch);    //final water volume in a channel node (m3)

	}	//end if chnopt == 1

	/*************************************************/
	/*        Free Global Rainfall Variables         */
	/*************************************************/

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Free memory for rainfall parameters (values are initialized in Rainfall and Interception)
		free(grossrainrate[i]);   //gross rainfall rate (m/s)
		free(grossraindepth[i]);  //cumulative gross rainfall depth (m)
		free(grossrainvol[i]);    //gross rainfall volume (m3)
		free(netrainrate[i]);     //net rainfall rate (m/s)
		free(netrainvol[i]);      //net rainfall volume (m3)

		//Free memory for interception parameters
		free(interceptiondepth[i]);  //interception depth (m)
		free(interceptionvol[i]);    //cumulative interception volume (m3)

	}	//End loop over rows

	//Free remaining memory for rainfall parameters
	free(grossrainrate);	//gross rainfall rate (m/s)
	free(grossraindepth);	//cumulative gross rainfall depth (m)
	free(grossrainvol);	//gross rainfall volume (m3)
	free(netrainrate);		//net rainfall rate (m/s)
	free(netrainvol);		//net rainfall volume (m3)

	//Free remaining memory for interception parameters
	free(interceptiondepth);	//remaining (unstatisfied) interception depth (m)
	free(interceptionvol);		//cumulative interception volume (m3)

	/*****************************************************/
	/*        Free Global Infiltration Variables         */
	/*****************************************************/

	//if infiltration is simulated
	if(infopt > 0)
	{
		//Free memory for infiltration parameters
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			free(infiltrationdepth[i]);  //infiltration depth(m)
			free(infiltrationrate[i]);   //infiltration rate (m/s)
			free(infiltrationvol[i]);    //infiltration volume (m3)

		}	//End loop over rows

		free(infiltrationdepth);  //infiltration depth(m)
		free(infiltrationrate);   //infiltration rate (m/s)
		free(infiltrationvol);    //infiltration volume (m3)

	}	//end if infopt > 0

	/***********************************************************/
	/*        Free Global Overland Transport Variables         */
	/***********************************************************/

	//Free memory for overland flow parameters
	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//jfe this crashed when looping over sources, and columns...
		//need a fix for the third dimension when using a number (11) in malloc!!

		//Loop over number of columns
/*      for(j=1; j<=ncols; j++)
		{
			free(dqovin[i][j]);		//gross overland flow into a cell (m3/s)
			free(dqovout[i][j]);		//gross overland flow out of a cell (m3/s)
			free(dqovinvol[i][j]);		//cumulative gross overland flow volume into a cell (m3)
			free(dqovoutvol[i][j]);	//cumulative gross overland flow volume out of a cell (m3)

		}	//end loop over columns
*/		
		free(hovnew[i]);      //new overland water depth (m)
		free(dqov[i]);        //net overland flow (m3/s)
		free(dqovin[i]);      //gross overland flow into a cell (m3/s)
		free(dqovout[i]);     //gross overland flow out of a cell (m3/s)
		free(dqovinvol[i]);   //cumulative gross overland flow volume into a cell (m3)
		free(dqovoutvol[i]);  //cumulative gross overland flow volume out of a cell (m3)
		free(qwovvol[i]);     //cumulative external flow volume entering a cell
		free(sfov[i]);        //friction slope for overland flow from a cell

	}	//End loop over rows

	//Free memory for overland flow parameters indexed by row and column (and direction)
	free(hovnew);      //new overland water depth (m)
	free(dqov);        //net overland flow (m3/s)
	free(dqovin);      //gross overland flow into a cell (m3/s)
	free(dqovout);     //gross overland flow out of a cell (m3/s)
	free(dqovinvol);   //cumulative gross overland flow volume into a cell (m3)
	free(dqovoutvol);  //cumulative gross overland flow volume out of a cell (m3)
	free(qwovvol);     //cumulative external flow volume entering a cell
	free(sfov);        //friction slope for overland flow from a cell

	/**********************************************************/
	/*        Free Global Channel Transport Variables         */
	/**********************************************************/

	//if channels are simulated
	if(chnopt == 1)
	{
		//Loop over number of links
		for(i=1; i<=nlinks; i++)
		{
/*		    //Loop over number of nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Free memory for gross nodal inflow and outflow
				free(dqchin[i][j]);		//gross channel flow into a node (m3/s)
				free(dqchout[i][j]);	//gross channel flow out of a node (m3/s)
				free(dqchinvol[i][j]);	//cumulative gross channel flow volume into a node (m3)
				free(dqchoutvol[i][j]);	//cumulative gross channel flow volume out of a node (m3)

			}	//End loop over nodes
*/
			//Free memory for channel flow parameters
			free(hchnew[i]);		//new channel water depth (for next time t+dt) (m)
			free(sfch[i]);			//friction slope for channel water surface
			free(dqch[i]);			//net nodal flow (m3/s)
			free(dqchin[i]);		//gross channel flow into a node (m3/s)
			free(dqchout[i]);		//gross channel flow out of a node (m3/s)
			free(dqchinvol[i]);		//cumulative gross channel flow volume into a node (m3)
			free(dqchoutvol[i]);	//cumulative gross channel flow volume out of a node (m3)
			free(qwchvol[i]);		//cumulative external flow volume entering a node (m3)

		}	//End loop over links

		//Free remaining memory for channel flow parameters
		free(hchnew);		//new channel water depth (for next time t+dt) (m)
		free(sfch);			//friction slope for channel water surface
		free(dqch);			//net nodal flow (m3/s)
		free(dqchin);		//gross channel flow into a node (m3/s)
		free(dqchout);		//gross channel flow out of a node (m3/s)
		free(dqchinvol);	//cumulative gross channel flow volume into a node (m3)
		free(dqchoutvol);	//cumulative gross channel flow volume out of a node (m3)
		free(qwchvol);		//cumulative external flow volume entering a node (m3)
		free(qinchvol);		//cumulative flow volume entering domain from a link (m3)
		free(qoutchvol);	//cumulative flow volume leaving domain from a link (m3)
		free(qinch);		//link inflow from downstream boundary (m3/s)
		free(qoutch);		//link outflow to downstream boundary (m3/s)

	}	//End if channels are simulated

	/*******************************************************/
	/*        Free Global Reporting Station Arrays         */
	/*******************************************************/

	//Free memory for reported water discharges (export) time series
	free(qreportsum);	//reported sum of overland and channel water discharge (units vary)
	free(qreportov);	//reported overland water discharge (units vary)
	free(qreportch);	//reported channel water discharge (units vary)
	free(qconvert);		//discharge export conversion factor

	/*********************************************/
	/*        Free Outlet Cell Variables         */
	/*********************************************/

	//Free memory for overland outlet cell flows
	free(qinov);		//overland inflow entering domain from an outlet (m3/s)
	free(qoutov);		//overland outflow exiting domain from an outlet (m3/s)
	free(qoutovvol);	//cumulative flow volume leaving outlet i (m3)
	free(qpeakov);		//peak flow leaving outlet i (m3/s)
	free(tpeakov);		//time of peak flow leaving outlet i (hrs)

	//if channels are simulated
	if(chnopt == 1)
	{
		//Free memory for channel outlet flows (defined by outlet)
		free(qpeakch);	//peak flow leaving outlet i (m3/s)
		free(tpeakch);	//time of peak flow leaving outlet i (hrs)

	}	//end if chnopt == 0

	/*********************************************************/
	/*        Free Forcing Function and BC Variables         */
	/*********************************************************/
	//
	//Include forcing functions (rainfall, external flows, loads) and boundary conditions
	//
	//Free memory for rainfall function arrays
	free(nrft);		//next (upper) interval time for rainfall intensity time series
	free(prft);		//prior (lower) interval time for rainfall intensity time series
	free(brf);		//intercept for rainfall intensity interpolation
	free(mrf);		//slope for rainfall intensity interpolation
	free(rfpoint);	//pointer (to position in time series) for rainfall intensity
	free(rfinterp);	//rainfall intensity interpolated in time for gage [i]

	//if user specifies external overland flows
	if (nqwov > 0)
	{
		free(nqwovt);		//next (upper) interval time for external overland flow time series
		free(pqwovt);		//prior (lower) interval time for external overland flow time series
		free(bqwov);		//intercept for external overland flow interpolation
		free(mqwov);		//slope for external overland flow interpolation
		free(qwovpoint);	//pointer (to position in time series) for external overland flows
		free(qwovinterp);	//external overland flow interpolated in time for source [i]

	}	//end if nqwov > 0

	//if channels are simulated
	if(chnopt == 1)
	{
		//if user specifies external channel flows
		if(nqwch > 0)
		{
			//Free memory for external flow function arrays
			free(nqwcht);		//next (upper) interval time for external flow time series
			free(pqwcht);		//prior (lower) interval time for external flow time series
			free(bqwch);		//intercept for external flow interpolation
			free(mqwch);		//slope for external flow interpolation
			free(qwchpoint);	//pointer (to position in time series) for external flows
			free(qwchinterp);	//external flow interpolated in time for gage [i]

		}	//end if nqwch > 0

	}	//end if chnopt = 1

	//Loop over number of outlet cells
	for(i=1; i<=noutlets; i++)
	{
		//If user specifies time series outlet BC
		if(dbcopt[i] > 0)
		{
			//free memory for boundary condition (BC) water depth function arrays
			free(nhbct);		//next (upper) interval time for water depth BC time series
			free(phbct);		//prior (lower) interval time for water depth BC  time series
			free(bhbc);			//intercept for water depth BC interpolation
			free(mhbc);			//slope for water depth BC interpolation
			free(hbcpoint);		//pointer (to position in time series) for water depth BC
			free(hbcinterp);	//water depth BC interpolated in time for outlet[i]

			//free memory for boundary condition (BC) time series input arrays
			free(hbc[i]);		//boundary water depth (m)
			free(hbctime[i]);	//time break for boundary water depth (hours)

		}	//end if dbcopt[i] > 0

	}	//end loop over number of outlets

	//Free memory for overland outlet cell properties
	free(ocx);		//outlet cell x location (m)
	free(ocy);		//outlet cell y location (m)
	free(sovout);	//outlet cell slope
	free(dbcopt);	//outlet cell domain boundary conditions option
	free(iout);		//outlet cell row
	free(jout);		//outlet cell column

	//Free memory for flow reporting stations
	free(qreprow);	//reporting station cell row
	free(qrepcol);	//reporting station cell column
	free(qarea);	//reporting station cell drainage area
	free(qunitsopt);	//flow report units option

//End of function: Return to FreeMemory
}
