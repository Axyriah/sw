#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <xcb/xcb.h>

char *merge(int argc, char *argv[]) {
  size_t command_length = 0;
  for (int i = 1; i < argc; i++) {
    int modifier = i == argc - 1 ? 0 : 1;
    command_length += strlen(argv[i]) + modifier;
  }
  char *command = malloc(command_length);
  for (int i = 1; i < argc; i++) {
    strcat(command, argv[i]);
    if (i != argc - 1) {
      strcat(command, " ");
    }
  }
  return command;
}

xcb_window_t hide_current_window(xcb_connection_t **connection) {
  xcb_get_input_focus_cookie_t cookie = xcb_get_input_focus(*connection);
  xcb_get_input_focus_reply_t *reply =
      xcb_get_input_focus_reply(*connection, cookie, NULL);
  xcb_unmap_window(*connection, reply->focus);
  xcb_flush(*connection);
  return reply->focus;
}

void bring_window_back(xcb_connection_t **connection, xcb_window_t window) {
  xcb_map_window(*connection, window);
  xcb_flush(*connection);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("ERROR: No shell command given.\n");
    printf("Usage: %s [command]\n", argv[0]);
    return -1;
  }
  char *command = merge(argc, argv);
  xcb_connection_t *connection = xcb_connect(NULL, NULL);
  xcb_window_t window = hide_current_window(&connection);
  int status = system(command);
  bring_window_back(&connection, window);
  return status;
}
