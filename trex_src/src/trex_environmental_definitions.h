/*----------------------------------------------------------------------
C-  Header:     trex_environmental_definitions.h
C-
C-	Purpose/	Global definitions header file for environmental
C-	Methods:	conditions.  Used with trex_environmental_declarations.h.
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
C-              Denver, CO  80225
C-
C-	Date:		16-MAR-2004
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C---------------------------------------------------------------------*/

/*************************/
/*   FILE DECLARATIONS   */
/*************************/

//Data Group E
FILE *envpropfile_fp;			//environmental property file pointer
FILE *envtfpointerfile_fp;		//environmental time function pointet file pointer

/************************************/
/*   GLOBAL VARIABLE DECLARATIONS   */
/************************************/

char
	pname[MAXNAMESIZE],		//environmental property name
	tfname[MAXNAMESIZE],	//environmental time function name
	*envpropfile,			//environmental property file
	*envtfpointerfile;		//environmental time function pointer file

int
	npropg,				//number of general environmental properties
	npropov,			//number of overland environmental properties
	npropch,			//number of channel environmental properties
	fpocovopt,			//overland particle fraction organic carbon option
	fpocchopt,			//channel particle fraction organic carbon option

	*pidg,				//property identification number for general properties [iprop]
	*nenvgtf,			//number of general property time functions [pid]
	***envgtfid, 		//general property time function id (pointer) [pid][row][col]
	**nenvgtfpairs,		//number of time series pairs in general property time function [pid][itf]
	**envgtfpoint,		//pointer to current position in series of an general time function [pid][itf]

	*pidov,				//property identification number for overland properties [iprop]
	*nenvovtf,			//number of overland property time functions [pid]
	****envovtfid, 		//overland property time function id (pointer) [pid][row][col][ilayer]
	**nenvovtfpairs,	//number of time series pairs in overland property time function [pid][itf]
	**envovtfpoint,		//pointer to current position in series of an overland time function [pid][itf]

	*pidch,				//property identification number for channel properties [iprop]
	*nenvchtf,			//number of channel property time functions [pid]
	****envchtfid, 		//channel property time function id (pointer) [pid][link][node][ilayer]
	**nenvchtfpairs,	//number of time series pairs in channel property time function [pid][itf]
	**envchtfpoint,		//pointer to current position in series of a channel time function [pid][itf]

	*nfpocovtf,			//number of overland particle time functions [isolid]
	****fpocovtfid,		//overland particle property time function id (pointer) [isolid][row][col][ilayer]
	**nfpocovtfpairs,	//number of time series pairs in overland particle time function [isolid][itf]
	**fpocovtfpoint,	//pointer to current position for in series of a overland particle time function [isolid][itf]

	*nfpocchtf,			//number of channel particle time functions [isolid]
	****fpocchtfid,		//channel particle property time function id (pointer) [isolid][link][node][ilayer]
	**nfpocchtfpairs,	//number of time series pairs in channel particle time function [isolid][itf]
	**fpocchtfpoint;	//pointer to current position for an channel particle time function [isolid][itf]

float
	***envg,			//general environmental property cell value [pid][row][col]
	***envgtf,			//general environmental property time function value [pid][itf][itfpair]
	***envgtftime,		//general environmental property time function time break [pid][itf][itfpair]

	****envov,			//overland environmental property cell value [pid][row][col][layer]
	***envovtf,			//overland environmental property time function value [pid][itf][itfpair]
	***envovtftime,		//overland environmental property time function time break [pid][itf][itfpair]
    
	****envch,			//channel environmental property node value [pid][link][node][layer]
	***envchtf,			//channel environmental property time function value [pid][itf][itfpair]
	***envchtftime,		//channel environmental property time function time break [pid][itf][itfpair]

	****fpocov,			//overland particle organic carbon cell value [isolid][row][col][layer]
	***fpocovtf,		//overland particle property time function value [isolid][itf][itfpair]
	***fpocovtftime,	//overland particle property time function time break [isolid][itf][itfpair]

	****fpocch,			//channel particle property node value [isolid][link][node][layer]
	***fpocchtf,		//channel particle property time function value [isolid][itf][itfpair]
	***fpocchtftime,	//channel particle property time function time break [isolid][itf][itfpair]

	*ntenvgtf,			//next time to update general time function [iprop]
	*ntenvovtf,			//next time to update overland time function [iprop]
	*ntenvchtf,			//next time to update channel time function [iprop]
	*ntfpocovtf,		//next time to update overland particle time function [iprop]
	*ntfpocchtf,		//next time to update channel particle time function [iprop]

	**nenvgtft,			//next (upper) interval time for general time function [iprop][itf]
	**penvgtft,			//prior (lower) interval time for general time function [iprop][itf]
	**benvgtf,			//intercept for general time function interpolation [iprop][itf]
	**menvgtf,			//slope for general time function interpolation [iprop][itf]
	**envgtfinterp,		//temporally interpolated general time function [iprop][itf]

	**nenvovtft,		//next (upper) interval time for overland time function [iprop][itf]
	**penvovtft,		//prior (lower) interval time for overland time function [iprop][itf]
	**benvovtf,			//intercept for overland time function interpolation [iprop][itf]
	**menvovtf,			//slope for overland time function interpolation [iprop][itf]
	**envovtfinterp,	//temporally interpolated time function [iprop][itf]

	**nenvchtft,		//next (upper) interval time for channel time function [iprop][itf]
	**penvchtft,		//prior (lower) interval time for channel time function [iprop][itf]
	**benvchtf,			//intercept for channel time function interpolation [iprop][itf]
	**menvchtf,			//slope for channel time function interpolation [iprop][itf]
	**envchtfinterp,	//temporally interpolated channel time function [iprop][itf]

	**nfpocovtft,		//next (upper) interval time for overland particle time function [isolid][itf]
	**pfpocovtft,		//prior (lower) interval time for overland particle time function [isolid][itf]
	**bfpocovtf,		//intercept for overland particle time function interpolation [isolid][itf]
	**mfpocovtf,		//slope for overland particle time function interpolation [isolid][itf]
	**fpocovtfinterp,	//temporally interpolated overland particle time function [isolid][itf]

	**nfpocchtft,		//next (upper) interval time for channel particle time function [isolid][itf]
	**pfpocchtft,		//prior (lower) interval time for channel particle time function [isolid][itf]
	**bfpocchtf,		//intercept for channel particle time function interpolation [isolid][itf]
	**mfpocchtf,		//slope for channel particle time function interpolation [isolid][itf]
	**fpocchtfinterp,	//temporally interpolated channel particle time function [isolid][itf]

	**windspeed,		//wind speed (m/s) [row][col]
	**airtemp,			//air temperature (C) [row][col]
	**solarrad,			//solar radiation (W/m2) [row][col]

	***cdocov,			//overland concentration of doc (g/m3) [row][col][layer]
	***fdocov,			//overland effection fraction doc (g/m3) [row][col][layer]
	***hardnessov,		//overland hardness concentration (g/m3) [row][col][layer]
	***phov,			//overland pH (s.u.) [row][col][layer]
	***temperatureov,	//overland water/soil temperature (C) [row][col][layer]
	***oxradov,			//overland concentration of oxidant/radical (g/m3) [row][col][layer]
	***bacteriaov,		//overland bacterial concentration (cell/100 mL) [row][col][layer]
	***extinctionov,	//overland light extinction coefficient (1/m) [row][col]
	***udrpropov,		//overland user-defined reaction property (units vary) [row][col][layer]

	***cdocch,			//channel concentration of doc (g/m3) [link][node][layer]
	***fdocch,			//channel effection fraction doc (g/m3) [link][node][layer]
	***hardnessch,		//channel hardness concentration (g/m3) [link][node][layer]
	***phch,			//channel pH (s.u.) [link][node][layer]
	***temperaturech,	//channel water/sediment temperature (C) [link][node][layer]
	***oxradch,			//channel concentration of oxidant/radical (g/m3) [link][node][layer]
	***bacteriach,		//channel bacterial concentration (cell/100 mL) [link][node][layer]
	***extinctionch,	//channel light extinction coefficient (1/m) [link][node]
	***udrpropch;		//channel user-defined reaction property (units vary) [link][node][layer]

//inputs for snowmelt and other general environmental functions
float
	*atlapse,			//air temperature lapse rate [istation]
	**stnelevenvg,		//station elevation for general environmental properties [iprop][istation]
	**cloudcover,		//fraction of sky covered by clouds (0-1) [row][col]
	**albedo;			//albedo (reflectance) of snow (0-1) [row][col]

double
	timemeteorology;	//update time for meteorological conditions (hours)
