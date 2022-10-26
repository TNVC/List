#include "hash.h"
#include "systemlike.h"

const hash_t DEFAULT_HASH_OFFSET = 17;

hash_t getHash(const void *dataStart, const void *dataEnd)
{
  if (!isPointerCorrect(dataStart) || !isPointerCorrect(dataEnd))
    return 0;

  int hash = DEFAULT_HASH_OFFSET;

  for (const char *ptr = (const char *)dataStart; ptr != dataEnd; ++ptr)
      hash += (hash << 5) + hash + *ptr;

  return (hash_t)hash;
}
