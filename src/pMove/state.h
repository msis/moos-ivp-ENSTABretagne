#ifndef STATE_H
#define STATE_H


#include <string>
#include "MBUtils.h"
#include "ColorParse.h"
#include "IMove.h"
#include "math.h"

class State{

	public :	
		virtual void doing();
		virtual State * next_State(unsigned char *wall);
		virtual int arrived();
		virtual void maj_sonar();
		// virtual int need_sonar();

		void set_bas(double a);
		void set_haut(double a);
		void set_gauche(double a);
		void set_droite(double a);
		void set_cap(float a);
		void set_x2reach(double a);
		void set_y2reach(double a);
		void set_x(double a);
		void set_y(double a);
		void set_app(void * lapp);
		void set_objectif(unsigned int objectif);

		float get_cap();
		float get_x();
		float get_y();
		float get_bas();
		float get_haut();
		float get_gauche();
		float get_droite();

		unsigned int lookx;
		unsigned int sens;

	protected:
		double		d_haut;
		double		d_droite;
		double		d_gauche;
		double		d_bas;
		double		x;
		double		y;
		double		x2reach;
		double		y2reach;
		float		cap_robot;

		unsigned int pos_objectif;

		IMove*		app;
};

class Init : public State{

	public :
		virtual void doing();
		virtual State * next_State(unsigned char *wall);
		virtual int arrived();
		virtual void maj_sonar();
};

class REGULX : public State{

	public :
		virtual void doing();
		virtual State * next_State(unsigned char *wall);
		virtual int arrived();
		virtual void maj_sonar();
};

class REGULY : public State{

	public :
		virtual void doing();
		virtual State * next_State(unsigned char *wall);
		virtual int arrived();
		virtual void maj_sonar();
};

class LOC : public State{

	public :
		virtual void doing();
		virtual State * next_State(unsigned char *wall);
		virtual int arrived();
		virtual void maj_sonar();
};

class STRAIGHT : public State{

	public :
		virtual void doing();
		virtual State * next_State(unsigned char *wall);
		virtual int arrived();
		virtual void maj_sonar();
};

class OBJECTIF : public State{

	public :
		virtual void doing();
		virtual State * next_State(unsigned char *wall);
		virtual int arrived();
		virtual void maj_sonar();
};

#endif // STATE_H