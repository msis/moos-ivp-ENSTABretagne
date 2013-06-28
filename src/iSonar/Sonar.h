/**
 * \file Sonar.h
 * \brief Classe Sonar
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données liées au sonar
 *
 */

#ifndef Sonar_HEADER
#define Sonar_HEADER

#include "../common/constantes.h"
//#include "SonarDF.h"
#include "seanetmsg.h"

#include "MOOS/libMOOS/App/MOOSInstrument.h"
/*#ifdef _WIN32
	#include "MOOS/libMOOS/Utils/MOOSNTSerialPort.h"
#else
	#include "MOOS/libMOOS/Utils/MOOSLinuxSerialPort.h"
#endif
*/
using namespace std;

#include <cv.hpp>

class Sonar : public CMOOSInstrument
{
        //friend bool listen_sonar_messages_thread_func(void *pSonarObject);
    
	public:
		Sonar();
		bool initialiserPortSerie(string nom_port);
		~Sonar();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
                
               void ListenSonarMessages();
               
               bool SendMessage(const SeaNetMsg & msg) {msg.print_hex();printf("\n");return (m_Port.Write(msg.data().data(), msg.data().size()) == msg.data().size());}

	private: // Configuration variables
		string m_portName;

	private: // State variables
		unsigned int			m_iterations;
		double			        m_timewarp;
		//CMOOSLinuxSerialPort	m_moos_serial_port;
		//SonarDF*				m_cissonar;
                bool m_bNoParams;
                bool m_bSentCfg;
                
                bool m_bSonarReady;
                bool m_bPollSonar;
                
                CMOOSThread m_serial_thread;
                
                cv::Mat img;

    
        static bool listen_sonar_messages_thread_func(void *pSonarObject) {
            Sonar* pSonar = static_cast<Sonar*> (pSonarObject);
            if (pSonar) {
                pSonar->ListenSonarMessages();
                return true;
            }
            else return false;
        }
};

#endif 
