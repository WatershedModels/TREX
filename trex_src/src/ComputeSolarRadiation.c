/*----------------------------------------------------------------------
C-  Function:	ComputeSolarRadiation.c
C-
C-	Purpose/	ComputeSolarRadiation is called to compute the incident
C-  Methods:    solar radiation reaching the land surface as a function
C-              of atmospheric conditions with adjustments for solar
C-              position (declination, hour angle, azimuth), topographic
C-              slope, and landuse.
C-
C-              Based on equations presented by Liston and Elder (2006)
C-              for MicroMet (Apr-2006) and Snowmodel (Dec-2006).
C-
C-
C-	Inputs:		tzero, simtime, latitude, slope[][], azslope[][],
C-              cloudcover[][]
C-
C-	Outputs:	solarrad[][]
C-
C-	Controls:	chnopt, npropg, npropov, npropch, nsolids,
C-              nenvgtf, nenvovtf, nenvchtf, nfpocovtf, nfpocchtf,
C-              nenvgtfpairs, nenvovtfpairs, nenvchtfpairs,
C-              nfpocovpairs, nfpocchpairs
C-
C-	Calls:		None
C-
C-	Called by:	UpdateEnvironment
C-
C-	Created:	Mark Velleux
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		20-JAN-2007
C-
C-	Revised:
C-
C-	Date:	
C-
C-	Revisions:
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

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void ComputeSolarRadiation()
{
	//local variable declarations/definitions
	float
		aisrad,			//average incident solar radiation intensity (top of atmosphere) (W/m2)
		year,			//number of days in one year (averaging leap years)
		solstice,		//julian day of year of summer solstice
		jday,			//julian day of the simulation
		hour,			//hour of day
		declination,	//solar declination angle (radians)
		hangle,			//solar hour angle (radians)
		cosz,			//cosine of solar zenith angle (Z) (radians)
		sinz,			//sine of solar zenith angle (Z) (radians)
		azsun,			//azimuth of sun (radians)
		azslope,		//azimuth of land surface slope (radians)
		cosi,			//cosine of solar radiation incidence angle (radians)
		pi,				//pi = 3.1415926...
		degtorad,		//conversion factor for degrees to radians
		psidirect,		//atmospheric transmissivity for direct solar radiation
		psidiffuse,		//atmospheric transmissivity for diffuse solar radiation
		psicover,		//land cover transmissivity for direct/diffuse solar radiation
		qsidirect,		//direct shortwave solar radiation (W/m2)
		qsidiffuse;		//diffuse shortwave solar radiation (W/m2)

	//Define required constants
	aisrad = 1370.0;				//average incident solar radiation intensity (W/m2)
	year = 365.25;					//average number of days in one year (average, counting leap years)
	solstice = 173.0;				//julian day of year of summer solstice
	pi = (float)(2.0 * acos(0.0));	//pi
	degtorad = (float)(pi / 180.0);	//conversion factor for degrees to radians

	//Some basic definitions:
	//
	//  latitude = geographic latitude of the center of
	//             the model domain (degrees)
	//
	//  declination = angle between the rays of the sun and
	//                the plane of the equator, equals 0.41
	//                radians (23.45 degrees)
	//
	//  hour angle = angular distance between the sun and a
	//               meridian line along the earth, measured
	//               in hours (1 hour = 15 degrees).
	//
	//               Angle between an observer's meridian and
	//               the hour circle (celestial meridian) on
	//               which sun lies, expressed in units of time
	//               (hours, minutes, and seconds), which gives
	//               the time elapsed since the sun last passed
	//               the observer's meridian (> 0) or the time
	//               until the next transit (> 0).
	//
	//  zenith = point at which the sun is directly
	//           overhead relative to the local horizon
	//
	//  zenith angle = angle measured from the zenith to
	//                 the position of the sun
	//
	//  elevation angle = angle measured from the local
	//                    horizon to the position of the sun,
	//                    also called elevation or altitude
	//
	//  Note: The sine of the elevation angle is equal
	//        to the cosine of the zenith angle...
	//
	//  azimuth = angle measured clockwise along the local
	//            horizon from a reference point (the origin)
	//            to an object
	//
	//  solar azimuth = the azimuth angle measured along the
	//                  local horizon to the sun where due
	//                  south is the origin (due south = 0),
	//                  measured from south to west
	//
	//  slope azimuth = the azimuth angle measured along the
	//                  local horizon to a slope face where
	//                  due north is the origin (due north = 0),
	//                  measured from north to east = aspect
	//
	//  Symbols used in this code:
	//
	//    decliniation = declination (radians)
	//    hangle = hour angle (radians)
	//    z = zenith angle (radians), used as cos(z) or sin(z)
	//    alpha = elevation angle (radians)
	//    
	//
	//Compute solar radiation parameters...
	//
	//compute julian day for current sumulation time
	jday = (float)(tzero + (simtime / 24.0));

	//Adjust julian day when the simulation time > 365.25 days
	//
	//while the julian day > 365.25 (average number of days in a year)
	while(jday > 365.25)
	{
		jday = (float)(jday - 365.25);

	}	//end while jday > 365.25

	//Note:  tzero is a decimal value in julian days.  The
	//       hour of the day is the decimal part of tzero.
	//       At any time in the simulation, the hour of the
	//       day can be computed from the decimal part of
	//       (tzero + simtime / 24.0).  Similarly, the hour
	//       can also be computed the same way from the
	//       decimal part of jday.
	//
	//compute hour of day (hours) (value is: 0 <= hour < 24)
	hour = (float)((jday - (int)(jday)) * 24.0);

	//Compute solar declination angle (radians)
	declination = (float)(0.41 * cos(2.0 * pi * (jday - solstice) / year));

	//Compute solar hour angle (radians)
	hangle = (float)((hour * 15.0 - 180.0) * degtorad);

	//Compute cosine of solar zenith angle
	cosz = (float)(sin(declination) * sin(latitude * degtorad)
		+ cos(declination) * cos(latitude * degtorad) * cos(hangle));

	//if the cosine of the zenith angle < 0, reset it to zero
	if (cosz < 0.0) cosz = 0.0;

	//Compute corrections for terrain slope and aspect...
	//
	//Compute the sine of the solar zenith angle sin(z)
	sinz = (float)(sqrt(1.0 - cosz * cosz));

	//Azimuth of the sun (south has zero azimuth) (radians)
	azsun = (float)(asin(Max(-1.0, Min(1.0, cos(declination)*sin(hangle)/sinz))));

	//Corrections so azimuth angles below the local horizon
	//are measured from the normal to the slope...
	//
	//if the hour angle < zero
	if(hangle < 0.0)
	{
		//if the hour angle < solar azimuth,
		//  correct the solar azimuth
		if(hangle < azsun) azsun = -pi - azsun;
	}
	//else if the hour angle > zero
	else if(hangle > 0.0)
	{
		//if the hour angle > solar azimuth,
		//  correct the solar azimuth
		if(hangle > azsun) azsun = pi - azsun;

	}	//endif hangle < 0.0

	//Compute solar radiation for each cell in the domain
	//
	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Convert the slope aspect (where north is zero azimuth) to the
				//slope azimuth (azslope) with south having zero azimuth...
				//
				//if the slope aspect is >= 180 degrees (a west aspect)
				if(aspect[i][j] >= 180.0)
				{
					//subtract 180 degrees to convert aspect to azimuth (south zero azimuth) (degrees)
					azslope = (float)(aspect[i][j] - 180.0);
				}
				else	//else slope aspect is < 180 (an east aspect)
				{
					//add 180 degrees to convert aspect to azimuth (south zero azimuth) (degrees)
					azslope = (float)(aspect[i][j] + 180.0);

				}	//endif aspect[][] >= 180

				//Compute the angle between the normal to the slope and the angle
				//  at which direct solar radiation impinges on sloping terrain (radians).
				cosi = (float)(cos(slope[i][j] * degtorad) * cosz
					+ sin(slope[i][j] * degtorad) * sinz * 
					cos(azsun - azslope * degtorad));

				//Adjust the topographic correction for local slope so that
				//the correction is zero if the sun is below the local horizon 
				//(i.e., the slope is in the shade) or if the sun is below the
				//global horizon.
				//
				//if the sun is below the local horizon of the surface (the angle of incidence, cosi < 0)
				if(cosi < 0.0) cosi = 0.0;

				//if the sun is below the global horizon (the zenith angle, cosz <= 0)
				if(cosz <= 0.0) cosi = 0.0;

				//Compute atmospheric transmissivities for direct
				//and diffuse radiation...
				//
				//Account for clouds, water vapor, pollution, etc.
				//
				//compute atmospheric transmissivity (psi) for direct and diffuse radiation
				psidirect = (float)((0.6 + 0.2 * cosz) * (1.0 - cloudcover[i][j]));
				psidiffuse = (float)((0.3 + 0.1 * cosz) * cloudcover[i][j]);

				//Compute land cover transmissivity.
				//
				//  As described by Liston and Elder (2006), 0.71 provided a
				//  best-fit to the observations, when averaged over the two years
				//  of hourly data.  For pine forest, psicover = exp(-0.71 * LAI).
				//  LAI = leaf area index.
				//
				//  Account for any gaps in the forest canopy that will allow
				//  direct incoming solar radiation to reach the snow surface.
				//
				//Developer's Note:  The land cover transmissivity could be input
				//                   as a function of land use type and read in
				//                   Data Group B.  However, the transmissivity
				//                   would need to account for shade by buildings
				//                   through which no light penetrates...
				//
				//                   For now, adjust the sky view factor to account
				//                   for light penetration through forest canopy or
				//                   other cover and set cover transmissivity to zero.
				//
				//compute land cover (canopy) transmissivity (set to zero for now...)
				psicover = 0.0;

				//Compute solar radiation transmitted through the atmosphere and
				//  adjust for slope and terrain slope, aspect, and solar angle...
				//
				//compute direct short wave radiation (qsi) near the land surface (W/m2)
				qsidirect = cosi * psidirect * aisrad;

				//compute diffuse short wave radiation (qsi) near the land surface (W/m2)
				qsidiffuse = cosz * psidiffuse * aisrad;

				//Adjust solar radiation for sky view (blocking by tree canopy, shading by buildings etc.)
				//
				//Combine direct and diffuse solar radiation (W/m2)
				solarrad[i][j] = (float)(((1.0 - skyview[i][j]) * psicover
					+ skyview[i][j]) * (qsidirect + qsidiffuse));

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to UpdateEnvironment
}
