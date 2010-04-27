#include "fisica.h"

#include <string.h>
#include <malloc.h>

#include <netinet/in.h>
// typedef uint32_t in_addr_t;
// struct in_addr
// {
//     in_addr_t s_addr;
// };
// struct sockaddr_in
// {
//     // __SOCKADDR_COMMON(sin_);, which is defined at <bits/sockaddr.h>,
//     // which expands to: 
//     sa_family_t sin_family;
//     // which should have AF_INET value, defined at <bits/socket.h>
//
//     in_port_t sin_port;
//     struct in_addr sin_addr;
//
//     // zero-padding here
// };
// #define INADDR_ANY       ((in_addr_t) 0x00000000)
// #define INADDR_BROADCAST ((in_addr_t) 0xffffffff)
// #define INADDR_NONE      ((in_addr_t) 0xffffffff)
// #define INADDR_LOOPBACK  ((in_addr_t) 0x7f000001)

#include <sys/socket.h>
// bind()

#include <arpa/inet.h>
// inet_aton()
// inet_addr()


// Global variables (for current state)
static char* remote_host = NULL;
static int remote_port = 0;
static int local_port = 0;
static struct sockaddr_in local_addr;
static struct sockaddr_in remote_addr;


void set_pointer(void** cp, void* new_pointer)
{
	// This function frees the (*cp), if it is not null, and then make
	// it point to the new_pointer

	if( !cp )
		return;  // Passed with wrong parameter...
	if( *cp )
		free(*cp);
	*cp = new_pointer;
}


int fill_remote_addr_struct()
{
	// Fills the remote_addr global var using the values of the other
	// global vars.
	//
	// Returns 1 in success, or 0 in failure.

	// If the remote host is empty, let's assume it's the loopback
	if( remote_host[0] == '\0' )
		remote_addr.sin_addr.s_addr = INADDR_LOOPBACK;
	else
	{
		if( ! inet_aton(remote_host, &remote_addr.sin_addr) )
		{
			// Invalid address
			return 0;
		}
	}
	remote_addr.sin_port = htons(remote_port);
	remote_addr.sin_family = AF_INET;

	return 1;

	// getaddrinfo() is powerful, but too complicated for this program.
	// inet_aton() has error-checking, while inet_addr() doesn't.
}


void Pex_Set_Local_Port(int port)
{
	// This function should be called BEFORE P_Activate_Request()
	//
	// Setting the local port to zero will make P_Activate_Request() use
	// the same port as the destination one.

	local_port = port;
}

int P_Activate_Request(int port, char* host)
{
	// Returns 1 in success, or 0 in failure.
	//
	// The "host" string is duplicated here, so the caller is free to
	// modify and free it. This internal copy is automatically freed
	// when needed.

	set_pointer((void*) &remote_host, strdup(host));
	remote_port = port;

	if( ! fill_remote_addr_struct() )
		return 0;

	return 1;
	// TODO: stub
}

void P_Deactivate_Request(void)
{
	set_pointer((void*) &remote_port, NULL);
	// TODO: stub
}

void P_Data_Request(char c)
{
	// TODO: stub
}

int P_Data_Indication(void)
{
	// TODO: stub
	return 0;
}

char P_Data_Receive(void)
{
	// TODO: stub
	return 'F';
}

