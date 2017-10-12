/*---------------------------------------------------------------------
C-  Function:	ComputeInitialStateChemical.c
C-
C-  Purpose/    Computes the initial chemical concentrations at the
C-  Methods:    start of the simulation.
C-
C-  Inputs:     
C-
C-  Outputs:    
C-
C-  Controls:   ksim, imask, chnopt
C-
C-  Calls:      None
C-
C-  Called by:  ComputeInitialState
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-				Colorado State University
C-				Fort Collins, CO 80523
C-
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		20-MAR-2004
C-
C-	Revised:
C-
C-	Date:	
C-
C-	Revisions:
C-
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void ComputeInitialStateChemical()
{
	//local variable declarations
	int
		ichem,		//loop index for chemical type
		chanlink,	//channel link number
		channode;	//channel node number

	float
		hflow,		//flow depth of channel (m)
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achcross,	//cross section area of channel (m)
		achsurf;	//surface area of channel (m)

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Compute the chemical mass in the water column
				//
				//if the cell is an overland cell
				if(imask[i][j] == 1)
				{
					//No channel exists, channel surface area is zero
					achsurf = 0.0;
				}
				//else if the cell is a channel cell
				else if(imask[i][j] == 2)
				{
					//Get channel link and node
					chanlink = link[i][j];
					channode = node[i][j];

					//Compute area of channel within cell...
					//
					//Assign channel flow depth
					hflow = hch[chanlink][channode];				//channel water depth (m)

					//Assign channel characteristics
					lch = chanlength[chanlink][channode];			//channel length (m) (includes sinuosity)
					twch = twidth[chanlink][channode];				//channel top width at bank height (m)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;

				}	//end if imask == 1

				//loop over chemical
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//Compute chemical mass in this overland cell (kg)
					initialchemov[ichem][i][j][0] = (float)(cchemov[ichem][i][j][0]
						* hov[i][j] * (w * w - achsurf) / 1000.0);

					//Compute cumulative chemical mass in the overland plane (kg)
					initialchemmassov[ichem][0] = initialchemmassov[ichem][0]
						+ initialchemov[ichem][i][j][0];

					//Compute the chemical mass in the soil stack
					//
					//loop over number of layers
					for(k=1; k<=nstackov0[i][j]; k++)
					{
						//Compute chemical mass in this soil layer cell (kg)
						initialchemov[ichem][i][j][k] = (float)(cchemov[ichem][i][j][k]
							* vlayerov[i][j][k] / 1000.0);

						//Compute cumulative chemical mass in this entire soil layer (kg)
						initialchemmassov[ichem][k] = initialchemmassov[ichem][k]
							+ initialchemov[ichem][i][j][k];

					}	//end loop over layers

				}	//end loop over chemicals

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//if channels are simulated
	if(chnopt == 1)
	{
		//Loop over links
		for(i=1; i<=nlinks; i++)
		{
			//Loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Compute area of channel within cell...
				//
				//Assign channel flow depth
				hflow = hch[i][j];				//channel water depth (m)

				//Assign channel characteristics
				bwch = bwidth[i][j];			//channel bottom width (m)
				hbch = hbank[i][j];				//channel bank height (m)
				sslope = sideslope[i][j];		//channel side slope
				lch = chanlength[i][j];			//channel length (m) (includes sinuosity)

				//if flow depth <= bank height (cell is not flooded)
				if(hflow <= hbch)
				{
					//channel cross-sectional area at flow depth (m2)
					achcross = (float)(bwch + sslope * hflow) * hflow;
				}
				else	//cell is flooded
				{
					//channel top width of channel at top of bank (m)
					twch = twidth[i][j];

					//channel cross-sectional area at top of bank
					//plus retangular area above the bank height (m2)
					achcross = (bwch + sslope * hbch) * hbch
						+ (hflow - hbch) * twch;

				}	//end if hflow <= hbch

				//loop over chemicals
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//Compute chemical mass in this channel node (kg)
					initialchemch[ichem][i][j][0] = (float)(cchemch[ichem][i][j][0]
						* achcross * lch / 1000.0);

					//Compute cumulative chemical mass in the channel network (kg)
					initialchemmassch[ichem][0] = initialchemmassch[ichem][0]
						+ initialchemch[ichem][i][j][0];

					//Compute the chemical mass in the sediment stack
					//
					//loop over number of layers
					for(k=1; k<=nstackch0[i][j]; k++)
					{
						//Compute chemical mass in this channel sediment node (kg)
						initialchemch[ichem][i][j][k] = (float)(cchemch[ichem][i][j][k]
							* vlayerch[i][j][k] / 1000.0);

						//Compute cumulative chemical mass in the entire sediment layer (kg)
						initialchemmassch[ichem][k] = initialchemmassch[ichem][k]
							+ initialchemch[ichem][i][j][k];

					}	//end loop over layers

				}	//end loop over chemicals

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt == 1

//End of function: Return to ComputeInitialState
}
