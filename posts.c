#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posts.h"

static void *post_get_likes(void *post)
{
	return &((post_t *)post)->likes->length;
}

static int cmp_posts_likes(void *no_of_likes1, void *no_of_likes2)
{
	return *(unsigned int *)no_of_likes1 - *(unsigned int *)no_of_likes2;
}

void handle_input_posts(char *input, social_manager_t *social_manager)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd) {
		free(commands);
		return;
	}

	if (!strcmp(cmd, "create")) {
		char *username = strtok(NULL, "\n ");
		char *title = username + strlen(username) + 1 + 1;
		while (*title == ' ')
			title++;
		title[strlen(title) - 2] = 0;

		if (post_init(social_manager, username, title))
			printf("Created \"%s\" for %s\n", title, username);
	} else if (!strcmp(cmd, "repost")) {
		char *username = strtok(NULL, "\n ");
		char *id = strtok(NULL, "\n ");
		unsigned int post_id = atoi(id);
		unsigned int repost_id = 0;
		id = strtok(NULL, "\n ");
		if (id)
			repost_id = atoi(id);
		printf("Created repost #%u for %s\n",
			   repost_init(social_manager, username, post_id, repost_id)->id,
			   username);
	} else if (!strcmp(cmd, "common-repost")) {
		char *id = strtok(NULL, "\n ");
		unsigned int post_id = atoi(id);
		id = strtok(NULL, "\n ");
		unsigned int repost_id1 = atoi(id);
		id = strtok(NULL, "\n ");
		unsigned int repost_id2 = atoi(id);

		post_t *post1 = post_find_by_id(social_manager, post_id, repost_id1);
		post_t *post2 = post_find_by_id(social_manager, post_id, repost_id2);
		list_t *parents_of_post1 = list_init(cmp_uint, no_copy, get_post_id_from_post, NULL);

		post_t *current_parent = post1;
		while (current_parent) {
			list_append_data(parents_of_post1, current_parent);
			current_parent = current_parent->parent_post;
		}

		current_parent = post2;
		while (current_parent) {
			unsigned int prev_len = parents_of_post1->length;
			list_remove_data(parents_of_post1, current_parent);
			if (parents_of_post1->length < prev_len)
				break;
			current_parent = current_parent->parent_post;
		}

		list_free(&parents_of_post1);
		printf("The first common repost of %u and %u is %u\n",
			   repost_id1, repost_id2, current_parent->id);
	} else if (!strcmp(cmd, "like")) {
		char *username = strtok(NULL, "\n ");
		char *id = strtok(NULL, "\n ");
		unsigned int post_id = atoi(id);
		unsigned int repost_id = 0;
		id = strtok(NULL, "\n ");
		if (id)
			repost_id = atoi(id);
		post_t *post = post_find_by_id(social_manager, post_id, repost_id);
		if (!post)
			return;
		printf("User %s %s %s \"%s\"\n", username,
			   post_like(social_manager, username, post) == 1 ?
			   "liked" : "unliked", repost_id == 0 ? "post" : "repost",
			   post->title);
	} else if (!strcmp(cmd, "ratio")) {
		char *id = strtok(NULL, "\n ");
		unsigned int post_id = atoi(id);
		post_t *greatest_post = post_with_greatest_value(post_find_by_id(social_manager, post_id, 0),
														 post_get_likes, cmp_posts_likes);
		if (greatest_post->id == post_id)
			printf("The original post is the highest rated\n");
		else
			printf("Post %u got ratio'd by repost %u\n", post_id, greatest_post->id);

	} else if (!strcmp(cmd, "delete")) {
		char *id = strtok(NULL, "\n ");
		unsigned int post_id = atoi(id);
		id = strtok(NULL, "\n ");
		unsigned int repost_id = 0;
		if (id)
			repost_id = atoi(id);
		post_t *original_post = post_find_by_id(social_manager, post_id, 0);
		post_t *deleted_post = repost_find_by_id(social_manager, original_post, repost_id);
		if (deleted_post->parent_post)
			list_remove_data(deleted_post->parent_post->events, deleted_post);
		post_delete(social_manager, deleted_post);
		printf("Deleted %s #%u of post \"%s\"\n",
			   repost_id == 0 ? "post" : "repost",
			   repost_id == 0 ? post_id : repost_id,
			   original_post->title);
	} else if (!strcmp(cmd, "get-likes")) {
		char *id = strtok(NULL, "\n ");
		unsigned int post_id = atoi(id);
		id = strtok(NULL, "\n ");
		unsigned int repost_id = 0;
		if (id)
			repost_id = atoi(id);
		post_t *desired_post = post_find_by_id(social_manager, post_id,
											   repost_id);
		if (desired_post) {
			if (!repost_id)
				printf("Post \"%s\" has %u likes\n", desired_post->title,
					   desired_post->likes->length);
			else
				printf("Repost #%u has %u likes\n", desired_post->id,
					   desired_post->likes->length);
		}
	} else if (!strcmp(cmd, "get-reposts")) {
		char *id = strtok(NULL, "\n ");
		unsigned int post_id = atoi(id);
		id = strtok(NULL, "\n ");
		unsigned int repost_id = 0;
		if (id)
			repost_id = atoi(id);

		post_t *original_post = post_find_by_id(social_manager,
												post_id, repost_id);

		if (original_post) {
			if (!repost_id)
				printf("\"%s\" - Post by %s\n", original_post->title,
					get_user_name(original_post->user_id));
			else
				printf("Repost #%u by %s\n", original_post->id,
					   get_user_name(original_post->user_id));

			vector_t reposts = {0, malloc(social_manager->post_table->current_size * sizeof(post_t **))};
			DIE(!reposts.vector, "result vector allocation failed!");
			post_find_all_reposts_recursive(social_manager, original_post, &reposts);
			for (unsigned int i = 1; i < reposts.len; i++) {
				post_t *current_post = *(post_t **)(reposts.vector + i * sizeof(void *));
				printf("Repost #%u by %s\n", current_post->id,
					get_user_name(current_post->user_id));
			}
			free(reposts.vector);
		}
	}

	free(commands);
}

post_t *post_init(social_manager_t *social_manager, char *username, char *title)
{
	graph_node_t *node = graph_get_node(social_manager->user_graph, username);
	if (!node)
		node = graph_add_node(social_manager->user_graph, username)->value;

	post_t *new_post = malloc(sizeof(*new_post));
	DIE(!new_post, "new post allocation failed!");

	new_post->user_id = ((user_t *)node->data)->user_id;
	new_post->id = social_manager->post_id++;
	new_post->events = NULL;
	new_post->parent_post = NULL;
	new_post->title = calloc(strlen(title) + 1, sizeof(char));
	DIE(!new_post->title, "new post title allocation failed!");
	strcpy(new_post->title, title);
	new_post->likes = list_init(cmp_uint, no_copy, user_get_id, NULL);

	arr_ht_put(social_manager->post_table, &new_post->id, new_post);
	list_append_node(((user_t *)node->data)->posts, node_init(new_post));

	return new_post;
}

post_t *repost_find_by_id(social_manager_t *social_manager, post_t *root,
						  unsigned int target)
{
	if (!root)
		return NULL;

	if (root->id == target)
		return root;

	queue_t *queue = queue_init(social_manager->post_table->current_size,
								sizeof(post_t *), NULL);

	queue_enqueue(queue, &root);
	while (!queue_is_empty(queue)) {
		post_t *current_post = *(post_t **)queue_front(queue);
		if (current_post->id == target) {
			queue_free(&queue);
			return current_post;
		}

		if (current_post->events) {
			ll_node_t *to_be_queued = current_post->events->head;
			if (to_be_queued) {
				do {
					queue_enqueue(queue, &to_be_queued->data);
					to_be_queued = to_be_queued->next;
				} while (to_be_queued != current_post->events->head);
			}
		}
		queue_dequeue(queue);
	}
	queue_free(&queue);

	return NULL;
}

post_t *repost_init(social_manager_t *social_manager, char *username,
                    unsigned int post_id, unsigned int repost_id)
{
	post_t *original_post = arr_ht_retrieve_value(social_manager->post_table,
												  &post_id)->value;

	if (!original_post)
		return NULL;

	if (repost_id != 0) {
		original_post = repost_find_by_id(social_manager, original_post, repost_id);
		if (!original_post)
			return NULL;
	}

	if (original_post->events == NULL)
		original_post->events = list_init(cmp_posts, no_copy, get_post_id_from_post, NULL);
	post_t *new_post = post_init(social_manager, username, original_post->title);
	new_post->parent_post = original_post;
	list_append_data(original_post->events, new_post);

	return new_post;
}

post_t *post_find_by_id(social_manager_t *social_manager,
                        unsigned int post_id, unsigned int repost_id)
{
	post_t *original_post = arr_ht_retrieve_value(social_manager->post_table,
												  &post_id)->value;

	if (!original_post)
		return NULL;

	if (repost_id == 0)
		return original_post;

	return repost_find_by_id(social_manager, original_post, repost_id);
}

vector_t post_find_all_reposts(social_manager_t *social_manager,
                               unsigned int post_id, unsigned int repost_id)
{
	post_t *root = post_find_by_id(social_manager, post_id, repost_id);
	if(!root || !root->events)
		return (vector_t){0, NULL};

	vector_t result = {0, malloc(social_manager->post_table->current_size * sizeof(post_t **))};
	DIE(!result.vector, "result vector allocation failed!");

	ll_node_t *current_repost = root->events->head;
	if (current_repost) {
		do {
			post_find_all_reposts_recursive(social_manager, current_repost->data, &result);
			current_repost = current_repost->next;
		} while (current_repost != root->events->head);
	}

	return result;
}

void post_find_all_reposts_recursive(social_manager_t *social_manager,
                                     post_t *original_post,
                                     vector_t *accumulator)
{
	*(post_t **)(accumulator->vector + accumulator->len * sizeof(void *)) = original_post;
	accumulator->len++;
	if (!original_post->events)
		return;

	ll_node_t *current_repost = original_post->events->head;
	if (current_repost) {
		do {
			post_find_all_reposts_recursive(social_manager, current_repost->data, accumulator);
			current_repost = current_repost->next;
		} while (current_repost != original_post->events->head);
	}
}

uint8_t post_like(social_manager_t *social_manager, char *username,
				  post_t *post)
{
	graph_node_t *node = graph_get_node(social_manager->user_graph, username);
	if (!node)
		node = graph_add_node(social_manager->user_graph, username)->value;
	user_t *user = node->data;

	if (arr_ht_has_key(user->liked_posts, &post->id) == -1) {
		arr_ht_put(user->liked_posts, &post->id, post);
		list_append_data(post->likes, user);
		return 1;
	} else {
		arr_ht_remove_by_key(user->liked_posts, &post->id);
		list_remove_data(post->likes, user);
		return 0;
	}
}

void post_delete(social_manager_t *social_manager, post_t *post)
{
	if (!post)
		return;

	if (post->events) {
		ll_node_t *current_repost = post->events->head;
		do {
			post_delete(social_manager, (post_t *)current_repost->data);
			current_repost = current_repost->next;
		} while (current_repost != post->events->head);
	}

	ll_node_t *current_liking_user = post->likes->head;
	if (current_liking_user) {
		do {
			user_t *current_user = current_liking_user->data;
			arr_ht_remove_by_key(current_user->liked_posts, &post->id);
			current_liking_user = current_liking_user->next;
		} while (current_liking_user != post->likes->head);
	}

	user_t *owning_user = ((graph_node_t *)arr_ht_retrieve_value(social_manager->user_graph->neighbor_table, get_user_name(post->user_id))->value)->data;
	list_remove_data(owning_user->posts, &post->id);
	arr_ht_remove_by_key(social_manager->post_table, &post->id);

	post_free((void **)&post);
}

post_t *post_with_greatest_value(post_t *root,
                                 void *(*access_value)(void *),
                                 int (*cmp)(void *, void *))
{
	if (!root)
		return NULL;

	if (!root->events)
		return root;

	post_t *greatest_post = root;
	void *greatest_value = access_value(root);

	ll_node_t *current_repost = root->events->head;
	if (current_repost) {
		do {
			if (cmp(greatest_value, access_value(current_repost->data)) < 0) {
				greatest_post = current_repost->data;
				greatest_value = access_value(current_repost->data);
			}
			current_repost = current_repost->next;
		} while (current_repost != root->events->head);
	}

	return greatest_post;
}

void post_free(void **post)
{
	free((*(post_t **)post)->title);
	list_free(&(*(post_t **)post)->events);
	list_free(&(*(post_t **)post)->likes);
	free(*post);
	*(post_t **)post = NULL;
}

int cmp_posts(void *post1, void *post2)
{
	if (((post_t *)post1)->id > ((post_t *)post2)->id)
		return 1;
	if (((post_t *)post1)->id < ((post_t *)post2)->id)
		return -1;
	return 0;
}

void *get_post_id_from_post(void *post)
{
	return &((post_t *)post)->id;
}