#include "list.h"
#include <stdio.h>

#include "logging.h"

#include <stdlib.h>

int main()
{
  int error = 0;

  List list = {};

  initList(&list, 20);

  FILE *file = getLogFile();

  dumpList(&list, validateList(&list), file);

  for (int i = 0; i < 15; ++i)
    {
      index_t index = list_pushFrontElement(&list, &i, &error);

      printf("1: %d: %d, %d\n", i, index, error);

      dumpList(&list, validateList(&list), file);
    }

  for (int i = 10; i < 15; ++i)
    {
      index_t index = list_insertElement(&list, rand() % 10 + 1, &i, &error);

      printf("2: %d: %d, %d\n", i, index, error);

      dumpList(&list, validateList(&list), file);
    }

  for (int i = 0; i < 5; ++i)
    {
      element_t el = 0;

      list_removeElement(&list, rand()%10 + 1, &el, &error);

      printf("3: %d: %d, %d\n", i, el, error);

      dumpList(&list, validateList(&list), file);
    }

  list_resize(&list, 1, 16);

  dumpList(&list, validateList(&list), file);

  list_restoreLinearity(&list);

  dumpList(&list, validateList(&list), file);

  for (int i = 0; i < 15; ++i)
    {
      element_t el = 0;

      list_popBackElement(&list, &el, &error);

      printf("4: %d: %d, %d\n", i, el, error);

      dumpList(&list, validateList(&list), file);
    }

  dumpList(&list, validateList(&list), file);

  destroyList(&list);

  return 0;
}
