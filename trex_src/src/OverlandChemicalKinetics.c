/*---------------------------------------------------------------------
C-  Function:	OverlandChemicalKinetics.c
C-
C-	Purpose/	Computes the transformation flux of chemicals subject to
C-	Methods:	kinetic reations (e.g. biodegradation, volatilization,
C-              photoloysis, etc.)  in the overland plane
C-
C-
C-  Inputs:	    None
C-
C-  Outputs:    None
C-
C-  Controls:   None
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
C-	Date:		29-DEC-2003
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

void OverlandChemicalKinetics()
{
	//if any chemical partitions (if partopt[0] > 0)
	if(partopt[0] > 0)
	{
		//Compute overland chemical partitioning
		OverlandChemicalPartitioning();

	}	//end if partopt[0] > 0

	//if any chemical biodegrades (if bioopt[0] > 0)
	if(bioopt[0] > 0)
	{
		//Compute overland chemical biodegradation
		OverlandChemicalBiodegradation();

	}	//end if bioopt[0] > 0

	//if any chemical dissolves (if dslopt[0] > 0)
	if(dslopt[0] > 0)
	{
		//Compute dissolution: particle-chemical interaction...
		//
		//Note:  Dissolution represents transformation of a pure solid
		//       phase chemical (modeled as a solids state variable) to
		//       a dissolved phase.  For this reaction, the solids state
		//       variable defined by cyieldfrom[] is transformed to the
		//       chemical state variable defined by cyieldto[].
		//
		//Compute overland chemical dissolution
		OverlandChemicalDissolution();

	}	//end if dslopt[0] > 0

	//if any chemical hydrolyzes (if hydopt[0] > 0)
	if(hydopt[0] > 0)
	{
		//Compute overland chemical hydrolysis
		OverlandChemicalHydrolysis();

	}	//end if hydopt[0] > 0

	//if any chemical oxidizes (if oxiopt[0] > 0)
	if(oxiopt[0] > 0)
	{
		//Compute overland chemical oxidation
		OverlandChemicalOxidation();

	}	//end if oxiopt[0] > 0

	//if any chemical photodegrades (if phtopt[0] > 0)
	if(phtopt[0] > 0)
	{
		//Compute overland chemical photolysis
		OverlandChemicalPhotolysis();

	}	//end if phtopt[0] > 0

	//if any chemical radioactively decays (if radopt[0] > 0)
	if(radopt[0] > 0)
	{
		//Compute overland chemical radioactive decay
		OverlandChemicalRadioactive();

	}	//end if radopt[0] > 0

	//if any chemical volatilizes (if vltopt[0] > 0)
	if(vltopt[0] > 0)
	{
		//Compute overland chemical volatilization
		OverlandChemicalVolatilization();

	}	//end if vltopt[0] > 0

	//if any chemical has a user-defined reaction (if udropt[0] > 0)
	if(udropt[0] > 0)
	{
		//Compute overland chemical user-defined reaction
		OverlandChemicalUDReaction();

	}	//end if udropt[0] > 0

	//if the number of chemical yields > 0
	if(ncyields > 0)
	{
		//Compute overland chemical yields (transformation between chemical types)
		OverlandChemicalYield();

	}	//end if ncyields > 0

//End of function: Return to ChemicalTransport
}
