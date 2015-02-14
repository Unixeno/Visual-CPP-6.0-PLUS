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


char GCCParameter[200] = "-Wall";       //gcc����
char OUT_PATH[100];                     //������obj��·��
char Filename[50][100];                 //����������ļ�
unsigned char FileNumber = 0;           //�����ļ�������
bool ShowState = false;                 //Ĭ�ϲ���ʾ�������

int main(int argc, char *argv[])
{
	unsigned int i = 0;
	Parameter parameter[20];        //���֧��20����Ч����
	char tmp[100];                  //��ʱ��Ų���

	if (argc == 0)                  //û�в������˳�
	{
		fprintf(stderr, "û�в���\n");
		exit(1);
	}
	analyse(argv[1], &parameter[0]);    //��������

	if (parameter[0].Type == RESPONSE_FILE)
	{
	    CopyFile(parameter[0].sValue, "CL.tmp", FALSE);      //��VC���ɵ���ʱ�ļ����Ƴ������������
		FILE *tmp_file = fopen(parameter[0].sValue, "r");
		if(!tmp_file)
		{
			fprintf(stderr, "����: �޷����ļ�:%s\n", parameter[0].sValue);
			exit(1);
		}
		for(i = 0; myread(tmp_file, tmp);)
		{
			if(analyse(tmp, &parameter[i]))     //����Ч�Ĳ�������parameter������
			{
				i++;
			}
		}
		fclose(tmp_file);
	}
	else
    {
        fprintf(stderr, "ֱ��ʹ�����������Ҫ����VIP��\n");
        exit(1);
    }

	for(int n = 0; n < i; n++)          //�ٴ˽����еĲ����ϲ�����
	{
		construct(&parameter[n]);
	}

	for(int n = 0; n < FileNumber; n++)
    {
        char tmp[200] = "\0";
        char std[15] = "\0";
        char ext[5];
        char ifilename[100];        //�����ļ�����i����input
        int returncode;

        get_filename(Filename[n], ifilename, ext);

//�˴������ļ����жϱ�׼��C����Ĭ��C99��C++��Ĭ��C++11
        if(!strcmp(".cpp", ext))
        {
            strcat(std, "-std=c++11");        //Ĭ��c++ʹ��c++11��׼
        }
        else if(!strcmp(".c", ext))
        {
            strcat(std, "-std=c99");           //c������ʹ��c99
        }
        else
        {
            fprintf(stderr, "���󣺱���%s%s ��Ҫ����VIP�� 50����ÿ�꣡\n", ifilename, ext);
            exit(1);
        }
        sprintf(tmp, "%s %s -o %s%s.obj %s\"", GCCParameter, std, OUT_PATH, ifilename, Filename[n]);

        ShowState?printf("->gcc.exe %s\n", tmp):printf("->%s%s\n", ifilename, ext);         //������ڱ�����ļ���
//����gcc ����������ֵ���ظ�VC
        returncode = MyExec("gcc.exe", tmp, "C_Output.txt");
        if (ShowFile("C_Output.txt"))
        {
            fprintf(stderr, "����: �޷�������ļ�!\n");
            exit(1);
        }
        if(returncode)
            exit(1);
    }

	return 0;
}

void get_filename(char *filename, char *fname, char *ext)                //����������ļ���
{
	char drive[5];
	char dir[100];
	_splitpath(filename, drive, dir, fname, ext);
}

int analyse(char *data, Parameter *parameter)     //��������
{
	if(data[0] == '@')      //��Ӧ�ļ���־
	{
		parameter->Type = RESPONSE_FILE;
		sprintf(parameter->sValue, "%s", data+1);    //���ļ������ݸ�value
		return 1;
	}

	if(0 == strcmp(data, "/c"))          //���뵫������
	{
		parameter->Type = C_WITHOUT_LINK;
		return 1;
	}

	if(0 == strcmp(data, "/debug"))
    {
        ShowState = true;
        return 0;
    }

	if(data[0] == '/' && data[1] == 'O')        //�Ż��ȼ�
	{
		parameter->Type = OPTIMIZATION;
		switch(data[2])
		{
		case 'd':parameter->iValue = '0';break;     //���Ż�
		case 'x':parameter->iValue = '3';break;     //����Ż�
		case '1':
		case '2':parameter->iValue = '2';break;     //һ���Ż�
		default :parameter->iValue = '1';           //Ĭ��
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

	//���س�����Ϣ
	return 0;
}


int construct(Parameter *parameter)             //����gcc����
{
    char tmp[50];
	switch(parameter->Type)
	{
	case C_WITHOUT_LINK:strcat(GCCParameter, " -c");break;
	case OPTIMIZATION:sprintf(tmp, " -O%c", parameter->iValue);         //�Ż�ѡ��
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
	} while(ch == ' ' || ch == '\n');          //ֱ��ȡ����Ч���ַ�
	if(ch == EOF)
		return 0;               //�����ļ�β�ͷ��ؼ�

	if(ch == '"')               //�ַ�����������
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
		str[i] = ch;            //��ֵ��str
		ch = fgetc(fp);
		i++;
	}while(ch != ' ' && ch != EOF);          //���ֿհ׷���ֹͣ

	str[i] = '\0';                //������������ֹ��
	if(0 == strcmp(str, "/D"))      //��Ҫ���������D����
	{
		myread(fp, str);                   //��������
		return myread(fp, str);    //�õݹ�ķ�ʽ�Ե����е�D����
	}
	return 1;                   //��������˵������������Ķ�����Ч�Ĳ���
}

