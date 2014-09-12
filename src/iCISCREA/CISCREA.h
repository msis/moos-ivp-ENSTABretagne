/************************************************************/
/*    NAME: MSIS                                              */
/*    ORGN: MIT                                             */
/*    FILE: CISCREA.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef CISCREA_HEADER
#define CISCREA_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class CISCREA : public AppCastingMOOSApp
{
 public:
   CISCREA();
   ~CISCREA() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables

 private: // State variables
};

#endif 
