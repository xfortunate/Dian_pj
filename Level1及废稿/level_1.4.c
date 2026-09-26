#include <ncurses.h>
#include <stdio.h>
#define CTRL_Q 17 
int main (int argc,char *argv[])    //接受输入的额外内容
{
    if (argc<2)
    {
        printf("please type ./dedit <filename>\n");
        return 1;
    }
    FILE *file=fopen(argv[1],"r");
    if (!file)
    {
        printf("not found.\n");
        return 2;
    }
    initscr();  //初始化ncurses
    keypad(stdscr, TRUE);   //启用功能键
    noecho();   //取消输入屏显
    raw();   //取消行缓冲与终端信号拦截
    int max_x,max_y;
    getmaxyx(stdscr,max_y,max_x);
    int x=0,y=0;
    int getc=fgetc(file);
    while (getc!=EOF)
    {
        if(getc=='\n')
        {
            x=0;
            y++;
        }
        else if(getc=='\t') 
        {        
            int spaces=8-(x%8);   // 制表符：补空格直到下一个 8 的倍数位置
            for (int i=0;i<spaces;i++) 
            {
                mvaddch(y,x,' ');
                x++;
            }
        } 
        else{
            mvprintw(y,x,"%c",getc);
            x++;
        }
        getc=fgetc(file);
    }
    fclose(file);
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