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


struct physical_connection
{
	int local_port;
	int remote_port;
	char* remote_host;
	physical_state_t PS;
};

// Global list of physical connections
struct physical_connection* g_conn;
int g_conn_len;


void print_help()
{
	printf(
		"How to use:\n"
		"  %s <local port> <remote host>:<port> [...]\n"
		"\n"
		"This program receives two or more tuples of local+remote addresses. Each\n"
		"tuple will tie a remote address with a local port.\n"
		"\n"
		"Whenever data is received on any of the addresses, it will be replicated\n"
		"(sent) to all other addresses, behaving just like a physical hub, or like a\n"
		"broadcast transmission media."
		"\n"
		"If the remote host is empty, it is a shortcut for the loopback address:\n"
		"  %s <local port> :<port>\n",
		g_program_name,
		g_program_name 
	);
}


void parse_arguments(int argc, char* argv[])
{
	int error = 0;
	int i;

	if( argc == 1 )
	{
		print_help();
		exit(1);
	}

	if( argc < 5 )
	{
		printf("Not enough parameters. (pass no parameters for help)\n");
		exit(1);
	}

	if( (argc-1)%2 != 0 )
	{
		printf("Odd number of parameters. (pass no parameters for help)\n");
		exit(1);
	}

	// Okay, correct number of parameters. Let's parse and store all of them.
	g_conn_len = (argc-1)/2;
	g_conn = malloc(g_conn_len * sizeof(*g_conn));
	if( !g_conn )
	{
		printf("Error calling malloc().\n");
		exit(1);
	}

	for(i=0; i<g_conn_len; i++)
	{
		// Parsing each "localport host:port" tuple
		g_conn[i].local_port = atoi(argv[1 + 2*i]);
		if( g_conn[i].local_port == 0 )
			error = 1 + 2*i;

		if(! split_host_port( argv[2 + 2*i], &(g_conn[i].remote_host), &(g_conn[i].remote_port) ) )
			error = 2 + 2*i;
		if( g_conn[i].remote_port == 0 )
			error = 2 + 2*i;

		// Initializing this struct
		// (else bad things will happen!)
		clear_physical_state(&g_conn[i].PS);
	}

	if( error > 0 )
	{
		printf("Error parsing parameter %d. (pass no parameters for help)\n", error);
		exit(1);
	}
}


void read_from_network(int fd, void* void_conn)
{
	char c;
	int i;
	struct physical_connection* conn;
	
	conn = (struct physical_connection*) void_conn;

	Pex_Receive_Callback(&conn->PS);
	//printf("read_from_network(%d, %p);\n", fd, void_conn);

	while( P_Data_Indication(&conn->PS) )
	{
		c = P_Data_Receive(&conn->PS);
		putchar(c);
		for(i=0; i<g_conn_len; i++)
		{
			if( &g_conn[i] != conn )
				P_Data_Request(&g_conn[i].PS, c);
		}
	}
	fflush(stdout);
}


int main(int argc, char* argv[])
{
	int i;

	g_program_name = argv[0];
	parse_arguments(argc, argv);

	for(i=0; i<g_conn_len; i++)
	{
		physical_state_t* TMP;
		TMP = P_Activate_Request(
			&g_conn[i].PS,
			g_conn[i].remote_port, g_conn[i].remote_host,
			g_conn[i].local_port);

		if( !TMP )
		{
			printf("P_Activate_Request failed to activate the tuple number %d (%d %s:%d).\n",
				i, g_conn[i].local_port, g_conn[i].remote_host, g_conn[i].remote_port);
			exit(1);
		}
	}

	for(i=0; i<g_conn_len; i++)
	{
		nbio_register(g_conn[i].PS.socket_fd, read_from_network, &g_conn[i]);
	}

	printf("Physical hub up, listening to %d simulated physical connections.\n", g_conn_len);
	printf("(note that this hub does not accept terminal input)\n");

	nbio_loop();

	printf("Goodbye!\n");

	for(i=0; i<g_conn_len; i++)
	{
		free(g_conn[i].remote_host);
		P_Deactivate_Request(&g_conn[i].PS);
	}
	free(g_conn);

	return 0;
}
