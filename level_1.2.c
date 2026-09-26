#include <ncurses.h>
int main ()
{
    initscr();  //初始化ncurses
    keypad(stdscr, TRUE);   //启用功能键
    noecho();   //取消输入屏显
    cbreak();   //取消行缓冲
    int max_x,max_y;
    getmaxyx(stdscr,max_y,max_x);
    int x=0,y=0;
    mvprintw(y,x,"Use arrow keys to move, type to draw, press any key to start/esc to exit");
    refresh();
    int jud=getch();
    if (jud==27)
    {
        endwin();
        return 0;
    }
    clear();
    refresh();
    int ch=getch();
    while (ch!=27)
    {
        switch (ch)             //判断键入内容区分行动
        {
            case KEY_UP:
                if (y>0)
                y--;
                break;
            case KEY_DOWN:
                if (y<max_y-1)
                y++;
                break;
            case KEY_LEFT:
                if (x>0)
                x--;
                break;
            case KEY_RIGHT:
                if (x<max_x-1)
                x++;
                break;
            default:
                if (ch>=32&&ch<=126)
                {
                    if(x==max_x-1&&y==max_y-1)
                    break;
                    printw("%c",ch);
                    getyx(stdscr,y,x); 
                }
                break;
        }
        move(y,x);
        refresh();
        ch=getch();
    }  
    endwin();
    return 0;  
}