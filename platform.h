#include <sys/types.h>
#include <xcb/xcb.h>

int find_parent_pid(pid_t pid);

pid_t find_window_pid(xcb_connection_t *connection, xcb_window_t window);
