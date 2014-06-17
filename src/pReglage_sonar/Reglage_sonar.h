/************************************************************/
/*    NAME: QJ                                              */
/*    ORGN: MIT                                             */
/*    FILE: Reglage_sonar.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Reglage_sonar_HEADER
#define Reglage_sonar_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class Reglage_sonar : public CMOOSApp
{
 public:
   Reglage_sonar();
   ~Reglage_sonar();
   void getSize(std::string data, std::string *scanline, float *heading, float * dist, int *size);
   void getData(std::string scanline, unsigned int *raw);

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
};

#endif 
