# software version
VERSION = 1.0

# installation prefix
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# path to libraries - you may need to remove dots (.) from the path

# FreeBSD and OpenBSD
INCS = -I/usr/local/include

# FreeBSD
LIBS = -L/usr/local/lib -lxcb -lutil

# OpenBSD (uncomment)
# LIBS = -L/usr/local/lib -lxcb -lkvm

# NetBSD (uncomment)
# INCS = -I/usr/pkg/include
# LIBS = -L/usr/pkg/lib

# Linux (uncomment)
# INCS = -I/usr/X11R6/include
# LIBS = -L/usr/X11R6/lib -lxcb -lxcb-res

# end

# compiler flags
CFLAGS = -Wall -Wextra -Wno-deprecated-declarations -g -pedantic -Os ${INCS} -DVERSION=\"${VERSION}\"
LDFLAGS = ${LIBS}

# compiler and linker
CC = cc
