#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CTRL_Q 17 
#define MAXFILE_Y 500
#define MAXFILE_X 500
int min(int a,int b);
void ScrollSc(char **buff_p, int max_y, int max_x, int minr, int minc, int row);    //绘制窗口
void Adjust(char **buff_p,int *y, int *x, int *minr, int *minc, int max_y, int max_x, int row);  //统一进行窗口坐标纠偏
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
    char *buff=(char *)malloc(sizeof(char)*MAXFILE_Y*MAXFILE_X);
    char **buff_p=(char **)malloc(sizeof(char *)*MAXFILE_Y);
    if (buff==NULL||buff_p==NULL) 
    {
        fprintf(stderr,"Failed to locate memory");
        return 3;
    }
    for (int i=0;i<MAXFILE_Y;i++)
    buff_p[i]=buff+i*MAXFILE_X;
    int row=0;
    while(row<MAXFILE_Y&&fgets(buff_p[row],MAXFILE_X,file))     //读取文件内容
    {
        buff_p[row][strcspn(buff_p[row],"\n")]='\0';
        row++;
    }
    fclose(file);
    initscr();  //初始化ncurses
    keypad(stdscr, TRUE);   //启用功能键
    noecho();   //取消输入屏显
    raw();   //取消行缓冲与终端信号拦截
    int max_x,max_y;
    getmaxyx(stdscr,max_y,max_x);
    int x=0,y=0,minr=0,maxr=max_y-1,minc=0,maxc=max_x-1;    //x,y代表屏幕光标位置，minr，maxr代表窗口纵坐标边界，minc，maxc代表窗口横坐标边界
    ScrollSc(buff_p,max_y,max_x,minr,minc,row);
    move(y,x);
    refresh();
    int ch=getch();
    while (ch!=CTRL_Q)
    {
        switch (ch)             //判断键入内容区分行动
        {
            case KEY_UP:
                if (y+minr>0)
                y--;
                break;
            case KEY_DOWN:
                if (y+minr<row-1)
                y++;
                break;
            case KEY_LEFT:
                if (x+minc>0)
                x--;
                break;
            case KEY_RIGHT:
                if (x+minc<strlen(buff_p[minr+y]))
                x++;
                break;
            case KEY_RESIZE:        //缩放屏幕后修正坐标参数
                getmaxyx(stdscr,max_y,max_x);
                clear();
                break;
            default:
                if (ch>=32&&ch<=126)
                {
                    if(x==max_x-1&&y==max_y-1)
                    break;
                    mvaddch(y, x, ch);
                    x++; 
                }
                break;
        }
        Adjust(buff_p,&y,&x,&minr,&minc,max_y,max_x,row);
        ScrollSc(buff_p,max_y,max_x,minr,minc,row);
        move(y,x);
        refresh();
        ch=getch();
    }  
    endwin();
    free(buff);
    free(buff_p);
    return 0;  
}

int min(int a,int b)
{
    if (a>b)
    return b;
    else 
    return a;
}
void ScrollSc(char **buff_p,int max_y,int max_x,int minr,int minc,int row)
{
    erase();
    int limit=min(max_y,row-minr);
    for(int i=0;i<limit;i++)
    {
        int length=strlen(buff_p[i+minr]);
        if(length>minc)
        mvaddnstr(i,0,buff_p[i+minr]+minc,min(length-minc,max_x));
    }
    refresh();
}
void Adjust(char **buff_p,int *y, int *x, int *minr, int *minc, int max_y, int max_x, int row)
{
    if (*y<0)
    {
        *minr+=*y;
        *y=0;
    }
    else if(*y>=max_y)
    {
        *minr+=*y-max_y+1;
        *y=max_y-1;
    }
    if (*x<0)
    {
        *minc+=*x;
        *x=0;
    }
    else if(*x>=max_x)
    {
        *minc+=*x-max_x+1;
        *x=max_x-1;
    }
    int cur=*minr+*y;
    if(cur>=0&&cur<row)
    {
        int length=strlen(buff_p[cur]);
        if (*x+*minc>length)
        {
            if(length>=*minc)
            *x=length-*minc;
            else
            {
                *x=0;
                *minc=length;
            }
        }
    }
}