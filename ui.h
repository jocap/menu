#pragma once

struct listing {
	WINDOW *pad;
	char **lines;
	size_t line_count;
	size_t skipped_rows; /* how many rows have we scrolled past? */
	size_t bottom_row;
	size_t rightmost_col;
} listing;

struct listing L;

void create_listing();
void movedown();
void moveup();
void scrollup();
void scrolldown();
void resize();
void ref();
