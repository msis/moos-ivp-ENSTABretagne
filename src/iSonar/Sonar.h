/**
 * \file Sonar.h
 * \brief Classe Sonar
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données liées au sonar
 *
 */

#ifndef Sonar_HEADER
#define Sonar_HEADER

#include "../common/constantes.h"
#include "SonarDF.h"

#include "MOOS/libMOOS/App/MOOSApp.h"
#ifdef _WIN32
	#include "MOOS/libMOOS/Utils/MOOSNTSerialPort.h"
#else
	#include "MOOS/libMOOS/Utils/MOOSLinuxSerialPort.h"
#endif

using namespace std;

class Sonar : public CMOOSApp
{
	public:
		Sonar();
		bool initialiserPortSerie(string nom_port);
		~Sonar();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();

	private: // Configuration variables

	private: // State variables
		unsigned int			m_iterations;
		double					m_timewarp;
		CMOOSLinuxSerialPort	m_moos_serial_port;
		SonarDF*				m_cissonar;
};

#endif 