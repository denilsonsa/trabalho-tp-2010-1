#include "enlace.h"
#include "fisica.h"

#include <stdlib.h>
// NULL, malloc(), free()


link_state_t* malloc_link_state()
{
	link_state_t* LS;
	LS = malloc(sizeof(link_state_t));
	if(LS)
		clear_link_state(LS);
	return LS;
}

void clear_link_state(link_state_t* LS)
{
	LS->local_addr = 0;
	LS->PS = NULL;
	LS->recv_buffer_begin = 0;
	LS->recv_buffer_end = 0;
}

void free_link_state(link_state_t* LS)
{
	// Warning! The PS pointer is NOT freed.
	free(LS);
}


link_state_t* L_Activate_Request(link_state_t* LS, link_address_t local_addr, physical_state_t* PS)
{
	// Similar to P_Activate_Request(), if the LS parameter is passed, then
	// everything is stored in that struct. If NULL is passed, then a new
	// struct will be allocated using malloc().
	//
	// Will return NULL on error, or LS on success.
	//
	// A pointer to the PS struct will be stored inside LS.

	int has_used_malloc = 0;

	if( LS == NULL )
	{
		LS = malloc_link_state();
		has_used_malloc = 1;
		if( ! LS )
			return NULL;
	}

	LS->local_addr = local_addr;
	LS->PS = PS;
	LS->recv_buffer_begin = 0;
	LS->recv_buffer_end = 0;

	return LS;
}


void L_Deactivate_Request(link_state_t* LS)
{
	// Do nothing?!
}


void L_Data_Request(link_state_t* LS, link_address_t dest_addr, const char* buf, int buf_len)
{
	char header_checksum;
	char payload_checksum;
	int i;

	if( buf_len < 1 )
		// I can't send... nothing!
		return;

	if( buf_len > 255 )
		// Sorry, but any data beyond the maximum payload size is
		// silently discarded.
		buf_len = 255;

	payload_checksum = 0;
	for(i=0; i<buf_len; i++)
	{
		payload_checksum ^= buf[i];
	}

	header_checksum = LS->local_addr ^ dest_addr ^ buf_len ^ payload_checksum;

	P_Data_Request(LS->PS, '!');  // Begin of frame marker
	P_Data_Request(LS->PS, LS->local_addr);   // Local Link address
	P_Data_Request(LS->PS, dest_addr);        // Remote Link address
	P_Data_Request(LS->PS, buf_len);          // Payload size
	P_Data_Request(LS->PS, payload_checksum); // Payload checksum
	P_Data_Request(LS->PS, header_checksum);  // XOR of all above fields
	for(i=0; i<buf_len; i++)
		P_Data_Request(LS->PS, buf[i]);  // The payload data
	P_Data_Request(LS->PS, '#');  // End of frame marker
}


void L_TODO_RENAME_ME(link_state_t* LS)
{
	// Looks at the link layer buffer and looks for a frame.
}


void L_Receive_Callback(link_state_t* LS)
{
	// Runs the physical layer callback,
	// and then gets the byte from the physical layer into the link layer.

	Pex_Receive_Callback(LS->PS);

	while( P_Data_Indication(LS->PS) )
	{
		char c;
		c = P_Data_Receive(LS->PS);
		LS->recv_buffer[ LS->recv_buffer_end ] = c;

		LS->recv_buffer_end++;
		LS->recv_buffer_end %= LINK_BUFFER_LEN;

		// WARNING! There is no buffer overflow checking!
	}

	L_TODO_RENAME_ME(LS);
}
