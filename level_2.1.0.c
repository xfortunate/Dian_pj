#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CTRL_Q 17 
#define MAXFILE_Y 200
#define MAXFILE_X 500
int ShowSc(char *buff,int length,int max_y,int max_x);
void ScrollSc(char *buff,int max_y,int max_x,int file_maxy);
int main (int argc,char *argv[])    //接受输入的额外内容
{
    if (argc<2)
    {
        fprintf(stderr,"Please type ./dedit <filename>\n");
        return 1;
    }
    FILE *file=fopen(argv[1],"r+");
    if (!file)
    {
        fprintf(stderr,"Not found.\n");
        return 2;
    }
    if (fseek(file,0,SEEK_END))
    {
        fprintf(stderr,"Failed to fseek\n");
        fclose(file);
        return 3;
    }
    long length=ftell(file);
    rewind(file);
    char *buff=(char *)malloc(sizeof(char)*length+1);
    fread(buff,sizeof(char),length,file);
    buff[length]='\0';
    fclose(file);
    initscr();  //初始化ncurses
    keypad(stdscr, TRUE);   //启用功能键
    noecho();   //取消输入屏显
    raw();   //取消行缓冲与终端信号拦截
    int max_x,max_y;
    getmaxyx(stdscr,max_y,max_x);
    int x=0,y=0;
    int file_maxy=ShowSc(buff,length,max_y,max_x);
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
                else
                ScrollSc(buff,max_y,max_x,file_maxy);
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
int ShowSc(char *buff,int length,int max_y,int max_x)
{
    int buff_p=0,cont=0,file_maxy=1,x=0,y=0;
    while (buff_p<=length)
    {
        if (cont==max_x)
        {
            x=0;
            y++;
            cont=0;
            file_maxy++;
            continue;
        }
        char getc=buff[buff_p];
        if(getc=='\n')
        {
            if(cont!=0)
            {
                x=0;
                y++;
                cont=0;
                file_maxy++;
            }
        }
        else{
            mvprintw(y,x,"%c",getc);
            x++;
            cont++;
        }
    }
    refresh();
    return file_maxy;
}
void ScrollSc(char *buff,int max_y,int max_x,int file_maxy)
{

}