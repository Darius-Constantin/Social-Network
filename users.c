#include "users.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *db_path = "users.db";

static char **users;
static uint16_t users_number;

user_t *user_init(char *username)
{
	user_t *new_user = malloc(sizeof(*new_user));
	DIE(!new_user, "new user allocation failed!");

	new_user->username = calloc(strlen(username) + 1, sizeof(char));
	new_user->user_id = get_user_id(username);
	DIE(!new_user->username, "new user username allocation failed!");
	strcpy(new_user->username, username);
	new_user->posts = list_init(cmp_posts, no_copy, get_post_id_from_post, NULL);
	new_user->liked_posts = arr_ht_init(INIT_MAX_BUCKETS, hash_uint, cmp_posts, NULL);

	return new_user;
}

void *create_user_from_username(void *username)
{
	return user_init(username);
}

void init_users(void)
{
	FILE *users_db = fopen(db_path, "r");

	if (!users_db) {
		perror("Error reading users.db");
		return;
	}

	fscanf(users_db, "%hu", &users_number);

	users = malloc(users_number * sizeof(char *));

	char temp[32];
	for (uint16_t i = 0; i < users_number; i++) {
		fscanf(users_db, "%s", temp);
		int size = strlen(temp);

		users[i] = malloc(size + 1);
		strcpy(users[i], temp);
	}

	fclose(users_db);
}

uint16_t get_user_id(char *name)
{
	if (!users)
		return -1;

	for (uint16_t i = 0; i < users_number; i++)
		if (!strcmp(users[i], name))
			return i;

	return -1;
}

char *get_user_name(uint16_t id)
{
	if (id >= users_number)
		return NULL;

	return users[id];
}

unsigned int hash_username(void *username)
{
    unsigned int user_id = get_user_id((char *)username);
	return hash_uint(&user_id);
}

void *cpy_username(void *user)
{
	char *username = ((user_t *)user)->username;
	char *username_copy = calloc(strlen(username) + 1,
								 sizeof(char));
	DIE(!username_copy, "Username copy failed!");
	strcpy(username_copy, username);
	return username_copy;
}

int cmp_usernames(void *username1, void *username2)
{
	return strcmp((char *)username1, (char *)username2);
}

int cmp_users(void *user1, void *user2)
{
	int user1_id = ((user_t *)user1)->user_id;
	int user2_id = ((user_t *)user2)->user_id;

	if (user1_id > user2_id)
		return 1;
	if (user1_id < user2_id)
		return -1;
	return 0;
}

void *user_get_username(void *user)
{
    return ((user_t *)user)->username;
}

void *user_get_id(void *user)
{
	return &((user_t *)user)->user_id;
}

void free_users()
{
	for (unsigned int i = 0; i < users_number; i++) {
		free(users[i]);
	}
	free(users);
}