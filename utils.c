#include "utils.h"

unsigned int hash_uint(void *key)
{
    unsigned int uint_key = *((unsigned int *)key);

    uint_key = ((uint_key >> 16u) ^ uint_key) * 0x45d9f3b;
    uint_key = ((uint_key >> 16u) ^ uint_key) * 0x45d9f3b;
    uint_key = (uint_key >> 16u) ^ uint_key;

    return uint_key;
}

unsigned int hash_string(void *key)
{
    unsigned char *key_string = (unsigned char *) key;
    unsigned int hash = 5381;
    int c;

    while ((c = *key_string++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

void *no_copy(void *data)
{
    return data;
}
