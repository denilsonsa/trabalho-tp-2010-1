#include <stdio.h>

#include <stdlib.h>
// atoi(), exit()

#include <time.h>
// time()

#include "fisica.h"
#include "enlace.h"

#include "nbiocore.h"
#include "terminal.h"
#include "util.h"


// Program name, used in print_help()
char* g_program_name;


// Global list of virtual connections
struct virtual_connection
{
	int interface_number;
	int local_port;
	int remote_port;
	char* remote_host;
	physical_state_t PS;
	link_state_t LS;
};
struct virtual_connection* g_conn;
int g_conn_len;


// Global host-interface table, the core of a link-layer switch
struct host_interface
{
	//link_address_t address;
	int interface;
	time_t last_seen;
};
// Since there are only 256 available addresses, I'm using a fast, simple
// and extremely easy-to-use lookup table
struct host_interface switch_table[256];

// Setting the cache expiration time (in seconds).
#define CACHE_EXPIRE 30


void print_help()
{
	printf(
		"How to use:\n"
		"  %s <local port> <remote host>:<port> [...]\n"
		"\n"
		"This program receives two or more tuples of local+remote addresses. Each\n"
		"tuple will tie a remote address with a local port.\n"
		"\n"
		"Whenever a frame is received on any of the addresses, it will be replicated\n"
		"(sent) only to the destination address, behaving like a link-layer switch.\n"
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
		// Setting the interface number
		g_conn[i].interface_number = i;

		// Parsing each "localport host:port" tuple
		g_conn[i].local_port = atoi(argv[1 + 2*i]);
		if( g_conn[i].local_port == 0 )
			error = 1 + 2*i;

		if(! split_host_port( argv[2 + 2*i], &(g_conn[i].remote_host), &(g_conn[i].remote_port) ) )
			error = 2 + 2*i;
		if( g_conn[i].remote_port == 0 )
			error = 2 + 2*i;

		// Initializing these structs
		// (else bad things will happen!)
		clear_physical_state(&g_conn[i].PS);
		clear_link_state(&g_conn[i].LS);
	}

	if( error > 0 )
	{
		printf("Error parsing parameter %d. (pass no parameters for help)\n", error);
		exit(1);
	}
}


void read_from_network(int fd, void* void_conn)
{
	struct virtual_connection* conn;
	
	conn = (struct virtual_connection*) void_conn;

	L_Receive_Callback(&conn->LS);
	//printf("read_from_network(%d, %p);\n", fd, void_conn);

	while( L_Data_Indication(&conn->LS) )
	{
		link_address_t src;
		link_address_t dst;
		char buf[512];
		int len;
		time_t now;
		int should_broadcast = 1;

		len = L_Data_Receive(&conn->LS, &src, &dst, buf, 512);
		now = time(0);

		// Adding the source address to the table.
		switch_table[src].interface = conn->interface_number;
		switch_table[src].last_seen = now;

		if( dst != LINK_ADDRESS_BROADCAST )
		{
			// Checking if the destination address is known and still fresh
			if( switch_table[dst].interface > -1
			&& uabs(now - switch_table[dst].last_seen) < CACHE_EXPIRE )
			{
				int dst_interface;

				dst_interface = switch_table[dst].interface;

				if( dst_interface == conn->interface_number )
				{
					// Oops? Destination interface == Source interface?
					// Okay, let me discard this.
					printf("NOT redirecting from %c@%d to %c@%d.\n",
						src, conn->interface_number,
						dst, g_conn[dst_interface].interface_number);
				}
				else
				{
					printf("Redirecting from %c@%d to %c@%d.\n",
						src, conn->interface_number,
						dst, g_conn[dst_interface].interface_number);

					// HACK: Let's fake our source address
					g_conn[dst_interface].LS.local_addr = src;
					L_Data_Request(&g_conn[dst_interface].LS, dst, buf, len);
				}

				should_broadcast = 0;
			}
		}

		if( should_broadcast )
		{
			int i;

			printf("Broadcasting from %c@%d to %c.\n",
				src, conn->interface_number,
				dst);

			for(i=0; i<g_conn_len; i++)
			{
				if( &g_conn[i] != conn )
				{
					// HACK: Let's fake our source address
					g_conn[i].LS.local_addr = src;
					L_Data_Request(&g_conn[i].LS, dst, buf, len);
				}
			}
		}
	}
}


int main(int argc, char* argv[])
{
	int i;

	g_program_name = argv[0];
	parse_arguments(argc, argv);

	for(i=0; i<256; i++)
	{
		switch_table[i].interface = -1;
		switch_table[i].last_seen = 0;
	}

	for(i=0; i<g_conn_len; i++)
	{
		void* TMP;

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

		TMP = L_Activate_Request(
			&g_conn[i].LS,
			0,  // link_address_t is zero, because this switch does not have an address,
			    // and because it doesn't matter while in promiscuous mode.
			&g_conn[i].PS);

		if( !TMP )
		{
			printf("L_Activate_Request failed to activate the tuple number %d (%d %s:%d).\n",
				i, g_conn[i].local_port, g_conn[i].remote_host, g_conn[i].remote_port);
			exit(1);
		}

		// Since this is a switch, it must receive ALL frames
		L_Set_Promiscuous(&g_conn[i].LS, 1);

		//L_Set_Loss_Probability(&g_conn[i].LS, 0.0);
	}

	srand(time(NULL));

	for(i=0; i<g_conn_len; i++)
	{
		nbio_register(g_conn[i].PS.socket_fd, read_from_network, &g_conn[i]);
	}

	printf("Link-layer switch up, listening to %d simulated physical connections.\n", g_conn_len);
	printf("(note that this switch does not accept terminal input)\n");

	nbio_loop();

	printf("Goodbye!\n");

	for(i=0; i<g_conn_len; i++)
	{
		free(g_conn[i].remote_host);
		L_Deactivate_Request(&g_conn[i].LS);
		P_Deactivate_Request(&g_conn[i].PS);
	}
	free(g_conn);

	return 0;
}
