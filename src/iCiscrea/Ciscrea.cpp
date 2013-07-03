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

/*
 * TO DO :
 * 
 * - Remettre automatiquement Vx, Vz, Vz, etc à 0 quand pas de réponse depuis
 * 		n secondes...
 */
 
 
/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Ciscrea::Ciscrea(int identifiant_AUV) : m_identifiant_auv_a_instancier(identifiant_AUV)
{
	this->m_iterations = 0;
	this->m_timewarp = 1;
	this->m_auv_ciscrea = NULL;
	
	// Enregistrement des variables de la MOOSDB à suivre
	this->m_listeVariablesSuivies.push_back("VVV_VX_DESIRED");
	this->m_listeVariablesSuivies.push_back("VVV_VY_DESIRED");
	this->m_listeVariablesSuivies.push_back("VVV_VZ_DESIRED");
	this->m_listeVariablesSuivies.push_back("VVV_RZ_DESIRED");
	this->m_listeVariablesSuivies.push_back("VVV_SPOTLIGHTS");
	this->m_listeVariablesSuivies.push_back("VVV_AUV_IDENTIFIER");
	this->m_listeVariablesSuivies.push_back("VVV_ON_MISSION");
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Ciscrea::OnNewMail(MOOSMSG_LIST &NewMail)
{
	bool mettre_a_jour_propulseurs = false;
	
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
		
		if(msg.GetKey() == "VVV_VX_DESIRED" && this->m_auv_ciscrea != NULL)
		{
			m_auv_ciscrea->setVx(msg.GetDouble());
			mettre_a_jour_propulseurs = true;
		}
		
		if(msg.GetKey() == "VVV_VY_DESIRED" && this->m_auv_ciscrea != NULL)
		{
			m_auv_ciscrea->setVy(msg.GetDouble());
			mettre_a_jour_propulseurs = true;
		}
		
		if(msg.GetKey() == "VVV_VZ_DESIRED" && this->m_auv_ciscrea != NULL)
		{
			m_auv_ciscrea->setVz(msg.GetDouble());
			mettre_a_jour_propulseurs = true;
		}
			
		if(msg.GetKey() == "VVV_RZ_DESIRED" && this->m_auv_ciscrea != NULL)
		{
			m_auv_ciscrea->setRz(msg.GetDouble());
			mettre_a_jour_propulseurs = true;
		}
		
		if(msg.GetKey() == "VVV_SPOTLIGHTS" && this->m_auv_ciscrea != NULL)
		{
			if(msg.GetDouble() != 0)
				m_auv_ciscrea->allumerProjecteurs(round(msg.GetDouble()));
			
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
		
	if(mettre_a_jour_propulseurs)
		m_auv_ciscrea->updatePropulseurs();

	return(true);
}

/**
 * \fn
 * \brief Méthode instanciant l'AUV
 */
 
void Ciscrea::instancierAUV()
{
	this->m_auv_ciscrea = new AUV(this->m_identifiant_auv_a_instancier);
	//this->m_auv_ciscrea->allumerProjecteurs(1000);
		
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
		if(m_iterations % 30 == 0)
		{
			m_auv_ciscrea->updateRegistresModbus();
			m_Comms.Notify("VVV_CURRENT_EXTERNAL_BATTERY_1", m_auv_ciscrea->getIntensiteBatterie1());
			m_Comms.Notify("VVV_CURRENT_EXTERNAL_BATTERY_2", m_auv_ciscrea->getIntensiteBatterie2());
			m_Comms.Notify("VVV_VOLTAGE_EXTERNAL_BATTERY_1", m_auv_ciscrea->getTensionBatterie1());
			m_Comms.Notify("VVV_VOLTAGE_EXTERNAL_BATTERY_2", m_auv_ciscrea->getTensionBatterie2());
			m_Comms.Notify("VVV_CONSUMPTION_EXTERNAL_BATTERY_1", m_auv_ciscrea->getConsommationBatterie1());
			m_Comms.Notify("VVV_CONSUMPTION_EXTERNAL_BATTERY_2", m_auv_ciscrea->getConsommationBatterie2());
		}
		
		else
			m_auv_ciscrea->updateRegistresModbusProfondeurEtCap();
		
		m_Comms.Notify("VVV_Z", (m_auv_ciscrea->getProfondeur() / 100.0) + AJUSTEMENT_CAPTEUR_Z);
		m_Comms.Notify("VVV_HEADING_CISCREA", m_auv_ciscrea->getCap());
	}
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Ciscrea::OnStartUp()
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

			if(param == "IDENTIFIANT_AUV")
			{
				this->m_identifiant_auv_a_instancier = atoi((char*)value.c_str());
			
				if(this->m_identifiant_auv_a_instancier != 0)
				{
					instancierAUV();
					//this->m_auv_ciscrea->allumerProjecteurs(1000);
					RegisterVariables();
				}
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
