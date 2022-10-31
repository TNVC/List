#include "list.h"
#include <stdio.h>

#include "logging.h"
#include <stdlib.h>

int main()
{
  int error = 0;

  List list = {};

  initList(&list, 10);

  FILE *file = getLogFile();

  dumpListWithMessage(&list, validateList(&list), file, "Start");

  for (int i = 6; i < 12; ++i)
    {
      index_t index = list_pushBackElement(&list, &i, &error);

      printf("1.1: %d: %d, %d\n", i, index, error);

      dumpListWithMessage(&list, validateList(&list), file, "Push back");
    }

  for (int i = 0; i < 6; ++i)
    {
      index_t index = list_pushFrontElement(&list, &i, &error);

      printf("1: %d: %d, %d\n", i, index, error);

      dumpListWithMessage(&list, validateList(&list), file, "Push front");
    }


  for (int i = 10; i < 15; ++i)
    {
      index_t in = rand() % 10 + 1;

      index_t index = list_insertElement(&list, in, &i, &error);

      printf("2: %d: %d, %d\n", i, index, error);

      dumpListWithMessage(&list, validateList(&list), file, "Insert index: %d", in);
    }

  for (int i = 0; i < 5; ++i)
    {
      element_t el = 0;

      index_t index = rand()%10 + 1;

      list_removeElement(&list, index, &el, &error);

      printf("3: %d: %d, %d\n", i, el, error);

      dumpListWithMessage(&list, validateList(&list), file, "Remove index: %d", index);
    }

  //list_resize(&list, 16, 1);

  //dumpListWithMessage(&list, validateList(&list), file, "Resize");

  list_restoreLinearity(&list);

  dumpListWithMessage(&list, validateList(&list), file, "Line alive");

  for (int i = 0; i < 15; ++i)
    {
      element_t el = 0;

      list_popBackElement(&list, &el, &error);

      printf("4: %d: %d, %d\n", i, el, error);

      dumpListWithMessage(&list, validateList(&list), file, "Pop back");
    }

  dumpListWithMessage(&list, validateList(&list), file, "End");

  destroyList(&list);

  return 0;
}
