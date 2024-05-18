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

int cmp_uint(void *val1, void *val2)
{
    if (*(unsigned int *)val1 > *(unsigned int *)val2)
        return 1;
    if (*(unsigned int *)val1 < *(unsigned int *)val2)
        return -1;
    return 0;
}

void *no_copy(void *data)
{
    return data;
}
