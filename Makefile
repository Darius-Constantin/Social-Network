CC=gcc
CFLAGS=-Wall -Wextra -g #-Werror # <- this last flag sucks!

.PHONY: build clean

all: clean build

build: friends posts feed

# Any other object file that is needed for the other files
# add it here, separated by space
UTILS = utils.o queue.o array_hashtable.o node.o list.o graph.o social_manager.o users.o posts.o

friends: $(UTILS) friends.o social_media_friends.o
	$(CC) $(CFLAGS) -o $@ $^

posts: $(UTILS) posts.o social_media_posts.o
	$(CC) $(CFLAGS) -o $@ $^

feed: $(UTILS) posts.o friends.o feed.o social_media_feed.o
	$(CC) $(CFLAGS) -o $@ $^

social_media_friends.o:
	$(CC) $(CFLAGS) -c -D TASK_1 -o $@ social_media.c

social_media_posts.o:
	$(CC) $(CFLAGS) -c -D TASK_2 -o $@ social_media.c

social_media_feed.o:
	$(CC) $(CFLAGS) -c -D TASK_1 -D TASK_2 -D TASK_3 -o $@ social_media.c

node.o:
	$(CC) $(CFLAGS) -c -o $@ node.c

list.o:
	$(CC) $(CFLAGS) -c -o $@ list.c

graph.o:
	$(CC) $(CFLAGS) -c -o $@ graph.c

utils.o:
	$(CC) $(CFLAGS) -c -o $@ utils.c

array_hashtable.o:
	$(CC) $(CFLAGS) -c -o $@ array_hashtable.c

queue.o:
	$(CC) $(CFLAGS) -c -o $@ queue.c

users.o:
	$(CC) $(CFLAGS) -c -o $@ users.c

posts.o:
	$(CC) $(CFLAGS) -c -o $@ posts.c

social_manager.o:
	$(CC) $(CFLAGS) -c -o $@ social_manager.c

clean:
	rm -rf *.o friends posts feed