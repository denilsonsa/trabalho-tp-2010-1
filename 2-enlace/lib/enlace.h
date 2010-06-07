#ifndef HAS_ENLACE_H
#define HAS_ENLACE_H

#include "fisica.h"

#define LINK_BUFFER_LEN 1024

typedef unsigned char link_address_t;

#define LINK_ADDRESS_BROADCAST ((link_address_t) '*')

enum has_frame_state_t {
	NO,
	HEADER_OK,
	YES
};

// The data link layer struct
typedef struct link_state
{
	link_address_t local_addr;
	physical_state_t* PS;

	int promiscuous;

	unsigned char recv_buffer[LINK_BUFFER_LEN];
	int recv_buffer_begin;
	int recv_buffer_end;

	enum has_frame_state_t recv_buffer_has_frame;
} link_state_t;

link_state_t* malloc_link_state();
void clear_link_state(link_state_t*);
void free_link_state(link_state_t*);

link_state_t* L_Activate_Request(link_state_t*, link_address_t, physical_state_t*);
void L_Deactivate_Request(link_state_t*);
void L_Data_Request(link_state_t*, link_address_t, const char*, int);
int L_Data_Indication(link_state_t*);
int L_Data_Receive(link_state_t*, link_address_t*, link_address_t*, char*, int);

void L_Set_Promiscuous(link_state_t*, int);
void L_Receive_Callback(link_state_t*);

#endif
