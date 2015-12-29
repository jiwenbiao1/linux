
#include <menu.h>
#include <curses.h>
#include <stdio.h>
char* colors[13]=
{
	"Black","Charcoal","Light Gray","Brown","Camel",
	"Navy","Light Blue","Hunter Green","Gold",
	"Burgundy","Rust","White",(char* )0
};

ITEM *item[13];
int main()
{

	initscr();
	nonl();
	raw();
	noecho();
	wclear(stdscr);
	
	MENU  *menu;
	ITEM** i = item;

	menu = new_menu();
	post_menu(menu);
	sleep(5);
	unpost_menu(menu);
	refresh();
	free_menu();
	while()
	 free_item();
	 endwin();
	 exit(0);
}
