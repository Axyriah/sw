#include <stdlib.h>
#include <sys/types.h>
#include <xcb/xcb.h>

#ifdef __FreeBSD__
#include <libutil.h>
#include <sys/user.h>
#endif

#ifdef __OpenBSD__
#include <kvm.h>
#include <sys/sysctl.h>
#endif

#if defined(__FreeBSD__) || defined(__OpenBSD__)
#include <stdio.h>
#include <string.h>
#endif

#ifdef __linux__
#include <stdio.h>
#include <sys/sysinfo.h>
#include <xcb/res.h>
#endif

// credits: dwm-swallow
int find_parent_pid(pid_t pid) {
  unsigned int value = 0;
#if defined(__FreeBSD__)
  struct kinfo_proc *proc = kinfo_getproc(pid);
  if (proc) {
    value = proc->ki_ppid;
    free(proc);
  }
#elif defined(__linux__)
  FILE *f;
  char buf[256];
  snprintf(buf, sizeof(buf) - 1, "/proc/%u/stat", (unsigned)pid);
  if ((f = fopen(buf, "r")) && fscanf(f, "%*u %*s %*c %u", &value) == 1) {
    fclose(f);
  }
#elif defined(__OpenBSD__)
  int n;
  kvm_t *kd;
  struct kinfo_proc *kp;
  kd = kvm_openfiles(NULL, NULL, NULL, KVM_NO_FILES, NULL);
  if (kd) {
    kp = kvm_getprocs(kd, KERN_PROC_PID, p, sizeof(*kp), &n);
    value = kp->p_ppid;
  }
#endif
  return (pid_t)value;
}

// linux code credits: dwm-swallow
pid_t find_window_pid(xcb_connection_t *connection, xcb_window_t window) {
  pid_t pid = 0;
#if defined(__FreeBSD__) || defined(__OpenBSD__)
  char *name = "_NET_WM_PID";
  xcb_intern_atom_cookie_t atom_cookie =
      xcb_intern_atom(connection, 0, strlen(name), name);
  xcb_intern_atom_reply_t *atom =
      xcb_intern_atom_reply(connection, atom_cookie, NULL);
  if (atom == NULL) {
    printf("[ERROR] Failed to find reply for atom %s.\n", name);
    return 0;
  }
  xcb_get_property_cookie_t cookie = xcb_get_property(
      connection, 0, window, atom->atom, XCB_ATOM_CARDINAL, 0, 1024);
  xcb_get_property_reply_t *reply =
      xcb_get_property_reply(connection, cookie, NULL);
  if (reply == NULL) {
    printf("[ERROR] Failed to find reply for window pid.\n");
    return 0;
  }
  pid = *(pid_t *)xcb_get_property_value(reply);
  free(reply);
#elif defined(__linux__)
  xcb_res_client_id_spec_t spec = {0};
  spec.client = window;
  spec.mask = XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID;
  xcb_generic_error_t *error = NULL;
  xcb_res_query_client_ids_cookie_t l_cookie =
      xcb_res_query_client_ids(connection, 1, &spec);
  xcb_res_query_client_ids_reply_t *l_reply =
      xcb_res_query_client_ids_reply(connection, c, &error);
  if (!l_reply) {
    printf("[ERROR] Failed to find reply for window pid.\n");
    free(error);
    return pid;
  }
  xcb_res_client_id_value_iterator_t iter =
      xcb_res_query_client_ids_ids_iterator(l_reply);
  for (; iter.rem; xcb_res_client_id_value_next(&iter)) {
    if (iter.data->spec.mask & XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID) {
      pid = *xcb_res_client_id_value_value(iter.data);
      break;
    }
  }
  free(l_reply);
  free(error);
#endif
  return pid;
}
