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
static AEGfxTexture *pTex1;		// ���󱳾�������

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
	AEGfxTriAdd(
		-450.0f, -300.0f, 0x00FF00FF, 0.0f,1.0f,
		450.0f, -300.0f, 0x00FFFF00, 1.0f, 1.0f,
		-450.0f, 300.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		450.0f, -300.0f, 0x00FFFFFF, 1.0f, 1.0f,
		450.0f, 300.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-450.0f, 300.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pMesh1 = AEGfxMeshEnd();
	AE_ASSERT_MESG(pMesh1, "Failed to create mesh 1!!");

	// ��������
	pTex1 = AEGfxTextureLoad("BackGround.PNG");

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
	if (KeyPressed[KeyESC] || (KeyPressed[KeyLeftBottom] && (posX<-160) && (posX>-270) && (posY>-135) && (posY<-85)))
		Next = GS_Quit;
	if (KeyPressed[KeyLeftBottom] && (posX<-70) && (posX>-320) && (posY>60) && (posY<110))
		Next = GS_L1;
	if (KeyPressed[Key2])
		Next = GS_L2;
	if (KeyPressed[KeyLeftBottom] && (posX<-160) && (posX>-270) && (posY > -40) && (posY < 10))
		Next = GS_L3;
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
	/*
	// ������1
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetPosition(obj1X, obj1Y);
	AEGfxTextureSet(NULL, 0, 0);
	AEGfxSetTransparency(1);
	AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
	AEGfxMeshDraw(pMesh1, AE_GFX_MDM_TRIANGLES);
	*/

	// Drawing object background
	// Set position for object background
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // �����������û���ģʽΪ����
	AEGfxSetPosition(0.0f, 0.0f);
	// Set texture for object background
	AEGfxTextureSet(pTex1, 0.0f, 0.0f); // ����1������ƫ����(x,y)
	AEGfxSetTransparency(1.0f);
	AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
	// Drawing the mesh (list of triangles)
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
	// ж�ض�����Դ	
	AEGfxTextureUnload(pTex1);//ж������
	// ǩ��
	fprintf(fp, "Menu:Unload\n");
}

//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------
