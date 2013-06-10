/**
 * \file TemperatureCPU.h
 * \brief Classe TemperatureCPU
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS surveillant l'état de chauffe du CPU
 *
 */

#ifndef TemperatureCPU_HEADER
#define TemperatureCPU_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class TemperatureCPU : public CMOOSApp
{
	public:
		TemperatureCPU();
		~TemperatureCPU();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
		double getTemperatureProcesseur();
		double getPresentRateBattery();

	private: // Configuration variables

	private: // State variables
		unsigned int	m_iterations;
		double			m_timewarp;
};

#endif 