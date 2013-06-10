/**
 * \file Matrice.cpp
 * \brief Classe Matrice
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Classe de manipulation simples de matrices
 *
 */

#include "Matrice.h"

/**
 * \fn
 * \brief Constructeur de la classe Matrice
 */

Matrice::Matrice(int nombre_lignes, int nombre_colonnes, string nom)
{
	colonnes = nombre_colonnes;
	lignes = nombre_lignes;

	nom_matrice = nom;

	// Création de la matrice : allocation dynamique du double tableau
	tableau = new double*[lignes];
	if(tableau == NULL) { cout << "Erreur d'allocation de la matrice (étape 1)." << endl; exit(1); }

	for(int i = 0 ; i < lignes ; i ++)
	{
		tableau[i] = new double[colonnes];
		if(tableau[i] == NULL) { cout << "Erreur d'allocation de la matrice (étape 2)." << endl; exit(1); }
	}
}

/**
 * \fn
 * \brief Méthode initialisant les valeurs de la matrice à 0
 */

void Matrice::initialisation()
{
	for(int a = 0 ; a < lignes ; a ++)
	{
		for(int b = 0 ; b < colonnes ; b ++)
		{
			tableau[a][b] = 0;
		}
	}
}

/**
 * \fn
 * \brief Définition permettant d'accéder au contenu d'une matrice depuis l'extérieur de la classe
 */

double * & Matrice::operator[](int rang)
{
	return tableau[rang];
}

/**
 * \fn
 * \brief Méthode permettant de changer le nom associé à la matrice
 */

void Matrice::changerNom(string nouveau_nom)
{
	nom_matrice = nouveau_nom;
}

/**
 * \fn
 * \brief Surcharge de l'opérateur '*'
 */

Matrice Matrice::operator * (const Matrice &X)
{
	double somme;

	// On créé une matrice produit temporaire qui sera renvoyée à la fin de la méthode
	Matrice produit(lignes, X.colonnes, "Produit temporaire");

	for(int i = 0 ; i < produit.lignes ; i ++)
	{
		for(int j = 0 ; j < produit.colonnes ; j ++)
		{
			somme = 0;

			for(int k = 0 ; k < produit.lignes ; k ++)
			{
				somme += tableau[i][k] * X.tableau[k][j];
			}

			produit[i][j] = somme;
		}
	}

	return produit;
}

/**
 * \fn
 * \brief Surcharge de l'opérateur '+'
 */

Matrice Matrice::operator + (const Matrice &X)
{
	if(lignes != X.lignes || colonnes != X.colonnes)
	{
		cout << "\tErreur dans les dimensions : calcul d'une somme matricielle." << endl;
		exit(1);
	}

	// On créé une matrice somme temporaire qui sera renvoyée à la fin de la méthode
	Matrice somme(lignes, colonnes, "Somme temporaire");

	for(int i = 0 ; i < lignes ; i++)
	{
		for(int j = 0 ; j < colonnes ; j++)
		{
			somme.tableau[i][j] = tableau[i][j] + X.tableau[i][j];
		}
	}

	return somme;
}

/**
 * \fn
 * \brief Surcharge de l'opérateur '='
 */

void Matrice::operator = (const Matrice &X)
{
	if(lignes != X.lignes || colonnes != X.colonnes)
	{
		cout << "\tErreur dans les dimensions : opérateur '='" << endl;
		exit(1);
	}

	for(int i = 0 ; i < lignes ; i ++)
		for(int j = 0 ; j < colonnes ; j ++)
			tableau[i][j] = X.tableau[i][j];

	// On ne va pas jusqu'à attribuer le même nom pour les deux matrices
}

/**
 * \fn
 * \brief Méthode retournant le nombre de lignes de la matrice
 */

int Matrice::nombreLignes()
{
	return lignes;
}

/**
 * \fn
 * \brief Méthode retournant le nombre de colonnes de la matrice
 */

int Matrice::nombreColonnes()
{
	return colonnes;
}

/**
 * \fn
 * \brief Méthode inversant (ou pseudo-inversant) la matrice
 */

void Matrice::inverser()
{
	// Utilisation d'OpenCV
	Mat copie(this->lignes, this->colonnes, CV_64FC1);

	for(int a = 0 ; a < lignes ; a ++)
		for(int b = 0 ; b < colonnes ; b ++)
			copie.at<double>(a,b)=tableau[a][b];

	Mat inverse(this->colonnes, this->lignes, CV_64FC1); // Dimensions inverses
	invert(copie, inverse, cv::DECOMP_SVD); // Inversion

	for(int a = 0 ; a < this->nombreLignes() ; a ++)
		for(int b = 0 ; b < this->nombreColonnes() ; b ++)
			tableau[a][b] = inverse.at<double>(a,b);

	this->nom_matrice += " pseudo inverse";
}

/**
 * \fn
 * \brief Méthode affichant la matrice (affichage amélioré et ordonné)
 */

void Matrice::afficher()
{
 /* Cette méthode permet un affichage élaboré de la matrice :
 * Le nom de la matrice est affiché, et ses valeurs sont ordonnées en fonction
 * du nombre de chiffres significatifs et du signe de chaque valeur de la matrice.
 *
 * La longueur des chaines représentatives de chaque valeur est calculée pour
 * connaitre la dimension de chaque tabulation à afficher. */

    int longueur_maximale = 0; // Récupère la longueur maximale trouvée parmi les valeurs de la matrice

    // Variables permettant le calcul de longueurs
    string affichage;
    ostringstream alias; // Flux

    cout << endl << nom_matrice + " :" << endl;

    // On récupère l'élément le plus long de la matrice
    for(int a = 0 ; a < lignes ; a ++)
    {
        for(int b = 0 ; b < colonnes ; b ++)
        {
            if(fabs(tableau[a][b]) < EPSILON) // A partir d'un certain rang fixé, on admet que le terme est nul
                alias << 0;

            else
                alias << tableau[a][b]; // On enregistre la valeur de la case [a][b] dans le flux

            affichage = alias.str(); // On inclut le flux dans la chaine de caractères
            alias.str(""); // On vide le flux pour les prochains calculs de longueur

            // Enfin, on compare la longueur de la chaine actuelle avec la longueur maximale précédemment trouvée
            if((int)affichage.length() > (int)longueur_maximale)
                longueur_maximale = affichage.length();
        }
    }

    // Cette nouvelle boucle affiche la matrice en incluant plus ou moins d'espaces entre ses valeurs
    for(int a = 0 ; a < lignes ; a ++)
    {
        cout << " ";

        if(a == 0)
            cout << "¦"; // ┌

        else if(a == lignes - 1)
            cout << "¦"; // L

        else
            cout << "¦";

        cout << " ";

        for(int b = 0 ; b < colonnes ; b ++)
        {
            if(fabs(tableau[a][b]) < EPSILON)
                alias << 0;

            else
                alias << tableau[a][b];

            affichage = alias.str();
            alias.str("");

            // On complète par des espaces
            for(int i = affichage.length() ; i < longueur_maximale ; i ++)
                cout << " ";

            cout << affichage;

            cout << " ";
        }

        if(a == 0)
            cout << "¦"; // ┐

        else if(a == lignes - 1)
            cout << "¦"; // ┘

        else
            cout << "¦";

        cout << endl;
    }
}

/**
 * \fn
 * \brief Destructeur de la classe Matrice
 */

Matrice::~Matrice()
{
	// On libère l'espace alloué par la matrice
	for(int i = 0; i < lignes ; i++)
		delete tableau[i];

	delete tableau;
}
