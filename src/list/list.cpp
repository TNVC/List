#include "list.h"

#include "logging.h"
#include "systemlike.h"

#include "elementfunctions.h"

#define ERROR(...)                                    \
  do                                                  \
    {                                                 \
      if (isPointerCorrect(error))                    \
        *error = -1;                                  \
                                                      \
      return __VA_ARGS__;                             \
    } while (0)

#define CHECK_ERROR(ERR_, ERROR_, ...)             \
  do                                               \
    {                                              \
      if (ERR_ && isPointerCorrect(ERROR_))        \
        {                                          \
          *ERROR_ = ERR_;                          \
                                                   \
          ERROR(__VA_ARGS__);                      \
        }                                          \
    } while (0)

#define CHECK_VALID(LIST, ERROR, ...)               \
  do                                                \
    {                                               \
      unsigned errorCode = validateList(LIST);      \
                                                    \
      if (errorCode)                                \
        {                                           \
          dumpList(LIST, errorCode, getLogFile());  \
                                                    \
          if (isPointerCorrect(ERROR))              \
            *ERROR = (int)errorCode;                \
                                                    \
          return __VA_ARGS__;                       \
        }                                           \
    } while (0)

#define UPDATE_HASH(LIST)                                                \
  do                                                                     \
    {                                                                    \
      LIST->dataHash = getHash(LIST->data, LIST->data + LIST->capacity); \
      LIST->hash     = getHash(LIST, &LIST->hash);                       \
    } while (0)

const index_t POISON_PREV = -1;

static void createDataArray(List *list, size_t capacity, int *error = nullptr);

unsigned validateList(const List *list)
{
  if (!list)
    return LIST_NULL_POINTER;

  if (!isPointerCorrect(list))
    return LIST_INCORRECT_POINTER;

  unsigned error = 0;

  if (!isPointerCorrect(list->data) && list->capacity)
    error |= LIST_NULL_DATA;

  if (list->size >= list->capacity)
    error |= LIST_CAPASITY_LESS_THEN_SIZE;

  if (list_head(list) == 0 && list->size)
    error |= LIST_NOT_HEAD;

  if (!list->free && list->size < list->capacity - 1)
    error |= LIST_NOT_FREE;

#ifdef NEED_CANARY_

  if (list->leftCanary != LEFT_CANARY)
    error |= LIST_LEFT_CANARY_DIED;

  if (list->rightCanary != RIGHT_CANARY)
    error |= LIST_RIGHT_CANARY_DIED;

  if (isPointerCorrect(list->data) && list->capacity)
    {
      if (!checkLeftCanary(list->data))
        error |= LIST_LEFT_DATA_CANARY_DIED;

      if (!checkRightCanary(list->data + list->capacity))
        error |= LIST_RIGHT_DATA_CANARY_DIED;
    }

#endif

#ifdef NEED_HASH_

  if (getHash(list, &list->hash) != list->hash)
    error |= LIST_BROKEN_HASH;

  if (isPointerCorrect(list->data))
    if (getHash(list->data, list->data + list->capacity) != list->dataHash)
      error |= LIST_BROKEN_DATA_HASH;

#endif

  return error;
}

void do_initList(List *list, size_t capacity, DebugInfo info, int *error)
{
  if (!isPointerCorrect(list))
    {
      if (isPointerCorrect(error))
        *error = -1;

      return;
    }

  list->data = nullptr;
  list->capacity = capacity + 1;
  list->size     = 0;
  list->free = nullindex;

#ifdef NEED_CANARY_

  list->leftCanary  = LEFT_CANARY;
  list->rightCanary = RIGHT_CANARY;

#endif

#ifdef NEED_HASH_

  list->hash     = nullhash;
  list->dataHash = nullhash;

#endif

#ifdef DEBUG_BUILD_

  list->info = info;

#endif

  if (list->capacity)
    {
      createDataArray(list, list->capacity, error);

      if (!list->data)
        return;
    }

  UPDATE_HASH(list);

  CHECK_VALID(list, error);
}

void destroyList(List *list, int *error)
{
  CHECK_VALID(list, error);

#ifdef NEED_CANARY_

  if (list->capacity)
    canaryFree(list->data);

#else

  if (list->capacity)
    free(list->data);

#endif

  list->data = nullptr;
  list->capacity = 0;
  list->size     = 0;
  list->free = nullindex;

#ifdef NEED_HASH_

  list->hash     = nullhash;
  list->dataHash = nullhash;

#endif
}

void list_resize(List *list, size_t newCapacity, int restoreLinearity, int *error)
{
  CHECK_VALID(list, error);

  if (restoreLinearity)
    {
      int err = 0;

      list_restoreLinearity(list, newCapacity, &err);

      CHECK_ERROR(err, error);

      UPDATE_HASH(list);

      CHECK_VALID(list, error);

      return;
    }

#ifdef NEED_CANARY_

   Node *temp = (Node *)canaryRecalloc(list->data, newCapacity, sizeof(Node));

#else

  Node *temp = (Node *)      recalloc(list->data, newCapacity, sizeof(Node));

#endif

  if (!temp)
    ERROR();

  list->data = temp;

  if (newCapacity > list->capacity)
    {
      element_t poison = getPoison(list->data[0].elem);

      for (size_t i = 0; i < newCapacity - list->capacity; ++i)
        {
          list->data[list->capacity + i] = Node {
            .elem = poison,
            .next = (list->capacity + i == newCapacity - 1) ?
                    list->free : (index_t)(list->capacity + i + 1),
            .prev = POISON_PREV
          };
        }

      if (list->free == nullindex)
        list->free = (index_t)(newCapacity - list->capacity);
    }

  list->capacity = newCapacity;

  UPDATE_HASH(list);

  CHECK_VALID(list, error);
}

void list_restoreLinearity(List *list, size_t newCapacity, int *error)
{
  CHECK_VALID(list, error);

  if (!list->capacity)
    return;

  if (!newCapacity)
    newCapacity = list->capacity;

#ifdef NEED_CANARY_

  Node *temp = (Node *)canaryMalloc(newCapacity*sizeof(Node));

#else

  Node *temp = (Node *)      malloc(newCapacity*sizeof(Node));

#endif

  if (!temp)
    ERROR();

  element_t poison = getPoison(list->data[0].elem);

  temp[0] = Node {.elem = poison, .next = 1, .prev = (index_t)list->size};

  size_t size = (list->size < newCapacity) ? list->size : newCapacity - 1;

  MutableListIterator iter = getMutableListIterator(list);

  temp[1] = Node {
    .elem = *value(&iter),
    .next = (1 == list->size) ? 1 : 2,
    .prev = 0
  };

  size_t i = 2;

  for (next(&iter); i < size; ++i, next(&iter))
    temp[i] = Node {
      .elem = *value(&iter),
      .next = (index_t)i + 1,
      .prev = (index_t)i - 1
    };

  temp[size] = Node {
    .elem = *value(&iter),
    .next = (size == list->size) ? 0 : (index_t)size + 1,
    .prev = (index_t)i - 1
  };

  set_head(list, 1);

  ++i;

  list->free = (index_t)i;

  for ( ; i < newCapacity - 1; ++i)
    temp[i] = Node {
      .elem = poison,
      .next = (index_t)i + 1,
      .prev = POISON_PREV
    };

  temp[newCapacity - 1] = Node {
    .elem = poison,
    .next = nullindex,
    .prev = POISON_PREV
  };

#ifdef NEED_CANARY_

  canaryFree(list->data);

#else

  free(list->data);

#endif

  list->data = temp;

  list->capacity = newCapacity;

  UPDATE_HASH(list);

  CHECK_VALID(list, error);
}

static void createDataArray(List *list, size_t capacity, int *error)
{
  if (!capacity)
    {
      list->data = nullptr;

      return;
    }

#ifdef NEED_CANARY_

  list->data = (Node *)canaryCalloc(capacity, sizeof(Node));

#else

  list->data = (Node *)      calloc(capacity, sizeof(Node));

#endif

  if (!list->data)
    ERROR();

  element_t poison = getPoison(list->data[0].elem);

  list->data[0] = Node {.elem = poison, .next = 0, .prev = 0};

  for (size_t i = 1; i < capacity; ++i)
    {
      list->data[i] = Node {
        .elem = poison,
        .next = (index_t)i + 1,
        .prev = POISON_PREV
      };
    }

  list->data[capacity - 1] = Node {
    .elem = poison,
    .next = nullindex,
    .prev = POISON_PREV
  };

  list->free = 1;
}

[[nodiscard("Return value need for work with list functions!")]]
index_t list_insertElement(List *list, index_t anchor, element_t *element, int *error)
{
  CHECK_VALID(list, error, nullindex);

  if (list->data[anchor].prev == POISON_PREV)
    ERROR(nullindex);

  if (list->size == list->capacity - 1)
    {
      int err = 0;

      list_resize(list, list->capacity*2, 0, &err);

      CHECK_ERROR(err, error, nullindex);
    }

  index_t firstFreeIndex = list->free;

  list->free = list->data[list->free].next;

  list->data[firstFreeIndex] =
    {
      .elem = *element,
      .next = list->data[anchor].next,
      .prev = anchor
    };

  list->data[list->data[anchor].next].prev = firstFreeIndex;

  list->data[anchor].next = firstFreeIndex;

  if (!list->size)
    set_head(list, firstFreeIndex);

  ++list->size;

  UPDATE_HASH(list);

  CHECK_VALID(list, error, nullindex);

  return firstFreeIndex;
}

[[nodiscard("Return value need for work with list functions!")]]
index_t list_pushBackElement(List *list, element_t *element, int *error)
{
  CHECK_VALID(list, error, nullindex);

  return list_insertElement(list, list_tail(list), element, error);
}

[[nodiscard("Return value need for work with list functions!")]]
index_t list_pushFrontElement(List *list, element_t *element, int *error)
{
  CHECK_VALID(list, error, nullindex);

  index_t result = list_insertElement(list, 0, element, error);

  set_head(list, result);

  UPDATE_HASH(list);

  CHECK_VALID(list, error, nullindex);

  return result;
}

element_t *list_get(const List *list, index_t anchor, element_t *element, int *error)
{
  CHECK_VALID(list, error, nullptr);

  if (anchor == nullindex)
    ERROR(nullptr);

  if (anchor < 0 || list->capacity <= (size_t)anchor)
    ERROR(nullptr);

  if (list->data[anchor].prev == POISON_PREV)
    ERROR(nullptr);

  *element = list->data[anchor].elem;

  CHECK_VALID(list, error, nullptr);

  return element;
}

element_t *list_removeElement(List *list, index_t anchor, element_t *element, int *error)
{
  CHECK_VALID(list, error, nullptr);

  if (!element)
    ERROR(nullptr);

  if (!list->size)
    ERROR(nullptr);

  if (anchor < 0 || list->capacity <= (size_t)anchor)
    ERROR(nullptr);

  if (list->data[anchor].prev == POISON_PREV)
    ERROR(nullptr);

  *element = list->data[anchor].elem;

  index_t next = list->data[anchor].next;
  index_t prev = list->data[anchor].prev;

  list->data[next].prev = prev;
  list->data[prev].next = next;

  if (anchor == list_head(list))
    set_head(list, next);

  list->data[anchor] =
    {
      .elem = getPoison(list->data[0].elem),
      .next = list->free,
      .prev = POISON_PREV
    };

  list->free = anchor;

  --list->size;

  if (!list->size)
    set_head(list, 0);

  UPDATE_HASH(list);

  CHECK_VALID(list, error, nullptr);

  return element;
}

element_t *list_popBackElement(List *list, element_t *element, int *error)
{
  CHECK_VALID(list, error, nullptr);

  return list_removeElement(list, list_tail(list), element, error);
}

element_t *list_popFrontElement(List *list, element_t *element, int *error)
{
  CHECK_VALID(list, error, nullptr);

  return list_removeElement(list, list_head(list), element, error);
}

size_t list_size(const List *list, int *error)
{
  CHECK_VALID(list, error, 0);

  return list->size;
}

size_t list_capacity(const List *list, int *error)
{
  CHECK_VALID(list, error, 0);

  return list->capacity;
}
