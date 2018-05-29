
all:
	gcc server.c -o server -lpthread
	gcc client.c -o client -lpthread -lcurses
server:
	gcc server.c -o server -lpthread
client:
	gcc client.c -o client -lpthread -lcurses
