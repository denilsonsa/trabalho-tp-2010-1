#include <stdlib.h>
// Just for the 'NULL'

#include <sys/select.h>

#include "nbiocore.h"

static int nbio_loop_alive = 1;
static int nbio_timeout_seconds = 1;
static nbio_callback_t nbio_idle_callback = NULL;


// Internal structure to store the registered callbacks
typedef struct nbio_list_item
{
	int fd;
	nbio_callback_t callback;
	void* user_data;
} nbio_list_item_t;

#define MAX_LIST_LEN 256
static nbio_list_item_t nbio_list[MAX_LIST_LEN];
static int nbio_list_len = 0;


int find_max_fd()
{
	// Undefined result if the list is empty
	int i;
	int max_fd;

	max_fd = nbio_list[0].fd;
	for(i=1; i < nbio_list_len; i++)
	{
		if( nbio_list[i].fd > max_fd )
			max_fd = nbio_list[i].fd;
	}
	return max_fd;
}

int find_index_by_fd(int fd)
{
	int i;
	for(i=0; i < nbio_list_len; i++)
	{
		if( nbio_list[i].fd == fd )
			return i;
	}
	return -1;
}


void nbio_register(int fd, nbio_callback_t func, void* user_data)
{
	nbio_list[nbio_list_len].fd = fd;
	nbio_list[nbio_list_len].callback = func;
	nbio_list[nbio_list_len].user_data = user_data;
	nbio_list_len++;
}

void nbio_unregister(int fd)
{
	int old;

	old = find_index_by_fd(fd);
	if( old < 0 )
		return;

	// After finding the element, we swap the last one with the
	// soon-to-be-deleted element, and then we decrease the list size.
	nbio_list[old] = nbio_list[nbio_list_len];
	nbio_list_len--;
}

void nbio_register_idle(int timeout_seconds, nbio_callback_t func)
{
	// The function that will be called after the timeout, if nothing
	// was available for reading.
	// The function will be called with -1 as parameter;
	//
	// Set negative timeout to mean "wait forever".
	//
	// Essentially, passing either a negative timeout or a NULL function
	// will disable this feature.

	nbio_timeout_seconds = timeout_seconds;
	nbio_idle_callback = func;
}

void nbio_stop_loop()
{
	nbio_loop_alive = 0;
}

void nbio_loop()
{
	int i;
	int max_fd;
	int retval;
	struct timeval timeout;
	struct timeval* select_timeout;
	fd_set read_set;

	nbio_loop_alive = 1;
	while(nbio_loop_alive)
	{
		if( nbio_timeout_seconds < 0 )
			select_timeout = NULL;
		else
		{
			timeout.tv_sec = nbio_timeout_seconds;
			timeout.tv_usec = 0;
			select_timeout = &timeout;
		}

		// Preparing the set...
		FD_ZERO(&read_set);
		for(i=0; i < nbio_list_len; i++)
			FD_SET(nbio_list[i].fd, &read_set);
		max_fd = find_max_fd();

		// Running select()
		retval = select(max_fd+1, &read_set, NULL, NULL, select_timeout);

		if( retval < 1 )
		{
			if( nbio_idle_callback )
				nbio_idle_callback(-1, NULL);
		}
		else
		{
			// Finding the returned FDs
			for(i=0; i < nbio_list_len; i++)
			{
				if( FD_ISSET(nbio_list[i].fd, &read_set) )
					nbio_list[i].callback(nbio_list[i].fd, nbio_list[i].user_data);
			}
		}
	}
}
