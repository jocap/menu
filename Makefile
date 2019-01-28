PREFIX = /usr/local

CC ?= cc
CFLAGS += -Wall -Wextra -pedantic -Wno-missing-braces -std=c99
LDFLAGS += -lncurses

menu: menu.c ui.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $>

install: menu
	cp menu ${PREFIX}/bin/menu
	cp menu.1 ${PREFIX}/man/man1/menu.1

uninstall:
	rm ${PREFIX}/bin/menu
	rm ${PREFIX}/man/man1/menu.1
