# sw

Automatic window hiding daemon/supervisor for X11. It works by listening to X11 remap
and destroy events and comparing the PID of parent windows to decide if a window should
be hidden or not until the child window is destroyed.

## Dependencies

-   libxcb
-   make, cc or any other compiler/linker

**You also need libxcb-res on Linux**

## Installation

On Linux, OpenBSD, and NetBSD, you need to uncomment two OS-specific lines in config.mk
before compiling this.

FreeBSD does not need any changes.

```sh
$ git clone https://code.axyria.dev/sw.git && cd sw
$ doas make clean install
```

Since this is a background process, you should add it to your startup script or any other
supervisor method. By default, only applications with the `st-256color` class name are
going to be swallowed. You can change this by editing the `config.h` file.

**Untested on Linux and OpenBSD. Please report any issues, if any are found.**

## Manual

This is also available as a manual page. You may want to read it through `man sw`.
