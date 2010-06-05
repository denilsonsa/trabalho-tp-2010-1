struct program_arguments
{
	int local_port;
	int remote_port;
	char* remote_host;
};

extern char* g_program_name;

void print_help();
int split_host_port(char* input, char** host, int* port);
void parse_arguments(int argc, char* argv[], struct program_arguments* args);
