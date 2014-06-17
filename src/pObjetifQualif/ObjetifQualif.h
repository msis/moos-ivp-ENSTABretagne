/************************************************************/
/*    NAME: MSIS                                              */
/*    ORGN: MIT                                             */
/*    FILE: ObjetifQualif.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef ObjetifQualif_HEADER
#define ObjetifQualif_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class ObjetifQualif : public CMOOSApp
{
 public:
   ObjetifQualif();
   ~ObjetifQualif();

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

   double	m_vyP, m_vxP; // P_VY,	P_VX
   double	m_vxMax, m_vyMax; // VX_MAX,	VY_MAX
   double	m_axisXangleN, m_axisXangleS, m_axisYangleW; // H_NORTH, H_SOUTH, H_WEST
   double	m_lowerX, m_upperX, m_upperY; // LOWER_X, UPPER_X, UPPER_Y
   double	m_distEcho; // ECHO_DIST_FLT

   bool		m_inLine;
   bool		m_crossed;
   bool 	m_useEcho; // USE_ECHO_DIST
   bool		m_done;
   bool		m_start;
};

#endif 
