SRC := $(wildcard ./*.c)
APP = $(patsubst %.c, %, $(SRC))
SHELL_RS := $(shell pwd)/shell-rs

all: cmd1 cmd2 cmd3 cmd4 shell

shell_rs: all
	make -C $(SHELL_RS) run

cmd1:
	gcc cmd1.c -o cmd1

cmd2:
	gcc cmd2.c -o cmd2
	
cmd3:
	gcc cmd3.c -o cmd3

cmd4:
	gcc cmd4.c -o cmd4

shell:
	gcc shell.c -o shell

clean:
	rm $(APP)
	make -C $(SHELL_RS) clean