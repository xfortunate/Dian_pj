#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "level_2.1.h"
#include "level_2.2.h"
#include "level_2.3.h"
#include "level_2.4.h"
#include "level_3.1.h"
#include "level_3.2.h"
int main (int argc,char *argv[])    //接受输入的额外内容
{
    if (argc<2)
    {
        fprintf(stderr,"Please type ./dedit <filename>\n");
        return 1;
    }
    FILE *file=fopen(argv[1],"r");      //打开文件
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
    initscr();      //初始化ncurses
    keypad(stdscr, TRUE);       //启用功能键
    noecho();       //取消输入屏显
    raw();      //取消行缓冲与终端信号拦截
    InitColor();
    int scmax_y;
    int max_x,max_y; 
    getmaxyx(stdscr,scmax_y,max_x);
    max_y=scmax_y-1;
    int x=0,y=0,minr=0,minc=0;      //x,y代表屏幕光标位置，minr代表窗口纵坐标边界，minc代表窗口横坐标边界
    int quit_confirm = 0;    // Ctrl-Q 二次确认
    ScrollSc(buff_p,max_y,max_x,minr,minc,row);
    DrawStatusBar(scmax_y,max_x,y,x,minr,minc,argv[1]);
    move(y,x);
    refresh();
    int ch=getch();
    while (1)
    {
        switch (ch)     //判断键入内容区分行动
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
                getmaxyx(stdscr,scmax_y,max_x);
                max_y=scmax_y-1;
                clear();
                break;
            case CTRL_S:        //保存
                if (Ismodified==1)
                {
                    if (SaveFile(buff_p,row,argv[1]))
                    {
                        Ismodified=0;
                        snprintf(message,sizeof(message)-1,"%s","Saved");
                    }
                    else
                    snprintf(message,sizeof(message)-1,"%s","Failed to save.");
                    DrawStatusBar(scmax_y,max_x,y,x,minr,minc,argv[1]);
                }
                break;
            case CTRL_Q:        //退出
                if (!Ismodified) 
                goto quit;      //未修改或已保存，直接退出
                if (quit_confirm==0) 
                {
                    snprintf(message,sizeof(message)-1,"%s","Unsaved! Ctrl-Q again to quit.");      //下次再按 Ctrl-Q 就退出
                    DrawStatusBar(scmax_y,max_x,y,x,minr,minc,argv[1]);
                    quit_confirm=1;
                } 
                else
                goto quit;
                break;
            case CTRL_F:        //开启搜索模式
            {
                MODE=1;
                DrawStatusBar(scmax_y,max_x,y,x,minr,minc,argv[1]);
                MODE=SetupSearch(scmax_y, max_x);       //输入搜索字串
                if (MODE==1)
                {
                    SearchMatch(buff_p, row);      //构建match_bf匹配索引
                    int ty,tx;
                    if (NextMatch(&ty, &tx))       //跳到第一个匹配
                    {
                        y=ty-minr;
                        x=tx-minc;
                    }
                    else
                    snprintf(message,sizeof(message),"Not found: %s",search_word);
                }
                break;
            case CTRL_R:        //开启替换模式
                MODE=REPLACE;
                DrawStatusBar(scmax_y,max_x,y,x,minr,minc,argv[1]);
                StartReplace(buff_p, &y, &x, &minr, &minc, &row, scmax_y, max_x, argv[1], max_y);
                break;
            }
            case '\n':
            case KEY_ENTER:     //插入换行或切换下一个匹配
            {
                if (MODE==1&&search_word[0]!='\0')    //搜索模式 Enter下一个匹配
                {
                    int ty, tx;     //tx，ty均为实际坐标，需要将其转换为当前窗口坐标
                    if (NextMatch(&ty, &tx))
                    {
                        y=ty-minr;
                        x=tx-minc;
                    }
                    else
                    snprintf(message, sizeof(message),"No more matches");
                    break;
                }
                //非搜索模式 Enter插入换行
                Enter(buff_p,&y,&x,minr,&minc,&row);
                ClearSearch();
                break;
            }
            case CTRL_P:        //搜索模式切换上一个匹配对象
            {
                if (MODE==1)
                {
                    int ty,tx;
                    if (PrevMatch(&ty,&tx))
                    {
                        y=ty-minr;
                        x=tx-minc;
                    }
                    else
                    snprintf(message, sizeof(message),"No previous match");
                }
                break;
            }
            case 27:        //关闭搜索模式
            {
                if (MODE==1)
                {
                    ClearSearch();
                    message[0]='\0';
                }
                break;
            }
            default:
                quit_confirm=0;     //取消退出确认
                if (ch>=32&&ch<=126)    //普通字符
                Insert(buff_p,&y,&x,minr,minc,&row,ch);
                else if (ch==KEY_BACKSPACE)   //退格键
                BackSp(buff_p,&y,&x,minr,minc,&row);
                else if (ch==KEY_DC)    //Delete 键
                Del(buff_p,&y,&x,minr,minc,&row);
                if (Ismodified&&MODE==1)
                ClearSearch();      //清除搜索状态
                break;
        }
        Adjust(buff_p,&y,&x,&minr,&minc,max_y,max_x,row);       //坐标纠偏
        ScrollSc(buff_p,max_y,max_x,minr,minc,row);     //绘制屏幕
        DrawStatusBar(scmax_y,max_x,y,x,minr,minc,argv[1]);     //绘制状态栏
        Refresh_Msg();      //清除临时消息文件夹
        move(y,x);
        refresh();
        ch=getch();
    }  
    quit:       //退出时释放缓存并取消屏幕覆盖
        endwin();
        free(buff);
        free(buff_p);
        return 0;  
}