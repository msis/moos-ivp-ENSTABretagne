/**
 * \file Logs.cpp
 * \brief Classe Logs
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS enregistrant les logs des missions de l'AUV
 *
 */
 
#include <iterator>
#include <fstream>
#include <iostream>
#include <string>
#include "MBUtils.h"
#include "Logs.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Logs::Logs()
{
	this->m_iterations 			= 0;
	this->m_timewarp   			= 1;
	this->m_auv_connecte 		= false;
	this->m_fichier_logs_cree	= false;
	this->m_generer_logs		= false;
	this->m_nom_fichier			= string(REPERTOIRE_LOGS) + "/" + string(PREFIXE_FICHIER_LOGS);
	this->m_position_auv_x		= 0;
	this->m_position_auv_y		= 0;
	this->m_position_auv_z		= 0;
	
	// Enregistrement des variables de la MOOSDB à suivre
	this->m_listeVariablesSuivies.push_back("VVV_NAV_X");
	this->m_listeVariablesSuivies.push_back("VVV_NAV_Y");
	this->m_listeVariablesSuivies.push_back("VVV_NAV_Z");
	this->m_listeVariablesSuivies.push_back("VVV_AUV_NAME");
	this->m_listeVariablesSuivies.push_back("VVV_AUV_IDENTIFIER");
	this->m_listeVariablesSuivies.push_back("VVV_ON_MISSION");
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Logs::~Logs()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Logs::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;
		
		if(msg.GetKey() == "VVV_ON_MISSION")
			this->m_generer_logs = (msg.GetDouble() == 1.0);
		
		if(msg.GetKey() == "VVV_NAV_X")
			m_position_auv_x = msg.GetDouble();
		
		if(msg.GetKey() == "VVV_NAV_Y")
			m_position_auv_y = msg.GetDouble();
		
		if(msg.GetKey() == "VVV_NAV_Z")
			m_position_auv_z = msg.GetDouble();
		
		if(msg.GetKey() == "VVV_AUV_NAME")
		{
			// Récupération de la date d'aujourd'hui
			char date[20];
			time_t maintenant;
			time(&maintenant);
			struct tm Today = *localtime(&maintenant);
			sprintf(date, "%2.2d-%2.2d-%4.4d", Today.tm_mday, Today.tm_mon + 1, Today.tm_year + 1900);
			
			// Calcul du nom du fichier de logs
			char chemin_fichier[500];
			sprintf(chemin_fichier, "%s/%s_%s_%s.log", REPERTOIRE_LOGS, PREFIXE_FICHIER_LOGS, (char*)msg.GetString().c_str(), date);
			this->m_nom_fichier = string(chemin_fichier);
			this->m_auv_connecte = true;
		}
		
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
 
bool Logs::OnConnectToServer()
{
	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool Logs::Iterate()
{
	m_iterations ++;
	
	if(this->m_auv_connecte && this->m_generer_logs)
	{
		if(!this->m_fichier_logs_cree)
		{
			if(!creerFichierLogs())
				cout << "Impossible de creer le fichier de logs" << endl;
		}
		
		else
			if(!enregistrerPositionLogs())
				cout << "Impossible d'ouvrir le fichier de logs" << endl;
	}
	
	return(true);
}

/**
 * \fn
 * \brief Méthode enregistrant la position de l'AUV dans les logs
 */
 
bool Logs::enregistrerPositionLogs()
{
	ofstream fichier((char*)this->m_nom_fichier.c_str(), ios::out | ios::app); // Ouverture : mode écriture à la suite

	if(fichier)
	{
		fichier << this->m_position_auv_x << ", " << this->m_position_auv_y << ", " << this->m_position_auv_z << endl;
		fichier.close();
		return true;
	}
	
	return false;
}

/**
 * \fn
 * \brief Méthode créant le fichier de logs
 */
 
bool Logs::creerFichierLogs()
{
	ofstream fichier((char*)this->m_nom_fichier.c_str(), ios::out | ios::trunc); // Ouverture : mode écriture + effacement du fichier ouvert

	if(fichier.is_open())
	{
		fichier.close();
		this->m_fichier_logs_cree = true;
		cout << "Fichierdddd de logs créé : \"" << this->m_nom_fichier << "\"" << endl;
		return true;
	}
	
	return false;
}

/**
 * \fn
 * \brief Méthode supprimant le fichier de logs
 */
 
bool Logs::supprimerFichierLogs()
{
	int retour = remove((char*)this->m_nom_fichier.c_str());
	
	if(retour == 0)
		cout << "Fichier de logs supprimé : \"" << this->m_nom_fichier << "\"" << endl;
		
	return retour == 0;
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Logs::OnStartUp()
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
 
void Logs::RegisterVariables()
{
	// Variables liées à la commande de l'AUV
	for(int i = 0 ; i < (int)this->m_listeVariablesSuivies.size() ; i++)
		m_Comms.Register(this->m_listeVariablesSuivies[i], 0);
}
