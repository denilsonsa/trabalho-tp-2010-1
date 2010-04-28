// - Isn't the function pointer syntax pretty?
// - No, it's not.
typedef void (* nbio_callback_t)(int);

void nbio_register(int fd, nbio_callback_t func);
void nbio_unregister(int fd);
void nbio_set_timeout(int seconds);
void nbio_register_idle(nbio_callback_t func);
void nbio_stop_loop();
void nbio_loop();
