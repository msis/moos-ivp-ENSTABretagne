/**
 * \file Modem.cpp
 * \brief Classe Modem
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS permettant le dialogue entre les AUV par modem
 *
 */
 
#include <time.h>
#include <iterator>
#include "MBUtils.h"
#include "Modem.h"
#include "Communication.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Modem::Modem(string nomPortSerie, bool initialisationAutomatique)
{
	this->m_nom_port = nomPortSerie;
	this->m_iterations = 0;
	this->m_timewarp   = 1;
	this->m_anomalie_detectee = false;
	this->m_position_x_anomalie = -1;
	this->m_position_y_anomalie = -1;
	this->m_position_z_anomalie = -1;
	this->m_position_x_anomalie_recue = false;
	this->m_position_y_anomalie_recue = false;
	this->m_position_z_anomalie_recue = false;
	
	// Enregistrement des variables de la MOOSDB à suivre
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_DETECTED");
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_X");
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_Y");
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_Z");
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_STATE");
	
	if(initialisationAutomatique && initialiserPortSerie())
		cout << "Port série initialisé !" << endl;
}

/**
 * \fn
 * \brief Méthode initialisant le port série
 */
 
bool Modem::initialiserPortSerie()
{
	int baud = 9600;
	
	// Instanciation de l'objet de communication avec le port série
	cout << "Initialisation de \"" << this->m_nom_port << "\" (" << baud << ")" << endl;
	this->m_moos_serial_port = CMOOSLinuxSerialPort();
	return this->m_moos_serial_port.Create((char*)this->m_nom_port.c_str(), baud);
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Modem::~Modem()
{
	this->m_moos_serial_port.Close();
}

/**
 * \fn
 * \brief Méthode envoyant un message par modem
 */

bool Modem::envoyerMessage(char* message)
{
	char reponse_ascii[4];
	ConversionsBinaireASCII::binaryToAscii(message, reponse_ascii);
	cout << "Envoi du message : \"" << reponse_ascii << "\"" << endl;
	return this->m_moos_serial_port.Write(reponse_ascii, 4);
}

/**
 * \fn
 * \brief Méthode envoyant un message par modem
 */

bool Modem::confirmationBonneReception()
{
	time_t temps_debut_reception, temps_actuel;
	time(&temps_debut_reception);
	bool confirmation = false;
	int type_message, data;
	char reponse_captee[NOMBRE_BITS_TOTAL];
	
	/*do
	{
		this->m_moos_serial_port.ReadNWithTimeOut(reponse_captee, NOMBRE_BITS_TOTAL);
		//sprintf(reponse_captee, "00000000000000000000000000000000"); // Réponse type de confirmation
		
		if(!Communication::decoderMessage(reponse_captee, &type_message, &data))
			continue;
		
		confirmation = (type_message == TYPE_AUTRE_CONFIRMATION_RECEPTION);
		time(&temps_actuel);
	} while(!confirmation && difftime(temps_actuel, temps_debut_reception) < 2);
	*/
	return confirmation;
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Modem::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;

		if(msg.GetKey() == "VVV_ANOMALY_DETECTED")
			this->m_anomalie_detectee = (msg.GetDouble() == 1.0);

		if(msg.GetKey() == "VVV_ANOMALY_X")
		{
			this->m_position_x_anomalie = msg.GetDouble();
			this->m_position_x_anomalie_recue = false;
		}

		if(msg.GetKey() == "VVV_ANOMALY_Y")
		{
			this->m_position_y_anomalie = msg.GetDouble();
			this->m_position_y_anomalie_recue = false;
		}

		if(msg.GetKey() == "VVV_ANOMALY_Z")
		{
			this->m_position_z_anomalie = msg.GetDouble();
			this->m_position_z_anomalie_recue = false;
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
 
bool Modem::OnConnectToServer()
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
 
bool Modem::Iterate()
{
	m_iterations++;
	
	if(this->m_anomalie_detectee)
	{
		char message[NOMBRE_BITS_TOTAL];
		
		// Information sur X
		if(!this->m_position_x_anomalie_recue)
		{
			cout << "Envoie de la position X de l'anomalie" << endl;
			Communication::encoderMesureExterneX(this->m_position_x_anomalie, message);
			this->envoyerMessage(message);
			/*if(this->envoyerMessage(message))
				this->m_position_x_anomalie_recue = this->confirmationBonneReception();
		}
		
		// Information sur Y
		if(!this->m_position_y_anomalie_recue)
		{
			cout << "Envoie de la position Y de l'anomalie" << endl;
			Communication::encoderMesureExterneY(this->m_position_y_anomalie, message);
			if(this->envoyerMessage(message))
				this->m_position_y_anomalie_recue = this->confirmationBonneReception();
		}
		
		// Information sur Z
		if(!this->m_position_z_anomalie_recue)
		{
			cout << "Envoie de la position Z de l'anomalie" << endl;
			Communication::encoderMesureExterneZ(this->m_position_z_anomalie, message);
			if(this->envoyerMessage(message))
				this->m_position_z_anomalie_recue = this->confirmationBonneReception();*/
				this->m_position_x_anomalie_recue = true;
		}
	}
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Modem::OnStartUp()
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
 
void Modem::RegisterVariables()
{
	// Variables liées à la commande de l'AUV
	for(int i = 0 ; i < (int)this->m_listeVariablesSuivies.size() ; i++)
		m_Comms.Register(this->m_listeVariablesSuivies[i], 0);
}
