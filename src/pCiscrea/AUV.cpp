/**
 * \file AUV.cpp
 * \brief Classe AUV
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Classe matérialisant un AUV de type Ciscrea
 *
 */

#include "AUV.h"
#include "ColorParse.h"

/**
 * \fn
 * \brief Constructeur de la classe AUV (selon l'identifiant du Ciscrea)
 */

AUV::AUV(int numero_AUV)
{
	this->identifiantAUV = numero_AUV;
	this->mb = new M6dbus(this->getIP(), PORT_MODBUS);
	
	cout << "Initialisation de l'AUV \"" << getNom() << "\" et de ses variateurs";
	
	for(int i = 0 ; i < 5 ; i ++) // Selon la doc, 5 secondes d'initialisation sont nécessaires
	{
		cout << ".";
		cout.flush();
		usleep(1000 * 1000);
	}
	
	cout << endl;
	
	this->mode_virtuel = false;
	initialisation();
}

/**
 * \fn
 * \brief Constructeur de la classe AUV (par récupération d'une instance modbus)
 */

AUV::AUV(M6dbus *m) : mb(m)
{
	this->mode_virtuel = false;
	initialisation();
}

/**
 * \fn
 * \brief Constructeur de la classe AUV (en mode virtuel sans connexion réelle)
 */

AUV::AUV()
{
	this->mode_virtuel = true;
	this->identifiantAUV = 2;
	initialisation();
}

/**
 * \fn
 * \brief Récupération de l'adresse IP d'un AUV en fonction de son identifiant
 */

string AUV::getIP()
{
	switch(this->identifiantAUV)
	{
		case 1:
			return "192.168.1.6";
			
		case 2:
			return "192.168.1.7";
			
		case 3:
			return "192.168.1.8";
			
		case 4:
			return "192.168.1.9";
			
		default:
			return "127.0.0.1";
	}
}

/**
 * \fn
 * \brief Récupération du nom de l'AUV en fonction de son identifiant
 */

string AUV::getNom()
{
	switch(this->identifiantAUV)
	{
		case 1:
			return "veni";
			
		case 2:
			return "vidi";
			
		case 3:
			return "vici";
			
		case 4:
			return "jack";
			
		default:
			return "?";
	}
}

/**
 * \fn
 * \brief Récupération de l'identifiant de l'AUV
 */

int AUV::getIdentifiant()
{
	return this->identifiantAUV;
}

/**
 * \fn
 * \brief Méthode d'initialisation de l'AUV
 */

void AUV::initialisation()
{
	if(!this->mode_virtuel)
	{
		this->on = this->mb->getOn();

		if(!this->on)
		{
			cerr << "Impossible de lancer l'AUV" << endl;
			cout << "\tm6dbus n'est pas pret" << endl;
			return;
		}
	}

	else
		this->on = this->mode_virtuel;

	this->Vx = 0;
	this->Vy = 0;
	this->Vz = 0;
	this->Rz = 0;

	switch(this->identifiantAUV)
	{
		case 1: // RESTE A DEFINIR !
			this->vitesseLimiteFixee = 30;

			// Initialisation des moteurs
			this->propFrRi = new Propulseur("FrRi", -1, 30.0, 1.0, 0); // Devant droite : F1
			this->propReRi = new Propulseur("ReRi", 1, 30.0, 1.0, 0); // Arrière droite : F2
			this->propReLe = new Propulseur("ReLe", 1, 30.0, 1.0, 0); // Arrière gauche : F3
			this->propFrLe = new Propulseur("FrLe", 1, 30.0, 1.0, 0); // Devant gauche : F4
			break;

		case 2: // RESTE A DEFINIR !
			this->vitesseLimiteFixee = 100.0;

			// Initialisation des moteurs
			this->propFrRi = new Propulseur("FrRi", 1, 30.0, 1.0, 0); // Devant droite : F1
			this->propReRi = new Propulseur("ReRi", 1, 30.0, 1.0, 0); // Arrière droite : F2
			this->propReLe = new Propulseur("ReLe", 1, 30.0, 1.0, 0); // Arrière gauche : F3
			this->propFrLe = new Propulseur("FrLe", 1, 30.0, 1.0, 0); // Devant gauche : F4
			break;

		default:
			this->vitesseLimiteFixee = 100.0;

			// Initialisation des moteurs
			this->propFrRi = new Propulseur("FrRi", 1, 30.0, 1.0, 0); // Devant droite : F1
			this->propReRi = new Propulseur("ReRi", 1, 30.0, 1.0, 0); // Arrière droite : F2
			this->propReLe = new Propulseur("ReLe", 1, 30.0, 1.0, 0); // Arrière gauche : F3
			this->propFrLe = new Propulseur("FrLe", 1, 30.0, 1.0, 0); // Devant gauche : F4
	}

	creerMatriceE();
	
	if(!this->mode_virtuel && this->getOn())
	{
		int err = this->mb->updateAll();

		if(err == -1)
		{
			cout << "Erreur lors de l'initialisation" << endl;
			return;
		}
	}
	
	cout << endl << termColor("green") << "Fin de l'initialisation de l'AUV \"" << getNom() << "\"" << termColor() << endl << endl;
}

/**
 * \fn
 * \brief Méthode de création du modèle physique lié aux propulseurs
 * 
 * Résolution d'un système linéaire pour trouver les consignes des propulseurs
 * en fonction de la consigne appliquée sur l'AUV
 */

void AUV::creerMatriceE()
{
	this->E = new Matrice(6, 6, "Modele AUV");

	// Vx :
	(*this->E)[0][0] = cos(this->propFrRi->getAngle() * M_PI / 180.0);	// Devant droite : F1
	(*this->E)[0][1] = -cos(this->propReRi->getAngle() * M_PI / 180.0);	// Derrière droite : F2
	(*this->E)[0][2] = -cos(this->propReLe->getAngle() * M_PI / 180.0);	// Derrière gauche : F3
	(*this->E)[0][3] = cos(this->propFrLe->getAngle() * M_PI / 180.0);	// Devant gauche : F4
	(*this->E)[0][4] = 0;
	(*this->E)[0][5] = 0;

	// Vy :
	(*this->E)[1][0] = -sin(this->propFrRi->getAngle() * M_PI / 180.0);
	(*this->E)[1][1] = sin(this->propReRi->getAngle() * M_PI / 180.0);
	(*this->E)[1][2] = sin(this->propReLe->getAngle() * M_PI / 180.0);
	(*this->E)[1][3] = -sin(this->propFrLe->getAngle() * M_PI / 180.0);
	(*this->E)[1][4] = 0;
	(*this->E)[1][5] = 0;

	// Vz :
	(*this->E)[2][0] = 0;
	(*this->E)[2][1] = 0;
	(*this->E)[2][2] = 0;
	(*this->E)[2][3] = 0;
	(*this->E)[2][4] = 1;
	(*this->E)[2][5] = 1;

	// Rx : impossible
	(*this->E)[3][0] = 0;
	(*this->E)[3][1] = 0;
	(*this->E)[3][2] = 0;
	(*this->E)[3][3] = 0;
	(*this->E)[3][4] = 0;
	(*this->E)[3][5] = 0;

	// Ry : impossible
	(*this->E)[4][0] = 0;
	(*this->E)[4][1] = 0;
	(*this->E)[4][2] = 0;
	(*this->E)[4][3] = 0;
	(*this->E)[4][4] = 0;
	(*this->E)[4][5] = 0;

	// Rz :
	(*this->E)[5][0] = cos(this->propFrRi->getAngle() * M_PI / 180.0);
	(*this->E)[5][1] = -cos(this->propReRi->getAngle() * M_PI / 180.0);
	(*this->E)[5][2] = cos(this->propReLe->getAngle() * M_PI / 180.0);
	(*this->E)[5][3] = -cos(this->propFrLe->getAngle() * M_PI / 180.0);
	(*this->E)[5][4] = 0;
	(*this->E)[5][5] = 0;

	if(AFFICHER_DETAILS)
		this->E->afficher();

	this->E->inverser();

	if(AFFICHER_DETAILS)
		this->E->afficher();
}

/**
 * \fn
 * \brief Méthode normalisant les forces à appliquer aux propulseurs
 * 
 * Limitation de la consommation des propulseurs et de leur impact négatif
 * sur la stabilité de l'AUV en vitesse élevée
 */

void AUV::normaliserVecteur(Matrice *forcesPropulseurs, double vitesse)
{
	double valeur_max = 0;

	for(int a = 0 ; a < forcesPropulseurs->nombreLignes() ; a ++)
		if(abs(valeur_max) < abs((*forcesPropulseurs)[a][0]))
			valeur_max = (*forcesPropulseurs)[a][0];

	if(valeur_max > 1.0) // On ne normalise que les valeurs superieures à 100%
		for(int a = 0 ; a < forcesPropulseurs->nombreLignes() ; a ++)
			for(int b = 0 ; b < forcesPropulseurs->nombreColonnes() ; b ++)
				(*forcesPropulseurs)[a][b] /= valeur_max / ((min(abs(vitesse), 100.0) / 100.0) * this->vitesseLimiteFixee);
}

/**
 * \fn
 * \brief Méthode retournant le vecteur consigne représentant les forces à appliquer sur le véhicule
 */
 
Matrice* AUV::getVecteurConsigneAUV()
{
	Matrice *consigneAUV = new Matrice(6, 1, "Consignes AUV");
	
	// Les deux cas Rx et Ry sont impossibles avec ces AUV
	(*consigneAUV)[0][0] = this->Vx;
	(*consigneAUV)[1][0] = this->Vy;
	(*consigneAUV)[2][0] = this->Vz;
	(*consigneAUV)[3][0] = 0;
	(*consigneAUV)[4][0] = 0;
	(*consigneAUV)[5][0] = this->Rz;
	
	return consigneAUV;
}

/**
 * \fn
 * \brief Méthode retournant le vecteur représentant les forces que chaque propulseur doit fournir
 */
 
Matrice* AUV::getForcesPropulseurs()
{
	Matrice *consigneAUV = getVecteurConsigneAUV();
	Matrice *forcesPropulseurs = new Matrice(6, 1, "Consignes propulseurs");
	*forcesPropulseurs = (*this->E) * (*consigneAUV); // Produit matriciel pour trouver les solutions
	return forcesPropulseurs;
}

/**
 * \fn
 * \brief Méthode de mise à jour des vitesses des propulseurs
 * 
 * A appeler à chaque changement de consigne sur l'AUV
 */

int AUV::updatePropulseurs()
{
	Matrice *consigneAUV = getVecteurConsigneAUV();
	Matrice *forcesPropulseurs = getForcesPropulseurs();
	double vitesseGlobale = max(max(max(abs(this->Vx), abs(this->Vy)), abs(this->Vz)), abs(this->Rz));
	this->normaliserVecteur(forcesPropulseurs, vitesseGlobale); // Normalisation des puissances

	if(AFFICHER_DETAILS)
	{
		cout << endl << "\tNouvelle consigne : vitesse = " << vitesseGlobale << endl;
		consigneAUV->afficher();
		forcesPropulseurs->afficher();
	}

	if(!this->mode_virtuel && this->getOn())
	{
		// Application de chacune des forces aux 6 propulseurs
		if(this->mb->setPropFrRi(this->propFrRi->affinerPuissance((*forcesPropulseurs)[0][0])) == -1) // F1
		{
			cout << termColor("red") << "Erreur sur propulseur FrRi" << termColor() << endl;
			return -1;
		}

		if(this->mb->setPropFrLe(this->propFrLe->affinerPuissance((*forcesPropulseurs)[3][0])) == -1) // F4
		{
			cout << termColor("red") << "Erreur sur propulseur FrLe" << termColor() << endl;
			return -1;
		}

		if(this->mb->setPropReRi(this->propReRi->affinerPuissance((*forcesPropulseurs)[1][0])) == -1) // F2
		{
			cout << termColor("red") << "Erreur sur propulseur ReRi" << termColor() << endl;
			return -1;
		}

		if(this->mb->setPropReLe(this->propReLe->affinerPuissance((*forcesPropulseurs)[2][0])) == -1) // F3
		{
			cout << termColor("red") << "Erreur sur propulseur ReLe" << termColor() << endl;
			return -1;
		}

		if(this->mb->setPropVert((*forcesPropulseurs)[4][0]) == -1) // F5 et F6
		{
			cout << termColor("red") << "Erreur sur propulseurs verticaux" << termColor() << endl;
			return -1;
		}

		int err = this->mb->updateAll();
		if(err == -1)
			cout << termColor("red") << "Erreur sur la mise a jour des propulseurs" << termColor() << endl;

		return err;
	}

	else
		return 1;
}

/**
 * \fn
 * \brief Méthode témoignant de l'activation du modbus
 */

bool AUV::getOn()
{
	return this->on;
}

/**
 * \fn
 * \brief Méthode définissant une nouvelle vitesse selon X
 */

int AUV::setVx(double val)
{
	if(this->Vx == val)
		return 1;
		
	this->Vx = val;
	return updatePropulseurs();
}

/**
 * \fn
 * \brief Méthode définissant une nouvelle vitesse selon Y
 */

int AUV::setVy(double val)
{
	if(this->Vy == val)
		return 1;
		
	this->Vy = val;
	return updatePropulseurs();
}

/**
 * \fn
 * \brief Méthode définissant une nouvelle vitesse selon Z
 */

int AUV::setVz(double val)
{
	if(this->Vz == val)
		return 1;
		
	this->Vz = val;
	return updatePropulseurs();
}

/**
 * \fn
 * \brief Méthode définissant une nouvelle rotation selon Z (= Yaw)
 */

int AUV::setRz(double val)
{
	if(this->Rz == val)
		return 1;
		
	this->Rz = val;
	return updatePropulseurs();
}

/**
 * \fn
 * \brief Méthode définissant une nouvelle rotation selon Z (= Yaw)
 */

int AUV::setYaw(double val)
{
	return this->setRz(val);
}

/**
 * \fn
 * \brief Méthode retournant la profondeur à laquelle se trouve l'AUV
 */

double AUV::getProfondeur()
{
	return this->mb->getDepth();
}

/**
 * \fn
 * \brief Méthode retournant la direction prise par l'AUV (boussole, expression en degrés)
 */

double AUV::getCap()
{
	return this->mb->getDirec() / 10.0;
}

/**
 * \fn
 * \brief Méthode retournant l'intensité de la batterie 1 (extérieure) de l'AUV
 */

double AUV::getIntensiteBatterie1()
{
	return this->mb->getI1();
}

/**
 * \fn
 * \brief Méthode retournant la tension de la batterie 1 (extérieure) de l'AUV
 */

double AUV::getTensionBatterie1()
{
	return this->mb->getU1() / 1000.0;
}

/**
 * \fn
 * \brief Méthode retournant la consommation de la batterie 1 (extérieure) de l'AUV
 */

double AUV::getConsommationBatterie1()
{
	return this->mb->getCapBat1();
}

/**
 * \fn
 * \brief Méthode retournant l'intensité de la batterie 2 (extérieure) de l'AUV
 */

double AUV::getIntensiteBatterie2()
{
	return this->mb->getI2();
}

/**
 * \fn
 * \brief Méthode retournant la tension de la batterie 2 (extérieure) de l'AUV
 */

double AUV::getTensionBatterie2()
{
	return this->mb->getU2() / 1000.0;
}

/**
 * \fn
 * \brief Méthode retournant la consommation de la batterie 2 (extérieure) de l'AUV
 */

double AUV::getConsommationBatterie2()
{
	return this->mb->getCapBat2();
}

/**
 * \fn
 * \brief Méthode permettant l'allumage des projecteurs frontaux de l'AUV
 */

int AUV::allumerProjecteurs()
{	
	cout << "Allumage des projecteurs de l'AUV" << endl;
	this->mb->setLight(1000);
	/*if(this->mb->turnLightOn(1) == -1)
	{
		cout << "Erreur sur allumage des projecteurs" << endl;
		return -1;
	}*/
	
	return 1;
}

/**
 * \fn
 * \brief Méthode permettant l'extinction des projecteurs frontaux de l'AUV
 */

int AUV::eteindreProjecteurs()
{
	cout << "Extinction des projecteurs de l'AUV" << endl;
	
	if(this->mb->turnLightOff() == -1)
	{
		cout << "Erreur sur extinction des projecteurs" << endl;
		return -1;
	}
	
	return 1;
}

/**
 * \fn
 * \brief Destructeur de la classe AUV
 */

AUV::~AUV()
{
	delete this->mb;
}
