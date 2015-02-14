/************************************************************************
 * link.h is the header file in LINK project!
 * http://yangwang.hk/
 *
 * Copyright (c) 2015 yanwgang
 * Released under the MIT license
 * http://www.opensource.org/licenses/mit-license.php
 *
 * Date: 2015-02-15
 ************************************************************************/


#ifndef __LINK_H__
#define __LINK_H__

typedef enum                //参数类型
{
	RESPONSE_FILE,          //响应文件
	OPTIMIZATION,           //优化等级
	USER,                   //用户自定义参数
	OUT_PATH,               //目标文件路径
	LIB                     //lib文件
}ParameterType;

typedef struct
{
	ParameterType Type;
	union
	{
		char sValue[50];
		int  iValue;
	};

}Parameter;             //参数


#endif // __LINK_H__
