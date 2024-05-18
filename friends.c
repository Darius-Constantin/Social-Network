#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "friends.h"

static int sort_suggestions(const void *node1, const void *node2)
{
	unsigned int id1 = ((user_t *)(*(graph_node_t **)node1)->data)->user_id;
	unsigned int id2 = ((user_t *)(*(graph_node_t **)node2)->data)->user_id;
	if (id1 > id2)
		return 1;
	if (id1 < id2)
		return -1;
	return 0;
}

void handle_input_friends(char *input, social_manager_t *social_manager)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd) {
		free(commands);
		return;
	}

	if (!strcmp(cmd, "add")) {
		char *user1 = strtok(NULL, "\n ");
		user1 = strdup(user1);
		DIE(!user1, "user1 dup failed");
		char *user2 = strtok(NULL, "\n ");
		user2 = strdup(user2);
		DIE(!user2, "user2 dup failed");

		graph_add_node(social_manager->user_graph, user1);
		graph_add_node(social_manager->user_graph, user2);

		graph_add_edge(social_manager->user_graph, user1, user2);
		printf("Added connection %s - %s\n", user1, user2);

		free(user1);
		free(user2);
	} else if (!strcmp(cmd, "remove")) {
		char *user1 = strtok(NULL, "\n ");
		user1 = strdup(user1);
		DIE(!user1, "user1 dup failed");
		char *user2 = strtok(NULL, "\n ");
		user2 = strdup(user2);
		DIE(!user2, "user2 dup failed");

		graph_remove_edge(social_manager->user_graph, user1, user2);
		printf("Removed connection %s - %s\n", user1, user2);

		free(user1);
		free(user2);
	} else if (!strcmp(cmd, "suggestions")) {
		char *user = strtok(NULL, "\n ");

		vector_t suggestions= graph_node_grand_neighbors(social_manager->user_graph, user);
		if (suggestions.vector) {
			qsort(suggestions.vector, suggestions.len, sizeof(graph_node_t *), sort_suggestions);
			printf("Suggestions for %s:\n", user);
			for (unsigned int i = 0; i < suggestions.len; i++)
				printf("%s\n", ((user_t *)((*(graph_node_t **)(suggestions.vector + i * sizeof(void *)))->data))->username);
			free(suggestions.vector);
		}
	} else if (!strcmp(cmd, "distance")) {
		char *user1 = strtok(NULL, "\n ");
		user1 = strdup(user1);
		DIE(!user1, "user1 dup failed");
		char *user2 = strtok(NULL, "\n ");
		user2 = strdup(user2);
		DIE(!user2, "user2 dup failed");

		unsigned int dist = graph_distance_between_nodes(social_manager->user_graph, user1,user2);
		if (dist == INFTY)
			printf("There is no way to get from %s to %s\n", user1, user2);
		else
			printf("The distance between %s - %s is %u\n", user1, user2, dist);

		free(user1);
		free(user2);
	} else if (!strcmp(cmd, "common")) {
		char *user1 = strtok(NULL, "\n ");
		user1 = strdup(user1);
		DIE(!user1, "user1 dup failed");
		char *user2 = strtok(NULL, "\n ");
		user2 = strdup(user2);
		DIE(!user2, "user2 dup failed");

		vector_t common = graph_node_common_neighbors(social_manager->user_graph, user1, user2);
		if (common.vector) {
			qsort(common.vector, common.len, sizeof(graph_node_t *), sort_suggestions);
			printf("The common friends between %s and %s are:\n", user1, user2);
			for (unsigned int i = 0; i < common.len; i++)
				printf("%s\n", ((user_t *)((*(graph_node_t **)(common.vector + i * sizeof(void *)))->data))->username);
			free(common.vector);
		} else {
			printf("No common friends for %s and %s\n", user1, user2);
		}

		free(user1);
		free(user2);
	} else if (!strcmp(cmd, "friends")) {
		char *user = strtok(NULL, "\n ");
		user = strdup(user);
		DIE(!user, "user dup failed");

		graph_node_t *node = graph_get_node(social_manager->user_graph, user);
		printf("%s has %u friends\n", user, node->neighbors->length);

		free(user);
	} else if (!strcmp(cmd, "popular")) {
		char *user = strtok(NULL, "\n ");
		graph_node_t *node = graph_get_node(social_manager->user_graph, user);

		user_t *current_max_friend = node->data;
		unsigned int max_friends = node->neighbors->length;
		ll_node_t *current_friend_node = node->neighbors->head;

		if (current_friend_node) {
			do {
				graph_node_t *curr_friend = ((arr_ht_pair_t *)current_friend_node->data)->value;
				if ((curr_friend->neighbors->length > max_friends) &&
					(cmp_users(node->data, current_max_friend) == 0 ||
					(cmp_users(node->data, current_max_friend) < 0))) {
						current_max_friend = curr_friend->data;
						max_friends = curr_friend->neighbors->length;
					}
				current_friend_node = current_friend_node->next;
			} while (current_friend_node != node->neighbors->head);
		}

		if (!cmp_users(node->data, current_max_friend))
			printf("%s is the most popular\n", current_max_friend->username);
		else
			printf("%s is the most popular friend of %s\n",
				   current_max_friend->username,
				   ((user_t *)node->data)->username);
	}

	free(commands);
}
