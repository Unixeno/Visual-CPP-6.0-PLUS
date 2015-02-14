/************************************************************************
 * main.c This file is part of the "lib" project!
 * This library is designed to be used in CL.EXE and LINK.EXE
 * http://yangwang.hk/
 *
 * Copyright (c) 2015 yanwgang
 * Released under the MIT license
 * http://www.opensource.org/licenses/mit-license.php
 *
 * Date: 2015-02-15
 ************************************************************************/

#include "lib.h"

/*
    函数：MyExec
    参数：
        cmd         要执行的程序
        parameter   传递给程序的参数
        Filename    stderr重定向的输出文件名

    返回值
        调用程序的返回值
*/

#include <windows.h>

int MyExec(char *cmd, char *parameter, char *FileName)
{
    PROCESS_INFORMATION pro_info;
    STARTUPINFO st_info;
    HANDLE file_handle;

    SECURITY_ATTRIBUTES se_attributes;
    se_attributes.nLength = sizeof(se_attributes);
    se_attributes.lpSecurityDescriptor = NULL;
    se_attributes.bInheritHandle = TRUE;

    file_handle = CreateFile(FileName, FILE_APPEND_DATA, FILE_SHARE_READ,
                        &se_attributes, CREATE_ALWAYS,      //总是创建文件
                        FILE_ATTRIBUTE_NORMAL, NULL);
    memset(&pro_info, 0, sizeof(pro_info));
    memset(&st_info, 0, sizeof(st_info));

    st_info.cb = sizeof(STARTUPINFO);
    st_info.dwFlags |= STARTF_USESTDHANDLES;
    st_info.wShowWindow =SW_HIDE;
    st_info.hStdInput = NULL;
    st_info.hStdError = file_handle;
    st_info.hStdOutput = NULL;

    char tmp[500];
    sprintf(tmp, "%s %s", cmd, parameter);
    CreateProcess(NULL, tmp, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &st_info, &pro_info);
    WaitForSingleObject(pro_info.hProcess,INFINITE);
    DWORD exit_code;
    GetExitCodeProcess(pro_info.hProcess, &exit_code);

    CloseHandle(file_handle);
    CloseHandle(pro_info.hProcess);
    CloseHandle(pro_info.hThread);


    return exit_code;
}


int ShowFile(char *filename)
{
    FILE *fp = fopen(filename, "r");
    char ch;

    if(!fp)
        return 1;

    while((ch = fgetc(fp)) != EOF)
    {
        putchar(ch);
    }
    fclose(fp);

    return 0;
}


