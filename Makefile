cc ?= gcc

.PHONY:default
default: main clean

.PHONY: main clean
main:
	$(cc) -Wall -Wextra -Iinc -g -fsanitize=undefined -O2 -pthread src/main.c src/queue.c src/mem.c src/signal.c src/hotload.c -ldl -o ./bin/event
	./bin/event

clean:
	rm -f bin/event
	rm -rf ./.cache

.PHONY: lib
lib:
	rm -f lib/libhot.so
	$(cc) -Wall -Wextra -g -Iinc -O2 src/lib.c -shared -fPIC -o lib/libhot.so

