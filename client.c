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

#include "defines.h"

#define PORTNUM 2300
#define MAXRCVLEN 500

int len, mysocket;
struct sockaddr_in dest;

int rowcount[CLIENT_COUNT];
int colcount[CLIENT_COUNT];

void *recieve_and_print(void *vargp)
{
	byte in[2];
	while(true)
	{
		recv(mysocket, &in, 2, 0);
		if(in[1] == 127)
		{
			move(rowcount[in[0]] + 3 + in[0] * 3,  colcount[in[0]]);
			delch();
			colcount[in[0]]--;
		}
		else if(in[1] == 10)
		{
			if(rowcount[in[0]])
				rowcount[in[0]]--;
			else
				rowcount[in[0]]++;

			colcount[in[0]] = 0;
			move(rowcount[in[0]] + 3 + in[0] * 3, colcount[in[0]]);
			for(int i = 0; i < 100; i++)
				delch();
		}
		else
		{	
			move(rowcount[in[0]] + 3 + in[0] * 3, colcount[in[0]]);
			colcount[in[0]]++;
			printw("%c", in[1]);
		}
		
		refresh();
	}
}

int main(int argc, char** argv)
{
	if(argc < 2)
		return printf("Enter ip as command arg.");
	initscr();

	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	memset(&dest, 0, sizeof(dest));

	for(int i = 0; i < CLIENT_COUNT; i++)
	{
		rowcount[i] = 0;
	}
		
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(argv[1]);
	//dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	dest.sin_port = htons(PORTNUM);

	if(connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in)) < 0)
	{
		printf("ERROR CONNECTING\n");
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
