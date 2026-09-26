#ifndef LEVEL_2_3_H
#define LEVEL_2_3_H

extern int Ismodified;
extern char message[500];
void InitColor(void);   //颜色初始化
void DrawStatusBar(int scmax_y,int max_x,int y,int x,int minr,int minc,const char *filename);       //绘制状态栏
void Refresh_Msg();     //清除message

#endif