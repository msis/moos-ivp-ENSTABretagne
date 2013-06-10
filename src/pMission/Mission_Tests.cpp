/**
 * \file Mission_Tests.cpp
 * \brief Tests de l'application pMission
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 10th 2013
 *
 * Tests unitaires à valider pour vérifier le bon fonctionnement 
 * de l'application MOOS
 *
 */

#include <cstdlib>
#include <iostream>
#include "Mission_Tests.h"

/**
 * \fn
 * \brief Procedure: launchTestsAndExitIfOk
 */

void launchTestsAndExitIfOk()
{
	Tests sessionDeTests("pMission");
	sessionDeTests.afficherConclusionTests();
}