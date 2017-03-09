all: client server

client: client.o
	gcc -g client.o -o client -lcrypt

client.o: client.c client.h
	gcc -g -c client.c

server: server.o
	gcc -g server.o -o server -lcrypt

server.o: server.c server.h
	gcc -g -c server.c

clean:
	rm -f *.o client server setup.txt authentication.txt teardown.txt
