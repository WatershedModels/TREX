/*---------------------------------------------------------------------
C-  Function:	SolidsTransport.c
C-
C-	Purpose/	SolidsTransport is called to compute derivative terms
C-	Methods:	(rates) for solids transport processes: deposition,
C-              erosion, kinetics, advection, and dispersion for the
C-              overland plane and channel network.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   depovopt, advovopt, dspovopt, ersovopt
C-              depchopt, advchopt, dspchopt, erschopt
C-
C-  Calls:      OverlandSolidsDeposition,
C-              OverlandSolidsAdvection,
C-              OverlandSolidsDispersion,
C-              OverlandSolidsTransportCapacity,
C-              OverlandSolidsErosion,
C-              ChannelSolidsDeposition,
C-              ChannelSolidsAdvection,
C-              ChannelSolidsDispersion,
C-              ChannelSolidsTransportCapacity,
C-              ChannelSolidsErosion,
C-              FloodplainSolidsTransfer
C-
C-  Called by:  trex (main)
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
C-	Date:		10-MAR-2004
C-
C-  Revised:
C-
C-  Date:
C-
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void SolidsTransport()
{
	//if any overland transformations are simulated
	if(tnsovopt > 0)
	{
        //Compute kinetic processes (degradation, reactions...) for overland plane
		OverlandSolidsKinetics();

	}	//end if tnsovopt > 0

	//if overland deposition is simulated (depovopt > 0)
	if(depovopt > 0)
	{
        //Compute effective deposition for overland plane
		OverlandSolidsDeposition();

	}	//end if depovopt > 0

	//if overland advection is similated (advovopt > 0)
	if(advovopt > 0)
	{
		//Compute overland advection
		OverlandSolidsAdvection();

	}	//end if advovopt > 0

	//if overland dispersion is similated (dspovopt > 0)
	if(dspovopt > 0)
	{
		//Compute overland dispersion
		OverlandSolidsDispersion();

	}	//end if dspovopt > 0

	//if overland erosion is simulated (ersovopt > 0)
	if(ersovopt > 0)
	{
		//Note: If other transport capacity options (Li et al.,
		//      Musgrave, etc.) are added, the ersovopt values
		//      would be inserted here so that the excess shear
		//      option would be the last (largest) ersovopt value.
		//
		//if the capacity limited erosion option is selected (K-R or generalized)
		if(ersovopt <= 2)
		{
			//Compute transport capacity for overland plane
			OverlandSolidsTransportCapacity();

		}	//end if ersovopt <= 2

		//Compute erosion for overland plane
		OverlandSolidsErosion();

	}	//end if ersovopt > 0

	//if channels are simulated
	if(chnopt > 0)
	{
		//if any channel solids transformations are simulated
		if(tnschopt > 0)
		{
			//Compute kinetic processes (degradation, reactions...) for channels
			ChannelSolidsKinetics();

		}	//end if tnschopt > 0

		//if channel deposition is similated (depchopt > 0)
		if(depchopt > 0)
		{
			//Compute deposition for channels
			ChannelSolidsDeposition();

		}	//end if depchopt > 0

		//if channel advection is similated (advovchopt > 0)
		if(advchopt > 0)
		{
			//Compute channel advection
			ChannelSolidsAdvection();

		}	//end if advchopt > 0

		//if channel dispersion is similated (dspchopt > 0)
		if(dspchopt > 0)
		{
			//Compute channel dispersion
			ChannelSolidsDispersion();

		}	//end if dspchopt > 0

		//if channel erosion is simulated (erschopt > 0)
		if(erschopt > 0)
		{
			//Note: If other transport capacity options (Ackers-White,
			//      Yang, etc.) are added, the erschopt values would
			//      be inserted here so that the excess shear option
			//      would be the last (largest) erschopt value.
			//
			//if the capacity limited erosion option is selected
			if(erschopt <= 2)
			{
				//Compute transport capacity for channels (E-H, A-W, etc.)
				ChannelSolidsTransportCapacity();

			}	//end if erschopt <= 2

			//Compute erosion for channels
			ChannelSolidsErosion();

		}	//end if erschopt > 0

		//Compute solids transfers between overland plane and channels
		FloodplainSolidsTransfer();

	}	//end if channels are simulated

//End of function: Return to trex
}
