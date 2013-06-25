/**
 * \file Regulator.h
 * \brief Classe Regulator
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 24th 2013
 *
 * Application MOOS régulant la profondeur et le cap
 *
 */

#ifndef Regulator_HEADER
#define Regulator_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

#include "ScalarPID.h"

using namespace std;

class Regulator : public CMOOSApp
{
	public:
		Regulator();
		~Regulator();

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
                
        ScalarPID m_pid_heading;
        ScalarPID m_pid_z;
        
        double m_desired_heading;
        double m_desired_z;
        
        // To be removed
        double kp_z, ki_z, kd_z;
        double kp_heading, ki_heading, kd_heading;
        double out_lim_z, int_lim_z;
        double out_lim_heading, int_lim_heading;
};

#endif 