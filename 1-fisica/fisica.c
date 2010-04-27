#include "fisica.h"

#include <malloc.h>

#include <netinet/in.h>
// typedef uint32_t in_addr_t;
// struct in_addr
// {
//     in_addr_t s_addr;
// };
// struct sockaddr_in
// {
//     // other stuff here
//     in_port_t sin_port;
//     struct in_addr sin_addr;
//     // zero-padding here
// };
// #define INADDR_ANY       ((in_addr_t) 0x00000000)
// #define INADDR_BROADCAST ((in_addr_t) 0xffffffff)
// #define INADDR_NONE      ((in_addr_t) 0xffffffff)
// #define INADDR_LOOPBACK  ((in_addr_t) 0x7f000001)

#include <sys/socket.h>
// bind()


// Global variables (for current state)
static char* remote_host = NULL;
static int remote_port = 0;
static int local_port = 0;
static struct sockaddr_in local_addr;
static struct sockaddr_in remote_addr;


void set_pointer(char ** cp, char * new_pointer)
{
	// This function frees the (*cp), if it is not null, and then make
	// it point to the new_pointer

	if( !cp )
		return;  // Passed with wrong parameter...
	if( *cp )
		free(*cp);
	*cp = new_pointer;
}


int Pex_Set_Local_Port(int port)
{
	// This function should be called BEFORE P_Activate_Request()
	//
	// Setting the local port to zero will make P_Activate_Request() use
	// the same port as the destination one.

	local_port = port;
}

int P_Activate_Request(int port, char* host)
{
	//set_pointer()
	// See getaddrinfo(), inet_aton() or inet_addr()
	// TODO: stub
}

void P_Deactivate_Request(void)
{
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

