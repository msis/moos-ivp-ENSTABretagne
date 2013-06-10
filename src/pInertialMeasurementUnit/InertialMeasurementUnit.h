/**
 * \file InertialMeasurementUnit.h
 * \brief Classe InertialMeasurementUnit
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 * 
 * Récupération des informations physiques du mouvement de l'AUV via la centrale inertielle
 */

#ifndef InertialMeasurementUnit_HEADER
#define InertialMeasurementUnit_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class InertialMeasurementUnit : public CMOOSApp
{
	public:
		InertialMeasurementUnit();
		~InertialMeasurementUnit();

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