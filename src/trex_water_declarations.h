/*----------------------------------------------------------------------
C-  Header:     trex_water_declarations.h
C-
C-  Purpose/    Global declarations header file for hydraulic/hydrologic
C-  Methods:    calculations.  Used with trex_water_definitions.h.
C-
C-  Created:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO  80225
C-
C-  Date:       17-JUL-2004
C-
C-  Revised:    John England, USBR
C-
C-  Date:       14-SEP-2008
C-
C-  Revisions:  rainopt integration
C-
C-	Revised:	Mark Velleux
C-              Masa Takamatsu
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		29-Jul-2010
C-
C-	Revisions:	Added declarations for time step option (dtopt)
C-              implementation and Courant number calculations.
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C---------------------------------------------------------------------*/

//general order of header:
//  1. include files
//  2. global constant definitions
//  3. file declarations (organized by DataGroups A-E)
//  4. function prototypes (organized by order of appearance/DataGroups A-E)
//  5. variable declarations by state variables, general controls, then ksim
//    (1=water, 2=solids, 3=chemicals)

/*************************/
/*   FILE DECLARATIONS   */
/*************************/

//Data Group B
extern FILE *maskfile_fp;  //watershed mask file pointer
extern FILE *elevationfile_fp;  //elevation grid file pointer
extern FILE *soiltypefile_fp;  //soils grid file pointer
extern FILE *landusefile_fp;  //land use grid file pointer
extern FILE *storagedepthfile_fp;  //storage depth in overland plane grid file pointer

extern FILE *linkfile_fp;  //link map grid file pointer
extern FILE *nodefile_fp;  //node map grid file pointer
extern FILE *channelfile_fp;  //channel file pointer
extern FILE *channeltlossfile_fp;  //channel transmission loss propery file pointer

extern FILE *wateroverlandfile_fp;  //initial water on overland plane grid file pointer
extern FILE *infiltrationfile_fp;  //initial water in soils grid file pointer
extern FILE *waterchannelfile_fp;  //initial water in channels file pointer
extern FILE *translossfile_fp;  //initial water in sediments file pointer
extern FILE *snowfile_fp;  //initial snow depth (as swe) on overland plane grid file pointer

extern FILE *designraingridfile_fp;  //design storm grid file pointer
extern FILE *radarlocationfile_fp;  //radar rainfall location file pointer
extern FILE *radarrainfile_fp;  //radar rainfall rate file pointer
extern FILE *radarverifyfile_fp;  //radar rainfall verification file pointer
extern FILE *spacetimestormfile_fp;  //space-time storm file pointer
extern FILE *dadstormgridfile_fp;  //D-A-D storm grid file pointer

extern FILE *raingridfile_fp;  //rainfall grid file pointer
extern FILE *snowgridfile_fp;  //snowfall grid file pointer

extern FILE *slopefile_fp;  //DEM slope grid file pointer
extern FILE *aspectfile_fp;  //DEM slope aspect grid file pointer
extern FILE *skyviewfile_fp;  //Land use skyview grid file pointer

//Data Group F
extern FILE *waterexpfile_fp;  //water export file pointer

/***************************/
/*   FUNCTION PROTOTYPES   */
/***************************/

extern void ReadDataGroupB(void);  //Reads Data Group B of input file
extern void ReadMaskFile(void);  //Reads Mask File of active grid cells
extern void ReadElevationFile(void);  //Reads Elevation File of active grid cell elevations
extern void ReadLandUseFile(void);  //Reads Land Use Classification file
extern void ReadSoilTypeFile(int);  //Reads Soil Type classification file (for each soil layer)
extern void ReadStorageDepthFile(void);  //Reads (depression) storage depth file
extern void ReadChannelFile(void);  //Reads channel property file (tplgyopt = 0)
extern void ReadLinkFile(void);  //Reads channel link file (tplgyopt = 0)
extern void ReadNodeFile(void);  //Reads channel nodel file (tplgyopt = 0)
extern void ReadChannelTransmissionFile(void);  //Reads channel transmission loss property files (ksim = 1)

//Computes connectivity and spatial relationships of channel network
extern void ComputeChannelTopology(void);
extern void ComputeChannelLength(void);  //Computes length of each link/node in channel network

//Computes initial bed elevation of each link/node in channel network
extern void ComputeChannelElevation(void);
extern void ReadTopologyFile(void);  //Reads channel topology file (tplgyopt = 1)

//Reads initial water depth overland (at start of simulation) file
extern void ReadInitialWaterOverlandFile(void);

//Reads initial depth of infiltration (at start of simulation) file
extern void ReadInitialInfiltrationFile(void);

//Reads initial water depth in channels (at start of simulation) file
extern void ReadInitialWaterChannelFile(void);

//Reads initial depth of transmission loss (at start of simulation) file
extern void ReadInitialTransmissionLossFile(void);

//Reads DEM slope for snowmelt calculations
extern void ReadSlopeFile(void);

//Reads DEM aspect for snowmelt calculations
extern void ReadAspectFile(void);

//Reads Land use skyview for snowmelt calculations
extern void ReadSkyviewFile(void);

//Reads initial snow depth (swe) (at start of simulation) file
extern void ReadInitialSnowFile(void);

//Computes initial states of water variables at beginning of simulation
extern void ComputeInitialStateWater(void);
extern void InitializeWater(void);  //Initializes water transport variables (sets initial values)

//Initializes piecewise linear time functions for water (Loads, BCs)
extern void TimeFunctionInitWater(void);

//Updates piecewise linear time functions for water (Loads, BCs)
extern void UpdateTimeFunctionWater(void);

extern void WaterTransport(void);  //Computes derivative terms for water transport processes
extern void Rainfall(void);  //Computes gross rainfall rates
extern void Snowfall(void);  //Computes gross snowfall (swe) rates
extern void ReadDesignRainGrid(void);	//Read design rainfall grid
extern void ReadRadarRainLocations(void);	//Read radar rainfall locations
extern void ReadRadarRainRates(float, float, float);	//Read radar rainfall rates
extern void ReadSpaceTimeStorm(float, float, float);	//Read space-time storm file
extern void ReadRainGridFile(void);  //reads rainfall rates from a row-column grid file
extern void ReadSnowGridFile(void);  //reads snowfall (SWE) rates from a row-column grid file
extern void Interception(void);  //Computes interception depth and net rainfall/snowfall rates
//mlv	extern void RainInterception(void);  //Computes interception depth and net rainfall rates
//mlv	extern void SnowInterception(void);  //Computes interception depth and net snowfall rates
extern void Snowmelt(void);  //Computes melt rate of snowpack
extern void Infiltration(void);  //Computes rate and cumulative depth of infiltration for overland plane
extern void OverlandWaterRoute(void);  //Routes flow overland (based on water depth overland)
extern void TransmissionLoss(void);  //Computes rate and cumulative depth of transmission loss (infiltration) for channels
extern void ChannelWaterRoute(void);  //Routes flow through channels
extern void FloodplainWaterTransfer(void);	//Transfers water (flow) between overland plane and channels
extern void WaterBalance(void);  //Computes mass balance for water
extern void OverlandWaterDepth(void);  //Computes water depth overland
extern void ChannelWaterDepth(void);  //Computes flow depth in channels
extern void NewStateWater(void);  //Set new state (conditions) for water (flow depths) for next time step
extern void WriteTimeSeriesWater(void);  //Writes time series (export) water files
extern void WriteGridsWater(int);  //Writes water grid files
extern void WriteEndGridsWater(int);  //Writes end of simulation water grid files
extern void ComputeFinalStateWater(void);  //Computes final states of water variables at end of simulation
extern void WriteMassBalanceWater(void);  //Writes end of simulation mass balance for water
extern void WriteSummaryWater(void);  //Writes summary (water) flow statistics at end of simulation

//Frees allocated memory for general and water variables at end of simulation
extern void FreeMemoryWater(void);

/************************************/
/*   GLOBAL VARIABLE DECLARATIONS   */
/************************************/

/**********************/
//  state variables
/**********************/
extern float
  **hov,  //overland water depth in a grid cell (at current time t) (m)
  **hovnew,  //new overland water depth in a grid cell (for next time t+dt) (m)
  **hch,  //depth of water in a channel (at current time t) (m)
  **hchnew,  //depth of water in a channel (for next time t+dt) (m)
  **sweov,  //overland snowpack water depth (as swe) in a grid cell (at current time t) (m)
  **sweovnew;  //new overland snowpack water depth (as swe) in a grid cell (for next time t+dt) (m)

/******************************************************************/
//  hydrologic/hydraulic (ksim = 1) controls, support, reporting
/******************************************************************/

extern char
  *maskfile,  //Mask file
  *elevationfile,  //Elevation File
  *soiltypefile,  //Soil type file
  *landusefile,  //Land Use Classification File
  *channelfile,  //channel geometry file
  *storagedepthfile,  //Storgage depth file (for grid cells in overland plane)
  *wateroverlandfile,  //Initial Depth of Water for Overland Plane file
  *waterchannelfile,  //Initial Depth of Water for Channel Network file
  *infiltrationfile,  //Initial infiltration depth file
  *translossfile,  //Initial transmission loss depth file
  *linkfile,  //link file
  *nodefile,  //node file
  *topologyfile,  //topology file
  *channeltlossfile,  //channel transmission loss property file

  *designraingridfile,  //design storm grid file
  *radarlocationfile,  //radar rainfall location file
  *radarrainfile,  //radar rainfall rate file
  *radarverifyfile,  //radar rainfall verification file
  *spacetimestormfile,  //space-time storm file
  *dadstormgridfile,  //dad storm grid file

  *raingridfileroot,  //rainfall grid file root name
  *raingridfile,  //rainfall grid file root name (assumed extension = .0, .1, .2, .3 etc.)

  *snowfile,  //Initial Depth of Snow (as SWE) (on the overland plane) file
  *snowgridfileroot,  //snowfall grid file root name
  *snowgridfile,  //snowfall grid file root name (assumed extension = .0, .1, .2, .3 etc.)

  *slopefile,  //DEM slope file name (slope in degrees)
  *aspectfile,  //DEM slope aspect file name (aspect in degrees, north = 0)
  *skyviewfile,  //Land use skyview file name (fraction of unobstructed sky overhead that can be seen)

  **landname,  //Land use name
  **soilname,  //Soil name

  **qwovdescription,  //Overland flow source description
  **qwchdescription,  //Channel flow source description
  **hbcdescription,  //Outlet boundary condition description

  *waterexpfile,  //water export file
  *rainrategrid,  //rainfall rate grid file
  *raindepthgrid,  //rainfall depth grid file
  *infrategrid,  //infiltration rate grid file
  *infdepthgrid,  //infiltration depth grid file
  *qgrid,  //water discharge (q) grid file
  *waterdepthgrid,  //water depth grid file
  *swefallrategrid,  //snowfall rate (as SWE) grid file
  *swefalldepthgrid,  //snowfall depth (as SWE) grid file
  *swemeltrategrid,  //snowmelt rate (as SWE) grid file
  *swemeltdepthgrid,  //snowmelt depth (as SWE) grid file
  *swedepthgrid;  //snow depth (as SWE) grid file

extern int
  noutlets,  //number of outlet cells
  *iout,  //Overland outlet cell row
  *jout,  //Overland outlet cell column
  ncells,  //number of active (non-null) grid cells in the simulation mask
  nodatavalue, //value used to represent a null entry (no data) in a grid file
  **imask,  //Location (row, column) of watershed mask index for each cell in overland plane
  *cellrow,  //Location row of watershed mask index for each cell in overland plane
  *cellcol,  //Location column of watershed mask index for each cell in overland plane
  infopt,  //Infiltration Option, 0 = no infiltration, 1 = infiltration
  chnopt,  //Channel Option, 0 = no channels, 1 = channels
  tplgyopt,  //Topology Option, 0 = Compute topology from channel property file
            //and link, and node masks, 1 = Topology read from topology file
  ctlopt,  //Channel Transmission Loss Option, 0 = no transmission loss, 1 = transmission loss
  fldopt,  //floodplain transfer option, 0 = water transferred from overland to channel only,
          //1 = water transfered in either direction between the overland plane and channels
  outopt,  //Outlet Option, 0 = route to channels first, 1 = route overland first

  nsoils,  //Number Of Soil Types
  nlands,  //Number of Land Use Classes
  nsoils,  //Number Of Soil Types
  nlands,  //Number of Land Use Classes

  maxstackov,		//maximum number of layers in an overland soil stack column
  maxstackch,		//maximum number of layers in a channel sediment stack column

  **nstackov,  //present number of layers in the overland soil stack [row][col] (1 for ksim=1)
  **nstackch,  //present number of layers in the channel sediment stack [link][node] (1 for ksim=1)

  ***soiltype,  //Location (row, column, layer) of soil index for each cell in overland plane
  **landuse,  //Location (row, column) of land use index for each cell in overland plane

  nlinks,  //Number of links
  *nnodes,  //Number of nodes (for a link) in the channel network
  **link,  //Location (row, column) of channels cells for each link in channel network
  **node,  //Location (row, column) of channels cells for each node of each link in channel network
  **ichnrow,  //row address of channel link and node
  **ichncol,  //column address of channel link and node
  **nupbranches,  //Number of links (branches) upstream at a junction
  **ndownbranches,  //Number of links (branches) downstream at a junction
  ***updirection,  //Direction to the upstream interface of a node in a link
  ***downdirection,  //Direction to the downstream interface of a node of a link

  rainopt,  //Rainfall spatial interpolation option
  nrg,  //Number Of raingages, 1 = Uniform rain
  *rgid,  //Raingage id
  *nrpairs,  //Number of pairs of rainfall intensity and time
  raingridcount,  //running count of the number of times rainfall grids are read

  **designrainindex,  //Location (row, column) of design rain index for each cell in overland plane
  **radaridpoint,  //Location (row, col) of nearest neighbor radar "gage" for each overland cell
  **dadstormindex,  //Location (row, col) of DAD storm rain index for each overland cell

  snowopt,  //Snowfall option
  meltopt,  //Snowmelt option
  nsg,  //Number of snowgages (snowopt = 2)
  *sgid,  //Snowgage id
  *nspairs,  //Number of pairs of snowfall intensity and time
  snowgridcount,  //running count of the number of times snowfall grids are read

  *dbcopt,  //Domain boundary Condition Option, 0 = normal depth BC, 1 = specified water depth

  iteration,  //flow direction index for overland routing

  nqreports,  //Number of Flow reports
  nqwov,  //Number of overland flow point sources (external flows)
  nqwch,  //Number of channel flow point sources (external flows)

  *qrsoflag,  //flag indicating if a flow reporting station is also an outlet cell
  *qreprow,  //Row number for flow report
  *qrepcol,  //Column number for flow report
  *qunitsopt,  //Units option for flow report
  *qwovrow,  //Overland flow source row number
  *qwovcol,  //Overland flow source column number
  *nqwovpairs,  //Number of pairs of overland external flow volume and time
  *qwchlink,  //Channel flow source link number
  *qwchnode,  //Channel flow source node number
  *nqwchpairs,  //Number of pairs of channel external flow volume and time
  *nhbcpairs,  //Number of pairs of outlet water depth and time

  *qchoutlet,  //Outlet number associated with a channel network boundary link

  *rfpoint,  //pointer to current position for a rainfall function
  *sfpoint,  //pointer to current position for a snowfall function
  *qwovpoint,  //pointer to current position for an external overland flow function
  *qwchpoint,  //pointer to current position for an external channel flow function
  *hbcpoint;  //pointer to current position for a water depth BC function

extern float
	xllcorner,		//x location of grid lower left corner (m) (GIS projection)
	yllcorner,		//y location projection of grid lower left corner (m) (GIS projection)
	**elevationov0,	//initial ground surface elevation of each cell in the overland plane (m)
	**elevationov,	//present ground surface elevation of each cell in the overland plane (m)
	**elevationch0,	//initial channel bed elevation of node in channel betwork (m)
	**elevationch,	//present channel bed elevation of node in channel betwork (m)
	*ocx,			//Overland outlet x location (m)
	*ocy,			//Overland outlet y location (m)
	*sovout,		//Overland outlet cell slope
	*schout,		//channel outlet node bed slope

	**grossrainrate,	//gross rainfall rate (m/s)
	**grossraindepth,	//cumulative gross rainfall depth (m)
	**grossrainvol,		//gross rainfall volume entering a cell (m3)
	**netrainrate,		//net rainfall rate (m/s)
	**netrainvol,		//net rainfall volume entering a cell (m3)

	**grossswerate,		//gross snowfall (swe) rate (m/s)
	**grossswedepth,	//cumulative gross snowfall (swe) depth (m)
	**grossswevol,		//gross snowfall (swe) volume entering a cell (m3)
	**netswerate,		//net snowfall (swe) rate (m/s)
	**netswevol,		//net snowfall (swe) volume entering a cell (m3)

	**interceptiondepth,	//remaining (unsatisfied) interception depth (m)
	**interceptionvol,		//cumulative interception volume in a cell (m3)

	**swemeltrate,			//snowmelt rate (as swe) (m/s)
	**swemeltdepth,			//cumulative snowmelt depth (as swe) (m)
	**swemeltvol,			//cumulative snowmelt volume (as swe) in a cell (m3)

	**infiltrationrate,		//infiltration rate (m/s)
	**infiltrationdepth,	//cumulative infiltration depth (m)
	**infiltrationvol,		//cumulative infiltration volume in a cell (m3)

	**translossrate,	//transmission loss rate (m/s)
	**translossdepth,	//cumulative transmission loss depth (m)
	**translossvol,		//cumulative transmission loss volume in a node (m3)

	**storagedepth,		//depression storage depth in overland plane

	*nmanningov,		//Manning's n value in the overland plane
	*interceptionclass,	//interception depth for a land use class

	*khsoil,			//Hydraulic Conductivity of soil
	*capshsoil,			//Capillary Suction Head of soil
	*soilmd,			//Soil Moisture Deficit

	**khsed,			//Hydraulic Conductivity of sediment
	**capshsed,			//Capillary Suction Head of sediment
	**sedmd,			//Sediment Moisture Deficit

	*rgx,				//Raingage x coordinate (m)
	*rgy,				//Raingage y coordinate (m)
	**rfintensity,		//Rainfall intensity (m/s)
	**rftime,			//Rainfall time break (hours)
	*rfinterp,			//rainfall intensity interpolated in time
	idwradius,			//radius of influence for IDW spatial interpolation
	idwexponent,		//weighting exponent for IDW spatial interpolation
	raingridfreq,		//frequency at which rainfall grids are read from file (hours)
	rainconvunits,		//rainfall conversion factors for length, area, volume or mass units
	rainconvtime,		//rainfall conversion factor for time units
	rainscale,			//rainfall scale factor
	timeraingrid,		//time at which the next rainfall grid is read from file

	radarcellw,  //uniform spacing between radar cell centers
	raincenterx,  //storm center X coordinate (UTM)
	raincentery,  //storm center Y coordinate (UTM)
	stormelong,  //storm shape ratio
	stormorient,  //storm orientation angle (degrees from North, clockwise direction)
	*rainarea,  //DAD storm rainfall area
    *cumdaddepth, //cumulative dad depth for rain gage
    avgdepthwatershed,  //basin-average cumulative depth over the entire watershed

	*sgx,				//snowgage x coordinate (m)
	*sgy,				//snowgage y coordinate (m)
	**sfintensity,		//snowfall intensity (m/s)
	**sftime,			//snowfall time break (hours)
	*sfinterp,			//snowfall intensity interpolated in time
	sidwradius,			//radius of influence for snow IDW spatial interpolation
	sidwexponent,		//weighting exponent for snow IDW spatial interpolation
	snowgridfreq,		//frequency at which snowfall grids are read from file (hours)
	snowconvunits,		//snowfall grid conversion factors for length, area, volume or mass units
	snowconvtime,		//snowfall grid conversion factor for time units
	snowscale,			//snowfall grid scale factor
	timesnowgrid,		//time at which the next snowfall grid is read from file

//mlv:  Should this block of declarations be moved to "environmental"?
	tsnow,			//temperature below which precipitation is snow and above which snow melts (Celsius)
	latitude,		//geographic latitude the model domain center (degrees)
	**slope,		//dem land surface slope (degrees) [row][col]
	**aspect,		//dem land surface aspect (degrees) (north = 0) [row][col]
	**skyview,		//fraction of cell with an open view of the sky (0-1) [row][col]
	atmelt,			//snowmelt (as swe) air temperature index (melt processes correlated to temperature) (m/s/degree C)
	srmelt,			//snowmelt (as swe) solar radiation index (melt processes correlated to energy input) (m/s/(W/m2))
//********************************************************************

	**twidth,			//top width at bank height
	**bwidth,			//bottom width
	**sideslope,		//sideslope
	**hbank,			//channel bank height of channel (distance from bed to top of bank)
	**nmanningch,		//manning's n roughness coefficient for channels
	**sinuosity,		//channel sinuosity
	**deadstoragedepth,	//channel dead storage depth (m)
	**chanlength,		//length of channel element (m) (link/node)
	**lengthup,			//upstream half length of channel element (m) (link/node)
	**lengthdown,		//downstream half length of channel element (m) (link/node)

	***sfov,		//friction slope of an overland cell (dimensionless) [row][col][direction: N-S, E-W]
	**sfch,			//friction slope of a channel node (dimensionless) [link][node]

	maxcourantov,	//maximum overland cell courant number (dimensionless)
	maxcourantch,	//maximum channel node courant number (dimensionless)

	**dqov,			//net overland flow (m3/s)
	***dqovin,		//gross overland flow into a cell (m3/s) [r][c][direction]
	***dqovinvol,	//cumulative gross overland flow volume into a cell (m3) [r][c][direction]
	***dqovout,		//gross overland flow out of a cell (m3/s) [r][c][direction]
	***dqovoutvol,	//cumulative gross overland flow volume out of a cell (m3) [r][c][direction]
	**dqch,			//net channel flow at a node (m3/s)
	***dqchin,		//gross channel flow into a node (m3/s) [l][n][direction]
	***dqchinvol,	//cumulative gross channel flow volume into a node (m3) [l][n][direction]
	***dqchout,		//gross channel flow out of a node (m3/s) [l][n][direction]
	***dqchoutvol,	//cumulative gross channel flow volume out of a node (m3) [l][n][direction]

	*qinov,				//overland inflow entering domain from an outlet (m3/s)
	*qoutov,			//overland outflow exiting domain from an outlet (m3/s)
	*qoutovvol,			//cumulative flow volume exiting an overland outlet (m3)
	*qinch,				//channel flow entering a channel link (at a downstream junction) (m3/s)
	*qinchvol,			//cumulative flow volume entering a channel link (m3)
	*qoutch,			//channel flow exiting a channel link (at a downstream junction) (m3/s)
	*qoutchvol,			//cumulative flow volume exiting a channel link (m3)

	**qwovvol,			//cumulative external flow volume entering a cell (m3)
	**qwchvol,			//cumulative external flow volume entering a node (m3)
	**qwov,				//Overland external flow volume
	*qwovinterp,		//Overland external flow interpolated in time
	**qwovtime,			//Overland external flow volume time break (hours)

	**qwch,				//Channel external flow volume
	*qwchinterp,		//Channel external flow interpolated in time
	**qwchtime,			//Channel external flow volume time break (hours)

	**hbc,				//Outlet water depth (m)
	*hbcinterp,			//Outlet water depth interpolated in time (m)
	**hbctime,			//Outlet water depth time break (hours)

	ntr,		//update time for rainfall functions
	nts,		//update time for snowfall functions
	ntqwov,		//update time for overland flow functions
	ntqwch,		//update time for channel flow functions
	nthbc,		//update time for outler water depth functions

	*nrft,		//next (upper) interval time for rainfall intensity time series
	*prft,		//prior (lower) interval time for rainfall intensity time series
	*brf,		//intercept for rainfall intensity interpolation
	*mrf,		//slope for rainfall intensity interpolation

	*nsft,		//next (upper) interval time for snowfall intensity time series
	*psft,		//prior (lower) interval time for snowfall intensity time series
	*bsf,		//intercept for snowfall intensity interpolation
	*msf,		//slope for snowfall intensity interpolation

	*nqwovt,	//next (upper) interval time for external overland flow time series
	*pqwovt,	//prior (lower) interval time for external overland flow time series
	*bqwov,		//intercept for external overland flow interpolation
	*mqwov,		//slope for external overland flow interpolation

	*nqwcht,	//next (upper) interval time for external channel flow time series
	*pqwcht,	//prior (lower) interval time for external channel flow time series
	*bqwch,		//intercept for external channel flow interpolation
	*mqwch,		//slope for external channel flow interpolation

	*nhbct,		//next (upper) interval time for outlet water depth time series
	*phbct,		//prior (lower) interval time for outlet water depth time series
	*bhbc,		//intercept for outlet water depth interpolation
	*mhbc,		//slope for outlet water depth interpolation

	*qarea,				//Area of a watershed basin at a reporting station for flow report (m3)
	*qreportsum,		//Sum of overland and channel discharge at a flow reporting station (m3/s)
	*qreportov,			//Overland discharge at a flow reporting station (m3/s)
	*qreportch,			//Channel discharge at a flow reporting station (m3/s)
	*qconvert,			//Discharge export time series reporting conversion factor

	**initialwaterov,	//initial water volume in a overland plane cell (m3)
	**initialwaterch,	//initial water volume in a channel node (m3)
	**initialsweov,		//initial snowpack volume (as swe) in a overland plane cell (m3)

	initialwaterovvol,	//cumulative initial water volume in the overland plane (m3)
	initialwaterchvol,	//cumulative initial water volume in the channel network (m3)
	initialsweovvol,	//cumulative initial snowpack volume (as swe) in the overland plane (m3)

	**finalwaterov,		//final water volume in a overland plane cell (m3)
	**finalwaterch,		//final water volume in a channel node (m3)
	**finalsweov,		//final snowpack volume (as swe) in a overland plane cell (m3)

	finalwaterovvol,	//cumulative final water volume in the overland plane (m3)
	finalwaterchvol,	//cumulative final water volume in the channel network (m3)
	finalsweovvol,		//cumulative snowpack volume (as swe) in the overland plane (m3)

	*qpeakov,			//Peak overland flow at an outlet (m3/s)
	*tpeakov,			//Time of peak overland flow at an outlet (hrs)
	*qpeakch,			//Peak channel flow at an outlet (m3/s)
	*tpeakch,			//Time of peak channel flow at an outlet (hrs)

	totalgrossrainvol,	//cumulative sum of gross rainfall volumes entering domain (m3)
	totalnetrainvol,	//cumulative sum of net rainfall volumes entering domain (m3)
	totalgrossswevol,	//cumulative sum of gross snowfall volumes (as swe) entering domain (m3)
	totalnetswevol,		//cumulative sum of net snowfall volumes (as swe) entering domain (m3)
	totalswemeltvol,	//cumulative sum of snowmelt volumes (as swe) in domain (m3)
	totalinterceptvol,	//cumulative sum of interception volumes in domain (m3)
	totalinfiltvol,		//cumulative sum of infiltration volumes in domain (m3)
	totaltranslossvol,	//cumulative sum of transmission loss volumes in domain (m3)

	totalqoutov,		//cumulative sum of flow volume exiting overland plane (m3)
	totalqinch,			//cumulative sum of flow volume entering channel network (m3)
	totalqoutch,		//cumulative sum of flow volume exiting channel network (m3)
	totalqwov,			//cumulative sum of external flow volume entering overland plane (m3)
	totalqwch;			//cumulative sum of external flow volume entering channel network (m3)

extern double
	maxhovdepth,	//maximum water depth in overland plane (m)
	minhovdepth,	//minimum water depth in overland plane (m)
	maxhchdepth,	//maximum water depth in channels (m)
	minhchdepth,	//minimum water depth in channels (m)
	maxinfiltdepth,	//maximum infiltration depth (cumulative) (m)
	mininfiltdepth,	//minimum infiltration depth (cumulative) (m)
	maxtranslossdepth,	//maximum transmission loss depth (cumulative) (m)
	mintranslossdepth,	//minimum transmission loss depth (cumulative) (m)
	maxrainintensity,	//maximum rainfall intensity (m/s)
	minrainintensity,	//minimum rainfall intensity (m/s)
	maxsweintensity,	//maximum snowfall (swe) intensity (m/s)
	minsweintensity,	//minimum snowfall (swe) intensity (m/s)
	maxsweovdepth,	//maximum snowpack depth (as swe) in overland plane (m)
	minsweovdepth,	//minimum snowpack depth (as swe) in overland plane (m)
	maxswemeltdepth,	//maximum snowmelt depth (as swe) (cumulative) (m)
	minswemeltdepth;	//minimum snowmelt depth (as swe) (cumulative) (m)
