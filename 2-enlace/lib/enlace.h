#ifndef HAS_ENLACE_H
#define HAS_ENLACE_H

#include "fisica.h"

#define LINK_BUFFER_LEN 1024

typedef unsigned char link_address_t;

// The data link layer struct
typedef struct link_state
{
	link_address_t local_addr;
	physical_state_t* PS;

	unsigned char recv_buffer[LINK_BUFFER_LEN];
	int recv_buffer_begin;
	int recv_buffer_end;
} link_state_t;

link_state_t* malloc_link_state();
void clear_link_state(link_state_t*);
void free_link_state(link_state_t*);

link_state_t* L_Activate_Request(link_state_t*, link_address_t, physical_state_t*);
void L_Deactivate_Request(link_state_t*);
void L_Data_Request(link_state_t*, link_address_t, const char*, int);

#endif
