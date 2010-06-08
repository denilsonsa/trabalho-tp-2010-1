#ifndef HAS_UTIL_H
#define HAS_UTIL_H

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

// stdlib.h already has abs(), but it's only for integers.
// "uabs()" works for every type. ("u" comes from "util.h")
#define uabs(a) (((a)<0)?-(a):(a))

void set_pointer(void** cp, void* new_pointer);
int split_host_port(const char* input, char** host, int* port);

#endif
