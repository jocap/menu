#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <err.h>
#include <locale.h>
#include <curses.h>
#include <sys/ttydefaults.h> /* CTRL */

#include "menu.h"

struct binding {
	char key[20]; /* fix later */
	char desc[80];
};

WINDOW *pad;

size_t count = 0;
char **lines;
size_t skipped_rows = 0; /* how many rows have we scrolled past? */
size_t current_row = 0;
size_t current_col = 0;

size_t pad_bottom_row;
size_t pad_rightmost_col;

int main(int argc, char *argv[]) {
	/* process arguments */
	struct binding bindings[argc - 1];
	if (argc > 1)
		for (int i = 1; i < argc; i++) {
			int n = sscanf(argv[i], "%[^=]=%[^\n]", bindings[i-1].key, bindings[i-1].desc);
			if (n < 2) {
				fprintf(stderr, "usage: %s [key=desc]\n", argv[0]);
				return 1;
			}
		}

	/* read items */
	size_t max_count = 100;
	size_t max_length = 0;
	if ((lines = calloc(max_count, sizeof(char *))) == NULL)
		err(1, "calloc");

	char *line = NULL;
	size_t size = 0;
	ssize_t len;

	while ((len = getline(&line, &size, stdin)) != -1) {
		if (count >= max_count) {
			max_count *= 2;
			if ((lines = reallocarray(lines, count, sizeof(char *))) == NULL)
				err(1, "reallocarray");
		}

		line[strlen(line) - 1] = '\0'; /* remove trailing newline */
		if ((lines[count] = strdup(line)) == NULL) err(1, "strdup");

		if (strlen(line) > max_length) max_length = strlen(line);
		count++;
	}

	free(line);
	if (ferror(stdin)) err(1, "getline");

	/* open tty */
	FILE *tty;
	if ((tty = fopen("/dev/tty", "r+")) == NULL) err(1, "fopen");

	/* setup curses */
	setlocale(LC_ALL, ""); /* set to LC_ALL environment variable */
	newterm(NULL, tty, tty);
	cbreak();
	noecho();
	keypad(stdscr, true); /* special keys */

	pad_bottom_row = LINES - 1;
	pad_rightmost_col = COLS - 1;

	if ((pad = newpad(count, max_length + 1)) == NULL) err(1, "newpad");

	/* print items */

	for (size_t i = 0; i < count; i++) {
		wprintw(pad, "%s\n", lines[i]);
		ref(pad);
	}
	move(0, 0);

	int key;
	while ((key = getch())) {
		switch (key) {
			case 'q':
				goto exit;
				break;
			case 'j':
				movedown();
				break;
			case 'k':
				moveup();
				break;
			case CTRL('e'):
				scrolldown();
				break;
			case CTRL('y'):
				scrollup();
				break;
			case KEY_RESIZE:
				resize();
				break;
			default:
				break;
		}
		refresh();
	}

exit:
	free(lines);
	endwin();
	return 0; /* assume that the kernel frees ttyfd */
}

void movedown() {
	getyx(stdscr, current_row, current_col);
	ref(pad);
	if (current_row + skipped_rows < count - 1) {
		if (current_row == pad_bottom_row) scrolldown();
		else move(current_row + 1, current_col);
	}
}

void moveup() {
	getyx(stdscr, current_row, current_col);
	if (current_row > 0 || (current_row == 0 && skipped_rows > 0)) {
		if (current_row == 0) scrollup();
		else move(current_row - 1, current_col);
	}
}

void scrollup() {
	if (skipped_rows > 0)
		skipped_rows--;
	ref(pad);
}

void scrolldown() {
	if (skipped_rows < (count - pad_bottom_row - 1) && count > pad_bottom_row - 1)
		skipped_rows++;
	ref(pad);
}

void resize() {
	int y, x;
	getmaxyx(stdscr, y, x);
	resizeterm(y, x);
	pad_bottom_row = y - 1;
	pad_rightmost_col = x - 1;
	ref(pad);
}

void ref(WINDOW *pad) {
	refresh();
	prefresh(pad, skipped_rows, 0, 0, 0, pad_bottom_row, pad_rightmost_col);
}
