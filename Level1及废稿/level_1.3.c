#include <ncurses.h>
#define CTRL_Q 17 
int main ()
{
    initscr();  //初始化ncurses
    keypad(stdscr, TRUE);   //启用功能键
    noecho();   //取消输入屏显
    raw();   //取消行缓冲与终端信号拦截
    int max_x,max_y;
    getmaxyx(stdscr,max_y,max_x);
    int x=0,y=0;
    mvprintw(y,x,"Use arrow keys to move, type to draw, press any key to start/ctrl-Q to exit");
    refresh();
    int jud=getch();
    if (jud==CTRL_Q)
    {
        endwin();
        return 0;
    }
    clear();
    refresh();
    int ch=getch();
    while (ch!=CTRL_Q)
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