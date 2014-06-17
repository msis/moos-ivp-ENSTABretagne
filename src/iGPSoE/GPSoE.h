/**
 * \file GPSoE.h
 * \brief Classe GPSoE
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Application MOOS donnant les consignes de déplacement de l'AUV
 *
 */

#ifndef GPSoE_HEADER
#define GPSoE_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

#include "GPSoEUtils.h"

using namespace std;

class GPSoE : public CMOOSApp
{
	public:
		GPSoE();
		~GPSoE();

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
		
		HGPSOEMGR hGPSoEMgr;
};

#endif 
