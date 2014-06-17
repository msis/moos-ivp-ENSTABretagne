/************************************************************/
/*    NAME: QJ                                              */
/*    ORGN: MIT                                             */
/*    FILE: Simulateur.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Simulateur_HEADER
#define Simulateur_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class Simulateur : public CMOOSApp
{
 public:
   Simulateur();
   ~Simulateur();

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

   float x, y, vx, vy, tt, ts, tmax, tmin;
};

#endif 
