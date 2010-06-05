#include "util.h"

#include <string.h>
// strchr(), stdndup()

#include <stdlib.h>
// atoi(), free()


void set_pointer(void** cp, void* new_pointer)
{
	// This function frees the (*cp), if it is not NULL, and then makes
	// it point to the new_pointer

	if( !cp )
		return;  // Called with wrong parameter...
	if( *cp )
		free(*cp);
	*cp = new_pointer;
}


int split_host_port(char* input, char** host, int* port)
{
	// This function receives a host IP and a port (like these:
	// "1.2.3.4:56", "localhost:1234") and saves a new string with
	// just the host and an integer with the port.
	//
	// If the port is empty (like this: "localhost:"), then port will be
	// considered zero.
	//
	// Returns 1 on success, 0 on failure.
	//
	// (*host) pointer will point to a newly allocated string that must
	// be freed with free().

	char* colon_pointer;

	colon_pointer = strchr(input, ':');
	if( ! colon_pointer )
		return 0;

	*host = strndup(input, colon_pointer - input);
	*port = atoi(colon_pointer + 1);
	return 1;

	// GNU sscanf has 'a' modifier that allocates a string of the
	// required size, but I chose to not use this feature.
}

