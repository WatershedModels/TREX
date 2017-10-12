/*----------------------------------------------------------------------
C-  Function:	ComputeChemicalGroupPhaseAve.c
C-
C-	Purpose/	Compute averages of chemical state variable phase
C-	Methods:	fractions for chemical reporting groups as specified
C-              in Data Group D.
C-
C-
C-	Inputs:		igrid, jgrid, itype
C-
C-	Outputs:	cgroupave[]
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
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		13-JUN-2008
C-
C-	Revisions:  Revised function argument list to provide an index
C-              to determine whether concentration sums are for water
C-              column or surface soil/sediment.  Use itype argument
C-              as a switch (water or bed) and then set array index
C-              for layer as ilayer (water: ilayer = 0; bed: ilayer =
C-              nstackov[][] or nstackch[][]).
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

void ComputeChemicalGroupPhaseAve(float *cgroupave, int igrid, int jgrid, int itype, int iphase)
{
	//Local variable declarations/definitions
	int
		igroup,		//local counter for reporting group number
		ichem,		//local counter for chemical type
		isolid,		//local counter for solids type
		chanlink,	//link number of channel in channel cell
		channode,	//node number of channel in channel cell
		ilayer;		//layer to summarize results (water column or surface soil/sediment)

	float
		*fraction,	//fraction of chemical in phase iphase [igroup]
		*total;		//total chemical concentration [igroup]

	//Allocate memory for local group phase fraction and total concentration arrays
	fraction = (float *)malloc((ncgroups+1) * sizeof(float));
	total = (float *)malloc((ncgroups+1) * sizeof(float));

	//initialize chemical group sums...
	//
	//loop over chemical groups
	for(igroup=1; igroup<=ncgroups; igroup++)
	{
        //set group values to zero
		cgroupave[igroup] = 0.0;

		//initialize the phase fraction
		fraction[igroup] = 0.0;

		//initialize the total concentration
		total[igroup] = 0.0;

	}	//end loop over chemical groups

	//if the cell is a channel cell
	if(imask[igrid][jgrid] > 1)
	{
		//set channel link number
		chanlink = link[igrid][jgrid];

		//set channel node number
		channode = node[igrid][jgrid];

		//set layer reference based on itype
		//
		//if itype = 0 (water column)
		if(itype == 0)
		{
			//set the layer to the water column
			ilayer = 0;
		}
		else	//else itype != 0 (sediment bed)
		{
			//set the layer to the surface sediment layer
			ilayer = nstackch[chanlink][channode];

		}	//end if itype = 0

		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//get group number for this chemical type
			igroup = cgroupnumber[ichem];

			//Set phase/fraction of chemical
			//
			//if the phase = 1 (dissolved)
			if(iphase == 1)
			{
				//the fraction is the dissolved fraction
				fraction[igroup] = fraction[igroup]
					+ fdissolvedch[ichem][chanlink][channode][ilayer]
					* cchemch[ichem][chanlink][channode][ilayer];
			}
			//else if the phase = 2 (bound)
			else if(iphase == 2)
			{
				//the fraction is the dissolved fraction
				fraction[igroup] = fraction[igroup]
					+ fboundch[ichem][chanlink][channode][ilayer]
					* cchemch[ichem][chanlink][channode][ilayer];
			}
			//else if the phase = 3 (mobile = dissolved + bound)
			else if(iphase == 3)
			{
				//the fraction is the mobile fraction
				fraction[igroup] = fraction[igroup]
					+ (fdissolvedch[ichem][chanlink][channode][ilayer]
					+ fboundch[ichem][chanlink][channode][ilayer])
					* cchemch[ichem][chanlink][channode][ilayer];
			}
			//else if the phase = 4 (particulate)
			else if(iphase == 4)
			{
				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//the fraction is the sum of particulate fractions
					fraction[igroup] = fraction[igroup]
						+ fparticulatech[ichem][isolid][chanlink][channode][ilayer]
						* cchemch[ichem][chanlink][channode][ilayer];

				}	//end loop over solids
			}
			else	//else the phase = 0 (total chemical)
			{
				//the fraction is the total
				fraction[igroup] = fraction[igroup]
					+ cchemch[ichem][chanlink][channode][ilayer];

			}	//end if iphase = 1

			//compute the total concentration
			total[igroup] = total[igroup]
				+ cchemch[ichem][chanlink][channode][ilayer];

		}	//end loop over chemicals
	}
	else	//else the cell is an overland cell
	{
		//set layer reference based on itype
		//
		//if itype = 0 (water column)
		if(itype == 0)
		{
			//set the layer to the water column
			ilayer = 0;
		}
		else	//else itype != 0 (soil column)
		{
			//set the layer to the surface soil layer
			ilayer = nstackov[igrid][jgrid];

		}	//end if itype = 0

		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//get group number for this chemical type
			igroup = cgroupnumber[ichem];

			//Set phase/fraction of chemical
			//
			//if the phase = 1 (dissolved)
			if(iphase == 1)
			{
				//the fraction is the dissolved fraction
				fraction[igroup] = fraction[igroup]
					+ fdissolvedov[ichem][igrid][jgrid][ilayer]
					* cchemov[ichem][igrid][jgrid][ilayer];
			}
			//else if the phase = 2 (bound)
			else if(iphase == 2)
			{
				//the fraction is the dissolved fraction
				fraction[igroup] = fraction[igroup]
					+ fboundov[ichem][igrid][jgrid][ilayer]
					* cchemov[ichem][igrid][jgrid][ilayer];
			}
			//else if the phase = 3 (mobile = dissolved + bound)
			else if(iphase == 3)
			{
				//the fraction is the mobile fraction
				fraction[igroup] = fraction[igroup]
					+ (fdissolvedov[ichem][igrid][jgrid][ilayer]
					+ fboundov[ichem][igrid][jgrid][ilayer])
					* cchemov[ichem][igrid][jgrid][ilayer];
			}
			//else if the phase = 4 (particulate)
			else if(iphase == 4)
			{
				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//the fraction is the sum of particulate fractions
					fraction[igroup] = fraction[igroup]
						+ fparticulateov[ichem][isolid][igrid][jgrid][ilayer]
						* cchemov[ichem][igrid][jgrid][ilayer];

				}	//end loop over solids
			}
			else	//else the phase = 0 (total chemical)
			{
				//the fraction is the total
				fraction[igroup] = fraction[igroup]
					+ cchemov[ichem][igrid][jgrid][ilayer];

			}	//end if iphase = 1

			//compute the total concentration
			total[igroup] = total[igroup] + cchemov[ichem][igrid][jgrid][ilayer];

		}	//end loop over chemicals

	}	//end if imask[][] > 1

	//loop over chemical groups
	for(igroup=1; igroup<=ncgroups; igroup++)
	{
		//if the total concentration is greater than zero
		if(total[igroup] > 0.0)
		{
			//compute average phase fraction of each chemical in the reporting group
			cgroupave[igroup] = fraction[igroup] / total[igroup];

		}	//end if total[igroup] > 0.0

	}	//end loop over chemical groups

	//deallocate memory for local variables 
	free(fraction);
	free(total);

//End of function: Return to WriteGridsChemical
}
