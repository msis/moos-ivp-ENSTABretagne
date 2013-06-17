//Code by Jan SLIWKA in 2010

#ifndef SONAR_H
#define SONAR_H

#include "OSMisc.h"
#include "MiniKingMgr.h"
#include <vector>
#include <deque>
//#include "interval.h"
//------------------------------------------------------------------------------
#ifndef OO
#define OO 10000000 //big num
#endif // OO
//#define OO 1000000
//------------------------------------------------------------------------------

using namespace std;

class SonarDF{
public:
	//variables
	int threshold;
	double PMinDist;//distance percentage of range
	double PMaxDist;//distance percentage of range
	int range;//for simulation only
	double distance_error;
	//
	bool data_1_or_2;
	//The reference structure
	HMINIKINGMGR hMiniKingMgr;
	//HMINIKINGMGR *phMiniKingMgr;
public:
	SonarDF();
	SonarDF(char* filename);//default config from file, call configure
	~SonarDF();//here we close the connection
	//initialise and status
	void init(char* filename);
	int configure(char* szDevice,int range,int nsteps);//initialize and configure //root
	int configure(char* szDevice,int range,int nsteps,double scan_direction,double scan_width);
	bool isConnected();
	//Locks the device until connection	
	void SonarDF_lock();
	//------------------------------------------------------------------------------
	//RAW data
	//------------------------------------------------------------------------------
	void get_sonar_raw_data(double &sonar_angle_param,vector<int> &scanline);//one data
	void get_sonar_raw_data(vector<double> &Valpha,vector<vector<int> > &Vscanlines);//one sector
	//------------------------------------------------------------------------------
	//data pre-processing for distance detection (threshold....)
	//------------------------------------------------------------------------------
	//:::::configure:::::::
	void configure_distance_processing(int threshold,double PMinDist=0,double PMaxDist=OO);
	//:::::process the last measure:::::::
	double get_sonar_distance();//!! possibility of desynchronization distance / angle
	double get_sonar_angle();//!! possibility of desynchronization distance / angle
//	void getIntervalData(double &alpha, vector<interval> &intervalUnionOfDistances);
	void get_sonar_data(double &sonar_angle_param,double &sonar_distance_param);
	//:::::get the last sector scan::::
//	void getSectorScanIntervalData(vector<double> &Valpha, vector<vector<interval>> &VintervalUnionOfDistances);
	void get_sonar_data(vector<double> &Valpha,vector<double> &Vdistance);//first obstacle
//	void get_sonar_data(vector<double> &Valpha,vector<vector<interval>> &Vdistance);//first obstacle
private:	
	int GetFirstObstacleDistance(int RangeScale, double minDist, double maxDist, int threshold, vector<int> scanline, double &dist);
	//-create interval vector corresponding to all bins above threshold
//	void getObstacleDistanceIntervalUnion(int threshold, double rangescale, double mindist, double maxdist, double distance_error, vector<int> scanline,vector<interval> &p_distanceIntervalUnion);	
};
//------------------------------------------------------------------------------
#endif
