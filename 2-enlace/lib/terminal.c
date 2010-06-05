#include <stdlib.h>
// atexit()

#include <termios.h>
// tcsetattr(), tcgetattr(), TCSANOW, ICANON, struct termios

#include <unistd.h>
// STDIN_FILENO, sleep()


// Terminal flags before setting the non-canonical mode
static struct termios g_terminal_flags;


void restore_terminal_flags(void)
{
	// Restores the terminal flags to the values before we changed them.
	// This should be called upon program exit.

	tcsetattr(STDIN_FILENO, TCSANOW, &g_terminal_flags);
}


void set_terminal_flags(void)
{
	// Sets the terminal flags so that I/O will be unbuffered.
	// It will also register restore_terminal_flags() on atexit().

	// These two functions didn't work as I expected:
	//
	// Setting unbuffered stdout
	//setvbuf(stdout, NULL, _IONBF, 0);
	//
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
}
