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
�˴����Ѿ������濪��  ���汣��    ����BUG
*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include "lib.h"
#include "link.h"
#include <stdbool.h>

char GCCParameter[500] = " -Wall";     //gcc����
char Filename[50][50];         //����Ŀ����ļ���
unsigned char  FileNumber = 0;         //�����ļ�������
bool ShowState = false;                 //Ĭ�ϲ���ʾ�������

//��������
static int analyse(char *data, Parameter *parameter);     //��������
static int construct(Parameter *parameter);             //����gcc����
static int myread(FILE *fp, char *str);                 //����Ӧ�ļ���ȡ����

int main(int argc, char *argv[])
{
	int i = 0;
	Parameter parameter[50];        //���֧��50����Ч����
	char tmp[100];                  //��ʱ��Ų���

	if (argc <= 1)
	{
		fprintf(stderr, "û�в���\n");
		exit(1);
	}
	analyse(argv[1], &parameter[0]);    //��������

	if (parameter[0].Type == RESPONSE_FILE)
	{
	    CopyFile(parameter[0].sValue, "LINK.tmp", FALSE);
		FILE *tmp_file = fopen(parameter[0].sValue, "r");
		if(!tmp_file)
		{
			fprintf(stderr, "����: �޷����ļ�:%s\n", parameter[0].sValue);
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
        fprintf(stderr, "ֱ��ʹ������������Ҫ����VIP��\n");
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
		fprintf(stderr, "����: �޷�������ļ�!\n");
		exit(1);
	}
	return returncode;
}

/*
 ���� data:��Ҫ����������
 ���� parameter: �����������Ľṹ��
 ����ֵ 1��ʾ�����ѱ�����parameter��ָ��Ľṹ��
        0��ʾ����û�б��浽�ṹ��
*/
static int analyse(char *data, Parameter *parameter)     //��������
{
	if(data[0] == '@')      //��Ӧ�ļ���־
	{
		parameter->Type = RESPONSE_FILE;
		sprintf(parameter->sValue, "%s", data+1);    //���ļ������ݸ�value
		return 1;
	}

    if(0 == strcmp(data, "/debug"))
    {
        ShowState = true;
        return 0;
    }

	if(data[0] == '/' && data[1] == 'o' && data[2] == 'u')      //Ӧ��������ļ�
	{
		parameter->Type = OUT_PATH;
        data[strlen(data)]=0;         //ȥ��ĩβ������
        sprintf(parameter->sValue, "%s", data + 6);
        return 1;
	}

	if(data[0] == '"')
	{
		sprintf(Filename[FileNumber], "%s", data);
		FileNumber++;
		return 0;
	}

	if(data[0] != '"' && data[0] != '/')        //Ӧ�������ӿ�
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

	//���س�����Ϣ
	return 0;
}


static int construct(Parameter *parameter)             //����gcc����
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
			sprintf(tmp, " -o \"%s\"", parameter->sValue);     //�ϳ�����Ŀ���ļ���·��
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
	int i= 0, flag = 0;     //flag�����˶�ȡ�ķ�ʽ
	do
	{
		ch = fgetc(fp);
	} while(ch == ' ' || ch == '\n');          //ֱ��ȡ����Ч���ַ�
	if(ch == EOF)
		return 0;               //�����ļ�β�ͷ��ؼ�

    while(((!flag && ch != ' ') || (flag && ch != '"')) &&ch != EOF)        //������߼���ĺ���...
	{
	    if(ch == '"')
        {
            flag = 1;       //������˫���žͱ��������һ��˫����
        }
		str[i] = ch;            //��ֵ��str
		ch = fgetc(fp);
		i++;
	}

	str[i] = '\0';                //������������ֹ��
	return 1;                   //��������˵������������Ķ�����Ч�Ĳ���
}

