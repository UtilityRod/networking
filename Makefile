CFLAGS += -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal

.PHONY: all
all: ftpd ftps ftpc

ftpd: ftpd.o
	$(CC) $(CFLAGS) -o ftpd ./obj/ftpd.o -lrt
	
ftpd.o: ./src/ftpd.c
	$(CC) $(CFLAGS) -o ./obj/ftpd.o -c ./src/ftpd.c
	
ftps: ftps.o
	$(CC) $(CFLAGS) -o ftps ./obj/ftps.o
	
ftps.o: ./src/ftps.c
	$(CC) $(CFLAGS) -o ./obj/ftps.o -c ./src/ftps.c
	
ftpc: ftpc.o
	$(CC) $(CFLAGS) -o ftpc ./obj/ftpc.o
	
ftpc.o: ./src/ftpc.c
	$(CC) $(CFLAGS) -o ./obj/ftpc.o -c ./src/ftpc.c
	
.PHONY: debug
debug: CFLAGS += -g
debug: clean
debug: all

.PHONY: clean
clean:
	$(RM) ./obj/*.o ftpd ftps ftpc
