/**
 * \file InertialMeasurementUnit.cpp
 * \brief Classe InertialMeasurementUnit
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 * 
 * Récupération des informations physiques du mouvement de l'AUV via la centrale inertielle
 */

#include <iterator>
#include "MBUtils.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "InertialMeasurementUnit.h"

using namespace std;
const string serial_port_name = "/dev/ttyUSB0";

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
InertialMeasurementUnit::InertialMeasurementUnit()
{
	m_iterations = 0;
	m_timewarp   = 1;
	this->m_rx	 = 0;
	this->m_ry	 = 0;
	this->m_rz	 = 0;
	this->m_vrx	 = 0;
	this->m_vry	 = 0;
	this->m_vrz	 = 0;
	this->m_rx	 = 0;
	this->m_ry	 = 0;
	this->m_rz	 = 0;

	try
	{
		cout << "Initialisation de Razor..." << endl;
		razor = new RazorAHRS(serial_port_name, 
								bind(&InertialMeasurementUnit::on_data, this, placeholders::_1),
								bind(&InertialMeasurementUnit::on_error, this, placeholders::_1),
								RazorAHRS::ACC_MAG_GYR_CALIBRATED);
	}
	
	catch(runtime_error &e)
	{
		cout << "  " << (string("Could not create tracker: ") + string(e.what())) << endl;
		cout << "  " << "Did you set a correct serial port ? (check permissions root)" << endl;
	}
}

/**
 * \fn
 * \brief Callback : lorsqu'il y a erreur à la réception de données
 */
 
void InertialMeasurementUnit::on_error(const string &msg)
{
	cout << "  " << "Erreur: " << msg << endl;
}

/**
 * \fn
 * \brief Callback : lorsqu'il y a réception de données
 */
 
void InertialMeasurementUnit::on_data(const float data[])
{
	this->m_rz = data[2];
	this->m_ry = data[1];
	this->m_rx = data[0];
	
	/* TO DO :
	this->m_az = data[3];
	this->m_ay = data[4];
	this->m_ax = data[5];
	
	this->m_vrz = data[6];
	this->m_vry = data[7];
	this->m_vrx = data[8];*/
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

InertialMeasurementUnit::~InertialMeasurementUnit()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool InertialMeasurementUnit::OnNewMail(MOOSMSG_LIST &NewMail)
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
 
bool InertialMeasurementUnit::OnConnectToServer()
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
 
bool InertialMeasurementUnit::Iterate()
{
	m_iterations++;
	cout.precision(4);
	
	// 3 champs : accelerometre, magnetometre et gyroscope
	
	// Boussole : angle de rotation
	cout << "Magnetometre\t RX : " << this->m_rx << "\t RY : " << this->m_ry << "\t RZ : " << this->m_rz << endl;
	
	m_Comms.Notify("VVV_NAV_RX", this->m_rx);
	m_Comms.Notify("VVV_NAV_RY", this->m_ry);
	m_Comms.Notify("VVV_NAV_RZ", this->m_rz);
	
	/* TO DO :
	// Accélération
	cout << "Accelerometre\t AX : " << this->m_ax << "\t AY : " << this->m_ay << "\t AZ : " << this->m_az << endl;
	
	m_Comms.Notify("VVV_NAV_AX", this->m_ax);
	m_Comms.Notify("VVV_NAV_AY", this->m_ay);
	m_Comms.Notify("VVV_NAV_AZ", this->m_az);
	
	// Vitesse angulaire
	cout << "Gyroscope\t VRX : " << this->m_vrx << "\t VRY : " << this->m_vry << "\t VRZ : " << this->m_vrz << endl << endl;
	
	m_Comms.Notify("VVV_NAV_VRX", this->m_vrx);
	m_Comms.Notify("VVV_NAV_VRY", this->m_vry);
	m_Comms.Notify("VVV_NAV_VRZ", this->m_vrz);*/
		
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool InertialMeasurementUnit::OnStartUp()
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
 
void InertialMeasurementUnit::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
}
