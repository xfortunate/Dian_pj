#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "level_2.1.h"
#include "level_2.2.h"
#include "level_2.3.h"
#include "level_2.4.h"
#include "level_2.5.h"

#define V_MAXY 50
#define V_MAXX 100
static char *buff=NULL;     //模拟构建内存buff
static char **buff_p=NULL;      //模拟buff_p指针数组
static int row=0;       //虚拟行数
typedef struct{
    int y,x;
    int minr,minc;
} Cursor;
static Cursor cur;

static void S_setup(char *initial_text)     //模拟初始化环境
{
    buff=(char *)malloc(sizeof(char)*MAXFILE_Y*MAXFILE_X);      //构建buff
    buff_p=(char **)malloc(sizeof(char *)*MAXFILE_Y);       //构建buff指针数组并进行指向
    if (buff==NULL||buff_p==NULL) 
    {
        fprintf(stderr,"Failed to locate memory");
        exit(1);
    }
    memset(buff,0,sizeof(char)*MAXFILE_Y*MAXFILE_X);    //初始化后整块清零以避免读取残留垃圾数据
    for (int i=0;i<MAXFILE_Y;i++)
    buff_p[i]=buff+i*MAXFILE_X;
    row=0;
    char *p=initial_text;
    if(!p)
    {
        buff_p[row][0]='\0';
        return;
    }
    while(*p!='\0'&&row<MAXFILE_Y)      //模拟写入内存
    {
        char *temp=strchr(p,'\n');
        if(temp)
        {
            int len=(int)(temp-p); 
            strncpy(buff_p[row],p,len);
            buff_p[row][len]='\0';
            row++;
            p=temp+1;
        }
        else
        {
            int len=min(MAXFILE_X-1,strlen(p));
            strncpy(buff_p[row],p,len);
            buff_p[row][len]='\0';
            row++;
            break;
        }
    }
}

static void S_clear(void)       //重置模拟环境
{
    free(buff);
    free(buff_p);
    buff=NULL;
    buff_p=NULL;
    row=0;
    Ismodified=0;
    message[0]='\0';
}

static void ResetCur(void)      //重设模拟坐标及屏幕参数
{
    cur.y=0;
    cur.x=0;
    cur.minc=0;
    cur.minr=0;
}

static void S_pushkey(char *key,const char *save_path)      //读取测试数据集
{
    char *p=key;
    while(*p)
    {
        if(*p=='<')     //读取<>中的特殊键
        {
            char *end=strchr(p,'>');
            if(!end)
            break;
            int len=end-p-1;
            char temp[50]="";
            strncpy(temp,p+1,len);
            temp[len]='\0';
            //根据所读的特殊键调用函数
            if(strcmp(temp,"Backspace")==0)
            BackSp(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row);
            else if(strcmp(temp,"Del")==0)
            Del(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row);
            else if(strcmp(temp,"Enter")==0)
            Enter(buff_p,&cur.y,&cur.x,cur.minr,&cur.minc,&row);
            else if(strcmp(temp,"Ctrl-S")==0)
            {
                if (save_path!=NULL)        //判断是否有保存路径
                {
                    if(SaveFile(buff_p,row,save_path))
                    Ismodified=0;
                }
            }
            p=end+1;
        }
        else        //读取普通输入
        {
            Insert(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row,*p);
            p++;
        }
        Adjust(buff_p,&cur.y,&cur.x,&cur.minr,&cur.minc,V_MAXY,V_MAXX,row);     //模拟调整屏幕坐标
    }
}

static void test_insert_basic(void)     //测试基础输入
{
    printf("\n[Test Case 1] Basic Insert\n");
    S_setup("");
    ResetCur();
    S_pushkey("hello",NULL);
    ASSERT_STR_EQ(buff_p[0],"hello","Buffer contains 'hello'");
    ASSERT_INT_EQ(cur.x,5,"Cursor.x moved to position 5");
    S_clear();
}

static void test_insert_middle(void)        //测试在字符中间输入
{
    printf("\n[Test Case 2] Insert in the middle\n");
    S_setup("hello");
    ResetCur();
    cur.x=2;
    Insert(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row,'w');
    ASSERT_STR_EQ(buff_p[0], "hewllo", "Character inserted in the middle");
    ASSERT_INT_EQ(cur.x, 3, "Cursor.x moved right");

    S_clear();
}

static void test_backspace_same_line(void)      //测试同行退格
{
    printf("\n[Test Case 3] Backspace same line\n");
    S_setup("hello");      //行尾退格
    ResetCur();
    cur.x = 5;
    BackSp(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row);
    ASSERT_STR_EQ(buff_p[0],"hell","Backspace at end of line");
    ASSERT_INT_EQ(cur.x,4,"Cursor.x moved left");
    S_clear();

    S_setup("hello");        //行中间退格
    ResetCur();
    cur.x=3;
    BackSp(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row);
    ASSERT_STR_EQ(buff_p[0],"helo","Backspace in middle of line");
    ASSERT_INT_EQ(cur.x,2,"Cursor.x moved left");
    S_clear();   
    
    S_setup("");       //空文件退格
    ResetCur();
    BackSp(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row);
    ASSERT_STR_EQ(buff_p[0],"","Backspace in empty file");
    S_clear();
}

static void test_backspace_cross_line(void)     //测试不同行退格
{
    printf("\n[Test Case 4] Cross-line Backspace\n");
    S_setup("hello\nworld");
    ASSERT_INT_EQ(row,2,"Initially 2 rows");
    ResetCur();     
    cur.y=1; 
    cur.x=0;       
    BackSp(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row);
    ASSERT_INT_EQ(row,1,"Rows merged");
    ASSERT_STR_EQ(buff_p[0],"helloworld","Two lines merged");
    ASSERT_INT_EQ(cur.y,0,"Cursor moved to previous line");
    S_clear();
}

static void test_delete(void)       //测试删除后一个字符
{
    printf("\n[Test Case 5] Delete\n");
    S_setup("hello");      //同行删除
    ResetCur();
    cur.x=1;
    Del(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row);
    ASSERT_STR_EQ(buff_p[0],"hllo","Delete in middle");
    ASSERT_INT_EQ(cur.x,1,"Cursor stays");
    S_clear();

    S_setup("hel\nlo");        //跨行合并
    ResetCur();
    cur.x=3;
    Del(buff_p,&cur.y,&cur.x,cur.minr,cur.minc,&row);
    ASSERT_INT_EQ(row,1,"Rows merged");
    ASSERT_STR_EQ(buff_p[0],"hello","Content merged");
    S_clear();
}

static void test_enter(void)        //测试换行
{
    printf("\n[Test Case 6] Enter key\n");
    S_setup("hello world");
    ResetCur();
    cur.x=5;
    Enter(buff_p,&cur.y,&cur.x,cur.minr,&cur.minc,&row);       //行中间回车
    ASSERT_INT_EQ(row,2,"Row count increased");
    ASSERT_STR_EQ(buff_p[0],"hello","First half before enter");
    ASSERT_STR_EQ(buff_p[1]," world","Second half after enter");
    ASSERT_INT_EQ(cur.y,1,"Cursor moved to new line");
    S_clear();

    S_setup("hello");
    ResetCur();
    cur.x=5;
    Enter(buff_p,&cur.y,&cur.x,cur.minr,&cur.minc,&row);       //行尾回车
    ASSERT_INT_EQ(row, 2, "Row count increased");
    ASSERT_STR_EQ(buff_p[1], "","Second line empty");
    S_clear();
}

static void test_save_file(void)        //测试文件保存
{
    printf("\n[Test Case 7] Save File\n");
    {
        char tmpl[]="/tmp/dedit_test_XXXXXX";       //XXXXXX会经过mkstemp更改为一特定值
        int fd=mkstemp(tmpl);       //创建临时文件
        if (fd<0) 
        { 
            printf("[SKIP] mkstemp failed\n"); 
            return; 
        }
        close(fd);

        S_setup("hello\nworld\nabcd");      //进行多行文本读取与保存
        int jud=SaveFile(buff_p,row,tmpl);
        ASSERT_INT_EQ(jud, 1,"SaveFile success");

        FILE *fp = fopen(tmpl, "r");
        char buf[100]="";
        if (fp) 
        {
            size_t len=fread(buf,1,sizeof(buf)-1,fp);
            buf[len]='\0';
            fclose(fp);
        }
        ASSERT_STR_EQ(buf,"hello\nworld\nabcd\n","Multi-line content saved");
        S_clear();
        unlink(tmpl);       //删除临时文件
    }
    {
        char tmpl[] = "/tmp/dedit_test_XXXXXX";
        int fd = mkstemp(tmpl);
        if (fd<0) 
        { 
            printf("[SKIP] mkstemp failed\n"); 
            return; 
        }
        close(fd);

        S_setup("");        //进行空文本读取与保存
        SaveFile(buff_p,row,tmpl);
        FILE *fp=fopen(tmpl,"r");
        char buf[100]="";
        if (fp) 
        {
            size_t n=fread(buf,1,sizeof(buf)-1,fp);
            buf[n]='\0';
            fclose(fp);
        }
        ASSERT_STR_EQ(buf,"","Empty content saved");
        S_clear();
        unlink(tmpl);
    }
}

static void test_special_script(void)       //测试具体复杂输入与保存
{
    printf("\n[Test Case 8] Script Parsing and Save\n");
    char tmpl[]="/tmp/dedit_test_XXXXXX";
    int fd=mkstemp(tmpl);
    if (fd<0) 
    { 
        printf("[SKIP] mkstemp failed\n"); 
        return; 
    }
    close(fd);
    S_setup("");
    ResetCur();
    S_pushkey("hello we<Backspace>orld<Ctrl-S>", tmpl);
    ASSERT_STR_EQ(buff_p[0],"hello world","Script produced 'hello world'");
    ASSERT_INT_EQ(Ismodified,0, "Ctrl-S reset modified");
    FILE *fp=fopen(tmpl,"r");        //回读文件
    char buf[100]="";
    if (fp) 
    {
        size_t n=fread(buf,1,sizeof(buf)-1,fp);
        buf[n]='\0';
        fclose(fp);
    }
    ASSERT_STR_EQ(buf,"hello world\n","Saved file matches");
    S_clear();
    unlink(tmpl);
}

int main()
{
    printf("==============================================\n");
    printf("  Dian-Editor Auto Test\n");
    printf("==============================================\n");

    test_insert_basic();
    test_insert_middle();
    test_backspace_same_line();
    test_backspace_cross_line();
    test_delete();
    test_enter();
    test_save_file();
    test_special_script();

    SUMMARY();
    return 0;
}