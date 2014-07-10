/**
 * \file Sonar.cpp
 * \brief Classe Sonar
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données liées au sonar
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "Sonar.h"
#include "seanetmsg.h"

//#include <opencv2/highgui/highgui.hpp>


using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Sonar::Sonar()
{
	cout << "creation de l'objet sonar..." << endl;
	m_iterations = 0;
	m_timewarp   = 1;

	m_bNoParams = true;
	m_bSentCfg = false;

	m_bSonarReady = false;
	m_bPollSonar = true;

	if (m_serial_thread.Initialise(listen_sonar_messages_thread_func, (void*)this))
	  MOOSTrace("Sonar thread initialized\n");
	else
	  MOOSFail("Sonar thread initialization error...\n");

	//img.create(360, 800, CV_8UC1);
	//img_polar.create(800, 800, CV_8UC1);
	//MOOSTrace("Sonar Img created\n");
	//cv::namedWindow("sonar");
	//cv::namedWindow("sonar pol");

	log1.open("SONAR_SCANLINES.txt", ios_base::out);
	log2.open("SONAR_DIST.txt", ios_base::out);
	cout << "...fine" << endl;
}

/**
 * \fn
 * \brief Méthode initialisant le port série
 */
 
bool Sonar::initialiserPortSerie(string nom_port)
{
/*	int baud = 115200;
	
	// Instanciation de l'objet de communication avec le port série
	cout << "Initialisation de \"" << nom_port << "\" (" << baud << ")" << endl;
	this->m_moos_serial_port = CMOOSLinuxSerialPort();
	return this->m_moos_serial_port.Create((char*)nom_port.c_str(), baud);*/
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Sonar::~Sonar()
{
    MOOSTrace("iSonar: stopping aquisition thread.\n");
    m_serial_thread.Stop();
    
        log1.close();
        log2.close();
	MOOSTrace("iSonar: finished.\n");
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Sonar::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;

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
		
		// Mise à jour des paramètres du sonar
		if ( msg.GetKey() == "SONAR_PARAMS" && msg.IsString() ) {
		  string msg_val = msg.GetString();
		  // Le message est de la forme "Range=25,Gain=45,Continuous=true"
		  double dVal=0.0; int iVal; bool bVal;
		  if (MOOSValFromString(dVal, msg_val, "Range", true))
		    m_msgHeadCommand.setRange(dVal);    
		  if (MOOSValFromString(iVal, msg_val, "nBins", true))
		    m_msgHeadCommand.setNbins(iVal);
		  if (MOOSValFromString(dVal, msg_val, "AngleStep", true))
	      	m_msgHeadCommand.setAngleStep(dVal);
		  if (MOOSValFromString(bVal, msg_val, "Continuous", true))
	     	m_msgHeadCommand.setContinuous(bVal);
		  if (MOOSValFromString(dVal, msg_val, "Gain", true))
		    m_msgHeadCommand.setGain(dVal);
		  if (MOOSValFromString(dVal, msg_val, "LeftLimit", true)){
		    m_msgHeadCommand.setLeftLimit(dVal); cout << "limite gauche " << dVal << endl;}
		  if (MOOSValFromString(dVal, msg_val, "RightLimit", true)){
		    m_msgHeadCommand.setRightLimit(dVal); cout << "limite droite " << dVal << endl;}
		  // Envoi de la commande au sondeur
		  // TODO: vérifier que le CMOOSSerialPort est bien thread safe. Sinon, rajouter un mutex
		  SendMessage(m_msgHeadCommand);
		}
	}

	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool Sonar::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool Sonar::Iterate()
{
	m_iterations++;
    //    cv::waitKey(10);
/*	
	if(this->m_cissonar->isConnected())
	{
		vector<double> Valpha;
		vector<double> Vdistance;
		
		this->m_cissonar->get_sonar_data(Valpha, Vdistance);
	}
	*/
	/*else
		cout << "Sonar non connecté !" << endl;*/
	
	return(true);
}

/**
 * \fn
 * \brief Thread d'acquisition des données sonar
 * Attente d'un état valide, puis demande et acquisition des données en continu
 */

void Sonar::ListenSonarMessages()
{
    const int buf_size = 512;
    char buf[buf_size];
    string sBuf;
    
    union HeadInf {
        char c;
        struct {
            int InCenter : 1;
            int Centered : 1;
            int Motoring : 1;
            int MotorOn  : 1;
            int Dir : 1;
            int InScan : 1;
            int NoParams : 1;
            int SentCfg : 1;
        } bits;
    } headInf;
    
    while (!m_serial_thread.IsQuitRequested())
    {
        int msg_size = 0;
        int needed_len = SeaNetMsg::numberBytesMissing(sBuf, msg_size);
        
        if (needed_len == SeaNetMsg::mrNotAMessage) {
            // Remove first character if the header cannot be decoded
            sBuf.erase(0,1);
        }
        else if (needed_len > 0) {
            // Read more data as needed
            int nb_read = m_Port.ReadNWithTimeOut(buf, needed_len);
            sBuf.append(buf, nb_read);
        }
        else if (needed_len == 0) {
            // Process message
            // cout << "Found message " << SeaNetMsg::detectMessageType(sBuf) << endl;
            SeaNetMsg snmsg(sBuf);
            //cout << "Created message with type " << snmsg.messageType() << endl;
            //snmsg.print_hex();
            
            if (snmsg.messageType() == SeaNetMsg::mtAlive) {
                headInf.c = snmsg.data().at(20);
                
                m_bNoParams = headInf.bits.NoParams;
                m_bSentCfg = headInf.bits.SentCfg;
                
                // Sonar polling was enabled but Sonar was not ready...
                // ...now that sonar is ready, start to poll.
                if (!m_bSonarReady && m_bPollSonar && !m_bNoParams && m_bSentCfg) {
                    cout << "Sonar is now ready, initiating scanline polling." << endl;
                    SeaNetMsg_SendData msg_SendData;
                    msg_SendData.setTime(MOOSTime());
                    SendMessage(msg_SendData);
                }   
                
                // Update m_bSonarReady
                m_bSonarReady = (!m_bNoParams) && m_bSentCfg;
                
                //cout << "InScan:"<<headInf.bits.InScan << " NoParams:"<<headInf.bits.NoParams << " SentCfg:"<<headInf.bits.SentCfg;
            }
            
            if (snmsg.messageType() == SeaNetMsg::mtHeadData) {
                const SeaNetMsg_HeadData * pHdta = reinterpret_cast<SeaNetMsg_HeadData*> (&snmsg);
                
                // Display
		//MOOSTrace("nBins=%d\n", pHdta->nBins());
                //uchar* line = img.ptr((int)pHdta->bearing());
                //memcpy(line, pHdta->scanlineData(), pHdta->nBins());
                //cvLogPolar(&img, &img_polar, cvPoint2D32f(320/2, 240/2), 40, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS + CV_WARP_INVERSE_MAP);
                //cv::imshow("sonar", img);
                //cv::imshow("sonar pol", img_polar);

                // MOOSDB raw data
                vector<int> vScanline;
                for (int k=0; k<pHdta->nBins(); ++k)
                    vScanline.push_back( pHdta->scanlineData()[k] );
                
                stringstream ss;
                ss << "bearing=" << pHdta->bearing() << ",";
                ss << "ad_interval=" << pHdta->ADInterval_m() << ",";
                ss << "scanline=";
                Write(ss, vScanline);
                
                Notify("SONAR_RAW_DATA", ss.str());
                //cout << endl << ss.str() << endl;
                log1 << MOOSGetTimeStampString() << " " << ss.str() << endl;
                
                stringstream ss2;
                ss2 << "bearing=" << pHdta->bearing() << ","
                    << "distance=" << pHdta->firstObstacleDist(90, 0.5, 100.); // thd, min, max
                Notify("SONAR_DISTANCE", ss2.str());
                
                log2 << MOOSGetTimeStampString() << " " << ss.str() << endl;
                                
                if (m_bSonarReady && m_bPollSonar) {
                    SeaNetMsg_SendData msg_SendData;
                    msg_SendData.setTime(MOOSTime());
                    SendMessage(msg_SendData);
                }
                                
                //cout << "InScan:"<<headInf.bits.InScan << " NoParams:"<<headInf.bits.NoParams << " SentCfg:"<<headInf.bits.SentCfg;
            }
            
            //cout << endl;
            
            sBuf.erase(0,msg_size);
        }
    }
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Sonar::OnStartUp()
{
	MOOSTrace("\nSonar start up\n");
	setlocale(LC_ALL, "C");
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
	{
	    MOOSTrace("iSonar: Reading configuration\n");
	    list<string>::iterator p;
	    for(p = sParams.begin() ; p != sParams.end() ; p++)
	    {
		string original_line = *p;
		string param = stripBlankEnds(toupper(biteString(*p, '=')));
		string value = stripBlankEnds(*p);

		MOOSTrace(original_line);

		if(param == "SERIAL_PORT_NAME")
		{
		    MOOSTrace("iSonar: Using %s serial port\n", value.c_str());
		    m_portName = value;
		}
		
		if(MOOSStrCmp(param, "Range"))
		    m_msgHeadCommand.setRange(atof(value.c_str()));
		if(MOOSStrCmp(param, "nBins"))
		    m_msgHeadCommand.setNbins(atoi(value.c_str()));
		if(MOOSStrCmp(param, "AngleStep"))
		    m_msgHeadCommand.setAngleStep(atof(value.c_str()));
		if(MOOSStrCmp(param, "Continuous"))
		    m_msgHeadCommand.setContinuous(MOOSStrCmp(value,"true"));
		if(MOOSStrCmp(param, "Gain"))
		    m_msgHeadCommand.setGain(atof(value.c_str()));
		if(MOOSStrCmp(param, "LeftLimit"))
		    m_msgHeadCommand.setLeftLimit(atof(value.c_str()));
		if(MOOSStrCmp(param, "RightLimit"))
		    m_msgHeadCommand.setRightLimit(atof(value.c_str()));
	    }
	}
	else
		MOOSTrace("No configuration read.\n");

/*	string fichier_config = "Sonar.txt";
	this->m_cissonar = new SonarDF((char*)fichier_config.c_str());
*/
	MOOSTrace("Opening serial port\n");
        bool portOpened = this->m_Port.Create(m_portName.c_str(), 115200);
	if (portOpened)
		MOOSTrace("Port opened\n");
	else
		MOOSTrace("Port not opened\n");

        //this->m_Port.SetTermCharacter('\n');
        m_Port.Flush();

	m_timewarp = GetMOOSTimeWarp();

	RegisterVariables();
        
	MOOSTrace("Starting thread\n");
        m_serial_thread.Start();
        
        //////
        SeaNetMsg_ReBoot msg_ReBoot;
        
        MOOSPause(50);
        cout << "REBOOT" << endl;
        SendMessage(msg_ReBoot);
        
        MOOSPause(1000);
        cout << "SEND VERSION" << endl;
        SendMessage(SeaNetMsg_SendVersion());

        MOOSPause(50);
        cout << "SEND BBUSER" << endl;
        SendMessage(SeaNetMsg_SendBBUser());
                
        MOOSPause(50);
        cout << "HEAD COMMAND" << endl;
        SendMessage(m_msgHeadCommand);
        
        //////
        
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void Sonar::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
	Register("SONAR_PARAMS", 0);
}
