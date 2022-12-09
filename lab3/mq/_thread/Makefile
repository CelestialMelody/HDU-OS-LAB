all: mq unlink

mq: mq.c
	$(CC) mq.c -o mq -pthread -g

unlink: unlink.c
	gcc unlink.c -pthread -o unlink

clean:
	rm mq unlink