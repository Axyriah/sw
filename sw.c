#include "config.h"
#include "platform.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

typedef struct Client Client;

struct Client {
  pid_t pid;
  xcb_window_t window;
  bool is_swallowing;
  Client *parent;
  Client *next;
};

char *get_window_class(xcb_connection_t *connection, xcb_window_t window) {
  xcb_get_property_cookie_t cookie = xcb_get_property(
      connection, 0, window, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 0, 1024);
  xcb_get_property_reply_t *reply =
      xcb_get_property_reply(connection, cookie, NULL);
  if (reply == NULL) {
    printf("[ERROR] Failed to find reply for window class.\n");
    return NULL;
  }
  char *class = (char *)xcb_get_property_value(reply);
  free(reply);
  return class;
}

Client *find_client_by_pid(Client *client, pid_t pid) {
  for (Client *c = client; c; c = c->next) {
    if (c->pid == pid - 1) {
      return c;
    }
  }
  return NULL;
}

void swallow(Client *client, xcb_connection_t *connection) {
  Client *pr = client;
  for (unsigned long i = 0; i < sizeof(classes) / sizeof(classes[0]); i++) {
    pid_t parent_pid = client->pid;
    while ((parent_pid = find_parent_pid(parent_pid)) != 0) {
      if ((pr = find_client_by_pid(client, parent_pid)) != NULL &&
          strcmp(get_window_class(connection, pr->window), classes[i]) == 0) {
        pr->is_swallowing = true;
        client->parent = pr;
        xcb_unmap_window(connection, pr->window);
        xcb_flush(connection);
        break;
      }
    }
  }
}

void unswallow(Client *client, xcb_connection_t *connection) {
  if (!client) {
    return;
  }
  Client *pr = client->parent;
  if (pr == NULL) {
    pid_t parent = find_parent_pid(client->pid);
    if (parent == 0) {
      return;
    }
    pr = find_client_by_pid(client, parent);
  }
  if (pr != NULL && pr->is_swallowing) {
    pr->is_swallowing = false;
    xcb_map_window(connection, pr->window);
    xcb_flush(connection);
  }
}

Client *on_x_map(Client *client, xcb_connection_t *connection,
                 xcb_generic_event_t *ev) {
  xcb_map_notify_event_t *event = (xcb_map_notify_event_t *)ev;
  xcb_window_t window = event->window;
  pid_t pid = find_window_pid(connection, window);
  Client *c = malloc(sizeof(Client));
  c->window = window;
  c->is_swallowing = false;
  c->pid = pid;
  if (client) {
    c->next = client;
  }
  swallow(c, connection);
  return c;
}

Client *on_x_destroy(Client *client, xcb_connection_t *connection,
                     xcb_generic_event_t *ev) {
  xcb_destroy_notify_event_t *event = (xcb_destroy_notify_event_t *)ev;
  xcb_window_t window = event->window;
  xcb_get_window_attributes_cookie_t cookie =
      xcb_get_window_attributes(connection, window);
  xcb_get_window_attributes_reply_t *reply =
      xcb_get_window_attributes_reply(connection, cookie, NULL);
  // if the reply is NULL, it means the window is really destroyed
  // this event is also triggered for some reason even when the window is
  // active
  if (reply == NULL && client) {
    unswallow(client, connection);
    Client *c = client->next;
    free(client);
    return c;
  }
  return client;
}

int main(void) {
  xcb_connection_t *connection = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(connection)) {
    printf("ERROR: Failed to connect to X server.\n");
    return -1;
  }
  xcb_screen_t *screen =
      xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
  uint32_t values[] = {XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY};
  xcb_change_window_attributes(connection, screen->root, XCB_CW_EVENT_MASK,
                               values);
  xcb_flush(connection);
  xcb_generic_event_t *ev;
  Client *client = NULL;
  while ((ev = xcb_wait_for_event(connection))) {
    switch (ev->response_type & ~0x80) {
    case XCB_MAP_NOTIFY: {
      client = on_x_map(client, connection, ev);
      break;
    }
    case XCB_DESTROY_NOTIFY: {
      client = on_x_destroy(client, connection, ev);
      break;
    }
    default: {
      break;
    }
    }
    free(ev);
  }
  return 0;
}
