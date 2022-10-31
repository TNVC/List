#include "list.h"
#include "asserts.h"
#include "logging.h"

#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wcast-qual"

#define CHECK(ITER, ...)                                  \
  do                                                      \
    {                                                     \
      assert(ITER);                                       \
                                                          \
      CHECK_VALID(ITER->list __VA_OPT__(,) __VA_ARGS__);  \
    } while (0)

#define CHECK_VALID(LIST, ...)                              \
  do                                                        \
    {                                                       \
      unsigned errorCode = validateList(LIST);              \
                                                            \
      if (errorCode)                                        \
        {                                                   \
          dumpList(LIST, errorCode, getLogFile());          \
                                                            \
          return __VA_ARGS__ __VA_OPT__(;) (int)errorCode;  \
        }                                                   \
    } while (0)

MutableListIterator getMutableListIterator(List *list)
{
  assert(list);

  MutableListIterator iter{list, list_head(list)};

  return iter;
}

ConstListIterator getConstListIterator(const List *list)
{
  assert(list);

  ConstListIterator iter{(List *)list, list_head(list)};

  return iter;
}

int next(ListIterator *iter)
{
  CHECK(iter, 1);

  iter->current = iter->list->data[iter->current].next;

  return 0;
}

int prev(ListIterator *iter)
{
  CHECK(iter, 1);

  iter->current = iter->list->data[iter->current].prev;

  return 0;
}

int hasNext(ListIterator *iter)
{
  CHECK(iter, 0);

  return iter->list->data[iter->current].next != list_head(iter->list);
}

int hasPrev(ListIterator *iter)
{
  CHECK(iter, 0);

  return iter->current != list_head(iter->list);
}

index_t index(ListIterator *iter)
{
  CHECK(iter, nullindex);

  return iter->current;
}

element_t *value(MutableListIterator *iter)
{
  CHECK(iter, nullptr);

 return &iter->list->data[iter->current].elem;
}

const element_t *value(ConstListIterator *iter)
{
  CHECK(iter, nullptr);

  return &iter->list->data[iter->current].elem;
}
