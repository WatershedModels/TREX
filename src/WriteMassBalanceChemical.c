/*---------------------------------------------------------------------
C-  Function:	WriteMassBalanceChemical.c
C-
C-	Purpose/	Write detailed mass balance summary of chemical
C-	Methods:	transport on a cell and node basis at the end of
C-              a successful model run.
C-
C-
C-	Inputs:
C-
C-	Outputs:	mass balance text file
C-
C-	Controls:	chnopt
C-
C-	Calls:		None
C-
C-	Called by:	trex
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
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

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void WriteMassBalanceChemical()
{
	//local variable declarations/definitions
	int
		ichem;		//loop index for chemical type

	int
		chanlink,	//channel link number
		channode;	//channel node number

	int uplink,		//link number of upstream link
		upnode,		//node number of upstream node
		downlink,	//link number of downstream link
		downnode,	//node number of downstream node
		row,		//row value of ichnrow[][] of a node
		col;		//col value of ichncol[][] of a node

	float
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf;	//surface area of channel (m)

	//set the mass balance file pointer to null
	msbfile_fp = NULL;

	//open the mass balance file (in append mode)
	msbfile_fp = fopen(msbfile, "a");

	//if file pointer is still null, then abort
	if(msbfile_fp == NULL)
	{
		//write error message to file
		fprintf(echofile_fp, "Error! Can't open mass balance file %s\n", msbfile);

		//write error message to screen
		printf("Error! Can't open mass balance file %s\n", msbfile);
		exit(EXIT_FAILURE);	//exit with error
    }

	//loop over chemical
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Label for an individual chemical type
		fprintf(msbfile_fp, "\n\n\nMass Balance for Chemical %d:  %s\n\n",
			ichem, chemname[ichem]);

		//Write label for overland plane (Part 1)
		fprintf(msbfile_fp,"\nMass summary for the overland plane:\n\n");
		fprintf(msbfile_fp,"\nrow\tcol\timask\tlink\tnode\tinitial mass (kg)");

		//loop over advective flow sources
		for(k=0; k<=10; k++)
		{
			//Write label for overland plane (Part 2a: inflows)
			fprintf(msbfile_fp,"\tadvchemovinmass %d (kg)", k);

		}	//end loop over advective flow sources

		//loop over dispersive flow sources
		for(k=1; k<=10; k++)
		{
			//Write label for overland plane (Part 2b: dispersion inflows)
			fprintf(msbfile_fp,"\tdspchemovinmass %d (kg)", k);

		}	//end loop over dispersive flow sources

		//Write label for overland plane (Part 2c: erosion inflows)
		fprintf(msbfile_fp,"\terschemovinmass (kg)");

		//Write label for overland plane (Part 2d: deposition inflows)
		fprintf(msbfile_fp,"\tdepchemovinmass (kg)");

		//Write label for overland plane (Part 2e: infiltration inflows)
		fprintf(msbfile_fp,"\tinfchemovinmass (kg)");

		//Write label for overland plane (Part 2f: biodegradation inflows)
		fprintf(msbfile_fp,"\tbiochemovinmass (kg)");

		//Write label for overland plane (Part 2g: hydrolysis inflows)
		fprintf(msbfile_fp,"\thydchemovinmass (kg)");

		//Write label for overland plane (Part 2h: oxidation inflows)
		fprintf(msbfile_fp,"\toxichemovinmass (kg)");

		//Write label for overland plane (Part 2i: photolysis inflows)
		fprintf(msbfile_fp,"\tphtchemovinmass (kg)");

		//Write label for overland plane (Part 2j: radioactive decay inflows)
		fprintf(msbfile_fp,"\tradchemovinmass (kg)");

		//Write label for overland plane (Part 2k: volatilization inflows)
		fprintf(msbfile_fp,"\tvltchemovinmass (kg)");

		//Write label for overland plane (Part 2l: user-defined reaction inflows)
		fprintf(msbfile_fp,"\tudrchemovinmass (kg)");

		//loop over advective flow sources
		for(k=0; k<=10; k++)
		{
			//Write label for overland plane (Part 3a: advection outflows)
			fprintf(msbfile_fp,"\tadvchemovoutmass %d (kg)", k);

		}	//end loop over advective flow sources

		//loop over dispersive flow sources
		for(k=1; k<=10; k++)
		{
			//Write label for overland plane (Part 3b: dispersion outflows)
			fprintf(msbfile_fp,"\tdspchemovoutmass %d (kg)", k);

		}	//end loop over flow sources

		//Write label for overland plane (Part 3c: erosion outflows)
		fprintf(msbfile_fp,"\terschemovoutmass (kg)");

		//Write label for overland plane (Part 3d: deposition outflows)
		fprintf(msbfile_fp,"\tdepchemovoutmass (kg)");

		//Write label for overland plane (Part 3e: infiltration outflows)
		fprintf(msbfile_fp,"\tinfchemovoutmass (kg)");

		//Write label for overland plane (Part 3f: biodegradation outflows)
		fprintf(msbfile_fp,"\tbiochemovoutmass (kg)");

		//Write label for overland plane (Part 3g: hydrolysis outflows)
		fprintf(msbfile_fp,"\thydchemovoutmass (kg)");

		//Write label for overland plane (Part 3h: oxidation outflows)
		fprintf(msbfile_fp,"\toxichemovoutmass (kg)");

		//Write label for overland plane (Part 3i: photolysis outflows)
		fprintf(msbfile_fp,"\tphtchemovoutmass (kg)");

		//Write label for overland plane (Part 3j: radioactive decay outflows)
		fprintf(msbfile_fp,"\tradchemovoutmass (kg)");

		//Write label for overland plane (Part 3k: volatilization outflows)
		fprintf(msbfile_fp,"\tvltchemovoutmass (kg)");

		//Write label for overland plane (Part 3l: user-defined reaction outflows)
		fprintf(msbfile_fp,"\tudrchemovoutmass (kg)");

		//Write label for overland plane (Part 4)
		fprintf(msbfile_fp,"\tfinal mass (kg)\n");

		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//loop over columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] != nodatavalue)
				{
					//if the cell is a channel cell
					if(imask[i][j] > 1)
					{
						//Get channel link and node
						chanlink = link[i][j];
						channode = node[i][j];

						//Compute area of channel within cell...
						//
						//Assign channel characteristics
						bwch = bwidth[chanlink][channode];			//channel bottom width (m)
						hbch = hbank[chanlink][channode];			//channel bank height (m)
						sslope = sideslope[chanlink][channode];		//channel side slope
						lch = chanlength[chanlink][channode];		//channel length (m) (includes sinuosity)
						twch = twidth[chanlink][channode];			//channel top width at bank height (m)

						//Surface area of channel portion of cell (m2)
						achsurf = twch * lch;
					}
					else	//cell is not a channel cell (overland only)
					{
						//no channel present, surface area is zero
						achsurf = 0.0;

						//set chanlink to -1
						chanlink = -1;

						//set channode to -1
						channode = -1;

					}	//end if imask > 1

					//write summary for the current cell (Part 1)
					fprintf(msbfile_fp,"\n%d\t%d\t%d\t%d\t%d\t%f",
						i, j, imask[i][j], chanlink, channode,
						initialchemov[ichem][i][j][0]);

					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//write summary for the current cell (Part 2a: advection inflows)
						fprintf(msbfile_fp,"\t%f", advchemovinmass[ichem][i][j][k]);

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//write summary for the current cell (Part 2b: dispersion inflows)
						fprintf(msbfile_fp,"\t%f", dspchemovinmass[ichem][i][j][k]);

					}	//end loop over dispersive flow sources

					//write summary for the current cell (Part 2c: erosion inflows)
					fprintf(msbfile_fp,"\t%f", erschemovinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2d: deposition inflows)
					fprintf(msbfile_fp,"\t%f", depchemovinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2e: infiltration inflows)
					fprintf(msbfile_fp,"\t%f", infchemovinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2f: biodegredation inflows)
					fprintf(msbfile_fp,"\t%f", biochemovinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2g: hydrolysis inflows)
					fprintf(msbfile_fp,"\t%f", hydchemovinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2h: oxidation inflows)
					fprintf(msbfile_fp,"\t%f", oxichemovinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2i: photolysis inflows)
					fprintf(msbfile_fp,"\t%f", phtchemovinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2j: radioactive decay inflows)
					fprintf(msbfile_fp,"\t%f", radchemovinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2k: volatilization inflows)
					fprintf(msbfile_fp,"\t%f", vltchemovinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2l: user-defined reaction inflows)
					fprintf(msbfile_fp,"\t%f", udrchemovinmass[ichem][i][j][0]);

					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//write summary for the current cell (Part 3a: advection outflows)
						fprintf(msbfile_fp,"\t%f", advchemovoutmass[ichem][i][j][k]);

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//write summary for the current cell (Part 3b: dispersion outflows)
						fprintf(msbfile_fp,"\t%f", dspchemovoutmass[ichem][i][j][k]);

					}	//end loop over dispersive flow sources

					//write summary for the current cell (Part 3c: erosion outflows)
					fprintf(msbfile_fp,"\t%f", erschemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3d: deposition outflows)
					fprintf(msbfile_fp,"\t%f", depchemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3e: infiltration outflows)
					fprintf(msbfile_fp,"\t%f", infchemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3f: biodegredation outflows)
					fprintf(msbfile_fp,"\t%f", biochemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3g: hydrolysis outflows)
					fprintf(msbfile_fp,"\t%f", hydchemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3h: oxidation outflows)
					fprintf(msbfile_fp,"\t%f", oxichemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3i: photolysis outflows)
					fprintf(msbfile_fp,"\t%f", phtchemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3j: radioactive decay outflows)
					fprintf(msbfile_fp,"\t%f", radchemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3k: volatilization outflows)
					fprintf(msbfile_fp,"\t%f", vltchemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3l: user-defined reaction outflows)
					fprintf(msbfile_fp,"\t%f", udrchemovoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 4)
					fprintf(msbfile_fp,"\t%f", finalchemov[ichem][i][j][0]);

				}	//end if imask[][] != nodatavalue

			}	//end loop over columns

			//write a blank line to file
			fprintf(msbfile_fp,"\n");

		}	//end loop over rows

		//if channels are simulated
		if(chnopt == 1)
		{
			//Write label for channel network (Part 1)
			fprintf(msbfile_fp,"\n\n\nMass summary for the channel network:\n\n");
			fprintf(msbfile_fp,"\nlink\tnode\tuplink\tupnode\tdownlink\tdownnode");
			fprintf(msbfile_fp,"\trow\tcol\tinitial mass (kg)");

			//loop over advective flow sources
			for(k=0; k<=10; k++)
			{
				//Write label for channel network (Part 2a: advective inflows)
				fprintf(msbfile_fp,"\tadvchemchinmass %d (kg)", k);

			}	//end loop over advective flow sources

			//loop over dispersive flow sources
			for(k=1; k<=10; k++)
			{
				//Write label for channel network (Part 2b: dispersive inflows)
				fprintf(msbfile_fp,"\tdspchemchinmass %d (kg)", k);

			}	//end loop over dispersive flow sources

			//Write label for channel network (Part 2c: erosion inflows)
			fprintf(msbfile_fp,"\terschemchinmass (kg)");

			//Write label for channel network (Part 2d: deposition inflows)
			fprintf(msbfile_fp,"\tdepchemchinmass (kg)");

			//Write label for channel network (Part 2e: infiltration inflows)
			fprintf(msbfile_fp,"\tinfchemchinmass (kg)");

			//Write label for channel network (Part 2f: biodegradation inflows)
			fprintf(msbfile_fp,"\tbiochemchinmass (kg)");

			//Write label for channel network (Part 2g: hydrolysis inflows)
			fprintf(msbfile_fp,"\thydchemchinmass (kg)");

			//Write label for channel network (Part 2h: oxidation inflows)
			fprintf(msbfile_fp,"\toxichemchinmass (kg)");

			//Write label for channel network (Part 2i: photolysis inflows)
			fprintf(msbfile_fp,"\tphtchemchinmass (kg)");

			//Write label for channel network (Part 2j: radioactive decay inflows)
			fprintf(msbfile_fp,"\tradchemchinmass (kg)");

			//Write label for channel network (Part 2k: volatilization inflows)
			fprintf(msbfile_fp,"\tvltchemchinmass (kg)");

			//Write label for channel network (Part 2l: user-defined reaction inflows)
			fprintf(msbfile_fp,"\tudrchemchinmass (kg)");

			//loop over advective flow sources
			for(k=0; k<=10; k++)
			{
				//Write label for channel network (Part 3a: advective outflows)
				fprintf(msbfile_fp,"\tadvchemchoutmass %d (kg)", k);

			}	//end loop over advective flow sources

			//loop over dispersive flow sources
			for(k=1; k<=10; k++)
			{
				//Write label for channel network (Part 3b: dispersive outflows)
				fprintf(msbfile_fp,"\tdspchemchoutmass %d (kg)", k);

			}	//end loop over dispersive flow sources

			//Write label for channel network (Part 3c: erosion outflows)
			fprintf(msbfile_fp,"\terschemchoutmass (kg)");

			//Write label for channel network (Part 3d: deposition outflows)
			fprintf(msbfile_fp,"\tdepchemchoutmass (kg)");

			//Write label for channel network (Part 3e: infiltration outflows)
			fprintf(msbfile_fp,"\tinfchemchoutmass (kg)");

			//Write label for channel network (Part 3f: biodegradation inflows)
			fprintf(msbfile_fp,"\tbiochemchoutmass (kg)");

			//Write label for channel network (Part 3g: hydrolysis inflows)
			fprintf(msbfile_fp,"\thydchemchoutmass (kg)");

			//Write label for channel network (Part 3h: oxidation inflows)
			fprintf(msbfile_fp,"\toxichemchoutmass (kg)");

			//Write label for channel network (Part 3i: photolysis inflows)
			fprintf(msbfile_fp,"\tphtchemchoutmass (kg)");

			//Write label for channel network (Part 3j: radioactive decay inflows)
			fprintf(msbfile_fp,"\tradchemchoutmass (kg)");

			//Write label for channel network (Part 3k: volatilization inflows)
			fprintf(msbfile_fp,"\tvltchemchoutmass (kg)");

			//Write label for channel network (Part 3l: user-defined reaction inflows)
			fprintf(msbfile_fp,"\tudrchemchoutmass (kg)");

			//Write label for channel network (Part 4)
			fprintf(msbfile_fp,"\tfinal mass (kg)\n");

			//loop over links
			for(i=1; i<=nlinks; i++)
			{
				//loop over nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Get overland cell row and column
					row = ichnrow[i][j];
					col = ichncol[i][j];

					//Find the upstream link and node...
					//
					//if it is the first node of the link
					if(j == 1)
					{
	//mlv:  expand reporting so that all possible uplink/node and downlink/node
	//mlv   combinations are reported.  Use 8 pairs of columns for uplink and
	//mlv   another 8 pairs of columns for downlink.  Loop over directions...
	//mlv   use updirection[][][] and downdirection[][][] to place the values
	//mlv   in the right columns and nupbranches[i][k] and ndownbranches[i][k]
	//mlv   to assign values...

	//mlv:  no. just report one valid upstream link even if there are more...

						//a link upstream of the current link
						uplink = -9999;

						//the upstream node is the last node of the upstream link
						upnode = -9999;   //nnodes[uplink];
					}
					else	//else it is not the first node of the link (j > 1)
					{
						//the upstream link is the current link
						uplink = i;

						//the upstream node is the prior node
						upnode = j - 1;

					}	//end if j = 1

					//Find the downstream link and node...
					//
					//if it not the last node of the link
					if(j < nnodes[i])
					{
						//the downstream link is the current link
						downlink = i;

						//the downstream node is the next node
						downnode = j + 1;
					}
					else	//else it is the last node of the link (j = nnodes)
					{
						//Determine the downstream link
						downlink = link[ichnrow[link[row][col]][nnodes[link[row][col]]+1]]
							[ichncol[link[row][col]][nnodes[link[row][col]]+1]];

						//Determine downstream node (first node of link, except at boundaries)
						downnode = node[ichnrow[link[row][col]][nnodes[link[row][col]]+1]]
							[ichncol[link[row][col]][nnodes[link[row][col]]+1]];

					}	//end if j = 1

					//Compute area of channel within cell...
					//
					//Assign channel characteristics
					bwch = bwidth[i][j];		//channel bottom width (m)
					hbch = hbank[i][j];			//channel bank height (m)
					sslope = sideslope[i][j];	//channel side slope
					lch = chanlength[i][j];		//channel length (m) (includes sinuosity)
					twch = twidth[i][j];		//channel top width at bank height (m)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;

					//write summary for the current node (Part 1)
					fprintf(msbfile_fp,"\n%d\t%d\t%d\t%d",
						i, j, uplink, upnode);
					fprintf(msbfile_fp,"\t%d\t%d\t%d\t%d",
						downlink, downnode, row, col);
					fprintf(msbfile_fp,"\t%f",
						initialchemch[ichem][i][j][0]);
					
					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//write summary for the current node (Part 2a: inflows)
						fprintf(msbfile_fp,"\t%f", advchemchinmass[ichem][i][j][k]);

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//write summary for the current node (Part 2a: dispersive inflows)
						fprintf(msbfile_fp,"\t%f", dspchemchinmass[ichem][i][j][k]);

					}	//end loop over dispersive flow sources

					//write summary for the current cell (Part 2c: erosion inflows)
					fprintf(msbfile_fp,"\t%f", erschemchinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2d: deposition inflows)
					fprintf(msbfile_fp,"\t%f", depchemchinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2e: infiltration inflows)
					fprintf(msbfile_fp,"\t%f", infchemchinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2f: biodegredation inflows)
					fprintf(msbfile_fp,"\t%f", biochemchinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2g: hydrolysis inflows)
					fprintf(msbfile_fp,"\t%f", hydchemchinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2h: oxidation inflows)
					fprintf(msbfile_fp,"\t%f", oxichemchinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2i: photolysis inflows)
					fprintf(msbfile_fp,"\t%f", phtchemchinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2j: radioactive decay inflows)
					fprintf(msbfile_fp,"\t%f", radchemchinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2k: volatilization inflows)
					fprintf(msbfile_fp,"\t%f", vltchemchinmass[ichem][i][j][0]);

					//write summary for the current cell (Part 2l: user-defined reaction inflows)
					fprintf(msbfile_fp,"\t%f", udrchemchinmass[ichem][i][j][0]);

					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//write summary for the current cell (Part 3a: advection outflows)
						fprintf(msbfile_fp,"\t%f", advchemchoutmass[ichem][i][j][k]);

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//write summary for the current cell (Part 3b: dispersion outflows)
						fprintf(msbfile_fp,"\t%f", dspchemchoutmass[ichem][i][j][k]);

					}	//end loop over dispersive flow sources

					//write summary for the current cell (Part 3c: erosion outflows)
					fprintf(msbfile_fp,"\t%f", erschemchoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3d: deposition outflows)
					fprintf(msbfile_fp,"\t%f", depchemchoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3e: infiltration outflows)
					fprintf(msbfile_fp,"\t%f", infchemchoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3f: biodegredation outflows)
					fprintf(msbfile_fp,"\t%f", biochemchoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3g: hydrolysis outflows)
					fprintf(msbfile_fp,"\t%f", hydchemchoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3h: oxidation outflows)
					fprintf(msbfile_fp,"\t%f", oxichemchoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3i: photolysis outflows)
					fprintf(msbfile_fp,"\t%f", phtchemchoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3j: radioactive decay outflows)
					fprintf(msbfile_fp,"\t%f", radchemchoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3k: volatilization outflows)
					fprintf(msbfile_fp,"\t%f", vltchemchoutmass[ichem][i][j][0]);

					//write summary for the current cell (Part 3l: user-defined reaction outflows)
					fprintf(msbfile_fp,"\t%f", udrchemchoutmass[ichem][i][j][0]);

					//write summary for the current node (Part 4)
					fprintf(msbfile_fp,"\t%f", finalchemch[ichem][i][j][0]);

				}	//end loop over nodes

				//write a blank line to file
				fprintf(msbfile_fp,"\n");

			}	//end loop over links

		}	//end if chnopt = 1

	}	//end loop over chemicals

	//close the mass balance file
	fclose(msbfile_fp);

//End of function: Return to WriteMassBalance
}
