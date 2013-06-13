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
#include "ColorParse.h"
#include "MBUtils.h"
#include "Modem.h"
#include "Communication.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Modem::Modem()
{
	this->m_iterations = 0;
	this->m_timewarp   = 1;
	this->m_anomalie_detectee = false;
	this->m_position_x_anomalie = -1;
	this->m_position_y_anomalie = -1;
	this->m_position_z_anomalie = -1;
	this->m_position_x_anomalie_recue = false;
	this->m_position_y_anomalie_recue = false;
	this->m_position_z_anomalie_recue = false;
	this->m_mode_emetteur = true;
	
	// Enregistrement des variables de la MOOSDB à suivre
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_DETECTED");
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_X");
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_Y");
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_Z");
	this->m_listeVariablesSuivies.push_back("VVV_ANOMALY_STATE");
}

/**
 * \fn
 * \brief Méthode initialisant le port série
 */
 
bool Modem::initialiserPortSerie(string nom_port)
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
	cout << "Envoi du message : \"" << message << "\" [" << reponse_ascii << "]" << endl;
	return this->m_moos_serial_port.Write(reponse_ascii, 4);
}

/**
 * \fn
 * \brief Méthode recevant un message par modem
 */

bool Modem::recevoirMessage(char* resultat_binaire)
{
	char reponse_ascii[32];
	
	if(this->m_moos_serial_port.ReadNWithTimeOut(reponse_ascii, 4, 3))
	{
		ConversionsBinaireASCII::asciiToBinary(reponse_ascii, resultat_binaire);
		cout << "Réception du message : \"" << resultat_binaire << "\" [" << reponse_ascii << "]" << endl;
		return true;
	}
	
	return false;
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
	int type_message, data;
	char message[NOMBRE_BITS_TOTAL];
	m_iterations++;
	
	if(this->m_mode_emetteur)
	{
		if(recevoirMessage(message))
		{
			if(!Communication::decoderMessage(message, &type_message, &data))
				cout << "Erreur dans le message" << endl;
			
			else
			{
				if(type_message == TYPE_CONFIRMATION_RECEPTION && data == 1)
				{
					this->m_position_x_anomalie_recue = true;
					this->m_position_y_anomalie_recue = true;
					this->m_position_z_anomalie_recue = true;
					cout << termColor("green") << "\tAccusé de bonne réception" << endl << endl << termColor();
				}
					
				if(type_message == TYPE_CONFIRMATION_RECEPTION && data != 1)
					cout << termColor("red") << "\tAccusé de mauvaise réception" << endl << endl << termColor();
			}
		}
		
		if(this->m_anomalie_detectee)
		{
			// Information sur X
			if(!this->m_position_x_anomalie_recue)
			{
				cout << "Envoie de la position X de l'anomalie" << endl;
				Communication::encoderMesureExterneX(this->m_position_x_anomalie, message);
				if(!this->envoyerMessage(message))
					cout << "Erreur lors de l'envoi du message" << endl;
			}
		}
	}
	
	else
	{
		bool message_valide_et_compris = false;
		
		if(recevoirMessage(message))
		{
			if(!Communication::decoderMessage(message, &type_message, &data))
				cout << "Erreur dans le message" << endl;
			
			else
			{
				switch(type_message)
				{
					case TYPE_MESURE_EXTERNE_X:
						message_valide_et_compris = true;
						cout << "Mesure externe de X : " << data << endl;
						break;
					
					case TYPE_ETAT_ANOMALIE:
						message_valide_et_compris = true;
						cout << "État de l'anomalie : ";
						if(data)
							cout << "bouée allumée" << endl;
						else
							cout << "bouée éteinte" << endl;
						break;
						
					default:
						cout << "Type : " << type_message << " - Data : " << data << endl;
				}
			}
				
			Communication::encoderConfirmationReception(message, message_valide_et_compris);
			if(!this->envoyerMessage(message))
				cout << "Erreur lors de l'envoi du message de confirmation de (mauvaise?) réception" << endl;
				
			cout << endl;
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

			if(param == "SERIAL_PORT_NAME")
			{
				if(initialiserPortSerie(value))
					cout << "Port série initialisé !" << endl;
			}

			else if(param == "EMETTEUR" && value == "true")
			{
				cout << termColor("blue") << "Configuration en mode \"émetteur\"" << termColor() << endl;
				this->m_mode_emetteur = true;
			}

			else if(param == "RECEPTEUR" && value == "true")
			{
				cout << termColor("blue") << "Configuration en mode \"récepteur\"" << termColor() << endl;
				this->m_mode_emetteur = false;
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
