/**
 * \file Mapping.cpp
 * \brief Classe Mapping
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 27th 2013
 *
 * Application MOOS de mapping
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "Mapping.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/stitching/stitcher.hpp>
#include <iostream>
#include <vector>

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Mapping::Mapping()
{
	Mat carte;
	Mat fr1 = imread("");
	Mat fr2 = imread("");
	vector<Mat> imgs;

	Stitcher stitcher = Stitcher::createDefault();

	imgs.push_back(fr1);
	imgs.push_back(fr2);
	Stitcher::Status status;

	status = stitcher.stitch(imgs, carte);

	if(status != Stitcher::OK)
		cout << "Error stitching - Code: " <<int(status)<<endl;

	imshow("Frame 1", imgs[0]);
	imshow("Frame 2", imgs[1]);
	imshow("Stitched Image", carte);
	waitKey();
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Mapping::~Mapping()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Mapping::OnNewMail(MOOSMSG_LIST &NewMail)
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
	}

	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool Mapping::OnConnectToServer()
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
 
bool Mapping::Iterate()
{
	m_iterations++;
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Mapping::OnStartUp()
{
	setlocale(LC_ALL, "C");
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
	{
		list<string>::iterator p;
		for(p = sParams.begin() ; p != sParams.end() ; p++)
		{
			string original_line = *p;
			string param = stripBlankEnds(toupper(biteString(*p, '=')));
			string value = stripBlankEnds(*p);

			if(param == "FOO")
			{
				//handled
			}
			
			else if(param == "BAR")
			{
				//handled
			}
		}
	}

	m_timewarp = GetMOOSTimeWarp();

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void Mapping::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
}
