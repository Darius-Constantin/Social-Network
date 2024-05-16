/**
 * The entrypoint of the homework. Every initialization must be done here
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "users.h"
#include "friends.h"
#include "posts.h"
#include "feed.h"
#include "social_manager.h"

void init_tasks(social_manager_t *social_manager)
{
	social_manager->user_graph = graph_init(hash_username, cmp_users,
									   		create_user_from_username,
											no_copy, free);
	social_manager->post_table = arr_ht_init(INIT_MAX_BUCKETS, hash_uint,
											 cmp_posts);
	social_manager->post_id = 1;
}

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	social_manager_t social_manager;

	init_users();

	init_tasks(&social_manager);

	char *input = (char *)malloc(MAX_COMMAND_LEN);
	while (1) {
		input = fgets(input, MAX_COMMAND_LEN, stdin);

		// If fgets returns null, we reached EOF
		if (!input)
			break;

		#ifdef TASK_1
		handle_input_friends(input);
		#endif

		#ifdef TASK_2
		handle_input_posts(input);
		#endif

		#ifdef TASK_3
		handle_input_feed(input);
		#endif
	}

	return 0;
}
