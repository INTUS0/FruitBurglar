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
#include "Matrix2D.h"
#include "Vector2D.h"
#include "Math2D.h"


//------------------------------------------------------------------------------
// Private Consts:
//------------------------------------------------------------------------------
#define GAME_OBJ_BASE_NUM_MAX	32			// �������ͣ�������ࣩ��Ŀ����
#define GAME_OBJ_NUM_MAX		2048		// ������Ŀ����

#define BURGLAR_INITIAL_NUM			3		// ���ǵ�lives��Ŀ
#define FRUIT_NUM				3		// ˮ����Ŀ
#define BURGLAR_SIZE					30.0f	// ���ǳߴ�
#define BURGLER_ACCEL_FORWARD			50.0f	// ����ǰ����ٶ�(m/s^2)
#define BURGLAR_ACCEL_BACKWARD			-100.0f	// ���Ǻ�����ٶ�(m/s^2)

#define FLAG_ACTIVE					0x00000001  // ������־

//------------------------------------------------------------------------------
// Private Structures:
//------------------------------------------------------------------------------

//��Ϸ�������
typedef struct
{
	unsigned long		type;		// ��Ϸ��������
	AEGfxVertexList*	pMesh;		// ��״
}GameObjBase;

//��Ϸ������
typedef struct
{
	GameObjBase*       pObject;     //ָ�����
	unsigned long      flag;        //���־
	float              scale;       //�ߴ�
	Vector2D           posCurr;     //��ǰλ��
	Vector2D           velCurr;     //��ǰ�ٶ�
	float              dirCurr;     //��ǰ����

	AEMtx33            transform;   //�任����ÿһ֡����ҪΪһ���������
}GameObj;


//private variables
// ��Ϸ������ࣨ���ͣ��б�

static GameObjBase		sGameObjBaseList[GAME_OBJ_BASE_NUM_MAX];	// �������е�Ԫ������Ϸ��������ʵ������״������
static unsigned long	sGameObjBaseNum;							// �Ѷ������Ϸ�������

// ��Ϸ�����б�
static GameObj			sGameObjList[GAME_OBJ_NUM_MAX];				// �������е�Ԫ������Ϸ�����ʵ��
static unsigned long	sGameObjNum;								// ��Ϸ����ĸ���

//С������,player
static GameObj* Burglar;
//Burglar = (GameObj*)malloc(sizeof(GameObj));

//С��Ѫ��
static int BurglarBlood;
static unsigned long sScore; //�񵽵�ˮ����

//ˮ������
static int Fruit_NUM = 20;

//������
static int DOG_NUM = 5;

//ũ����Ѫ��
static int BossBlood;


//------------------------------------------------------------------------------
// Private Variables monkey:
//------------------------------------------------------------------------------
static AEGfxTexture *pTex1;		// ����Burglar������
static AEGfxVertexList*	pMesh1;				// ����1������(mesh)ģ��
static float obj1X, obj1Y;		// ����1��λ��

static AEGfxTexture *pTex2;		//����dog������

//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------
// ����/ɾ����Ϸ����
static GameObj*		gameObjCreate(unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir);
static void			gameObjDestroy(GameObj* pInst);

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

void Load1(void)
{


	GameObjBase* pObjBase;

	// ��ʼ����Ϸ��������ʵ���б�
	memset(sGameObjBaseList, 0, sizeof(GameObjBase) * GAME_OBJ_BASE_NUM_MAX);
	sGameObjBaseNum = 0;

	// ���������ʵ��	
	// =====================
	//����
	// =====================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLAR;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-30.0f, -30.0f, 0x00FF00FF, 0.0f, 1.0f,
		30.0f, -30.0f, 0x00FFFF00, 1.0f, 1.0f,
		-30.0f, 30.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		30.0f, -30.0f, 0x00FFFFFF, 1.0f, 1.0f,
		30.0f, 30.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-30.0f, 30.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create object!!");
	pTex1 = AEGfxTextureLoad("MonkeyStand.png");//��������

	// =======================
	// ʯͷ���ߴ��С���򻯳������ζ���
	// =======================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_STONE;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f,
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,
		0.5f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);

	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Bullet object!!");

	// =========================
	// ���壺������������ƴ��һ����Բ�Ρ�
	// =========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_TRAP;

	AEGfxMeshStart();
	AEGfxTriAdd(
		0.0f, 0.0f, 0x010000FF, 0.0f, 0.0f,
		0.5f, 0.0f, 0xFF0000FF, 0.0f, 0.0f,
		0.25f, 0.4f, 0xFF0000FF, 0.0f, 0.0f);
	AEGfxTriAdd(
		0.0f, 0.0f, 0x010000FF, 0.0f, 0.0f,
		0.25f, 0.4f, 0xFF0000FF, 0.0f, 0.0f,
		-0.25f, 0.4f, 0xFF0000FF, 0.0f, 0.0f);
	AEGfxTriAdd(
		0.0f, 0.0f, 0x010000FF, 0.0f, 0.0f,
		-0.5f, 0.0f, 0xFF0000FF, 0.0f, 0.0f,
		-0.25f, 0.4f, 0xFF0000FF, 0.0f, 0.0f);
	AEGfxTriAdd(
		0.0f, 0.0f, 0x010000FF, 0.0f, 0.0f,
		0.5f, 0.0f, 0xFF0000FF, 0.0f, 0.0f,
		0.25f, -0.4f, 0xFF0000FF, 0.0f, 0.0f);
	AEGfxTriAdd(
		0.0f, 0.0f, 0x010000FF, 0.0f, 0.0f,
		0.25f, -0.4f, 0xFF0000FF, 0.0f, 0.0f,
		-0.25f, -0.4f, 0xFF0000FF, 0.0f, 0.0f);
	AEGfxTriAdd(
		0.0f, 0.0f, 0x010000FF, 0.0f, 0.0f,
		-0.5f, 0.0f, 0xFF0000FF, 0.0f, 0.0f,
		-0.25f, -0.4f, 0xFF0000FF, 0.0f, 0.0f);

	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// ũ����������������ƴ�ӵ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BOSS;

	AEGfxMeshStart();
	AEGfxTriAdd(
		0.5f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, 0.5f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, -0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	AEGfxTriAdd(
		-0.5f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, 0.5f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, -0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// ��������������ƴ�ӵ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_DOG;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-50.0f, -30.0f, 0x00FF00FF, 0.0f, 1.0f,
		50.0f, -30.0f, 0x00FFFF00, 1.0f, 1.0f,
		-50.0f, 30.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		50.0f, -30.0f, 0x00FFFFFF, 1.0f, 1.0f,
		50.0f, 30.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-50.0f, 30.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");
	pTex2 = AEGfxTextureLoad("Dog1.png");//��������
	// ========================
	// ��ݮ������������ƴ�ӵ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_STRAWBERRY;

	AEGfxMeshStart();
	AEGfxTriAdd(
		0.5f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, 0.5f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, -0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	AEGfxTriAdd(
		-0.5f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, 0.5f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, -0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// ���ϣ�����������ƴ�ӵ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_WATERMELON;

	AEGfxMeshStart();
	AEGfxTriAdd(
		0.5f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, 0.5f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, -0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	AEGfxTriAdd(
		-0.5f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, 0.5f, 0xFFFFFF00, 0.0f, 0.0f,
		0.0f, -0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");


	// ǩ��
	fprintf(fp, "Level1:Load\n");




}

void Ini1(void)
{

	// ����1�ĳ�ʼλ��
	obj1X = 0.0f;
	obj1Y = 0.0f;

	GameObj* pObj;
	int i;

	// Ϊ��ʼ��������׼��
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// ����ʵ��������Ϸ��ʼֻ��С���͹���Ҫʵ����
	// С������ʵ����
	Burglar = gameObjCreate(TYPE_BURGLAR, BURGLAR_SIZE, 0, 0, 0.0f);
	AE_ASSERT(Burglar);
	Burglar->posCurr.x = AEGfxGetWinMaxX();
	Burglar->posCurr.y = 100.0f;
	Burglar->dirCurr = acosf(Burglar->posCurr.x / ((float)sqrt(Burglar->posCurr.x*Burglar->posCurr.x + Burglar->posCurr.y * Burglar->posCurr.y))) - PI;
	Burglar->scale = 10.0f;

	//������ʵ���� �� ��ʼ��
	for (i = 0; i < DOG_NUM; i++)
	{
		// ʵ����
		pObj = gameObjCreate(TYPE_DOG, 20.0f, 0, 0, 0.0f);
		AE_ASSERT(pObj);

		// ��ʼ��: ����λ�� ����ͳߴ��С
		switch (i)
		{
		case 0:
			pObj->posCurr.x = AEGfxGetWinMaxX() - 30;
			pObj->posCurr.y = 100.0f;
			break;
		case 1:
			pObj->posCurr.x = 100.0f;
			pObj->posCurr.y = AEGfxGetWinMaxY() - 20;
			break;
		case 2:
			pObj->posCurr.x = AEGfxGetWinMinX() + 30;
			pObj->posCurr.y = 50.0f;
			break;
		case 3:
			pObj->posCurr.x = 300.0f;
			pObj->posCurr.y = AEGfxGetWinMaxY() - 40;
			break;
		case 4:
			pObj->posCurr.x = AEGfxGetWinMinX() + 20;
			pObj->posCurr.y = 200.0f;
			break;
		}

		pObj->dirCurr = acosf(pObj->posCurr.x / ((float)sqrt(pObj->posCurr.x*pObj->posCurr.x + pObj->posCurr.y * pObj->posCurr.y))) - PI;

		pObj->scale = 10.0f;
	}
}

void Update1(void)
{
	//unsigned long i;
	float winMaxX, winMaxY, winMinX, winMinY;
	double frameTime;

	
	GameObj* pObj;//ָ�򹷵�ָ��

	// ==========================================================================================
	// ��ȡ���������ߵ����꣬�����ڷ����ƶ������ţ�����ֵ�ᷢ���仯
	// ==========================================================================================
	winMaxX = AEGfxGetWinMaxX();
	winMaxY = AEGfxGetWinMaxY();
	winMinX = AEGfxGetWinMinX();
	winMinY = AEGfxGetWinMinY();

	// ======================
	// ֡ʱ�䣺�൱��zero�е�dt
	// ======================
	frameTime = AEFrameRateControllerGetFrameTime();

	// =========================
	// ��Ϸ�߼���Ӧ����
	// =========================

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
	if (KeyPressed[KeyUp] || KeyPressed[KeyLeftBottom])
	{
		obj1Y += 20.0f;
		
	}
	else
		if (KeyPressed[KeyDown])
		{
			obj1Y -= 20.0f;
			
		}
	if (KeyPressed[KeyLeft])
	{
		obj1X -= 20.0f;
		
	}
	else
		if (KeyPressed[KeyRight])
		{
			obj1X += 20.0f;
			
		}
	if (KeyPressed[KeyRightBottom])
	{
	obj1X = posX;
	obj1Y = posY;//����Ҽ����긳������
	}

	//����Ƿ�������ײ
	for (int i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{
		GameObj* pObj = sGameObjList + i;
		if (pObj->flag)
		{
			if (i % 2)
			{
				pObj->velCurr.x = 0.05;
				pObj->velCurr.y = -0.05;
			}
			else
			{
				pObj->velCurr.x = -0.05;
				pObj->velCurr.y = -0.05;
			}


			
			pObj->posCurr.x += pObj->velCurr.x;
			pObj->posCurr.y += pObj->velCurr.y;
		}
		if (StaticRectToStaticRect(&Burglar->posCurr, 30, 30, &pObj->posCurr, 20, 20)&&pObj->flag )
			gameObjDestroy(Burglar);
	}


	// ��������
	Input_Initialize();

	// ǩ��
	fprintf(fp, "Level1:Update\n");




}

void Draw1(void)
{
	unsigned long i;
	// ������1
	
	if (Burglar->flag& FLAG_ACTIVE)
	{
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetPosition(obj1X, obj1Y);
		AEGfxTextureSet(pTex1, obj1X, obj1Y);//���ú���λ��
		//���º���λ��
		Burglar->posCurr.x = obj1X;
		Burglar->posCurr.y = obj1Y;

		AEGfxSetTransparency(1.0f);
		AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
		AEGfxMeshDraw(Burglar->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
	}

	// ������ƶ����б��е����ж���
	for (i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{
		GameObj* pInst = sGameObjList + i;

		// �����ǻ����
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		// ���û���ģʽ(Color or texture)
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		// ���ù�������λ��
		AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
		// ������
		AEGfxTextureSet(pTex2, 0.0f, 0.0f);
		// ������
		AEGfxSetTransparency(1);
		AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
		AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
	}
	// ǩ��
	fprintf(fp, "Level1:Draw\n");
}

void Free1(void)
{
	gameObjDestroy(Burglar);
	// ǩ��
	fprintf(fp, "Level1:Free\n");
}

void Unload1(void)
{
	AEGfxMeshFree(Burglar->pObject->pMesh);		// ע�������Ķ���
	//AEGfxTextureUnload(pTex1); //���ٺ���

	// ǩ��
	fprintf(fp, "Level1:Unload\n");
}

GameObj* gameObjCreate(unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir)
{
	unsigned long i;
	Vector2D zero = { 0.0f, 0.0f };

	AE_ASSERT_PARM(type < sGameObjBaseNum);

	// ����Ϸ�����б���û�ù���λ��
	for (i = 0; i < GAME_OBJ_NUM_MAX; i++)
	{
		GameObj* pInst = sGameObjList + i;

		// �ҷǻ�����λ��
		if (pInst->flag == 0)
		{
			// �ҵ���
			pInst->pObject = sGameObjBaseList + type;
			pInst->flag = FLAG_ACTIVE;
			pInst->scale = scale;
			pInst->posCurr = pPos ? *pPos : zero;
			pInst->velCurr = pVel ? *pVel : zero;
			pInst->dirCurr = dir;

			// �����´����Ķ���ʵ��
			return pInst;
		}
	}

	// λ������
	return 0;
}

// ---------------------------------------------------------------------------

void gameObjDestroy(GameObj* pInst)
{
	// ���Ѿ�����
	if (pInst->flag == 0)
		return;

	// ����
	pInst->flag = 0;
}