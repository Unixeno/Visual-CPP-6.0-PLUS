/************************************************************************
 * lib.h This file is part of the "lib" project!
 * This library is designed to be used in CL.EXE and LINK.EXE
 * http://yangwang.hk/
 *
 * Copyright (c) 2015 yanwgang
 * Released under the MIT license
 * http://www.opensource.org/licenses/mit-license.php
 *
 * Date: 2015-02-15
 ***********************************************************************/


#ifndef __LIB_H__
#define __LIB_H__

#ifndef _WINDOWS_H
#include <windows.h>
#endif // _WINDOWS_H

#ifndef _STDIO_H_
#include <stdio.h>
#endif // _STDIO_H_

int MyExec(char *cmd, char *parameter, char *FileName);
int ShowFile(char *filename);

#endif // __LIB_H__
