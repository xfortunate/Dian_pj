#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "level_2.1.h"
#include "level_3.1.h"
int MODE=0;
const char *MODENAME[3]={
    "EDIT",
    "SEARCH",
    "REPLACE"
};
int min(int a,int b)
{
    if (a>b)
    return b;
    else 
    return a;
}
void ScrollSc(char **buff_p,int max_y,int max_x,int minr,int minc,int row)      //绘制窗口
{
    for (int i=0;i<max_y;i++)   //每行清除
    {
        move(i,0);
        clrtoeol();
    }
    int limit=min(max_y,row-minr);
    for(int i=0;i<limit;i++)
    {
        int length=strlen(buff_p[i+minr]);
        if(length>minc)
        mvaddnstr(i,0,buff_p[i+minr]+minc,min(length-minc,max_x));      //每行绘制
    }
    if (MODE==1||2)
    DrawHighlights(buff_p,max_y,max_x,minr,minc);   //绘制高亮区
}
void Adjust(char **buff_p,int *y,int *x,int *minr,int *minc,int max_y,int max_x,int row)        //统一进行窗口坐标纠偏
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