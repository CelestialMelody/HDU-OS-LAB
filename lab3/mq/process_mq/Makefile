all: sender1 sender2 receiver unlink
	./unlink
sender1: sender1.c
	gcc sender1.c -pthread -o sender1
sender2: sender2.c
	gcc sender2.c -pthread -o sender2
receiver: receiver.c
	gcc receiver.c -pthread -o receiver
unlink:
	gcc unlink.c -o unlink
clean:
	rm sender1 sender2 receiver unlink