/************************************************************/
/*    NAME: QJ                                              */
/*    ORGN: MIT                                             */
/*    FILE: Reglage_sonar.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <math.h>
#include "MBUtils.h"
#include "Reglage_sonar.h"
#include "sonar.h"

#define ANGLE M_PI/2

int neue = 1;
float cap = 0;

using namespace std;


void Reglage_sonar::getSize(std::string data, std::string *scanline, float *heading, float * dist, int *size){
  std::string b = "bearing=";
  std::string a = "ad_interval=";
  std::string s = "scanline=";

  size_t bs = data.find(b);
  size_t as = data.find(a);
  size_t ss = data.find(s);


  std::string bearing = data.substr(bs + b.size(), as - (bs + b.size())-1);
  std::string ad_interval = data.substr(as + a.size(), ss - (as + a.size())-1);
  scanline->assign(data.substr(ss + s.size(), data.size() - (ss + s.size()) -1));

  size_t xs = scanline->find("x");

  *size = atoi(scanline->substr(1,xs-1).c_str());

  size_t vs = scanline->find("{");

  scanline->assign(scanline->substr(vs + 1, scanline->size()));

  *heading = atof(bearing.c_str());
  *dist = atof(ad_interval.c_str());
}

void Reglage_sonar::getData(std::string scanline, unsigned int *raw){

  const char *deb = scanline.c_str();
  char val[] = "\0\0\0\0";
  int i = 0;
  int j = 0;
  while( *deb !='\0'){
    if(*deb !=',' & j < 4){
      val[j] = *deb;
      j++;
    }
    else{
      //printf("taille %d val %s\n",j,val);
      j = 0;
      raw[i] = atoi(val);
      val[0] = '\0';val[1] = '\0';val[2] = '\0';val[3] = '\0';
      i++;
    }
    deb++;
  }

}

//---------------------------------------------------------
// Constructor

Reglage_sonar::Reglage_sonar()
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

Reglage_sonar::~Reglage_sonar()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Reglage_sonar::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if(msg.GetKey() == "SONAR_RAW_DATA"){
      // on recupère les données
    
      string data  = msg.GetString();
      float heading, dist;
      int taille_vec;
      std::string scanline;
      getSize(data, &scanline, &heading, &dist, &taille_vec);

      unsigned int raw[taille_vec];
      getData(scanline, raw);

      int ind;
      Sonar::getIndMax(raw, taille_vec, &ind);

      dist = ind * dist;

      printf("distance au mur : %f, pour un cap de %f %f et n %d\n", dist, heading, cap, taille_vec);

      if(neue){          
        std::ostringstream param_sonar_stream;
        float left = 0;/*ANGLE - M_PI/20;
        if(left < 0){
          left = left * 180/M_PI + 360;
        }
        else{
          left = left * 180/M_PI;
        }*/
        float right = 359;/*ANGLE + M_PI/20;
        if(right < 0){
          right = right * 180/M_PI + 360;
        }
        else{
          right = right * 180/M_PI;
        }*/
/*
        param_sonar_stream << "LeftLimit=" << left << ",RightLimit=" << right;
        std::string param_sonar = param_sonar_stream.str();
        m_Comms.Notify("SONAR_PARAMS", param_sonar);
*/
	m_Comms.Notify("SONAR_PARAMS", "Continuous=true");
        neue = 0;
      }

    }
    if(msg.GetKey() == "VVV_HEADING"){
      cap = msg.GetDouble();
    }

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

bool Reglage_sonar::OnConnectToServer()
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

bool Reglage_sonar::Iterate()
{
  m_iterations++;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Reglage_sonar::OnStartUp()
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

void Reglage_sonar::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register("SONAR_RAW_DATA", 0);  
}

