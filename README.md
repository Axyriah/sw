# sw

WM-agnostic window swalling tool using xcb.

## Dependencies

-   libxcb
-   libxcb-res (only if you are running Linux)
-   make
-   cc or any other compiler/linker

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

_Untested on Linux and OpenBSD. If you are using dwm on either of these, it may better to
use the swallow patch._

## Manual

This is also available as a manual page. You may want to read it through `man sw`.
