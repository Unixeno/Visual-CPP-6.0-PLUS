/************************************************************************
 * CL.h is the herder file in CL project!
 * http://yangwang.hk/
 *
 * Copyright (c) 2015 yanwgang
 * Released under the MIT license
 * http://www.opensource.org/licenses/mit-license.php
 *
 * Date: 2015-02-15
 ************************************************************************/

#ifndef __CL_H__
#define __CL_H__

typedef enum                //��������
{
	RESPONSE_FILE,          //��Ӧ�ļ�
	C_WITHOUT_LINK,         //���뵫������
	OPTIMIZATION,           //�Ż��ȼ�
	USER,                   //�û�����Ĳ���
	OBJ_PATH                //Ŀ���ļ�·��
}ParameterType;


typedef struct              //����
{
	ParameterType Type;
	union
	{
		char sValue[50];
		int  iValue;
	};

}Parameter;


//��������
int analyse(char *data, Parameter *parameter);     //��������
int construct(Parameter *parameter);             //����gcc����
int myread(FILE *fp, char *str);
void get_filename(char *filename, char *fname, char* ext);
#endif // __CL_H__
