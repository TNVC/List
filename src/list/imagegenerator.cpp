#include "list.h"
#include "listdump.h"

#include <ctype.h>
#include <string.h>
#include <time.h>
#include "elementfunctions.h"
#include "asserts.h"

const char *const POISON_COLOR  = "\"#968d78\"";
const char *const ELEMENT_COLOR = "\"#b59d65\"";

static char *generateDotFile(const List *list);

static char *generateNewFileName();

static void openDigraph(FILE *file);

static void setDefaultNodeParameters(FILE *file);

static void generateNode(const List *list, const Node *node, size_t index, FILE *file);

static void generateMainSequence(const List *list, FILE *file);

static void generateSequence(const List *list, FILE *file);

static void closeDigraph(FILE *file);

char *createImage(const List *list)
{
  assert(list);

  char *dotFileName = generateDotFile(list);

  size_t size = strlen(dotFileName);

  char *command = (char *)calloc(2*size + 16, sizeof(char));

  if (!command)
    return nullptr;

  int offset = sprintf(command, "dot %s -T png ", dotFileName);

  sprintf(dotFileName + size - 3, "png");

  sprintf(command + offset, "-o %s", dotFileName);

  system(command);

  free(command);

  return dotFileName;
}

static char *generateDotFile(const List *list)
{
  assert(list);

  char *fileName = generateNewFileName();

  if (!fileName)
    return nullptr;

  FILE *file = fopen(fileName, "w");

  if (!file)
    return nullptr;

  openDigraph(file);

  setDefaultNodeParameters(file);

  for (size_t i = 0; i < list->capacity; ++i)
    generateNode(list, &list->data[i], i, file);

  generateMainSequence(list,file);

  generateSequence(list,file);

  closeDigraph(file);

  fclose(file);

  return fileName;
}

static char *generateNewFileName()
{
  time_t now = 0;
  time(&now);
  char *dataString = ctime(&now);

  int i = 0;

  for ( ; dataString[i]; ++i)
    if (isspace(dataString[i]) || ispunct(dataString[i]))
      dataString[i] = '_';

  dataString[i - 1] = '\0';

  static char buff[64] = "";

  static int imageCount = 0;

  sprintf(buff, ".log/image_%s_%10.10d.dot", dataString, imageCount++);

  return buff;
}

static void openDigraph(FILE *file)
{
  fprintf(file, "digraph {\n");
}

static void setDefaultNodeParameters(FILE *file)
{
  fprintf(file, "\tsplines=ortho;\n");
  fprintf(file, "\trankdir=LR;\n");
  fprintf(file, "\tnode[shape=Mrecord];\n");
  fprintf(file, "\tedge[color=\"#00000000\"];\n");
}

static void generateNode(const List *list, const Node *node, size_t index, FILE *file)
{
  fprintf(
          file,
          "\t\tNODE_%08zu [ style=filled,color=%s,label=\""
          " %s%zu | "
          " Elem: %s |"
          " Next: %4d | Prev: %d"
          "\" ];\n",
          index,
          (getPoison(node->elem) == node->elem) ? POISON_COLOR : ELEMENT_COLOR,
          ((index_t)index == list_head(list)) ? "Head:" : ((index_t)index == list->free) ? "Free:" : "",
          index,
          toString(node->elem),
          node->next,
          node->prev
          );
}

static void generateMainSequence(const List *list, FILE *file)
{
  for (size_t i = 0; i < list->capacity - 1; ++i)
    fprintf(file, "\tNODE_%08zu->NODE_%08zu [ weight=300 ];\n", i, i + 1);
}

static void generateSequence(const List *list, FILE *file)
{

  index_t curr = list_head(list);

  fprintf(file, "\tedge[color=\"RED\"];\n");

  for (int i = 0; i < (int)list->size - 1; ++i, curr = list->data[curr].next)
    fprintf(file, "\tNODE_%08d->NODE_%08d [ weight=10 ];\n", curr, list->data[curr].next);

  fprintf(file, "\tedge[color=\"BLUE\"];\n");

  for (int i = 0; i < (int)list->size - 1; ++i, curr = list->data[curr].prev)
    fprintf(file, "\tNODE_%08d->NODE_%08d [ weight=10 ];\n", curr, list->data[curr].prev);

  curr = list->free;

  fprintf(file, "\tedge[color=\"GREEN\"];\n");

  for (int i = 0; i < (int)list->capacity - (int)list->size - 2; ++i, curr = list->data[curr].next)
    fprintf(file, "\tNODE_%08d->NODE_%08d [ weight=10 ];\n", curr, list->data[curr].next);
}

static void closeDigraph(FILE *file)
{
  fprintf(file, "}");
}
