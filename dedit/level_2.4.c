#include <stdio.h>
#include <string.h>
#include "level_2.4.h"

int SaveFile(char **buff_p,int row,const char *filename)        //保存文件
{
    FILE *file=fopen(filename,"w");
    if(!file)
    return 0;
    for (int i=0;i<row;i++)     //每行输入
    {
        fputs(buff_p[i],file);
        fputc('\n',file);       //结尾加入\n进行分隔
    }
    fclose(file);
    return 1;
}