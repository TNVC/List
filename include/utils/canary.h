#ifndef CANARY_H_
#define CANARY_H_

#include <stddef.h>

#define LEFT_CANARY 0xABADBABE
#define RIGHT_CANARY 0xDED00DED

typedef unsigned canary_t;

void *canaryCalloc(size_t elementCount, size_t elementSize);

void *canaryRecalloc(void *address, size_t elementCount, size_t elementSize);

void canaryFree(void *address);

int checkLeftCanary(void *address);

int checkRightCanary(void *address);

int checkAddress(void *address);

#endif
