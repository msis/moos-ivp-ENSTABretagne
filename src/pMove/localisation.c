#include "localisation.h"
#include <stdio.h>
#include <math.h>

#define pi M_PI
#undef DEBUG // permet d'afficher sur une console des infos
#undef MESURE // permet d'enregistrer les données dans une fichier

int partitionner(float *T, int premier, int dernier, int pivot){
     float tamp=T[pivot];
     T[pivot]=T[dernier];
     T[dernier]=tamp;
     int i;
     int j = premier;
     for(i = premier; i< dernier; i++){
         if(T[i] <= T[dernier]){
             float temp=T[i];
             T[i]=T[j];
             T[j]=temp;
             j++;
        }
    }
     float t=T[dernier];
     T[dernier]=T[j];
     T[j]=t;
     return j;
}

 void tri_rapide(float *t, int premier, int dernier){
     if( premier < dernier ){
        srand(time(NULL));
        int pivot = rand()%(dernier-premier+1)+premier;
        pivot = partitionner(t,premier, dernier,pivot);
        tri_rapide(t,premier,pivot-1);
        tri_rapide(t,pivot+1,dernier);
    }
}

float mediane(float *tab,int t){
    tri_rapide(tab,0,t);
    if(t%2==0) return ((tab[t/2]+tab[t/2-1])/2);
    else return tab[(t+1)/2];
}

// calcule la transformé de hough en polaire un jeux de donnée x et y de taille size
int hough_pol(float *x, float *y, int debut, int fin, float *residu, float *theta, float *dist){
	int i;
	
    double a,b,c,d;
	a = 0; b = 0; d = 0;
	
    for( i = debut; i < fin; i++){
		
		a = a + x[i]*x[i];
		b = b + x[i]*y[i];
		d = d + y[i]*y[i];
	}
	c = b;
	
	if( a*d-b*c == 0){
	
		return -1;
	}
    else{
        double undet = (a*d-b*c);
        double inva = d /undet;
        double invb = -b /undet;
        double invc = -c /undet;
        double invd = a /undet;
		
        double c1 = 0;
        double c2 = 0;
		
        for( i = debut; i < fin; i++){
			
			c1 = c1 + (inva * x[i]) + (invb * y[i]);
			c2 = c2 + (invc * x[i]) + (invd * y[i]);
		}

        //printf("coef %f | %f  : %f %f %f %f %f\n", c1, c2, inva, invb, invc, invd, undet);
		
		*theta = atan2(c2, c1);
        if( cos( *theta) > 0){
			*dist = cos( *theta)/ c1;
		}
		else{
            *dist = sin( *theta)/ c2;
		}
		
		*residu = 0;
		for( i = debut; i < fin; i++){
			
            *residu = *residu + sqrt( (c2*y[i] + c1*x[i] - 1)  * (c2*y[i] + c1*x[i] - 1) );
		}
		
		return 1;
	}
}


void loc_vect(float *rho, float *tt, int size, PARAM *param, POS *pos, float cap){
	
	int i;
	float x[size];
	float y[size];
	float bas[size];
	float gauche[size];
	float droite[size];
	int ib = 0; int ig = 0; int id = 0;
	
	
#ifdef MESURE
		char pat[200];
		sprintf(pat,"mesure_loc_vect_%d",param->id);
		FILE * f = fopen(pat,"w");			
#endif
	
	for( i = 0; i < size; i++){
			x[i] = cos( tt[i] - cap) * rho[i];
			if( x[i] < 0){
				bas[ib] = x[i];
				ib++;
			}
			y[i] = sin( tt[i] - cap) * rho[i];
			if( y[i] < 0){
				gauche[ig] = y[i];
				ig++;
			}
			else{
				droite[id] = y[i];
				id++;
			}
#ifdef MESURE
			fprintf( f, "%f,%f,%f,%f\n", tt[i], rho[i], x[i], y[i]);
#endif
	}
	
#ifdef MESURE
		fclose(f);
		param->id = param->id +1;
#endif
	
	float med_bas = mediane(bas, ib);
	float med_gauche = mediane(gauche, ig);
	float med_droite = mediane(droite, id);
	
	int iB = 0;
	float pbas[ib];
	
	for( i = 0; i < ib; i++){
		if( bas[i] < med_bas/2){
			pbas[iB] = -bas[i];
			iB++;
		}
	}
	
	int iX = 0;
	float pX[ig + id];
	
	for( i = 0; i < ig; i++){
		if( gauche[i] < med_gauche/2){
			pX[iX] = -gauche[i];
			iX++;
		}
	}
	
	for( i = 0; i < id; i++){
		if( droite[i] > med_droite/2){
			pX[iX] = param->xmax - droite[i];
			iX++;;
		}
	}
	
	pos->x = mediane(pX, iX);
	pos->y = mediane(pbas, iB);
}

void loc_hough(float *rho, float *tt, int size, PARAM *param, POS *pos, float cap){
	
	int i;
	float x[size];
	float y[size];
#ifdef MESURE
		char pat[200];
		sprintf(pat,"mesure_loc_houg_%d",param->id);
		FILE * f = fopen(pat,"w");			
#endif
	for( i = 0; i < size; i++){
			x[i] = cos( tt[i]) * rho[i];
			y[i] = sin( tt[i]) * rho[i];
#ifdef MESURE
			fprintf( f, "%f,%f,%f,%f\n", tt[i], rho[i], x[i], y[i]);
#endif

	}
	
#ifdef MESURE
		fclose(f);
		param->id = param->id +1;
#endif

	int j = 0;
	float theta[size - param->step];
	float dist[size - param->step];
	
	for( i = 0; i < size - param->step; i++){
	
		float residu, thet, dis;
		
		if(hough_pol( x, y, i, i+ param->step, &residu, &thet, &dis) == 1){
			//printf("résidu : %f\n", residu);
			if(residu < 0.2){
			
				theta[j] = thet;
				dist[j] = dis;
				//printf("val : %f %f\n", thet*180/pi, dis);
				j++;
			}
		}
	}
	
	printf("%d droites trouvé\n", j);
	int i1 = 0;
	int i2 = 0;
	float d1[j];
	float d2[j];
	
	for( i = 0; i < j; i ++){
		
		if( cos(theta[i] - cap + pi/2) > 0.9 && sin(theta[i] - cap + pi/2) < 0.1){
			d1[i1] = dist[i];
			i1++;
		}
		else if(cos(theta[i] - cap) > 0.9 && sin(theta[i] - cap) < 0.1){
			d2[i2] = param->xmax - dist[i];
			i2++;
		}
		else if(cos(theta[i] - cap + pi) > 0.9 && sin(theta[i] - cap + pi) < 0.1){
			d2[i2] = dist[i];
			i2++;
		}
/****************************************************************************************\
* Dans le cas où on voit le quatrième mur,  on a une information supplémetaire sur la   *
* coordonnée selon y  																	*
*		else if(cos(theta[i] - cap - pi/2) > 0.9 && sin(theta[i] - cap -pi) < 0.1){		*
*			d1[i1] = param->ymax - dist[i];												*
*			i1++;																		*
*		}																				*
\****************************************************************************************/
	}

	printf("%d valeurs pour le mur 1 et %d pour le mur 2\n", i1, i2);
	
	pos->x = mediane(d2, i2);
	pos->y = mediane(d1, i1);
}

// trouve la position du robot à partir des points mesurer sur les mur grasse à des capteurs
void localisation( float *rho, float *tt, int size, PARAM *param, POS *pos, float cap, int mode){
	
	if(mode == HOUGH){
#ifdef DEBUG
		printf("mode avec alignement\n");
#endif
		loc_hough( rho, tt, size, param, pos, cap);
	}
	else if( mode == VECT){	
#ifdef DEBUG
		printf("mode vectoriel\n");
#endif
		loc_vect( rho, tt, size, param, pos, cap);	
	}
	else{
#ifdef DEBUG
	printf("mauvais parmètre de mode\n");
#endif
	}
	
}

#define STEP 3
#define XMAX 50
#define YMAX 63

int findWall(unsigned char *raw, int size){

	int pr[size];
	int pas = 20;

	int k;
	for( k = 0; k < size; k++){
		if( (k + pas) < size){
			pr[k] = raw[k + pas] - raw[k];
		}
		else{
			pr[k] = raw[size-1] - raw[k];
		}

	}

	int ind = 20;
	unsigned char maxi = 0;

	for( k = 20; k < size; k++){

		if(maxi < -pr[k]){
			ind = k;
			maxi = -pr[k];
		}
	}

	return ind;
}


void findWalls(unsigned char **raw_data, int nl, int nc, float *ang, float *d, POS *pos){

	float rho[nl];

	int k;
	for( k = 0; k < nl; k++){
		rho[k] = findWall(raw_data[k], nc) * d[k];
	}

    PARAM param;
    param.step = STEP;
    param.xmax = XMAX;
    param.ymax = YMAX;
	param.id = 0;


	localisation( rho, ang, nl, &param, pos, -(M_PI/4 + M_PI/12 - M_PI), HOUGH);
}