#include "common.h"


//---------------------------------------------------------------------------
char* fgets2(FILE* f,char* line)   //Afin d'ignorer les lignes qui commencent par %
{ char *r;
  do   { r=fgets(line,256,f);}  while ((line[0]=='%')&&(r!= NULL));
  return r;
}
//---------------------------------------------------------------------------
