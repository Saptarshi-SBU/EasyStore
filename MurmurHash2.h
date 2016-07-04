#ifndef _MURMURHASH2_H
#define _MURMURHASH2_H

#include <stdint.h>

uint64_t MurmurHash64A ( const void * key, int len, uint64_t seed );

#endif
