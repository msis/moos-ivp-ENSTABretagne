/**
 * \file TemperatureCPU_Tests.cpp
 * \brief Tests de l'application pTemperatureCPU
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
#include "TemperatureCPU.h"
#include "TemperatureCPU_Tests.h"

/**
 * \fn
 * \brief Procedure: launchTestsAndExitIfOk
 */

void launchTestsAndExitIfOk()
{
	TemperatureCPU temp;
	Tests sessionDeTests("pTemperatureCPU");
	sessionDeTests.tester(temp.getTemperatureProcesseur() != -1, "Ouverture des données de température");
	sessionDeTests.tester(temp.getTemperatureProcesseur() != -2 && temp.getTemperatureProcesseur() != -1, "Lecture des données de température");
	sessionDeTests.afficherConclusionTests();
}