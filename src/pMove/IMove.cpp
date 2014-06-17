#include "IMove.h"
#include <string>
#include <stdio.h>

void IMove::myNotify(std::string key, double arg){
  printf("erreur appel du notufy interface\n");
}

void IMove::myNotify(std::string key, std::string arg){
  printf("erreur appel du notufy interface\n");
}