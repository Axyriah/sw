# sw

WM-agnostic window swalling tool using xcb.

## Dependencies

-   libxcb
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

## Manual

This is also available as a manual page. You may want to read it through `man sw`.
