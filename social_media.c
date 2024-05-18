/**
 * The entrypoint of the homework. Every initialization must be done here
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "friends.h"
#include "posts.h"
#include "feed.h"

void init_tasks(social_manager_t *social_manager)
{
	*social_manager = social_manager_init();
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
		if (!input || !strncmp(input, "EXIT", strlen("EXIT")))
			break;

		#ifdef TASK_1
		handle_input_friends(input, &social_manager);
		#endif

		#ifdef TASK_2
		handle_input_posts(input, &social_manager);
		#endif

		#ifdef TASK_3
		// handle_input_feed(input, &social_manager);
		#endif
	}

	social_manager_free_contents(social_manager);
	free_users();
	free(input);

	return 0;
}
