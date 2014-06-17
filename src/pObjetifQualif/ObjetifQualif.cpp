/************************************************************/
/*    NAME: MSIS                                              */
/*    ORGN: MIT                                             */
/*    FILE: ObjetifQualif.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ObjetifQualif.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ObjetifQualif::ObjetifQualif()
{
  m_iterations = 0;
  m_timewarp   = 1;

  m_vyP	= 1.0;
  m_vxP = 1.0;
  m_vxMax = 50.0;
  m_vyMax = 20.0;
  m_axisXangleN = 45.0;
  m_axisXangleS = -100.0;
  m_axisYangleW = -30;
  m_lowerX = 23.5;
  m_upperX = 26.5;
  m_upperY = 30.0;
  m_distEcho = 2.0;

  m_inLine = false;
  m_crossed = false;
  m_useEcho = false;
  m_done = false;
}

//---------------------------------------------------------
// Destructor

ObjetifQualif::~ObjetifQualif()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ObjetifQualif::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
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
    if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
    else if(MOOSStrCmp(key, "P_VX"))
    {
      m_vxP = GetDouble();
    }
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ObjetifQualif::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool ObjetifQualif::Iterate()
{
  m_iterations++;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool ObjetifQualif::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      
      if(param == "FOO") {
        //handled
      }
      else if(param == "BAR") {
        //handled
      }
    }
  }
  
  m_timewarp = GetMOOSTimeWarp();

  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void ObjetifQualif::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register("START",0);
  m_Comms.Register("SURFACE",0);
  m_Comms.Register("VVV_X",0);
  m_Comms.Register("VVV_Y",0);

  //Configuration
  m_Comms.Register("VX_MAX",0);
  m_Comms.Register("VY_MAX",0);
  m_Comms.Register("H_NORTH",0);
  m_Comms.Register("H_SOUTH",0);
  m_Comms.Register("H_WEST",0);
  m_Comms.Register("P_VY",0);
  m_Comms.Register("P_VX",0);
  m_Comms.Register("LOWER_X",0);
  m_Comms.Register("UPPER_X",0);
  m_Comms.Register("UPPER_Y",0);
}

