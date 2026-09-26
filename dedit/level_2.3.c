#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "level_2.1.h"
#include "level_2.3.h"
int Ismodified=0;
char message[500]="";
void InitColor(void)
{
    if (has_colors())   //颜色初始化
    {
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);     //定义颜色对1为白底黑字
    }
}

void Refresh_Msg()      //清空message
{
    message[0]='\0';
}

void DrawStatusBar(int scmax_y,int max_x,int y,int x,int minr,int minc,const char *filename)        //绘制状态栏
{
    attron(COLOR_PAIR(1));  //开启颜色对
    mvhline(scmax_y - 1, 0, ' ', max_x);
    char bar[500]="";       //定义状态栏需要绘制的字符
    snprintf(bar,sizeof(bar),"%s",filename);    //文件名
    if(Ismodified==1)
    strncat(bar,"|Modified",sizeof(bar)-strlen(bar)-1);     //是否修改
    char pos[60];
    snprintf(pos,sizeof(pos)-1,"|row:%d col:%d|MODE: %s",y+minr+1,x+minc+1,MODENAME[MODE]);     //光标行号，列号，当前模式
    strncat(bar,pos,sizeof(bar)-strlen(bar));   
    mvaddnstr(scmax_y-1,0,bar,min(strlen(bar),max_x-1));
    if(message[0]!='\0')
    {
        int msg_len=strlen(message);
        int msg_pos=max_x-msg_len-1;
        if(msg_pos<strlen(bar)+2)
        msg_pos=strlen(bar)+2;
        if(msg_pos<max_x)
        mvaddnstr(scmax_y-1,msg_pos,message,min(msg_len,max_x-msg_pos));        //从右至左绘制临时消息
    }
    attroff(COLOR_PAIR(1));    //关闭颜色对
}