/* Project:		GSMWinStep
File Name:	Input.c
Author(former):	刘芳
Author Int us
Date:		2015-9-15
Purpose:		输入模块  */

#include "System.h"
#include "Input.h"



//新增函数
void Input_Initialize()
{
	int i;
	for (i = 0; i<KeyNUM; i++)
	{
		KeyPressed[i] = FALSE;
	}
}

//------------------------------------------------------------------------------
// Private Consts:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private Structures:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------
LRESULT CALLBACK Input_Handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC dc;           // 设备
	PAINTSTRUCT ps;   // 重绘结构

	// 签到
	//fprintf(fp,"Input:Handle\n");

	switch (msg)
	{
		//  窗口创建
	case WM_CREATE:
		break;

	case WM_RBUTTONDOWN:
		if (wParam == VK_RBUTTON)
		{
			KeyPressed[KeyRightBottom] = TRUE;
			
		}
			
			
		break;

	case WM_LBUTTONDOWN:
		//GS_Running = 0;
		//鼠标键
		if (wParam == VK_LBUTTON)
		{
			KeyPressed[KeyLeftBottom] = TRUE;
			p.x = LOWORD(lParam) - 400;    //获取鼠标位置x坐标信息
			p.y = -HIWORD(lParam) + 300;    //获取鼠标位置y坐标信息
			posX = p.x;
			posY = p.y;
		}
		break;

	case WM_MOUSEMOVE:
		

		break;

		// 重绘
	case WM_PAINT:
		dc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

		// 窗口关闭
	//case WM_DESTROY:
	//	break;
	case WM_DESTROY:
		KeyPressed[KeyESC] = TRUE;
		fprintf(fp, "Input:Forcing Shut Down\n");
		break;

	//case WM_KEYDOWN:
		//if(wParam == VK_ESCAPE)
		//GS_Running = 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			KeyPressed[KeyESC] = TRUE;
			fprintf(fp, "Input:ESC\n");
		}
		if (wParam == 'R')
		{
			KeyPressed[KeyR] = TRUE;
			fprintf(fp, "Input:R\n");
		}
		if (wParam == '1')
		{
			KeyPressed[Key1] = TRUE;
			fprintf(fp, "Input:1\n");
		}
		if (wParam == '2')
		{
			KeyPressed[Key2] = TRUE;
			fprintf(fp, "Input:2\n");
		}
		
		if (wParam == VK_UP|| wParam == 'W')
			KeyPressed[KeyUp] = TRUE;
		if (wParam == VK_DOWN || wParam =='S' )
			KeyPressed[KeyDown] = TRUE;
		if (wParam == VK_LEFT || wParam == 'A')
			KeyPressed[KeyLeft] = TRUE;
		if (wParam == VK_RIGHT || wParam == 'D')
			KeyPressed[KeyRight] = TRUE;
		if (wParam == 'M')
			KeyPressed[KeyMenu] = TRUE;
		if (wParam == VK_SPACE)
			KeyPressed[KeySpace] = TRUE;

		

		break;

		// 窗口发生移动
	case WM_MOVE:
		break;

		// 调用默认窗口过程
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------

