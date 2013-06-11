/**
 * \file Ciscrea.cpp
 * \brief Classe Ciscrea
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS controlant le Ciscrea :
 * récupération des informations sur l'état de l'AUV et fonctions de commande
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "Ciscrea.h"

/**
 * \fn
 * \brief Constructeur de l'application MOOS (passage de l'identifiant d'un AUV)
 */
 
Ciscrea::Ciscrea(int identifiant_AUV)
{
	this->m_iterations = 0;
	this->m_timewarp = 1;
	this->m_auv_ciscrea = NULL;
	this->m_identifiant_auv_a_instancier = identifiant_AUV;
	
	// Enregistrement des variables de la MOOSDB à suivre
	this->m_listeVariablesSuivies.push_back("VVV_NAV_VX");
	this->m_listeVariablesSuivies.push_back("VVV_NAV_VY");
	this->m_listeVariablesSuivies.push_back("VVV_NAV_VZ");
	this->m_listeVariablesSuivies.push_back("VVV_NAV_RZ");
	this->m_listeVariablesSuivies.push_back("VVV_NAV_AX");
	this->m_listeVariablesSuivies.push_back("VVV_NAV_AY");
	this->m_listeVariablesSuivies.push_back("VVV_NAV_AZ");
	this->m_listeVariablesSuivies.push_back("VVV_SPOTLIGHTS");
	this->m_listeVariablesSuivies.push_back("VVV_AUV_IDENTIFIER");
	this->m_listeVariablesSuivies.push_back("VVV_ON_MISSION");
}

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Ciscrea::Ciscrea()
{
	Ciscrea(0);
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Ciscrea::OnNewMail(MOOSMSG_LIST &NewMail)
{
	if(this->m_auv_ciscrea == NULL)
	{
		if(this->m_identifiant_auv_a_instancier == 0)
			cout << "L'AUV doit etre identifie !" << endl;
		
		else
			instancierAUV();
	}
	
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;
		
		if(msg.GetKey() == "VVV_AUV_IDENTIFIER" && this->m_auv_ciscrea == NULL)
		{
			this->m_identifiant_auv_a_instancier = round(msg.GetDouble());
			
			if(this->m_identifiant_auv_a_instancier != 0)
				instancierAUV();
		}
		
		if(msg.GetKey() == "VVV_NAV_VX" && this->m_auv_ciscrea != NULL)
			m_auv_ciscrea->setVx(msg.GetDouble());
		
		if(msg.GetKey() == "VVV_NAV_VY" && this->m_auv_ciscrea != NULL)
			m_auv_ciscrea->setVy(msg.GetDouble());
		
		if(msg.GetKey() == "VVV_NAV_VZ" && this->m_auv_ciscrea != NULL)
			m_auv_ciscrea->setVz(msg.GetDouble());
		
		if(msg.GetKey() == "VVV_RZ" && this->m_auv_ciscrea != NULL)
			m_auv_ciscrea->setRz(msg.GetDouble());
		
		if(msg.GetKey() == "VVV_SPOTLIGHTS" && this->m_auv_ciscrea != NULL)
		{
			if(msg.GetDouble() == 1)
				m_auv_ciscrea->allumerProjecteurs();
			
			else
				m_auv_ciscrea->eteindreProjecteurs();
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
 * \brief Méthode instanciant l'AUV
 */
 
void Ciscrea::instancierAUV()
{
	this->m_auv_ciscrea = new AUV(this->m_identifiant_auv_a_instancier);
		
	// Mise à jour du nom dans la MOOSDB
	m_Comms.Notify("VVV_AUV_NAME", m_auv_ciscrea->getNom());
	m_Comms.Notify("VVV_AUV_IDENTIFIER", m_auv_ciscrea->getIdentifiant());
	//m_Comms.Notify("VVV_ON_MISSION", 1); // Début d'une mission
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool Ciscrea::OnConnectToServer()
{
	RegisterVariables();
	
	// Initialisations à 0
	for(int i = 0 ; i < (int)this->m_listeVariablesSuivies.size() ; i++)
		m_Comms.Notify((char*)this->m_listeVariablesSuivies[i].c_str(), 0.0);
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool Ciscrea::Iterate()
{
	m_iterations ++;
	
	// Informations sur l'AUV
	if(this->m_auv_ciscrea != NULL)
	{
		m_Comms.Notify("VVV_NAV_CAP", m_auv_ciscrea->getCap());
		m_Comms.Notify("VVV_NAV_Z", m_auv_ciscrea->getProfondeur());
		m_Comms.Notify("VVV_CURRENT_EXTERNAL_BATTERY_1", m_auv_ciscrea->getIntensiteBatterie1());
		m_Comms.Notify("VVV_CURRENT_EXTERNAL_BATTERY_2", m_auv_ciscrea->getIntensiteBatterie2());
		m_Comms.Notify("VVV_VOLTAGE_EXTERNAL_BATTERY_1", m_auv_ciscrea->getTensionBatterie1());
		m_Comms.Notify("VVV_VOLTAGE_EXTERNAL_BATTERY_2", m_auv_ciscrea->getTensionBatterie2());
		m_Comms.Notify("VVV_CONSUMPTION_EXTERNAL_BATTERY_1", m_auv_ciscrea->getConsommationBatterie1());
		m_Comms.Notify("VVV_CONSUMPTION_EXTERNAL_BATTERY_2", m_auv_ciscrea->getConsommationBatterie2());
	}
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Ciscrea::OnStartUp()
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
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void Ciscrea::RegisterVariables()
{
	// Variables liées à la commande de l'AUV
	for(int i = 0 ; i < (int)this->m_listeVariablesSuivies.size() ; i++)
	{
		m_Comms.Register(this->m_listeVariablesSuivies[i], 0);
		cout << "Suivi de la variable MOOS \"" << this->m_listeVariablesSuivies[i] << "\"..." << endl;
	}
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Ciscrea::~Ciscrea()
{
	
}
