#include <stdio.h>

#include "fisica.h"
#include "fisica-teste-args.h"

int main(int argc, char* argv[])
{
	struct program_arguments args;

	g_program_name = argv[0];
	parse_arguments(argc, argv, &args);

	printf("local_port: %d\n", args.local_port);
	printf("remote_host: %s\n", args.remote_host);
	printf("remote_port: %d\n", args.remote_port);

	return 0;
}
