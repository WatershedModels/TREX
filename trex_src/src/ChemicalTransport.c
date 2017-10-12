/*---------------------------------------------------------------------
C-  Function:	ChemicalTransport.c
C-
C-	Purpose/	ChemicalTransport is called to compute derivative terms
C-	Methods:	(rates) for chemical transport processes: deposition,
C-              erosion, kinetics, advection, and dispersion for the
C-              overland plane and channel network.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:	infopt, chnopt, ctlopt
C-
C-  Calls:      OverlandChemicalKinetics,
C-              OverlandChemicalInfiltration,
C-              OverlandChemicalDeposition,
C-              OverlandChemicalAdvection,
C-              OverlandChemicalErosion,
C-              ChannelChemicalKinetics,
C-              ChannelChemicalTransmissionLoss,
C-              ChannelChemicalDeposition,
C-              ChannelChemicalAdvection,
C-              ChannelChemicalDispersion,
C-              ChannelChemicalErosion,
C-              FloodplainChemicalTransfer
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
C-  Revisions:
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

void ChemicalTransport()
{
    //Compute kinetic processes (degradation, reactions...) for overland plane
	OverlandChemicalKinetics();

	//if infiltration is simulated (infopt > 0)
	if(infopt > 0)
	{
		//Compute overland chemical infiltration
		OverlandChemicalInfiltration();

	}	//end if infopt > 0

    //Compute effective deposition for overland plane (quiescent velocity * Pdep)
	OverlandChemicalDeposition();

	//Compute overland advection
	OverlandChemicalAdvection();

	//Compute overland dispersion
	OverlandChemicalDispersion();

	//Compute erosion for overland plane
	OverlandChemicalErosion();

	//if channels are simulated
	if(chnopt > 0)
	{
		//Compute kinetic processes (degradation, reactions...) for channels
		ChannelChemicalKinetics();

		//if channel transmission loss is simulated (ctlopt > 0)
		if(ctlopt > 0)
		{
			//Compute channel chemical transmission loss
			ChannelChemicalTransmissionLoss();

		}	//end if ctlopt > 0

		//Compute deposition for channels
		ChannelChemicalDeposition();

		//Compute channel advection
		ChannelChemicalAdvection();

		//Compute channel dispersion
		ChannelChemicalDispersion();

		//Compute erosion for channels
		ChannelChemicalErosion();

		//Compute chemical transfers between overland plane and channels
		FloodplainChemicalTransfer();

	}	//end if channels are simulated

//End of function: Return to trex
}
