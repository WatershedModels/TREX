/*----------------------------------------------------------------------
C-  Function:   ComputeChannelTopology.c
C-
C-  Purpose/    Computes channel topology from link and node maps. Main
C-  Methods:	calcs are: finds link/node # of outlet cell; initialize
C-              and populate ichnrow[][] and ichncol[][] arrays used for
C-				channel depth and flow routing computations; modify mask
C-				grid (imask = 2) to flag for channel cells.
C-
C-  Inputs:		link[][], node[][], imask[][]  	(Globals)
C-
C-  Outputs:	ichnrow[][], ichncol[][], imask[][],
C-              nupbranches[], ndownbranches[],
C-              updirection[][][], downdirection[][][]  (Globals)
C-
C-  Controls:
C-
C-  Calls:      none
C-
C-  Called by:	ReadDataGroupB
C-
C-  Created:	Original Coding: Rosalia Rojas-Sanchez
C-				Civil Engineering, Colorado State University
C-				Fort Collins, CO 80523
C-
C-  Date:		19-JUN-2003
C-
C-  Revised:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO  80225
C-
C-              Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80521
C-
C-  Date:		01-AUG-2003
C-
C-  Revisions:	Document variables and main computations, correct
C-				channel outlet slope for 8 directions;
C-				deleted commented out version for 4 directions;
C-				renamed routine; turned on print out of ichn[][][]
C-
C-  Date:       17-JUN-2004
C-
C-  Revisions:  bug fixes for cases where link has one node
C-              and for links double-counted as upstream links
C-
C--------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void ComputeChannelTopology()
{
	//local variable declarations, initialization and description
	int
		uplink,  //link number of upstream link
		upnode,  //node number of upstream node
		downlink,  //link number of downstream link
		downnode,  //node number of downstream node
		row,  //row value of ichnrow[][] for present node
		col,  //col value of ichncol[][] for present node
		uprow,  //row value of ichnrow[][] for upstream node
		upcol,  //col value of ichncol[][] for upstream node
		downrow,  //row value of ichnrow[][] for downstream node
		downcol;  //col value of ichncol[][] for downstream node

	int
		**uplinkflag;  //flag to indicate grid location is a valid upstream link
	//at start, set to -9999 off stream  and 1 on stream
	//as move upstream and ID cell as uplink, reset cell to -9999
	//to prevent upstream links to be counted more than once

	//Allocate memory for upstream/downstream branch tracking at junctions
	nupbranches = (int **)malloc((nlinks+1) * sizeof(int *));
	ndownbranches = (int **)malloc((nlinks+1) * sizeof(int *));

	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Memory is allocated for nine elements (0-8).  The zeroeth element
		//is used to store the number of branches (upstream or downstream)
		//of the current link.  The remaining elements (1-8) indicate the
		//direction of branch convergence/divergence.  The link number of
		//a branch is stored in the element corresponding to the direction
		//by which the upstream/downstream link connects to the current
		//link.
		//
		//From the starting cell (s), the eight directions of branch
		//connection are defined as:
		//
		//   8 1 2
		//   7 s 3
		//   6 5 4
		//
		//North = 1, Northeast = 2, East = 3, Southeast = 4, South = 5,
		//Southwest = 6, West = 7, and Northwest = 8.
		//
		//Allocate remaining memory for branch tracking at junctions
		nupbranches[i] = (int *)malloc(9 * sizeof(int));
		ndownbranches[i] = (int *)malloc(9 * sizeof(int));

		//initialize number of branches...
		nupbranches[i][0] = 0;  //upstream branches and connections
		ndownbranches[i][0] = 0;  //downstream branches and connections

		//Loop over number of elements (1-8)
		for(j=1; j<=8; j++)
		{
			//initialize connection directions...
			nupbranches[i][j] = -1;  //upstream branches and connections
			ndownbranches[i][j] = -1;  //downstream branches and connections
		}  //end loop over elements

	}  //end loop over nlinks

	//Allocate memory for ichn row and column arrays: ichnrow[][], ichncol[][]
	ichnrow = (int**)malloc((nlinks+1) * sizeof(int *));
	ichncol = (int**)malloc((nlinks+1) * sizeof(int *));

	//Allocate remaining memory and initialize ichnrow[][] and ichncol[][] arrays
	//
	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Note: the number of array elements needed for the ichn row/col
		//      arrays is nnodes[i]+2 (where i is the link number):
		//      1) one for the zeroeth array element;
		//      2) one for each node (1 through nnodes[i]); and
		//      3) one for the location of the downstream link/node.
		//
		//Allocate remaining memory for ichnrow and ichncol
		ichnrow[i] = (int *)malloc((nnodes[i]+2) * sizeof(int));
		ichncol[i] = (int *)malloc((nnodes[i]+2) * sizeof(int));

		//Loop over number of nodes in current link
		for(j=0; j<=nnodes[i]+1; j++)
		{
			ichnrow[i][j] = 0;	//initialize ichnrow
			ichncol[i][j] = 0;	//initialize ichncol

		}  //end loop over number of nodes for current link (nnodes[i])

	}  //end loop over number over nlinks

	//Allocate memory and initialize uplinkflag grid
	//
	//The uplinkflag grid is used to signify that an
	//upstream link is available for connectivity at a junction.
	//As one moves from downstream to upstream in the channel network,
	//we only allow an upstream link to connect to a single downstream
	//link.  This takes care of the case in an 8-direction sweep that
	//there may be three adjacent links with lower link numbers than the
	//present link, but one of them has been identified as connecting to a
	//downstream link.  This trick prevents double-counting of upstream links.
	//An example is:
	//
	//     43 XX 62 XX   Here we move upstream. Links 43 and 63 are uplinks
	//     43 XX 63 31   to link 64.  However, when we are at link 63,
	//     XX 43 63 XX   we want to make sure and not count 43 as an upstream
	//     XX 64 XX XX   link.  This is done with an uplinkflag array.
	//                   Then, when we are at link 63, we properly see
	//                   only links 31 and 62 as valid upstream connections,
	//                   NOT 43
	//
	//Allocate memory for uplinkflag grid
	uplinkflag = (int **)malloc((nrows+1) * sizeof(int *));

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate remaining memory for uplinkflag grid
		uplinkflag[i] = (int *)malloc((ncols+1) * sizeof(int));

		//Loop over number of columns
		for(j=1; j<=ncols; j++)
		{
			//set dummy value
			uplinkflag[i][j] = -9999;

		}  //end loop over grid columns

	}  //end loop over grid rows

	//Update simulation mask (imask[][]) and begin population
	//of ichnrow and ichncol arrays...
	//
	//Also update uplinkflag grid with 1 for each channel location
	//
	//Identify channel cell locations from link and node arrays
	//
	//If a cell has a link and node (must have both!) then set
	//imask[row][col] = 2 to signify a channel cell exists and
	//set ichnrow[link][node] = row and ichncol[link][node] = col
	//
	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=ncols; j++)
		{
			//if link is not null (i.e. cell has a link)
			if(link[i][j] >= 1)
			{
				//if node is not null (i.e. cell has a node)
				if(node[i][j] >= 1)
				{
					//Set imask to signify a channel cell exists
					imask[i][j] = 2;

					//Set uplink flag to signify a possible uplink exists
					uplinkflag[i][j] = 1;

					//Set row and column of channel cell (assumes link/node number is correct)
					ichnrow[link[i][j]][node[i][j]] = i;  //row
					ichncol[link[i][j]][node[i][j]] = j;  //column
				}
				//if node is null (and link is not null), abort...
				else
				{
					//Write error message to file
					fprintf(echofile_fp, "\n\nError! Link and node locations do not match!\n");
					fprintf(echofile_fp,       "  row = %5d   column = %5d\n", i, j);
					fprintf(echofile_fp,       "  link = %5d   node = %5d\n", link[i][j], node[i][j]);

					//Write error message to screen
					printf("\n\nError! Link and node locations do not match!\n");
					printf(    "  row = %5d   column = %5d\n", i, j);
					printf(    "  link = %5d   node = %5d\n", link[i][j], node[i][j]);

					//abort
					exit(EXIT_FAILURE);

				}  //end check if node[i][j] not null

			}  //end check if link not null

		}  //end loop over columns

	}  //end loop over rows

	//Write label for channel network topology to file
	fprintf(echofile_fp,"\n\n\n  Channel Network Topology  \n");
	fprintf(echofile_fp,      "----------------------------\n\n");
	fprintf(echofile_fp,      "\n  Number of Links = %5d\n", nlinks);

	fprintf(echofile_fp, "\n\n  Link    ");
	fprintf(echofile_fp,     "  Nodes    ");
	fprintf(echofile_fp,     "  Uplink/Upnode    ");
	fprintf(echofile_fp,     "  Downlink/Downnode  \n");
	fprintf(echofile_fp,     "~~~~~~~~  ");
	fprintf(echofile_fp,     "~~~~~~~~~  ");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~  ");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Complete population of ichnrow and ichncol arrays...
	//
	//Search for upstream links that converge or diverge from the current
	//link and populate the start (zeroeth) and end (nnodes[]+1) elements
	//of the ichnrow and ichncol arrays... 
	//
	//Loop over over number of links in reverse order
	for(i=nlinks; i>=1; i--)
	{
		row = ichnrow[i][1];  //channel cell row (upstream element of link)
		col = ichncol[i][1];  //channel cell column (upstream element of link)

		uplink = -1;  //initialize upstream link
		upnode = -1;  //initialize upstream node
		downlink = -1;  //initialize downstream link
		downnode = -1;  //initialize downstream node

		//Search cells (9 total: 8 adjacent (a) plus the starting cell (s)):
		//
		//   a a a      8 1 2
		//   a s a      7 s 3
		//   a a a      6 5 4
		//
		//In terms of directions, sweep from 8-1-2, 7-3, 6-5-4
		//
		//Loop over adjacent rows
		for(j=row-1; j<=row+1; j++)
		{
			//Loop over adjacent columns
			for(k=col-1; k<=col+1; k++)
			{
				//if adjacent row and column are in the domain and cell is not the starting cell
				if((j>=1 && j<=nrows) && (k>=1 && k<=ncols) && (j != row || k != col))
				{
					//if the adjacent cell is a channel cell AND the cell has
					//  not already been chosen as an uplink to a previous link
					if(imask[j][k] > 1 && uplinkflag[j][k] == 1)
					{
						//Get link number of the adjacent cell (adjacent cell is a channel cell)
						uplink = link[j][k];

						//if the link of the upstream cell is an upstream link (uplink < i)
						if(uplink < i)	//recall: i is the current link number
						{
							//Get node number of adjacent (upstream) cell
							upnode = node[j][k];

							//if adjacent cell is last node of upstream link
							//(then this is a junction)
							if(upnode == nnodes[uplink])
							{
								//the upstream link is an upstream branch of the current link
								nupbranches[i][0] = nupbranches[i][0] + 1;

								//re-set the uplinkflag for this cell to null
								//so that it will not be selected as an uplink again
								uplinkflag[j][k] = -9999;

								//store the link number of the upstream link by its direction
								if((j-row) == -1)
								{
									if((k-col) == -1) nupbranches[i][8] = uplink;
									if((k-col) == 0) nupbranches[i][1] = uplink;
									if((k-col) == 1) nupbranches[i][2] = uplink;
								}

								if((j-row) == 0)
								{
									if((k-col) == -1) nupbranches[i][7] = uplink;
									if((k-col) == 1) nupbranches[i][3] = uplink;
								}

								if((j-row) == 1)
								{
									if((k-col) == -1) nupbranches[i][6] = uplink;
									if((k-col) == 0) nupbranches[i][5] = uplink;
									if((k-col) == 1) nupbranches[i][4] = uplink;
								}

								//the current link is a downstream branch of the upstream link
								ndownbranches[uplink][0] = ndownbranches[uplink][0] + 1;

								//store the link number of the downstream link by its direction
								if((j-row) == -1)
								{
									if((k-col) == -1) ndownbranches[uplink][4] = i;
									if((k-col) == 0) ndownbranches[uplink][5] = i;
									if((k-col) == 1) ndownbranches[uplink][6] = i;
								}

								if((j-row) == 0)
								{
									if((k-col) == -1) ndownbranches[uplink][3] = i;
									if((k-col) == 1) ndownbranches[uplink][7] = i;
								}

								if((j-row) == 1)
								{
									if((k-col) == -1) ndownbranches[uplink][2] = i;
									if((k-col) == 0) ndownbranches[uplink][1] = i;
									if((k-col) == 1) ndownbranches[uplink][8] = i;
								}

								//Add upstream (adjacent) link to start of current link (i)
								ichnrow[i][0] = j;
								ichncol[i][0] = k;

								//Add current (starting) cell to end of upstream link (uplink)
								ichnrow[uplink][upnode+1] = row;
								ichncol[uplink][upnode+1] = col;

								//Define downstream link and node for echo to file
								//
								//Remember: we added the link number of the upstream link
								//to the zeroeth array element of the current link...
								//
								//The downstream link is the link number of the channel cell
								//at the row and column location of the (nnodes[]+1) element of
								//current (starting) link --- this should always be greater
								//than the link number of the upstream link!
								//
								//yes, this is ugly...
								downlink = link[ichnrow[link[row][col]][nnodes[link[row][col]]+1]]
								[ichncol[link[row][col]][nnodes[link[row][col]]+1]];

								//Remember: we added the node number of the last node of the
								//upstream link to the nnodes[]+1 element of the current link...
								//
								//The downstream node is the node number of the channel cell
								//at the row and column location of the (nnodes[]+1) element of
								//current (starting) link --- this should always be the first
								//node of the downstream link!
								//
								//yes, this is ugly too...
								downnode = node[ichnrow[link[row][col]][nnodes[link[row][col]]+1]]
								[ichncol[link[row][col]][nnodes[link[row][col]]+1]];

								//Echo topology to file
								//
								//Check number of branches: report link, nnodes for nupbranches = 1
								if(nupbranches[i][0] == 1)
								{
									//First echo of topology to file (include link and nnodes)
									fprintf(echofile_fp,"%8d  %9d  %8d %8d  %10d %10d\n",
										i, nnodes[i], uplink, upnode, downlink, downnode);
								}
								else
								{
									//Multiple echo of topology to file
									//(do not include link and nnodes)
									fprintf(echofile_fp,"%s  %s  %8d %8d  %10d %10d\n",
										"        ", "         ", uplink, upnode,
										downlink, downnode);

								}	//end if nupbranches = 1

							}	//end if upnode = last node
						}
						else  //else the link is not an upstream link
						{
							//reset the upstream link/node (the link found is not upstream)
							uplink = -1;
							upnode = -1;

						}  //end if uplink is an upstream link

					}  //end if imask > 1 AND uplinkflag == 1

				}  //end if row and column are in domain...

			}  //end loop over adjacent columns

		}  //end loop over adjacent rows

		//if there are no upstream branches, we still need
		//to define the downlink and downnode for echo
		//
		//if the current link has no upstream branches
		if(nupbranches[i][0] == 0)
		{
			//Define downstream link and node for echo to file
			//
			//The downstream link is the link number of the channel cell
			//at the row and column location of the (nnodes[]+1) element of
			//current (starting) link --- this should always be greater
			//than the link number of the upstream link!
			downlink = link[ichnrow[link[row][col]][nnodes[link[row][col]]+1]]
			[ichncol[link[row][col]][nnodes[link[row][col]]+1]];

			//The downstream node is the node number of the channel cell
			//at the row and column location of the (nnodes[]+1) element of
			//current (starting) link --- this should always be the first
			//node of the downstream link!
			downnode = node[ichnrow[link[row][col]][nnodes[link[row][col]]+1]]
			[ichncol[link[row][col]][nnodes[link[row][col]]+1]];

			//Echo topology to file
			fprintf(echofile_fp,"%8d  %9d  %8d %8d  %10d %10d\n",
				i, nnodes[i], uplink, upnode, downlink, downnode);

		}  //end if the current link has no upstream branches

		//Add a blank line to the echo file
		fprintf(echofile_fp,"\n");

	}  //end loop over links (in reverse order)

	//Determine sources/directions to upstream and downstream nodes...
	//
	//For each link in the network, the nupbranches and ndownbranches
	//arrays store the link and node numbers of any links that connect
	//to a junction node (the first or last nodes) of a given link by
	//the direction number (1-8) of that link.
	//
	//However, for mass balance tracking purposes, we need to define the
	//interface number of the flow path between all nodes (interior as
	//well as junction nodes) along both their upstream and downstream
	//interfaces.
	//
	//Allocate memory for upstream/downstream direction between nodes
	updirection = (int ***)malloc((nlinks+1) * sizeof(int **));
	downdirection = (int ***)malloc((nlinks+1) * sizeof(int **));

	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Allocate additional memory for upstream/downstream directions
		updirection[i] = (int **)malloc((nnodes[i]+1) * sizeof(int *));
		downdirection[i] = (int **)malloc((nnodes[i]+1) * sizeof(int *));

		//Loop over number of nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//Allocate remaining memory for upstream/downstream directions
			updirection[i][j] = (int *)malloc(9 * sizeof(int));
			downdirection[i][j] = (int *)malloc(9 * sizeof(int));

			//Loop over directions (icluding zeroeth element)
			for(k=0; k<=8; k++)
			{
				//Initialize upstream/downstream direction arrays
				updirection[i][j][k] = -1;
				downdirection[i][j][k] = -1;

			}  //End loop over directions

		}  //End loop over nodes

		//Search adjacent nodes to find upstream and downstream directions
		//
		//Start of link (node j=1)
		j = 1;

		//If the present link has upstream branches
		if(nupbranches[i][0] > 0)
		{
			//Loop over directions
			for(k=1; k<=8; k++)
			{
				//An upstream branch can be either:
				//  1) a domain boundary (nupbranches[][] = 0); or
				//  2) an link in the network (nupbranches[][] > 0).
				//
				//If no branch exists, nupbranches[][] = -1
				//
				//If a branch exists in this direction
				if(nupbranches[i][k] >= 0)
				{
					//If the branch is a boundary (direction = 10)
					if(nupbranches[i][k] == 0)
					{
						//Set the direction to 10 (network/domain boundary)
						updirection[i][j][k] = 10;
					}
					else	//Else the branch is interior (not a boundary)
					{
						//Set the direction to the upstream node
						updirection[i][j][k] = k;

					}  //End if nupbranches[i][k] = 0

				}  //End if nupbranches[i][k] >= 0

			}  //End loop over directions (for first node)
		}
		else  //there are no upstream branches (head water)
		{
			//The node is the first node of a head water link that
			//originates within the model domain...
			//
			//Set the direction to -1 (headwater link/node)
			updirection[i][j][0] = -1;

		}  //End if nupbranches[i][0] > 0

		//Interior of link (nodes j=2 through j=nnodes[i])
		//
		//Loop over interior nodes (include upstream interface of last node)
		for(j=2; j<=nnodes[i]; j++)
		{
			//Position of upstream node
			uprow = ichnrow[i][j-1];
			upcol = ichncol[i][j-1];

			//Position of present node
			row = ichnrow[i][j];
			col = ichncol[i][j];

			//Position of downstream node
			downrow = ichnrow[i][j+1];
			downcol = ichncol[i][j+1];

			//For interior nodes, the flow path is limited to two
			//directions (interior nodes  do not have branches).
			//For convenience in flow routing, interface numbers
			//for interior nodes are stored in the zeroeth element
			//of the third index of the up/down direction arrays.
			//
			//Set direction of upstream interface
			if((uprow-row) == -1)
			{
				if((upcol-col) == -1)
				{
					downdirection[i][j-1][0] = 4;
					updirection[i][j][0] = 8;
				}
				else if((upcol-col) == 0)
				{
					downdirection[i][j-1][0] = 5;
					updirection[i][j][0] = 1;
				}
				else if((upcol-col) == 1)
				{
					downdirection[i][j-1][0] = 6;
					updirection[i][j][0] = 2;
				}
			}
			else if((uprow-row) == 0)
			{
				if((upcol-col) == -1)
				{
					downdirection[i][j-1][0] = 3;
					updirection[i][j][0] = 7;
				}
				else if((upcol-col) == 1)
				{
					downdirection[i][j-1][0] = 7;
					updirection[i][j][0] = 3;
				}
			}
			else if((uprow-row) == 1)
			{
				if((upcol-col) == -1)
				{
					downdirection[i][j-1][0] = 2;
					updirection[i][j][0] = 6;
				}
				else if((upcol-col) == 0)
				{
					downdirection[i][j-1][0] = 1;
					updirection[i][j][0] = 5;
				}
				else if((upcol-col) == 1)
				{
					downdirection[i][j-1][0] = 8;
					updirection[i][j][0] = 4;
				}
			}	//end set direction of upstream interface

			//if the node is not the last node of the link
			if(j != nnodes[i])
			{
				//Set direction of downstream interface
				if((downrow-row) == -1)
				{
					if((downcol-col) == -1)
					{
						downdirection[i][j][0] = 8;
						updirection[i][j+1][0] = 4;
					}
					else if((downcol-col) == 0)
					{
						downdirection[i][j][0] = 1;
						updirection[i][j+1][0] = 5;
					}
					else if((downcol-col) == 1)
					{
						downdirection[i][j][0] = 2;
						updirection[i][j+1][0] = 6;
					}
				}
				else if((downrow-row) == 0)
				{
					if((downcol-col) == -1)
					{
						downdirection[i][j][0] = 7;
						updirection[i][j+1][0] = 3;
					}
					else if((downcol-col) == 1)
					{
						downdirection[i][j][0] = 3;
						updirection[i][j+1][0] = 7;
					}
				}
				else if((downrow-row) == 1)
				{
					if((downcol-col) == -1)
					{
						downdirection[i][j][0] = 6;
						updirection[i][j+1][0] = 2;
					}
					else if((downcol-col) == 0)
					{
						downdirection[i][j][0] = 5;
						updirection[i][j+1][0] = 1;
					}
					else if((downcol-col) == 1)
					{
						downdirection[i][j][0] = 4;
						updirection[i][j+1][0] = 8;
					}
				}  //end set direction of downstream interface

			}	//end if the node is not the last node of the link

		}  //End loop over interior nodes

		//End of link (node j=nnode[i])
		j = nnodes[i];

		//If the present link has downstream branches
		if(ndownbranches[i][0] > 0)
		{
			//Loop over directions
			for(k=1; k<=8; k++)
			{
				//A downstream branch can be either:
				//  1) a domain boundary (ndownbranches[][] = 0); or
				//  2) an link in the network (ndownbranches[][] > 0).
				//
				//If no branch exists, ndownbranches[][] = -1
				//
				//If a branch exists in this direction
				if(ndownbranches[i][k] >= 0)
				{
					//If the branch is a boundary (direction = 10)
					if(ndownbranches[i][k] == 0)
					{
						//Set the direction to 10 (network/domain boundary)
						downdirection[i][j][k] = 10;
					}
					else	//Else the branch is interior (not a boundary)
					{
						//Set the direction to the downstream node
						downdirection[i][j][k] = k;

					}  //End if ndownbranches[i][0] = 0

				}  //End if ndownbranches[i][k] >= 0

			}  //End loop over directions (for last node)
		}
		else  //there are no downstream branches
		{
			//Set the direction to to 10 (network/domain boundary)
			downdirection[i][j][0] = 10;

		}  //End if ndownbranches[i][0] > 0

	}  //end loop over links

	//Report channel interface directions...
	//
	//Write label for channel node directions to file
	fprintf(echofile_fp,"\n\n\n  Channel Node Interface Directions  \n");
	fprintf(echofile_fp,      "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	fprintf(echofile_fp, "\n  Link    ");
	fprintf(echofile_fp,   "  Node    ");
	fprintf(echofile_fp,   "  Updirection    ");
	fprintf(echofile_fp,   "  Downdirection  \n");
	fprintf(echofile_fp,   "--------  ");
	fprintf(echofile_fp,   "--------  ");
	fprintf(echofile_fp,   "---------------  ");
	fprintf(echofile_fp,   "-----------------\n\n");

	//Echo directions to file...
	//
	//Loop over links
	for(i=1; i<=nlinks; i++)
	{
		//Start of link (node j=1)
		j = 1;

		//if there is no upstream branch (or boundary)
		if(nupbranches[i][0] == 0)
		{
			//Set direction to zero
			k = 0;

			//Write directions to file
			fprintf(echofile_fp,"%8d  %8d  %15d  %17d\n",
				i, j, updirection[i][j][k], downdirection[i][j][k]);
		}
		else	//else there is at least one upstream branch (or boundary)
		{
			//Loop over directions (including zero)
			for(k=1; k<=8; k++)
			{
				//if there is a branch in this direction
				if(nupbranches[i][k] >= 0)
				{
					//Write directions to file
					fprintf(echofile_fp,"%8d  %8d  %15d  %17d\n",
						i, j, updirection[i][j][k], downdirection[i][j][0]);

				}  //end if nupbranches[i][k] >= 0

			}  //end loop over directions

		}  //end if nupbranches[i][0] == 0

		//Set direction number
		k = 0;

		//Loop over interior nodes
		for(j=2; j<=nnodes[i]-1; j++)
		{
			//Write directions to file
			fprintf(echofile_fp,"%8d  %8d  %15d  %17d\n",
				i, j, updirection[i][j][k], downdirection[i][j][k]);

		}  //end loop over interior nodes

		//End of link (node j=1)
		j = nnodes[i];

		//if there is no downstream branch (or boundary)
		if(ndownbranches[i][0] == 0)
		{
			//Set direction to zero
			k = 0;

			//Write directions to file
			fprintf(echofile_fp,"%8d  %8d  %15d  %17d\n",
				i, j, updirection[i][j][k], downdirection[i][j][k]);
		}
		else	//else there is at least one downstream branch (or boundary)
		{
			//Loop over directions (including zero)
			for(k=1; k<=8; k++)
			{
				//if there is a branch in this direction
				if(ndownbranches[i][k] >= 0)
				{
					//Write directions to file
					fprintf(echofile_fp,"%8d  %8d  %15d  %17d\n",
						i, j, updirection[i][j][0], downdirection[i][j][k]);

				}  //end if ndownbranches[i][k] >= 0

			}  //end loop over directions

		}  //end if nupbranches[i][0] == 0

		//Add a blank line for formatting to file 
		fprintf(echofile_fp,"\n");

	}  //end loop over links

	//free local array for uplinkflag
	for(i=1; i<=nrows; i++)
	{
		free(uplinkflag[i]);
	}
	free(uplinkflag);

//End function: Return to ReadDataGroupB
}
