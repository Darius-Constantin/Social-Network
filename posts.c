#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posts.h"

void handle_input_posts(char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "create"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "repost"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "common-repost"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "like"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "ratio"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "delete"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-reposts"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
		// TODO: Add function
}

int cmp_posts(void *post1, void *post2)
{
	if (((post_t *)post1)->id > ((post_t *)post2)->id)
		return 1;
	if (((post_t *)post1)->id < ((post_t *)post2)->id)
		return -1;
	return 0;
}

void post_create(social_manager_t *social_manager, char *username, char *title)
{
	graph_node_t *node = graph_get_node(social_manager->user_graph, username);
	if (!node)
		return;

	post_t *new_post = malloc(sizeof(*new_post));
	DIE(!new_post, "new post allocation failed!");

	new_post->user_id = ((user_t *)node->data)->user_id;
	new_post->id = social_manager->post_id++;
	new_post->events = NULL;
	new_post->title = calloc(strlen(title) + 1, sizeof(char));
	DIE(!new_post->title, "new post title allocation failed!");
	strcpy(new_post->title, title);

	arr_ht_put(social_manager->post_table, &new_post->id, new_post);
	list_append_node(((user_t *)node->data)->posts, node_init(new_post));
}