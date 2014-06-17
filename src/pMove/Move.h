/************************************************************/
/*    NAME: QJ                                              */
/*    ORGN: MIT                                             */
/*    FILE: Move.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Move_HEADER
#define Move_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "autom.h"
#include "IMove.h"

class Move : public IMove // herie aussi de IMove
{
 public:
   Move();
   ~Move();
   virtual void myNotify(std::string key, double arg);
   virtual void myNotify(std::string key, std::string arg);

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

   Autom		state_machine;

};

#endif 
