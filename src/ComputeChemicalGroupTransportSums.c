/*----------------------------------------------------------------------
C-  Function:	ComputeChemicalGroupTransportSums.c
C-
C-	Purpose/	Compute sums of chemical state variable concentrations
C-  Methods:    for chemical reporting groups as specified in Data
C-              Group D.
C-
C-
C-	Inputs:		igrid, jgrid, ilayer 
C-
C-	Outputs:	cgroupsum[]
C-
C-	Controls:   nchems, ncgroups, imask[][]
C-
C-	Calls:		none
C-
C-	Called by:	WriteGridsChemical
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		18-FEB-2005
C-
C-	Revisions:
C-
C-	Date:		
C-
C-	Revisions:
C-
C-  Date:
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

void ComputeChemicalGroupTransportSums(float *cgroupsum, int igrid, int jgrid, int ipath)
{
	//Local variable declarations/definitions
	int
		igroup,		//local counter for reporting group number
		ichem,		//local counter for chemical type
		chanlink,	//link number of channel in channel cell
		channode;	//node number of channel in channel cell

	float
		transport;	//transport flux (g/s) or mass (kg)

	//initialize chemical group sums...
	//
	//loop over chemical groups
	for(igroup=1; igroup<=ncgroups; igroup++)
	{
        //set group values to zero
		cgroupsum[igroup] = 0.0;

	}	//end loop over chemical groups

	//if the cell is a channel cell
	if(imask[igrid][jgrid] > 1)
	{
		//set channel link number
		chanlink = link[igrid][jgrid];

		//set channel node number
		channode = node[igrid][jgrid];

		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//get group number for this chemical type
			igroup = cgroupnumber[ichem];

			//Set transport path
			//
			//if the path = 0 (infiltration)
			if(ipath == 0)
			{
				//the transport is the chemical infiltration flux (g/s)
				transport = infchemchoutflux[ichem][chanlink][channode][0];
			}
			//if the path = 1 (gross erosion)
			if(ipath == 1)
			{
				//the transport is the gross erosion (kg)
				transport = (float)(erschemchinmass[ichem][chanlink][channode][0]);
			}
			//else if the path = 2 (gross deposition)
			else if(ipath == 2)
			{
				//the transport is the gross deposition (kg)
				transport = (float)(depchemchoutmass[ichem][chanlink][channode][0]);
			}
			//else if the path = 3 (net accumulation = deposition - erosion)
			else if(ipath == 3)
			{
				//the transport is net accumulation (deposition - erosion) (kg)
				transport = (float)(depchemchoutmass[ichem][chanlink][channode][0]
					- erschemchinmass[ichem][chanlink][channode][0]);

			}	//end if ipath = 0...

			//add the transport of each chemical type to its reporting group
			cgroupsum[igroup] = cgroupsum[igroup] + transport;

		}	//end loop over chemicals
	}
	else	//else the cell is an overland cell
	{
		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//get group number for this chemical type
			igroup = cgroupnumber[ichem];

			//Set transport path
			//
			//if the path = 0 (infiltration)
			if(ipath == 0)
			{
				//the transport is the chemical infiltration flux (g/s)
				transport = infchemovoutflux[ichem][igrid][jgrid][0];
			}
			//if the path = 1 (gross erosion)
			if(ipath == 1)
			{
				//the transport is the gross erosion (kg)
				transport = (float)(erschemovinmass[ichem][igrid][jgrid][0]);
			}
			//else if the path = 2 (gross deposition)
			else if(ipath == 2)
			{
				//the transport is the gross deposition (kg)
				transport = (float)(depchemovoutmass[ichem][igrid][jgrid][0]);
			}
			//else if the path = 3 (net accumulation = deposition - erosion)
			else if(ipath == 3)
			{
				//the transport is net accumulation (deposition - erosion) (kg)
				transport = (float)(depchemovoutmass[ichem][igrid][jgrid][0]
					- erschemovinmass[ichem][igrid][jgrid][0]);

			}	//end if ipath = 0...

			//add the transport of each chemical type to its reporting group
			cgroupsum[igroup] = cgroupsum[igroup] + transport;

		}	//end loop over chemicals

	}	//end if imask[][] > 1
				
//End of function: Return to WriteGridsChemical
}
