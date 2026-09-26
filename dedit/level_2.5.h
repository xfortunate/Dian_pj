#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_pass=0;     //测试通过计数
static int test_fail=0;     //测试错误计数

//断言str类型是否相等，(actual)表示实际结果，(expected)表示期望结果，msg为相关描述，用\来连接不同行
//用do-while(0)来保证使用时结构的正确，避免悬空else
#define ASSERT_STR_EQ(actual,expected,msg)  \
    do{                                     \
        if(strcmp((actual),(expected))==0)  \
        {                                   \
            test_pass++;                    \
            printf("[PASS] %s\n",(msg));    \
        }                                   \
        else                                \
        {                                   \
            test_fail++;                    \
            printf("[FAIL] %s\n",(msg));    \
            printf("[EXPECTED] %s\n",(expected));    \
            printf("[ACTUAL] %s\n",(actual));        \
        }                                   \
    }while(0);                              \

//断言int类型是否相等
#define ASSERT_INT_EQ(actual,expected,msg)  \
    do{                                     \
        if((actual)==(expected))            \
        {                                   \
            test_pass++;                    \
            printf("[PASS] %s\n",(msg));    \
        }                                   \
        else                                \
        {                                   \
            test_fail++;                    \
            printf("[FAIL] %s\n",(msg));    \
            printf("[EXPECTED] %d\n",(expected));    \
            printf("[ACTUAL] %d\n",(actual));        \
        }                                   \
    }while(0);                              \

//总结通过与错误数
#define SUMMARY()                           \
    do{                                     \
        printf("==============================================\n");     \
        printf("Test Summary: %d passed, %d failed\n",test_pass,test_fail);     \
        printf("==============================================\n");     \
        if (test_fail>0)                    \
        exit(1);                            \
    }while(0);                              \

#endif