PREFIX = /usr/local

CFLAGS += -Wall -Wextra -pedantic -Wno-missing-braces
LDFLAGS += -lncurses

menu: menu.c
	cc $(CFLAGS) $(LDFLAGS) -o $@ $<

install: menu
	cp menu ${PREFIX}/bin/menu
	cp menu.1 ${PREFIX}/man/man1/menu.1

uninstall:
	rm ${PREFIX}/bin/menu
	rm ${PREFIX}/man/man1/menu.1
