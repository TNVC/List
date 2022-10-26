#include "list.h"
#include <stdio.h>

#include "logging.h"

#include <stdlib.h>

int main()
{
  List list = {};

  initList(&list, 15);

   FILE *file = getLogFile();

  dumpList(&list, validateList(&list), file);

  for (int i = 0; i < 10; ++i)
    {
      index_t index = list_pushBackElement(&list, &i);

      printf("%d: %d\n", i, index);

      dumpList(&list, validateList(&list), file);
    }

  for (int i = 0; i < 5; ++i)
    {
      index_t index = list_insertElement(&list, rand() % 10 + 1, &i);

      printf("%d: %d\n", i, index);

      dumpList(&list, validateList(&list), file);
    }

  for (int i = 0; i < 5; ++i)
    {
      element_t el = 0;

      list_removeElement(&list, rand()%15 + 1, &el);

      printf("%d: %d\n", i, el);

      dumpList(&list, validateList(&list), file);
    }

  list_restoreLinearity(&list);

  dumpList(&list, validateList(&list), file);

  for (int i = 0; i < 10; ++i)
    {
      element_t el = 0;

      list_popBackElement(&list, &el);

      printf("%d: %d\n", i, el);

      dumpList(&list, validateList(&list), file);
    }

  dumpList(&list, validateList(&list), file);

  destroyList(&list);

  return 0;
}
