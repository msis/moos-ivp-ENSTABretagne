/**
 * \file Logs_Tests.cpp
 * \brief Tests de l'application pLogs
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Tests unitaires à valider pour vérifier le bon fonctionnement 
 * de l'application MOOS
 *
 */

#include <cstdlib>
#include <iostream>
#include "Logs.h"
#include "Logs_Tests.h"

/**
 * \fn
 * \brief Procedure: launchTestsAndExitIfOk
 */

void launchTestsAndExitIfOk()
{
	Logs logs;
	Tests sessionDeTests("pLogs");
	sessionDeTests.tester(logs.creerFichierLogs(), "Creation/ouverture du fichier de logs");
	sessionDeTests.tester(logs.supprimerFichierLogs(), "Suppression du fichier de logs");
	sessionDeTests.afficherConclusionTests();
}