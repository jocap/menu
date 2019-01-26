#include <stdio.h>

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
			if (n < 2) goto usage;
		}

	/* read items */
	size_t max_count = 100;
	char **lines;
	if ((lines = calloc(max_count, sizeof(char *))) == NULL)
		err(1, "calloc");

	char *line = NULL;
	size_t size = 0;
	ssize_t len;
	size_t count = 0;

	while ((len = getline(&line, &size, stdin)) != -1) {
		if (count >= max_count) {
			max_count *= 2;
			if ((lines = reallocarray(lines, count, sizeof(char *))) == NULL)
				err(1, "reallocarray");
		}
		line[strlen(line) - 1] = '\0'; /* remove trailing newline */
		lines[count] = strdup(line);
		count++;
	}
	free(line);
	if (ferror(stdin)) err(1, "getline");
usage:
	fprintf(stderr, "usage: %s [key=desc]\n", argv[0]);
	return 1;
}
