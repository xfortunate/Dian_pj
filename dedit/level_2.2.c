#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "level_2.1.h"
#include "level_2.2.h"
#include "level_2.3.h"
void Insert(char **buff_p,int *y,int *x,int minr,int minc,int *row,char data)       //插入字符
{
    int length=strlen(buff_p[minr+*y]);
    int curln=*y+minr;
    if (length>=MAXFILE_X-1)        //防止溢出
    return;
    if (*row==0) 
    *row=1;
    if(minc+*x==length)
    {
        buff_p[curln][length]=data;
        buff_p[minr+*y][length+1]='\0';
    }
    else{
        char *temp=(char*)malloc(strlen(buff_p[curln]+minc+*x)+1);
        strcpy(temp,buff_p[curln]+minc+*x);     //光标后字符统一偏移
        buff_p[minr+*y][*x+minc]=data;
        strcpy(buff_p[curln]+minc+*x+1,temp);
        free (temp);
    }
    (*x)++;
    Ismodified=1;
}
void BackSp(char **buff_p,int *y,int *x,int minr,int minc,int *row)     //退格
{
    int col=minc+*x;
    int curln=*y+minr;
    int length=strlen(buff_p[curln]);
    if(col==0&&curln==0)        //文件开头
    return;
    if(col>0&&minc+*x==length)      //每一行最后一个字符
    {
        buff_p[minr+*y][*x+minc-1]='\0';        //不需要进行后续字符偏移，仅进行删除
        (*x)--;
    }
    else{
        char *temp=(char*)malloc(strlen(buff_p[curln]+minc+*x)+1);
        strcpy(temp,buff_p[curln]+minc+*x);
        if (col==0)     //每一行开头进行多行合并
        {
            int prev=curln-1;
            int prevln=strlen(buff_p[prev]);
            if(prevln+length>=MAXFILE_X-1)      //判断是否溢出
            {
                free(temp);
                return;
            }
            strcpy(buff_p[prev]+prevln,temp);
            memmove(buff_p[curln],buff_p[curln+1],(*row-curln-1)*MAXFILE_X);        //统一向上偏移
            (*row)--;
            (*y)--;
            (*x)=prevln-minc;
        }
        else        //同一行内删除
        {
            strcpy(buff_p[curln]+minc+*x-1,temp);
            (*x)--;
        }
        free (temp);
    }
    Ismodified=1;
}
void Del(char **buff_p,int *y,int *x,int minr,int minc,int *row)        //删除后一字符
{
    int col=minc+*x;
    int curln=*y+minr;
    int length=strlen(buff_p[curln]);
    if(curln==*row-1&&col==length)      //文件末尾
    return;
    if(minc+*x==length-1)       //每一行倒数第二个字符
    buff_p[minr+*y][*x+minc]='\0';      //不需要进行后续字符偏移，仅删除
    else{
        if(*x+minc==length)     //每一行末尾，与下一行进行合并
        {
            if (curln>=*row-1) 
            return;
            int next=curln+1;
            int nextln=strlen(buff_p[next]);
            if(nextln+length>=MAXFILE_X-1)      //防止溢出
            return;
            char *temp=(char*)malloc(strlen(buff_p[next])+1);
            strcpy(temp,buff_p[next]);
            strcpy(buff_p[curln]+minc+*x,temp);
            memmove(buff_p[curln+1],buff_p[curln+2],(*row-curln-2)*MAXFILE_X);      //后续每一行向上偏移
            (*row)--;
            free(temp);
        }
        else
        {
            char *temp=(char*)malloc(strlen(buff_p[curln]+minc+*x));        //同一行删除后一个字符
            strcpy(temp,buff_p[curln]+minc+*x+1);
            strcpy(buff_p[curln]+minc+*x,temp);     //仅在本行进行后续字符偏移
            free (temp);
        }
    }
    Ismodified=1;
}
void Enter(char **buff_p,int *y,int *x,int minr,int *minc,int *row)     //EDIT模式下插入换行
{
    int length=strlen(buff_p[minr+*y]);
    int curln=*y+minr;
    if (*row==MAXFILE_Y-1)      //防止溢出
    return;
    if(length==0||*minc+*x==length)     //若光标后无需要偏移的字符或者改行为空
    {
        memmove(buff_p[curln+2],buff_p[curln+1],(*row-curln-1)*MAXFILE_X);      //仅将后续每一行向下偏移留出一行空行
        buff_p[curln+1][0]='\0';
    }
    else{
        char *temp=(char*)malloc(strlen(buff_p[curln]+*minc+*x)+1);
        strcpy(temp,buff_p[curln]+*minc+*x);        //记录后续需要偏移的字符
        buff_p[curln][*minc+*x]='\0';
        memmove(buff_p[curln+2],buff_p[curln+1],(*row-curln-1)*MAXFILE_X);      //将后续字符另起一行放入，后续每一行向下偏移
        strcpy(buff_p[curln+1],temp);
        free (temp);
    }
    (*row)++;
    (*x)=0;
    (*y)++;
    (*minc)=0;
    Ismodified=1;
}