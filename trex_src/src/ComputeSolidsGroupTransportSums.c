/*----------------------------------------------------------------------
C-  Function:	ComputeSolidsGroupTransportSums.c
C-
C-	Purpose/	Compute sums of solids state variable concentrations
C-  Methods:    for total solids and other solids reporting groups as
C-              specified in Data Group C.
C-
C-
C-	Inputs:		igrid, jgrid
C-
C-	Outputs:	sgroupsum[]
C-
C-	Controls:   nsolids, nsgroups, imask[][]
C-
C-	Calls:		none
C-
C-	Called by:	WriteGridsSolids
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

void ComputeSolidsGroupTransportSums(float *sgroupsum, int igrid, int jgrid, int ipath)
{
	//Local variable declarations/definitions
	int
		igroup,		//local counter for reporting group number
		isolid,		//local counter for solids type
		chanlink,	//link number of channel in channel cell
		channode;	//node number of channel in channel cell

	float
		transport;	//transport mass (kg)

	//initialize solids group sums...
	//
	//loop over solids groups (include zero for sum of all solids)
	for(igroup=0; igroup<=nsgroups; igroup++)
	{
        //set group values to zero
		sgroupsum[igroup] = 0.0;

	}	//end loop over solids groups

	//if the cell is a channel cell
	if(imask[igrid][jgrid] > 1)
	{
		//set channel link number
		chanlink = link[igrid][jgrid];

		//set channel node number
		channode = node[igrid][jgrid];

		//loop over solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//get group number for this solids type
			igroup = sgroupnumber[isolid];

			//Set transport path
			//
			//if the path = 1 (gross erosion)
			if(ipath == 1)
			{
				//the transport is the gross erosion (kg)
				transport = (float)(erssedchinmass[isolid][chanlink][channode][0]);
			}
			//else if the path = 2 (gross deposition)
			else if(ipath == 2)
			{
				//the transport is the gross deposition (kg)
				transport = (float)(depsedchoutmass[isolid][chanlink][channode][0]);
			}
			//else if the path = 3 (net accumulation = deposition - erosion)
			else if(ipath == 3)
			{
				//the transport is net accumulation (deposition - erosion) (kg)
				transport = (float)(depsedchoutmass[isolid][chanlink][channode][0]
					- erssedchinmass[isolid][chanlink][channode][0]);

			}	//end if ipath = 1...

			//sum total solids transport
			sgroupsum[0] = sgroupsum[0] + transport;

			//add the transport of each solids type to its reporting group
			sgroupsum[igroup] = sgroupsum[igroup] + transport;

		}	//end loop over solids
	}
	else	//else the cell is an overland cell
	{
		//loop over solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//get group number for this solids type
			igroup = sgroupnumber[isolid];

			//Set transport path
			//
			//if the path = 1 (gross erosion)
			if(ipath == 1)
			{
				//the transport is the gross erosion (kg)
				transport = (float)(erssedovinmass[isolid][igrid][jgrid][0]);
			}
			//else if the path = 2 (gross deposition)
			else if(ipath == 2)
			{
				//the transport is the gross deposition (kg)
				transport = (float)(depsedovoutmass[isolid][igrid][jgrid][0]);
			}
			//else if the path = 3 (net accumulation = deposition - erosion)
			else if(ipath == 3)
			{
				//the transport is net accumulation (deposition - erosion) (kg)
				transport = (float)(depsedovoutmass[isolid][igrid][jgrid][0]
					- erssedovinmass[isolid][igrid][jgrid][0]);

			}	//end if ipath = 1...

			//sum total solids transport
			sgroupsum[0] = sgroupsum[0] + transport;

			//add the transport of each solids type to its reporting group
			sgroupsum[igroup] = sgroupsum[igroup] + transport;

		}	//end loop over solids

	}	//end if imask[][] > 1
				
//End of function: Return to WriteGridsSolids
}
