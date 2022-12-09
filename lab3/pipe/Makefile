SRC := $(wildcard ./*.c)
APP = $(patsubst %.c, %, $(SRC))

all: pipe fifo_read fifo_write size_of_pipe fifo

pipe :
	gcc pipe.c -o pipe -pthread

fifo :
	gcc fifo.c -o fifo -pthread

fifo_read :
	gcc fifo_read.c -o fifo_read -pthread

fifo_write :
	gcc fifo_write.c -o fifo_write -pthread

size_of_pipe :
	gcc size_of_pipe.c -o size_of_pipe -pthread

clean:
# 如果文件存在，就删除
	for i in $(APP); do \
		if [ -f $$i ]; then \
			rm $$i; \
		fi; \
	done