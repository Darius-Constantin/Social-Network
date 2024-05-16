#ifndef FRIENDS_H
#define FRIENDS_H

#include "graph.h"

#define MAX_COMMAND_LEN 100

#ifndef MAX_PEOPLE
#define MAX_PEOPLE 550
#endif

/**
 * Function that handles the calling of every command from task 1
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_friends(char *input);

#endif // FRIENDS_H
