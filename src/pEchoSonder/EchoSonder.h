/**
 * \file EchoSonder.h
 * \brief Classe EchoSonder
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données de l'écho-sondeur
 *
 */

#ifndef EchoSonder_HEADER
#define EchoSonder_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"
#ifdef _WIN32
	#include "MOOS/libMOOS/Utils/MOOSNTSerialPort.h"
#else
	#include "MOOS/libMOOS/Utils/MOOSLinuxSerialPort.h"
#endif

using namespace std;

class EchoSonder : public CMOOSApp
{
	public:
		EchoSonder(string nomPortSerie, bool initialisationAutomatique = true);
		bool initialiserPortSerie();
		~EchoSonder();

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
		string					m_nom_port;
};

#endif 