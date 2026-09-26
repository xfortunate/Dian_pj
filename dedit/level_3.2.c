#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "level_2.1.h"
#include "level_2.3.h"
#include "level_3.1.h"
#include "level_3.2.h"

char replace_word[256] = "";

static int SetupReplaceInput(int scmax_y, int max_x)
{
    int liner=scmax_y - 2;
    int len=0;
    replace_word[0] = '\0';

    attron(COLOR_PAIR(1));
    mvhline(liner, 0, ' ', max_x);
    mvprintw(liner, 0, "Replace: ");
    attroff(COLOR_PAIR(1));
    move(liner, 9);
    refresh();
    int ch;
    while ((ch=getch())!='\n'&&ch!=KEY_ENTER)
    {
        if (ch==27) // Esc 取消
        {
            mvhline(liner, 0, ' ', max_x);
            refresh();
            return 0;
        }
        if (ch==KEY_BACKSPACE||ch==127||ch==8)
        {
            if (len>0)
            {
                len--;
                replace_word[len]='\0';
                attron(COLOR_PAIR(1));
                mvaddch(liner, 9+len, ' ');
                attroff(COLOR_PAIR(1));
            }
        }
        else if (ch>=32&&ch<=126)
        {
            if (len<255)
            {
                replace_word[len]=(char)ch;
                len++;
                replace_word[len]='\0';
                attron(COLOR_PAIR(1));
                mvaddch(liner, 9+len-1, ch);
                attroff(COLOR_PAIR(1));
            }
        }
        move(liner, 9+len);
        refresh();
    }
    mvhline(liner, 0, ' ', max_x);
    refresh();
    return 1;
}

int SetupReplace(int scmax_y, int max_x)
{
    if (!SetupSearch(scmax_y, max_x))       
    return 0;   //读搜索词
    if (!SetupReplaceInput(scmax_y, max_x)) 
    return 0;   //读替换词
    
    return 1;
}

static void RescanFrom(char **buff_p, int row, int from_y, int from_x)
{
    if (match_bf==NULL)
    {
        match_bf=(Match *)malloc(bufcapacity * sizeof(Match));
        if (!match_bf) 
        return;
    }
    match_cnt=0;
    match_p=-1;
    int wlen=strlen(search_word);
    if (wlen==0) 
    return;

    for (int i=from_y;i<row;i++)
    {
        char *line=buff_p[i];
        int line_len=(int)strlen(line);

        // 起始行从 from_x 开始；后续行从 0 开始
        if (i==from_y && from_x > line_len)
        continue;
        char *pos=(i==from_y)?line+from_x:line;
        while ((pos = strstr(pos, search_word))!=NULL)
        {
            if (match_cnt >= bufcapacity)
            {
                bufcapacity *= 2;
                Match *new_bf=(Match *)realloc(match_bf,bufcapacity * sizeof(Match));
                if (!new_bf) 
                return;
                match_bf=new_bf;
            }
            match_bf[match_cnt].y=i;
            match_bf[match_cnt].x=(int)(pos - line);
            match_cnt++;
            pos+=wlen;
        }
    }
}

int ReplaceCurrent(char **buff_p, int row)
{
    if (match_cnt==0||match_p<0||match_p>=match_cnt)        //没有需要替换的子串
    return 0;

    int my=match_bf[match_p].y;
    int mx=match_bf[match_p].x;
    int wlen=strlen(search_word);
    int rlen=strlen(replace_word);
    int line_len=strlen(buff_p[my]);

    if (line_len-wlen+rlen>=MAXFILE_X)        //溢出保护
    return 0;
    memmove(buff_p[my]+mx+rlen,buff_p[my]+mx+wlen,line_len-mx-wlen+1);   //把 mx 之后的内容整体移动（+rlen - wlen 位置） +1连带'\0'移动
    memcpy(buff_p[my] + mx, replace_word, rlen);        //写入替换词
    Ismodified=1;
    RescanFrom(buff_p, row, my, mx + rlen);     //从替换位置之后重扫匹配
    return (match_cnt>0)?1:0;
}

int ReplaceAllMatches(char **buff_p, int row)
{
    if (match_cnt==0) return 0;

    int wlen=strlen(search_word);
    int rlen=strlen(replace_word);
    int total=0;

    for (int i=match_cnt-1; i>=0;i--)        //从后往前遍历
    {
        int my=match_bf[i].y;
        int mx=match_bf[i].x;
        int line_len=strlen(buff_p[my]);
        if (line_len-wlen+rlen>=MAXFILE_X) 
        continue;
        memmove(buff_p[my]+mx+rlen,buff_p[my]+mx+wlen,line_len-mx-wlen+1);
        memcpy(buff_p[my]+mx,replace_word,rlen);
        total++;
    }

    if (total>0) 
    Ismodified=1;
    match_cnt=0;      //全部替换后匹配索引失效，清空
    match_p=-1;
    return total;
}

void ClearReplace(void)
{
    replace_word[0]='\0';
    MODE=0;
}

void StartReplace(char **buff_p, int *y, int *x, int *minr, int *minc, int *row, int scmax_y, int max_x, const char *filename, int max_y)
{
    if (!SetupSearch(scmax_y, max_x))       //获取搜索词汇
    {
        MODE = EDIT;
        return;
    }
    if (!SetupReplaceInput(scmax_y, max_x))         //获取替换文本
    {
        MODE = EDIT;
        return;
    }
    SearchMatch(buff_p, *row);      //查找所有匹配项
    if (match_cnt==0) 
    {
        snprintf(message, sizeof(message), "No matches found");
        MODE=EDIT;
        return;
    }
    MODE=REPLACE;       //进入替换状态
    int ty, tx;
    if (NextMatch(&ty, &tx))        //初始跳转到第一个匹配
    {
        *y=ty-*minr;
        *x=tx-*minc;
    }

    while (1) 
    {
        //每次循环前修正坐标、绘制屏幕和状态栏
        Adjust(buff_p, y, x, minr, minc, max_y, max_x, *row);
        ScrollSc(buff_p, max_y, max_x, *minr, *minc, *row);
        DrawStatusBar(scmax_y, max_x, *y, *x, *minr, *minc, filename);
        Refresh_Msg();
        move(*y, *x);
        refresh();

        int ch=getch();
        if (ch==27||ch==3)     //按Esc或Ctrl-C 退出
        break;
        if (ch=='y'||ch=='Y')     //按 Y 替换并移动至下一个候选
        {
            if (ReplaceCurrent(buff_p, *row)) 
            {
                if (match_cnt>0) 
                {
                    if (NextMatch(&ty, &tx))    //替换成功后重新扫描
                    {
                        *y=ty-*minr;
                        *x=tx-*minc;
                    }
                } else {
                    snprintf(message, sizeof(message), "Replaced. No more matches");
                    break;
                }
            }
        }
        else if (ch=='n'||ch=='N' ||ch=='\n'||ch==KEY_ENTER)        //按 N 移动至下一个候选（不替换）
        {
            if (!NextMatch(&ty, &tx)) 
            {
                snprintf(message, sizeof(message), "No more matches");
            } else {
                *y=ty-*minr;
                *x=tx-*minc;
            }
        }
        else if (ch==CTRL_P)        //按 Ctrl-P 切换至上一个候选
        {
            if (!PrevMatch(&ty, &tx)) 
            snprintf(message, sizeof(message), "No previous match");
            else 
            {
                *y=ty-*minr;
                *x=tx-*minc;
            }
        }
        else if (ch=='a'||ch=='A')          //按 A 替换全部
        {
            int count=ReplaceAllMatches(buff_p, *row);
            if (count>0) 
            snprintf(message, sizeof(message), "Replaced all (%d)", count);
            else 
            snprintf(message, sizeof(message), "No matches to replace");
            break;
        }
    }
    ClearSearch();      
    ClearReplace();     //退出替换模式，清理状态
    MODE = EDIT;
}