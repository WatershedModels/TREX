/*---------------------------------------------------------------------
C-  Function:	SimulationError.c
C-
C-	Purpose/	SimulationError is called when errors occur
C-  Methods:    (and are trapped) during a simulation.  The error is 
C-              reported to the user. Reported error codes are:
C-
C-              simerrorno = 1: Negative water depth in overland cell
C-
C-              simerrorno = 2: Negative SWE depth in overland cell
C-
C-              simerrorno = 3: Negative water depth in channel cell
C-
C-              simerrorno = 4: Negative solids concentration in overland cell
C-
C-              simerrorno = 5: Negative solids concentration in channel cell
C-
C-              simerrorno = 6: Negative chemical concentration in overland cell
C-
C-              simerrorno = 7: Negative chemical concentration in channel cell
C-
C-              simerrorno = 8: Negative bank height in channel cell (Pop Soil Stack)
C-
C-              simerrorno = 9: Negative bank height in channel cell (Push Sediment Stack)
C-
C-              simerrorno = 10: Full soil stack
C-
C-              simerrorno = 11: Full sediment stack
C-
C-
C-  Inputs:     ierr, jerr, itype
C-
C-  Outputs:    None
C-
C-  Controls:   simerrorno
C-
C-  Calls:      FreeMemory
C-
C-  Called by:  OverlandWaterDepth, ChannelWaterDepth,
C-              OverlandSolidsConcentration, ChannelSolidsConcentration,
C-              OverlandChemicalConcentration, ChannelchemicalConcentration
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
C-				Denver, CO 80225
C-
C-	Date:		27-OCT-2003
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		30-JAN-2011
C-
C-  Revisions:	Revised error outputs for snowmelt
C-
C-  Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C----------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void SimulationError(int simerrorno, int ierr, int jerr, int itype)
{
	//Open the simulation error file in append mode (append to existing file)
	errorfile_fp = fopen(errorfile, "a");

	//Write simulation error file header line
	fprintf(errorfile_fp, "\n\n\nSimulation Errors Encountered!!!\n\n");

	//Echo message to screen
	printf("\n\n\n     Simulation Errors Encountered!!!\n\n");

	//Echo simulation abort message to screen
	printf("     TREX simulation aborted...\n\n");

	//Echo simulation error file location to screen
	printf("     Please read the Simulation Error File for error message details...\n\n");

	//Write current simulation time
	fprintf(errorfile_fp,"Current Simulation time = %f hours.\n", simtime);

	//Write current value of time step
	fprintf(errorfile_fp,"Current time step dt = %f seconds\n", dt[idt]);

	//negative water depth in overland cell
	if(simerrorno == 1)
	{
		//Write general simulation running time error message
		fprintf(errorfile_fp,
			"TREX stopped running during time loop; input file is: %s \n\n", inputfile);

		//Write error type
		fprintf(errorfile_fp,
			"A negative water depth has occurred in an overland cell\n");
	
		//Write location of negative water depth in overland cell
		fprintf(errorfile_fp,"Overland cell row: %d  column: %d  imask = %d\n",
			ierr, jerr, imask[ierr][jerr]);

		//Write current value of water depth in overland cell (row/col)
		fprintf(errorfile_fp,
			"Ending water depth in this cell is: %e m.\n", hovnew[ierr][jerr]);

		//Write prior value of water depth in overland cell (row/col)
		fprintf(errorfile_fp,
			"Starting water depth in this cell is: %e m.\n", hov[ierr][jerr]);

		//If the overland cell is a channel cell
		if(imask[ierr][jerr] > 1)
		  {
		    //Inform user that this is a channel cell
		    fprintf(errorfile_fp,"This Overland Cell is a Channel cell\n");

		    //Write location in terms of link and node
		    fprintf(errorfile_fp,"Overland/Channel Cell link: %d  node: %d  imask = %d\n",
			   link[ierr][jerr], node[ierr][jerr], imask[ierr][jerr]);

		    //Write current value of water depth in channel (link/node)
		    fprintf(errorfile_fp,
			    "Ending water depth in this link/node is: %e m.\n", hchnew[link[ierr][jerr]][node[ierr][jerr]]);

		    //Write prior value of water depth in channel (link/node)
		    fprintf(errorfile_fp,
			    "Starting water depth in this link/node is: %e m.\n", hch[link[ierr][jerr]][node[ierr][jerr]]);

		    //Write channel bank height (link/node)
		    fprintf(errorfile_fp,
			    "Channel bank height in this link/node is: %e m.\n", hbank[link[ierr][jerr]][node[ierr][jerr]]);

		  }  //end if overland cell is a channel cell

		//Write simulation abort message
		fprintf(errorfile_fp,"TREX simulation aborted...\n\n");

		//Free all allocated memory during program execution
//mlv		FreeMemory();
	}
	//negative swe depth in overland cell
	if(simerrorno == 2)
	{
		//Write general simulation running time error message
		fprintf(errorfile_fp,
			"TREX stopped running during time loop; input file is: %s \n\n", inputfile);

		//Write error type
		fprintf(errorfile_fp,
			"A negative SWE depth has occurred in an overland cell\n");
	
		//Write location of negative water depth in overland cell
		fprintf(errorfile_fp,"Overland cell row: %d  column: %d  imask = %d\n",
			ierr, jerr, imask[ierr][jerr]);

		//Write current value of swe depth in overland cell (row/col)
		fprintf(errorfile_fp,
			"Ending water depth in this cell is: %e m.\n", sweovnew[ierr][jerr]);

		//Write prior value of swe depth in overland cell (row/col)
		fprintf(errorfile_fp,
			"Starting water depth in this cell is: %e m.\n", sweov[ierr][jerr]);

		//If the overland cell is a channel cell
		if(imask[ierr][jerr] > 1)
		  {
		    //Inform user that this is a channel cell
		    fprintf(errorfile_fp,"This Overland Cell is a Channel cell\n");

		    //Write location in terms of link and node
		    fprintf(errorfile_fp,"Overland/Channel Cell link: %d  node: %d  imask = %d\n",
			   link[ierr][jerr], node[ierr][jerr], imask[ierr][jerr]);

		  }  //end if overland cell is a channel cell

		//Write simulation abort message
		fprintf(errorfile_fp,"TREX simulation aborted...\n\n");

		//Free all allocated memory during program execution
//mlv		FreeMemory();
	}
	//negative water depth in channel
	else if(simerrorno == 3)
	{
		//Write general simulation running time error message
		fprintf(errorfile_fp,
			"TREX stopped running during time loop; input file is: %s \n\n", inputfile);

		//Write error type
		fprintf(errorfile_fp,
			"A negative water depth has occurred in a channel link/node\n");

		//Write location of negative water depth in channel (link/node)
		fprintf(errorfile_fp,"Channel link: %d  node: %d\n", ierr, jerr);

		//Write location of negative depth in channel (row/col)
		fprintf(errorfile_fp,"Channel row: %d  column: %d\n",
			ichnrow[ierr][jerr], ichncol[ierr][jerr]);

		//if the channel is trapezoidal or triangular (not rectangular)
		if(sideslope[ierr][jerr] > 0)
		{
			//Write current value of discriminant (link/node)
			fprintf(errorfile_fp,
				"Channel is trapezoidal or triangular and discriminant is less than zero.\n");
			fprintf(errorfile_fp,
				"  New water depth in channel cannot be determined (square root of negative number)");
		}
		else	//else the channel is rectangular (sideslope = 0, it cannot be negative)
		{
			//Write current value of depth in channel (link/node)
			fprintf(errorfile_fp,
				"Ending water depth in this link/node is: %e m.\n", hchnew[ierr][jerr]);

		}	//end if the channel is trapezoidal or triangular (sideslope[][] > 0)...

		//Write prior value of water depth in channel (link/node)
		fprintf(errorfile_fp,
			"Starting water depth in this link/node is: %e m.\n", hch[ierr][jerr]);

		//Write channel bank height (link/node)
		fprintf(errorfile_fp,
			"Channel bank height in this link/node is: %e m.\n", hbank[ierr][jerr]);

		//Write current value of water depth in overland cell (row/col)
		fprintf(errorfile_fp,
			"Ending overland water depth in this cell is: %e m.\n",
			hovnew[ichnrow[ierr][jerr]][ichncol[ierr][jerr]]);

		//Write prior value of water depth in overland cell (row/col)
		fprintf(errorfile_fp,
			"Starting overland water depth in this cell is: %e m.\n",
			hov[ichnrow[ierr][jerr]][ichncol[ierr][jerr]]);

		//Write simulation abort message
		fprintf(errorfile_fp,"TREX simulation aborted...\n\n");

		//Free all allocated memory during program execution
//mlv		FreeMemory();
	}
	//negative solids concentration overland
	else if(simerrorno == 4)
	{
		//Write general simulation running time error message
		fprintf(errorfile_fp,
			"Possible numerical instability; input file is: %s \n\n", inputfile);

		//Write error type
		fprintf(errorfile_fp,
			"A negative solids concentration has occurred in an overland cell\n");

		//Write location of negative solids concentration overland (row/col)
		fprintf(errorfile_fp,"Overland row: %d  col: %d  isolid: %d  imask: %d\n",
			ierr, jerr, itype, imask[ierr][jerr]);

		//Write current value of negative solids concentration overland (row/col)
		fprintf(errorfile_fp,
			"Ending solids concentration in this row/col is: %e g/m3.\n",
			csedovnew[itype][ierr][jerr][0]);

		//Write prior value of negative solids concentration overland (row/col)
		fprintf(errorfile_fp,
			"Starting solids concentration in this row/col is: %e g/m3.\n",
			csedov[itype][ierr][jerr][0]);
	}
	//negative solids concentration in channel
	else if(simerrorno == 5)
	{
		//Write general simulation running time error message
		fprintf(errorfile_fp,
			"Possible numerical instability; input file is: %s \n\n", inputfile);

		//Write error type
		fprintf(errorfile_fp,
			"A negative solids concentration has occurred in a channel link/node\n");

		//Write location of negative concentration in channel (link/node)
		fprintf(errorfile_fp,"Channel link: %d node: %d isolid: %d\n",
			ierr, jerr, itype);

		//Write location of negative concentration in channel (row/col)
		fprintf(errorfile_fp,"Channel row: %d  column: %d\n",
			ichnrow[ierr][jerr], ichncol[ierr][jerr]);

		//Write current value of concentration in channel (link/node)
		fprintf(errorfile_fp,
			"Ending solids concentration in this link/node is: %e g/m3.\n",
			csedchnew[itype][ierr][jerr][0]);

		//Write prior value of concentration in channel (link/node)
		fprintf(errorfile_fp,
			"Starting solids concentration in this link/node is: %e g/m3.\n",
			csedch[itype][ierr][jerr][0]);
	}
	//negative chemical concentration overland
	else if(simerrorno == 6)
	{
		//Write general simulation running time error message
		fprintf(errorfile_fp,
			"Possible numerical instability; input file is: %s \n\n", inputfile);

		//Write error type
		fprintf(errorfile_fp,
			"A negative chemical concentration has occurred in an overland cell\n");

		//Write location of negative chemical concentration overland (row/col)
		fprintf(errorfile_fp,"Overland row: %d col: %d  ichem: %d\n",
			ierr, jerr, itype);

		//Write current value of chemical concentration overland (row/col)
		fprintf(errorfile_fp,
			"Ending solids concentration in this row/col is: %e g/m3.\n",
			cchemovnew[itype][ierr][jerr][0]);

		//Write prior value of chemical concentration overland (row/col)
		fprintf(errorfile_fp,
			"Starting solids concentration in this row/col is: %e g/m3.\n",
			cchemov[itype][ierr][jerr][0]);
	}
	//negative chemical concentration in channel
	else if(simerrorno == 7)
	{
		//Write general simulation running time error message
		fprintf(errorfile_fp,
			"Possible numerical instability; input file is: %s \n\n", inputfile);

		//Write error type
		fprintf(errorfile_fp,
			"A negative chemical concentration has occurred in a channel link/node\n");

		//Write location of negative concentration in channel (link/node)
		fprintf(errorfile_fp,"Channel link: %d node: %d ichem: %d\n",
			ierr, jerr, itype);

		//Write location of negative concentration in channel (row/col)
		fprintf(errorfile_fp,"Channel row: %d  column: %d\n",
			ichnrow[ierr][jerr], ichncol[ierr][jerr]);

		//Write current value of concentration in channel (link/node)
		fprintf(errorfile_fp,
			"Ending chemical concentration in this link/node is: %e g/m3.\n",
			cchemchnew[itype][ierr][jerr][0]);

		//Write prior value of concentration in channel (link/node)
		fprintf(errorfile_fp,
			"Starting chemical concentration in this link/node is: %e g/m3.\n",
			cchemch[itype][ierr][jerr][0]);
	}
	//overland erosion (PopSoilStack) reduced channel bank height <= zero
	else if(simerrorno == 8)
	{
		//Write general simulation running time error message
		fprintf(errorfile_fp,
			"Channel bank height error; input file is: %s \n\n", inputfile);

		//Write error type
		fprintf(errorfile_fp,
			"Channel bank height <= zero due to soil stack erosion\n");

		//Write location of eroding overland cell (row/col)
		fprintf(errorfile_fp,"Overland row: %d col: %d  nstackov: %d\n",
			ierr, jerr, itype);

		//Write new value of bank height (row/col)
		fprintf(errorfile_fp,
			"Ending bank height in this row/col is: %e m.\n",
			hbank[link[ierr][jerr]][node[ierr][jerr]]);

		//Write link/node location of cell
		fprintf(errorfile_fp,
			"Channel link: %d node: %d\n",
			link[ierr][jerr], node[ierr][jerr]);
	}
	//channel deposition (PushSedimentStack) reduced channel bank height <= zero
	else if(simerrorno == 9)
	{
		//Write general simulation running time error message
		fprintf(errorfile_fp,
			"Channel bank height error; input file is: %s \n\n", inputfile);

		//Write error type
		fprintf(errorfile_fp,
			"Channel bank height <= zero due to sediment stack deposition\n");

		//Write location of depositing channel element (link/node)
		fprintf(errorfile_fp,"Channel link: %d node: %d  nstackch: %d\n",
			ierr, jerr, itype);

		//Write new value of bank height (link/node)
		fprintf(errorfile_fp,
			"Ending bank height in this link/node is: %e m.\n",
			hbank[ierr][jerr]);
	}
	//Soil stack full error
	else if(simerrorno == 10)
	{
		//Write message to file: alert user of full soil stack
		fprintf(errorfile_fp, "\n\n\nSoil Stack Full:\n\n");
		fprintf(errorfile_fp,     "  Exceeded maximum number of layers. Set\n");
		fprintf(errorfile_fp,     "  stkovopt = 1 or increase maxstackov...\n\n");
		fprintf(errorfile_fp,     "  row = %d  col = %d  time = %9.4f\n\n",
			i, j, simtime);
		fprintf(errorfile_fp,     "  stkovopt = %d  maxstackov = %d\n",
			stkovopt, maxstackov);
	}
	//sediment stack full error
	else if(simerrorno == 11)
	{
		//Write message to file: alert user of full sediment stack
		fprintf(errorfile_fp, "\n\n\nSediment Stack Full: nstackch = %d\n\n",
			itype);
		fprintf(errorfile_fp,     "  Exceeded maximum number of layers. Set\n");
		fprintf(errorfile_fp,     "  stkchopt = 1 or increase maxstackch...\n\n");
		fprintf(errorfile_fp,     "  link = %d  node = %d  time = %9.4f\n\n",
			ierr, jerr, simtime);
		fprintf(errorfile_fp,     "  stkchopt = %d  maxstackch = %d\n",
			stkchopt, maxstackch);
	}
	//additional error codes to be added here
	else
	{

	}

	//Close the simulation error file
	fclose(errorfile_fp);

//End of function: Return to trex
}
