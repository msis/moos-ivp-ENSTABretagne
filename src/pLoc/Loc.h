/************************************************************/
/*    NAME: QJ                                              */
/*    ORGN: MIT                                             */
/*    FILE: Loc.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Loc_HEADER
#define Loc_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class Loc : public CMOOSApp
{
 public:
   Loc();
   ~Loc();
   void getData(std::string scanline, unsigned int *raw);
   void getSize(std::string data, std::string *scanline, float *heading, float * dist, int *size);

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

   char loc;
   int size;
   float *rho;
   float *theta;

   float cap;
};

#endif 
