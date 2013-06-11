/**
 * \file InertialMeasurementUnit_Tests.cpp
 * \brief Tests de l'application pInertialMeasurementUnit
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
#include "InertialMeasurementUnit.h"
#include "InertialMeasurementUnit_Tests.h"

/**
 * \fn
 * \brief Procedure: launchTestsAndExitIfOk
 */

void launchTestsAndExitIfOk(string param)
{
	Tests sessionDeTests("pInertialMeasurementUnit");
	InertialMeasurementUnit centrale(param, false);
	sessionDeTests.tester(centrale.initialiserRazorAHRS(), "Initialisation du capteur");
	sessionDeTests.afficherConclusionTests();
}