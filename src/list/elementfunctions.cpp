#include "elementfunctions.h"

#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

char *toString(int element)
{
  static char buff[10] = "";

  sprintf(buff, "%4d", element);

  return buff;
}

int getPoison(int element)
{
  return -1;
}
