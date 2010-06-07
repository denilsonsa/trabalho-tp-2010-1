#include "enlace.h"
#include "fisica.h"

#include <stdlib.h>
// NULL, malloc(), free(), rand(), RAND_MAX

#include <stdio.h>
// For error messages


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
	LS->promiscuous = 0;
	LS->recv_buffer_begin = 0;
	LS->recv_buffer_end = 0;
	LS->recv_buffer_has_frame = NO;
	LS->loss_probability = 0.0;
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
	LS->promiscuous = 0;
	LS->recv_buffer_begin = 0;
	LS->recv_buffer_end = 0;
	LS->recv_buffer_has_frame = NO;
	LS->loss_probability = 0.0;

	return LS;
}


void L_Set_Loss_Probability(link_state_t* LS, float chance)
{
	LS->loss_probability = chance;
}

void L_Set_Promiscuous(link_state_t* LS, int promiscuous)
{
	if( promiscuous )
		LS->promiscuous = 1;
	else
		LS->promiscuous = 0;
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


// Handy macro to access this circular buffer
#define BUF(i) (LS->recv_buffer[ (LS->recv_buffer_begin + (i)) % LINK_BUFFER_LEN])
// Used in parse_header() and L_Analyze_Buffer()

typedef struct link_frame_header {
	link_address_t src_addr;
	link_address_t dst_addr;
	unsigned char payload_len;
	unsigned char payload_checksum;
	unsigned char header_checksum;
} link_frame_header_t;

void parse_header(link_state_t* LS, link_frame_header_t* header)
{
	header->src_addr = BUF(1);
	header->dst_addr = BUF(2);
	header->payload_len = BUF(3);
	header->payload_checksum = BUF(4);
	header->header_checksum = BUF(5);
}


void L_Analyze_Buffer(link_state_t* LS)
{
	// Looks at the link layer buffer and looks for a frame.
	// In case there is a frame, a flag is set.
	// In case there is garbage, discard the garbage bytes.
	// In case there are too few bytes to decide, do nothing.

	if( LS->recv_buffer_has_frame == YES )
	{
		// Okay, we have a frame already in the buffer. There is
		// NOTHING I can do right now.
		return;
	}

	while( LS->recv_buffer_begin != LS->recv_buffer_end )
	{
		// The character at the beginning of the buffer
		char c = LS->recv_buffer[ LS->recv_buffer_begin ];

		if( c == '!' )
		{
			// It might be the beginning of a frame... Further processing
			// is required.

			int buf_len;

			buf_len = LS->recv_buffer_end - LS->recv_buffer_begin;
			if( buf_len < 0 )
				buf_len += LINK_BUFFER_LEN;

			if( buf_len < 8 )
			{
				// The minimum packet has 8 bytes.
				// Let's wait until we have enough data.
				break;
			}
			else
			{
				link_frame_header_t header;
				parse_header(LS, &header);

				if( LS->recv_buffer_has_frame == NO )
				{
					// Let's check the header!
					if( header.header_checksum
					!= ( header.src_addr
						^ header.dst_addr
						^ header.payload_len
						^ header.payload_checksum )
					)
					{
						printf("Incorrect header checksum.\n");
					}
					else
					{
						// Nice, we have a good header.
						LS->recv_buffer_has_frame = HEADER_OK;
					}
				}

				// Yeah, this is NOT "else if"
				if( LS->recv_buffer_has_frame == HEADER_OK )
				{
					if( buf_len < header.payload_len + 7)
					{
						// Not enough bytes. Come back later, after we receive
						// the rest of this frame.
						break;
					}
					else
					{
						// Does the package end correctly?
						if( BUF(header.payload_len+6) != '#' )
						{
							// Sorry... Discarding...
							printf("End-of-frame marker was not found.\n");
							LS->recv_buffer_has_frame = NO;
						}
						else
						{
							int i;
							unsigned char checksum;

							// Let's check the payload checksum
							checksum = 0;
							for(i=0; i<header.payload_len; i++)
							{
								checksum ^= BUF(6+i);
							}

							if( header.payload_checksum != checksum )
							{
								// Sorry...
								printf("Corrupted payload\n");
								LS->recv_buffer_has_frame = NO;
							}
							else
							{
								// Congratulations! You've got a frame!
								// But... Is it for you?
								if( LS->promiscuous
								|| header.dst_addr == LINK_ADDRESS_BROADCAST
								|| header.dst_addr == LS->local_addr )
								{
									// All the work so far...
									// 9-levels deep in conditionals...
									// And still there is a chance of
									// simulating a frame loss.
									if( rand() < RAND_MAX * LS->loss_probability )
									{
										printf("Frame lost due to probability.\n");
										LS->recv_buffer_has_frame = NO;
										LS->recv_buffer_begin = (LS->recv_buffer_begin + 7-1 + header.payload_len) % LINK_BUFFER_LEN;
									}
									else
									{
										LS->recv_buffer_has_frame = YES;
										break;
									}
								}
								else
								{
									printf("This frame is for '%c', and not for me.\n", header.dst_addr);
									LS->recv_buffer_has_frame = NO;
									LS->recv_buffer_begin = (LS->recv_buffer_begin + 7-1 + header.payload_len) % LINK_BUFFER_LEN;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			// Just garbage. Let's ignore it.
			//printf("Ignoring garbage '%c'...\n", c);
		}

		// Removing the first byte from the buffer.
		LS->recv_buffer_begin = (LS->recv_buffer_begin + 1) % LINK_BUFFER_LEN;
	}
}


void L_Receive_Callback(link_state_t* LS)
{
	// Runs the physical layer callback,
	// and then gets the byte from the physical layer into the link layer.

	P_Receive_Callback(LS->PS);

	while( P_Data_Indication(LS->PS) )
	{
		char c;
		int next_pos = (LS->recv_buffer_end + 1) % LINK_BUFFER_LEN;

		// Check if the buffer is full
		if( next_pos == LS->recv_buffer_begin )
		{
			// And then just stop receiving bytes, dropping any new
			// received bytes.
			printf("L_Receive_Callback(): Buffer full.\n");
			break;
		}
		else
		{
			// Well, the buffer is not full. Let's store that byte!
			c = P_Data_Receive(LS->PS);
			//printf("Storing '%c'...\n", c);
			LS->recv_buffer[ LS->recv_buffer_end ] = c;
			LS->recv_buffer_end = next_pos;
		}
	}

	L_Analyze_Buffer(LS);
}


int L_Data_Indication(link_state_t* LS)
{
	if( LS->recv_buffer_has_frame == YES )
		return 1;
	else
		return 0;
}

int L_Data_Receive(link_state_t* LS, link_address_t* src, link_address_t* dst, char* buf, int buf_len)
{
	// src will receive the src address.
	// dst will receive the destination address.
	// buf will receive the data.
	// buf_len is the size of buf.
	//
	// src and dst are optional (pass NULL in that case).
	//
	// Returns the number of bytes stored in buf, or returns -1 in case of
	// failure, or if buf_len is not long enough.

	int i;
	link_frame_header_t header;

	parse_header(LS, &header);

	if( buf_len < header.payload_len )
		// Buffer not long enough.
		return -1;
	if( ! buf )
		// Huh... Sanity check.
		return -1;

	if( src )
		*src = header.src_addr;
	if( dst )
		*dst = header.dst_addr;

	for(i=0; i<header.payload_len; i++)
	{
		buf[i] = BUF(6+i);
	}

	LS->recv_buffer_has_frame = NO;
	LS->recv_buffer_begin = (LS->recv_buffer_begin + 7 + header.payload_len) % LINK_BUFFER_LEN;

	return header.payload_len;
}
