#include "list.h"
#include "listdump.h"

#include <stdio.h>

#include "systemlike.h"

#pragma GCC diagnostic ignored "-Wsign-compare"

#define SEPARATOR "=========================================================="

const char *const ERROR_MESSAGE[] =
  {
    "Pointer to list is nullptr",
    "Pointer to list is incorrect",
    "Pointer to data is nullptr or incorrect when capacity isn`t 0",
    "Capacity less than size",
    "Head isn`t correct",
    "Tail isn`t correct",
    "Left canary died",
    "Right canary died",
    "Left data canary died",
    "Right data canary died",
    "Hash of list isn`t correct",
    "Hash of data isn`t correct",
    "Invalid free index",
    "Free sequence is incorrect",
    "Main sequence is incorrect"
  };

#ifdef DEBUG_BUILD_

static void printDebugInfo(const List *list, FILE *file);

#endif

static void printCallInfo(
                          const char *fileName,
                          const char *functionName,
                          int line,
                          FILE *file
                          );

#ifdef NEED_HASH_

static void printHash(const List *list, FILE *file);

#endif

static void printError(unsigned error, FILE *file);

static void printFields(const List *list, FILE *file);

static void printData(const List *list, FILE *file);

void do_dumpList(
              const List *list,
              unsigned error,
              FILE *file,
              const char *fileName,
              const char *functionName,
              int line,
              const char *message,
              ...
              )
{
  if (!file)
    file = stdout;

  fprintf(file, "<h2>");

  va_list args = {};

  va_start(args, message);

  vfprintf(file, message, args);

  va_end(args);

  fprintf(file, "</h2>\n");

#ifdef DEBUG_BUILD_

  printDebugInfo(list, file);

#endif

  fprintf(file, "List[%p] ", (const void *)list);

  if (isPointerCorrect(list))
    {
      printCallInfo(fileName, functionName, line, file);

#ifdef NEED_HASH_

      printHash(list, file);

#endif
    }

  printError(error, file);

  if (isPointerCorrect(list))
    {
      printFields(list, file);

      printData(list, file);
    }

  fputc('\n', file);
}

#ifdef DEBUG_BUILD_

static void printDebugInfo(const List *list, FILE *file)
{
  fprintf(
          file,
          "\"%s\" at %s at %s(%d)\n",
          isPointerCorrect(list->info.name)         ? list->info.name         : "null",
          isPointerCorrect(list->info.functionName) ? list->info.functionName : "null",
          isPointerCorrect(list->info.fileName)     ? list->info.fileName     : "null",
          list->info.line
          );
}

#endif

static void printCallInfo(
                          const char *fileName,
                          const char *functionName,
                          int line,
                          FILE *file
                          )
{
  fprintf(
          file,
          "%s at %s(%d):\n",
          isPointerCorrect(functionName) ? functionName : "null",
          isPointerCorrect(fileName)     ? fileName     : "null",
          line
          );
}

#ifdef NEED_HASH_

static void printHash(const List *list, FILE *file)
{
  fprintf(file, "Hash: %X Data hash: %X\n", list->hash, list->dataHash);
}

#endif

static void printError(unsigned error, FILE *file)
{
  if (!error)
    {
      fprintf(file, "No error\n");

      return;
    }

  fprintf(file, "%s\n", SEPARATOR);

  fprintf(file, "<font color = red />Erors:\n");

  for (int i = 0; i < ERROR_COUNT; ++i)
    if ((error >> i) & 0x01)
      fprintf(file, "%s,\n", ERROR_MESSAGE[i]);

  fprintf(file, "<font color = black />%s\n", SEPARATOR);
}

static void printFields(const List *list, FILE *file)
{
  fprintf(file, "%s\n", SEPARATOR);

  fprintf(file, "canary_t leftCanary = %X;\n", list->leftCanary);

  fprintf(file, "Node *data = %p;\n", (void *)list->data);

  fprintf(file, "size_t capacity = %zu;\n", list->capacity);

  fprintf(file, "size_t size = %zu;\n", list->size);

  fprintf(file, "size_t head = %d;\n", list_head(list));

  fprintf(file, "size_t tail = %d;\n", list_tail(list));

  fprintf(file, "size_t free = %d;\n", list->free);

  fprintf(file, "canary_t rightCanary = %X;\n", list->rightCanary);

  fprintf(file, "%s\n", SEPARATOR);
}

static void printData(const List *list, FILE *file)
{
  const char *image = createImage(list);

  fprintf(file, "<hr>\n");

  fprintf(file, "<!%s>", SEPARATOR);

  fprintf(file, "<image src=../%s />", image);

  fprintf(file, "<hr>\n");

  fprintf(file, "<!%s>", SEPARATOR);
}
