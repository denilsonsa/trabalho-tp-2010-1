#include <stdio.h>
#include <stdlib.h>

#include <termios.h>
// tcsetattr(), tcgetattr(), TCSANOW, ICANON, struct termios

#include <unistd.h>
// STDIN_FILENO, sleep()

#include "fisica.h"
#include "fisica-teste-args.h"

#include "nbiocore.h"


static struct termios g_terminal_flags;

void restore_terminal_flags()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &g_terminal_flags);
}


void read_from_stdin(int fd)
{
	int c;
	c = getchar();

	// 0x04 is EOT (end of transmission), which is received
	// when the terminal is in non-canonical mode
	if( c == EOF || c == 0x04 )
		nbio_stop_loop();
	else
		P_Data_Request(c);
}

void read_from_network(int fd)
{
	Pex_Receive_Callback(fd);

	while( P_Data_Indication() )
	{
		putchar(P_Data_Receive());
	}
	fflush(stdout);
}


int main(int argc, char* argv[])
{
	struct program_arguments args;

	g_program_name = argv[0];
	parse_arguments(argc, argv, &args);

	//printf("local_port: %d\n", args.local_port);
	//printf("remote_host: %s\n", args.remote_host);
	//printf("remote_port: %d\n", args.remote_port);

	Pex_Set_Local_Port(args.local_port);
	if( ! P_Activate_Request(args.remote_port, args.remote_host) )
	{
		printf("I'm really sorry to tell you that P_Activate_Request failed to activate your request.\n");
		return 1;
	}

	// Setting unbuffered stdout
	//setvbuf(stdout, NULL, _IONBF, 0);

	// Setting unbuffered stdin (but this doesn't quite work)
	//setvbuf(stdin, NULL, _IONBF, 0);

	// Changing terminal settings for non-canonical mode
	// (i.e. disables line-editing and line-buffering)
	// http://stackoverflow.com/questions/1798511/how-to-avoid-press-enter-with-any-getchar/1798833#1798833
	if( tcgetattr(STDIN_FILENO, &g_terminal_flags) == 0)
	{
		struct termios new_flags;

		atexit(restore_terminal_flags);	
		new_flags = g_terminal_flags;
		new_flags.c_lflag &= ~(ICANON); 
		tcsetattr(STDIN_FILENO, TCSANOW, &new_flags);
	}

	// The Non-Blocking I/O core, also called "N-B I/O"
	nbio_register(STDIN_FILENO, read_from_stdin);
	nbio_register(Pex_Get_Socket_Fd(), read_from_network);

	printf("\"At your service.\" - Footman from Warcraft II\n");
	nbio_loop();
	printf("\nSo long, and thanks for all the fish!\n");

	P_Deactivate_Request();

	return 0;
}
