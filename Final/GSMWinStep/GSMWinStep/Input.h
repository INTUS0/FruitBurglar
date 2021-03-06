/* Project:		GSMWinStep
File Name:	Input.h
Author（former）:		刘芳
Author KY
Date:
Purpose:		输入模块  */


#ifndef _Input
#define _Input


//新加全局变量
float posX, posY; //为了获取鼠标位置
POINTS p;//鼠标获取的临时位置

enum{
	KeyR,
	KeyESC,
	Key1,
	Key2,
	KeyUp,
	KeyDown,
	KeyLeft,
	KeyRight,
	KeyMenu,//菜单键
	KeyLeftBottom,//鼠标左键
	KeyRightBottom,//鼠标右键，
	KeySpace,//空格键
	KeyNUM    // 总放在最后一个
};
boolean KeyPressed[KeyNUM];

//新加全局函数
// 输入状态重置
void Input_Initialize(void);


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

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

// 处理输入
LRESULT CALLBACK Input_Handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif