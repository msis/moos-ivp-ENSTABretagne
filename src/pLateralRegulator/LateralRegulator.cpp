/**
 * \file LateralRegulator.cpp
 * \brief Classe LateralRegulator
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 24th 2013
 *
 * Application MOOS régulant la profondeur et le cap
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "LateralRegulator.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
LateralRegulator::LateralRegulator()
{
	m_iterations = 0;
	m_timewarp   = 1;
        
        m_desired_dist = 10.0;
	
        m_input_filter_min = 5.0;
	m_input_filter_min = 100.0;

        // To be removed
        //double kp, ki, kd;
        out_lim = 5.0;
        int_lim = 0.0;
	
	output_var = GetAppName() + "_OUT";
	input_var = GetAppName() + "_IN";
	
	is_active = true;
	activate_var = GetAppName() + "_ACTIVE";
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

LateralRegulator::~LateralRegulator()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool LateralRegulator::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;

		#if 0 // Keep these around just for template
		string key   = msg.GetKey();
		string comm  = msg.GetCommunity();
		double dval  = msg.GetDouble();
		string sval  = msg.GetString(); 
		string msrc  = msg.GetSource();
		double mtime = msg.GetTime();
		bool   mdbl  = msg.IsDouble();
		bool   mstr  = msg.IsString();
		#endif
                
                string key   = msg.GetKey();
//cout << "K="<<key << endl;
//cout << "tv=" << target_var << endl;
//cout << "iv=" << input_var << endl;
                if (key == target_var && msg.IsDouble()) {
                  m_desired_dist = msg.GetDouble();
                  m_pid.SetGoal(msg.GetDouble());
                }
                
                if (key == input_var && msg.IsDouble() && is_active) {
		  if (msg.GetDouble() >= m_input_filter_min
		      && msg.GetDouble() <= m_input_filter_max) {
		    double output_value = 0.0;
		    m_pid.Run(m_desired_dist - msg.GetDouble(),msg.GetTime(), output_value);
		    m_Comms.Notify(output_var, output_value);
//cout << "ERR=" << m_desired_dist - msg.GetDouble() << "->out=" << output_value <<endl;
		  }
                }
                
                if (key == activate_var) {
		  /*if (msg.IsDouble()) {
		    is_active = (msg.GetDouble() != 0.0);
		  }
		  else if (msg.IsString())*/ {
		    if (msg.GetString() == "true")
		      is_active =true;
		    else if (msg.GetString() == "false")
		      is_active = false;
		  }
                }
                
	}
        
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool LateralRegulator::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

//	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool LateralRegulator::Iterate()
{
	m_iterations++;
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool LateralRegulator::OnStartUp()
{
/*        double kp_z = 0.0, ki_z = 0.0, kd_z = 0.0;
        double kp_heading = 0.0, ki_heading = 0.0, kd_heading = 0.0;
        
        double out_lim_z = 0.0, int_lim_z = 0.0;
        double out_lim_heading = 0.0, int_lim_heading = 0.0;
*/
	setlocale(LC_ALL, "C");
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
	{
		list<string>::iterator p;
		for(p = sParams.begin() ; p != sParams.end() ; p++)
		{
			string original_line = *p;
			string param = stripBlankEnds(toupper(biteString(*p, '=')));
			string value = stripBlankEnds(*p);

			if(param == "KP")
				kp = atof(value.c_str());
                        if(param == "KI")
                                ki = atof(value.c_str());
                        if(param == "KD")
                                kd = atof(value.c_str());
                        if(param == "INTEGRAL_LIMIT")
                                int_lim = atof(value.c_str());
                        if(param == "OUTPUT_LIMIT")
                                out_lim = atof(value.c_str());
			
                        if(param == "INPUT_FILTER_MIN")
                                m_input_filter_min = atof(value.c_str());
                        if(param == "INPUT_FILTER_MAX")
				 m_input_filter_max = atof(value.c_str());
			
			if(param == "DESIRED_DIST") {
			  m_desired_dist = atof(value.c_str());
			  m_pid.SetGoal(atof(value.c_str()));
			} 
			
			if(param == "INPUT_VAR")
                                input_var = value;
			if(param == "OUTPUT_VAR")
                                output_var = value;                        
			if(param == "DESIRED_DIST_VAR")
                                target_var = value;
			if(param == "ACTIVATE_VAR")
                                activate_var = value;
			if(param == "ACTIVE") {
			    if (value == "true")
                                is_active = true;
			    else if (value == "false")
			      is_active = false;
			    else
			      is_active = (atoi(value.c_str()) != 0);
			}
		 
		}
	}
        
        m_pid.SetGains(kp, kd, ki);
        m_pid.SetLimits(int_lim, out_lim); 

	m_timewarp = GetMOOSTimeWarp();

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void LateralRegulator::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
        m_Comms.Register(input_var, 0);
        m_Comms.Register(output_var, 0);
        
        m_Comms.Register(target_var, 0);
        m_Comms.Register(activate_var, 0);
}
