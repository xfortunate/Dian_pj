#ifndef LEVEL_2_2_H
#define LEVEL_2_2_H

void Insert(char **buff_p,int *y,int *x,int minr,int minc,int *row,char data);      //插入
void BackSp(char **buff_p,int *y,int *x,int minr,int minc,int *row);        //退格
void Del(char **buff_p,int *y,int *x,int minr,int minc,int *row);       //删除后一字符
void Enter(char **buff_p,int *y,int *x,int minr,int *minc,int *row);        //EDIT模式下换行

#endif 