/*---------------------------------------------------------------------
C-  Function:	Infiltration.c
C-
C-  Purpose/    Infiltration.c computes the infiltration rate
C-  Methods:    and cumulative depth of infiltration for each
C-              cell in the overland plane.  Uses the Green-Ampt
C-              equation neglecting a term for head for the ponded
C-              water depth.
C-
C-
C-  Inputs:     hov[][] (at time t),
C-              infiltrationdepth[][] (at time t),
C-              soiltype[][][]
C-
C-  Outputs:    infiltrationrate[][] (at time t),
C-              infiltrationdepth[][] (at time t+dt)
C-              infiltrationvol[][] (at time t+dt)
C-
C-  Controls:   hov[][] (at time t), soiltemp[][][]
C-
C-  Calls:      None
C-
C-  Called by:  WaterTransport
C-
C-  Created:	P. Y. Julien, B. Saghafian, B. Johnson,
C-              and R. Rojas (CSU)
C-
C-  Date:       1991
C-
C-  Revised:    Mark Velleux (CSU)
C-              John England (USBR)
C-
C-  Date:       18-AUG-2003
C-
C-  Revisions:  Restructured code.
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		15-DEC-2006
C-
C-  Revisions:	Added option for snow hydrology with soil temperature
C-              as a switch to toggle infiltration (no infiltration
C-              when the soil is frozen).
C-
C-	Revised:	Mark Velleux
C-				Masa Takamatsu
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		29-Jul-2010
C-
C-	Revisions:	Cumulative mass balance calculations moved to
C-              OverlandWaterdepth for time step option (dtopt)
C-              implementation.
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

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void Infiltration()
{
	//local variable declarations/definitions
	int
		ilayer,		//index for layer in the soil stack (ksim > 1)
		isoil,		//index for soil type [row][col][layer]
		chanlink,	//channel link number
		channode;	//channel node number

	float
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf;	//surface area of channel (m2)

	float
		p1,	//first term in average infiltration rate equation
		p2;	//second term in average infiltration rate equation

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Note:  nstackov[][] always = 1 when ksim = 1...
				//
				//set the soil layer number for this location
				ilayer = nstackov[i][j];

				//set the soil type for this location
				isoil = soiltype[i][j][ilayer];

				//Set the cell surface area
				//
				//if the cell is a channel cell
				if(imask[i][j] > 1)
				{
					//Get channel link and node
					chanlink = link[i][j];
					channode = node[i][j];

					//Compute area of channel within cell...
					//
					//Assign channel characteristics
					lch = chanlength[chanlink][channode];		//channel length (m) (includes sinuosity)
					twch = twidth[chanlink][channode];			//channel top width at bank height (m)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;
				}
				else	//cell is not a channel cell (overland only)
				{
					//no channel present, surface area is zero
					achsurf = 0.0;

				}	//end if imask > 1

				//Infiltration occurs when the soil is not frozen
				//
				//Note: Only the soil surface layer is considered.
				//      Frozen soil layers below the surface are not
				//      considered.
				//
				//if the soil temperature > zero
				if(temperatureov[i][j][ilayer] > 0.0)
				{
					//set first and second terms for infiltration rate equation
					p1 = (float)(khsoil[isoil]*dt[idt] - 2.0*infiltrationdepth[i][j]);
					p2 = khsoil[isoil]*(infiltrationdepth[i][j] + capshsoil[isoil]*soilmd[isoil]);

					//Compute infiltration rate (m/s)
					infiltrationrate[i][j] = (float)((p1 + sqrt(pow(p1,2.0) + 8.0*p2*dt[idt]))/(2.0*dt[idt]));

					//check if the infiltration potential exceeds the available water
					if(infiltrationrate[i][j]*dt[idt] > hov[i][j])
					{
						//limit infiltration rate to available water supply
						infiltrationrate[i][j] = hov[i][j] / dt[idt];

					}	//end check of infiltration potential
				}
				else	//else soiltemp[][][] <= 0.0
				{
					//set the infiltration rate to zero
					infiltrationrate[i][j] = 0.0;

				}	//end if soiltemp[][][] > 0

		/*
		//mlv  Case-specific hack to run snowmelt model without soil temperature...
		//mlv  Assume that soil is frozen as long as the snowpack (swe) depth > 0.005 m.
		//mlv  When snowov[][] > 0.005, infiltration is zero
		//mlv
				//if the snowpack depth (as swe) > 0.005, set the infiltration rate to zero
				if(sweov[i][j] > 0.005) infiltrationrate[i][j] = 0.0;
		*/

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to WaterTransport
}
