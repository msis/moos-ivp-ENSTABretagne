/**
 * \file Sonar.cpp
 * \brief Classe Sonar
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données liées au sonar
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "Sonar.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Sonar::Sonar()
{
	m_iterations = 0;
	m_timewarp   = 1;
}

/**
 * \fn
 * \brief Méthode initialisant le port série
 */
 
bool Sonar::initialiserPortSerie(string nom_port)
{
	int baud = 9600;
	
	// Instanciation de l'objet de communication avec le port série
	cout << "Initialisation de \"" << nom_port << "\" (" << baud << ")" << endl;
	this->m_moos_serial_port = CMOOSLinuxSerialPort();
	return this->m_moos_serial_port.Create((char*)nom_port.c_str(), baud);
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Sonar::~Sonar()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Sonar::OnNewMail(MOOSMSG_LIST &NewMail)
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
 
bool Sonar::OnConnectToServer()
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
 
bool Sonar::Iterate()
{
	m_iterations++;
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Sonar::OnStartUp()
{
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

			if(param == "SERIAL_PORT_NAME")
			{
				if(initialiserPortSerie(value))
					cout << "Port série initialisé !" << endl;
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
 
void Sonar::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
}
