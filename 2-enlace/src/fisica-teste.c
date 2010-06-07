#include <stdio.h>

#include <stdlib.h>
// atoi(), exit()

#include <unistd.h>
// STDIN_FILENO, sleep()

#include "fisica.h"

#include "nbiocore.h"
#include "terminal.h"
#include "util.h"


// Program name, used in print_help()
char* g_program_name;


struct program_arguments
{
	int local_port;
	int remote_port;
	char* remote_host;
};


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


void read_from_stdin(int fd, void* PS)
{
	int c;
	c = getchar();

	// 0x04 is EOT (end of transmission), which is received
	// when the terminal is in non-canonical mode
	if( c == EOF || c == 0x04 )
		nbio_stop_loop();
	else
		P_Data_Request(PS, c);
}

void read_from_network(int fd, void* PS)
{
	P_Receive_Callback(PS);

	while( P_Data_Indication(PS) )
	{
		putchar(P_Data_Receive(PS));
	}
	fflush(stdout);
}


int main(int argc, char* argv[])
{
	struct program_arguments args;
	physical_state_t* PS;

	g_program_name = argv[0];
	parse_arguments(argc, argv, &args);

	//printf("local_port: %d\n", args.local_port);
	//printf("remote_host: %s\n", args.remote_host);
	//printf("remote_port: %d\n", args.remote_port);

	PS = P_Activate_Request(NULL, args.remote_port, args.remote_host, args.local_port);
	if( !PS )
	{
		printf("I'm really sorry to tell you that P_Activate_Request failed to activate your request.\n");
		return 1;
	}

	// Set the terminal to non-canonical mode
	set_terminal_flags();

	// The Non-Blocking I/O core, also called "N-B I/O"
	nbio_register(STDIN_FILENO, read_from_stdin, PS);
	nbio_register(PS->socket_fd, read_from_network, PS);

	printf("\"At your service.\" - Footman from Warcraft II\n");
	nbio_loop();
	printf("\n\"So long, and thanks for all the fish!\" - Dolphins from Hitchhiker's Guide to the Galaxy\n");

	P_Deactivate_Request(PS);
	free_physical_state(PS);

	return 0;
}
