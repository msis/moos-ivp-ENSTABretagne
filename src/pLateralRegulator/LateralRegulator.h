/**
 * \file LateralRegulator.h
 * \brief Classe LateralRegulator
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 24th 2013
 *
 * Application MOOS régulant la profondeur et le cap
 *
 */

#ifndef LateralRegulator_HEADER
#define LateralRegulator_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

#include "ScalarPID.h"

using namespace std;

class LateralRegulator : public CMOOSApp
{
	public:
		LateralRegulator();
		~LateralRegulator();

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
                
        ScalarPID m_pid;
        
        double m_desired_dist;
	
        double m_input_filter_min, m_input_filter_max;

        
        // To be removed
        double kp, ki, kd;
        double out_lim, int_lim;
	
	string output_var;
	string input_var;
	string target_var;
	
	bool is_active;
	string activate_var;
};

#endif 