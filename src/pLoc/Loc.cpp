/************************************************************/
/*    NAME: QJ                                              */
/*    ORGN: MIT                                             */
/*    FILE: Loc.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <string> 
#include <iterator>
#include <math.h>
#include "MBUtils.h"
#include "Loc.h"
#include "sonar.h"
#include "localisation.h"

#undef SIMU

#define nombreDeMesureParTour 200

using namespace std;

PARAM param;

void reglage_venant_sonar(float *heading){
  *heading = *heading*M_PI/180 + M_PI;
}

void reglage_venant_imu(float *heading){
  *heading = -*heading*M_PI/180 + M_PI;
}

void Loc::getSize(std::string data, std::string *scanline, float *heading, float * dist, int *size){
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
  reglage_venant_sonar(heading);
  *dist = atof(ad_interval.c_str())/2;
}

void Loc::getData(std::string scanline, unsigned int *raw){

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

Loc::Loc()
{
  m_iterations = 0;
  m_timewarp   = 1;

  size = 0;

  rho = (float *)malloc(nombreDeMesureParTour*sizeof(float));
  theta = (float *)malloc(nombreDeMesureParTour*sizeof(float));

  param.xmax = 3;
  param.ymax = 4;
  param.step = 4;
  param.id = 0;
}

//---------------------------------------------------------
// Destructor

Loc::~Loc()
{
  free(rho);
  free(theta);
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Loc::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if(msg.GetKey() == "SONAR_RAW_DATA")
    {
      if(loc == 'l'){
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
        
        rho[size] = ind*dist;
        theta[size] = heading + cap;
        size ++;
        if(size == nombreDeMesureParTour){
          
          POS pos;
          Sonar::localisations(rho, theta, size, &param, &pos, 0.0, HOUGH);
          printf("%f %f cap : %f\n",pos.x,pos.y, cap);
          Notify("LOCALISATIONX", pos.x);
          Notify("LOCALISATIONY", pos.y);
#ifdef SIMU
#else
          Notify("SONAR_PARAMS", "Continuous=false");
#endif
          loc = 'n';
          size = 0;
        } 
      }
    }

    if(msg.GetKey() == "VVV_HEADING"){
      // recuperer le cap et le mettre à jour
      cap = msg.GetDouble();
      reglage_venant_imu(&cap);
    }

    if(msg.GetKey() == "DO_LOC"){
      loc = 'l';
      printf("On fait la %cocalisation\n",loc);
#ifdef SIMU
      Notify("TTMAX",M_PI);
      Notify("TTMIN",-M_PI);
      // Notify au iSonar avec paramètre
#else

      Notify("SONAR_PARAMS", "Continuous=true");

#endif

    }

    if(msg.GetKey() == "STOP_LOC"){
      loc = 'n';
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

bool Loc::OnConnectToServer()
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

bool Loc::Iterate()
{
  m_iterations++;
  //OnNewMail();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Loc::OnStartUp()
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

void Loc::RegisterVariables()
{
    m_Comms.Register("DO_LOC", 0);
    m_Comms.Register("STOP_LOC", 0);
    m_Comms.Register("VVV_HEADING", 0);
    m_Comms.Register("SONAR_RAW_DATA", 0);
}

