all: user1 user2

user1: user1.c
	gcc user1.c -o user1 -pthread -g

user2: user2.c
	gcc user2.c -o user2 -pthread -g

clean:
	rm -f user1 user2