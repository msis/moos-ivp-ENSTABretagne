/**
 * \file AUV.h
 * \brief Classe AUV
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Classe matérialisant un AUV de type Ciscrea
 *
 */

#ifndef AUV_H
#define AUV_H

#include <cmath>
#include "Propulseur.h"
#include "../common/constantes.h"
#include "Matrice.h"
#include "M6dbus.h"

#define	PORT_MODBUS			502
#define AFFICHER_DETAILS    1

using namespace std;

class AUV
{
	public:

	protected:
		M6dbus *mb;
		Matrice *E;
		int identifiantAUV;
		bool on, mode_virtuel;
		double Vx, Vy, Vz, Rz;
		float vitesseLimiteFixee;
		Propulseur *propFrRi, *propFrLe, *propReRi, *propReLe;
	
	public:
		AUV();
		AUV(M6dbus *m);
		AUV(int AUVnum);
		string getIP();
		string getNom();
		int getIdentifiant();
		bool getOn();
		~AUV();
		
		// Commandes du Ciscrea :
		int setVx(double);
		int setVy(double);
		int setVz(double);
		int setRz(double);
		int setYaw(double);
		int allumerProjecteurs();
		int eteindreProjecteurs();
		
		// Retours du Ciscrea :
		double getProfondeur();
		double getCap();
		double getIntensiteBatterie1();
		double getTensionBatterie1();
		double getConsommationBatterie1();
		double getIntensiteBatterie2();
		double getTensionBatterie2();
		double getConsommationBatterie2();
		
		Matrice* getVecteurConsigneAUV();
		Matrice* getForcesPropulseurs();
	
	protected:
		void creerMatriceE();
		void initialisation();
		int updatePropulseurs();
		void normaliserVecteur(Matrice *forcesPropulseurs, double vitesse);
};

#endif // AUV_H
