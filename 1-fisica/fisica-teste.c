#include <stdio.h>

#include "fisica.h"
#include "fisica-teste-args.h"

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



	P_Deactivate_Request();

	return 0;
}
