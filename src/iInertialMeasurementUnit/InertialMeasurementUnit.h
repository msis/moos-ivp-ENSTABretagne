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
#include "RazorAHRS/RazorAHRS.h"

#define AJUSTEMENT_HEADING_RAZOR	320

using namespace std;
		
class InertialMeasurementUnit : public CMOOSApp
{
	public:
		InertialMeasurementUnit();
		~InertialMeasurementUnit();
		bool initialiserRazorAHRS(string serial_port_name);

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
		void on_error(const string &msg);
		void on_data(const float data[]);

	private: // Configuration variables

	private: // State variables
		unsigned int	m_iterations;
		double			m_timewarp;
		double 			m_rx, m_ry, m_rz;
		double 			m_vrx, m_vry, m_vrz;
		double 			m_ax, m_ay, m_az;
		
		RazorAHRS *razor;
};

#endif 