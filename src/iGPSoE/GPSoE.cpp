/**
 * \file GPSoE.cpp
 * \brief Classe GPSoE
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Application MOOS donnant les consignes de déplacement de l'AUV
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "GPSoE.h"

#define SZDEV "192.168.0.9:4001"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
GPSoE::GPSoE()
{
	m_iterations = 0;
	m_timewarp   = 1;
	
	hGPSoEMgr = INVALID_HGPSOEMGR_VALUE;
	//InitGPSoEMgr(&hGPSoEMgr, (char*)SZDEV, 100, DATA_POLLING_MODE_GPSOE, THREAD_PRIORITY_NORMAL, TRUE);
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

GPSoE::~GPSoE()
{
  	//ReleaseGPSoEMgr(&hGPSoEMgr);
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool GPSoE::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;

		#if 0 // Keep these around just for template
		string key   = msg.GetKey();
		string comm  = msg.GetCommunity();
		double dval  = msg.GetDouble();
		string sval  = msg.GetString(); 
		string msrc  = msg.GetSource();
		double mtime = msg.GetTime();
		bool   mdbl  = msg.IsDouble();
		bool   mstr  = msg.IsString();
		#endif
	}

	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool GPSoE::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool GPSoE::Iterate()
{
	m_iterations++;
	
	double utc=0.0, latitude=0.0, longitude=0.0;
	/*
	if (GetDataGPSoEMgr(hGPSoEMgr, &utc, &latitude, &longitude) == EXIT_SUCCESS)
	{
	    double moos_time = MOOSTime();
	    Notify("GPS_LAT", latitude, moos_time);
	    Notify("GPS_LON", longitude, moos_time);
	    Notify("GPS_UTC", utc, moos_time);
	    printf("UTC : %f, Latitude : %f, Longitude : %f\n", utc, latitude, longitude);
	}
	*/
	
	HGPSOE hGPSoE = INVALID_HGPSOE_VALUE;
	
	OpenGPSoE(&hGPSoE, SZDEV);
	
	GetDataGPSoE(hGPSoE, &utc, &latitude, &longitude);
	
	double moos_time = MOOSTime();
	Notify("GPS_LAT", latitude, moos_time);
	Notify("GPS_LON", longitude, moos_time);
	Notify("GPS_UTC", utc, moos_time);
	printf("UTC : %f, Latitude : %f, Longitude : %f\n", utc, latitude, longitude);
	
	CloseGPSoE(&hGPSoE);
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool GPSoE::OnStartUp()
{
	setlocale(LC_ALL, "C");
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
	{
		list<string>::iterator p;
		for(p = sParams.begin() ; p != sParams.end() ; p++)
		{
			string original_line = *p;
			string param = stripBlankEnds(toupper(biteString(*p, '=')));
			string value = stripBlankEnds(*p);

			if(param == "FOO")
			{
				//handled
			}
			
			else if(param == "BAR")
			{
				//handled
			}
		}
	}

	m_timewarp = GetMOOSTimeWarp();

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void GPSoE::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
}
