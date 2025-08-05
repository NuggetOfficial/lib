cc ?= gcc

.PHONY:default
default: main clean

.PHONY: main clean
main:
	$(cc) -Wall -Wextra -Iinc -g -fsanitize=undefined -O2 -pthread src/main.c src/queue.c src/mem.c -o ./bin/event
	./bin/event

clean:
	rm -f bin/event
	rm -rf ./.cache


