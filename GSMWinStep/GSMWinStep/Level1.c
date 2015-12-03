/* Project:		GSMWinStep
File Name:	Level1.c
Author(former):		刘芳
Author KY
Date:		2015-9-15
Purpose:		关卡1  */

#include <stdio.h>
#include "GameStateList.h"
#include "System.h"
#include "Level1.h"
#include "AEEngine.h"
#include "Input.h"

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
static AEGfxTexture *pTex1;		// 对象1的纹理
static AEGfxVertexList*	pMesh1;				// 对象1的网格(mesh)模型
static float obj1X, obj1Y;		// 对象1的位置
//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

void Load1(void)
{
	// 开始添加对象1
	AEGfxMeshStart();
	AEGfxTriAdd(
		-30.0f, -30.0f, 0x00FF00FF, 0.0f, 1.0f,
		30.0f, -30.0f, 0x00FFFF00, 1.0f, 1.0f,
		-30.0f, 30.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		30.0f, -30.0f, 0x00FFFFFF, 1.0f, 1.0f,
		30.0f, 30.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-30.0f, 30.0f, 0x00FFFFFF, 0.0f, 0.0f);
		
	pMesh1 = AEGfxMeshEnd();
	AE_ASSERT_MESG(pMesh1, "Failed to create mesh 1!!");

	pTex1 = AEGfxTextureLoad("MonkeyStand.png");//载入纹理

	// 签到
	fprintf(fp, "Level1:Load\n");
}

void Ini1(void)
{
	// 对象1的初始位置
	obj1X = 0.0f;
	obj1Y = 0.0f;

	// 为开始画对象做准备
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);   // 背景色RGB
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// 签到
	fprintf(fp, "Level1:Initialize\n");
}

void Update1(void)
{
	// 签到
	//fprintf(fp, "Level1:Update\n");
	// 状态切换
	if (KeyPressed[KeyR])
		Next = GS_Restart;
	if (KeyPressed[KeyESC])
		Next = GS_Quit;
	if (KeyPressed[Key2])
		Next = GS_L2;
	if (KeyPressed[KeyMenu])
		Next = GS_MENU;


	// 对象移动
	if (KeyPressed[KeyUp] ||KeyPressed[KeyLeftBottom])
		obj1Y += 2.0f;
	else
		if (KeyPressed[KeyDown] || KeyPressed[KeyRightBottom])
			obj1Y -= 2.0f;
			
	if (KeyPressed[KeyLeft])
		obj1X -= 2.0f;
	else
		if (KeyPressed[KeyRight])
			obj1X += 2.0f;
	
	
	// 输入重置
	Input_Initialize();

	// 签到
	fprintf(fp, "Level1:Update\n");
}

void Draw1(void)
{
	// 画对象1
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	AEGfxSetPosition(obj1X, obj1Y);
	AEGfxTextureSet(pTex1, obj1X, obj1Y);//设置猴子位置
	AEGfxSetTransparency(1.0f);
	AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
	AEGfxMeshDraw(pMesh1, AE_GFX_MDM_TRIANGLES);

	// 签到
	fprintf(fp, "Level1:Draw\n");
}

void Free1(void)
{
	// 签到
	fprintf(fp, "Level1:Free\n");
}

void Unload1(void)
{
	AEGfxMeshFree(pMesh1);		// 注销创建的对象
	AEGfxTextureUnload(pTex1); //销毁猴子

	// 签到
	fprintf(fp, "Level1:Unload\n");
}

//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------
