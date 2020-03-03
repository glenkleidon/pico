
#include "picoutils.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>

char *assign_string(char *targetstr, const char *sourcestr)
{
  if (targetstr)
  {
      if (strlen(targetstr)>0) free(targetstr);
  }
  int sz = strlen(sourcestr);
  targetstr = malloc(sizeof(char)*(1+sz));
  targetstr[sz]='\0';
  strncpy(targetstr, sourcestr, sz);
  return targetstr;
}
