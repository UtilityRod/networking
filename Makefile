CFLAGS += -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal
INCLUDE = ./include/


.PHONY: all
all: ftp udp

ftp: ftpd ftps ftpc

ftpd: ftpd.o socket_factory.o
	$(CC) $(CFLAGS) -o ftpd ./obj/ftpd.o ./obj/socket_factory.o -lrt
	
ftpd.o: ./src/ftp/ftpd.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/ftpd.o -c ./src/ftp/ftpd.c
	
ftps: ftps.o
	$(CC) $(CFLAGS) -o ftps ./obj/ftps.o
	
ftps.o: ./src/ftp/ftps.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/ftps.o -c ./src/ftp/ftps.c
	
ftpc: ftpc.o
	$(CC) $(CFLAGS) -o ftpc ./obj/ftpc.o ./obj/socket_factory.o
	
ftpc.o: ./src/ftp/ftpc.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/ftpc.o -c ./src/ftp/ftpc.c
	
udp: socket_factory.o udps udpc 
	
udps: udps.o
	$(CC) $(CFLAGS) -o udps ./obj/udps.o ./obj/socket_factory.o -lrt
	
udps.o: ./src/udp/udps.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/udps.o -c ./src/udp/udps.c
	
udpc: udpc.o
	$(CC) $(CFLAGS) -o udpc ./obj/udpc.o ./obj/socket_factory.o -lrt
	
udpc.o: ./src/udp/udpc.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/udpc.o -c ./src/udp/udpc.c
	
tcp: socket_factory.o tcps tcpc

tcps: tcps.o
	$(CC) $(CFLAGS) -o tcps ./obj/tcps.o ./obj/socket_factory.o
	
tcps.o: ./src/tcp/tcps.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/tcps.o -c ./src/tcp/tcps.c
	
tcpc: tcpc.o
	$(CC) $(CFLAGS) -o tcpc ./obj/tcpc.o ./obj/socket_factory.o
	
tcpc.o: ./src/tcp/tcpc.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/tcpc.o -c ./src/tcp/tcpc.c
	
socket_factory.o: ./src/socket_factory/socket_factory.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/socket_factory.o -c ./src/socket_factory/socket_factory.c
	
http: httpd https httpc

httpd: httpd.o socket_factory.o
	$(CC) $(CFLAGS) -o httpd ./obj/httpd.o ./obj/socket_factory.o
	
httpd.o: ./src/http/httpd.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/httpd.o -c ./src/http/httpd.c
	
https: https.o socket_factory.o
	$(CC) $(CFLAGS) -o https ./obj/https.o ./obj/socket_factory.o
	
https.o: ./src/http/https.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/https.o -c ./src/http/https.c
	
httpc: httpc.o socket_factory.o
	$(CC) $(CFLAGS) -o httpc ./obj/httpc.o ./obj/socket_factory.o
	
httpc.o: ./src/http/httpc.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ./obj/httpc.o -c ./src/http/httpc.c
	
.PHONY: debug
debug: CFLAGS += -g
debug: clean
debug: all

.PHONY: clean
clean:
	$(RM) ./obj/*.o ftpd ftps ftpc udps udpc tcpc tcps httpd https httpc
