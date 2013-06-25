/**
 * \file Propulseur.h
 * \brief Classe Propulseur
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Classe matérialisant un propulseur sur un AUV
 *
 */

#ifndef PROP_H
#define PROP_H

#include <iostream>
#include <algorithm>
#include <cmath>
#include "../common/constantes.h"

using namespace std;

class Propulseur
{
	public:
		Propulseur(string nom,
					int sens_rotation,
					double angle_orientation,
					double coefficient_correctif,
					double coefficient_compensation_marche_arriere);
		double affinerPuissance(double puissance);
		double getAngle();
		string getNom();
		~Propulseur();

	protected:
		string nom;
		int sens_rotation;
		double angle_orientation;
		double coefficient_correctif;
		double coefficient_compensation_marche_arriere;
};

#endif // PROP_H
