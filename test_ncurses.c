#include <ncurses.h>

int main() {
    initscr();
    printw("Hello ncurses! Press any key to exit.");
    refresh();
    getch();
    endwin();
    return 0;
}

