/**
 * \file WallFollowing.cpp
 * \brief Classe WallFollowing
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 25th 2013
 *
 * Application MOOS de suivi de mur
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "WallFollowing.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
WallFollowing::WallFollowing()
{
	m_iterations = 0;
	m_timewarp   = 1;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

WallFollowing::~WallFollowing()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool WallFollowing::OnNewMail(MOOSMSG_LIST &NewMail)
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
 
bool WallFollowing::OnConnectToServer()
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
 
bool WallFollowing::Iterate()
{
	m_iterations++;
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool WallFollowing::OnStartUp()
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
 
void WallFollowing::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
}
