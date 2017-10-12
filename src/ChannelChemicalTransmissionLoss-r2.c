/*---------------------------------------------------------------------
C-  Function:	ChannelChemicaltransmissionLoss.c
C-
C-	Purpose/	Computes the transmission loss flux of chemicals in the
C-	Methods:	channel network (chemical tranmission loss to sediment).
C-
C-				Chemical tranmission loss is linked to the sediment
C-				column.  The transmission loss rate is used to compute
C-				the flux of chemicals entering or leaving each layer
C-				of the stack depending on the wetting front elevation.
C-
C-
C-  Inputs:	    translossrate[][], fdissolvedch[][][][],
C-              fboundch[][][][]
C-
C-  Outputs:    infchemchoutflux[][][][][], infchemchinflux[][][][][]
C-
C-  Controls:   elevlayerch[][][]
C-
C-  Calls:      None
C-
C-  Called by:  ChemicalTransport
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
C-	Date:		17-JUN-2005
C-
C-	Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       13-JUN-2008
C-
C-	Revisions:  Added code to calculate fluxes as the tranmission loss
C-              wetting front moves through bed layers and transports
C-              chemicals between layers.
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

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void ChannelChemicalTransmissionLoss()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical type
		ilayer;		//index for surface layer in the soil stack

	float
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		twch,		//top width of channel at top of bank (m)
		achcross,	//cross sectional area of channel (m2)
		watervol,	//water column volume of node (at time t) (m3)
		abed,		//surface area of sediment bed
		fmobile,	//sum of mobile fractions (dissolved + bound)
		wfe,		//wetting front elevation (m)
		outflow;	//flow rate leaving a bed layer (a velocity) (m/s)

	double
		potential,	//chemical infiltration mass potential (g)
		available,	//chemical mass available for transport (g)
		sedvol;		//sediment layer volume of node (at time t) (m3)

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//Flux at the sediment surface (transmission loss)
			//
			//Note:  Infiltration transports dissolved and bound phases
			//
			//Assign channel characteristics
			bwch = bwidth[i][j];				//bottom width (m)
			hbch = hbank[i][j];					//bank height (m)
			sslope = sideslope[i][j];			//side slope (dimensionless)

			//Compute present water volume...
			//
			//if the present flow depth <= bank height
			if(hch[i][j] <= hbch)
			{
				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hch[i][j]) * hch[i][j];
			}
			else	//else flow depth greater than channel depth
			{
				//Channel top width at top of bank (m)
				twch = twidth[i][j];

				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hbch) * hbch
						+ (hch[i][j] - hbch) * twch;

			}	//end if hch <= bank height

			//compute present water column volume (m3) (at time t)
			watervol = achcross * chanlength[i][j];

			//set the surface layer number
			ilayer = nstackch[i][j];

			//set surface area of bed (m2)
			abed = achbed[i][j][ilayer];

			//loop over number of chemical types
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//compute mobile fraction (sum of dissolved and bound)
				fmobile = fdissolvedch[ichem][i][j][0] + fboundch[ichem][i][j][0];

				//Developer's Note:  Also, for consistency with code for overland
				//                   chemical infiltration, flux and mass tracking
				//                   variables for channel chemical transmission
				//                   loss are infchemch (for infiltration) rather
				//                   than trlchemch (for transmission loss).  As
				//                   needed the variable names can be changed by
				//                   a global search and replace...
				//                   
				//compute the infiltration flux leaving the water column (g/s)
				infchemchoutflux[ichem][i][j][0] = translossrate[i][j]
					* abed * cchemch[ichem][i][j][0] * fmobile;

				//compute the infiltration mass potential (g)
				potential = infchemchoutflux[ichem][i][j][0] * dt[idt];

				//Developer's Note:  When computing the mass available
				//                   for transmission loss, the mass
				//                   lost to transformation processes
				//                   should only include the mass lost
				//                   from the mobile phase.  This will
				//                   require further code development.
				//
				//compute the mass available in the water column (g)
				available = watervol * cchemch[ichem][i][j][0] * fmobile
					- (biochemchoutflux[ichem][i][j][0]
					+ hydchemchoutflux[ichem][i][j][0]
					+ oxichemchoutflux[ichem][i][j][0]
					+ phtchemchoutflux[ichem][i][j][0]
					+ radchemchoutflux[ichem][i][j][0]
					+ vltchemchoutflux[ichem][i][j][0]
					+ udrchemchoutflux[ichem][i][j][0]) * dt[idt];

				//if the available mass < 0.0
				if(available < 0.0)
				{
					//set the available mass (g) to zero
					available = 0.0;

				}	//end if available < 0.0

				//if the infiltration potential > available mass
				if(potential > available)
				{
					//scale the chemical infiltration flux leaving the water column (g/s)
					infchemchoutflux[ichem][i][j][0] = (float)(available / dt[idt]);

				}	//end if potential > available

				//compute the infiltration flux entering the surface sediment layer (g/s)
				infchemchinflux[ichem][i][j][ilayer] = infchemchoutflux[ichem][i][j][0];

			}	//end loop over number of chemical types

			//compute the wetting front elevation (m)
			wfe = elevationch0[i][j] - translossdepth[i][j];

			//Compute flux between sediment layers...
			//
			//loop over layers (in reverse order: top down)
			for(ilayer=nstackch[i][j]; ilayer>=1; ilayer--)
			{
				//set surface area of bed (m2)
				abed = achbed[i][j][ilayer];

				//assign local value for sediment layer volume (m3)
				sedvol = vlayerch[i][j][ilayer];

				//Determine if the wetting front is within this bed layer
				//
				//Two cases exist:
				//
				//   (1) wetting front is above bottom of this layer,
				//       so the flow across the interface is zero (no
				//       transport occurs)
				//
				//   (2) wetting front is below bottom of this layer, so
				//       the flow across the interface is equal to the
				//       transmission loss rate at the sediment-water
				//       interface (transport out of the layer occurs)
				//
				//Note:  elevlayerch[][][ilayer] = elevation at layer top
				//       elevlayerch[][][ilayer-1] = elevation at layer bottom
				//       elevlayerch[][][0] = elevation at bottom of stack
				//
				//if the wetting from elevation is above the layer bottom boundary
				if(wfe >= elevlayerch[i][j][ilayer-1])
				{
					//flow rate leaving the present layer (and entering the next) is zero
					outflow = 0.0;
				}
				else	//else the wetting front is below the layer bottom boundary
				{
					//flow rate leaving the present layer is the transmission loss rate (m/s)
					outflow = translossrate[i][j];

				}	//end if wfe >= elevlayerch[][][]...

				//loop over number of chemical types
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//compute mobile fraction (sum of dissolved and bound)
					fmobile = fdissolvedch[ichem][i][j][ilayer] + fboundch[ichem][i][j][ilayer];

					//compute the infiltration flux leaving this layer (g/s)
					infchemchoutflux[ichem][i][j][ilayer] = outflow
						* abed * cchemch[ichem][i][j][ilayer] * fmobile;

					//compute the infiltration mass potential (g)
					potential = infchemchoutflux[ichem][i][j][ilayer] * dt[idt];

					//Developer's Note:  When computing the mass available
					//                   for transmission loss, the mass
					//                   lost to transformation processes
					//                   should only include the mass lost
					//                   from the mobile phase.  This will
					//                   require further code development.
					//
					//compute the mass available in the water column (g)
					available = sedvol * cchemch[ichem][i][j][ilayer] * fmobile
						- (biochemchoutflux[ichem][i][j][ilayer]
						+ hydchemchoutflux[ichem][i][j][ilayer]
						+ oxichemchoutflux[ichem][i][j][ilayer]
						+ phtchemchoutflux[ichem][i][j][ilayer]
						+ radchemchoutflux[ichem][i][j][ilayer]
						+ vltchemchoutflux[ichem][i][j][ilayer]
						+ udrchemchoutflux[ichem][i][j][ilayer]) * dt[idt];

					//if the available mass < 0.0
					if(available < 0.0)
					{
						//set the available mass (g) to zero
						available = 0.0;

					}	//end if available < 0.0

					//if the infiltration potential > available mass
					if(potential > available)
					{
						//scale the chemical infiltration flux leaving this layer (g/s)
						infchemchoutflux[ichem][i][j][ilayer] = (float)(available / dt[idt]);

					}	//end if potential > available

					//compute the infiltration flux entering the next layer (g/s)
					infchemchinflux[ichem][i][j][ilayer] = infchemchoutflux[ichem][i][j][0];

				}	//end loop over number of chemical types

			}	//end loop over layers

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to ChemicalTransport
}
