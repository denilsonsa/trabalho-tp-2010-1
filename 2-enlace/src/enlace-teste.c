#include <stdio.h>

#include <string.h>

#include <stdlib.h>
// atoi(), exit(), srand()

#include <time.h>
// time()

#include <unistd.h>
// STDIN_FILENO, sleep()

#include "fisica.h"
#include "enlace.h"

#include "nbiocore.h"
#include "util.h"


// Program name, used in print_help()
char* g_program_name;


struct program_arguments
{
	link_address_t link_addr;
	int local_port;
	int remote_port;
	char* remote_host;
};


void print_help()
{
	printf(
		"Possible command-lines:\n"
		"  %s <local link address> <remote host>:<port>\n"
		"  %s <local link address> <local port> <remote host>:<port>\n"
		"If the remote host is empty, it is a shortcut for the loopback address:\n"
		"  %s <local link address> <local port> :<port>\n"
		"The local link address is just one character.\n",
		g_program_name,
		g_program_name,
		g_program_name 
	);
}


void parse_arguments(int argc, char* argv[], struct program_arguments* args)
{
	int error = 0;

	if( argc == 3 )
	{
		// progname link_addr host:port
		args->link_addr = argv[1][0];
		args->local_port = 0;
		if(! split_host_port( argv[2], &(args->remote_host), &(args->remote_port) ) )
			error = 1;
		if( args->link_addr == 0 )
			error = 1;
		if( args->remote_port == 0 )
			error = 1;
	}
	else if( argc == 4 )
	{
		// progname link_addr localport host:port
		args->link_addr = argv[1][0];
		args->local_port = atoi(argv[2]);
		if(! split_host_port( argv[3], &(args->remote_host), &(args->remote_port) ) )
			error = 1;
		if( args->link_addr == 0 )
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


void read_from_stdin(int fd, void* LS)
{
	char s[1024];

	if( fgets(s, 1024, stdin) )
	{
		int len = strlen(s);

		// Two explict vars just to solve compiler signed warnings
		link_address_t dest = s[0];

		if( len > 1 )
			L_Data_Request(LS, dest, &s[1], len-1);
	}
	else
		nbio_stop_loop();
}

void read_from_network(int fd, void* LS)
{
	L_Receive_Callback(LS);
	while( L_Data_Indication(LS) ) // This should be a while()
	{
		link_address_t src;
		link_address_t dst;
		char buf[512];
		int len;

		len = L_Data_Receive(LS, &src, &dst, buf, 512);
		printf("Got a frame of size %d from '%c' to '%c':\n", len, src, dst);
		fwrite(buf, 1, len, stdout);
		fflush(stdout);
	}
}


int main(int argc, char* argv[])
{
	struct program_arguments args;
	physical_state_t* PS;
	link_state_t* LS;

	g_program_name = argv[0];
	parse_arguments(argc, argv, &args);

	PS = P_Activate_Request(NULL, args.remote_port, args.remote_host, args.local_port);
	if( !PS )
	{
		printf("Noooooo! P_Activate_Request failed to activate your request.\n");
		return 1;
	}

	LS = L_Activate_Request(NULL, args.link_addr, PS);
	if( !LS )
	{
		printf("Noooooo! L_Activate_Request failed to activate your request.\n");
		return 1;
	}

	//L_Set_Promiscuous(LS, 1);

	L_Set_Loss_Probability(LS, 0.0);
	srand(time(NULL));

	// The Non-Blocking I/O core, also called "N-B I/O"
	nbio_register(STDIN_FILENO, read_from_stdin, LS);
	nbio_register(PS->socket_fd, read_from_network, LS);

	printf("\"Ready to work.\" - Peon from Warcraft II\n");
	nbio_loop();
	printf("\nSo long, and thanks for all the... frames?\n");

	L_Deactivate_Request(LS);
	free_link_state(LS);
	P_Deactivate_Request(PS);
	free_physical_state(PS);

	return 0;
}
