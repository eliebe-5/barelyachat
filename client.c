#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <curses.h>
#include <pthread.h>

#include "cb.h"

#define PORTNUM 2300
#define MAXRCVLEN 500

int len, mysocket;
struct sockaddr_in dest;

void *recieve_and_print(void *vargp)
{
	byte in;
	while(true)
	{
		recv(mysocket, &in, 1, 0);
		if(in == 127)
		{
			int x;
			int y;
			getsyx(y, x);
			move(y, x -  1);
			delch();
		}
		else
			printw("%c", in);
		refresh();
	}
}

int main(int argc, char** argv)
{
	initscr();

	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	memset(&dest, 0, sizeof(dest));

	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	dest.sin_port = htons(PORTNUM);

	if(connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in)) < 0)
	{
		return 1;
	}

	byte a;
	printw("Connected\n");

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, recieve_and_print, NULL);

	char in;
	noecho();
	printw("Write messages!\n");
	while(true)
	{
		in = getch();
		send(mysocket, &in, 1, 0);
		if(in == 27)
		{
			printw("\nDisconnected.");
			break;
		}
	}

	close(mysocket);
	endwin();
	return EXIT_SUCCESS;
}
