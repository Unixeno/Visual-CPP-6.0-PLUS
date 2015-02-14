/************************************************************************
 * main.c is the main sources file in cl project!
 * http://yangwang.hk/
 *
 * Copyright (c) 2015 yanwgang
 * Released under the MIT license
 * http://www.opensource.org/licenses/mit-license.php
 *
 * Date: 2015-02-15
 ************************************************************************/
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "CL.h"
#include <stdbool.h>
#include "lib.h"


char GCCParameter[200] = "-Wall";       //gcc参数
char OUT_PATH[100];                     //输出输出obj的路径
char Filename[50][100];                 //编译的输入文件
unsigned char FileNumber = 0;           //输入文件的数量
bool ShowState = false;                 //默认不显示编译参数

int main(int argc, char *argv[])
{
	unsigned int i = 0;
	Parameter parameter[20];        //最多支持20个有效参数
	char tmp[100];                  //临时存放参数

	if (argc == 0)                  //没有参数则退出
	{
		fprintf(stderr, "没有参数\n");
		exit(1);
	}
	analyse(argv[1], &parameter[0]);    //分析参数

	if (parameter[0].Type == RESPONSE_FILE)
	{
	    CopyFile(parameter[0].sValue, "CL.tmp", FALSE);      //将VC生成的临时文件复制出来，方便调试
		FILE *tmp_file = fopen(parameter[0].sValue, "r");
		if(!tmp_file)
		{
			fprintf(stderr, "错误: 无法打开文件:%s\n", parameter[0].sValue);
			exit(1);
		}
		for(i = 0; myread(tmp_file, tmp);)
		{
			if(analyse(tmp, &parameter[i]))     //将有效的参数放入parameter数组中
			{
				i++;
			}
		}
		fclose(tmp_file);
	}
	else
    {
        fprintf(stderr, "直接使用命令编译需要加入VIP！\n");
        exit(1);
    }

	for(int n = 0; n < i; n++)          //再此将所有的参数合并起来
	{
		construct(&parameter[n]);
	}

	for(int n = 0; n < FileNumber; n++)
    {
        char tmp[200] = "\0";
        char std[15] = "\0";
        char ext[5];
        char ifilename[100];        //输入文件名，i代表input
        int returncode;

        get_filename(Filename[n], ifilename, ext);

//此处根据文件名判断标准，C语言默认C99，C++则默认C++11
        if(!strcmp(".cpp", ext))
        {
            strcat(std, "-std=c++11");        //默认c++使用c++11标准
        }
        else if(!strcmp(".c", ext))
        {
            strcat(std, "-std=c99");           //c语言则使用c99
        }
        else
        {
            fprintf(stderr, "错误：编译%s%s 需要加入VIP， 50美刀每年！\n", ifilename, ext);
            exit(1);
        }
        sprintf(tmp, "%s %s -o %s%s.obj %s\"", GCCParameter, std, OUT_PATH, ifilename, Filename[n]);

        ShowState?printf("->gcc.exe %s\n", tmp):printf("->%s%s\n", ifilename, ext);         //输出正在编译的文件名
//调用gcc ，并将返回值返回给VC
        returncode = MyExec("gcc.exe", tmp, "C_Output.txt");
        if (ShowFile("C_Output.txt"))
        {
            fprintf(stderr, "错误: 无法打开输出文件!\n");
            exit(1);
        }
        if(returncode)
            exit(1);
    }

	return 0;
}

void get_filename(char *filename, char *fname, char *ext)                //产生结果的文件名
{
	char drive[5];
	char dir[100];
	_splitpath(filename, drive, dir, fname, ext);
}

int analyse(char *data, Parameter *parameter)     //解析参数
{
	if(data[0] == '@')      //响应文件标志
	{
		parameter->Type = RESPONSE_FILE;
		sprintf(parameter->sValue, "%s", data+1);    //将文件名传递给value
		return 1;
	}

	if(0 == strcmp(data, "/c"))          //编译但不链接
	{
		parameter->Type = C_WITHOUT_LINK;
		return 1;
	}

	if(0 == strcmp(data, "/debug"))
    {
        ShowState = true;
        return 0;
    }

	if(data[0] == '/' && data[1] == 'O')        //优化等级
	{
		parameter->Type = OPTIMIZATION;
		switch(data[2])
		{
		case 'd':parameter->iValue = '0';break;     //无优化
		case 'x':parameter->iValue = '3';break;     //最高优化
		case '1':
		case '2':parameter->iValue = '2';break;     //一般优化
		default :parameter->iValue = '1';           //默认
		}

		return 1;
	}

	if(data[0] == '/' && data[1] == 'F')
	{
		switch(data[2])
		{
			case 'o':parameter->Type = OBJ_PATH;
				data[strlen(data)-1]=0;
			sprintf(OUT_PATH, "%s", data + 4);return 1;
		}
	}

	if(data[0] == '"')
	{
		sprintf(Filename[FileNumber], "%s", data);
		FileNumber++;
		return 0;
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


int construct(Parameter *parameter)             //生成gcc参数
{
    char tmp[50];
	switch(parameter->Type)
	{
	case C_WITHOUT_LINK:strcat(GCCParameter, " -c");break;
	case OPTIMIZATION:sprintf(tmp, " -O%c", parameter->iValue);         //优化选项
	    strcat(GCCParameter, tmp);break;
    case USER:
        sprintf(tmp, " %s", parameter->sValue);
        strcat(GCCParameter, tmp);break;
	default:;
	}
	return 0;
}

int myread(FILE *fp, char *str)
{
	char ch;
	int i= 0;
	do
	{
		ch = fgetc(fp);
	} while(ch == ' ' || ch == '\n');          //直到取出有效的字符
	if(ch == EOF)
		return 0;               //到了文件尾就返回假

	if(ch == '"')               //字符串则必须读完
	{
		str[0] = ch;
		ch = fgetc(fp);
		while(ch != '"' && ch != EOF)
		{
			i++;
			str[i] = ch;
			ch = fgetc(fp);
		}
		i++;
	}
	else do
	{
		str[i] = ch;            //赋值给str
		ch = fgetc(fp);
		i++;
	}while(ch != ' ' && ch != EOF);          //出现空白符则停止

	str[i] = '\0';                //别忘了填上终止符
	if(0 == strcmp(str, "/D"))      //需要忽略讨厌的D参数
	{
		myread(fp, str);                   //丢弃参数
		return myread(fp, str);    //用递归的方式吃掉所有的D参数
	}
	return 1;                   //理论上来说到这里读出来的都是有效的参数
}

