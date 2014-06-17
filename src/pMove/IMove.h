#ifndef IMove_HEADER
#define IMove_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <string>

class IMove : public CMOOSApp
{
 public:
   virtual void myNotify(std::string key, double arg);
   virtual void myNotify(std::string key, std::string arg);

};

#endif 
