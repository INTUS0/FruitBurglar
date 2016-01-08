/* Project:		GSMWinStep
File Name:	Menu.c
Author:  INT US
Date:		2015-11-19
Purpose:		关卡1  */

#include <stdio.h>
#include "GameStateList.h"
#include "System.h"
#include "Level3.h"
#include "AEEngine.h"
#include "Input.h"

//------------------------------------------------------------------------------
// Private Consts:


//------------------------------------------------------------------------------
// Private Structures:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private Variables:
//------------------------------------------------------------------------------




static AEGfxVertexList*	pMesh1;	// 对象1的网格(mesh)模型
static float obj1X, obj1Y;		// 对象1的位置
static AEGfxTexture *pTex1;		// 对象1背景的纹理


								//------------------------------------------------------------------------------
								// Private Function Declarations:
								//------------------------------------------------------------------------------

								//------------------------------------------------------------------------------
								// Public Functions:
								//------------------------------------------------------------------------------

void Load3(void)
{



	// 开始添加对象1
	AEGfxMeshStart();
	AEGfxTriAdd(
		-450.0f, -300.0f, 0x00FF00FF, 0.0f, 1.0f,
		450.0f, -300.0f, 0x00FFFF00, 1.0f, 1.0f,
		-450.0f, 300.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		450.0f, -300.0f, 0x00FFFFFF, 1.0f, 1.0f,
		450.0f, 300.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-450.0f, 300.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pMesh1 = AEGfxMeshEnd();
	AE_ASSERT_MESG(pMesh1, "Failed to create mesh 1!!");
	pTex1 = AEGfxTextureLoad("About.PNG");
	// 载入纹理

	


	// 签到
	fprintf(fp, "Menu:Load\n");
}

void Ini3(void)
{



	// 对象1的初始位置
	obj1X = 0.0f;
	obj1Y = 0.0f;


	
	// 为开始画对象做准备
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);   // 背景色RGB
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);



	// 签到
	fprintf(fp, "Menu:Initialize\n");
}

void Update3(void)
{
	//(KeyPressed[KeyLeftBottom] && (posX<-160) && (posX>-270) && (posY>-40) && (posY<10))
	// 签到
	//fprintf(fp, "Level1:Update\n");
	// 状态切换
	
	if (KeyPressed[KeyMenu] || (KeyPressed[KeyLeftBottom] && (posX<100) && (posX>-100) && (posY>-250) && (posY<-200)))
		Next = GS_MENU;
	
	// 对象移动
	//不需要运动
	/*
	if (KeyPressed[KeyUp])
	obj1Y += 0.5f;
	else
	if (KeyPressed[KeyDown])
	obj1Y -= 0.5f;
	if (KeyPressed[KeyLeft])
	obj1X -= 0.5f;
	else
	if (KeyPressed[KeyRight])
	obj1X += 0.5f;
	*/

	// 输入重置
	Input_Initialize();

	// 签到
	fprintf(fp, "Level1:Update\n");
}

void Draw3(void)
{

	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // 必须最先设置绘制模式为纹理
	AEGfxSetPosition(obj1X, obj1Y);
	// Set texture for object background
	AEGfxTextureSet(pTex1, 0.0f, 0.0f); // 参数1：纹理，偏移量(x,y)
	AEGfxSetTransparency(1.0f);
	AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
	// Drawing the mesh (list of triangles)
	AEGfxMeshDraw(pMesh1, AE_GFX_MDM_TRIANGLES);

	



	// 签到
	fprintf(fp, "Level1:Draw\n");
}

void Free3(void)
{
	// 签到
	fprintf(fp, "Menu:Free\n");
}

void Unload3(void)
{
	AEGfxMeshFree(pMesh1);		// 注销创建的对象
								// 卸载对象资源	
	AEGfxTextureUnload(pTex1);//卸载纹理
	


							  // 签到
	fprintf(fp, "Menu:Unload\n");
}

//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------
