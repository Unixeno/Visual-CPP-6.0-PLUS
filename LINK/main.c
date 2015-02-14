/************************************************************************
 * main.c is the main sources file in LINK project!
 * http://yangwang.hk/
 *
 * Copyright (c) 2015 yanwgang
 * Released under the MIT license
 * http://www.opensource.org/licenses/mit-license.php
 *
 * Date: 2015-02-15
 ************************************************************************/
/**
                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
             .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | _|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
                   `=---='
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
此代码已经过佛祖开光  佛祖保佑    永无BUG
*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include "lib.h"
#include "link.h"
#include <stdbool.h>

char GCCParameter[500] = " -Wall";     //gcc参数
char Filename[50][50];         //编译目标的文件名
unsigned char  FileNumber = 0;         //输入文件的数量
bool ShowState = false;                 //默认不显示编译参数

//函数声明
static int analyse(char *data, Parameter *parameter);     //解析参数
static int construct(Parameter *parameter);             //生成gcc参数
static int myread(FILE *fp, char *str);                 //从响应文件读取内容

int main(int argc, char *argv[])
{
	int i = 0;
	Parameter parameter[50];        //最多支持50个有效参数
	char tmp[100];                  //临时存放参数

	if (argc <= 1)
	{
		fprintf(stderr, "没有参数\n");
		exit(1);
	}
	analyse(argv[1], &parameter[0]);    //分析参数

	if (parameter[0].Type == RESPONSE_FILE)
	{
	    CopyFile(parameter[0].sValue, "LINK.tmp", FALSE);
		FILE *tmp_file = fopen(parameter[0].sValue, "r");
		if(!tmp_file)
		{
			fprintf(stderr, "错误: 无法打开文件:%s\n", parameter[0].sValue);
			exit(1);
		}
		for(i = 0; myread(tmp_file, tmp);)
		{
			if(analyse(tmp, &parameter[i]))
			{
				i++;
			}
		}
		fclose(tmp_file);
	}
    else
    {
        fprintf(stderr, "直接使用命令链接需要加入VIP！\n");
        exit(1);
    }

	for(int n = 0; n < i; n++)
	{
		construct(&parameter[n]);
	}

	for(int n = 0; n < FileNumber; n++)
    {
        sprintf(tmp, " %s\"", Filename[n]);
        strcat(GCCParameter, tmp);
    }

    ShowState?printf("->gcc.exe %s\n", GCCParameter):printf("->Linking...\n");

	int returncode = MyExec("gcc.exe", GCCParameter, "L_Output.txt");
	if (ShowFile("L_Output.txt"))
	{
		fprintf(stderr, "错误: 无法打开输出文件!\n");
		exit(1);
	}
	return returncode;
}

/*
 参数 data:需要分析的数据
 参数 parameter: 保存分析结果的结构体
 返回值 1表示参数已保存与parameter所指向的结构体
        0表示参数没有保存到结构体
*/
static int analyse(char *data, Parameter *parameter)     //解析参数
{
	if(data[0] == '@')      //响应文件标志
	{
		parameter->Type = RESPONSE_FILE;
		sprintf(parameter->sValue, "%s", data+1);    //将文件名传递给value
		return 1;
	}

    if(0 == strcmp(data, "/debug"))
    {
        ShowState = true;
        return 0;
    }

	if(data[0] == '/' && data[1] == 'o' && data[2] == 'u')      //应该是输出文件
	{
		parameter->Type = OUT_PATH;
        data[strlen(data)]=0;         //去掉末尾的引号
        sprintf(parameter->sValue, "%s", data + 6);
        return 1;
	}

	if(data[0] == '"')
	{
		sprintf(Filename[FileNumber], "%s", data);
		FileNumber++;
		return 0;
	}

	if(data[0] != '"' && data[0] != '/')        //应该是链接库
    {
        parameter->Type = LIB;
        data[strlen(data) - 4] = 0;
        sprintf(parameter->sValue, "%s", data);
        return 1;
    }

    if(data[0] == '-')
    {
        parameter->Type = USER;
        sprintf(parameter->sValue, "%s", data);
        return 1;
    }

	//返回出错信息
	return 0;
}


static int construct(Parameter *parameter)             //生成gcc参数
{
    char tmp[50];
	switch(parameter->Type)
	{
	case LIB:
	    {
	        sprintf(tmp, " -l %s",parameter->sValue);
	        strcat(GCCParameter, tmp);
	        break;
	    }
	case OUT_PATH:
		{
			sprintf(tmp, " -o \"%s\"", parameter->sValue);     //合成生成目标文件的路径
			strcat(GCCParameter, tmp);
			break;
		}

    case USER:
        {
            sprintf(tmp, " %s", parameter->sValue);
            strcat(GCCParameter, tmp);
            break;
        }
	default:;
	}
	return 0;
}

static int myread(FILE *fp, char *str)
{
	char ch;
	int i= 0, flag = 0;     //flag决定了读取的方式
	do
	{
		ch = fgetc(fp);
	} while(ch == ' ' || ch == '\n');          //直到取出有效的字符
	if(ch == EOF)
		return 0;               //到了文件尾就返回假

    while(((!flag && ch != ' ') || (flag && ch != '"')) &&ch != EOF)        //这里的逻辑真的好乱...
	{
	    if(ch == '"')
        {
            flag = 1;       //出现了双引号就必须读到下一个双引号
        }
		str[i] = ch;            //赋值给str
		ch = fgetc(fp);
		i++;
	}

	str[i] = '\0';                //别忘了填上终止符
	return 1;                   //理论上来说到这里读出来的都是有效的参数
}

