/*---------------------------------------------------------------------
C-  Function:	ChannelChemicalHydrolysis.c
C-
C-	Purpose/	Compute the hydrolysis flux of chemicals in the
C-	Methods:	channel network.  Chemical hydrolysis is assumed
C-				to occur at the same rate for all chemical phases.
C-
C-
C-  Inputs:	    channel properties,
C-              khydwch[], khydsch[]
C-
C-  Outputs:    hydchemchoutflux[][][][]
C-
C-  Controls:   imask[][], hydopt[]
C-
C-  Calls:      None
C-
C-  Called by:  ChannelChemicalKinetics
C-
C-	Created:	Zhonglong Zhang
C-              Environmental Laboratory - ERDC
C-              U.S. Army Corps of Engineers
C-              Vicksburg, MS
C-
C-	Date:		15-MAY-2005
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

void ChannelChemicalHydrolysis()
{
/*
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical type
		isolid,		//loop index for solids (particle) type
		ilayer;		//index for surface layer in the sediment stack

	float
		hchan,		//depth of water in channel (m)
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		twch,		//top width of channel at top of bank (m)
		achcross,	//cross sectional area of channel at present water depth (m2)
		watervol,	//water column volume of node (at time t) (m3)
		sedvol;		//sediment layer volume of node (at time t) (m3)

	float
		kw,			//water column reaction rate (1/s)
		ks;			//sediment layer reaction rate (1/s)

	double
		potential,	//chemical hydrolysis potential (g)
		available;	//chemical mass available for hydrolysis (g)

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//Assign local water depth (m)
			hchan = hch[i][j];

			//Assign channel characteristics
			hbch = hbank[i][j];				//bank height (m)
			sslope = sideslope[i][j];		//side slope (dimensionless)
			twch = twidth[i][j];			//channel top width at top of bank (m)
			bwch = bwidth[i][j];			//bottom width (m)

			//Compute channel characteristics...
			//
			//if the flow depth <= bank height
			if(hchan <= hbch)
			{
				//Cross-sectional area for trapezoidal, triangular, or rectangular channel
				achcross = (bwch + sslope * hchan) * hchan;
			}
			else	//Flow depth greater than channel depth
			{
				//Cross-sectional area for trapezoidal, triangular, or rectangular channel
				//plus rectangular area above bank height
				achcross = (bwch + sslope * hbch) * hbch
					+ (hchan - hbch) * twch;

			}	//end if hchan <= bank height

			//compute present water column volume (m3) (at time t)
			watervol = achcross * chanlength[i][j];

			//set the surface layer number
			ilayer = nstackch[i][j];

			//set the surface sediment layer volume (m3)
			sedvol = vlayerch[i][j][ilayer];

			//loop over number of chemicals
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//if the chemical hydrolizes
				if(hydopt[ichem] > 0)
				{
					//Compute hydrolysis flux for water column...
					//
					//Note:  Hydrolysis rates may be either first order (1/s)
					//       or second order (m3/cell/s).  When a second oder rate
					//       is input, bacterial concentrations must be specified.
					//       When multiplied by the bacterial concentration, the
					//       resulting rate value is in 1/s.  When a first order
					//       rate is input, the default bacterial concentration
					//       is 1.0.
					//
					//compute the effective first order hydrolysis rate (1/s)
					kw = khydwch[ichem] * bacteriach[i][j][0];

					//compute the hydrolysis flux in the water column (g/s)
					hydchemchoutflux[ichem][i][j][0] = kw * cchemch[ichem][i][j][0]
						* watervol;

					//compute the chemical hydrolysis mass potential (g)
					potential = hydchemchoutflux[ichem][i][j][0] * dt[idt];

					//compute the mass available in the water column (g)
					available = watervol * cchemch[ichem][i][j][0]
						- biochemchoutflux[ichem][i][j][0] * dt[idt];

					//Developer's Note:  The check for available < 0.0 is not
					//                   needed here but is needed in general.
					//                   It is left here as a template for
					//                   future code development.

					//if the available mass < 0.0
					if(available < 0.0)
					{
						//set the available mass (g) to zero
						available = 0.0;

					}	//end if available < 0.0

					//if the hydrolysis potential > available mass
					if(potential > available)
					{
						//scale the hyddegrdation flux for the water column (g/s)
						hydchemchoutflux[ichem][i][j][0] = (float)(available / dt[idt]);

					}	//end if potential > available

					//Compute hydrolysis flux for surface sediment layer...
					//
					//compute the effective first order hydrolysis rate (1/s)
					ks = khydsch[ichem] * bacteriach[i][j][ilayer];

					//compute the hydrolysis flux in the sediment bed (g/s)
					hydchemchoutflux[ichem][i][j][ilayer] = ks * cchemch[ichem][i][j][ilayer]
							* sedvol;

					//compute the chemical hydrolysis mass potential (g)
					potential = hydchemchoutflux[ichem][i][j][ilayer] * dt[idt];

					//compute the mass available in the sediment layer (g)
					available = sedvol * cchemch[ichem][i][j][ilayer]
						- biochemchoutflux[ichem][i][j][ilayer] * dt[idt];

					//if the available mass < 0.0
					if(available < 0.0)
					{
						//set the available mass (g) to zero
						available = 0.0;

					}	//end if available < 0.0

					//if the hyddegrdation potential > available mass
					if(potential > available)
					{
						//scale the hydrolysis flux for the sediment (g/s)
						hydchemchoutflux[ichem][i][j][ilayer] = (float)(available / dt[idt]);

					}	//end if potential > available

					//Developer's Note:  If reaction processes occur in the
					//                   subsurface sediment layers, flux
					//                   terms for these layers would need
					//                   to be computed here...

				}	//end if hydopt[] > 0

			}	//end loop over number of chemicals

		}	//end loop over nodes

	}	//end loop over links
*/
//End of function: Return to ChannelChemicalKinetics
}
