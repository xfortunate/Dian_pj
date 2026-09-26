#ifndef LEVEL_2_1_H
#define LEVEL_2_1_H

#define CTRL_Q 17 
#define MAXFILE_Y 1500
#define MAXFILE_X 1500
#define EDIT 0
#define SEARCH 1
#define REPLACE 2

extern int MODE;
extern const char *MODENAME[3];
int min(int a,int b);
void ScrollSc(char **buff_p, int max_y, int max_x, int minr, int minc, int row);    //绘制窗口
void Adjust(char **buff_p,int *y, int *x, int *minr, int *minc, int max_y, int max_x, int row);  //统一进行窗口坐标纠偏

#endif