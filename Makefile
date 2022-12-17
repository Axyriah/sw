# sw - wm-agnostic swallowing feature, inspired by dwm
# See LICENSE for copyright and license details.

include config.mk

# All needed source files
SRC = sw.c
OBJ = ${SRC:.c=.o}

# All compilation rules
all: options sw
sw: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}
%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
clean:
	rm -f sw ${OBJ} sw.core sw-${VERSION}.tar.gz
dist: clean
	mkdir -p sw-${VERSION}
	cp -R LICENSE Makefile README config.h config.mk ${SRC} sw-${VERSION}
	tar -cf sw-${VERSION}.tar sw-${VERSION}
	gzip sw-${VERSION}.tar
	rm -rf sw-${VERSION}
install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f sw ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/sw 
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < sw.1 > ${DESTDIR}${MANPREFIX}/man1/sw.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/sw.1
uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/sw ${DESTDIR}${MANPREFIX}/man1/sw.1

# Debugging stuff
options:
	@echo sw build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

