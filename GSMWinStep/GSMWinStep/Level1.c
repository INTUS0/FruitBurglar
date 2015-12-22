/* Project:		GSMWinStep
File Name:	Level1.c
Author(former):		����
Author  INT US
Date:		2015-9-15 -- 2015-1-20
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

	Matrix2D           transform;   //�任����ÿһ֡����ҪΪһ���������
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

//ʯͷ����
GameObj* pStone;

//ũ��������
GameObj* pBoss;


//С��Ѫ��
static int BurglarBlood;
static unsigned long sScore; //�񵽵�ˮ����

//ˮ������
static int Fruit_NUM =0;
//ˮ������ʱ����
static int TimeTot = 0;
//��ʱ���幷�˶�ʱ��
static int TimeTot1 = 0;

//������
static int DOG_NUM = 5;

//ũ����Ѫ��
static int BossBlood;

//�񵽵�ʯͷ����
static int StoneCount=0;

//֡�ʿ��Ƴ���
static int AnimationCounter = 0; // ���Ʋ���֡
static int AnimationFPS = 12;
static float ElapsedTime = 0.0f;


//------------------------------------------------------------------------------
// Private Variables monkey:
//------------------------------------------------------------------------------
static AEGfxTexture *pTex1;		// ����Burglar������
static AEGfxVertexList*	pMesh1;	// ����Burglar������(mesh)ģ��
static float obj1X, obj1Y;		// ����burglar��λ��

static AEGfxTexture *pTex2;		//����dog������
static AEGfxTexture *pTexStone; //�ƶ�ʯͷ����

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
		-20.0f, -20.0f, 0x00FF00FF, 0.0f, 1.0f,
		20.0f, -20.0f, 0x00FFFF00, 0.125f, 1.0f,
		-20.0f, 20.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		20.0f, -20.0f, 0x00FFFFFF, 0.125f, 1.0f,
		20.0f, 20.0f, 0x00FFFFFF, 0.125f, 0.0f,
		-20.0f, 20.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create object!!");
	pTex1 = AEGfxTextureLoad("PlayerRun.png");//��������

	// =======================
	// �����ȥ��ʯͷ���ߴ��С���򻯳������ζ���
	// =======================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_STONE;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-10.0f, -10.0f, 0x00FF00FF, 0.0f, 1.0f,
		10.0f, -10.0f, 0x00FFFF00, 1.0f, 1.0f,
		-10.0f, 10.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		10.0f, -10.0f, 0x00FFFFFF, 1.0f, 1.0f,
		10.0f, 10.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-10.0f, 10.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Bullet object!!");
	pTexStone = AEGfxTextureLoad("planetTexture.png");


	// =======================
	// ��ֹ�������ʯͷ���ߴ��С���򻯳������ζ���
	// =======================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_STONE_STATIC;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-10.0f, -10.0f, 0x00FF00FF, 0.0f, 1.0f,
		10.0f, -10.0f, 0x00FFFF00, 1.0f, 1.0f,
		-10.0f, 10.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		10.0f, -10.0f, 0x00FFFFFF, 1.0f, 1.0f,
		10.0f, 10.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-10.0f, 10.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Bullet object!!");
	pTexStone = AEGfxTextureLoad("planetTexture.png");

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
		-30.0f, -30.0f, 0x00FF00FF, 0.0f, 1.0f,
		30.0f, -30.0f, 0x00FFFF00, 1.0f, 1.0f,
		-30.0f, 30.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		30.0f, -30.0f, 0x00FFFFFF, 1.0f, 1.0f,
		30.0f, 30.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-30.0f, 30.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// ��������������ƴ�ӵ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_DOG;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-25.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		25.0f, -15.0f, 0x00FFFF00, 1.0f, 1.0f,
		-25.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		25.0f, -15.0f, 0x00FFFFFF, 1.0f, 1.0f,
		25.0f, 15.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-25.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
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
	// ���ϣ�Բ��
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_WATERMELON;

	AEGfxMeshStart();
	float CircleAngleStep = PI / 12.0f;
	int Parts = 24;
	for (int i = 0; i < Parts; ++i)
	{
		AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf(i * 2 * PI / Parts)*10.0f, sinf(i * 2 * PI / Parts)*10.0f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf((i + 1) * 2 * PI / Parts)*10.0f, sinf((i + 1) * 2 * PI / Parts)*10.0f, 0xFFFFFF00, 0.0f, 0.0f);
	}
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");


	// ========================
	// BOSSѪ��������������ƴ�ӵĳ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BOSSBLOOD;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-20.0f, 4.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 ��ɫ
		20.0f, 6.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 ��ɫ
		-20.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF ��ɫ
	AEGfxTriAdd(
		20.0f, -2.0f, 0x01FF0000, 1.0f, 1.0f,
		20.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,
		-20.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// С��Ѫ��������������ƴ�ӵĳ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARBLOOD;

	AEGfxMeshStart();
	AEGfxTriAdd(
	    -20.0f, 4.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 ��ɫ
		28.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 ��ɫ
		-20.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF ��ɫ
	AEGfxTriAdd(
		16.0f, -4.0f, 0x01FF0000, 1.0f, 1.0f,
		20.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,
		-20.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// С��������������������ƴ�ӵĳ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARLIVES;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-30.0f, 4.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 ��ɫ
		30.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 ��ɫ
		-30.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF ��ɫ
	AEGfxTriAdd(
		30.0f, -4.0f, 0x01FF0000, 1.0f, 1.0f,
		30.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,
		-30.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);
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
	memset(sGameObjList, 0, sizeof(GameObj)*GAME_OBJ_NUM_MAX);
	GameObj* pObj;
	int i;

	BossBlood = 2;      //��ʼ��BOSSѪ��

	// Ϊ��ʼ��������׼��
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// ����ʵ��������Ϸ��ʼֻ��С���͹���ũ������Ҫʵ����
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
		pObj = gameObjCreate(TYPE_DOG, 10.0f, 0, 0, 0.0f);
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

	//��ʼ��ũ����
	pBoss = gameObjCreate(TYPE_BOSS, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pBoss);
	//��ʼ��ũ����λ�ü��������
	pBoss->posCurr.x = 100.0f;
	pBoss->posCurr.y = 100;
	pBoss->dirCurr = acosf(pBoss->posCurr.x / ((float)sqrt(pBoss->posCurr.x*pBoss->posCurr.x + pBoss->posCurr.y * pBoss->posCurr.y))) - PI;

	pBoss->scale = 10.0f;

	//ũ����Ѫ����ʼ��
	pObj = gameObjCreate(TYPE_BOSSBLOOD, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//��ʼ��Ѫ��λ��
	pObj->posCurr.x = pBoss->posCurr.x;
	pObj->posCurr.y = pBoss->posCurr.y + 35.0f;


	//��ʼ����ֹ��ʯͷ
	for (i = 0; i < 3; i++)
	{
		pObj = gameObjCreate(TYPE_STONE_STATIC, 3.0f, 0, 0, 0.0f);;
		AE_ASSERT(pObj);
		// ʵ����
		// ��ʼ��: ����λ�� ����ͳߴ��С
		switch (i)
		{
		case 0:
			pObj->posCurr.x = AEGfxGetWinMaxX() - 50;
			pObj->posCurr.y = 100.0f;
			break;
		case 1:
			pObj->posCurr.x = 100.0f;
			pObj->posCurr.y = AEGfxGetWinMaxY() - 30;
			break;
		case 2:
			pObj->posCurr.x = AEGfxGetWinMinX() + 40;
			pObj->posCurr.y = 50.0f;
			break;
		}
		pObj->dirCurr = acosf(pObj->posCurr.x / ((float)sqrt(pObj->posCurr.x*pObj->posCurr.x + pObj->posCurr.y * pObj->posCurr.y))) - PI;

		pObj->scale = 5.0f;
	}
	
	
}

void Update1(void)
{
	//unsigned long i;
	float winMaxX, winMaxY, winMinX, winMinY;
	double frameTime;


	//GameObj* pObj;//ָ�򹷵�ָ��

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

	//����Ҽ�����ʯͷ����
	if ((KeyPressed[KeyRightBottom]) && (Burglar->flag& FLAG_ACTIVE) &&StoneCount)
	{
		//obj1X = posX;
		//obj1Y = posY;//����Ҽ����긳��ʯͷ
		pStone = gameObjCreate(TYPE_STONE, 3.0f, 0, 0, 0.0f);;
		AE_ASSERT(pStone);
		// ʵ����

		// ��ʼ��: ����λ�� ����ͳߴ��С
		pStone->posCurr.x = Burglar->posCurr.x ;
		pStone->posCurr.y = Burglar->posCurr.y;
		

		//pStone->dirCurr = acosf(pStone->posCurr.x / ((float)sqrt(pStone->posCurr.x*pStone->posCurr.x + pStone->posCurr.y * pStone->posCurr.y))) - PI;
		//pStone->scale = 5.0f;

		//ʯͷ��һ
		StoneCount -= 1;	
	}


	//�������ˮ����ÿ��3�����һ����һ������������10��ˮ��
	TimeTot++;
	TimeTot1++;
	if (TimeTot == 180 && Fruit_NUM<10)
	{
		Fruit_NUM++;
		int Xx, Yy;
		Xx = rand();
		Yy = rand();

		//����ˮ���ڽ����ڲ���
		while (Xx > 800.0f) //����ˮ���ڽ������ڲ���
		{
			Xx = (int)rand();
		}
		while (Yy > 600.0f)//����ˮ���ڽ���߶ȶ��ڲ���
		{
			Yy = (int)rand();
		}
		Xx = Xx - 400;
		Yy = Yy - 300;

		TimeTot = 0;
		GameObj* pFruit = gameObjCreate(TYPE_WATERMELON, 3.0f, 0, 0, 0.0f);;
		AE_ASSERT(pFruit);
		// ʵ����

		// ��ʼ��: ����λ�� ����ͳߴ��С
		pFruit->posCurr.x = Xx;
		pFruit->posCurr.y = Yy;

		pFruit->scale = 5.0f;
	}
	else if (TimeTot == 200) TimeTot = 0;


	
	//�������ж����Ծ�������
	for (int i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{

		GameObj* pObj = sGameObjList + i;


		if (pObj->flag && pObj->pObject->type == TYPE_BURGLARBLOOD)
		{
			//����Ѫ��λ��
			pObj->posCurr.x = Burglar->posCurr.x;
			pObj->posCurr.y = Burglar->posCurr.y + 35.0f;

			if (!Burglar->flag&FLAG_ACTIVE)
			{
				gameObjDestroy(pObj);   //�������ǵ�ͬʱ���������ǵ�Ѫ�� 
			}
		}

		if (pObj->flag&&pObj->pObject->type == TYPE_DOG)
		{
			//�����˶�
			if (pObj->posCurr.x < winMaxX && pObj->posCurr.x > winMaxX-50)
			{
				pObj->velCurr.x = -1.5;
			}
			else if (pObj->posCurr.x > winMinX  && pObj->posCurr.x < winMinX + 50)
			{
				pObj->velCurr.x = 1.5;
				
			}
			else if (TimeTot1 % 360 < 180)
			{
				pObj->velCurr.x = rand() % 3;
			}
			else
			{
				pObj->velCurr.x =  -rand() % 3;
			}

			if (pObj->posCurr.y < winMinY + 50 && pObj->posCurr.y > winMinY )
			{
				pObj->velCurr.y = 1.5;
			}
			else if (pObj->posCurr.y >winMaxY - 50 && pObj->posCurr.y <winMaxY )
			{
				pObj->velCurr.y = -5;
			}
			else if (TimeTot1 % 360 <180 )
			{
				pObj->velCurr.y = -rand() % 3;
			}
			else
			{
				pObj->velCurr.y = rand() % 3;
			}
			
		
			pObj->posCurr.x += pObj->velCurr.x;
			pObj->posCurr.y += pObj->velCurr.y;

			//�Ƿ��빷������ײ
			if (StaticRectToStaticRect(&Burglar->posCurr, 30, 30, &pObj->posCurr, 30, 30) && pObj->flag)
			{
				Burglar->scale -= 0.5;
			}
			if (Burglar->scale < 1.0f)
			{
				gameObjDestroy(Burglar);//��ײ�ˣ�����scale�������ٶ���
			}
		}


		//��ʯͷ
		if (pObj->flag&&pObj->pObject->type == TYPE_STONE_STATIC)
		{
			if (StaticRectToStaticRect(&Burglar->posCurr, 33, 30, &pObj->posCurr, 15, 15) )
			{
				gameObjDestroy(pObj);
				StoneCount += 1;
			}
		}

		//ʯͷ�ķ���
		if (pObj->flag && pObj->pObject->type == TYPE_STONE)
		{
			//ʯͷ�������λ���˶�
			pStone->velCurr.x = (posX - Burglar->posCurr.x) / 10.0f;
			pStone->velCurr.y = (posY - Burglar->posCurr.y) / 10.0f;

			pObj->posCurr.x += pObj->velCurr.x ;
			pObj->posCurr.y += pObj->velCurr.y ;

			//��ײ��������boss
			for (int j = 1; j < GAME_OBJ_NUM_MAX; j++)
			{

				GameObj* pObj0 = sGameObjList + j;
				if (pObj0->flag && (pObj0->pObject->type == TYPE_DOG || pObj0->pObject->type == TYPE_BOSS))
				{
					if (StaticPointToStaticCircle(&pObj->posCurr, &pObj0->posCurr, 10.0))
					{
						gameObjDestroy(pObj0);
						gameObjDestroy(pObj);
					}
				}
			}
		}

		//������
		if (pObj->flag&&pObj->pObject->type == TYPE_WATERMELON)
		{
			if (StaticPointToStaticCircle(&Burglar->posCurr, &pObj->posCurr, 10.0))
			{
				gameObjDestroy(pObj);
				Fruit_NUM -= 1;
				sScore += 10;
			}
		}


		//Boss ���˶�����ײ
		if (pObj->flag && pObj->pObject->type ==TYPE_BOSS)
		{
			pObj->velCurr.x = Burglar->posCurr.x - pObj->posCurr.x/20.0f;
			pObj->velCurr.y = Burglar->posCurr.y - pObj->posCurr.y/20.0f;

			if (pObj->posCurr.x == Burglar->posCurr.x)
			{
				pObj->velCurr.y = 1.0f;
			}
			if (pObj->posCurr.y == Burglar->posCurr.y)
			{
				pObj->velCurr.x = 1.0f;
			}
			//ũ�������������ƶ�
			pObj->posCurr.x += pObj->velCurr.x / 100.0f;
			pObj->posCurr.y += pObj->velCurr.y / 100.0f;

			
			//������ײ����������
			if (StaticRectToStaticRect(&Burglar->posCurr, 15.0f, 15.0f, &pObj->posCurr, 15.0f, 15.0f))
			{
				gameObjDestroy(Burglar);
			}
		}
	}

	//���ո��
	if (KeyPressed[KeySpace])
	{
		/*
		pStone = gameObjCreate(TYPE_STONE, 10.0f, 0, 0, 0.0f);;
		AE_ASSERT(pStone);

		// ʵ����

		// ��ʼ��: ����λ�� ����ͳߴ��С
		pStone->posCurr.x = (Burglar->posCurr.x+100.0f);
		pStone->posCurr.y = (Burglar->posCurr.y+10.0f);

		pStone->dirCurr = acosf(pStone->posCurr.x / ((float)sqrt(pStone->posCurr.x*pStone->posCurr.x + pStone->posCurr.y * pStone->posCurr.y))) - PI;

		pStone->scale = 5.0f;
	*/
	}
	// ��������
	Input_Initialize();

	// ǩ��
	fprintf(fp, "Level1:Update\n");

}

void Draw1(void)
{
	unsigned long i;

	// �����ǣ�������
	if (Burglar->flag& FLAG_ACTIVE)
	{
		//�����ƶ�֡��ʱ��
		float frameTime = AEFrameRateControllerGetFrameTime()*10.0f;

		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetPosition(obj1X, obj1Y);
		AEGfxTextureSet(pTex1, obj1X, obj1Y);//���ú���λ��
		//���º���λ��,ͬʱ�������λ��
		Burglar->posCurr.x = obj1X;
		Burglar->posCurr.y = obj1Y;
		//������ƶ�
		//AEGfxSetCamPosition(obj1X, obj1Y);

		//ʵ�����ǵĶ�ͼЧ��
		ElapsedTime += frameTime;               // ��Ϸ֡һ֡��ʱ��
		if (ElapsedTime < 1.f / AnimationFPS)	// ���ʱ�仹��������֡һ֡��Ҫ��ʱ��
			return;								// �򷵻�
		else
			ElapsedTime = 0.0f;                 // ��������elapsedTime

		AEGfxTextureSet(pTex1, AnimationCounter*0.125f, 0.0f);
		AnimationCounter += 1;
		if (AnimationCounter > 7)
			AnimationCounter = 0;


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

		//���ƹ�����
		if (pInst->pObject->type == TYPE_DOG)
		{
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// ���ù�������λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ������
			AEGfxTextureSet(pTex2, 0.0f, 0.0f);
			// ������
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//�����ӳ�ȥ��ʯͷ����
		if(( pInst->pObject->type == TYPE_STONE ) && ( Burglar->flag & FLAG_ACTIVE ))
		{
			//if (KeyPressed[KeySpace])
		//	{
				// Drawing object stone
				// Set position for object stone
				AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // �����������û���ģʽΪ����
				//AEGfxSetPosition(Burglar->posCurr.x+100.0f, Burglar->posCurr.y+10.0f);
				// Set texture for object stone
				AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
				AEGfxTextureSet(pTexStone, 0.0f, 0.0f); // ����1������ƫ����(x,y)
				AEGfxSetTransparency(1.0f);
				AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
				// Drawing the mesh (list of triangles)
				AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
			//}
		}
		//���Ʊ���������ʯͷ
		if (pInst->pObject->type == TYPE_STONE_STATIC)
		{
			//if (KeyPressed[KeySpace])
			//	{
			// Drawing object stone
			// Set position for object stone
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // �����������û���ģʽΪ����
			//AEGfxSetPosition(Burglar->posCurr.x+100.0f, Burglar->posCurr.y+10.0f);
			// Set texture for object stone
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			AEGfxTextureSet(pTexStone, 0.0f, 0.0f); // ����1������ƫ����(x,y)
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			// Drawing the mesh (list of triangles)
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
			//}
		}

		//����boss
		if (pInst->pObject->type == TYPE_BOSS)
		{
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// ���ù�������λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ������
			AEGfxTextureSet(pTexStone, 0.0f, 0.0f);
			// ������
			AEGfxSetTransparency(1);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//�������϶���
		if (pInst->pObject->type == TYPE_WATERMELON)
		{
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			// �������ϵ�����λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ������
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// ����������
			//AEGfxSetTransparency(1);
			//AEGfxSetBlendColor(1.0f, 1.0f, 1.0, 1.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//��������Ѫ��
		if (pInst->pObject->type == TYPE_BURGLARBLOOD)
		{
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			// �������ϵ�����λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ������
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// ����������
			//AEGfxSetTransparency(1);
			//AEGfxSetBlendColor(1.0f, 1.0f, 1.0, 1.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}
		
	}
	// ǩ��
	fprintf(fp, "Level1:Draw\n");
}

void Free1(void)
{
	gameObjDestroy(Burglar);
	
	for (int i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{
		GameObj* pObj = sGameObjList + i;
		if (pObj->flag)
			gameObjDestroy(pObj);
	}
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