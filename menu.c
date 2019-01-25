#include <stdio.h>

struct binding {
	char key[20]; /* fix later */
	char desc[80];
};

int main(int argc, char *argv[]) {
	struct binding bindings[argc - 1];

	if (argc > 1)
		for (int i = 1; i < argc; i++) {
			int n = sscanf(argv[i], "%[^=]=%[^\n]", bindings[i-1].key, bindings[i-1].desc);
			if (n < 2) goto usage;
		}

usage:
	fprintf(stderr, "usage: %s [key=desc]\n", argv[0]);
	return 1;
}
