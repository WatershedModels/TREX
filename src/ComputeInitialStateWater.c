/*---------------------------------------------------------------------
C-  Function:	ComputeInitialStateWater.c
C-
C-  Purpose/
C-  Methods:    Computes the total volume of water stored in
C-              the overland plane and channel network at the
C-              start of the simulation.
C-
C-  Inputs:     hov[][] (at tstart),
C-              sweov[][] (at tstart),
C-              hch[][] (at tstart)
C-
C-  Outputs:    initialwaterov[][],
C-              initialsweov[][],
C-              initialwaterch[][]
C-
C-  Controls:   imask, snowopt, chnopt
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
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		20-DEC-2006
C-
C-  Revisions:	Added option for snow hydrology
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

void ComputeInitialStateWater()
{
	//local variable declarations
	int
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

				//Compute water volume in this overland cell (m3)
				initialwaterov[i][j] = hov[i][j] * (w * w - achsurf);

				//Compute cumulative water volume in the overland plane (m3)
				initialwaterovvol = initialwaterovvol + initialwaterov[i][j];

				//if snowfall or snowmelt are simulated
				if(snowopt > 0 || meltopt > 0)
				{
					//Compute snowpack volume (as swe) in this overland cell (m3)
					initialsweov[i][j] = sweov[i][j] * (w * w - achsurf);

					//Compute cumulative snowpack volume (as swe) in the overland plane (m3)
					initialsweovvol = initialsweovvol + initialsweov[i][j];

				}	//end if snowfall > 0 or meltop > 0

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

				//channel volume at flow depth
				initialwaterch[i][j] = achcross * lch;

				//Compute cumulative water volume in the channel network (m3)
				initialwaterchvol = initialwaterchvol + initialwaterch[i][j];

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt == 1

//End of function: Return to ComputeInitialState
}
