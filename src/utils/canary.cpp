#include "canary.h"

#include <malloc.h>
#include "systemlike.h"

void *canaryMalloc(size_t size)
{
  size += 2*sizeof(canary_t);

  void *adr = malloc(size);

  if (!adr)
    return nullptr;

  *(canary_t *)adr = LEFT_CANARY;

  *(canary_t *)((char *)adr + size - sizeof(canary_t)) = RIGHT_CANARY;

  adr = (char *)adr + sizeof(canary_t);

  return adr;
}

void *canaryCalloc(size_t elementCount, size_t elementSize)
{
  size_t size = elementCount*elementSize + 2*sizeof(canary_t);

  void *adr = calloc(1, size);

  if (!adr)
    return nullptr;

  *(canary_t *)adr = LEFT_CANARY;

  *(canary_t *)((char *)adr + size - sizeof(canary_t)) = RIGHT_CANARY;

  adr = (char *)adr + sizeof(canary_t);

  return adr;
}

void *canaryRecalloc(void *address, size_t elementCount, size_t elementSize)
{
  if (!address)
    return canaryCalloc(elementCount, elementSize);

  size_t newSize = (elementCount && elementSize) ?
    elementCount*elementSize + 2*sizeof(canary_t) : 0;

  void *newAdr = recalloc((char *)address - sizeof(canary_t), 1, newSize);

  if (!newAdr)
    return nullptr;

  *(canary_t *)((char *)newAdr + newSize - sizeof(canary_t)) = RIGHT_CANARY;

  return (char *)newAdr + sizeof(canary_t);
}

void canaryFree(void *address)
{
  free((char *)address - sizeof(canary_t));
}

int checkLeftCanary(void *address)
{
  if (!address)
    return 0;

  address = (char *)address - sizeof(canary_t);

  return *(canary_t *)address == LEFT_CANARY;
}

int checkRightCanary(void *address)
{
  if (!address)
    return 0;

  return *(canary_t *)address == RIGHT_CANARY;
}
