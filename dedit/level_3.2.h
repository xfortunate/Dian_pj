#ifndef LEVEL_3_2_H
#define LEVEL_3_2_H
#define CTRL_R 18
#include "level_3.1.h"

extern char replace_word[256];

int  SetupReplace(int scmax_y, int max_x);       //获取搜索词+替换词
int  ReplaceCurrent(char **buff_p, int row);        //替换当前匹配
int  ReplaceAllMatches(char **buff_p, int row);         //全部替换
void ClearReplace(void);          //清空替换词
void StartReplace(char **buff_p, int *y, int *x, int *minr, int *minc, int *row, int scmax_y, int max_x, const char *filename, int max_y);      //替换模式入口，处理输入及交互循环

#endif
