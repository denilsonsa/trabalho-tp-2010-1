#ifndef HAS_NBIOCORE_H
#define HAS_NBIOCORE_H

// - Isn't the function pointer syntax pretty?
// - No, it's not.
typedef void (* nbio_callback_t)(int, void*);

void nbio_register(int fd, nbio_callback_t func, void* user_data);
void nbio_unregister(int fd);
void nbio_register_idle(int timeout_seconds, nbio_callback_t func);
void nbio_stop_loop();
void nbio_loop();

#endif
