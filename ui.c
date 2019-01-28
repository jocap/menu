#include <curses.h>
#include <err.h>
#include "ui.h"

struct listing L;

void create_listing(size_t line_count, char *lines[], size_t max_length) {
	int y, x;
	getmaxyx(stdscr, y, x);

	L.line_count = line_count;
	L.lines = lines;
	L.skipped_rows = 0;
	L.bottom_row = y - 2;
	L.rightmost_col = x - 1;
	if ((L.pad = newpad(line_count, max_length + 1)) == NULL) err(1, "newpad");

	for (size_t i = 0; i < line_count; i++) {
		wprintw(L.pad, "%s\n", lines[i]);
		ref();
	}
	move(0, 0);
}

void movedown() {
	int y, x;
	getyx(stdscr, y, x);
	ref();
	if (y + L.skipped_rows < L.line_count - 1) {
		if (y == (int)L.bottom_row) scrolldown();
		else move(y + 1, x);
	}
}

void moveup() {
	int y, x;
	getyx(stdscr, y, x);
	if (y > 0 || (y == 0 && L.skipped_rows > 0)) {
		if (y == 0) scrollup();
		else move(y - 1, x);
	}
}

void scrollup() {
	if (L.skipped_rows > 0)
		L.skipped_rows--;
	ref();
}

void scrolldown() {
	if (L.skipped_rows < (L.line_count - L.bottom_row - 1) && L.line_count > L.bottom_row - 1)
		L.skipped_rows++;
	ref();
}

void resize() {
	int y, x;
	getmaxyx(stdscr, y, x);
	resizeterm(y, x);
	L.bottom_row = y - 2; /* make room for last line */
	L.rightmost_col = x - 1;
	ref();
}

void ref() {
	refresh();
	prefresh(L.pad, L.skipped_rows, 0, 0, 0, L.bottom_row, L.rightmost_col);
}
