#ifndef AUTOM_H
#define AUTOM_H

#include "MOOS/libMOOS/MOOSLib.h"
#include "state.h"
#include <string>

class Autom{

	public :

		Autom();
		void transition();
		void doing();
		void onNewMes(CMOOSMsg mes);
		State* get_State();
		void getData(std::string scanline, unsigned int *raw);
		void getSize(std::string data, std::string *scanline, float *heading, float * dist, int *size);

	private:

		State *		state;
		unsigned char wall;
};

#endif // AUTOM_H
