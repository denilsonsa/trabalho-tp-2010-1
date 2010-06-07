#ifndef HAS_FISICA_H
#define HAS_FISICA_H

#include <netinet/in.h>

// This code will not work with length different than 1
#define PHYSICAL_BUFFER_LEN 1

// The struct that holds all the required state about the physical layer
typedef struct physical_state
{
	char* remote_host;
	int remote_port;
	int local_port;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	int socket_fd;

	char recv_buffer[PHYSICAL_BUFFER_LEN];
	int recv_buffer_has_data;
} physical_state_t;

physical_state_t* malloc_physical_state();
void clear_physical_state(physical_state_t*);
void free_physical_state(physical_state_t*);

physical_state_t* P_Activate_Request(physical_state_t*, int, const char*, int);
void P_Deactivate_Request(physical_state_t*);
void P_Data_Request(physical_state_t*, char);
int P_Data_Indication(physical_state_t*);
char P_Data_Receive(physical_state_t*);

void P_Receive_Callback(physical_state_t*);

#endif
