#ifndef HASH_H_
#define HASH_H_

#include <stddef.h>

typedef unsigned hash_t;

const hash_t nullhash = 0;

hash_t getHash(const void *dataStart, const void *dataEnd);

#endif
