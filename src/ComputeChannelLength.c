/*----------------------------------------------------------------------
C-  Function:	ComputeChannelLength.c
C-
C-	Purpose/	
C-	Methods:	Computes length of each channel node of each link in the
C-              channel network.  Channel lengths are computed from cell
C-              center to cell center.  The half the length is assigned
C-              to the downstream half of the upstream node and half to
C-              the upstream half of the downstream node. This approach
C-              allows cell legnths for flow routing to be computed from
C-              from node to node while allowing other cell properties
C-              to be defined fron the center of each cell as needed to
C-              compute a water balance for each cell.  Channel lengths
C-              are multiplied by stream sinuosity.
C-
C-	Inputs:		link[r][c], node[r][c], sinuosity[l][n]  (Globals)
C-	Outputs:	lengthup[l][n], lengthdown[l][n],        (Globals)
C-              chanlength[l][n]                         (Global)
C-	Controls:
C-
C-	Calls:	none
C-
C-	Called by:	ReadDataGroupB
C-
C-	Created:	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80521
C-
C-	Date:		09-AUG-2003
C-
C-	Revisions:
C-
C-	Date:
C-
C--------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void ComputeChannelLength()
{
	//local variable declarations
	int
		samerow,	//nodal difference in row location
		samecol;	//nodal difference in column location

	int
		downlink;	//downstream link at a junction

	//Note: lengthup = half cell length from upstream edge to cell mid-point
	//      lengthdown = half cell length from cell mid-point to downstream edge
	//
	//Allocate memory for channel length arrays
	chanlength = (float **)malloc((nlinks+1) * sizeof(float *));	//channel length
	lengthup = (float **)malloc((nlinks+1) * sizeof(float *));		//upstream half cell length
	lengthdown = (float **)malloc((nlinks+1) * sizeof(float *));	//downstream half cell length

	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Allocate remaining memory for channel length arrays
		chanlength[i] = (float *)malloc((nnodes[i]+2) * sizeof(float));		//channel length
		lengthup[i] = (float *)malloc((nnodes[i]+2) * sizeof(float));		//upstream half cell length
		lengthdown[i] = (float *)malloc((nnodes[i]+2) * sizeof(float));		//downstream half cell length

		//Loop over number of nodes in current link
		for(j=0; j<=nnodes[i]+1; j++)
		{
			//initialize channel length arrays to zero
			chanlength[i][j] = 0.0;
			lengthup[i][j] = 0.0;
			lengthdown[i][j] = 0.0;
		}	//end loop over number over nlinks

	}	//end loop over number of nodes for current link (nnodes[i])

	//Write label for channel lengths to file
	fprintf(echofile_fp,"\n\n\n  Channel Lengths  \n");
	fprintf(echofile_fp,      "~~~~~~~~~~~~~~~~~~~\n");
	fprintf(echofile_fp,    "\n    From: Link/Node     To: Link/Node     Length (m) \n");
	fprintf(echofile_fp,      "   -----------------   ---------------   ------------\n\n");

	//Determine channel lengths...
	//
	//For each node of each link, determine flow direction.  Two
	//cases exits:
	//
	//   1. flow is in a north-south (N-S) or east-west (E-W)
	//      (straight) orientation; or
	//   2. flow is in a NW-SE or NE-SW (diagonal)orientation.
	//
	//At the upstream end of first order links (links that have no
	//upstream links), the channel length is defined by the flow
	//direction to the second node of the link.
	//
	//At the downstream end of links at watershed outlets (links that
	//have no downstream links), the channel length is defined by the
	//flow direction from the second to last node of the link.
	//
	//At internal link junctions (where links converge or diverge),
	//the channel length defined by the flow direction to/from the
	//upstream/downstream branch using the information stored in
	//the zeroeth and nnodes[]+1 array elements of the link/node.
	//
	//Recall that the location of the first node of the downstream link
	//was stored in the nnodes[]+1 element of the all links and that
	//the location of the last node of the upstream link was stored in
	//the zeroeth element of all links.
	//
	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Loop over number of nodes-1 (last node handled after this loop)
		for(j=1; j<=nnodes[i]-1; j++)
		{
			//determine direction to downstream node from its row/column
			samerow = ichnrow[i][j+1] - ichnrow[i][j];	//nodal row difference
			samecol = ichncol[i][j+1] - ichncol[i][j];	//nodal column difference

			//if samerow = 0 or same column = 0, then the direction is N-S or E-W
			if((samerow == 0) || (samecol == 0))
			{
				//flow is straight
				//
				//Set cell half lengths: 0.5 * cell size
				lengthdown[i][j] = (float)(0.5 * w);	//downstream half length of current node
				lengthup[i][j+1] = (float)(0.5 * w);	//upstream half length of downstream node
			}
			else	//flow is on a diagonal
			{
				//Set cell half lengths: 0.5 * sqrt(2.0) * cell size
				lengthdown[i][j] = (float)(0.5 * sqrt(2.0) * w);	//downstream half length of current node
				lengthup[i][j+1] = (float)(0.5 * sqrt(2.0) * w);	//upstream half length of downstream node
			}	//end if samerow = 0 or samecol = 0

		}	//end loop over nodes

		//Compute lengths for last node of link (a junction)
		//
		//if number of downstream branches = 0
		if(ndownbranches[i][0] == 0)
		{
			//Note: if there are no donwstream branches, the exit direction of the link
			//is assumed to be the same as the direction between the nnodes-1 and nnodes
			//elements of the link.
			//
			//downstream half length of current node = upstream half length of current node
			lengthdown[i][nnodes[i]] = lengthup[i][nnodes[i]];

			//upstream half length of (virtual) exit node = downstream half length of current node
			lengthup[i][nnodes[i]+1] = lengthdown[i][nnodes[i]];
		}
		else if(ndownbranches[i][0] == 1)	//there is exactly one downstream branch
		{
			//determine direction to downstream node from its row/column
			samerow = ichnrow[i][nnodes[i]+1] - ichnrow[i][nnodes[i]];	//nodal row difference
			samecol = ichncol[i][nnodes[i]+1] - ichncol[i][nnodes[i]];	//nodal column difference

			//if samerow = 0 or same column = 0, then the direction is N-S or E-W
			if((samerow == 0) || (samecol == 0))
			{
				//flow is straight
				//
				//Set cell half lengths: 0.5 * cell size
				lengthdown[i][nnodes[i]] = (float)(0.5 * w);	//downstream half length of current node
				lengthup[i][nnodes[i]+1] = (float)(0.5 * w);	//upstream half length of downstream node
			}
			else	//flow is on a diagonal
			{
				//Set cell half lengths: 0.5 * sqrt(2.0) * cell size
				lengthdown[i][nnodes[i]] = (float)(0.5 * sqrt(2.0) * w);	//downstream half length of current node
				lengthup[i][nnodes[i]+1] = (float)(0.5 * sqrt(2.0) * w);	//upstream half length of downstream node
			}	//end if samerow = 0 or samecol = 0
		}
		else	//there is more than one downstream branch > 1
		{
			//if there is more than one downstream link, length
			//information must be stored in the zeroeth array of
			//the downstream link...
			//
			//Loop over directions to find downstream links
			for(j=1; j<=8; j++)
			{
				//if the downstream link number > 0, a downstream link exists
				if(ndownbranches[i][j] > 0)
				{
					//set downstream link numbers (downstream node is 1)
					downlink = ndownbranches[i][j];

					//Note: The value of the direction vector can be used to assign
					//channel lengths (i.e. if j is odd the flow path is straight,
					//if j is even the flow path is diagonal).  For consistency, the
					//direction determination algorithm used between nodes within a
					//link is used for junction as well.
					//
					//determine direction to first node of downstream link from its row/column
					samerow = ichnrow[downlink][1] - ichnrow[i][nnodes[i]];	//nodal row difference
					samecol = ichncol[downlink][1] - ichncol[i][nnodes[i]];	//nodal column difference

					//if samerow = 0 or same column = 0, then the direction is N-S or E-W
					if((samerow == 0) || (samecol == 0))
					{
						//flow is straight
						//
						//Set cell half lengths: 0.5 * cell size
						lengthdown[downlink][0] = (float)(0.5 * w);	//downstream half length of current node
						lengthup[downlink][1] = (float)(0.5 * w);	//upstream half length of downstream node
					}
					else	//flow is on a diagonal
					{
						//Set cell half lengths: 0.5 * sqrt(2.0) * cell size
						lengthdown[downlink][0] = (float)(0.5 * sqrt(2.0) * w);	//downstream half length of current node
						lengthup[downlink][1] = (float)(0.5 * sqrt(2.0) * w);		//upstream half length of downstream node
					}	//end if samerow = 0 or samecol = 0

				}	//end if link number in direction array (ndownbranches) > 0

			}	//end loop over directions

		}	//end if number of downstream branches = 0, 1, or >1

	}	//end loop over links

	//Add cell half lengths to obtain total channel lengths
	//
	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Loop over number of nodes
		for(j=1; j<=nnodes[i]-1; j++)
		{
			//Lengths are defines from node j to node j+1 (node to node)
			//
			//Compute channel length from cell half lengths
			chanlength[i][j] = (lengthdown[i][j] + lengthup[i][j+1]) * sinuosity[i][j];

			//Echo channel lengths to file
			fprintf(echofile_fp,"   %11d %4d   %10d %4d   %12.3f\n",
				i, j, i, j+1, chanlength[i][j]);
		}	//end loop over nnodes[i]-1 (j is index)

		//Compute lengths for last node of link (a junction)
		//
		//set index to last node of link
		j = nnodes[i];

		//if number of downstream branches = 0
		if(ndownbranches[i][0] == 0)
		{
			//Compute channel length from cell half lengths
			chanlength[i][j] = (lengthdown[i][j] + lengthup[i][j+1]) * sinuosity[i][j];

			//Echo channel lengths to file
			fprintf(echofile_fp,"   %11d %4d   %10d %4d   %12.3f\n",
				i, j, 0, 0, chanlength[i][j]);
		}
		else if(ndownbranches[i][0] == 1)	//there is exactly one downstream branch
		{
			//Compute channel length from cell half lengths
			chanlength[i][j] = (lengthdown[i][j] + lengthup[i][j+1]) * sinuosity[i][j];

			//Echo channel lengths to file
			fprintf(echofile_fp,"   %11d %4d   %10d %4d   %12.3f\n",
				i, j, link[ichnrow[i][nnodes[i]+1]][ichncol[i][nnodes[i]+1]],
				node[ichnrow[i][nnodes[i]+1]][ichncol[i][nnodes[i]+1]], chanlength[i][j]);
		}
		else	//there is more than one downstream branch > 1
		{
			//Loop over directions to find downstream links
			for(j=1; j<=8; j++)
			{
				//if the downstream link number > 0, a downstream link exists
				if(ndownbranches[i][j] > 0)
				{
					//set downstream link numbers (downstream node is 1)
					downlink = ndownbranches[i][j];

					//Compute channel length from cell half lengths
					chanlength[downlink][0] = (lengthdown[downlink][0] +
						lengthup[downlink][1]) * sinuosity[i][nnodes[i]];

					//Echo channel lengths to file
					fprintf(echofile_fp,"   %11d %4d   %10d %4d   %12.3f\n",
						i, nnodes[i], downlink, 1, chanlength[downlink][0]);
				}	//end if ndownbranches[i][j] > 0
			}	//end loop over branches
		}	//end if number of downstream branches > 0

		//Add a blank line to the echo file
		fprintf(echofile_fp,"\n");

	}	//end loop over links

//End function: Return to ReadDataGroupB
}
