#include <stdlib.h>
#include <stdbool.h>

#define ERROR_SIGNAL 1
#define OK_SIGNAL 0
typedef unsigned uint;
typedef unsigned char byte;

typedef struct
{
	byte* buffer;
	uint read;
	uint write;
	size_t size;
} circular_buffer;

int init_circular_buffer(size_t size, circular_buffer* cb)
{
	cb->read = 0;
	cb->write = 0;
	cb->buffer = (byte*) malloc(size);
	cb->size = size;
	if(cb->buffer == NULL)
		return ERROR_SIGNAL;
	return OK_SIGNAL;
}

int destroy_circular_buffer(circular_buffer* cb)
{
	free(cb->buffer);
	return 0;
}

int read_buffer(circular_buffer* cb, byte* out)
{
	if(cb->read == cb->write) 
		return ERROR_SIGNAL;

	*out = cb->buffer[cb->read];
	cb->read++;
	if(cb->read == cb->size)
		cb->read = 0;

	return OK_SIGNAL;	
}

int write_buffer(circular_buffer* cb, char* word, size_t size)
{
	for(size_t i = 0; i < size; i++)
	{
		cb->buffer[cb->write] = word[i];
		cb->write++;
		if(cb->write == cb->size)
			cb->write = 0;
	}

	return OK_SIGNAL;
}

bool is_buffer_empty(circular_buffer* cb)
{
	return cb->read==cb->write;
}
