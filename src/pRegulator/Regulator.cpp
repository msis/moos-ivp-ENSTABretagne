/**
 * \file Regulator.cpp
 * \brief Classe Regulator
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 24th 2013
 *
 * Application MOOS régulant la profondeur et le cap
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "Regulator.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Regulator::Regulator()
{
	m_iterations = 0;
	m_timewarp   = 1;
        
        m_desired_heading = 0.0;
        m_desired_z = 0.0;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Regulator::~Regulator()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Regulator::OnNewMail(MOOSMSG_LIST &NewMail)
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
                if (key == "VVV_Z_DESIRED" && msg.IsDouble()) {
                  m_desired_z = msg.GetDouble();
                  m_pid_z.SetGoal(msg.GetDouble());
                }
                if (key == "VVV_Z" && msg.IsDouble()) {
                  double output_vz = 0.0;
                  m_pid_z.Run(m_desired_z - msg.GetDouble(),msg.GetTime(), output_vz);
                  m_Comms.Notify("VVV_VZ_DESIRED", output_vz);
                }
                
                if (key == "VVV_HEADING_DESIRED" && msg.IsDouble()) {
                  m_desired_heading = msg.GetDouble();
                  m_pid_heading.SetGoal(msg.GetDouble());
                }
                if (key == "VVV_HEADING" && msg.IsDouble()) {
                  double output_rz = 0.0;
                  double err_heading = m_desired_heading - msg.GetDouble();
                  while (err_heading < -180.) err_heading += 360.0;
                  while (err_heading >= 180.) err_heading -= 360.0;
                 // cout << "Heading error = " << err_heading << endl;
                  m_pid_heading.Run(err_heading, msg.GetTime(), output_rz);
                  m_Comms.Notify("VVV_RZ_DESIRED", output_rz);
                }
                
                
                
                if(key == "KP_Z")
                        kp_z = msg.GetDouble();
                if(key == "KI_Z")
                        ki_z = msg.GetDouble();
                if(key == "KD_Z")
                        kd_z = msg.GetDouble();
                if(key == "INTEGRAL_LIMIT_Z")
                        int_lim_z = msg.GetDouble();
                if(key == "OUTPUT_LIMIT_Z")
                        out_lim_z = msg.GetDouble();
                                
                if(key == "KP_HEADING")
                        kp_heading = msg.GetDouble();
                if(key == "KI_HEADING")
                        ki_heading = msg.GetDouble();
                if(key == "KD_HEADING")
                        kd_heading = msg.GetDouble();
                if(key == "INTEGRAL_LIMIT_HEADING")
                        int_lim_heading = msg.GetDouble();
                if(key == "OUTPUT_LIMIT_HEADING")
                        out_lim_heading = msg.GetDouble();
                        
                            
                m_pid_z.SetGains(kp_z, kd_z, ki_z);
                m_pid_z.SetLimits(int_lim_z, out_lim_z); 
                
                m_pid_heading.SetGains(kp_heading, kd_heading, ki_heading);
                m_pid_heading.SetLimits(int_lim_heading, out_lim_heading); 
                
	}
        
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool Regulator::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool Regulator::Iterate()
{
	m_iterations++;
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Regulator::OnStartUp()
{
/*        double kp_z = 0.0, ki_z = 0.0, kd_z = 0.0;
        double kp_heading = 0.0, ki_heading = 0.0, kd_heading = 0.0;
        
        double out_lim_z = 0.0, int_lim_z = 0.0;
        double out_lim_heading = 0.0, int_lim_heading = 0.0;
*/        
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

			if(param == "KP_Z")
				kp_z = atof(value.c_str());
                        if(param == "KI_Z")
                                ki_z = atof(value.c_str());
                        if(param == "KD_Z")
                                kd_z = atof(value.c_str());
                        if(param == "INTEGRAL_LIMIT_Z")
                                int_lim_z = atof(value.c_str());
                        if(param == "OUTPUT_LIMIT_Z")
                                out_lim_z = atof(value.c_str());
                        
                        if(param == "KP_HEADING")
                                kp_heading = atof(value.c_str());
                        if(param == "KI_HEADING")
                                ki_heading = atof(value.c_str());
                        if(param == "KD_HEADING")
                                kd_heading = atof(value.c_str());
                        if(param == "INTEGRAL_LIMIT_HEADING")
                                int_lim_heading = atof(value.c_str());
                        if(param == "OUTPUT_LIMIT_HEADING")
                                out_lim_heading = atof(value.c_str());
		}
	}
        
        m_pid_z.SetGains(kp_z, kd_z, ki_z);
        m_pid_z.SetLimits(int_lim_z, out_lim_z); 
        
        m_pid_heading.SetGains(kp_heading, kd_heading, ki_heading);
        m_pid_heading.SetLimits(int_lim_heading, out_lim_heading); 

	m_timewarp = GetMOOSTimeWarp();

	RegisterVariables();	
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void Regulator::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
        m_Comms.Register("VVV_HEADING", 0);
        m_Comms.Register("VVV_HEADING_DESIRED", 0);
        
        m_Comms.Register("VVV_Z", 0);
        m_Comms.Register("VVV_Z_DESIRED", 0);
        
        
                m_Comms.Register("KP_Z",0);
                m_Comms.Register( "KI_Z",0);
                m_Comms.Register("KD_Z",0);
                m_Comms.Register( "INTEGRAL_LIMIT_Z",0);
                m_Comms.Register( "OUTPUT_LIMIT_Z",0);
                m_Comms.Register("KP_HEADING",0);
                m_Comms.Register( "KI_HEADING",0);
                m_Comms.Register( "KD_HEADING",0);
                m_Comms.Register( "INTEGRAL_LIMIT_HEADING",0);
                m_Comms.Register( "OUTPUT_LIMIT_HEADING",0);
}
