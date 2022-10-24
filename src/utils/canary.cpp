#include "canary.h"

#include <malloc.h>
#include "systemlike.h"

#include <stdio.h>

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

  if (!checkAddress(address))
    return nullptr;

  size_t newSize = (elementCount && elementSize) ?
    elementCount*elementSize + 2*sizeof(canary_t) : 0;

  void *newAdr = recalloc(address, 1, newSize);

  if (!newAdr)
    return nullptr;

  *(canary_t *)((char *)newAdr + newSize - sizeof(canary_t)) = RIGHT_CANARY;

  return newAdr;
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

int checkAddress(void *address)
{
  if (!address)
    return 0;

  address = (char *)address - sizeof(canary_t);

  size_t size = malloc_usable_size(address);

  if (size <= 2*sizeof(canary_t))
    return 0;

  return *(canary_t *)         address                            == LEFT_CANARY  &&
         *(canary_t *)((char *)address + size - sizeof(canary_t)) == RIGHT_CANARY;
}
