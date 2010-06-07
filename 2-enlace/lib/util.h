#ifndef HAS_UTIL_H
#define HAS_UTIL_H

void set_pointer(void** cp, void* new_pointer);
int split_host_port(const char* input, char** host, int* port);

#endif
