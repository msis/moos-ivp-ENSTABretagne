/************************************************************/
/*    NAME: QJ                                              */
/*    ORGN: MIT                                             */
/*    FILE: Move.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Move.h"
#include "autom.h"

using namespace std;

void Move::myNotify(string key, double arg){
  m_Comms.Notify(key,arg);
}

void Move::myNotify(string key, string arg){
  m_Comms.Notify(key,arg);
}

//---------------------------------------------------------
// Constructor

Move::Move()
{
  m_iterations = 0;
  m_timewarp   = 1;
  state_machine.get_State()->set_app((void *)this);
  cout << "objet Move bien créer" << endl;
}

//---------------------------------------------------------
// Destructor

Move::~Move()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Move::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    state_machine.onNewMes(msg);
    
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
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Move::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
  RegisterVariables();

  //this->myNotify("DO_LOC",0.0);
  state_machine.doing();
  // state_machine.transition();
  // state_machine.doing();
  printf("on connect to serveur finie \n");

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Move::Iterate()
{
  m_iterations++;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Move::OnStartUp()
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

void Move::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register("SONAR_RAW_DATA", 0);
  m_Comms.Register("VVV_HEADING", 0);
  m_Comms.Register("LOCALISATIONX", 0);
  m_Comms.Register("LOCALISATIONY", 0);
  m_Comms.Register("X2REACH", 0);
  m_Comms.Register("Y2REACH", 0);
}

