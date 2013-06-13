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
	cout << (round(4/PRECISION_DONNEES_ENVOYEES)) << endl;
	// Encodages
	
	sessionDeTests.tester(Communication::encoderMesureExterneX(4, chaine) && strcmp(chaine, "00000000000000010000000000000001") == 0, "Encodage d'une position selon X : 4");
	sessionDeTests.tester(Communication::encoderMesureExterneY(691, chaine) && strcmp(chaine, "00010000100010100001000010001010") == 0, "Encodage d'une position selon Y : 691");
	sessionDeTests.tester(Communication::encoderMesureExterneZ(4080, chaine) && strcmp(chaine, "00100011001100000010001100110000") == 0, "Encodage d'une position selon Z : 4080");
	sessionDeTests.tester(Communication::encoderMesureExterneZ(169351, chaine) == false, "Encodage d'une position trop longue");
	sessionDeTests.tester(Communication::encoderPositionAnomalieX(36, chaine) && strcmp(chaine, "11000000000001111100000000000111") == 0, "Encodage d'une position de l'anomalie selon X : 36");
	sessionDeTests.tester(Communication::encoderPositionAnomalieY(52, chaine) && strcmp(chaine, "11010000000010101101000000001010") == 0, "Encodage d'une position de l'anomalie selon Y : 52");
	sessionDeTests.tester(Communication::encoderPositionAnomalieZ(750, chaine) && strcmp(chaine, "11100000100101101110000010010110") == 0, "Encodage d'une position de l'anomalie selon Z : 750");
	sessionDeTests.tester(Communication::encoderConfirmationReception(chaine) && strcmp(chaine, "10100000000000011010000000000001") == 0, "Encodage d'une confirmation de réception");
	sessionDeTests.tester(Communication::encoderEtatAnomalie(true, chaine) && strcmp(chaine, "10110000000000011011000000000001") == 0, "Encodage d'une information sur la bouée (allumée)");
	sessionDeTests.tester(Communication::encoderEtatAnomalie(false, chaine) && strcmp(chaine, "10110000000000001011000000000000") == 0, "Encodage d'une information sur la bouée (éteinte)");
	sessionDeTests.tester(strlen(chaine) == NOMBRE_BITS_TOTAL, "Taille de la chaine encodée (32 bits)");
	
	// Vérifications - recherche d'erreurs
	sessionDeTests.tester(Communication::messageValide((char*)"11010010011011001101001001101100"), "Message valide");
	sessionDeTests.tester(Communication::messageValide((char*)"00010000000000000001000000000000"), "Message valide");
	sessionDeTests.tester(Communication::messageValide((char*)"11110010011011001101001001101100") == false, "Message contenant une erreur");
	sessionDeTests.tester(Communication::messageValide((char*)"00001111111111110000111101111111") == false, "Message contenant une erreur");
	sessionDeTests.tester(Communication::messageValide((char*)"11010110011011001101001001101101") == false, "Message contenant deux erreurs");
	
	// Décodage
	int type_message, data;
	sprintf(chaine, "00000000101011110000000010101111");
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_MESURE_EXTERNE_X, "Décodage du type du message (position X - mesure externe)");
	sessionDeTests.tester(data == 875, "Décodage des données (position X - mesure externe)");
	sprintf(chaine, "00010000111110100001000011111010");
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_MESURE_EXTERNE_Y, "Décodage du type du message (position Y - mesure externe)");
	sessionDeTests.tester(data == 1250, "Décodage des données (position Y - mesure externe)");
	sprintf(chaine, "00100001010001010010000101000101");
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_MESURE_EXTERNE_Z, "Décodage du type du message (position Z - mesure externe)");
	sessionDeTests.tester(data == 1625, "Décodage des données (position Z - mesure externe)");
	sprintf(chaine, "11000001011000001100000101100000");
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_ANOMALIE_X, "Décodage du type du message (position anomalie X)");
	sessionDeTests.tester(data == 1760, "Décodage des données (position anomalie X)");
	sprintf(chaine, "11010010110101101101001011010110");
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_ANOMALIE_Y, "Décodage du type du message (position anomalie Y)");
	sessionDeTests.tester(data == 3630, "Décodage des données (position anomalie Y)");
	sprintf(chaine, "11100010101010001110001010101000");
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_ANOMALIE_Z, "Décodage du type du message (position anomalie Z)");
	sessionDeTests.tester(data == 3400, "Décodage des données (position anomalie Z)");
	sprintf(chaine, "10110000000000001011000000000000");
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_ETAT_ANOMALIE, "Décodage du type du message (état anomalie)");
	sessionDeTests.tester(data == 0, "Décodage des données (état anomalie faux)");
	sprintf(chaine, "10110000000000011011000000000001");
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_ETAT_ANOMALIE, "Décodage du type du message (état anomalie)");
	sessionDeTests.tester(data == 1, "Décodage des données (état anomalie vrai)");
	sprintf(chaine, "10100000000000011010000000000001");
	sessionDeTests.tester(Communication::decoderMessage(chaine, &type_message, &data), "Décodage sans erreur");
	sessionDeTests.tester(type_message == TYPE_CONFIRMATION_RECEPTION, "Décodage du type du message (confirmation réception)");
	
	// Initialisation du modem
	Modem modem("/dev/ttyUSB1", false);
	sessionDeTests.tester(modem.initialiserPortSerie(), "Initialisation du modem (port série)");
	
	// Conversions binaires
	ConversionsBinaireASCII::asciiToBinary((char*)"test", chaine);
	sessionDeTests.tester(strcmp(chaine, "01110100011001010111001101110100") == 0, "Conversion ASCII -> binaire");
	ConversionsBinaireASCII::asciiToBinary((char*)"toto", chaine);
	sessionDeTests.tester(strcmp(chaine, "01110100011011110111010001101111") == 0, "Conversion ASCII -> binaire");
	ConversionsBinaireASCII::binaryToAscii((char*)"01110110011001010110111001101001", chaine);
	sessionDeTests.tester(strcmp(chaine, "veni") == 0, "Conversion binaire -> ASCII");
	ConversionsBinaireASCII::binaryToAscii((char*)"01110110011010010110001101101001", chaine);
	sessionDeTests.tester(strcmp(chaine, "vici") == 0, "Conversion binaire -> ASCII");
	
	sessionDeTests.afficherConclusionTests();
}
