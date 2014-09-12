/************************************************************/
/*    NAME: MSIS                                              */
/*    ORGN: MIT                                             */
/*    FILE: CISCREA.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "CISCREA.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CISCREA::CISCREA()
{
  m_serial_port = "/dev/ttyUSB0";

  m_frri_lo_dz = 100;
  m_reri_lo_dz = 100;
  m_frle_lo_dz = 100;
  m_rele_lo_dz = 100;
  m_vert_lo_dz = 100;

  m_frri_up_dz = 130;
  m_reri_up_dz = 130;
  m_frle_up_dz = 130;
  m_rele_up_dz = 130;
  m_vert_up_dz = 130;

  m_frri_sens = 1;
  m_reri_sens = 1;
  m_frle_sens = 1;
  m_rele_sens = 1;
  m_vert_sens = 1;
  
  m_desired_thrust=0;
  m_desired_rudder=0;
  m_desired_slide=0;
  m_desired_elevator=0;
  m_front_lights=0;

}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CISCREA::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    double dval  = msg.GetDouble();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "DESIRED_THRUST") 
      m_desired_thrust = dval;
    else if (key == "DESIRED_SLIDE")
      m_desired_slide = dval;
    else if (key == "DESIRED_RUDDER")
      m_desired_rudder = dval;
    else if (key == "DESIRED_ELEVATOR")
      m_desired_elevator = dval;
    else if (key == "FRONT_LIGHTS")
      m_front_lights = dval;


     else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CISCREA::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CISCREA::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CISCREA::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "SERIAL_PORT") {
      m_serial_port = value;
      handled = true;
    }
    else if(param == "MAX_MODBUS_RETRIES") {
      m_max_modbus_retries = atoi(value.c_str());
      handled = true;
    }
    else if(param == "MAX_TURNON_RETRIES") {
      m_max_turn_on_retries = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRRI_LO_DZ") {
      m_frri_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RERI_LO_DZ") {
      m_reri_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RELE_LO_DZ") {
      m_rele_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRLE_LO_DZ") {
      m_frle_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "VERT_LO_DZ") {
      m_vert_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRRI_UP_DZ") {
      m_frri_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RERI_UP_DZ") {
      m_reri_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RELE_UP_DZ") {
      m_rele_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRLE_UP_DZ") {
      m_frle_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "VERT_UP_DZ") {
      m_vert_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRRI_SENS") {
      m_frri_sens = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RERI_SENS") {
      m_reri_sens = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RELE_SENS") {
      m_rele_sens = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRLE_SENS") {
      m_frle_sens = atoi(value.c_str());
      handled = true;
    }
    else if(param == "VERT_SENS") {
      m_vert_sens = atoi(value.c_str());
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  Notify("CISCREA_ON", "false");
  registerVariables();	
  // return(true);
  return connectToAUV();
}

//---------------------------------------------------------
// Procedure: registerVariables

void CISCREA::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("DESIRED_THRUST",0);
  Register("DESIRED_SLIDE",0);
  Register("DESIRED_RUDDER",0);
  Register("DESIRED_ELEVATOR",0);
  Register("FRONT_LIGHTS",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool CISCREA::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}

bool CISCREA::connectToAUV()
{
  m_ciscrea = new M6dbus(m_serial_port);
  m_ciscrea->setMaxRetries(m_max_modbus_retries);
  bool on = false;
  for (int i = 0;i < m_max_turn_on_retries;++i)
    if (m_ciscrea->getOn())
    {
      on = true;
      Notify("CISCREA_ON", "true");
      break;
    }
  return on;
}


