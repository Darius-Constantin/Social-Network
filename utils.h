#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_COMMAND_LEN 200
#define INIT_MAX_BUCKETS 550
#define MAX_PEOPLE 550
#define INFTY (MAX_PEOPLE + 1)

#define DIE(assertion, call_description)							\
	do {															\
		if (assertion) {											\
			fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);		\
			perror(call_description);								\
			exit(errno);											\
		}															\
	} while (0)

typedef struct vector_t {
	unsigned int len;
	void *vector;
} vector_t;

unsigned int hash_uint(void *key);

unsigned int hash_string(void *key);

int cmp_uint(void *val1, void *val2);

void *no_copy(void *data);

#endif /* UTILS_H */