/**
 * \file Modem_Tests.cpp
 * \brief Tests de l'application pModem
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
#include <string.h>
#include "Modem.h"
#include "Modem_Tests.h"
#include "Communication.h"

/**
 * \fn
 * \brief Procedure: launchTestsAndExitIfOk
 */

void launchTestsAndExitIfOk()
{
	char chaine[NOMBRE_BITS_TOTAL];
	Tests sessionDeTests("pModem");
	
	// Logique binaire - décimale
	sessionDeTests.tester(Communication::conversionEnBinaire(4) == 100, "Conversion binaire de 4");
	sessionDeTests.tester(Communication::conversionEnBinaire(46131) == 1011010000110011, "Conversion binaire de 46131");
	sessionDeTests.tester(Communication::conversionEnBinaire(429487) == 1101000110110101111, "Conversion binaire 429487");
	sessionDeTests.tester(Communication::conversionEnDecimal(1000101110101010) == 35754, "Conversion binaire de 01000101110101010");
	sessionDeTests.tester(Communication::conversionEnDecimal(1001) == 9, "Conversion binaire de 01001");
	sessionDeTests.tester(Communication::conversionEnDecimal(1110101) == 117, "Conversion binaire 1110101");
	
	// Encodages
	sessionDeTests.tester(Communication::encoderX(4, chaine) && strcmp(chaine, "01000000000001000100000000000100") == 0, "Encodage d'une position selon X : 4");
	sessionDeTests.tester(Communication::encoderX(2545, chaine) && strcmp(chaine, "01001001111100010100100111110001") == 0, "Encodage d'une position selon X : 2545");
	sessionDeTests.tester(Communication::encoderY(691, chaine) && strcmp(chaine, "10000010101100111000001010110011") == 0, "Encodage d'une position selon Y : 691");
	sessionDeTests.tester(Communication::encoderY(742, chaine) && strcmp(chaine, "10000010111001101000001011100110") == 0, "Encodage d'une position selon Y : 742");
	sessionDeTests.tester(Communication::encoderZ(4716, chaine) && strcmp(chaine, "11010010011011001101001001101100") == 0, "Encodage d'une position selon Z : 4716");
	sessionDeTests.tester(Communication::encoderZ(18, chaine) && strcmp(chaine, "11000000000100101100000000010010") == 0, "Encodage d'une position selon Z : 18");
	sessionDeTests.tester(Communication::encoderZ(169351, chaine) == false, "Encodage d'une position trop longue");
	sessionDeTests.tester(Communication::encoderConfirmationReception(chaine) && strcmp(chaine, "00000000000000000000000000000000") == 0, "Encodage d'une confirmation de réception");
	sessionDeTests.tester(Communication::encoderEtatAnomalie(true, chaine) && strcmp(chaine, "00011111111111110001111111111111") == 0, "Encodage d'une information sur la bouée (allumée)");
	sessionDeTests.tester(Communication::encoderEtatAnomalie(false, chaine) && strcmp(chaine, "00010000000000000001000000000000") == 0, "Encodage d'une information sur la bouée (éteinte)");
	sessionDeTests.tester(strlen(chaine) == NOMBRE_BITS_TOTAL, "Taille de la chaine encodée (32 bits)");
	
	// Vérifications - recherche d'erreurs
	sessionDeTests.tester(Communication::messageValide((char*)"11010010011011001101001001101100"), "Message valide");
	sessionDeTests.tester(Communication::messageValide((char*)"00010000000000000001000000000000"), "Message valide");
	sessionDeTests.tester(Communication::messageValide((char*)"11110010011011001101001001101100") == false, "Message contenant une erreur");
	sessionDeTests.tester(Communication::messageValide((char*)"00001111111111110000111101111111") == false, "Message contenant une erreur");
	sessionDeTests.tester(Communication::messageValide((char*)"11010110011011001101001001101101") == false, "Message contenant deux erreurs");
	
	// Décodage
	int type_message, data;
	sprintf(chaine, "01001001111100010100100111110001");
	Communication::decoderMessage(chaine, &type_message, &data);
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_POSITION_X, "Décodage du type du message (position X)");
	sessionDeTests.tester(data == 2545, "Décodage des données (position X)");
	sprintf(chaine, "11010010011011001101001001101100");
	Communication::decoderMessage(chaine, &type_message, &data);
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_POSITION_Z, "Décodage du type du message (position Z)");
	sessionDeTests.tester(data == 4716, "Décodage des données (position Z)");
	sprintf(chaine, "00010000000000010001000000000001");
	Communication::decoderMessage(chaine, &type_message, &data);
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_AUTRE_ETAT_ANOMALIE, "Décodage du type du message (état anomalie)");
	sessionDeTests.tester(data == 1, "Décodage des données (état anomalie)");
	sprintf(chaine, "00010000000000000001000000000000");
	Communication::decoderMessage(chaine, &type_message, &data);
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_AUTRE_ETAT_ANOMALIE, "Décodage du type du message (état anomalie)");
	sessionDeTests.tester(data == 0, "Décodage des données (état anomalie)");
	sprintf(chaine, "00000000000000000000000000000000");
	Communication::decoderMessage(chaine, &type_message, &data);
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_AUTRE_CONFIRMATION_RECEPTION, "Décodage du type du message (confirmation réception)");
	
	// Initialisation du modem
	Modem modem("/dev/ttyUSB0", false);
	sessionDeTests.tester(modem.initialiserPortSerie(), "Initialisation du modem (port série)");
	
	sessionDeTests.afficherConclusionTests();
}
