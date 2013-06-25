/**
 * \file Ciscrea_Tests.cpp
 * \brief Tests de l'application pCiscrea
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
#include "Ciscrea_Tests.h"
#include "AUV.h"

/**
 * \fn
 * \brief Procedure: launchTestsAndExitIfOk
 */

void launchTestsAndExitIfOk()
{
	Tests sessionDeTests("pCiscrea");
	
	/* 	Test des calculs de propulsion vectorielle	*/
	
	// Contrôle de la consigne
		AUV auv_ciscrea_test;
		
		auv_ciscrea_test.setVx(3);
		auv_ciscrea_test.setVy(5);
		auv_ciscrea_test.setVz(-10);
		auv_ciscrea_test.setRz(-31);
		Matrice *vecteurConsigne = auv_ciscrea_test.getVecteurConsigneAUV();
		
		sessionDeTests.tester((*vecteurConsigne)[0][0] == 3, "enregistrement consigne Vx");
		sessionDeTests.tester((*vecteurConsigne)[1][0] == 5, "enregistrement consigne Vy");
		sessionDeTests.tester((*vecteurConsigne)[2][0] == -10, "enregistrement consigne Vz");
		sessionDeTests.tester((*vecteurConsigne)[3][0] == 0, "enregistrement consigne Rx");
		sessionDeTests.tester((*vecteurConsigne)[4][0] == 0, "enregistrement consigne Ry");
		sessionDeTests.tester((*vecteurConsigne)[5][0] == -31, "enregistrement consigne Rz");
	
	// Contrôle des forces des propulseurs (non nulles en Vy et Vz seulement)
		auv_ciscrea_test.setVx(0);
		auv_ciscrea_test.setVy(5);
		auv_ciscrea_test.setVz(36);
		auv_ciscrea_test.setRz(0);
		vecteurConsigne = auv_ciscrea_test.getVecteurConsigneAUV();
		Matrice *forcesPropulseurs = auv_ciscrea_test.getForcesPropulseurs();
		
		sessionDeTests.tester((*forcesPropulseurs)[0][0] != 0, "Vy & Vz != 0 : controle force non nulle appliquee en F1");	// F1
		sessionDeTests.tester((*forcesPropulseurs)[1][0] != 0, "Vy & Vz != 0 : controle force non nulle appliquee en F2");	// F2
		sessionDeTests.tester((*forcesPropulseurs)[2][0] != 0, "Vy & Vz != 0 : controle force non nulle appliquee en F3");	// F3
		sessionDeTests.tester((*forcesPropulseurs)[3][0] != 0, "Vy & Vz != 0 : controle force non nulle appliquee en F4");	// F4
		sessionDeTests.tester((*forcesPropulseurs)[4][0] != 0, "Vy & Vz != 0 : controle force non nulle appliquee en F5");	// F5
		sessionDeTests.tester(round((*forcesPropulseurs)[5][0]) == round((*forcesPropulseurs)[4][0]), "Vy & Vz != 0 : controle non force F5 = F6");	// F6
	
	// Contrôle des forces des propulseurs (non nulles en Vz seulement)
		auv_ciscrea_test.setVx(0);
		auv_ciscrea_test.setVy(0);
		auv_ciscrea_test.setVz(12);
		auv_ciscrea_test.setRz(0);
		vecteurConsigne = auv_ciscrea_test.getVecteurConsigneAUV();
		forcesPropulseurs = auv_ciscrea_test.getForcesPropulseurs();
		
		sessionDeTests.tester((*forcesPropulseurs)[0][0] == 0, "Vz != 0 : controle force nulle appliquee en F1");	// F1
		sessionDeTests.tester((*forcesPropulseurs)[1][0] == 0, "Vz != 0 : controle force nulle appliquee en F2");	// F2
		sessionDeTests.tester((*forcesPropulseurs)[2][0] == 0, "Vz != 0 : controle force nulle appliquee en F3");	// F3
		sessionDeTests.tester((*forcesPropulseurs)[3][0] == 0, "Vz != 0 : controle force nulle appliquee en F4");	// F4
		sessionDeTests.tester((*forcesPropulseurs)[4][0] != 0, "Vz != 0 : controle force non nulle appliquee en F5");	// F5
		sessionDeTests.tester(round((*forcesPropulseurs)[5][0]) == round((*forcesPropulseurs)[4][0]), "Vz != 0 : controle force non nulle F5 = F6");	// F6
	
	// Contrôle des forces des propulseurs (non nulles en Vz seulement)
		auv_ciscrea_test.setVx(-29);
		auv_ciscrea_test.setVy(0);
		auv_ciscrea_test.setVz(0);
		auv_ciscrea_test.setRz(0);
		vecteurConsigne = auv_ciscrea_test.getVecteurConsigneAUV();
		forcesPropulseurs = auv_ciscrea_test.getForcesPropulseurs();
		forcesPropulseurs->afficher();
		sessionDeTests.tester((*forcesPropulseurs)[0][0] != 0, "Vx != 0 : controle force non nulle appliquee en F1");	// F1
		sessionDeTests.tester((*forcesPropulseurs)[1][0] != 0, "Vx != 0 : controle force non nulle appliquee en F2");	// F2
		sessionDeTests.tester((*forcesPropulseurs)[2][0] != 0, "Vx != 0 : controle force non nulle appliquee en F3");	// F3
		sessionDeTests.tester((*forcesPropulseurs)[3][0] != 0, "Vx != 0 : controle force non nulle appliquee en F4");	// F4
		sessionDeTests.tester((*forcesPropulseurs)[4][0] == 0, "Vx != 0 : controle force nulle appliquee en F5");	// F5
		sessionDeTests.tester(round((*forcesPropulseurs)[5][0]) == round((*forcesPropulseurs)[4][0]), "Vx != 0 : controle force nulle F5 = F6");	// F6
		
	sessionDeTests.afficherConclusionTests();
}
