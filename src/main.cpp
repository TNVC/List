#include "list.h"
#include <stdio.h>

#include "logging.h"

int main()
{
  List list = {};

  initList(&list, 10);

   FILE *file = getLogFile();

  dumpList(&list, validateList(&list), file);

  for (int i = 0; i < 11; ++i)
    {
      index_t index = list_pushBackElement(&list, &i);

      printf("%d: %d\n", i, index);

      dumpList(&list, validateList(&list), file);
    }

  for (int i = 0; i < 11; ++i)
    {
      element_t el = 0;

      list_popBackElement(&list, &el);

      printf("%d: %d\n", i, el);

      dumpList(&list, validateList(&list), file);
    }

  //  dumpList(&list, validateList(&list), file);

  destroyList(&list);

  return 0;
}
