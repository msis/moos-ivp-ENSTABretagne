#ifndef OBSERVERH
#define OBSERVERH


#include "common.h"


//---------------------------------------------------------------------------
class observer
{
public:
        observer();
        void DrawDiagram();
        bool Contract(box&);
        void DrawChenille(double xhat,double yhat, double thetahat, double alpha);
        box SIVIA(box& X0, box& Xsecours);


        bool clock(double u1,double u2,interval d,double alpha, interval boussole);

        void Incremente(interval& X1,interval& Y1, interval& Theta1,interval& V1,
                        interval& X,interval& Y,interval& Theta,interval& V,double u1,double u2);
        void Decremente(interval&,interval&,interval&,interval&,
                interval& X,interval& Y,interval& Theta,interval& V,double u1,double u2);
        int N_window;
        int N_outliers;
        int N_Bissect_0;
        int N_Bissect_k;
        double dt;
        double ws;
        double coef_friction;
        double coef_puissance_moteur;
        double coef_ecart_moteur;
        double xmin,xmax,ymin,ymax;
        double thetamin,thetamax;
        double vmax0,vmax;
        int N_murs;
        vector <double> murs_xa,murs_xb,murs_ya,murs_yb;
        int N_cercles;
        vector <double> cercles_x,cercles_y,cercles_r;

        box Xprior0;
        box Xpriork;
        box Xsecours;

        int phase;

        list<double> Lu1;
        list<double> Lu2;
        list<double> Lalpha;
        list<interval> Ld;
        list<box> LXhat;
        list<double> chenillex;
        list<double> chenilley;
        list<double> chenilletheta;
        list<double> chenillealpha;
        list<double> chenilled;
        box Xhat;
};
//---------------------------------------------------------------------------
#endif


