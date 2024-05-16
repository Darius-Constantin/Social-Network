/*
 * Copyright (c) 2024, Constantin Darius-Andrei <darcon2004@gmail.com>
 */

#include "array_hashtable.h"

#define IS_EMPTY(x) (x == NULL || ((x)->key == NULL && (x)->value == NULL))

array_hashtable_t *arr_ht_init(unsigned int max_buckets,
                               unsigned int (*hash_key)(void *),
                               int (*cmp)(void *, void *))
{
    array_hashtable_t *new_hashtable = malloc(sizeof(*new_hashtable));
    DIE(!new_hashtable, "new array_hashtable allocation failed");

    new_hashtable->hash_key = hash_key;
    new_hashtable->cmp = cmp;
    new_hashtable->number_of_buckets = max_buckets;
    new_hashtable->current_size = 0;
    new_hashtable->buckets = calloc(max_buckets,
                                    sizeof(*new_hashtable->buckets));
    DIE(!new_hashtable->buckets, "array_hashtable buckets allocation failed");
    return new_hashtable;
}

void arr_ht_free(array_hashtable_t **ht, void (*free_data)(void *))
{
    for (uint32_t i = 0; i < (*ht)->number_of_buckets; i++) {
        if (free_data)
            free_data((*ht)->buckets[i]);
        free((*ht)->buckets[i]);
    }

    free((*ht)->buckets);
    free(*ht);
}

uint8_t arr_ht_is_full(array_hashtable_t *ht)
{
    return ht->number_of_buckets == ht->current_size;
}

arr_ht_pair_t *arr_ht_put(array_hashtable_t *ht,
                          void *key, void *value)
{
    arr_ht_pair_t *current_pair = malloc(sizeof(*current_pair));
    current_pair->hash = ht->hash_key(key) % ht->number_of_buckets;
    current_pair->key = key;
    current_pair->value = value;

    unsigned int current_offset = 0, current_index = current_pair->hash;

    while (current_offset <= ht->number_of_buckets) {
        if (IS_EMPTY(ht->buckets[current_index])) {
            ht->buckets[current_index] = current_pair;
            ht->current_size++;
            return ht->buckets[current_index];
        }

        if (ht->cmp(ht->buckets[current_index]->key, key) == 0)
            return ht->buckets[current_index];

        uint32_t tested_probe;
        if (current_index >= ht->buckets[current_index]->hash) {
            tested_probe = current_index - ht->buckets[current_index]->hash;
        } else {
            tested_probe = current_index + ht->number_of_buckets -
                           ht->buckets[current_index]->hash;
        }

        if (current_offset > tested_probe) {
            arr_ht_pair_t *tmp_pair = ht->buckets[current_index];
            ht->buckets[current_index] = current_pair;
            current_pair = tmp_pair;
            current_offset = tested_probe;
        }

        current_offset++;
        current_index++;
        if (current_index >= ht->number_of_buckets)
            current_index = 0;
    }

    return NULL;
}

void *arr_ht_remove_by_key(array_hashtable_t *ht, void *key)
{
    unsigned int hash = ht->hash_key(key) % ht->number_of_buckets;
    unsigned int current_offset = 0, current_index = hash;
    while (current_offset != ht->number_of_buckets) {
        if (IS_EMPTY(ht->buckets[current_index]))
            return NULL;

        uint32_t tested_probe;
        if (current_index >= ht->buckets[current_index]->hash) {
            tested_probe = current_index - ht->buckets[current_index]->hash;
        } else {
            tested_probe = current_index + ht->number_of_buckets -
                           ht->buckets[current_index]->hash;
        }

        if (current_offset > tested_probe)
            return NULL;

        if (ht->cmp(ht->buckets[current_index]->key, key) == 0) {
            void *retval = ht->buckets[current_index]->value;
            free(ht->buckets[current_index]);
            ht->buckets[current_index] = NULL;

            unsigned int prev = current_index;
            unsigned int stop_index = (current_index + 1) %
                                      ht->number_of_buckets;

            while (stop_index != current_index) {
                if (IS_EMPTY(ht->buckets[stop_index]) ||
                    stop_index - ht->buckets[stop_index]->hash == 0)
                    break;

                stop_index++;
                if (stop_index >= ht->number_of_buckets)
                    stop_index = 0;
            }

            for (uint32_t i = (current_index + 1) %
                 ht->number_of_buckets; i != stop_index;) {
                ht->buckets[prev] = ht->buckets[i];
                prev = i;
                i++;
                if (i >= ht->number_of_buckets)
                    i = 0;
            }

            ht->buckets[prev] = NULL;
            ht->current_size--;
            return retval;
        }

        current_offset++;
        current_index++;
        if (current_index >= ht->number_of_buckets)
            current_index = 0;
    }
    return NULL;
}

int64_t arr_ht_has_key(array_hashtable_t *ht, void *key)
{
    unsigned int hash = ht->hash_key(key) % ht->number_of_buckets;
    unsigned int current_offset = 0, current_index = hash;
    while (1) {
        if (IS_EMPTY(ht->buckets[current_index]))
            return -1;

        uint32_t tested_probe;
        if (current_index >= ht->buckets[current_index]->hash) {
            tested_probe = current_index - ht->buckets[current_index]->hash;
        } else {
            tested_probe = current_index + ht->number_of_buckets -
                           ht->buckets[current_index]->hash;
        }

        if (current_offset > tested_probe ||
            current_offset >= ht->number_of_buckets)
            return -1;

        if (ht->cmp(key, ht->buckets[current_index]->key) == 0)
            return (int64_t)current_index;

        current_offset++;
        current_index++;
        if (current_index >= ht->number_of_buckets)
            current_index = 0;
    }
}

arr_ht_pair_t *arr_ht_retrieve_value(array_hashtable_t *ht, void *key)
{
    unsigned int hash = ht->hash_key(key) % ht->number_of_buckets;
    unsigned int current_offset = 0, current_index = hash;
    while (1) {
        if (IS_EMPTY(ht->buckets[current_index]))
            return NULL;

        uint32_t tested_probe;
        if (current_index >= ht->buckets[current_index]->hash) {
            tested_probe = current_index - ht->buckets[current_index]->hash;
        } else {
            tested_probe = current_index + ht->number_of_buckets -
                           ht->buckets[current_index]->hash;
        }

        if (current_offset > tested_probe ||
            current_offset >= ht->number_of_buckets)
            return NULL;

        if (ht->cmp(key, ht->buckets[current_index]->key) == 0)
            return ht->buckets[current_index];

        current_offset++;
        current_index++;
        if (current_index >= ht->number_of_buckets)
            current_index = 0;
    }
}

void arr_ht_upsize(array_hashtable_t *ht)
{
    (void) ht;
}

void arr_ht_downsize(array_hashtable_t *ht)
{
    (void) ht;
}
