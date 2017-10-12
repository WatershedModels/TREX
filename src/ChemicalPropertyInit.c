/*----------------------------------------------------------------------
C-  Function:	ChemicalPropertyInit.c
C-
C-	Purpose/	ChemicalPropertyInit is called from ReadDataGroupD at
C-	Methods:	the start of the simulation to allocate memory and set
C-				default values for chemical properties.
C-
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:   chnopt
C-
C-	Calls:		None
C-
C-	Called by:	ReadDataGroupD
C-
C-	Created:	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		13-SEP-2004
C-
C-	Revisions:
C-
C-	Date:
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

void ChemicalPropertyInit()
{
	/*************************************************************/
	/*        Initialize Chemical Process Option Variables       */
	/*************************************************************/

	//Allocate memory for chemical process option arrays
	partopt = (int *)malloc((nchems+1) * sizeof(int));			//chemical partitioning option
	bioopt = (int *)malloc((nchems+1) * sizeof(int));			//chemical biodegradation option
	dslopt = (int *)malloc((nchems+1) * sizeof(int));			//chemical dissolution option
	hydopt = (int *)malloc((nchems+1) * sizeof(int));			//chemical hydrolysis option
	oxiopt = (int *)malloc((nchems+1) * sizeof(int));			//chemical oxidation option
	phtopt = (int *)malloc((nchems+1) * sizeof(int));			//chemical photolysis option
	radopt = (int *)malloc((nchems+1) * sizeof(int));			//chemical radioactive decay option
	vltopt = (int *)malloc((nchems+1) * sizeof(int));			//chemical volatilization option
	udropt = (int *)malloc((nchems+1) * sizeof(int));			//chemical extra (user-defined) reaction option

	//Set default values...
	//
	//Loop over number of chemicals (plus zeroeth element for process control)
	for(i=0; i<=nchems; i++)
	{
		//set default values to zero (0 = process not simulated)
		partopt[i] = 0;			//chemical partitioning option
		bioopt[i] = 0;			//chemical biodegradation option
		dslopt[i] = 0;			//chemical dissolution option
		hydopt[i] = 0;			//chemical hydrolysis option
		oxiopt[i] = 0;			//chemical oxidation option
		radopt[i] = 0;			//chemical radioactive decay option
		phtopt[i] = 0;			//chemical photolysis option
		vltopt[i] = 0;			//chemical volatilization option
		udropt[i] = 0;			//chemical extra (user-defined) reaction option

	}	//end loop over chemicals

	/***********************************************************/
	/*        Initialize Chemical Partitioning Variables       */
	/***********************************************************/

	//Allocate memory for chemical partitioning arrays
	kp = (float *)malloc((nchems+1) * sizeof(float));	//chemical partition coefficient (m3/g)
	kb = (float *)malloc((nchems+1) * sizeof(float));	//chemical DOC binding coefficient (m3/g)
	koc = (float *)malloc((nchems+1) * sizeof(float));	//chemical organic carbon coefficient (m3/g)
	nux = (float *)malloc((nchems+1) * sizeof(float));	//particle interaction parameter for partitioning (dimensionless)

	//Set default values...
	//
	//Loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//set default values
		kp[i] = 0.0;				//chemical partition coefficient (m3/g)
		kb[i] = 0.0;				//chemical DOC binding coefficient (m3/g)
		koc[i] = 0.0;				//chemical organic carbon coefficient (m3/g)
		nux[i] = (float)(1.0e20);	//particle interaction parameter for partitioning (dimensionless)

	}	//end loop over chemicals


	/*************************************************************/
	/*        Initialize Chemical Biodegradation Variables       */
	/*************************************************************/

	//Allocate memory for chemical biodegradation arrays
	kbiowov = (float *)malloc((nchems+1) * sizeof(float));	//chemical biodegradation rate for overland water (1/sec)
	kbiosov = (float *)malloc((nchems+1) * sizeof(float));	//chemical biodegradation rate for overland soil (1/sec)
	kbiowch = (float *)malloc((nchems+1) * sizeof(float));	//chemical biodegradation rate for channel water (1/sec)
	kbiosch = (float *)malloc((nchems+1) * sizeof(float));	//chemical biodegradation rate for channel sediment (1/sec)

	//Loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		kbiowov[i] = 0.0;	//chemical biodegradation rate for overland water (1/sec)
		kbiosov[i] = 0.0;	//chemical biodegradation rate for overland soil (1/sec)
		kbiowch[i] = 0.0;	//chemical biodegradation rate for channel water (1/sec)
		kbiosch[i] = 0.0;	//chemical biodegradation rate for channel sediment (1/sec)

	}	//end loop over chemicals

	/*********************************************************/
	/*        Initialize Chemical Hydrolysis Variables       */
	/*********************************************************/



	/********************************************************/
	/*        Initialize Chemical Oxidation Variables       */
	/********************************************************/



	/*********************************************************/
	/*        Initialize Chemical Photolysis Variables       */
	/*********************************************************/




	/*************************************************************/
	/*        Initialize Chemical Volatilization Variables       */
	/*************************************************************/



	/********************************************************************/
	/*        Initialize Chemical User-Defined Reaction Variables       */
	/********************************************************************/




//End of function: Return to ReadDataGroupD
}
