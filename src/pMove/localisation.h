#ifndef LOCALISATION_H
#define LOCALISATION_H

#ifdef __cplusplus
extern "C"{
#endif

#define HOUGH 1
#define VECT 0

typedef struct parametre{
    float xmax;
    float ymax;
    int step;
	int id;

}PARAM;

typedef struct position{
    float x;
    float y;

}POS;

void localisation( float *rho, float *tt, int size, PARAM *param, POS *pos, float cap, int mode);

void findWalls(unsigned char **raw_data, int nl, int nc, float *ang, float *d, POS *pos);

#ifdef __cplusplus
}
#endif

#endif // LOCALISATION_H
