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

using namespace std;

class EchoSonder : public CMOOSApp
{
	public:
		EchoSonder();
		~EchoSonder();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();

	private: // Configuration variables

	private: // State variables
		unsigned int	m_iterations;
		double			m_timewarp;
};

#endif 