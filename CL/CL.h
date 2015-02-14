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

typedef enum                //参数类型
{
	RESPONSE_FILE,          //响应文件
	C_WITHOUT_LINK,         //编译但不连接
	OPTIMIZATION,           //优化等级
	USER,                   //用户定义的参数
	OBJ_PATH                //目标文件路径
}ParameterType;


typedef struct              //参数
{
	ParameterType Type;
	union
	{
		char sValue[50];
		int  iValue;
	};

}Parameter;


//函数声明
int analyse(char *data, Parameter *parameter);     //解析参数
int construct(Parameter *parameter);             //生成gcc参数
int myread(FILE *fp, char *str);
void get_filename(char *filename, char *fname, char* ext);
#endif // __CL_H__
