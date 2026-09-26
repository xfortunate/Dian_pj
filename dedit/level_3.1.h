#ifndef LEVEL_3_1_H
#define LEVEL_3_1_H
#define CTRL_F 6
#define CTRL_P 16
#include "level_2.1.h"

typedef struct{
    int y;
    int x;
}Match;
extern char search_word[256];
extern Match *match_bf;
extern int match_cnt;
extern int match_p;
extern int bufcapacity;
int  SetupSearch(int scmax_y, int max_x);       //开启搜索框并传回搜索子串
void SearchMatch(char **buff_p, int row);       //查找所有匹配字串坐标并传入索引
int  NextMatch(int *out_y, int *out_x);         //传出下一个匹配子串实际y,x坐标
int  PrevMatch(int *out_y, int *out_x);         //传出上一个匹配子串实际y,x坐标
void DrawHighlights(char **buff_p,int max_y,int max_x,int minr,int minc);       //在文本中重新绘制高亮子串
void ClearSearch(void);     //清除匹配状态

#endif