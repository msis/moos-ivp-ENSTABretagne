/**
 * \file Propulseur.cpp
 * \brief Classe Propulseur
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Classe matérialisant un propulseur sur un AUV
 *
 */

#include "Propulseur.h"

/**
 * \fn
 * \brief Constructeur de la classe Propulseur
 */

Propulseur::Propulseur(string nom,
                       int sens_rotation,
                       double angle_orientation,
                       double coefficient_correctif,
                       double coefficient_compensation_marche_arriere)
{
	this->nom = nom;
	this->sens_rotation = sens_rotation;
	this->angle_orientation = angle_orientation;
	this->coefficient_correctif = coefficient_correctif;
	this->coefficient_compensation_marche_arriere = coefficient_compensation_marche_arriere;
}

/**
 * \fn
 * \brief Méthode retournant la vitesse que doit produire un propulseur en fonction du cap et de la puissance voulus
 */

double Propulseur::affinerPuissance(double puissance)
{
	return puissance * this->sens_rotation;
}

/**
 * \fn
 * \brief Méthode retournant le nom du propulseur
 */

string Propulseur::getNom()
{
	return this->nom;
}

/**
 * \fn
 * \brief Méthode retournant l'angle (orienté) du propulseur par rapport à son axe principal
 */

double Propulseur::getAngle()
{
	return this->angle_orientation;
}

/**
 * \fn
 * \brief Destructeur de la classe Propulseur
 */

Propulseur::~Propulseur()
{
	
}
