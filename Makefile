CFLAGS = -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal
INCLUDE = -I../include/ -I../sockets/
export CFLAGS
export INCLUDE


.PHONY: all
all: sockets src exe

exe:
	$(CC) $(CFLAGS) -o exe ./obj/socket_factory.o ./obj/test_main.o

.PHONY: src
src:
	$(MAKE) -C ./src/

.PHONY: sockets
sockets:
	$(MAKE) -C ./sockets
	
.PHONY: debug
debug: CFLAGS += -g
debug: clean
debug: all

.PHONY: clean
clean:
	$(RM) ./obj/*.o exe
