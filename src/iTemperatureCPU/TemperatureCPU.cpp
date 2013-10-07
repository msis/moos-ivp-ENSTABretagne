/**
 * \file TemperatureCPU.cpp
 * \brief Classe TemperatureCPU
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS surveillant l'état de chauffe du CPU
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "TemperatureCPU.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
TemperatureCPU::TemperatureCPU()
{
	m_iterations = 0;
	m_timewarp   = 1;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

TemperatureCPU::~TemperatureCPU()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool TemperatureCPU::OnNewMail(MOOSMSG_LIST &NewMail)
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
 
bool TemperatureCPU::OnConnectToServer()
{
	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool TemperatureCPU::Iterate()
{
	m_iterations++;
	m_Comms.Notify("VVV_TEMPERATURE_CPU", getTemperatureProcesseur());
	return(true);
}

/**
 * \fn
 * \brief Méthode retournant la température du processeur
 */
 
double TemperatureCPU::getTemperatureProcesseur()
{
	string data;
	ifstream fichier("/sys/bus/acpi/devices/LNXTHERM:00/thermal_zone/temp");
	if(!fichier)
	{
		cout << "Erreur d'ouverture des donnees de temperature\n";
		return -1;
	}
	
	if(!(fichier >> data))
	{
		cout << "Erreur de lecture des donnees de temperature\n";
		return -2;
	}
	
	return atoi(data.c_str()) / 1000.0;
}

/**
 * \fn
 * \brief Méthode retournant l'état de la batterie
 */
 
double TemperatureCPU::getPresentRateBattery()
{
	// Piste : /proc/acpi/battery/BAT0/state
		// present rate
		// remaining capacity
		// present voltage
	return 0.0;
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool TemperatureCPU::OnStartUp()
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
 
void TemperatureCPU::RegisterVariables()
{
	
}
