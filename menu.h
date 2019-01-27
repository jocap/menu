#pragma once

#include <curses.h>

int main(int, char **);
void movedown();
void moveup();
void scrolldown();
void scrollup();
void resize();
void ref(WINDOW *);
