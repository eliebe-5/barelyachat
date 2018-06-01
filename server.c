#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include "cb.h"

#include "defines.h"

#define PORTNUM 2300

typedef struct
{
	int consocket;
	char* name;
	circular_buffer* cb;
} member;

member members[CLIENT_COUNT];
bool free_slots[CLIENT_COUNT];

struct sockaddr_in dest;
struct sockaddr_in serv;
int mysocket;
socklen_t socksize = sizeof(struct sockaddr_in);

void *listen_for_new(void *vargp)
{
	printf("Waiting for connection\n");
	int id = 0;

	while(!free_slots[id])
		id++;

	int con = accept(mysocket, (struct sockaddr *)&dest, &socksize);
	free_slots[id] = false;
	printf("Recieved connection, id: %d\n", id);
	members[id].consocket = con;
	members[id].cb = (circular_buffer*)malloc(sizeof(circular_buffer));
	init_circular_buffer(512, members[id].cb);
	
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, listen_for_new, NULL);

	int len;
	byte out;
	while(true)
	{
		if((len = recv(con, &out, 1, 0)) > 0)
		{
			if(out == 27)
			{
				close(members[id].consocket);
				destroy_circular_buffer(members[id].cb);
				free_slots[id] = true;
				break;
			}
			else
				write_buffer(members[id].cb, &out, 1);
		}
	}
}

int main(int argc, char** argv)
{
	for(int i = 0; i < CLIENT_COUNT; i++)
	{
		free_slots[i] = true;
	}

	memset(&serv, 0, sizeof(serv));

	serv.sin_family = AF_INET;

	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	serv.sin_port = htons(PORTNUM);

	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

	listen(mysocket, CLIENT_COUNT);

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, listen_for_new, NULL);

	while(true)
	{
		for(int i = 0; i < CLIENT_COUNT; i++)
		{
			if(!free_slots[i])
			{
				byte out[2];
				out[0] = i;
				if(read_buffer(members[i].cb, &(out[1])) == OK_SIGNAL)
				{
					for(int j = 0; j < CLIENT_COUNT; j++)
					{
						if(!free_slots[j])
							send(members[j].consocket, &out[0], 2, 0);
					}
				}
			}
		}
	}

	close(mysocket);
	return EXIT_SUCCESS;
}
