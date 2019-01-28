#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <err.h>
#include <locale.h>
#include <curses.h>
#include <sys/ttydefaults.h> /* CTRL */
#include "ui.h"

struct binding {
	char key[20]; /* fix later */
	char desc[80];
};

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
	size_t line_count = 0;
	char **lines;
	size_t max_count = 100;
	size_t max_length = 0;
	if ((lines = calloc(max_count, sizeof(char *))) == NULL)
		err(1, "calloc");

	char *line = NULL;
	size_t size = 0;
	ssize_t len;

	while ((len = getline(&line, &size, stdin)) != -1) {
		if (line_count >= max_count) {
			max_count *= 2;
			if ((lines = reallocarray(lines, line_count, sizeof(char *))) == NULL)
				err(1, "reallocarray");
		}

		line[strlen(line) - 1] = '\0'; /* remove trailing newline */
		if ((lines[line_count] = strdup(line)) == NULL) err(1, "strdup");

		if (strlen(line) > max_length) max_length = strlen(line);
		line_count++;
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

	/* print info and items */
	create_listing(line_count, lines, max_length); /* sets L */

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
