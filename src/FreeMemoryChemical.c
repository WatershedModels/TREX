/*----------------------------------------------------------------------
C-  Function:	FreeMemoryChemical.c                                              
C-
C-	Purpose/    Free allocated memory for chemical variables
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
C-	Date:		20-MAR-2004
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

void FreeMemoryChemical()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical number (type)
		isolid;		//loop index for solids number (type)

	/**********************************************/
	/*        Free ReadDataGroupD Variables       */
	/**********************************************/

/*
	//Loop over number of chemical reporting groups
	for(i=1; i<=ncgroups; i++)
	{
		//Deallocate memory for chemical reporting group names
		free(cgroupname[i]);	//deallocate memory

	}	//end loop over ncgroups
*/
	//Deallocate memory for chemical identification arrays
	free(cgroupnumber);	//chemical reporting group number

/*
	//Loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//Deallocate memory for chemical name array
		free(chemname[i]);

	}	//end loop over chemicals
*/
	//Deallocate memory for chemical reporting group names
	free(cgroupname);	//deallocate memory
	free(chemname);	//name of chemical

	//if the number of yield is greater than zero
	if(ncyields > 0)
	{
		//Deallocate memory for chemical reaction yield parameters
		free(cyldfrom);		//reacting chemical
		free(cyldto);		//product chemical
		free(cyldprocess);	//reaction process pathway number
		free(cyield);	//reaction yield

	}	//end if ncyields > 0

	//Deallocate memory for soil chemical initial condition file name
//mlv	free(soilchemicfile);	//deallocate memory

	//loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
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
					//Deallocate memory for overland chemical concentration
					free(cchemov[ichem][i][j]);

				}	//end if imask[][] > 0

			}	//end loop over columns

			//Deallocate memory for overland chemical concentration
			free(cchemov[ichem][i]);

		}	//end loop over rows

		//Deallocate memory for overland chemical concentration
		free(cchemov[ichem]);

	}	//end loop over chemicals

	//Deallocate memory for global overland chemical concentration
	free(cchemov);

	//Deallocate memory for overland initial chemical water column concentration file name
//mlv	free(initialchemovfile);

	//if channels are simulated)
	if(chnopt > 0)
	{
		//Deallocate memory for sediment chemical initial condition file name
//mlv		free(sedimentchemicfile);	//deallocate memory

		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//loop over links
			for(i=1; i<=nlinks; i++)
			{
				//loop over nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Deallocate memory for channel chemical concentration
					free(cchemch[ichem][i][j]);

				}	//end loop over nodes

				//Deallocate memory for channel chemical concentration
				free(cchemch[ichem][i]);

			}	//end loop over links

			//Deallocate memory for channel chemical concentration
			free(cchemch[ichem]);

		}	//end loop over chemicals

		//Deallocate memory for global channel chemical concentration
		free(cchemch);

		//Deallocate memory for initial chemical channel file name
//mlv		free(initialchemchfile);	//deallocate memory

	}	//end if chnopt > 0

	//loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//if the number of overland point source chemical loads > 0
		if(ncwpov[i] > 0)
		{
			//Loop over number of overland point source loads for current chemical type
			for(j=1; j<=ncwpov[i]; j++)
			{
				//Deallocate memory for point source chemical loads/forcing functions
				free(cwpov[i][j]);		//deallocate memory
				free(cwpovtime[i][j]);	//deallocate memory

			}	//end loop over number of overland point source loads

			//Deallocate memory for overland point source chemical loads/forcing functions
			free(cwpovrow[i]);			//deallocate memory
			free(cwpovcol[i]);			//deallocate memory
			free(ncwpovpairs[i]);		//deallocate memory
			free(cwpovopt[i]);			//deallocate memory
			free(cwpov[i]);				//deallocate memory
			free(cwpovtime[i]);			//deallocate memory

		}	//end if ncwpov > 0

		//if the number of overland distributed chemical loads > 0
		if(ncwdov[i] > 0)
		{
			//Loop over number of overland distributed loads for current chemical type
			for(j=1; j<=ncwdov[i]; j++)
			{
				//Deallocate memory for distributed chemical loads/forcing functions
				free(cwdov[i][j]);		//deallocate memory
				free(cwdovtime[i][j]);	//deallocate memory

			}	//end loop over number of overland distributed loads

			//Deallocate memory for overland distributed chemical loads/forcing functions
			free(ncwdovpairs[i]);		//deallocate memory
			free(cwdov[i]);				//deallocate memory
			free(cwdovtime[i]);			//deallocate memory

		}	//end if ncwdov > 0

	}	//end loop over chemicals

	//Deallocate memory for overland point source chemical loads/forcing functions (w)
	free(ncwpov);			//deallocate memory
	free(cwpovrow);			//deallocate memory
	free(cwpovcol);			//deallocate memory
	free(ncwpovpairs);		//deallocate memory
	free(cwpovopt);			//deallocate memory
	free(cwpov);			//deallocate memory
	free(cwpovtime);		//deallocate memory

	//Deallocate memory for overland distributed chemical loads/forcing functions (w)
	free(ncwdov);			//deallocate memory
	free(ncwdovpairs);		//deallocate memory
	free(cwdov);			//deallocate memory
	free(cwdovtime);		//deallocate memory

	//if channels are simulated)
	if(chnopt > 0)
	{
		//loop over number of chemical
		for(i=1; i<=nchems; i++)
		{
			//if the number of channel chemical loads > 0
			if(ncwch[i] > 0)
			{
				//Loop over number of channel loads for current chemical type
				for(j=1; j <= ncwch[i]; j++)
				{
					//Deallocate memory for chemical loads/forcing functions
					free(cwch[i][j]);		//deallocate memory
					free(cwchtime[i][j]);	//deallocate memory

				}	//end loop over number of channel loads

				//Deallocate memory for chemical loads/forcing functions
				free(cwchlink[i]);		//deallocate memory
				free(cwchnode[i]);		//deallocate memory
				free(ncwchpairs[i]);		//deallocate memory
				free(cwchopt[i]);			//deallocate memory
				free(cwch[i]);		//deallocate memory
				free(cwchtime[i]);	//deallocate memory

			}	//end if ncwch > 0

		}	//end loop over number of chemical types

		//Deallocate memory for channel chemical loads/forcing functions (w)
		free(ncwch);				//deallocate memory
		free(cwchlink);			//deallocate memory
		free(cwchnode);			//deallocate memory
		free(ncwchpairs);		//deallocate memory
		free(cwchopt);			//deallocate memory
		free(cwch);		//deallocate memory
		free(cwchtime);	//deallocate memory

	}	//end if chnopt > 0

	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//if the domain boundary condition option for this outlet > 0
		if(dbcopt[i] > 0)
		{
			//Deallocate memory for chemical boundary conditions
			free(ncbcpairs[i]);		//deallocate memory
			free(cbc[i]);		//deallocate memory
			free(cbctime[i]);	//deallocate memory

			//Loop over number of chemicals
			for(j=1; j<=nchems; j++)
			{
				//Deallocate memory for chemical boundary conditions
				free(cbc[i][j]);		//deallocate memory
				free(cbctime[i][j]);	//deallocate memory

			}	//end loop over number of chemicals

		}	//end if dbcopt[i] > 0

	}	//end loop over number of outlets

	//Deallocate memory for chemical boundary conditions
	free(ncbcpairs);		//deallocate memory
	free(cbc);		//deallocate memory
	free(cbctime);	//deallocate memory

	//Deallocate memory for chemical reporting stations
	free(chemreprow);
	free(chemrepcol);
	free(chemarea);
	free(chemunitsopt);
	
	/************************************************/
	/*        Free InitializeSolids Variables       */
	/************************************************/

	/***************************************************/
	/*        Free Total Chemical Mass Variables       */
	/***************************************************/

	//loop over number of chemicals
	for(ichem=1; ichem<=nchems; ichem++)
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
					//Deallocate memory for overland chemical mass by cell (values are initialized in ComputeInitialStateChemical)
					free(initialchemov[ichem][i][j]);	//initial chemical mass in an overland cell by layer (kg)
					free(finalchemov[ichem][i][j]);		//final chemical mass in an overland cell by layer (kg)

				}	//end if imask != nodatavalue

			}	//end loop over columns

			//Deallocate memory for overland chemical mass by cell (values are initialized in ComputeInitialStateChemical)
			free(initialchemov[ichem][i]);	//initial chemical mass in an overland cell by layer (kg)
			free(finalchemov[ichem][i]);		//final chemical mass in an overland cell by layer (kg)

		}	//end loop over rows

		//Deallocate memory for cumulative overland chemical mass variables
		free(initialchemmassov[ichem]);	//initial chemical mass by layer overland (kg)
		free(finalchemmassov[ichem]);	//final chemical mass by layer overland (kg)

		//Deallocate memory for total mass by cell variables
		free(initialchemov[ichem]);	//initial chemical mass in an overland cell by layer (kg)
		free(finalchemov[ichem]);	//final chemical mass in an overland cell by layer (kg)

	}	//end loop over chemicals

	//Deallocate memory for overland chemical mass variables
	free(initialchemmassov);	//initial chemical mass by layer overland (kg)
	free(finalchemmassov);	//final chemical mass by layer overland (kg)
	free(initialchemov);	//initial chemical mass in an overland cell by layer (kg)
	free(finalchemov);	//final chemical mass in an overland cell by layer (kg)

	//if channels are simulated
	if(chnopt > 0)
	{
		//loop over number of chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Deallocate memory for channel chemical mass by node (values are initialized in ComputeInitialVolume)
					free(initialchemch[ichem][i][j]);	//initial chemical mass in a channel node by layer (kg)
					free(finalchemch[ichem][i][j]);		//final chemical mass in a channel node by layer (kg)

				}	//end loop over nodes

				//Deallocate memory for channel chemical mass by node (values are initialized in ComputeInitialVolume)
				free(initialchemch[ichem][i]);		//initial chemical mass in a channel node by layer (kg)
				free(finalchemch[ichem][i]);		//final chemical mass in a channel node by layer (kg)

			}	//end loop over links

			//Deallocate memory for cumulative channel chemical mass variables
			free(initialchemmassch[ichem]);	//initial chemical mass by layer in channels (kg)
			free(finalchemmassch[ichem]);	//final chemical mass by layer in channels (kg)

			//Deallocate memory for channel chemical mass by node variables
			free(initialchemch[ichem]);	//initial chemical mass in a channel node by layer (kg)
			free(finalchemch[ichem]);	//final chemical mass in a channel node by layer (kg)

		}	//end loop over chemicals

		//Deallocate memory for channel chemical mass variables
		free(initialchemmassch);		//initial chemical mass in layers of channels (kg)
		free(finalchemmassch);		//final chemical mass in layers of channel (kg)
		free(initialchemch);		//initial chemical mass in a channel node by layer (kg)
		free(finalchemch);		//final chemical mass in a channel node by layer (kg)

	}	//end if chnopt > 0

	/********************************************************************/
	/*        Free Global Overland Chemical Transport Variables         */
	/********************************************************************/

	//loop over number of chemicals
	for(ichem=1; ichem<=nchems; ichem++)
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
					//Deallocate memory for overland transport parameters
					free(cchemovnew[ichem][i][j]);		//new overland chemical concentration (for next time t+dt) (g/m3)
					free(advchemovinflux[ichem][i][j]);	//gross chemical flux transported into a cell by advection (g/s)
					free(advchemovinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a cell by advection (kg)
					free(advchemovoutflux[ichem][i][j]);	//gross chemical flux transported out of a cell by advection (g/s)
					free(advchemovoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a cell by advection (kg)
					free(dspchemovinflux[ichem][i][j]);	//gross chemical flux transported into a cell by dispersion (g/s)
					free(dspchemovinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a cell by dispersion (kg)
					free(dspchemovoutflux[ichem][i][j]);	//gross chemical flux transported out of a cell by dispersion (g/s)
					free(dspchemovoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a cell by dispersion (kg)
					free(depchemovinflux[ichem][i][j]);	//gross chemical flux transported into a cell by deposition (g/s)
					free(depchemovinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a cell by deposition (kg)
					free(depchemovoutflux[ichem][i][j]);	//gross chemical flux transported out of a cell by deposition (g/s)
					free(depchemovoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a cell by deposition (kg)
					free(erschemovinflux[ichem][i][j]);	//gross chemical flux transported into a cell by erosion (g/s)
					free(erschemovinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a cell by erosion (kg)
					free(erschemovoutflux[ichem][i][j]);	//gross chemical flux transported out of a cell by erosion (g/s)
					free(erschemovoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a cell by erosion (kg)
					free(pwrchemovinflux[ichem][i][j]);	//gross chemical flux transported into a cell by porewater (g/s)
					free(pwrchemovinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a cell by porewater (kg)
					free(pwrchemovoutflux[ichem][i][j]);	//gross chemical flux transported out of a cell by porewater (g/s)
					free(pwrchemovoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a cell by porewater (kg)
					free(burchemovinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a cell by burial (kg)
					free(burchemovoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a cell by burial (kg)
					free(scrchemovinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a cell by scour (kg)
					free(scrchemovoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a cell by scour (kg)

					free(biochemovinflux[ichem][i][j]);		//biodegredation influx of chemical in overland plane [ichem][row][col][layer]
					free(biochemovinmass[ichem][i][j]);		//biodegredation inflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(biochemovoutflux[ichem][i][j]);	//biodegredation outflux of chemical in overland plane [ichem][row][col][layer]
					free(biochemovoutmass[ichem][i][j]);	//biodegredation outflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(hydchemovinflux[ichem][i][j]);		//hydrolysis influx of chemical in overland plane [ichem][row][col][layer]
					free(hydchemovinmass[ichem][i][j]);		//hydrolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(hydchemovoutflux[ichem][i][j]);	//hydrolysis outflux of chemical in overland plane [ichem][row][col][layer]
					free(hydchemovoutmass[ichem][i][j]);	//hydrolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(oxichemovinflux[ichem][i][j]);		//oxidation influx of chemical in overland plane [ichem][row][col][layer]
					free(oxichemovinmass[ichem][i][j]);		//oxidation inflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(oxichemovoutflux[ichem][i][j]);	//oxidation outflux of chemical in overland plane [ichem][row][col][layer]
					free(oxichemovoutmass[ichem][i][j]);	//oxidation outflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(phtchemovinflux[ichem][i][j]);		//photolysis influx of chemical in overland plane [ichem][row][col][layer]
					free(phtchemovinmass[ichem][i][j]);		//photolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(phtchemovoutflux[ichem][i][j]);	//photolysis outflux of chemical in overland plane [ichem][row][col][layer]
					free(phtchemovoutmass[ichem][i][j]);	//photolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(radchemovinflux[ichem][i][j]);		//radioactive decay influx of chemical in overland plane [ichem][row][col][layer]
					free(radchemovinmass[ichem][i][j]);		//radioactive decay inflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(radchemovoutflux[ichem][i][j]);	//radioactive decay outflux of chemical in overland plane [ichem][row][col][layer]
					free(radchemovoutmass[ichem][i][j]);	//radioactive decay outflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(vltchemovinflux[ichem][i][j]);		//volatilization influx of chemical in overland plane [ichem][row][col][layer]
					free(vltchemovinmass[ichem][i][j]);		//volatilization inflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(vltchemovoutflux[ichem][i][j]);	//volatilization outflux of chemical in overland plane [ichem][row][col][layer]
					free(vltchemovoutmass[ichem][i][j]);	//volatilization outflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(udrchemovinflux[ichem][i][j]);		//user-defined reaction influx of chemical in overland plane [ichem][row][col][layer]
					free(udrchemovinmass[ichem][i][j]);		//user-defined reaction inflowing mass of chemical in overland plane [ichem][row][col][layer]
					free(udrchemovoutflux[ichem][i][j]);	//user-defined reaction outflux of chemical in overland plane [ichem][row][col][layer]
					free(udrchemovoutmass[ichem][i][j]);	//user-defined reaction outflowing mass of chemical in overland plane [ichem][row][col][layer]

				}	//end if imask[i][j] is not nodatavalue (cell is is domain)

			}	//end loop over columns

			//Deallocate memory for overland transport parameters
			free(cchemovnew[ichem][i]);		//new overland chemical concentration (for next time t+dt) (g/m3)
			free(advchemovinflux[ichem][i]);	//gross chemical flux transported into a cell by advection (g/s)
			free(advchemovinmass[ichem][i]);	//cumulative gross chemical mass transported into a cell by advection (kg)
			free(advchemovoutflux[ichem][i]);	//gross chemical flux transported out of a cell by advection (g/s)
			free(advchemovoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a cell by advection (kg)
			free(dspchemovinflux[ichem][i]);	//gross chemical flux transported into a cell by dispersion (g/s)
			free(dspchemovinmass[ichem][i]);	//cumulative gross chemical mass transported into a cell by dispersion (kg)
			free(dspchemovoutflux[ichem][i]);	//gross chemical flux transported out of a cell by dispersion (g/s)
			free(dspchemovoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a cell by dispersion (kg)
			free(depchemovinflux[ichem][i]);	//gross chemical flux transported into a cell by deposition (g/s)
			free(depchemovinmass[ichem][i]);	//cumulative gross chemical mass transported into a cell by deposition (kg)
			free(depchemovoutflux[ichem][i]);	//gross chemical flux transported out of a cell by deposition (g/s)
			free(depchemovoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a cell by deposition (kg)
			free(erschemovinflux[ichem][i]);	//gross chemical flux transported into a cell by erosion (g/s)
			free(erschemovinmass[ichem][i]);	//cumulative gross chemical mass transported into a cell by erosion (kg)
			free(erschemovoutflux[ichem][i]);	//gross chemical flux transported out of a cell by erosion (g/s)
			free(erschemovoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a cell by erosion (kg)
			free(pwrchemovinflux[ichem][i]);	//gross chemical flux transported into a cell by porewater (g/s)
			free(pwrchemovinmass[ichem][i]);	//cumulative gross chemical mass transported into a cell by porewater (kg)
			free(pwrchemovoutflux[ichem][i]);	//gross chemical flux transported out of a cell by porewater (g/s)
			free(pwrchemovoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a cell by porewater (kg)
			free(burchemovinmass[ichem][i]);	//cumulative gross chemical mass transported into a cell by burial (kg)
			free(burchemovoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a cell by burial (kg)
			free(scrchemovinmass[ichem][i]);	//cumulative gross chemical mass transported into a cell by scour (kg)
			free(scrchemovoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a cell by scour (kg)

			free(biochemovinflux[ichem][i]);		//biodegredation influx of chemical in overland plane [ichem][row][col][layer]
			free(biochemovinmass[ichem][i]);		//biodegredation inflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(biochemovoutflux[ichem][i]);	//biodegredation outflux of chemical in overland plane [ichem][row][col][layer]
			free(biochemovoutmass[ichem][i]);	//biodegredation outflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(hydchemovinflux[ichem][i]);		//hydrolysis influx of chemical in overland plane [ichem][row][col][layer]
			free(hydchemovinmass[ichem][i]);		//hydrolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(hydchemovoutflux[ichem][i]);	//hydrolysis outflux of chemical in overland plane [ichem][row][col][layer]
			free(hydchemovoutmass[ichem][i]);	//hydrolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(oxichemovinflux[ichem][i]);		//oxidation influx of chemical in overland plane [ichem][row][col][layer]
			free(oxichemovinmass[ichem][i]);		//oxidation inflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(oxichemovoutflux[ichem][i]);	//oxidation outflux of chemical in overland plane [ichem][row][col][layer]
			free(oxichemovoutmass[ichem][i]);	//oxidation outflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(phtchemovinflux[ichem][i]);		//photolysis influx of chemical in overland plane [ichem][row][col][layer]
			free(phtchemovinmass[ichem][i]);		//photolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(phtchemovoutflux[ichem][i]);	//photolysis outflux of chemical in overland plane [ichem][row][col][layer]
			free(phtchemovoutmass[ichem][i]);	//photolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(radchemovinflux[ichem][i]);		//radioactive decay influx of chemical in overland plane [ichem][row][col][layer]
			free(radchemovinmass[ichem][i]);		//radioactive decay inflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(radchemovoutflux[ichem][i]);	//radioactive decay outflux of chemical in overland plane [ichem][row][col][layer]
			free(radchemovoutmass[ichem][i]);	//radioactive decay outflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(vltchemovinflux[ichem][i]);		//volatilization influx of chemical in overland plane [ichem][row][col][layer]
			free(vltchemovinmass[ichem][i]);		//volatilization inflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(vltchemovoutflux[ichem][i]);	//volatilization outflux of chemical in overland plane [ichem][row][col][layer]
			free(vltchemovoutmass[ichem][i]);	//volatilization outflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(udrchemovinflux[ichem][i]);		//user-defined reaction influx of chemical in overland plane [ichem][row][col][layer]
			free(udrchemovinmass[ichem][i]);		//user-defined reaction inflowing mass of chemical in overland plane [ichem][row][col][layer]
			free(udrchemovoutflux[ichem][i]);	//user-defined reaction outflux of chemical in overland plane [ichem][row][col][layer]
			free(udrchemovoutmass[ichem][i]);	//user-defined reaction outflowing mass of chemical in overland plane [ichem][row][col][layer]

		}	//end loop over rows

		//Deallocate memory for chemical transport parameters indexed by chemical type, row, column, and source/layer
		free(cchemovnew[ichem]);		//new overland chemical concentration (for next time t+dt) (g/m3)
		free(advchemovinflux[ichem]);	//gross chemical flux transported into a cell by advection (g/s)
		free(advchemovinmass[ichem]);	//cumulative gross chemical mass transported into a cell by advection (kg)
		free(advchemovoutflux[ichem]);	//gross chemical flux transported out of a cell by advection (g/s)
		free(advchemovoutmass[ichem]);	//cumulative gross chemical mass transported out of a cell by advection (kg)
		free(dspchemovinflux[ichem]);	//gross chemical flux transported into a cell by dispersion (g/s)
		free(dspchemovinmass[ichem]);	//cumulative gross chemical mass transported into a cell by dispersion (kg)
		free(dspchemovoutflux[ichem]);	//gross chemical flux transported out of a cell by dispersion (g/s)
		free(dspchemovoutmass[ichem]);	//cumulative gross chemical mass transported out of a cell by dispersion (kg)
		free(depchemovinflux[ichem]);	//gross chemical flux transported into a cell by deposition (g/s)
		free(depchemovinmass[ichem]);	//cumulative gross chemical mass transported into a cell by deposition (kg)
		free(depchemovoutflux[ichem]);	//gross chemical flux transported out of a cell by deposition (g/s)
		free(depchemovoutmass[ichem]);	//cumulative gross chemical mass transported out of a cell by deposition (kg)
		free(erschemovinflux[ichem]);	//gross chemical flux transported into a cell by erosion (g/s)
		free(erschemovinmass[ichem]);	//cumulative gross chemical mass transported into a cell by erosion (kg)
		free(erschemovoutflux[ichem]);	//gross chemical flux transported out of a cell by erosion (g/s)
		free(erschemovoutmass[ichem]);	//cumulative gross chemical mass transported out of a cell by erosion (kg)
		free(pwrchemovinflux[ichem]);	//gross chemical flux transported into a cell by porewater (g/s)
		free(pwrchemovinmass[ichem]);	//cumulative gross chemical mass transported into a cell by porewater (kg)
		free(pwrchemovoutflux[ichem]);	//gross chemical flux transported out of a cell by porewater (g/s)
		free(pwrchemovoutmass[ichem]);	//cumulative gross chemical mass transported out of a cell by porewater (kg)
		free(burchemovinmass[ichem]);	//cumulative gross chemical mass transported into a cell by burial (kg)
		free(burchemovoutmass[ichem]);	//cumulative gross chemical mass transported out of a cell by burial (kg)
		free(scrchemovinmass[ichem]);	//cumulative gross chemical mass transported into a cell by scour (kg)
		free(scrchemovoutmass[ichem]);	//cumulative gross chemical mass transported out of a cell by scour (kg)

		free(biochemovinflux[ichem]);		//biodegredation influx of chemical in overland plane [ichem][row][col][layer]
		free(biochemovinmass[ichem]);		//biodegredation inflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(biochemovoutflux[ichem]);	//biodegredation outflux of chemical in overland plane [ichem][row][col][layer]
		free(biochemovoutmass[ichem]);	//biodegredation outflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(hydchemovinflux[ichem]);		//hydrolysis influx of chemical in overland plane [ichem][row][col][layer]
		free(hydchemovinmass[ichem]);		//hydrolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(hydchemovoutflux[ichem]);	//hydrolysis outflux of chemical in overland plane [ichem][row][col][layer]
		free(hydchemovoutmass[ichem]);	//hydrolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(oxichemovinflux[ichem]);		//oxidation influx of chemical in overland plane [ichem][row][col][layer]
		free(oxichemovinmass[ichem]);		//oxidation inflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(oxichemovoutflux[ichem]);	//oxidation outflux of chemical in overland plane [ichem][row][col][layer]
		free(oxichemovoutmass[ichem]);	//oxidation outflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(phtchemovinflux[ichem]);		//photolysis influx of chemical in overland plane [ichem][row][col][layer]
		free(phtchemovinmass[ichem]);		//photolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(phtchemovoutflux[ichem]);	//photolysis outflux of chemical in overland plane [ichem][row][col][layer]
		free(phtchemovoutmass[ichem]);	//photolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(radchemovinflux[ichem]);		//radioactive decay influx of chemical in overland plane [ichem][row][col][layer]
		free(radchemovinmass[ichem]);		//radioactive decay inflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(radchemovoutflux[ichem]);	//radioactive decay outflux of chemical in overland plane [ichem][row][col][layer]
		free(radchemovoutmass[ichem]);	//radioactive decay outflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(vltchemovinflux[ichem]);		//volatilization influx of chemical in overland plane [ichem][row][col][layer]
		free(vltchemovinmass[ichem]);		//volatilization inflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(vltchemovoutflux[ichem]);	//volatilization outflux of chemical in overland plane [ichem][row][col][layer]
		free(vltchemovoutmass[ichem]);	//volatilization outflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(udrchemovinflux[ichem]);		//user-defined reaction influx of chemical in overland plane [ichem][row][col][layer]
		free(udrchemovinmass[ichem]);		//user-defined reaction inflowing mass of chemical in overland plane [ichem][row][col][layer]
		free(udrchemovoutflux[ichem]);	//user-defined reaction outflux of chemical in overland plane [ichem][row][col][layer]
		free(udrchemovoutmass[ichem]);	//user-defined reaction outflowing mass of chemical in overland plane [ichem][row][col][layer]

	}	//end loop over chemicals

	//Deallocate memory for overland mass tracking parameters
	free(totalerscheminov);		//cumulative gross chemical mass transported into the overland water column by erosion (kg)
	free(totalpwrcheminov);		//cumulative gross chemical mass transported into the overland water column by porewater release (kg)
	free(totaldepchemoutov);	//cumulative gross chemical mass transported out of the overland water column by deposition (kg)
	free(totalcwov);			//cumulative net chemical mass entering the channel water column from an external source (kg)

	//Deallocate memory for chemical transport parameters indexed by chemical type, row, column, and source/layer
	free(cchemovnew);		//new overland chemical concentration (for next time t+dt) (g/m3)
	free(advchemovinflux);	//gross chemical flux transported into a cell by advection (g/s)
	free(advchemovinmass);	//cumulative gross chemical mass transported into a cell by advection (kg)
	free(advchemovoutflux);	//gross chemical flux transported out of a cell by advection (g/s)
	free(advchemovoutmass);	//cumulative gross chemical mass transported out of a cell by advection (kg)
	free(dspchemovinflux);	//gross chemical flux transported into a cell by dispersion (g/s)
	free(dspchemovinmass);	//cumulative gross chemical mass transported into a cell by dispersion (kg)
	free(dspchemovoutflux);	//gross chemical flux transported out of a cell by dispersion (g/s)
	free(dspchemovoutmass);	//cumulative gross chemical mass transported out of a cell by dispersion (kg)
	free(depchemovinflux);	//gross chemical flux transported into a cell by deposition (g/s)
	free(depchemovinmass);	//cumulative gross chemical mass transported into a cell by deposition (kg)
	free(depchemovoutflux);	//gross chemical flux transported out of a cell by deposition (g/s)
	free(depchemovoutmass);	//cumulative gross chemical mass transported out of a cell by deposition (kg)
	free(erschemovinflux);	//gross chemical flux transported into a cell by erosion (g/s)
	free(erschemovinmass);	//cumulative gross chemical mass transported into a cell by erosion (kg)
	free(erschemovoutflux);	//gross chemical flux transported out of a cell by erosion (g/s)
	free(erschemovoutmass);	//cumulative gross chemical mass transported out of a cell by erosion (kg)
	free(pwrchemovinflux);	//gross chemical flux transported into a cell by porewater (g/s)
	free(pwrchemovinmass);	//cumulative gross chemical mass transported into a cell by porewater (kg)
	free(pwrchemovoutflux);	//gross chemical flux transported out of a cell by porewater (g/s)
	free(pwrchemovoutmass);	//cumulative gross chemical mass transported out of a cell by porewater (kg)
	free(burchemovinmass);	//cumulative gross chemical mass transported into a cell by burial (kg)
	free(burchemovoutmass);	//cumulative gross chemical mass transported out of a cell by burial (kg)
	free(scrchemovinmass);	//cumulative gross chemical mass transported into a cell by scour (kg)
	free(scrchemovoutmass);	//cumulative gross chemical mass transported out of a cell by scour (kg)

	free(biochemovinflux);		//biodegredation influx of chemical in overland plane [ichem][row][col][layer]
	free(biochemovinmass);		//biodegredation inflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(biochemovoutflux);	//biodegredation outflux of chemical in overland plane [ichem][row][col][layer]
	free(biochemovoutmass);	//biodegredation outflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(hydchemovinflux);		//hydrolysis influx of chemical in overland plane [ichem][row][col][layer]
	free(hydchemovinmass);		//hydrolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(hydchemovoutflux);	//hydrolysis outflux of chemical in overland plane [ichem][row][col][layer]
	free(hydchemovoutmass);	//hydrolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(oxichemovinflux);		//oxidation influx of chemical in overland plane [ichem][row][col][layer]
	free(oxichemovinmass);		//oxidation inflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(oxichemovoutflux);	//oxidation outflux of chemical in overland plane [ichem][row][col][layer]
	free(oxichemovoutmass);	//oxidation outflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(phtchemovinflux);		//photolysis influx of chemical in overland plane [ichem][row][col][layer]
	free(phtchemovinmass);		//photolysis inflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(phtchemovoutflux);	//photolysis outflux of chemical in overland plane [ichem][row][col][layer]
	free(phtchemovoutmass);	//photolysis outflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(radchemovinflux);		//radioactive decay influx of chemical in overland plane [ichem][row][col][layer]
	free(radchemovinmass);		//radioactive decay inflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(radchemovoutflux);	//radioactive decay outflux of chemical in overland plane [ichem][row][col][layer]
	free(radchemovoutmass);	//radioactive decay outflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(vltchemovinflux);		//volatilization influx of chemical in overland plane [ichem][row][col][layer]
	free(vltchemovinmass);		//volatilization inflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(vltchemovoutflux);	//volatilization outflux of chemical in overland plane [ichem][row][col][layer]
	free(vltchemovoutmass);	//volatilization outflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(udrchemovinflux);		//user-defined reaction influx of chemical in overland plane [ichem][row][col][layer]
	free(udrchemovinmass);		//user-defined reaction inflowing mass of chemical in overland plane [ichem][row][col][layer]
	free(udrchemovoutflux);	//user-defined reaction outflux of chemical in overland plane [ichem][row][col][layer]
	free(udrchemovoutmass);	//user-defined reaction outflowing mass of chemical in overland plane [ichem][row][col][layer]

	/*******************************************************************/
	/*        Free Global Channel Chemical Transport Variables         */
	/*******************************************************************/

	//if channels are simulated
	if(chnopt > 0)
	{
		//loop over number of chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//Loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Deallocate memory for channel chemical transport parameters
					free(cchemchnew[ichem][i][j]);		//new channel chemical concentration (for next time t+dt) (g/m3)
					free(advchemchinflux[ichem][i][j]);	//gross chemical flux transported into a node by advection (g/s)
					free(advchemchinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a node by advection (kg)
					free(advchemchoutflux[ichem][i][j]);	//gross chemical flux transported out of a node by advection (g/s)
					free(advchemchoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a node by advection (kg)
					free(dspchemchinflux[ichem][i][j]);	//gross chemical flux transported into a node by dispersion (g/s)
					free(dspchemchinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a node by dispersion (kg)
					free(dspchemchoutflux[ichem][i][j]);	//gross chemical flux transported out of a node by dispersion (g/s)
					free(dspchemchoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a node by dispersion (kg)
					free(depchemchinflux[ichem][i][j]);	//gross chemical flux transported into a node by deposition (g/s)
					free(depchemchinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a node by deposition (kg)
					free(depchemchoutflux[ichem][i][j]);	//gross chemical flux transported out of a node by deposition (g/s)
					free(depchemchoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a node by deposition (kg)
					free(erschemchinflux[ichem][i][j]);	//gross chemical flux transported into a node by erosion (g/s)
					free(erschemchinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a node by erosion (kg)
					free(erschemchoutflux[ichem][i][j]);	//gross chemical flux transported out of a node by erosion (g/s)
					free(erschemchoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a node by erosion (kg)
					free(pwrchemchinflux[ichem][i][j]);	//gross chemical flux transported into a node by porewater (g/s)
					free(pwrchemchinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a node by porewater (kg)
					free(pwrchemchoutflux[ichem][i][j]);	//gross chemical flux transported out of a node by porewater (g/s)
					free(pwrchemchoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a node by porewater (kg)
					free(burchemchinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a node by burial (kg)
					free(burchemchoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a node by burial (kg)
					free(scrchemchinmass[ichem][i][j]);	//cumulative gross chemical mass transported into a node by scour (kg)
					free(scrchemchoutmass[ichem][i][j]);	//cumulative gross chemical mass transported out of a node by scour (kg)

					free(biochemchinflux[ichem][i][j]);		//biodegredation influx of chemical in the channel network [ichem][link][node][layer]
					free(biochemchinmass[ichem][i][j]);		//biodegredation inflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(biochemchoutflux[ichem][i][j]);	//biodegredation outflux of chemical in the channel network [ichem][link][node][layer]
					free(biochemchoutmass[ichem][i][j]);	//biodegredation outflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(hydchemchinflux[ichem][i][j]);		//hydrolysis influx of chemical in the channel network [ichem][link][node][layer]
					free(hydchemchinmass[ichem][i][j]);		//hydrolysis inflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(hydchemchoutflux[ichem][i][j]);	//hydrolysis outflux of chemical in the channel network [ichem][link][node][layer]
					free(hydchemchoutmass[ichem][i][j]);	//hydrolysis outflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(oxichemchinflux[ichem][i][j]);		//oxidation influx of chemical in the channel network [ichem][link][node][layer]
					free(oxichemchinmass[ichem][i][j]);		//oxidation inflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(oxichemchoutflux[ichem][i][j]);	//oxidation outflux of chemical in the channel network [ichem][link][node][layer]
					free(oxichemchoutmass[ichem][i][j]);	//oxidation outflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(phtchemchinflux[ichem][i][j]);		//photolysis influx of chemical in the channel network [ichem][link][node][layer]
					free(phtchemchinmass[ichem][i][j]);		//photolysis inflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(phtchemchoutflux[ichem][i][j]);	//photolysis outflux of chemical in the channel network [ichem][link][node][layer]
					free(phtchemchoutmass[ichem][i][j]);	//photolysis outflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(radchemchinflux[ichem][i][j]);		//radioactive decay influx of chemical in the channel network [ichem][link][node][layer]
					free(radchemchinmass[ichem][i][j]);		//radioactive decay inflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(radchemchoutflux[ichem][i][j]);	//radioactive decay outflux of chemical in the channel network [ichem][link][node][layer]
					free(radchemchoutmass[ichem][i][j]);	//radioactive decay outflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(vltchemchinflux[ichem][i][j]);		//volatilization influx of chemical in the channel network [ichem][link][node][layer]
					free(vltchemchinmass[ichem][i][j]);		//volatilization inflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(vltchemchoutflux[ichem][i][j]);	//volatilization outflux of chemical in the channel network [ichem][link][node][layer]
					free(vltchemchoutmass[ichem][i][j]);	//volatilization outflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(udrchemchinflux[ichem][i][j]);		//user-defined reaction influx of chemical in the channel network [ichem][link][node][layer]
					free(udrchemchinmass[ichem][i][j]);		//user-defined reaction inflowing mass of chemical in the channel network [ichem][link][node][layer]
					free(udrchemchoutflux[ichem][i][j]);	//user-defined reaction outflux of chemical in the channel network [ichem][link][node][layer]
					free(udrchemchoutmass[ichem][i][j]);	//user-defined reaction outflowing mass of chemical in the channel network [ichem][link][node][layer]

				}	//end loop over nodes

				//Deallocate memory for channel chemical transport parameters
				free(cchemchnew[ichem][i]);		//new channel chemical concentration (for next time t+dt) (g/m3)
				free(advchemchinflux[ichem][i]);	//gross chemical flux transported into a node by advection (g/s)
				free(advchemchinmass[ichem][i]);	//cumulative gross chemical mass transported into a node by advection (kg)
				free(advchemchoutflux[ichem][i]);	//gross chemical flux transported out of a node by advection (g/s)
				free(advchemchoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a node by advection (kg)
				free(dspchemchinflux[ichem][i]);	//gross chemical flux transported into a node by dispersion (g/s)
				free(dspchemchinmass[ichem][i]);	//cumulative gross chemical mass transported into a node by dispersion (kg)
				free(dspchemchoutflux[ichem][i]);	//gross chemical flux transported out of a node by dispersion (g/s)
				free(dspchemchoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a node by dispersion (kg)
				free(depchemchinflux[ichem][i]);	//gross chemical flux transported into a node by deposition (g/s)
				free(depchemchinmass[ichem][i]);	//cumulative gross chemical mass transported into a node by deposition (kg)
				free(depchemchoutflux[ichem][i]);	//gross chemical flux transported out of a node by deposition (g/s)
				free(depchemchoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a node by deposition (kg)
				free(erschemchinflux[ichem][i]);	//gross chemical flux transported into a node by erosion (g/s)
				free(erschemchinmass[ichem][i]);	//cumulative gross chemical mass transported into a node by erosion (kg)
				free(erschemchoutflux[ichem][i]);	//gross chemical flux transported out of a node by erosion (g/s)
				free(erschemchoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a node by erosion (kg)
				free(pwrchemchinflux[ichem][i]);	//gross chemical flux transported into a node by porewater (g/s)
				free(pwrchemchinmass[ichem][i]);	//cumulative gross chemical mass transported into a node by porewater (kg)
				free(pwrchemchoutflux[ichem][i]);	//gross chemical flux transported out of a node by porewater (g/s)
				free(pwrchemchoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a node by porewater (kg)
				free(burchemchinmass[ichem][i]);	//cumulative gross chemical mass transported into a node by burial (kg)
				free(burchemchoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a node by burial (kg)
				free(scrchemchinmass[ichem][i]);	//cumulative gross chemical mass transported into a node by scour (kg)
				free(scrchemchoutmass[ichem][i]);	//cumulative gross chemical mass transported out of a node by scour (kg)

				free(biochemchinflux[ichem][i]);		//biodegredation influx of chemical in the channel network [ichem][link][node][layer]
				free(biochemchinmass[ichem][i]);		//biodegredation inflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(biochemchoutflux[ichem][i]);	//biodegredation outflux of chemical in the channel network [ichem][link][node][layer]
				free(biochemchoutmass[ichem][i]);	//biodegredation outflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(hydchemchinflux[ichem][i]);		//hydrolysis influx of chemical in the channel network [ichem][link][node][layer]
				free(hydchemchinmass[ichem][i]);		//hydrolysis inflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(hydchemchoutflux[ichem][i]);	//hydrolysis outflux of chemical in the channel network [ichem][link][node][layer]
				free(hydchemchoutmass[ichem][i]);	//hydrolysis outflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(oxichemchinflux[ichem][i]);		//oxidation influx of chemical in the channel network [ichem][link][node][layer]
				free(oxichemchinmass[ichem][i]);		//oxidation inflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(oxichemchoutflux[ichem][i]);	//oxidation outflux of chemical in the channel network [ichem][link][node][layer]
				free(oxichemchoutmass[ichem][i]);	//oxidation outflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(phtchemchinflux[ichem][i]);		//photolysis influx of chemical in the channel network [ichem][link][node][layer]
				free(phtchemchinmass[ichem][i]);		//photolysis inflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(phtchemchoutflux[ichem][i]);	//photolysis outflux of chemical in the channel network [ichem][link][node][layer]
				free(phtchemchoutmass[ichem][i]);	//photolysis outflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(radchemchinflux[ichem][i]);		//radioactive decay influx of chemical in the channel network [ichem][link][node][layer]
				free(radchemchinmass[ichem][i]);		//radioactive decay inflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(radchemchoutflux[ichem][i]);	//radioactive decay outflux of chemical in the channel network [ichem][link][node][layer]
				free(radchemchoutmass[ichem][i]);	//radioactive decay outflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(vltchemchinflux[ichem][i]);		//volatilization influx of chemical in the channel network [ichem][link][node][layer]
				free(vltchemchinmass[ichem][i]);		//volatilization inflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(vltchemchoutflux[ichem][i]);	//volatilization outflux of chemical in the channel network [ichem][link][node][layer]
				free(vltchemchoutmass[ichem][i]);	//volatilization outflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(udrchemchinflux[ichem][i]);		//user-defined reaction influx of chemical in the channel network [ichem][link][node][layer]
				free(udrchemchinmass[ichem][i]);		//user-defined reaction inflowing mass of chemical in the channel network [ichem][link][node][layer]
				free(udrchemchoutflux[ichem][i]);	//user-defined reaction outflux of chemical in the channel network [ichem][link][node][layer]
				free(udrchemchoutmass[ichem][i]);	//user-defined reaction outflowing mass of chemical in the channel network [ichem][link][node][layer]

			}	//end loop over links

			//Deallocate memory for channel chemical transport parameters indexed by link and node (and chemical type and source/layer)
			free(cchemchnew[ichem]);		//new channel chemical concentration (for next time t+dt) (g/m3)
			free(advchemchinflux[ichem]);	//gross chemical flux transported into a node by advection (g/s)
			free(advchemchinmass[ichem]);	//cumulative gross chemical mass transported into a node by advection (kg)
			free(advchemchoutflux[ichem]);	//gross chemical flux transported out of a node by advection (g/s)
			free(advchemchoutmass[ichem]);	//cumulative gross chemical mass transported out of a node by advection (kg)
			free(dspchemchinflux[ichem]);	//gross chemical flux transported into a node by dispersion (g/s)
			free(dspchemchinmass[ichem]);	//cumulative gross chemical mass transported into a node by dispersion (kg)
			free(dspchemchoutflux[ichem]);	//gross chemical flux transported out of a node by dispersion (g/s)
			free(dspchemchoutmass[ichem]);	//cumulative gross chemical mass transported out of a node by dispersion (kg)
			free(depchemchinflux[ichem]);	//gross chemical flux transported into a node by deposition (g/s)
			free(depchemchinmass[ichem]);	//cumulative gross chemical mass transported into a node by deposition (kg)
			free(depchemchoutflux[ichem]);	//gross chemical flux transported out of a node by deposition (g/s)
			free(depchemchoutmass[ichem]);	//cumulative gross chemical mass transported out of a node by deposition (kg)
			free(erschemchinflux[ichem]);	//gross chemical flux transported into a node by erosion (g/s)
			free(erschemchinmass[ichem]);	//cumulative gross chemical mass transported into a node by erosion (kg)
			free(erschemchoutflux[ichem]);	//gross chemical flux transported out of a node by erosion (g/s)
			free(erschemchoutmass[ichem]);	//cumulative gross chemical mass transported out of a node by erosion (kg)
			free(pwrchemchinflux[ichem]);	//gross chemical flux transported into a node by porewater (g/s)
			free(pwrchemchinmass[ichem]);	//cumulative gross chemical mass transported into a node by porewater (kg)
			free(pwrchemchoutflux[ichem]);	//gross chemical flux transported out of a node by porewater (g/s)
			free(pwrchemchoutmass[ichem]);	//cumulative gross chemical mass transported out of a node by porewater (kg)
			free(burchemchinmass[ichem]);	//cumulative gross chemical mass transported into a node by burial (kg)
			free(burchemchoutmass[ichem]);	//cumulative gross chemical mass transported out of a node by burial (kg)
			free(scrchemchinmass[ichem]);	//cumulative gross chemical mass transported into a node by scour (kg)
			free(scrchemchoutmass[ichem]);	//cumulative gross chemical mass transported out of a node by scour (kg)

			free(biochemchinflux[ichem]);		//biodegredation influx of chemical in the channel network [ichem][link][node][layer]
			free(biochemchinmass[ichem]);		//biodegredation inflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(biochemchoutflux[ichem]);	//biodegredation outflux of chemical in the channel network [ichem][link][node][layer]
			free(biochemchoutmass[ichem]);	//biodegredation outflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(hydchemchinflux[ichem]);		//hydrolysis influx of chemical in the channel network [ichem][link][node][layer]
			free(hydchemchinmass[ichem]);		//hydrolysis inflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(hydchemchoutflux[ichem]);	//hydrolysis outflux of chemical in the channel network [ichem][link][node][layer]
			free(hydchemchoutmass[ichem]);	//hydrolysis outflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(oxichemchinflux[ichem]);		//oxidation influx of chemical in the channel network [ichem][link][node][layer]
			free(oxichemchinmass[ichem]);		//oxidation inflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(oxichemchoutflux[ichem]);	//oxidation outflux of chemical in the channel network [ichem][link][node][layer]
			free(oxichemchoutmass[ichem]);	//oxidation outflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(phtchemchinflux[ichem]);		//photolysis influx of chemical in the channel network [ichem][link][node][layer]
			free(phtchemchinmass[ichem]);		//photolysis inflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(phtchemchoutflux[ichem]);	//photolysis outflux of chemical in the channel network [ichem][link][node][layer]
			free(phtchemchoutmass[ichem]);	//photolysis outflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(radchemchinflux[ichem]);		//radioactive decay influx of chemical in the channel network [ichem][link][node][layer]
			free(radchemchinmass[ichem]);		//radioactive decay inflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(radchemchoutflux[ichem]);	//radioactive decay outflux of chemical in the channel network [ichem][link][node][layer]
			free(radchemchoutmass[ichem]);	//radioactive decay outflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(vltchemchinflux[ichem]);		//volatilization influx of chemical in the channel network [ichem][link][node][layer]
			free(vltchemchinmass[ichem]);		//volatilization inflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(vltchemchoutflux[ichem]);	//volatilization outflux of chemical in the channel network [ichem][link][node][layer]
			free(vltchemchoutmass[ichem]);	//volatilization outflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(udrchemchinflux[ichem]);		//user-defined reaction influx of chemical in the channel network [ichem][link][node][layer]
			free(udrchemchinmass[ichem]);		//user-defined reaction inflowing mass of chemical in the channel network [ichem][link][node][layer]
			free(udrchemchoutflux[ichem]);	//user-defined reaction outflux of chemical in the channel network [ichem][link][node][layer]
			free(udrchemchoutmass[ichem]);	//user-defined reaction outflowing mass of chemical in the channel network [ichem][link][node][layer]

		}	//end loop over chemical

		//Deallocate memory for channel mass tracking parameters
		free(totalerscheminch);		//cumulative gross chemical mass transported into the channel water column by erosion (kg)
		free(totalpwrcheminch);		//cumulative gross chemical mass transported into the channel water column by porewater release (kg)
		free(totaldepchemoutch);	//cumulative gross chemical mass transported out of the channel water column by deposition (kg)
		free(totalcwch);			//cumulative net chemical mass entering the channel water column from an external source (kg)

		//Deallocate memory for channel chemical transport parameters indexed by link and node (and chemical type and source/layer)
		free(cchemchnew);		//new channel chemical concentration (for next time t+dt) (g/m3)
		free(advchemchinflux);	//gross chemical flux transported into a node by advection (g/s)
		free(advchemchinmass);	//cumulative gross chemical mass transported into a node by advection (kg)
		free(advchemchoutflux);	//gross chemical flux transported out of a node by advection (g/s)
		free(advchemchoutmass);	//cumulative gross chemical mass transported out of a node by advection (kg)
		free(dspchemchinflux);	//gross chemical flux transported into a node by dispersion (g/s)
		free(dspchemchinmass);	//cumulative gross chemical mass transported into a node by dispersion (kg)
		free(dspchemchoutflux);	//gross chemical flux transported out of a node by dispersion (g/s)
		free(dspchemchoutmass);	//cumulative gross chemical mass transported out of a node by dispersion (kg)
		free(depchemchinflux);	//gross chemical flux transported into a node by deposition (g/s)
		free(depchemchinmass);	//cumulative gross chemical mass transported into a node by deposition (kg)
		free(depchemchoutflux);	//gross chemical flux transported out of a node by deposition (g/s)
		free(depchemchoutmass);	//cumulative gross chemical mass transported out of a node by deposition (kg)
		free(erschemchinflux);	//gross chemical flux transported into a node by erosion (g/s)
		free(erschemchinmass);	//cumulative gross chemical mass transported into a node by erosion (kg)
		free(erschemchoutflux);	//gross chemical flux transported out of a node by erosion (g/s)
		free(erschemchoutmass);	//cumulative gross chemical mass transported out of a node by erosion (kg)
		free(pwrchemchinflux);	//gross chemical flux transported into a node by porewater (g/s)
		free(pwrchemchinmass);	//cumulative gross chemical mass transported into a node by porewater (kg)
		free(pwrchemchoutflux);	//gross chemical flux transported out of a node by porewater (g/s)
		free(pwrchemchoutmass);	//cumulative gross chemical mass transported out of a node by porewater (kg)
		free(burchemchinmass);	//cumulative gross chemical mass transported into a node by burial (kg)
		free(burchemchoutmass);	//cumulative gross chemical mass transported out of a node by burial (kg)
		free(scrchemchinmass);	//cumulative gross chemical mass transported into a node by scour (kg)
		free(scrchemchoutmass);	//cumulative gross chemical mass transported out of a node by scour (kg)

		free(biochemchinflux);		//biodegredation influx of chemical in the channel network [ichem][link][node][layer]
		free(biochemchinmass);		//biodegredation inflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(biochemchoutflux);	//biodegredation outflux of chemical in the channel network [ichem][link][node][layer]
		free(biochemchoutmass);	//biodegredation outflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(hydchemchinflux);		//hydrolysis influx of chemical in the channel network [ichem][link][node][layer]
		free(hydchemchinmass);		//hydrolysis inflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(hydchemchoutflux);	//hydrolysis outflux of chemical in the channel network [ichem][link][node][layer]
		free(hydchemchoutmass);	//hydrolysis outflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(oxichemchinflux);		//oxidation influx of chemical in the channel network [ichem][link][node][layer]
		free(oxichemchinmass);		//oxidation inflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(oxichemchoutflux);	//oxidation outflux of chemical in the channel network [ichem][link][node][layer]
		free(oxichemchoutmass);	//oxidation outflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(phtchemchinflux);		//photolysis influx of chemical in the channel network [ichem][link][node][layer]
		free(phtchemchinmass);		//photolysis inflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(phtchemchoutflux);	//photolysis outflux of chemical in the channel network [ichem][link][node][layer]
		free(phtchemchoutmass);	//photolysis outflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(radchemchinflux);		//radioactive decay influx of chemical in the channel network [ichem][link][node][layer]
		free(radchemchinmass);		//radioactive decay inflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(radchemchoutflux);	//radioactive decay outflux of chemical in the channel network [ichem][link][node][layer]
		free(radchemchoutmass);	//radioactive decay outflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(vltchemchinflux);		//volatilization influx of chemical in the channel network [ichem][link][node][layer]
		free(vltchemchinmass);		//volatilization inflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(vltchemchoutflux);	//volatilization outflux of chemical in the channel network [ichem][link][node][layer]
		free(vltchemchoutmass);	//volatilization outflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(udrchemchinflux);		//user-defined reaction influx of chemical in the channel network [ichem][link][node][layer]
		free(udrchemchinmass);		//user-defined reaction inflowing mass of chemical in the channel network [ichem][link][node][layer]
		free(udrchemchoutflux);	//user-defined reaction outflux of chemical in the channel network [ichem][link][node][layer]
		free(udrchemchoutmass);	//user-defined reaction outflowing mass of chemical in the channel network [ichem][link][node][layer]

	}	//end if chnopt > 0

	/****************************************************************/
	/*        Free Global Chemical Reporting Station Arrays         */
	/****************************************************************/

	//Loop over number of chemical reporting stations
	for(i=1; i<=nchems; i++)
	{
		//Deallocate memory for reported chemical discharges (export)
		free(totchemreportov[i]);	//reported overland total chemical discharge (units vary)
		free(dischemreportov[i]);	//reported overland dissolved chemical discharge (units vary)
		free(bndchemreportov[i]);	//reported overland bound chemical discharge (units vary)
		free(prtchemreportov[i]);	//reported overland particulate chemical discharge (units vary)
		free(totchemreportch[i]);	//reported channel total chemical discharge (units vary)
		free(dischemreportch[i]);	//reported channel dissolved chemical discharge (units vary)
		free(bndchemreportch[i]);	//reported channel bound chemical discharge (units vary)
		free(prtchemreportch[i]);	//reported channel particulate chemical discharge (units vary)

	}	//end loop over chemical reporting stations

	//Deallocate memory for reported chemical discharges (export)
	free(totchemreportov);	//reported overland total chemical discharge (units vary)
	free(dischemreportov);	//reported overland dissolved chemical discharge (units vary)
	free(bndchemreportov);	//reported overland bound chemical discharge (units vary)
	free(prtchemreportov);	//reported overland particulate chemical discharge (units vary)
	free(totchemreportch);	//reported channel total chemical discharge (units vary)
	free(dischemreportch);	//reported channel dissolved chemical discharge (units vary)
	free(bndchemreportch);	//reported channel bound chemical discharge (units vary)
	free(prtchemreportch);	//reported channel particulate chemical discharge (units vary)
	free(chemconvert);		//chemical export conversion factor

	/******************************************************/
	/*        Free Outlet Cell Chemical Variables         */
	/******************************************************/

	//loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Deallocate memory for overland outlet mass tracking variables
		free(totaladvcheminov[ichem]);	//cumulative chemical mass entering domain by overland advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		free(totaladvchemoutov[ichem]);	//cumulative chemical mass exiting domain by overland advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		free(totaldspcheminov[ichem]);	//cumulative chemical mass entering domain by overland dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		free(totaldspchemoutov[ichem]);	//cumulative chemical mass exiting domain by overland dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		free(chemflowpeakov[ichem]);		//peak chemical discharge leaving an outlet (kg/s) [ichem][oulet]
		free(chemtimepeakov[ichem]);		//time of peak chemical discharge leaving an outlet (hrs) [ichem][outlet]

	}	//end loop over chemicals

	//Deallocate memory for overland outlet mass tracking variables
	free(totaladvcheminov);		//cumulative chemical mass entering domain by overland advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
	free(totaladvchemoutov);		//cumulative chemical mass exiting domain by overland advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
	free(totaldspcheminov);		//cumulative chemical mass entering domain by overland dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
	free(totaldspchemoutov);		//cumulative chemical mass exiting domain by overland dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
	free(chemflowpeakov);		//peak chemical discharge leaving an outlet (kg/s) [ichem][outlet]
	free(chemtimepeakov);		//time of peak chemical discharge leaving an outlet (hrs) [ichem][outlet]

	//if channels are simulated
	if(chnopt > 0)
	{
		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Deallocate memory for channel outlet mass tracking variables
			free(totaladvcheminch[ichem]);	//cumulative chemical mass entering domain by channel advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
			free(totaladvchemoutch[ichem]);	//cumulative chemical mass exiting domain by channel advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
			free(totaldspcheminch[ichem]);	//cumulative chemical mass entering domain by channel dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
			free(totaldspchemoutch[ichem]);	//cumulative chemical mass exiting domain by channel dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
			free(chemflowpeakch[ichem]);		//peak channel chemical discharge leaving an outlet (kg/s) [ichem][outlet]
			free(chemtimepeakch[ichem]);		//time of peak channel chemical discharge leaving an outlet (hrs) [ichem][outlet]

		}	//end loop over chemicals

		//Deallocate memory for channel outlet mass tracking variables
		free(totaladvcheminch);		//cumulative chemical mass entering domain by channel advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		free(totaladvchemoutch);		//cumulative chemical mass exiting domain by channel advection at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		free(totaldspcheminch);		//cumulative chemical mass entering domain by channel dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		free(totaldspchemoutch);		//cumulative chemical mass exiting domain by channel dispersion at an outlet (kg) [ichem][outlet] (zeroeth element used to sum all outlets)
		free(chemflowpeakch);		//peak channel chemical discharge leaving an outlet (kg/s) [ichem][outlet]
		free(chemtimepeakch);		//time of peak channel chemical discharge leaving an outlet (hrs) [ichem][outlet]

	}	//end if chnopt > 0

	/******************************************************************/
	/*        Free Chemical Forcing Function and BC Variables         */
	/******************************************************************/

	//Include forcing functions (loads) and boundary conditions (outlets)
	//
	//Free arrays related to chemical loads and boundary conditions
	//
	//Loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//Deallocate memory for point source chemical loads
		free(ncwpovt[i]);	//next (upper) interval time for point source chemical load time series
		free(pcwpovt[i]);	//prior (lower) interval time for point source chemical load time series
		free(bcwpov[i]);	//intercept for point source chemical load interpolation
		free(mcwpov[i]);	//slope for point source chemical load interpolation
		free(cwpovpoint[i]);	//pointer (to position in time series) for point source chemical loads
		free(cwpovinterp[i]);	//external chemical point source load interpolated in time for source[i]

		//Deallocate memory for distributed chemical loads
		free(ncwdovt[i]);	//next (upper) interval time for distributed chemical load time series
		free(pcwdovt[i]);	//prior (lower) interval time for distributed chemical load time series
		free(bcwdov[i]);	//intercept for distributed chemical load interpolation
		free(mcwdov[i]);	//slope for distributed chemical load interpolation
		free(cwdovpoint[i]);	//pointer (to position in time series) for distributed chemical loads
		free(cwdovinterp[i]);	//external distributed chemical load interpolated in time for source[i]

		//Deallocate memory for chemical boundary
		free(ncbct[i]);	//next (upper) interval time for chemical bc time series
		free(pcbct[i]);	//prior (lower) interval time for chemical bc time series
		free(bcbc[i]);	//intercept for chemical bc interpolation
		free(mcbc[i]);	//slope for chemical bc interpolation
		free(cbcpoint[i]);	//pointer (to position in time series) for chemical BCs
		free(cbcinterp[i]);	//external chemical BC interpolated in time for outlet[i]

	}	//end loop over number of chemicals

	//Deallocate memory for point source chemical loads
	free(ncwpovt);		//next (upper) interval time for point source chemical load time series
	free(pcwpovt);		//prior (lower) interval time for point source chemical load time series
	free(bcwpov);			//intercept for point source chemical load interpolation
	free(mcwpov);			//slope for point source chemical load interpolation
	free(cwpovpoint);			//pointer (to position in time series) for point source chemical loads
	free(cwpovinterp);	//external point source chemical load interpolated in time for source[i]
	free(ntcwpov);			//next time to update any point source load function for chemical type [i]

	//Deallocate memory for distributed chemical loads
	free(ncwdovt);		//next (upper) interval time for distributed chemical load time series
	free(pcwdovt);		//prior (lower) interval time for distributed chemical load time series
	free(bcwdov);			//intercept for distributed chemical load interpolation
	free(mcwdov);			//slope for distributed chemical load interpolation
	free(cwdovpoint);			//pointer (to position in time series) for distributed chemical loads
	free(cwdovinterp);	//external distributed chemical load interpolated in time for source[i]
	free(ntcwdov);			//next time to update any distributed load function for chemical type [i]

	//Deallocate memory for chemical boundary conditions
	free(ncbct);		//next (upper) interval time for chemical bc time series
	free(pcbct);		//prior (lower) interval time for chemical bc time series
	free(bcbc);		//intercept for chemical bc interpolation
	free(mcbc);		//slope for chemical bc interpolation
	free(cbcpoint);		//pointer (to position in time series) for chemical BCs
	free(cbcinterp);	//external chemical BC interpolated in time for outlet[i]
	free(ntcbc);		//next time to update any bc function for chemical type [i]

	//if channels are simulated
	if(chnopt > 0)
	{
		//Free arrays related to chemical loads and boundary conditions
		//
		//Loop over number of chemicals
		for(i=1; i<=nchems; i++)
		{
			//Deallocate memory for chemical loads
			free(ncwcht[i]);	//next (upper) interval time for chemical load time series
			free(pcwcht[i]);	//prior (lower) interval time for chemical load time series
			free(bcwch[i]);	//intercept for chemical load interpolation
			free(mcwch[i]);	//slope for chemical load interpolation
			free(cwchpoint[i]);	//pointer (to position in time series) for chemical loads
			free(cwchinterp[i]);	//external chemical load interpolated in time for source[i]

		}	//end loop over number of chemicals

		//Deallocate memory for chemical loads
		free(ncwcht);		//next (upper) interval time for chemical load time series
		free(pcwcht);		//prior (lower) interval time for chemical load time series
		free(bcwch);			//intercept for chemical load interpolation
		free(mcwch);			//slope for chemical load interpolation
		free(cwchpoint);			//pointer (to position in time series) for chemical loads
		free(cwchinterp);	//external chemical load interpolated in time for source[i]
		free(ntcwch);			//next time to update any load function for chemical type [i]

	}	//end if chhnopt = 1

	/******************************************************/
	/*        Free Chemical Export And Grid Files         */
	/******************************************************/

	//Free export files...
	//
	//Note:  Memory for chemical export files is allocated for each
	//       reporting group.  Sums of chemical types for each group
	//       are computed in ComputeChemicalGroupConcSums.
	//
	//loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//Deallocate memory for chemical export file
		free(chemexpfile[i]);

	}	//end loop over number of chemicals

	//Deallocate memory for chemical export file names
	free(chemexpfile);

	//Free grid files...
	//
	//Note:  Memory for chemical grid files is allocated for each
	//       reporting group.  Sums of particle types for each group
	//       are computed in ComputeChemicalGroupConcSums.
	//
	//loop over number of chemical reports
	for(i=1; i<=ncgroups; i++)
	{
		//Deallocate memory for chemical grid files
		free(totchemconcwatergrid[i]);
		free(dischemconcwatergrid[i]);
		free(bndchemconcwatergrid[i]);
		free(prtchemconcwatergrid[i]);
		free(srbchemconcwatergrid[i]);
		free(totchemconcsurfgrid[i]);
		free(dischemconcsurfgrid[i]);
		free(bndchemconcsurfgrid[i]);
		free(prtchemconcsurfgrid[i]);
		free(srbchemconcsurfgrid[i]);
		free(dischemfracwatergrid[i]);
		free(bndchemfracwatergrid[i]);
		free(mblchemfracwatergrid[i]);
		free(prtchemfracwatergrid[i]);
		free(dischemfracsurfgrid[i]);
		free(bndchemfracsurfgrid[i]);
		free(mblchemfracsurfgrid[i]);
		free(prtchemfracsurfgrid[i]);
		free(infchemfluxgrid[i]);
		free(chemerosiongrid[i]);
		free(chemdepositiongrid[i]);
		free(chemnetaccumgrid[i]);

	}	//end loop over number of reporting groups

	//Deallocate memory for chemical grid file names (total, dissolved, bound, and particulate)
	free(totchemconcwatergrid);
	free(dischemconcwatergrid);
	free(bndchemconcwatergrid);
	free(prtchemconcwatergrid);
	free(srbchemconcwatergrid);
	free(totchemconcsurfgrid);
	free(dischemconcsurfgrid);
	free(bndchemconcsurfgrid);
	free(prtchemconcsurfgrid);
	free(srbchemconcsurfgrid);
	free(dischemfracwatergrid);
	free(bndchemfracwatergrid);
	free(mblchemfracwatergrid);
	free(prtchemfracwatergrid);
	free(dischemfracsurfgrid);
	free(bndchemfracsurfgrid);
	free(mblchemfracsurfgrid);
	free(prtchemfracsurfgrid);
	free(infchemfluxgrid);
	free(chemerosiongrid);
	free(chemdepositiongrid);
	free(chemnetaccumgrid);

	/******************************************************/
	/*        Free Min and Max Statistics Variables       */
	/******************************************************/

	//Deallocate memory for minimum and maximum overland concentrations
	free(maxcchemov0);	//maximum chemical concentration in the overland plane water column (g/m3)
	free(mincchemov0);	//minimum chemical concentration in the overland plane water column (g/m3)
	free(maxcchemov1);	//maximum chemical concentration in the overland plane surface soil layer (g/m3)
	free(mincchemov1);	//minimum chemical concentration in the overland plane surface soil layer (g/m3)

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Deallocate memory for minimum and maximum channel concentrations
		free(maxcchemch0);	//maximum chemical concentration in the channel network water column (g/m3)
		free(mincchemch0);	//minimum chemical concentration in the channel network water column (g/m3)
		free(maxcchemch1);	//maximum chemical concentration in the channel network surface soil layer (g/m3)
		free(mincchemch1);	//minimum chemical concentration in the channel network surface soil layer (g/m3)

	}	//end if chnopt > 0

	/************************************************/
	/*        Free Chemical Property Arrays         */
	/************************************************/

	//Loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
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
					//Deallocate memory for chemical properties
					free(fdissolvedov[ichem][i][j]);		//fraction dissolved of a chemical overland [ichem][row][col][layer]
					free(fboundov[ichem][i][j]);			//fraction bound of a chemical overland [ichem][row][col][layer]

				}	//end if imask[i][j] is not nodatavalue (cell is is domain)

			}	//end loop over columns

			//Deallocate memory for chemical properties
			free(fdissolvedov[ichem][i]);		//fraction dissolved of a chemical overland [ichem][row][col][layer]
			free(fboundov[ichem][i]);			//fraction bound of a chemical overland [ichem][row][col][layer]

		}	//end loop over rows

		//Deallocate memory for chemical properties
		free(fdissolvedov[ichem]);		//fraction dissolved of a chemical overland [ichem][row][col][layer]
		free(fboundov[ichem]);			//fraction bound of a chemical overland [ichem][row][col][layer]

		//Loop over number of solids
		for(isolid=1; isolid<=nsolids; isolid++)
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
						//Deallocate memory for chemical properties
						free(fparticulateov[ichem][isolid][i][j]);		//fraction particulate of a chemical overland [ichem][row][col][layer]

					}	//end if imask[i][j] is not nodatavalue (cell is is domain)

				}	//end loop over columns

				//Deallocate memory for chemical properties
				free(fparticulateov[ichem][isolid][i]);	//fraction dissolved of a chemical overland [ichem][row][col][layer]

			}	//end loop over rows

			//Deallocate memory for chemical properties
			free(fparticulateov[ichem][isolid]);	//fraction particulate of a chemical overland [ichem][row][col][layer]

		}	//end loop over solids

		//Deallocate memory for chemical properties
		free(fparticulateov[ichem]);	//fraction particulate of a chemical overland [ichem][row][col][layer]

	}	//end loop over chemicals

	//Deallocate memory for chemical properties
	free(fdissolvedov);		//fraction dissolved of a chemical overland [ichem][row][col][layer]
	free(fboundov);			//fraction bound of a chemical overland [ichem][row][col][layer]
	free(fparticulateov);	//fraction particulate of a chemical overland [ichem][isolid][row][col][layer]

	//if channels are simulated
	if(chnopt > 0)
	{
		//Loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//Loop over number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Deallocate memory for chemical properties
					free(fdissolvedch[ichem][i][j]);		//fraction dissolved of a chemical in channels [ichem][row][col][layer]
					free(fboundch[ichem][i][j]);			//fraction bound of a chemical in channels [ichem][row][col][layer]

				}	//end loop over nodes

				//Deallocate memory for chemical properties
				free(fdissolvedch[ichem][i]);		//fraction dissolved of a chemical in channels [ichem][row][col][layer]
				free(fboundch[ichem][i]);			//fraction bound of a chemical in channels [ichem][row][col][layer]

			}	//end loop over links

			//Deallocate memory for chemical properties
			free(fdissolvedch[ichem]);		//fraction dissolved of a chemical in channels [ichem][row][col][layer]
			free(fboundch[ichem]);			//fraction bound of a chemical in channels [ichem][row][col][layer]

			//Loop over number of solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//Loop over number of links
				for(i=1; i<=nlinks; i++)
				{
					//Loop over number of nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//Deallocate memory for chemical properties
						free(fparticulatech[ichem][isolid][i][j]);		//fraction particulate of a chemical in channels [ichem][row][col][layer]

					}	//end loop over nodes

					//Deallocate memory for chemical properties
					free(fparticulatech[ichem][isolid][i]);	//fraction dissolved of a chemical in channels [ichem][row][col][layer]

				}	//end loop over links

				//Deallocate memory for chemical properties
				free(fparticulatech[ichem][isolid]);	//fraction particulate of a chemical in channels [ichem][row][col][layer]

			}	//end loop over solids

			//Deallocate memory for chemical properties
			free(fparticulatech[ichem]);	//fraction particulate of a chemical in channels [ichem][row][col][layer]

		}	//end loop over chemicals

		//Deallocate memory for chemical properties
		free(fdissolvedch);		//fraction dissolved of a chemical in channels [ichem][row][col][layer]
		free(fboundch);			//fraction bound of a chemical in channels [ichem][row][col][layer]
		free(fparticulatech);	//fraction particulate of a chemical in channels [ichem][isolid][row][col][layer]

	}	//end if chnopt > 0


	/*******************************************************/
	/*        Free Chemical Process Option Variables       */
	/*******************************************************/

	//Dellocate memory for chemical process option arrays
	free(partopt);			//chemical partitioning option
	free(bioopt);			//chemical biodegradation option
	free(hydopt);			//chemical hydrolysis option
	free(oxiopt);			//chemical oxidation option
	free(phtopt);			//chemical photolysis option
	free(vltopt);			//chemical volatilization option
	free(udropt);			//chemical extra (user-defined) reaction option

	/*****************************************************/
	/*        Free Chemical Partitioning Variables       */
	/*****************************************************/

	//Deallocate memory for chemical partitioning arrays
	free(kp);	//chemical partition coefficient (m3/g)
	free(kb);	//chemical DOC binding coefficient (m3/g)
	free(koc);	//chemical organic carbon coefficient (m3/g)
	free(nux);	//particle interaction parameter for partitioning (dimensionless)

	/*******************************************************/
	/*        Free Chemical Biodegradation Variables       */
	/*******************************************************/

//mlv: free variables from kinetic block in Data Group D...

	/***************************************************/
	/*        Free Chemical Hydrolysis Variables       */
	/***************************************************/



	/**************************************************/
	/*        Free Chemical Oxidation Variables       */
	/**************************************************/



	/***************************************************/
	/*        Free Chemical Photolysis Variables       */
	/***************************************************/




	/*******************************************************/
	/*        Free Chemical Volatilization Variables       */
	/*******************************************************/



	/**************************************************************/
	/*        Free Chemical User-Defined Reaction Variables       */
	/**************************************************************/




//End of function: Return to FreeMemory
}
