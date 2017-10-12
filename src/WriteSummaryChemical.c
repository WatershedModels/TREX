/*---------------------------------------------------------------------
C-  Function:	WriteSummaryChemical.c
C-
C-	Purpose/	Write summary chemical balance and transport
C-	Methods:	information	at the end of a successful model run.
C-
C-	Inputs:
C-
C-	Outputs:
C-
C-	Controls:	
C-
C-	Calls:		None
C-
C-	Called by:	WriteSummary
C-
C-  Created:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-	Date:		12-SEP-2004
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		31-JUL-2009
C-
C-  Revisions:	Added code so that errors caused by bad file names
C-              and paths are are reported to the simulation error
C-              file in addition to the screen.
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C--------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void WriteSummaryChemical()
{
	//local variable declarations
	int
		ichem;			//loop index for chemical type

	double
		*initialmass,	//initial mass in domain (m3) [chem]
		*finalmass,		//final mass in domain (m3) [chem]
		*totalmassin,	//total mass entering domain (m3) [chem]
		*totalmassout,	//total mass exiting domain (m3) [chem]
		*sourcemass,	//sum of all sources (m3) [chem]
		*sinkmass,		//sum of all sinks (m3) [chem]
		*mberror;		//mass balance error (%) [chem]

	//set the summary statistics file pointer to null
	statsfile_fp = NULL;

	//open the summary statistics file (in append mode)
	statsfile_fp = fopen(statsfile, "a");

	//if file pointer is still null, then abort
	if(statsfile_fp == NULL)
	{
		//write error message to file
		fprintf(echofile_fp, "Error! Can't open Summary Statistics file %s\n", statsfile);

		//write error message to screen
		printf("Error! Can't open Summary Statistics file %s\n", statsfile);
		exit(EXIT_FAILURE);	//exit with error
    }

	//Alocate memory for local arrays
	initialmass = (double *)malloc((nchems+1) * sizeof(double));	//initial mass in the water column (kg) [chem]
	finalmass = (double *)malloc((nchems+1) * sizeof(double));		//final mass in the water column (kg) [chem]
	totalmassin = (double *)malloc((nchems+1) * sizeof(double));	//total mass entering domain (m3) [chem]
	totalmassout = (double *)malloc((nchems+1) * sizeof(double));	//total mass exiting domain (m3) [chem]
	sourcemass = (double *)malloc((nchems+1) * sizeof(double));		//sum of all sources (m3) [chem]
	sinkmass = (double *)malloc((nchems+1) * sizeof(double));		//sum of all sinks (m3) [chem]
	mberror = (double *)malloc((nchems+1) * sizeof(double));		//mass balance error (%) [chem]

	//Initialize local arrays
	//
	//loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//set initial values to zero
		initialmass[ichem] = 0.0;	//initial mass in the water column (kg) [chem]
		finalmass[ichem] = 0.0;	//final mass in the water column (kg) [chem]
		totalmassin[ichem] = 0.0;	//total mass entering domain (m3) [chem]
		totalmassout[ichem] = 0.0;	//total mass exiting domain (m3) [chem]
		sourcemass[ichem] = 0.0;	//sum of all sources (m3) [chem]
		sinkmass[ichem] = 0.0;		//sum of all sinks (m3) [chem]
		mberror[ichem] = 0.0;		//mass balance error (%) [chem]

	}	//end loop over chemicals

	//loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Compute total and individual chemicals masses in domain at
		//simulation start and end...
		//
		//compute initial overland water column chemicals mass at start (kg)
		initialmass[ichem] = initialchemmassov[ichem][0];

		//Compute final overland water column chemicals mass at end (kg)
		finalmass[ichem] = finalchemmassov[ichem][0];

		//if channels are simulated
		if(chnopt > 0)
		{
			//add initial channel water column chemicals mass at start (kg)
			initialmass[ichem] = initialmass[ichem]
				+ initialchemmassch[ichem][0];

			//add final channel water column chemicals mass at end (kg)
			finalmass[ichem] = finalmass[ichem]
				+ finalchemmassch[ichem][0];

		}	//end if chnopt > 0

		//Compute cumulative individual and total chemicals mass entering domain
		//during the simulation...
		//
		//Compute cumulative chemicals mass entering domain overland (water column) (kg)
		totalmassin[ichem] = totalcwov[ichem]
			+ totaladvcheminov[ichem][0] + totaldspcheminov[ichem][0];

		//Add cumulative mass entering the overland water column by erosion (kg)
		totalmassin[ichem] = totalmassin[ichem]
			+ totalerscheminov[ichem];

		//Add cumulative mass entering the overland water column by porewater release (kg)
		totalmassin[ichem] = totalmassin[ichem]
			+ totalpwrcheminov[ichem];

		//Compute cumulative individual and total chemicals mass exiting domain
		//during the simulation...
		//
		//Compute cumulative mass leaving the overland plane (kg)
		totalmassout[ichem] = totaladvchemoutov[ichem][0]
			+ totaldspchemoutov[ichem][0];

		//Add cumulative mass leaving the overland water column by deposition (kg)
		totalmassout[ichem] = totalmassout[ichem]
			+ totaldepchemoutov[ichem];

		//Add cumulative mass leaving the overland water column by infiltration (kg)
		totalmassout[ichem] = totalmassout[ichem]
			+ totalinfchemoutov[ichem][0];

		//if channels are simulated
		if(chnopt > 0)
		{
			//Compute cumulative individual and total chemicals mass entering domain
			//during the simulation...
			//
			//Compute cumulative chemicals mass entering domain in channels (water column) (kg)
			totalmassin[ichem] = totalmassin[ichem]
				+ totalcwch[ichem] + totaladvcheminch[ichem][0]
				+ totaldspcheminch[ichem][0];

			//Add cumulative mass entering the channel water column by erosion (kg)
			totalmassin[ichem] = totalmassin[ichem]
				+ totalerscheminch[ichem];

			//Add cumulative mass entering the channel water column by porewater release (kg)
			totalmassin[ichem] = totalmassin[ichem]
				+ totalpwrcheminch[ichem];

			//Compute cumulative individual and total chemicals mass exiting domain
			//during the simulation...
			//
			//Compute cumulative mass leaving the channel network (kg)
			totalmassout[ichem] = totalmassout[ichem]
				+ totaladvchemoutch[ichem][0] + totaldspchemoutch[ichem][0];

			//Add cumulative mass leaving the channel water column by deposition (kg)
			totalmassout[ichem] = totalmassout[ichem]
				+ totaldepchemoutch[ichem];

			//Add cumulative mass leaving the channel water column by infiltration (kg)
			totalmassout[ichem] = totalmassout[ichem]
				+ totalinfchemoutch[ichem][0];

		}	//end if chnopt > 0

	}	//end loop over chemicals

	//loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Compute sum of water column chemicals source masses (kg)
		sourcemass[ichem] = initialmass[ichem]
			+ totalmassin[ichem];

		//Compute sum of water column chemicals sink masses (kg)
		sinkmass[ichem] = finalmass[ichem]
			+ totalmassout[ichem];

		//if sourcemass[] > zero
		if(sourcemass[ichem] > 0.0)
		{
			//Compute chemicals mass balance error (percent)
			mberror[ichem] = (sourcemass[ichem] - sinkmass[ichem])
				/ sourcemass[ichem] * 100.0;
		}
		else	//else sourcemass[] = 0 (it can't be < 0)
		{
			//set the chemical mass balance error to zero
			mberror[ichem] = 0.0;

		}	//end if sourcemass[] > 0.0

		//Summary of Chemical Transport Output
		fprintf(statsfile_fp, "\n\nSUMMARY OF CHEMICAL TRANSPORT OUTPUT\n");
		fprintf(statsfile_fp,     "====================================\n\n");

		//Label for chemical
		fprintf(statsfile_fp, "Chemical Type %d:  %s\n\n",
			ichem, chemname[ichem]);

		fprintf(statsfile_fp, "Summary for Water Column (Layer 0)\n");
		fprintf(statsfile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

		//Loop over outlets
		for(i=1; i<=noutlets; i++)
		{
			fprintf(statsfile_fp,
				"Outlet: %d (Row %d, Col %d)\n", i, iout[i], jout[i]);

			//if channels are not simulated or the outlet option is 1
			if(chnopt == 0 || outopt == 1)
			{
				fprintf(statsfile_fp,
					"  Peak Overland Chemical Discharge (kg/s)....................= ");
				fprintf(statsfile_fp,"%15.2e\n", chemflowpeakov[ichem][i]);

				fprintf(statsfile_fp,
					"  Time to Peak Chemical Discharge (hours)....................= ");
				fprintf(statsfile_fp,"%15.2e\n", chemtimepeakov[ichem][i]);

			}	//end if chnopt = 0 or outopt = 1

			//if channels are simulated
			if(chnopt > 0)
			{
				fprintf(statsfile_fp,
					"  Peak Channel Chemical Discharge (kg/s).....................= ");
				fprintf(statsfile_fp,"%15.2e\n", chemflowpeakch[ichem][i]);

				fprintf(statsfile_fp,
					"  Time to Peak Chemical Discharge (hours)....................= ");
				fprintf(statsfile_fp,"%15.2e\n", chemtimepeakch[ichem][i]);

			}	//end of chnopt > 0

			fprintf(statsfile_fp, "\n");

		}	//end loop over outlets

		fprintf(statsfile_fp,
			"Initial Chemical Mass Within Water Column (kg)...............= ");
		fprintf(statsfile_fp,"%15.2e\n\n", initialmass[ichem]);

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Entering By Advection (kg)..........= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaladvcheminov[ichem][0]+totaladvcheminch[ichem][0]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaladvcheminov[ichem][0]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"  Overland Advection (kg)...........= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaladvcheminov[ichem][0]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Advection (kg)............= ");

			fprintf(statsfile_fp,"%15.2e\n\n",
				totaladvcheminch[ichem][0]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Entering By Dispersion (kg).........= ");

		//if channels are simulated
		if(chnopt > 1)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldspcheminov[ichem][0]+totaldspcheminch[ichem][0]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldspcheminov[ichem][0]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"  Overland Dispersion (kg)..........= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaldspcheminov[ichem][0]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Dispersion (kg)...........= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaldspcheminch[ichem][0]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Entering By Erosion (kg)............= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalerscheminov[ichem]+totalerscheminch[ichem]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalerscheminov[ichem]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"  Overland Erosion (kg).............= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totalerscheminov[ichem]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Erosion (kg)..............= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totalerscheminch[ichem]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Entering By Porewater Release (kg)..= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalpwrcheminov[ichem]+totalpwrcheminch[ichem]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalpwrcheminov[ichem]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"  Overland Porewater Release (kg)...= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totalpwrcheminov[ichem]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Porewater Release (kg)....= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totalpwrcheminch[ichem]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Entering By Transformation (kg).....= ");

		//if channels are simulated
		if(chnopt > 1)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaltnscheminov[ichem]+totaltnscheminch[ichem]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
			totaltnscheminov[ichem]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"  Overland Transformations (kg).....= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaltnscheminov[ichem]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Transformations (kg)......= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaltnscheminch[ichem]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"Chemical Mass Entering Domain from External Sources (kg).....= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",totalcwov[ichem]+totalcwch[ichem]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",totalcwov[ichem]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Sources (kg).............= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totalcwov[ichem]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Sources (kg)..............= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totalcwch[ichem]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Leaving By Advection (kg)...........= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaladvchemoutov[ichem][0]+totaladvchemoutch[ichem][0]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaladvchemoutov[ichem][0]);
			
		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Advection (kg)...........= ");

		fprintf(statsfile_fp,"%15.2e\n",
			totaladvchemoutov[ichem][0]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Advection (kg)............= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaladvchemoutch[ichem][0]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Leaving By Dispersion (kg)..........= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldspchemoutov[ichem][0]+totaldspchemoutch[ichem][0]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldspchemoutov[ichem][0]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Dispersion (kg)..........= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaldspchemoutov[ichem][0]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Dispersion (kg)...........= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaldspchemoutch[ichem][0]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Leaving By Deposition (kg)..........= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldepchemoutov[ichem]+totaldepchemoutch[ichem]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldepchemoutov[ichem]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Deposition (kg)..........= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaldepchemoutov[ichem]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Deposition (kg)...........= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaldepchemoutch[ichem]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Leaving By Infiltration (kg)........= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalinfchemoutov[ichem][0]+totalinfchemoutch[ichem][0]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalinfchemoutov[ichem][0]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Infiltration (kg)........= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totalinfchemoutov[ichem][0]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Infiltration (kg).........= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totalinfchemoutch[ichem][0]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Leached From Upper Bed Layer (kg)...= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalinfchemoutov[ichem][1]+totalinfchemoutch[ichem][1]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalinfchemoutov[ichem][1]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Leaching (kg)............= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totalinfchemoutov[ichem][1]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Leaching (kg).............= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totalinfchemoutch[ichem][1]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Chemical Mass Leaving By Transformation (kg)......= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaltnschemoutov[ichem]+totaltnschemoutch[ichem]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaltnschemoutov[ichem]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Transformations (kg).....= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaltnschemoutov[ichem]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Transformations (kg)......= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaltnschemoutch[ichem]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"Cumulative Mass Entering Domain, M_in (kg)...................= ");
		fprintf(statsfile_fp,"%15.2e\n",totalmassin[ichem]);

		fprintf(statsfile_fp,
			"Cumulative Mass Leaving the Water Column, M_out (kg).........= ");
		fprintf(statsfile_fp,"%15.2e\n",totalmassout[ichem]);

		//if totalmassin[] > zero
		if(totalmassin[ichem] > 0.0)
		{
			fprintf(statsfile_fp,
				"Percentage of M_out to M_in (%%)..............................= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				(totalmassout[ichem]/totalmassin[ichem])*100.0);
		}
		else	//else totalmassin[] = 0.0 (it can't be < 0)
		{
			fprintf(statsfile_fp,
				"Percentage of M_out to M_in (%%)............................= ");
			fprintf(statsfile_fp,"%s\n\n",
				"Not Calculated");

		}	//end if totalmassin[] > 0.0

		fprintf(statsfile_fp,
			"Final Chemical Mass Within Water Column, M_final (kg)........= ");
		fprintf(statsfile_fp,"%15.2e\n",finalmass[ichem]);

		//if totalmassin[] > zero
		if(totalmassin[ichem] > 0.0)
		{
			fprintf(statsfile_fp,
				"Percentage of M_final to M_in (%%)............................= ");
			fprintf(statsfile_fp,"%15.2f\n\n",
				(finalmass[ichem]/totalmassin[ichem])*100.0);
		}
		else	//else totalmassin[] = 0.0 (it can't be < 0)
		{
			fprintf(statsfile_fp,
				"Percentage of M_final to M_in (%%)..........................= ");
			fprintf(statsfile_fp,"%s\n\n",
				"Not Calculated");

		}	//end if totalmassin[] > 0.0

		fprintf(statsfile_fp,
			"Percent Mass Balance Error (%%)...............................= ");
		fprintf(statsfile_fp,"%15.2f\n\n",mberror[ichem]);

		//print chemicals concentration min/max summary
		fprintf(statsfile_fp,
			"\nTOTAL CHEMICAL MINIMUM AND MAXIMUM VALUES\n");
		fprintf(statsfile_fp,
			  "=========================================\n\n");

		fprintf(statsfile_fp,
			"Min. Overland Total Water Column Chemical Concentration (g/m3).....= ");
		fprintf(statsfile_fp,"%15.2e\n",mincchemov0[ichem]);

		fprintf(statsfile_fp,
			"Max. Overland Total Water Column Chemical Concentration (g/m3).....= ");
		fprintf(statsfile_fp,"%15.2e\n\n",maxcchemov0[ichem]);

		fprintf(statsfile_fp,
			"Min. Overland Surface Soil Total Chemical Concentration (mg/kg)....= ");
		fprintf(statsfile_fp,"%15.2e\n",mincchemov1[ichem]);

		fprintf(statsfile_fp,
			"Max. Overland Surface Soil Total Chemical Concentration (mg/kg)....= ");
		fprintf(statsfile_fp,"%15.2e\n\n",maxcchemov1[ichem]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"Min. Channel Total Water Column Chemical Concentration (g/m3)......= ");
			fprintf(statsfile_fp,"%15.2e\n",mincchemch0[ichem]);

			fprintf(statsfile_fp,
				"Max. Channel Total Water Column Chemical Concentration (g/m3)......= ");
			fprintf(statsfile_fp,"%15.2e\n\n",maxcchemch0[ichem]);

			fprintf(statsfile_fp,
				"Min. Channel Surface Sediment Total Chemical Concentration (mg/kg).= ");
			fprintf(statsfile_fp,"%15.2e\n",mincchemch1[ichem]);

			fprintf(statsfile_fp,
				"Max. Channel Surface Sediment Total Chemical Concentration (mg/kg).= ");
			fprintf(statsfile_fp,"%15.2e\n\n",maxcchemch1[ichem]);
		}
		else	//else channels are not simulated
		{
			//print message that channel flow not simulated...
			fprintf(statsfile_fp, "\nChannel transport was not simulated\n\n");

		}	//end if chnopt = 1

		//add blank space for formatting...
		fprintf(statsfile_fp, "\n\n\n");

	}	//end loop over chemicals

	//close the summary statistics file
	fclose(statsfile_fp);

	//free memory for local arrays
	free(initialmass);
	free(finalmass);
	free(totalmassin);
	free(totalmassout);
	free(sourcemass);
	free(sinkmass);
	free(mberror);

//End of function: Return to WriteSummary
}
