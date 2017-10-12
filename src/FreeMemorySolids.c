/*----------------------------------------------------------------------
C-  Function:	FreeMemorySolids.c                                              
C-
C-	Purpose/    Free allocated memory for solids variables
C-  Methods     at end of model run.
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:   None
C-
C-	Calls:		None
C-
C-	Called by:	FreeMemory
C-
C-	Created:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-	Date:		19-MAR-2004
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

void FreeMemorySolids()
{
	//local variable declarations/definitions
	int
		isolid;		//loop index for solids number (type)

/*
	int
		ilayer,		//index for surface layer in the sediment stack
		row,		//row number of node (for reporting)
		col;		//column number of node (for reporting)
*/

	/**********************************************/
	/*        Free ReadDataGroupC Variables       */
	/**********************************************/

/*
	//Loop over number of solids reporting groups
	for(i=1; i<=nsgroups; i++)
	{
		//Free memory for solids reporting group names
		free(sgroupname[i]);

	}	//end loop over reporting groups

	//Free remaining memory for solids reporting group names
	free(sgroupname);
*/

	//Free memory for particle characteristics
	free(ds);
	free(spgravity);
	free (ws);

	//if any of the overland or channel deposition/erosion options > 1
	if(depovopt > 1 || depchopt > 1 || ersovopt > 1 || erschopt > 1)
	{
		free(cncopt);

		//if the overland deposition option > 1
		if(depovopt > 1)
		{
			free(tcdov);

		}	//end if depovopt > 1

		//if the overland erosion option > 1
		if(ersovopt > 1)
		{
			free(tceov);
			free(zageov);

		}	//end if ersovopt > 1

		//if channels are simulated
		if(chnopt > 0)
		{
			//if the channel deposition option > 1
			if(depchopt > 1)
			{
				free(tcdch);

			}	//end if depchopt > 1

			//if the channel erosion option > 1
			if(erschopt > 1)
			{
				free(tcech);
				free(zagech);

			}	//end if erschopt > 1

		}	//end if chnopt > 0

	}	//end if depovopt > 1 || depchopt > 1 || ersovopt > 1 || erschopt > 1

/*
	//Free memory for particle name
	//
	//loop over solids
	for(i=1; i<=nsolids; i++)
	{
		free(particlename[i]);

	}	//end loop over solids

	//Free remaining memory for particle name and reporting group, etc...
	free(particlename);
*/
	free(sgroupnumber);

	//Check value of infopt and free memory
	if(infopt == 1)
	{
		//Free memory for infiltration parameters
		free(khsoil);
		free(capshsoil);
		free(soilmd);

	}	//end check of infopt

	//Free memory for soil erosion parameters
	//
	//if the erosion option <= 1
	if(ersovopt <= 1)
	{
		//Free memory
		free(kusle);
	}
	else	//else ersovopt > 1
	{
		//Free memory for erosion exponent
		free(mexpov);

	}	//end if ersovopt <= 1

	//Free memory for porosity
	free(porosityov);

/*
	//Free memory for soil names
	//
	//loop over soils
	for(i=1; i<= nsoils; i++)
	{
		//Free memory for soil names
		free(soilname[i]);

	}	//end loop over soils

	//Free remaining memory for soil names
	free(soilname);
*/

	//Free memory for land use class parameters
	//
	//if the overland erosion option <= 1
	if(ersovopt <= 1)
	{
		//Free memory for land use class parameters
		free(cusle);
		free(pusle);
	}
	else	//else ersovopt > 1
	{
		//Free memory for soil erosion yield
		free(ayov);

	}	//end if ersovopt <= 1

	//Free memory for Manning n and interception
	free(nmanningov);
	free(interceptionclass);

/*
	//Free memory for land use names...
	//
	//Loop over number of land use classes
	for(i=1; i<=nlands; i++)
	{
		//Free memory for land use names
		free(landname[i]);	//allocate memory

	}	//end loop over number of land uses classes

	//Free remaining memory for land use names
	free(landname);
*/

	//Free memory for soil stack file name
//mlv	free(soilstackfile);

	//Free memory for soil element properties (thickness, soil type)...
	//
	//Free memory for soil layer property file names
//mlv	free(soillayerthicknessfile);
//mlv	free(soiltypefile);

	//loop over soils
	for(i=1; i<=nsoils; i++)
	{
		//Free memory for soil gsd
		free(gsdov[i]);		//soil layer gain size distribution (dimensionless)

	}	//end loop over solids

	//Free remaining memory for soil gsd
	free(gsdov);		//soil layer gain size distribution (dimensionless)

	//loop over rows
	for(i=1; i<=nrows; i++)
	{
		//loop over columns
		for(j=1; j<=ncols; j++)
		{
			//Free memory for soil layer thickness
			free(hlayerov0[i][j]);		//initial soil layer thickness (m)

		}	//end loop over columns

		//Free additional memory for soil layer thickness
		free(hlayerov0[i]);		//initial soil layer thickness (m)

	}	//end loop over rows
	
	//Free remaining memory for soil layer thickness
	free(hlayerov0);	//initial soil layer thickness (m)

	//Free memory for overland initial suspended solids concentration file name
//mlv	free(initialssovfile);

	//Free memory for overland solids concentrations (csedov)...
	//
	//loop over solids
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//loop over columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] > 0)
				{
					//Free memory for overland concentrations
					free(csedov[isolid][i][j]);

				}	//end if imask[][] > 0

			}	//end loop over columns

			//Free additional memory for overland concentrations
			free(csedov[isolid][i]);

		}	//end loop over rows

		//Free additional memory for overland concentrations
		free(csedov[isolid]);

	}	//end loop over solids

	//Free remaining memory for overland concentrations
	free(csedov);

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Free memory for sediment properties file name
//mlv		free(sedimentpropertiesfile);

		//Free channel initial suspended solids file name
//mlv		free(initialsschfile);

		//Free memory for sediment properties...
		//
		//loop over solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//loop over links
			for(i=1; i<=nlinks; i++)
			{
				//loop over nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Free memory...
					free(gsdch[isolid][i][j]);

				}	//end loop over nodes

				//Free memory...
				free(gsdch[isolid][i]);

			}	//end loop over links

			//Free memory...
			free(gsdch[isolid]);

		}	//end loop over solids

		//Free remaining memory...
		free(gsdch);

		//loop over links
		for(i=1; i<=nlinks; i++)
		{
			//loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Allocate remaining memory for sediment properties
				free(porositych[i][j]);	//layer porosity (dimensionless)
				free(hlayerch0[i][j]);	//initial layer thickness (m)
				free(bwlayerch0[i][j]);	//initial layer bottom width (m)

			}	//end loop over nodes

			//Free additional memory
			free(nstackch0[i]);		//initial number of layers in sediment stack
			free(porositych[i]);	//layer porosity (dimensionless)
			free(hlayerch0[i]);		//initial layer thickness (m)
			free(bwlayerch0[i]);	//initial layer bottom width (m)

		}	//end loop over links

		//Free remaining memory for sediment layer porosity, thickness, and width
		free(nstackch0);	//initial number of layers in sediment stack
		free(porositych);	//layer porosity (dimensionless)
		free(hlayerch0);	//initial layer thickness (m)
		free(bwlayerch0);	//initial layer bottom width (m)

		//if the channel erosion option > 1
		if(erschopt > 1)
		{
			//loop over links
			for(i=1; i<=nlinks; i++)
			{
				//Free memory for sediment erosion properties
				free(aych[i]);		//initial layer thickness (m)
				free(mexpch[i]);	//initial layer bottom width (m)

			}	//end loop over links

			//Free remaining memory for sediment erosion properties
			free(aych);		//initial layer thickness (m)
			free(mexpch);	//initial layer bottom width (m)

		}	//end if erschopt > 1

		//Free memory for global channel concentration array
		//
		//loop over number of solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//loop over links
			for(i=1; i<=nlinks; i++)
			{
				//loop over nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Free memory
					free(csedch[isolid][i][j]);

				}	//end loop over nodes

				//Free memory
				free(csedch[isolid][i]);

			}	//end loop over links

			//Free memory
			free(csedch[isolid]);

		}	//end loop over solids

		//Allocate initial memory
		free(csedch);

	}	//end if chnopt > 0

	//Free memory for overland solids loads
	//
	//loop over solids
	for(i=1; i<=nsolids; i++)
	{
		//if the number of overland point source solids loads > 0
		if(nswpov[i] > 0)
		{
			//Loop over number of overland point source loads for current solids type
			for(j=1; j<=nswpov[i]; j++)
			{
				//Free memory for overland point source solids loads/forcing functions
				free(swpov[i][j]);
				free(swpovtime[i][j]);

			}	//end loop over loads

			//Free memory for overland point source solids loads/forcing functions
			free(swpovrow[i]);
			free(swpovcol[i]);
			free(nswpovpairs[i]);
			free(swpovopt[i]);
			free(swpov[i]);
			free(swpovtime[i]);

		}	//end if nswpov[i] > 0

		//if the number of overland distributed solids loads > 0
		if(nswdov[i] > 0)
		{
			//Loop over number of overland distributed loads for current solids type
			for(j=1; j<=nswdov[i]; j++)
			{
				//Free memory for overland distributed solids loads/forcing functions
				free(swdovrow[i][j]);
				free(swdovcol[i][j]);
				free(swdov[i][j]);
				free(swdovtime[i][j]);

			}	//end loop over loads

			//Free memory for overland distributed solids loads/forcing functions
			free(nswdovcells[i]);
			free(nswdovpairs[i]);
			free(swdovrow[i]);
			free(swdovcol[i]);
			free(swdov[i]);
			free(swdovtime[i]);

		}	//end if nswdov[i] > 0

	}	//end loop over solids

	//Free remaining memory for overland point source solids loads/forcing functions (w)
	free(nswpov);
	free(swpovrow);
	free(swpovcol);
	free(nswpovpairs);
	free(swpovopt);
	free(swpov);
	free(swpovtime);

	//Free remaining memory for overland distributed solids loads/forcing functions (w)
	free(nswdov);
	free(nswdovcells);
	free(swdovrow);
	free(swdovcol);
	free(nswdovpairs);
	free(swdov);
	free(swdovtime);

	//Free memory for channel solids loads...
	//
	//Check value of chnopt (1 = channels are simulated)
	if(chnopt == 1)
	{
		//loop over solids
		for(i=1; i<=nsolids; i++)
		{
			//if the number of channel solids loads > 0
			if(nswch[i] > 0)
			{
				//Loop over number of channel loads for current solids type
				for(j=1; j<=nswch[i]; j++)
				{
					//Free memory for solids loads/forcing functions
					free(swch[i][j]);
					free(swchtime[i][j]);

				}	//end loop over loads

				//Free memory for channel solids loads/forcing functions
				free(swchlink[i]);
				free(swchnode[i]);
				free(nswchpairs[i]);
				free(swchopt[i]);
				free(swch[i]);
				free(swchtime[i]);

			}	//end if nswch[i] > 0

		}	//end loop over solids

		//Free remaining memory for channel solids loads/forcing functions (w)
		free(nswch);
		free(swchlink);
		free(swchnode);
		free(nswchpairs);
		free(swchopt);
		free(swch);
		free(swchtime);

	}	//end if chnopt > 0

	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//if the domain boundary condition option for this outlet > 0
		if(dbcopt[i] > 0)
		{
			//Loop over number of solids
			for(j=1; j<=nsolids; j++)
			{
				//Free memory for solids boundary conditions
				free(sbc[i][j]);
				free(sbctime[i][j]);

			}	//end loop over number of solids

			//Allocate additional memory for solids boundary conditions
			free(nsbcpairs[i]);
			free(sbc[i]);
			free(sbctime[i]);

		}	//end if dbcopt[i] > 0

	}	//end loop over number of outlets

	//Allocate memory for solids boundary conditions
	free(nsbcpairs);
	free(sbc);
	free(sbctime);

	//Free Global Reporting Station Arrays
	//
	//Free memory for sediment reporting stations
	free(sedreprow);
	free(sedrepcol);
	free(sedarea);
	free(sedunitsopt);

	/************************************************/
	/*        Free InitializeSolids Variables       */
	/************************************************/

	/*************************************************/
	/*        Free Total Solids Mass Variables       */
	/*************************************************/

	//loop over number of solids (include zero for sum of all solids)
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain (imask != nodatavalue)
				if(imask[i][j] != nodatavalue)
				{
					//Free memory for overland solids mass by cell (values are initialized in ComputeInitialStateSolids)
					free(initialsolidsov[isolid][i][j]);
					free(finalsolidsov[isolid][i][j]);

				}	//end if imask != nodatavalue

			}	//end loop over columns

			//Free additional memory for overland solids mass by cell (values are initialized in ComputeInitialStateSolids)
			free(initialsolidsov[isolid][i]);
			free(finalsolidsov[isolid][i]);

		}	//end loop over rows

		//Free memory for cumulative overland solids mass variables
		free(initialsolidsmassov[isolid]);
		free(finalsolidsmassov[isolid]);

		//Free additional memory for total mass by cell variables
		free(initialsolidsov[isolid]);
		free(finalsolidsov[isolid]);

	}	//end loop over solids

	//Free memory for overland solids mass variables
	free(initialsolidsmassov);
	free(finalsolidsmassov);
	free(initialsolidsov);
	free(finalsolidsov);

	//if channels are simulated
	if(chnopt == 1)
	{
		//loop over number of solids (include zero for sum of all solids)
		for(isolid=0; isolid<=nsolids; isolid++)
		{
			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Free memory for channel solids mass by node (values are initialized in ComputeInitialVolume)
					free(initialsolidsch[isolid][i][j]);
					free(finalsolidsch[isolid][i][j]);

				}	//end loop over nodes

				//Free memory for channel solids mass by node (values are initialized in ComputeInitialVolume)
				free(initialsolidsch[isolid][i]);
				free(finalsolidsch[isolid][i]);

			}	//end loop over links

			//Free memory for cumulative channel solids mass variables
			free(initialsolidsmassch[isolid]);
			free(finalsolidsmassch[isolid]);

			//Free memory for channel solids mass by node variables
			free(initialsolidsch[isolid]);
			free(finalsolidsch[isolid]);

		}	//end loop over solids

		//Free memory for channel solids mass variables
		free(initialsolidsmassch);
		free(finalsolidsmassch);
		free(initialsolidsch);
		free(finalsolidsch);

	}	//end if chnopt == 1

	/******************************************************************/
	/*        Free Global Overland Solids Transport Variables         */
	/******************************************************************/

	//loop over number of solids (include zeroeth element for total solids)
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] != nodatavalue)
				{
					//Free memory for overland transport parameters
					free(csedovnew[isolid][i][j]);
					free(advsedovinflux[isolid][i][j]);
					free(advsedovinmass[isolid][i][j]);
					free(advsedovoutflux[isolid][i][j]);
					free(advsedovoutmass[isolid][i][j]);
					free(dspsedovinflux[isolid][i][j]);
					free(dspsedovinmass[isolid][i][j]);
					free(dspsedovoutflux[isolid][i][j]);
					free(dspsedovoutmass[isolid][i][j]);
					free(depsedovinflux[isolid][i][j]);
					free(depsedovinmass[isolid][i][j]);
					free(depsedovoutflux[isolid][i][j]);
					free(depsedovoutmass[isolid][i][j]);
					free(erssedovinflux[isolid][i][j]);
					free(erssedovinmass[isolid][i][j]);
					free(erssedovoutflux[isolid][i][j]);
					free(erssedovoutmass[isolid][i][j]);
					free(bursedovinmass[isolid][i][j]);
					free(bursedovoutmass[isolid][i][j]);
					free(scrsedovinmass[isolid][i][j]);
					free(scrsedovoutmass[isolid][i][j]);

				}	//end if imask[i][j] is not nodatavalue (cell is is domain)

			}	//end loop over columns

			//Free memory for overland transport parameters
			free(csedovnew[isolid][i]);
			free(advsedovinflux[isolid][i]);
			free(advsedovinmass[isolid][i]);
			free(advsedovoutflux[isolid][i]);
			free(advsedovoutmass[isolid][i]);
			free(dspsedovinflux[isolid][i]);
			free(dspsedovinmass[isolid][i]);
			free(dspsedovoutflux[isolid][i]);
			free(dspsedovoutmass[isolid][i]);
			free(depsedovinflux[isolid][i]);
			free(depsedovinmass[isolid][i]);
			free(depsedovoutflux[isolid][i]);
			free(depsedovoutmass[isolid][i]);
			free(erssedovinflux[isolid][i]);
			free(erssedovinmass[isolid][i]);
			free(erssedovoutflux[isolid][i]);
			free(erssedovoutmass[isolid][i]);
			free(bursedovinmass[isolid][i]);
			free(bursedovoutmass[isolid][i]);
			free(scrsedovinmass[isolid][i]);
			free(scrsedovoutmass[isolid][i]);
			free(transcapov[isolid][i]);
			free(transrateov[isolid][i]);
			free(depflowov[isolid][i]);
			free(ersflowov[isolid][i]);

		}	//end loop over rows

		//Free memory for solids transport parameters indexed by solids type, row, column, and source/layer
		free(csedovnew[isolid]);
		free(advsedovinflux[isolid]);
		free(advsedovinmass[isolid]);
		free(advsedovoutflux[isolid]);
		free(advsedovoutmass[isolid]);
		free(dspsedovinflux[isolid]);
		free(dspsedovinmass[isolid]);
		free(dspsedovoutflux[isolid]);
		free(dspsedovoutmass[isolid]);
		free(depsedovinflux[isolid]);
		free(depsedovinmass[isolid]);
		free(depsedovoutflux[isolid]);
		free(depsedovoutmass[isolid]);
		free(erssedovinflux[isolid]);
		free(erssedovinmass[isolid]);
		free(erssedovoutflux[isolid]);
		free(erssedovoutmass[isolid]);
		free(bursedovinmass[isolid]);
		free(bursedovoutmass[isolid]);
		free(scrsedovinmass[isolid]);
		free(scrsedovoutmass[isolid]);
		free(transcapov[isolid]);
		free(transrateov[isolid]);
		free(depflowov[isolid]);
		free(ersflowov[isolid]);

	}	//end loop over solids

	//Free memory for solids transport parameters indexed by solids type, row, column, and source/layer
	free(csedovnew);
	free(advsedovinflux);
	free(advsedovinmass);
	free(advsedovoutflux);
	free(advsedovoutmass);
	free(dspsedovinflux);
	free(dspsedovinmass);
	free(dspsedovoutflux);
	free(dspsedovoutmass);
	free(depsedovinflux);
	free(depsedovinmass);
	free(depsedovoutflux);
	free(depsedovoutmass);
	free(erssedovinflux);
	free(erssedovinmass);
	free(erssedovoutflux);
	free(erssedovoutmass);
	free(bursedovinmass);
	free(bursedovoutmass);
	free(scrsedovinmass);
	free(scrsedovoutmass);
	free(transcapov);
	free(transrateov);
	free(depflowov);
	free(ersflowov);

	//Free memory for overland mass tracking parameters
	free(totalerssedinov);
	free(totaldepsedoutov);
	free(totalswov);


	//Free memory for solids transport parameters indexed by row, column, and source/layer
	//
	//loop over rows
	for(i=1; i<=nrows; i++)
	{
		//loop over columns
		for(j=1; j<=ncols; j++)
		{
			//Free remaining memory for solids transport parameters indexed by row, column, and source/layer
			free(advinflowov[i][j]);
			free(advoutflowov[i][j]);
			free(dspflowov[i][j]);

		}	//end loop over columns

		//Free memory for solids transport parameters indexed by row, column, and source/layer
		free(advinflowov[i]);
		free(advoutflowov[i]);
		free(dspflowov[i]);

	}	//end loop over rows

	//Free memory for solids transport parameters indexed by row, column, and source/layer
	free(advinflowov);
	free(advoutflowov);
	free(dspflowov);

	/*****************************************************************/
	/*        Free Global Channel Solids Transport Variables         */
	/*****************************************************************/

	//if channels are simulated
	if(chnopt == 1)
	{
		//loop over number of solids (include zeroeth element for total solids)
		for(isolid=0; isolid<=nsolids; isolid++)
		{
			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//Loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Free memory for channel solids transport parameters
					free(csedchnew[isolid][i][j]);
					free(advsedchinflux[isolid][i][j]);
					free(advsedchinmass[isolid][i][j]);
					free(advsedchoutflux[isolid][i][j]);
					free(advsedchoutmass[isolid][i][j]);
					free(dspsedchinflux[isolid][i][j]);
					free(dspsedchinmass[isolid][i][j]);
					free(dspsedchoutflux[isolid][i][j]);
					free(dspsedchoutmass[isolid][i][j]);
					free(depsedchinflux[isolid][i][j]);
					free(depsedchinmass[isolid][i][j]);
					free(depsedchoutflux[isolid][i][j]);
					free(depsedchoutmass[isolid][i][j]);
					free(erssedchinflux[isolid][i][j]);
					free(erssedchinmass[isolid][i][j]);
					free(erssedchoutflux[isolid][i][j]);
					free(erssedchoutmass[isolid][i][j]);
					free(bursedchinmass[isolid][i][j]);
					free(bursedchoutmass[isolid][i][j]);
					free(scrsedchinmass[isolid][i][j]);
					free(scrsedchoutmass[isolid][i][j]);

				}	//end loop over nodes

				//Free memory for channel solids transport parameters
				free(csedchnew[isolid][i]);
				free(advsedchinflux[isolid][i]);
				free(advsedchinmass[isolid][i]);
				free(advsedchoutflux[isolid][i]);
				free(advsedchoutmass[isolid][i]);
				free(dspsedchinflux[isolid][i]);
				free(dspsedchinmass[isolid][i]);
				free(dspsedchoutflux[isolid][i]);
				free(dspsedchoutmass[isolid][i]);
				free(depsedchinflux[isolid][i]);
				free(depsedchinmass[isolid][i]);
				free(depsedchoutflux[isolid][i]);
				free(depsedchoutmass[isolid][i]);
				free(erssedchinflux[isolid][i]);
				free(erssedchinmass[isolid][i]);
				free(erssedchoutflux[isolid][i]);
				free(erssedchoutmass[isolid][i]);
				free(bursedchinmass[isolid][i]);
				free(bursedchoutmass[isolid][i]);
				free(scrsedchinmass[isolid][i]);
				free(scrsedchoutmass[isolid][i]);
				free(transcapch[isolid][i]);
				free(transratech[isolid][i]);
				free(depflowch[isolid][i]);
				free(ersflowch[isolid][i]);

			}	//end loop over links

			//Free memory for channel solids transport parameters indexed by link and node (and solid type and source/layer)
			free(csedchnew[isolid]);
			free(advsedchinflux[isolid]);
			free(advsedchinmass[isolid]);
			free(advsedchoutflux[isolid]);
			free(advsedchoutmass[isolid]);
			free(dspsedchinflux[isolid]);
			free(dspsedchinmass[isolid]);
			free(dspsedchoutflux[isolid]);
			free(dspsedchoutmass[isolid]);
			free(depsedchinflux[isolid]);
			free(depsedchinmass[isolid]);
			free(depsedchoutflux[isolid]);
			free(depsedchoutmass[isolid]);
			free(erssedchinflux[isolid]);
			free(erssedchinmass[isolid]);
			free(erssedchoutflux[isolid]);
			free(erssedchoutmass[isolid]);
			free(bursedchinmass[isolid]);
			free(bursedchoutmass[isolid]);
			free(scrsedchinmass[isolid]);
			free(scrsedchoutmass[isolid]);
			free(transcapch[isolid]);
			free(transratech[isolid]);
			free(depflowch[isolid]);
			free(ersflowch[isolid]);

		}	//end loop over solids

		//Free memory for channel solids transport parameters indexed by link and node (and solid type and source/layer)
		free(csedchnew);
		free(advsedchinflux);
		free(advsedchinmass);
		free(advsedchoutflux);
		free(advsedchoutmass);
		free(dspsedchinflux);
		free(dspsedchinmass);
		free(dspsedchoutflux);
		free(dspsedchoutmass);
		free(depsedchinflux);
		free(depsedchinmass);
		free(depsedchoutflux);
		free(depsedchoutmass);
		free(erssedchinflux);
		free(erssedchinmass);
		free(erssedchoutflux);
		free(erssedchoutmass);
		free(bursedchinmass);
		free(bursedchoutmass);
		free(scrsedchinmass);
		free(scrsedchoutmass);
		free(transcapch);
		free(transratech);
		free(depflowch);
		free(ersflowch);

		//loop over links
		for(i=1; i<=nlinks; i++)
		{
			//loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Free remaining memory for solids transport parameters indexed by link, node, and source
				free(advinflowch[i][j]);
				free(advoutflowch[i][j]);
				free(dspflowch[i][j]);

			}	//end loop over nodes

			//Free memory for solids transport parameters indexed by link, node, and source
			free(advinflowch[i]);
			free(advoutflowch[i]);
			free(dspflowch[i]);

		}	//end loop over links

		//Free memory for channel mass tracking parameters
		free(totalerssedinch);
		free(totaldepsedoutch);
		free(totalswch);

		//Free memory for solids transport parameters indexed by link, node, and source
		free(advinflowch);
		free(advoutflowch);
		free(dspflowch);

	}	//end if chnopt = 1

	/****************************************************************/
	/*        Free Global Soil and Sediment Stack Variables         */
	/****************************************************************/

	//Free Global Soil Stack Variables
	//
	//loop over rows
	for(i=1; i<=nrows; i++)
	{
		//loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (imask > 0)
			if(imask[i][j] > 0)
			{
				//Free remaining memory for soil stack parameters that vary with each layer
				free(hlayerov[i][j]);
				free(vlayerov0[i][j]);
				free(vlayerov[i][j]);
				free(vlayerovnew[i][j]);
				free(minvlayerov[i][j]);
				free(maxvlayerov[i][j]);
				free(elevlayerov[i][j]);

			}	//end if imask > 0

		}	//end loop over columns

//mlv: nstackov moved to water...
//mlv		//Free memory for number soil stack layers
//mlv		free(nstackov[i]);

		//Free additional memory for soil stack parameters that are constant for all layers
		free(aovground[i]);

		//Free additional memory for soil stack parameters that vary with each layer
		free(hlayerov[i]);
		free(vlayerov0[i]);
		free(vlayerov[i]);
		free(vlayerovnew[i]);
		free(minvlayerov[i]);
		free(maxvlayerov[i]);
		free(elevlayerov[i]);

	}	//end loop over rows

//mlv: nstackov moved to water...
//mlv	//Free memory for number soil stack layers
//mlv	free(nstackov);

	//Free memory for soil stack parameters that are constant for all layers
	free(aovground);

	//Free memory for soil stack parameters that vary with each layer
	free(hlayerov);
	free(vlayerov0);
	free(vlayerov);
	free(vlayerovnew);
	free(minvlayerov);
	free(maxvlayerov);
	free(elevlayerov);

	//Free Global Sediment Stack Variables
	//
	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//loop over links
		for(i=1; i<=nlinks; i++)
		{
			//loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Free memory for sediment stack parameters that vary with each layer
				free(hlayerch[i][j]);
				free(bwlayerch[i][j]);
				free(achbed[i][j]);
				free(vlayerch0[i][j]);
				free(vlayerch[i][j]);
				free(minvlayerch[i][j]);
				free(maxvlayerch[i][j]);
				free(elevlayerch[i][j]);

				free(vlayerchnew[i][j]);

			}	//end loop over nodes

			//Free memory for number sediment stack layers
			free(nstackch[i]);

			//Free additional memory for sediment stack parameters that vary with each layer
			free(hlayerch[i]);
			free(bwlayerch[i]);
			free(achbed[i]);
			free(vlayerch0[i]);
			free(vlayerch[i]);
			free(minvlayerch[i]);
			free(maxvlayerch[i]);
			free(elevlayerch[i]);

			free(vlayerchnew[i]);

		}	//end loop over links

		//Free memory for number sediment stack layers
		free(nstackch);

		//Free memory for sediment stack parameters that vary with each layer
		free(hlayerch);
		free(bwlayerch);
		free(achbed);
		free(vlayerch0);
		free(vlayerch);
		free(minvlayerch);
		free(maxvlayerch);
		free(elevlayerch);

		free(vlayerchnew);

	}	//end if chnopt > 0

	/**************************************************************/
	/*        Free Global Solids Reporting Station Arrays         */
	/**************************************************************/

	//Loop over number of sediment reporting stations
	for(i=1; i<=nsedreports; i++)
	{
		//Free memory for reported sediment discharges (export)
		free(sedreportov[i]);	//reported sediment discharge (units vary)
		free(sedreportch[i]);	//reported sediment discharge (units vary)

	}	//end loop over sediment reporting stations

	//Free remaining memory for reported sediment discharges (export)
	free(sedreportov);	//reported sediment discharge (units vary)
	free(sedreportch);	//reported sediment discharge (units vary)

	/****************************************************/
	/*        Free Outlet Cell Solids Variables         */
	/****************************************************/

	//loop over solids (use zeroeth element for sum of all solids)
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Free memory for overland outlet mass tracking variables
		free(totaladvsedinov[isolid]);
		free(totaladvsedoutov[isolid]);
		free(totaldspsedinov[isolid]);
		free(totaldspsedoutov[isolid]);
		free(sedflowpeakov[isolid]);
		free(sedtimepeakov[isolid]);

	}	//end loop over solids

	//Free memory for overland outlet mass tracking variables
	free(totaladvsedinov);
	free(totaladvsedoutov);
	free(totaldspsedinov);
	free(totaldspsedoutov);
	free(sedflowpeakov);
	free(sedtimepeakov);

	//if channels are simulated
	if(chnopt > 0)
	{
		//loop over solids (include zeroeth element for sum of all solids)
		for(isolid=0; isolid<=nsolids; isolid++)
		{
			//Allocate remaining memory for channel outlet mass tracking variables
			free(totaladvsedinch[isolid]);
			free(totaladvsedoutch[isolid]);
			free(totaldspsedinch[isolid]);
			free(totaldspsedoutch[isolid]);
			free(sedflowpeakch[isolid]);
			free(sedtimepeakch[isolid]);

		}	//end loop over solids

		//Allocate memory for channel outlet mass tracking variables
		free(totaladvsedinch);
		free(totaladvsedoutch);
		free(totaldspsedinch);
		free(totaldspsedoutch);
		free(sedflowpeakch);
		free(sedtimepeakch);

	}	//end if chnopt > 0

	/****************************************************************/
	/*        Free Solids Forcing Function and BC Variables         */
	/****************************************************************/

	//Include forcing functions (loads) and boundary conditions (outlets)
	//
	//Loop over number of solids types
	for(i=1; i<=nsolids; i++)
	{
		//Free memory for overland point source solids (soil) loads
		free(nswpovt[i]);
		free(pswpovt[i]);
		free(bswpov[i]);
		free(mswpov[i]);
		free(swpovpoint[i]);
		free(swpovinterp[i]);

		//Free memory for overland distributed solids (soil) loads
		free(nswdovt[i]);
		free(pswdovt[i]);
		free(bswdov[i]);
		free(mswdov[i]);
		free(swdovpoint[i]);
		free(swdovinterp[i]);

		//Free memory for solids boundary conditions
		free(nsbct[i]);
		free(psbct[i]);
		free(bsbc[i]);
		free(msbc[i]);
		free(sbcpoint[i]);
		free(sbcinterp[i]);

	}	//end loop over number of solids

	//Free memory for overland point source solids (soil) loads
	free(nswpovt);
	free(pswpovt);
	free(bswpov);
	free(mswpov);
	free(swpovpoint);
	free(swpovinterp);
	free(ntswpov);

	//Free memory for overland distributed solids (soil) loads
	free(nswdovt);
	free(pswdovt);
	free(bswdov);
	free(mswdov);
	free(swdovpoint);
	free(swdovinterp);
	free(ntswdov);

	//Free memory for overland solids boundary conditions
	free(nsbct);
	free(psbct);
	free(bsbc);
	free(msbc);
	free(sbcpoint);
	free(sbcinterp);
	free(ntsbc);

	//if channels are simulated
	if(chnopt == 1)
	{
		//Free arrays related to sediment loads
		//
		//Loop over number of solids types
		for(i=1; i<=nsolids; i++)
		{
			//Free memory for channel solids (sediment) loads
			free(nswcht[i]);
			free(pswcht[i]);
			free(bswch[i]);
			free(mswch[i]);
			free(swchpoint[i]);
			free(swchinterp[i]);

		}	//end loop over number of solids

		//Free memory for channel solids loads
		free(nswcht);
		free(pswcht);
		free(bswch);
		free(mswch);
		free(swchpoint);
		free(swchinterp);
		free(ntswch);

	}	//end if chhnopt = 1

	/****************************************************/
	/*        Free Solids Export And Grid Files         */
	/****************************************************/

	//loop over number of solids reports (include zero for sum of solids)
	for(i=0; i<=nsgroups; i++)
	{
		//Free memory for sediment export file
		free(sedexpfile[i]);

	}	//end loop over number of solids reports

	//Free memory for solids export file names
	free(sedexpfile);

	//Free grid files...
	//
	//loop over number of solids reports
	for(i=0; i<=nsgroups; i++)
	{
		//Free memory for solids grid files
		free(solidsconcwatergrid[i]);
		free(solidsconcsurfgrid[i]);
		free(solidserosiongrid[i]);
		free(solidsdepositiongrid[i]);
		free(solidsnetaccumgrid[i]);

	}	//end loop over number of reporting groups

	//Free memory for net elevation grid file
//mlv	free(netelevationgrid);

	//Free memory for solids grid file names
	free(solidsconcwatergrid);
	free(solidsconcsurfgrid);
	free(solidserosiongrid);
	free(solidsdepositiongrid);
	free(solidsnetaccumgrid);

	/******************************************************/
	/*        Free Min and Max Statistics Variables       */
	/******************************************************/

	//Free memory for minimum and maximum overland concentrations
	free(maxcsedov0);
	free(mincsedov0);
	free(maxcsedov1);
	free(mincsedov1);

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Free memory for minimum and maximum channel concentrations
		free(maxcsedch0);
		free(mincsedch0);
		free(maxcsedch1);
		free(mincsedch1);

	}	//end if chnopt > 0

//End of function: Return to FreeMemory
}
