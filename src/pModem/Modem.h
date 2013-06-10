/**
 * \file Modem.h
 * \brief Classe Modem
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS permettant le dialogue entre les AUV par modem
 *
 */

#ifndef Modem_HEADER
#define Modem_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class Modem : public CMOOSApp
{
	public:
		Modem();
		~Modem();

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