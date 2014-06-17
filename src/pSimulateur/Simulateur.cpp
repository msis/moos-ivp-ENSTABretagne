/************************************************************/
/*    NAME: QJ                                              */
/*    ORGN: MIT                                             */
/*    FILE: Simulateur.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Simulateur.h"
#include <math.h>
#include <stdio.h>

#define D_INTER 0.01
#define dt 0.01
#define pas M_PI/20
#define ang(x) atan2(sin(x),cos(x))

#define jsus
#ifdef jsus
  #define js printf
#else
  #define js
#endif

using namespace std;

void write_simu(float x, float y, float cap){

    FILE *f = fopen("/home/ciscrea1/x.txt","w");
    if(f!=NULL){
      fwrite(&x, sizeof(float), 1, f);
      fclose(f);
    }
    else{
      printf("impossible d'ouvrir le fichier x\n");
    }

    f = fopen("/home/ciscrea1/y.txt","w");
    if(f!=NULL){
      fwrite(&y, sizeof(float), 1, f);
      fclose(f);
    }
    else{
      printf("impossible d'ouvrir le fichier y\n");
    }


    f = fopen("/home/ciscrea1/cap.txt","w");
    if(f!=NULL){
      fwrite(&cap,sizeof(float),1,f);
      fclose(f);
    }
    else{
      printf("impossible d'ouvrir le fichier cap\n");
    }
}

//---------------------------------------------------------
// Constructor

Simulateur::Simulateur()
{
  m_iterations = 0;
  m_timewarp   = 1;


  x = 2.5;
  y = 0.5;
  vx = 0;
  vy = 0;
  tt = 0;
  ts = 0;
  tmin = -M_PI;
  tmax = M_PI;
}

//---------------------------------------------------------
// Destructor

Simulateur::~Simulateur()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Simulateur::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if(msg.GetKey() == "VVV_VX_DESIRED")
    {
      vx = msg.GetDouble();
      printf("nouvelle vitesse : %f\npos : %f %f\n",vx,x,y);
    }

    if(msg.GetKey() == "VVV_VY_DESIRED")
    {
      vy = msg.GetDouble();
    }


    if(msg.GetKey() == "VVV_HEADING_DESIRED")
    {
      tt = msg.GetDouble();
      printf("___________CAP %f°\n",tt*180/M_PI);
    }

    if(msg.GetKey() == "TTMAX")
    {
      tmax = msg.GetDouble();
      printf("TTMAX : %f\n",msg.GetDouble()*180/M_PI);
    }

    if(msg.GetKey() == "TTMIN")
    {
      tmin = msg.GetDouble();
      printf("TTMIN : %f\n",msg.GetDouble()*180/M_PI);
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

bool Simulateur::OnConnectToServer()
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

bool Simulateur::Iterate()
{
  m_iterations++;
  x = x + dt*(cos(tt)*vx + sin(tt)*vy);
  y = y + dt*(sin(tt)*vx - cos(tt)*vy);
  

  if(m_iterations % 5 == 0){
    m_Comms.Notify("VVV_HEADING",tt);

    write_simu(x, y, tt);
  }

  if(m_iterations % 10 == 0){

    float x1, x2, y1, y2, dist;

    if(fabs(tan(tt+ts)) < 0.001){
      js("1 ");
      if(cos(tt+ts) > 0){
        dist = 3 - x;
      }
      else{
        dist = x;
      }
    }
    else{
      js("2");

      float a = tan(tt + ts);
      //js("a:%f %f\n",a, tan(tt + ts));
      float b = y-a*x;
      if(cos(tt + ts) >0 && sin(tt + ts) > 0){
        x1 = 3; y1 = a*x1 + b;
        y2 = 4; x2 = (y2-b)/a;
        js(".1 ");
      }
      else if(cos(tt + ts) <0 && sin(tt + ts) > 0){
        x1 = 0; y1 = a*x1 + b;
        y2 = 4; x2 = (y2-b)/a;
        js(".2 ");
      }
      else if(cos(tt + ts) >0 && sin(tt + ts) < 0){
        x1 = 3; y1 = a*x1 + b;
        y2 = 0; x2 = (y2-b)/a;
        js(".3 ");
      }
      else{
        x1 = 0; y1 = a*x1 + b;
        y2 = 0; x2 = (y2-b)/a;
        js(".4 ");
      }

      if(x1 >=0 && x1 <= 3 && y1 >=0 && y1 <= 4){
        js("3 ");
        dist = sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1));
        js("%f %f %f %f\n",x,y,x1,y1);
      }
      else{
        dist = sqrt((x-x2)*(x-x2)+(y-y2)*(y-y2));
        js("4 ");
        js("%f %f %f %f\n",x,y,x2,y2);
      }

    }


    js("distance aux mur : %f\n",dist);

    int ind = (int)(dist*100);

    if(ind >= 500) printf("inidce : %d, distance : %f\n", ind, dist);

    int nb = 5/D_INTER;

    unsigned char tab[(int)nb];
    
    for(int i = 0; i < (int)nb; i++){
      tab[i] = 0;
    }
    tab[ind] = 100;


    std::string s;
    std::ostringstream convert;
    convert <<"bearing="<< ts <<",ad_interval="<<D_INTER<<",scanline=["<<nb<<"x1]{";
    for(int i = 0; i < nb; i++){
      convert << (int)tab[i];
      if(i < nb-1) convert << ",";
    }
    convert << "}";
    
    s = convert.str();
    
    //printf("on aurait du notifier\n");
    Notify("SONAR_RAW_DATA",s);

    // float cosmax, cosmin, sinmax, sinmin;
    // if(cos(tmax) > cos(tmin)){
    //   cosmax = cos(tmax); cosmin = cos(tmin);
    // }
    // else{
    //   cosmax = cos(tmin); cosmin = cos(tmax);
    // }
    // if(sin(tmax) > sin(tmin)){
    //   sinmax = sin(tmax); sinmin = sin(tmin);
    // }
    // else{
    //   sinmax = sin(tmin); sinmin = sin(tmax);
    // }

    // if(cos(ts+pas)<= cosmax && cos(ts+pas)>=cosmin && sin(ts+pas)<=sinmax && sin(ts+pas)>=sinmin){
    //   ts = ts + pas;
    // }
    // else{
    //   ts = tmin;
    // }

    if(ang(ts + pas) > tmax){
      if(ang(tmin + pas - tmax + ts) < tmax){
        ts =ang(tmin + pas - tmax + ts);
      }
      else{
        if(tmax > tmin){
          ts = ang((tmax + tmin)/2);
        }
        else{
          ts = ang((tmax + tmin + 2*M_PI)/2);
        }
      }
    }
    else{
      ts = ang(ts + pas);
    }

    js("nouvel angle du sonar : %f, (%f)\n_coord_ x %f y %f\n",(ts + tt)*180/M_PI, ts*180/M_PI, x, y);

  }
  return(true);

}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Simulateur::OnStartUp()
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

void Simulateur::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register("TTMAX", 0);
  m_Comms.Register("TTMIN", 0);
  m_Comms.Register("VVV_VX_DESIRED", 0);
  m_Comms.Register("VVV_VY_DESIRED", 0);
  m_Comms.Register("VVV_HEADING_DESIRED", 0);  
}

