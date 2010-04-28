#include <stdlib.h>
// Just for the 'NULL'

#include <sys/select.h>

#include "nbiocore.h"

static int nbio_loop_alive = 1;
static int nbio_timeout_seconds = 1;
static nbio_callback_t nbio_idle_callback = NULL;

#define MAX_LIST_LEN 256
static int list_fd[MAX_LIST_LEN];
static nbio_callback_t list_callback[MAX_LIST_LEN];
static int list_len;


int find_max_fd()
{
	// Undefined result if the list is empty
	int i;
	int m;

	m = list_fd[0];
	for(i=1; i < list_len; i++)
	{
		if( list_fd[i] > m )
			m = list_fd[i];
	}
	return m;
}

int find_index_by_fd(int fd)
{
	int i;
	for(i=0; i < list_len; i++)
	{
		if( list_fd[i] == fd )
			return i;
	}
	return -1;
}


void nbio_register(int fd, nbio_callback_t func)
{
	list_fd[list_len] = fd;
	list_callback[list_len] = func;
	list_len++;
}

void nbio_unregister(int fd)
{
	int old;

	old = find_index_by_fd(fd);
	if( old < 0 )
		return;

	// After finding the element, we swap the last one with the
	// soon-to-be-deleted element, and then we decrease the list size.
	list_fd[old] = list_fd[list_len];
	list_callback[old] = list_callback[list_len];
	list_len--;
}

void nbio_set_timeout(int seconds)
{
	// How long to wait?
	// Set negative to "forever"
	nbio_timeout_seconds = seconds;
}

void nbio_register_idle(nbio_callback_t func)
{
	// The function that will be called after the timeout, if nothing
	// was available for reading.
	// The function will be called with -1 as parameter;
	// Register a NULL pointer to disable the idle function.
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
		for(i=0; i < list_len; i++)
			FD_SET(list_fd[i], &read_set);
		max_fd = find_max_fd();

		// Running select()
		retval = select(max_fd+1, &read_set, NULL, NULL, select_timeout);

		if( retval < 1 )
		{
			if( nbio_idle_callback )
				nbio_idle_callback(-1);
		}
		else
		{
			// Finding the returned FDs
			for(i=0; i < list_len; i++)
			{
				if( FD_ISSET(list_fd[i], &read_set) )
					list_callback[i](list_fd[i]);
			}
		}
	}
}
