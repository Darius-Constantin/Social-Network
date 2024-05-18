/*
 * Copyright (c) 2024, Constantin Darius-Andrei <darcon2004@gmail.com>
 */

#ifndef ARRAY_HASHTABLE_H
#define ARRAY_HASHTABLE_H

#include "utils.h"

typedef struct array_hashtable_pair_t {
    unsigned int hash;
    void *key;
    void *value;
} arr_ht_pair_t;

typedef struct array_hashtable_t {
    unsigned int (*hash_key)(void *);
    int (*cmp)(void *, void *);
    void (*free_data)(void *, void *);
    uint32_t number_of_buckets;
    uint32_t current_size;
    arr_ht_pair_t **buckets;
    uint32_t key_size;
    uint32_t value_size;
} array_hashtable_t;

/// @brief Initalizes a new array-based (open addressing) hashtable.
/// @param max_buckets The number of buckets/slots of the hashtable. The
/// size of the hashtable will never be bigger than this.
/// @param hash_key A pointer to a function responsible for hashing keys.
/// @param cmp A pointer to a function responsible for comparing keys.
/// @return A pointer to the newly created hashtable.
array_hashtable_t *arr_ht_init(unsigned int max_buckets,
                               unsigned int (*hash_key)(void *),
                               int (*cmp)(void *, void *),
                               void (*free_data)(void *, void *));

/// @brief Completely frees a hashtable.
/// @param ht A double pointer to the hashtable which will be freed. In the end,
/// the value of variable will be set to NULL.
/// @param free_data A pointer to a function reponsible for freeing the data
/// stored inside an array_hashtable_pair_t (which is held by a bucket).
void arr_ht_free(array_hashtable_t **ht);

/// @brief A function for putting/inserting a new value inside an array-based
/// hashtable. Follows the Robin Hood hashing principle, a type of linear
/// probing where, at insertion, elements are swapped such that, when a
/// collision is encountered, the element with the bigger distance from the
/// original bucket will remain at the index of the collision and the one with
/// the smaller distance will attempt to find an empty bucket, eventually
/// swapping again if the above condition is met. The function does not
/// overwrite a value if the key already exists.
/// @param ht The hashtable in which the value will be put.
/// @param key The key associated with the value. Does not copy memory! The
/// sameself pointer will be stored.
/// @param value The value which will be inserted in the hashtable.
/// Does not copy memory! The sameself pointer will be stored.
/// @return A pointer to the array_hashtable_pair_t placed in the hashtable.
/// NULL if no slot was available.
arr_ht_pair_t *arr_ht_put(array_hashtable_t *ht, void *key,
                                   void *value);

/// @brief A function for removing a value/element from an array-based hashtable
/// given its associated key. After removal, the remaining elements that
/// collided with it will shift such that their distance to their original
/// buckets will be decremented. Does not free memory!
/// @param ht The hashtable from which the element will be removed.
/// @param key The key associated with the value.
/// @return A pointer to the value stored in the array_hashtable_pair_t placed
/// at the found index.
void *arr_ht_remove_by_key(array_hashtable_t *ht, void *key);

/// @brief A function for checking if an array-based hashtable is full.
/// @param ht The checked hashtable.
/// @return 1 if it is full, 0 otherwise.
uint8_t arr_ht_is_full(array_hashtable_t *ht);


/// @brief A function for checking if an array-based hashtable stores a value
/// for a given key.
/// @param ht The searched hashtable.
/// @param key The searched key.
/// @return If the key is found, its index is returned. Otherwise, -1.
int64_t arr_ht_has_key(array_hashtable_t *ht, void *key);

/// @brief A function for getting the array_hashtable_pair_t stored at a given
/// key.
/// @param ht The searched hashtable.
/// @param key The searched key.
/// @return A pointer to the array_hashtable_pair_t at the found index. NULL
/// if the hashtable does not contain the key.
arr_ht_pair_t *arr_ht_retrieve_value(array_hashtable_t *ht, void *key);

void arr_ht_upsize(array_hashtable_t *ht);

void arr_ht_downsize(array_hashtable_t *ht);

#endif /* ARRAY_HASHTABLE_H */
