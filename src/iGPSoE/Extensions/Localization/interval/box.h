#ifndef __BOX__
#define __BOX__


#include "interval.h"

using namespace std;


class box
{
public:
	interval* data;
	int dim;

//---------------------CONSTRUCTEURS--------------------------------------------
box ();
box (int);
box (interval,int);
box (interval x);
box (interval x,interval y);
box (interval x,interval y, interval z);
box (const box&);
~box ();
interval& operator[] (int) const ;
box& operator=(const box&);
void Resize (int);
void Intersect(const box& Y);
bool IsEmpty (void) const;
double Width(void);

//----------------------OPERATEURS----------------------------------------------
friend box      operator&(const box&, const box&);
friend box	operator+(const box&,const box&);
friend box	operator-(const box&);
friend box	operator-(const box&,const box&);
friend box	operator*(const interval&,const box&);
friend box      operator*(const double,const box&);
friend box	operator*(const box&,const interval&);
friend ostream& operator<<(ostream&, const box&);
//-------------------FONCTIONS MEMBRES---------------------------------------

friend double       Angle             (box& ,box&); // Il faut des vecteurs de dim 2
friend int          Size              (const box&);
friend int	    AxePrincipal      (box&);
friend int	    AxePrincipal      (box&, vector<int>&);
friend int	    AxePrincipal      (box&, box&);
friend void         Update            (box&);
friend void	    Bisect	      (box&, box&, box&);
friend void	    Bisect	      (box&, box&, box&, vector<int>&);
friend void	    Decoup	      (box&, box&,box&);
friend void         Trisect           (box&, box&, box&, box&);
friend void	    Bisect	      (box&, box&,box&,box&);
friend void	    BisectAlong	      (box&, box&,box&,int);
friend void	    DecoupAlong	      (box&, box&,box&,int);
friend void         TrisectAlong      (box&,box&,box&,box&,int);
friend void         BisectHere        (box& ,box&,box&,int,reel);
friend box          Rand              (const box& X);
friend box	    Center	      (const box&);
friend box          Center            (const box&, vector<int>&);
//friend void	    CheckRange	      (box&,box&);
friend interval     Determinant       (box&, box&);
friend box          EmptyBox          (const box&);
friend bool	    Disjoint	      (const box&,const box&);
friend reel         decrease          (const box&, const box&);
friend reel         decrease          (const box&, const box&, vector<int>);
friend reel         Eloignement       (box&,box&);
friend reel         Eloignement2      (box&,box&);
friend reel         EloignementRelatif2(box&,box&);
friend reel         Marge             (box,box);
friend iboolean	    In		      (box,box);
friend box	    Inf 	      (box);
friend box	    Inflate 	      (box&,reel);
friend box	    Inter 	      (const box&,const box&);
friend box          Inter             (vector<box>&);
friend box          Union             (vector<box>&);
friend box          Concat            (const box&, const box&);
friend box          Proj              (const box&, int, int);
//friend void       Inter1            (box&,box&,const box&,const box&,const box&);
friend interval     Norm              (box);
friend interval     NormEuclid        (box, box);
friend interval     NormInf           (box, box);
friend interval     ProduitScalaire   (box&,box&);
friend bool         Prop              (box&,box&);
friend bool	    Subset	      (box&,box&);
friend bool         SubsetStrict      (box&,box&);
friend bool	    Subset	      (box&,box&,reel);
friend bool         IsBox             (box);
friend void         Sucre             (box&,box&);
friend box	    Sup 	      (box);
friend box	    Union	      (box&,box&);
friend reel         Volume            (box&);
friend reel	    Width	      (box&);
friend reel	    Width	      (box&,vector<int>&);
friend reel	    Width	      (box&,box&);
friend box          Zeros             (int);
friend box          Empty             (int);
friend box          Infinity          (int);


friend void Cplus  (box&, box&, box&, int sens);
friend void Cmoins (box&, box&, box&, int sens);
friend void C_q_in (box&, int, vector<box>&);
friend void Cnorm(interval&R, box& X);
friend void Cdistance(interval& R, box& X, box& Y);
friend void CProdScalaire(interval& R, box& X, box& Y);
friend void COrtho(box& X, box& Y);
friend void CProd(box& Y, interval& a, box& X, int sens);



};

box          Empty             (int);

#endif
