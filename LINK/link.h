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

typedef enum                //��������
{
	RESPONSE_FILE,          //��Ӧ�ļ�
	OPTIMIZATION,           //�Ż��ȼ�
	USER,                   //�û��Զ������
	OUT_PATH,               //Ŀ���ļ�·��
	LIB                     //lib�ļ�
}ParameterType;

typedef struct
{
	ParameterType Type;
	union
	{
		char sValue[50];
		int  iValue;
	};

}Parameter;             //����


#endif // __LINK_H__
