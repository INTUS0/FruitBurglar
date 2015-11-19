/* Project:		GSMWinStep
File Name:	Menu.c
Author KY
Date:		2015-11-19
Purpose:		�ؿ�1  */

#include <stdio.h>
#include "GameStateList.h"
#include "System.h"
#include "Menu.h"
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


static AEGfxVertexList*	pMesh1;				// ����1������(mesh)ģ��
static float obj1X, obj1Y;		// ����1��λ��


//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

void LoadMenuByIntUs(void)
{
	// ��ʼ��Ӷ���1
	AEGfxMeshStart();
	/*
	AEGfxTriAdd(
		-25.5f, -25.5f, 0xFFFF0000, 0.0f, 0.0f,
		25.5f, 0.0f, 0x00FF0000, 0.0f, 0.0f,
		-25.5f, 25.5f, 0xFFFFFF00, 0.0f, 0.0f);

	*/
	pMesh1 = AEGfxMeshEnd();
	AE_ASSERT_MESG(pMesh1, "Failed to create mesh 1!!");

	// ǩ��
	fprintf(fp, "Menu:Load\n");
}

void IniMenu(void)
{
	// ����1�ĳ�ʼλ��
	obj1X = 0.0f;
	obj1Y = 0.0f;

	// Ϊ��ʼ��������׼��
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);   // ����ɫRGB
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// ǩ��
	fprintf(fp, "Menu:Initialize\n");
}

void UpdateMenu(void)
{
	// ǩ��
	//fprintf(fp, "Level1:Update\n");
	// ״̬�л�
	if (KeyPressed[KeyR])
		Next = GS_Restart;
	if (KeyPressed[KeyESC])
		Next = GS_Quit;
	if (KeyPressed[Key1])
		Next = GS_L1;

	// �����ƶ�
	//����Ҫ�˶�
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

	// ��������
	Input_Initialize();

	// ǩ��
	fprintf(fp, "Level1:Update\n");
}

void DrawMenu(void)
{
	// ������1
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetPosition(obj1X, obj1Y);
	AEGfxTextureSet(NULL, 0, 0);
	AEGfxSetTransparency(1);
	AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
	AEGfxMeshDraw(pMesh1, AE_GFX_MDM_TRIANGLES);

	// ǩ��
	fprintf(fp, "Level1:Draw\n");
}

void FreeMenu(void)
{
	// ǩ��
	fprintf(fp, "Menu:Free\n");
}

void UnloadMenu(void)
{
	AEGfxMeshFree(pMesh1);		// ע�������Ķ���

	// ǩ��
	fprintf(fp, "Menu:Unload\n");
}

//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------
