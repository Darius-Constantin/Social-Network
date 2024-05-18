#include "social_manager.h"

static void *get_username_from_arr_ht_pair(void *pair)
{
    return user_get_username(((graph_node_t *)((arr_ht_pair_t *)pair)->value)->data);
}

static void social_manager_free_posts(void *key, void *value)
{
    (void)key;
    post_t *post = value;
    post_free((void **)&post);
}

static void social_manager_free_users(void *key, void *value)
{
    (void)key;
    graph_node_t *node = value;
    user_t *user = node->data;
    free(user->username);
    list_free(&user->posts);
    arr_ht_free(&user->liked_posts);
    free(user);
    list_free(&node->neighbors);
    free(node);
}

social_manager_t social_manager_init()
{
    social_manager_t res;
    res.user_graph = graph_init(hash_username,
                                cmp_usernames,
                                create_user_from_username,
                                no_copy,
                                user_get_username,
                                get_username_from_arr_ht_pair,
                                social_manager_free_users);
	res.post_table = arr_ht_init(INIT_MAX_BUCKETS,
                                 hash_uint,
                                 cmp_posts,
                                 social_manager_free_posts);
	res.post_id = 1;

    return res;
}

void social_manager_free_contents(social_manager_t social_manager)
{
    graph_free(&social_manager.user_graph);
    arr_ht_free(&social_manager.post_table);
}