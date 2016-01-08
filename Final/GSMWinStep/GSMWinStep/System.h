/* Project:		GSMWinStep
File Name:	System.h
Author（former）:		刘芳
Author KY
Date:		2015-9-15
Purpose:		系统管理器
*/

#ifndef _System
#define _System

#include <stdio.h>
#include <Windows.h>

//------------------------------------------------------------------------------
// Typedef:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Consts:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Structures:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------
FILE *fp;   // Output.txt

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

// 系统初始化
extern int System_Initialize(HINSTANCE, int);

// 系统退出
extern void System_Exit(void);

#endif
