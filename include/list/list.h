#ifndef LIST_H_
#define LIST_H_

#include "settings.h"

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

#include "debuginfo.h"
#include "canary.h"
#include "hash.h"

/// Node for List
struct Node {
  element_t elem; /// <- Element which contains in it
  index_t   next; /// <- Index of next Node
  index_t   prev; /// <- Index of previous Node
};

/// Chahe-friendly List
struct List {
#ifdef NEED_CANARY_

  canary_t leftCanary; /// <- Left struct canary for check intervention

#endif

  Node *data;       /// <- Dimanic allocate array with Nodes
  size_t capacity;  /// <- Capacity of data
  size_t size;      /// <- Count of elements in data
  index_t free;     /// <- Index of first free cell in free sequence which contains in data

#ifdef NEED_HASH_

  hash_t dataHash; /// <- Hash of data from .data to .data + .capacity
  hash_t hash;     /// <- Hash of this struct from ::leftCanary to ::hash

#endif

#ifdef NEED_CANARY_

  canary_t rightCanary; /// <- Right struct canary for check intervention

#endif

#ifdef DEBUG_BUILD_

  DebugInfo info; /// <- Information of first call initList()

#endif
};

/// Abstract List iterator
struct ListIterator
{
  List *list;      /// <- Parent data struct
  index_t current; /// <- Index of current element in list
};

/// Mutable List iterator
struct MutableListIterator : public ListIterator {};

/// Const List iterator
struct ConstListIterator   : public ListIterator {};

/// Errors` codes which may be return from next functions
enum ListError {
  LIST_NULL_POINTER            = 0x01 <<  0,
  LIST_INCORRECT_POINTER       = 0x01 <<  1,
  LIST_NULL_DATA               = 0x01 <<  2,
  LIST_CAPASITY_LESS_THEN_SIZE = 0x01 <<  3,
  LIST_NOT_HEAD                = 0x01 <<  4,
  LIST_NOT_TAIL                = 0x01 <<  5,
  LIST_LEFT_CANARY_DIED        = 0x01 <<  6,
  LIST_RIGHT_CANARY_DIED       = 0x01 <<  7,
  LIST_LEFT_DATA_CANARY_DIED   = 0x01 <<  8,
  LIST_RIGHT_DATA_CANARY_DIED  = 0x01 <<  9,
  LIST_BROKEN_HASH             = 0x01 << 10,
  LIST_BROKEN_DATA_HASH        = 0x01 << 11,
  LIST_NOT_FREE                = 0x01 << 12,
  LIST_FREE_SEQUENCE_IS_BROKEN = 0x01 << 13,
};

/// Count of errors
const int ERROR_COUNT = 14;

inline index_t list_head(const List *list)
{
  return list->data[nullindex].next;
}

inline index_t list_tail(const List *list)
{
  return list->data[nullindex].prev;
}

inline void set_head(List *list, index_t newHead)
{
  list->data[nullindex].next = newHead;
}

inline void set_tail(List *list, index_t newTail)
{
  list->data[nullindex].prev = newTail;
}

/// Create Mutable List iterator from head
/// @param [in] list Parent data struct
/// @return MutableListIterator set to head
/// @note For empty list return always incorrect iterator
/// @note Don`t check that list is correct
MutableListIterator getMutableListIterator    (List *list);

/// Create Const List iterator from head
/// @param [in] list Parent data struct
/// @return ConstListIterator set to head
/// @note For empty list return always incorrect iterator
/// @note Don`t check that list is correct
ConstListIterator   getConstListIterator(const List *list);

/// Set List iterator to next element in list
/// @param [in] iter Iterator what need to increase
/// @return Error`s code
int next(ListIterator *iter);

/// Set List iterator to previous element in list
/// @param [in] iter Iterator what need to decrease
/// @return Error`s code
int prev(ListIterator *iter);

/// Return true if iteraor has next element
/// @param [in] iter Iterator
/// @return Has next in sequence
/// @note If list is incorrect return false
int hasNext(ListIterator *iter);

/// Return true if iteraor has previous element
/// @param [in] iter Iterator
/// @return Has previous in sequence
/// @note If list is incorrect return false
int hasPrev(ListIterator *iter);

/// Return index of current element
/// @param [in] iter Iterator
/// @return Index of current element
/// @note If list is incorrect return nullindex
index_t index(ListIterator *iter);

/// Return poiter to current iterator element in list
/// @param [in] iter Iterator
/// @return Pointer to element in list
      element_t *value(MutableListIterator *iter);

/// Return poiter to current iterator element in list
/// @param [in] iter Iterator
/// @return Pointer to element in list
const element_t *value(ConstListIterator   *iter);

/// Check List to error
/// @param [in] list List for validate
/// @param [in] deep Need check deep
/// @return Errors` code
/// @note Deep validate takes much time
unsigned validateList(const List *list);

#define initList(LIST, CAPACITY, ...)                                   \
  do_initList(LIST, CAPACITY, DEBUG_INFO(LIST) __VA_OPT__(,) __VA_ARGS__);

/// Constructor for list
/// @param [in] list List for initilizate
/// @param [in] capacity Sart capacity for elements
/// @param [in] info Information about call of this function
/// @param [in/out] error Variable for save errors` code
/// @note Use for init list initList()
void do_initList(List *list, size_t capacity, DebugInfo info, int *error = nullptr);

/// Destructor for list
/// @param list List for destroy
/// @param [in/out] error Variable for save errors` code
void destroyList(List *list, int *error = nullptr);

[[nodiscard("Return value need for work with list functions!")]]
index_t list_insertElement(List *list, index_t anchor, element_t *element, int *error = nullptr);

[[nodiscard("Return value need for work with list functions!")]]
index_t list_pushBackElement(List *list, element_t *element, int *error = nullptr);

[[nodiscard("Return value need for work with list functions!")]]
index_t list_pushFrontElement(List *list, element_t *element, int *error = nullptr);

element_t *list_removeElement(List *list, index_t anchor, element_t *element, int *error = nullptr);

element_t *list_popBackElement(List *list, element_t *element, int *error = nullptr);

element_t *list_popFrontElement(List *list, element_t *element, int *error = nullptr);

element_t *list_get(const List *list, index_t anchor, element_t *element, int *error = nullptr);

/// Get list size of sequence in data
/// @param [in] list List
/// @param [in/out] error Variable for save errors` code
/// @return Size of sequence in .data
size_t list_size(const List *list, int *error = nullptr);

/// Get list capacity of data
/// @param [in] list List
/// @param [in/out] error Variable for save errors` code
/// @return Capacity of .data
size_t list_capacity(const List *list, int *error = nullptr);

/// Resize list
/// @param [in/out] list List for resize
/// @param [in] newCapacity Wanted capacity for list
/// @param [in] restoreLinearity Does need restore linearity of list
/// @param [in/out] error Variable for save errors` code
/// @note !!!Warning!!! Decrease size of list maight be invalid it
void list_resize(List *list, size_t newCapacity, int restoreLinearity = 0, int *error = nullptr);

/// Restore linearity of List
/// @param [in/out] list List for restore
/// @param [in] newCapacity Size of new alloced memeory where will be copied data
/// @param [in/out] error Variable for save errors` code
/// @note !!!Warning!!! After call this function each index_t will be invalid
void list_restoreLinearity(List *list, size_t newCapacity = 0, int *error = nullptr);

#define dumpList(LIST, ERROR, FILE)                       \
  do_dumpList(LIST, ERROR, FILE, __FILE__, __func__, __LINE__, "")

#define dumpListWithMessage(LIST, ERROR, FILE, MESSAGE, ...)            \
  do_dumpList(LIST, ERROR, FILE, __FILE__, __func__, __LINE__, MESSAGE __VA_OPT__(,) __VA_ARGS__)

/// Make dump of list
/// @param [in] list List for dump
/// @param [in] error Errors` code from validateList()
/// @param [in] file File for dump
/// @param [in] fileName Name of file where was call this function
/// @param [in] functionName Name of function where was call this function
/// @param [in] line Number of line  where was call this function
/// @param [in] message Message(C-like format string) for title
/// @param [in] ... arguments for message
void do_dumpList(
              const List *list,
              unsigned error,
              FILE *file,
              const char *fileName,
              const char *functionName,
              int line,
              const char *message,
              ...
              );

#endif
