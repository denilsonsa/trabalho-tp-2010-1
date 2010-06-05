#include "fisica.h"

#include "util.h"
// set_pointer()

#include <string.h>
#include <malloc.h>

#include <unistd.h>
// close()

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
// sendto()

#include <arpa/inet.h>
// inet_aton()
// inet_addr()


void close_socket_fd(physical_state_t* PS)
{
	// Closes the socket, if there is a socket open, then sets it -1.
	if( PS->socket_fd > -1 )
		close(PS->socket_fd);
	PS->socket_fd = -1;
}

physical_state_t* malloc_physical_state()
{
	physical_state_t* PS;
	PS = malloc(sizeof(physical_state_t));
	if(PS)
	{
		PS->remote_host = NULL;
		PS->remote_port = 0;
		PS->local_port = 0;
		PS->socket_fd = -1;
		PS->recv_buffer_has_data = 0;
	}
	return PS;
}

void free_physical_state(physical_state_t* PS)
{
	set_pointer((void*) &(PS->remote_host), NULL);
	free(PS);
}


int fill_local_addr_struct(physical_state_t* PS)
{
	// Fills the local_addr struct using the values of other vars.
	//
	// Returns 1 in success, or ... well, it never fails!

	// htonl() is not really needed in this case
	PS->local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	PS->local_addr.sin_port = htons(PS->local_port);
	PS->local_addr.sin_family = AF_INET;

	memset(PS->local_addr.sin_zero, 0, sizeof PS->local_addr.sin_zero);

	return 1;
}

int fill_remote_addr_struct(physical_state_t* PS)
{
	// Fills the remote_addr struct using the values of other vars.
	//
	// Returns 1 in success, or 0 in failure.

	// If the remote host is empty, let's assume it's the loopback
	if( PS->remote_host[0] == '\0' )
	{
		// htonl() is mandatory here
		PS->remote_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	}
	else
	{
		if( ! inet_aton(PS->remote_host, &(PS->remote_addr.sin_addr)) )
		{
			// Invalid address
			return 0;
		}
	}
	PS->remote_addr.sin_port = htons(PS->remote_port);
	PS->remote_addr.sin_family = AF_INET;

	memset(PS->remote_addr.sin_zero, 0, sizeof PS->remote_addr.sin_zero);

	return 1;

	// getaddrinfo() is powerful, but too complicated for this program.
	// inet_aton() has error-checking, while inet_addr() doesn't.
}


void Pex_Receive_Callback(physical_state_t* PS)
{
	// Callback for nbiocore call
	struct sockaddr_in src_addr;
	socklen_t src_addr_len;
	int bytes_received;

	bytes_received = recvfrom(PS->socket_fd,
		PS->recv_buffer, sizeof(PS->recv_buffer), 0,
		(struct sockaddr*) &src_addr, &src_addr_len);

	if( bytes_received > 0 )
		PS->recv_buffer_has_data = 1;

	// ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
	//   struct sockaddr *src_addr, socklen_t *addrlen);
}


physical_state_t* P_Activate_Request(int remote_port, char* host, int local_port)
{
	// Returns a new physical_state_t struct in success, or NULL in failure.
	//
	// The "host" string is duplicated here, so the caller is free to
	// modify and free it. This copy is automatically freed on free_physical_state().
	//
	// If the local_port is zero, then it will use the remote_port value instead.

	physical_state_t* PS;
	PS = malloc_physical_state();
	if( ! PS )
		return NULL;

	close_socket_fd(PS);
	set_pointer((void*) &(PS->remote_host), strdup(host));
	PS->remote_port = remote_port;
	PS->local_port = local_port;

	if( PS->local_port == 0 )
		PS->local_port = PS->remote_port;

	if( ! fill_remote_addr_struct(PS) )
	{
		free_physical_state(PS);
		return NULL;
	}

	if( ! fill_local_addr_struct(PS) )
	{
		free_physical_state(PS);
		return NULL;
	}

	PS->socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//socket_fd = socket(PF_INET, SOCK_DGRAM|SOCK_NONBLOCK, IPPROTO_UDP);
	if( PS->socket_fd < 0 )
	{
		free_physical_state(PS);
		return NULL;
	}

	if( bind(PS->socket_fd, (struct sockaddr*) &(PS->local_addr), sizeof PS->local_addr) < 0 )
	{
		close_socket_fd(PS);
		free_physical_state(PS);
		return NULL;
	}

	return PS;
}

void P_Deactivate_Request(physical_state_t* PS)
{
	close_socket_fd(PS);
	set_pointer((void*) &(PS->remote_host), NULL);
}

void P_Data_Request(physical_state_t* PS, char c)
{
	int bytes_sent;

	bytes_sent = sendto(PS->socket_fd, &c, 1, 0, (struct sockaddr*) &(PS->remote_addr), sizeof PS->remote_addr);

	// ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
	//   const struct sockaddr *dest_addr, socklen_t addrlen);
}

int P_Data_Indication(physical_state_t* PS)
{
	if( PS->recv_buffer_has_data )
		return 1;
	else
		return 0;
}

char P_Data_Receive(physical_state_t* PS)
{
	PS->recv_buffer_has_data = 0;
	return PS->recv_buffer[0];
}

