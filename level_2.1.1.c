#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CTRL_Q 17 
#define MAXFILE_Y 500
#define MAXFILE_X 500
int min(int a,int b)
{
    if (a>b)
    return b;
    else 
    return a;
}
void ScrollSc(char **buff_p, int max_y, int max_x, int minr, int minc, int row);    //刷新屏幕以适应窗口
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
                if (y>0)
                y--;
                else if(y==0&&minr>0)
                {
                    minr--;
                    maxr--;
                    ScrollSc(buff_p,max_y,max_x,minr,minc,row);
                }
                break;
            case KEY_DOWN:
                if (y<max_y-1&&(minr+y+1)<row)
                y++;
                else if(y==max_y-1&&maxr<row-1)
                {
                    minr++;
                    maxr++;
                    ScrollSc(buff_p,max_y,max_x,minr,minc,row);
                } 
                break;
            case KEY_LEFT:
                if (x>0)
                x--;
                else if(x==0&&minc>0)
                {
                    minc--;
                    maxc--;
                    ScrollSc(buff_p,max_y,max_x,minr,minc,row);
                }
                break;
            case KEY_RIGHT:
                if (x<max_x-1&&(minc+x)<strlen(buff_p[minr+y]))
                x++;
                else if(x==max_x-1&&maxc+1<strlen(buff_p[minr+y]))
                {
                    minc++;
                    maxc++;
                    ScrollSc(buff_p,max_y,max_x,minr,minc,row);
                }
                break;
            case KEY_RESIZE:        //缩放屏幕后进行修正
                getmaxyx(stdscr,max_y,max_x);   
                maxr=minr+max_y-1;
                maxc=minc+max_x-1;
                ScrollSc(buff_p,max_y,max_x,minr,minc,row);
                if(y>=max_y)
                y=max_y-1;
                if(x>=max_x)
                x=max_x-1;
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
        move(y,x);
        refresh();
        ch=getch();
    }  
    endwin();
    free(buff);
    free(buff_p);
    return 0;  
}
void ScrollSc(char **buff_p,int max_y,int max_x,int minr,int minc,int row)
{
    clear();
    int limit=min(max_y,row-minr);
    for(int i=0;i<limit;i++)
    {
        int length=strlen(buff_p[i+minr]);
        if(length>minc)
        mvaddnstr(i,0,buff_p[i+minr]+minc,min(length-minc,max_x));
    }
    refresh();
}
