/* Project:		GSMWinStep
File Name:	Level1.c
Author(former):		����
Author KY
Date:		2015-9-15
Purpose:		�ؿ�1  */

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
static AEGfxTexture *pTex1;		// ����1������
static AEGfxVertexList*	pMesh1;				// ����1������(mesh)ģ��
static float obj1X, obj1Y;		// ����1��λ��
//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

void Load1(void)
{
	// ��ʼ��Ӷ���1
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

	pTex1 = AEGfxTextureLoad("MonkeyStand.png");//��������

	// ǩ��
	fprintf(fp, "Level1:Load\n");
}

void Ini1(void)
{
	// ����1�ĳ�ʼλ��
	obj1X = 0.0f;
	obj1Y = 0.0f;

	// Ϊ��ʼ��������׼��
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);   // ����ɫRGB
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// ǩ��
	fprintf(fp, "Level1:Initialize\n");
}

void Update1(void)
{
	// ǩ��
	//fprintf(fp, "Level1:Update\n");
	// ״̬�л�
	if (KeyPressed[KeyR])
		Next = GS_Restart;
	if (KeyPressed[KeyESC])
		Next = GS_Quit;
	if (KeyPressed[Key2])
		Next = GS_L2;
	if (KeyPressed[KeyMenu])
		Next = GS_MENU;


	// �����ƶ�
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
	
	
	// ��������
	Input_Initialize();

	// ǩ��
	fprintf(fp, "Level1:Update\n");
}

void Draw1(void)
{
	// ������1
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	AEGfxSetPosition(obj1X, obj1Y);
	AEGfxTextureSet(pTex1, obj1X, obj1Y);//���ú���λ��
	AEGfxSetTransparency(1.0f);
	AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
	AEGfxMeshDraw(pMesh1, AE_GFX_MDM_TRIANGLES);

	// ǩ��
	fprintf(fp, "Level1:Draw\n");
}

void Free1(void)
{
	// ǩ��
	fprintf(fp, "Level1:Free\n");
}

void Unload1(void)
{
	AEGfxMeshFree(pMesh1);		// ע�������Ķ���
	AEGfxTextureUnload(pTex1); //���ٺ���

	// ǩ��
	fprintf(fp, "Level1:Unload\n");
}

//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------
