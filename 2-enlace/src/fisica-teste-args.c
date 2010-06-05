#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fisica-teste-args.h"

// Global var used in print_help()
char* g_program_name;


void print_help()
{
	printf(
		"Possible command-lines:\n"
		"  %s <remote host>:<port>\n"
		"  %s <local port> <remote host>:<port>\n"
		"If the remote host is empty, it is a shortcut for the loopback address:\n"
		"  %s <local port> :<port>\n",
		g_program_name,
		g_program_name,
		g_program_name 
	);
}


int split_host_port(char* input, char** host, int* port)
{
	// This function make *host point to a newly allocated string.
	// Returns 1 on success, 0 on failure.

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


void parse_arguments(int argc, char* argv[], struct program_arguments* args)
{
	int error = 0;

	if( argc == 2 )
	{
		// progname host:port
		args->local_port = 0;
		if(! split_host_port( argv[1], &(args->remote_host), &(args->remote_port) ) )
			error = 1;
		if( args->remote_port == 0 )
			error = 1;
	}
	else if( argc == 3 )
	{
		// progname localport host:port
		args->local_port = atoi(argv[1]);
		if(! split_host_port( argv[2], &(args->remote_host), &(args->remote_port) ) )
			error = 1;
		if( args->local_port == 0 )
			error = 1;
		if( args->remote_port == 0 )
			error = 1;
	}
	else
	{
		print_help();
		exit(1);
	}
	if( error == 1 )
	{
		printf("Error parsing parameters. (pass no parameters for help)\n");
		exit(1);
	}
}
