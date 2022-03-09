CFLAGS = -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal
INCLUDE = -I ./include/


.PHONY: all
all: objects exe

exe:
	$(CC) $(CFLAGS) $(INCLUDE) -o sockets ./obj/socket_factory.o ./obj/test_main.o

objects:
	$(MAKE) -C ./src/
	
.PHONY: debug
debug: CFLAGS += -g
debug: clean
debug: all

.PHONY: clean
clean:
	$(RM) ./obj/*.o sockets
