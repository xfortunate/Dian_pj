#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "level_2.1.h"
#include "level_3.1.h"

char search_word[256]="";       //初始化搜索字串
int match_cnt=0;     //初始化匹配计数
Match *match_bf=NULL;        //初始化匹配索引
int bufcapacity=500;     //初始匹配索引最大容量
int match_p=-1;      //索引指针，-1表示未匹配

int SetupSearch(int scmax_y, int max_x)
{
    int liner=scmax_y-2;       // 输入栏所在行
    int len=0;
    search_word[0] ='\0';         // 每次从零开始输入
    attron(COLOR_PAIR(1));
    mvhline(liner, 0, ' ', max_x);      //绘制搜索框
    mvprintw(liner, 0, "Search: ");
    attroff(COLOR_PAIR(1));
    move(liner, 8);
    refresh();

    int ch;
    while ((ch=getch())!='\n'&&ch!= KEY_ENTER)
    {
        if (ch==27)              // Esc取消搜索
        {
            mvhline(liner, 0, ' ', max_x);     //清除搜索栏
            refresh();
            return 0;
        }
        if (ch==KEY_BACKSPACE)      //退格重新输入
        {
            if (len>0)
            {
                len--;
                search_word[len]='\0';
                attron(COLOR_PAIR(1));
                mvaddch(liner,8+len,' ');       //覆盖退格字符
                attroff(COLOR_PAIR(1));
            }
        }
        else if (ch>=32&&ch<=126)       //输入字符
        {
            if (len<255)
            {
                search_word[len]=(char)ch;
                len++;
                search_word[len]='\0';      //保证及时输入\0避免字符串末尾无\0
                attron(COLOR_PAIR(1));
                mvaddch(liner, 8 + len- 1, ch);     //绘制字符
                attroff(COLOR_PAIR(1));
            }
        }
        move(liner,8+len);
        refresh();
    }
    mvhline(liner, 0, ' ', max_x);      //enter或esc后及时取消搜索框
    refresh();
    return 1;   //1表示成功进入搜索模式
}

void SearchMatch(char **buff_p, int row)        //查找所有匹配字串坐标并传入索引
{
    match_cnt=0;
    match_p=-1;
    if (match_bf==NULL)       //首次分配
    {
        match_bf=(Match *)malloc(bufcapacity*sizeof(Match));
        if (!match_bf) return;
    }

    int wlen=strlen(search_word);       //子串长度
    if (wlen==0) 
    return;        //无输入

    for (int i=0; i<row; i++)       //每行查找
    {
        char *line = buff_p[i];
        char *pos = line;
        while ((pos=strstr(pos, search_word))!=NULL)        //使用strstr查找
        {
            if (match_cnt>=bufcapacity)       //动态数组满则扩容
            {
                bufcapacity*=2;     //容量翻倍
                Match *new_bf=(Match *)realloc(match_bf,bufcapacity*sizeof(Match));
                if (!new_bf) 
                return;     
                match_bf=new_bf;
            }
            match_bf[match_cnt].y=i;        //保存查找到匹配的坐标
            match_bf[match_cnt].x=(int)(pos - line);
            match_cnt++;
            pos+=wlen;
        }
    }
}

int NextMatch(int *out_y, int *out_x)       //传出下一个匹配子串实际y,x坐标
{
    if (match_cnt==0)       //无匹配
    return 0;

    int next=match_p+1;
    if (next>=match_cnt) 
    return 0;      //已到末尾，不绕回

    match_p=next;
    *out_y=match_bf[match_p].y;
    *out_x=match_bf[match_p].x;
    return 1;       //成功查到匹配坐标
}

int PrevMatch(int *out_y, int *out_x)       //传出上一个匹配子串实际y,x坐标
{
    if (match_cnt==0) 
    return 0;

    int prev;
    if (match_p==-1)
    prev=match_cnt-1;       //首次按Ctrl-P跳转至最后一个
    else
    prev=match_p-1;
    if (prev<0) 
    return 0;       //已到开头，不绕回

    match_p=prev;
    *out_y=match_bf[match_p].y;
    *out_x=match_bf[match_p].x;
    return 1;
}

void DrawHighlights(char **buff_p, int max_y, int max_x, int minr, int minc)        //在文本中重新绘制高亮子串
{
    int wlen=strlen(search_word);
    if (wlen==0||match_cnt==0)      //无搜索
    return;

    for (int i=0;i<match_cnt;i++)
    {
        int abs_y=match_bf[i].y;      //匹配的绝对行号
        int abs_x=match_bf[i].x;      //匹配的绝对列号
        //换算成屏幕坐标
        int screen_y=abs_y-minr;
        int screen_x=abs_x-minc;
        if (screen_y<0||screen_y>=max_y) 
        continue;       //不在当前可见行内，跳过
        int draw_x=abs_x;
        int visible_x=screen_x;
        int visible_len=wlen;     
        if (visible_x>=max_x)       //匹配整体在屏幕右侧之外
        continue;
        if (visible_x<0)        //匹配左半部分被横向滚动滚出去了，裁掉左边
        {
            draw_x-=visible_x;
            visible_len+=visible_x;
            visible_x=0;
        }
        if (visible_x+visible_len>max_x)    //匹配延伸到屏幕右侧之外，裁掉右边
        visible_len=max_x-visible_x;
        if (visible_len<=0) 
        continue;
        attron(A_REVERSE);
        mvaddnstr(screen_y,visible_x,buff_p[abs_y]+draw_x,visible_len);     //重绘屏幕内可见高亮子串
        attroff(A_REVERSE);
    }
}

void ClearSearch(void)      //清除匹配状态
{
    match_cnt=0;
    match_p=-1;
    search_word[0]='\0';
    MODE=0;
}