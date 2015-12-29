#include <curses.h>
#include <menu.h>
#include <stdio.h>
void initial()
{
	initscr();
	cbreak();
	nonl();
	noecho();
	intrflush(stdscr,false);
	keypad(stdscr,true);
	refresh();
}
int main()
{
	initscr();
	clear();
	move(10,20);
	addstr("hello word");

	move(LINES-1,0);
	refresh();
	getch();
	endwin();
	return 0;
}
