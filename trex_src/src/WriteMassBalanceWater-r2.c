/*---------------------------------------------------------------------
C-  Function:	WriteMassBalanceWater.c
C-
C-	Purpose/	Write detailed mass balance summary of flow (hydrology
C-	Methods:	and hydraulics) on a cell and and node basis at the
C-              end of a successful model run.
C-
C-
C-	Inputs:
C-
C-	Outputs:	mass balance text file
C-
C-	Controls:	ksim, chnopt
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
C-	Date:		05-FEB-2004
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

void WriteMassBalanceWater()
{
	//local variable declarations/definitions
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

	//Write header to file
	fprintf(msbfile_fp,"\n\n\nMass Balance for Water\n\n");

	//Write label for overland plane (Part 1)
	fprintf(msbfile_fp,"\nVolume summary for the overland plane:\n\n");
	fprintf(msbfile_fp,"\nrow\tcol\timask\tlink\tnode");
	fprintf(msbfile_fp,"\tinitial water vol (m3)\tinitial swe vol (m3)");
	fprintf(msbfile_fp,"\tgrossrain (m3)\tnetrain (m3)");
	fprintf(msbfile_fp,"\tgrossswe (m3)\tnetswe (m3)\tinterception (m3)");
	fprintf(msbfile_fp,"\tswemelt (m3)\tinfiltration (m3)");

	//loop over flow sources
	for(k=0; k<=10; k++)
	{
		//Write label for overland plane (Part 2: inflows)
		fprintf(msbfile_fp,"\tdqovinvol %d (m3)",k);

	}	//end loop over flow sources

	//loop over flow sources
	for(k=0; k<=10; k++)
	{
		//Write label for overland plane (Part 3: outflows)
		fprintf(msbfile_fp,"\tdqovoutvol %d (m3)",k);

	}	//end loop over flow sources

	//Write label for overland plane (Part 4)
	fprintf(msbfile_fp,"\tfinal vol (m3)\n");

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
					initialwaterov[i][j]);

				//if snowfall or snowmelt is simulated
				if(snowopt > 0 || meltopt > 0)
				{
					//write snowpack swe summary
					fprintf(msbfile_fp,"\t%f", initialsweov[i][j]);
				}
				else	//else snow hydrology (snowfall or snowmelt) is not simulated
				{
					//write blank (zero) snowpack summary
					fprintf(msbfile_fp,"\t%f", 0.0);

				}	//end if snowopt > 0 or meltopt > 0

				//write rainfall summary
				fprintf(msbfile_fp,"\t%f\t%f",
					grossrainvol[i][j] * (w * w - achsurf) / (w * w),
					netrainvol[i][j] * (w * w - achsurf) / (w * w));

				//if snowfall is simulated
				if(snowopt > 0)
				{
					//Developer's Note:  Right now, snowfall is entirely to the overland
					//                   plane.  No snowfall occurs on channels.  When
					//                   snowfall to channels is added, snowfall must be
					//                   apportioned to the overland part of the cell and
					//                   the remained added to the channel...
					//
//mlv					//write snowfall summary
//mlv					fprintf(msbfile_fp,"\t%f\t%f",
//mlv						grossswevol[i][j] * (w * w - achsurf) / (w * w),
//mlv						netswevol[i][j] * (w * w - achsurf) / (w * w));

					//write snowfall summary
					fprintf(msbfile_fp,"\t%f\t%f",
						grossswevol[i][j],
						netswevol[i][j]);
				}
				else	//else snowfall is not simulated
				{
					//write blank (zero) snowfall summary
					fprintf(msbfile_fp,"\t%f\t%f", 0.0, 0.0);

				}	//end if snowopt > 0

				//write interception summary
				fprintf(msbfile_fp,"\t%f",
					interceptionvol[i][j]);

				//if snowmelt is simulated
				if(meltopt > 0)
				{
					//write snowmelt summary
					fprintf(msbfile_fp,"\t%f",
						swemeltvol[i][j]);
				}
				else	//else snowmelt is not simulated
				{
					//write blank (zero) snowmelt summary
					fprintf(msbfile_fp,"\t%f", 0.0);

				}	//end if meltopt > 0

				//if infitration is simulated
				if(infopt > 0)
				{
					//write infiltration summary
					fprintf(msbfile_fp,"\t%f", infiltrationvol[i][j]);
				}
				else	//else infiltration is not simulated
				{
					//write blank (zero) infiltration summary
					fprintf(msbfile_fp,"\t%f", 0.0);

				}	//end if infopt > 0

				//loop over flow sources
				for(k=0; k<=10; k++)
				{
					//write summary for the current cell (Part 2: inflows)
					fprintf(msbfile_fp,"\t%f", dqovinvol[i][j][k]);

				}	//end loop over flow sources

				//loop over flow sources
				for(k=0; k<=10; k++)
				{
					//write summary for the current cell (Part 3: outflows)
					fprintf(msbfile_fp,"\t%f", dqovoutvol[i][j][k]);

				}	//end loop over flow sources

			//write summary for the current cell (Part 4)
			fprintf(msbfile_fp,"\t%f", finalwaterov[i][j]);

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

		//write a blank line to file
		fprintf(msbfile_fp,"\n");

	}	//end loop over rows

	//if channels are simulated
	if(chnopt == 1)
	{
		//Write label for channel network (Part 1)
		fprintf(msbfile_fp,"\n\n\nVolume summary for the channel network:\n\n");
		fprintf(msbfile_fp,"\nlink\tnode\tuplink\tupnode\tdownlink\tdownnode");
		fprintf(msbfile_fp,"\trow\tcol\tinitial vol (m3)\tgrossrain (m3)");
		fprintf(msbfile_fp,"\ttransmission loss vol (m3)");

		//loop over flow sources
		for(k=0; k<=10; k++)
		{
			//Write label for overland plane (Part 2: inflows)
			fprintf(msbfile_fp,"\tdqchinvol %d (m3)", k);

		}	//end loop over flow sources

		//loop over flow sources
		for(k=0; k<=10; k++)
		{
			//Write label for overland plane (Part 3: outflows)
			fprintf(msbfile_fp,"\tdqchoutvol %d (m3)", k);

		}	//end loop over flow sources

		//Write label for overland plane (Part 4)
		fprintf(msbfile_fp,"\tfinal vol (m3)\n");

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
/*
					//if there is only one upstream link/boundary
					if(nupbranches[i][0] == 1)
					{
*/
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


				//Developer's Note:  Right now, snowfall is entirely to the overland
				//                   plane.  No snowfall occurs on channels
				//
				//write summary for the current node (Part 1)
				fprintf(msbfile_fp,"\n%d\t%d\t%d\t%d",
					i, j, uplink, upnode);
				fprintf(msbfile_fp,"\t%d\t%d\t%d\t%d",
					downlink, downnode, row, col);
				fprintf(msbfile_fp,"\t%f\t%f",
					initialwaterch[i][j],
					grossrainvol[row][col] * achsurf / (w * w));
				
				//if channel trnamission loss is simulated
				if(ctlopt > 0)
				{
					//write transmission loss summary
					fprintf(msbfile_fp,"\t%f", translossvol[i][j]);
				}
				else	//else transmission loss is not simulated
				{
					//write blank (zero) transmission loss summary
					fprintf(msbfile_fp,"\t%f", 0.0);

				}	//end if ctlopt > 0

				//loop over flow sources
				for(k=0; k<=10; k++)
				{
					//write summary for the current node (Part 2: inflows)
					fprintf(msbfile_fp,"\t%f", dqchinvol[i][j][k]);

				}	//end loop over flow sources

				//loop over flow sources
				for(k=0; k<=10; k++)
				{
					//write summary for the current node (Part 3: outflows)
					fprintf(msbfile_fp,"\t%f", dqchoutvol[i][j][k]);

				}	//end loop over flow sources

			//write summary for the current node (Part 4)
			fprintf(msbfile_fp,"\t%f", finalwaterch[i][j]);

			}	//end loop over nodes

			//write a blank line to file
			fprintf(msbfile_fp,"\n");

		}	//end loop over links

	}	//end if chnopt = 1

	//close the mass balance file
	fclose(msbfile_fp);

//End of function: Return to WriteMassBalance
}
