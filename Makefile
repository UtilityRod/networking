CFLAGS = -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal
INCLUDE = -I ./include/


.PHONY: all
all: objects exe fork_test

exe:
	$(CC) $(CFLAGS) $(INCLUDE) -o sockets ./obj/socket_factory.o ./obj/test_main.o

objects:
	$(MAKE) -C ./src/

fork_test:
	$(CC) $(CFLAGS) $(INCLUDE) -o fork_test ./obj/fork_main.o
	
.PHONY: debug
debug: CFLAGS += -g
debug: clean
debug: all

.PHONY: clean
clean:
	$(RM) ./obj/*.o sockets fork_test
