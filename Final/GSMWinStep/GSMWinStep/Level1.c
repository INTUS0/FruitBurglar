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
#include <Windows.h>
//#include <time.h>
//#pragma comment(lib,"Winmm.lib")

//------------------------------------------------------------------------------
// Private Consts:
//------------------------------------------------------------------------------
#define GAME_OBJ_BASE_NUM_MAX	32			// �������ͣ�������ࣩ��Ŀ����
#define GAME_OBJ_NUM_MAX		4096		// ������Ŀ����

#define BURGLAR_INITIAL_NUM			3		// ���ǵ�lives��Ŀ
#define FRUIT_NUM				3		// ˮ����Ŀ
#define BURGLAR_SIZE					30.0f	// ���ǳߴ�
#define BURGLER_ACCEL_FORWARD			50.0f	// ����ǰ����ٶ�(m/s^2)
#define BURGLAR_ACCEL_BACKWARD			-100.0f	// ���Ǻ�����ٶ�(m/s^2)

#define FLAG_ACTIVE					0x00000001  // ������־

#define CollisionMin                1   //��ͼ�д���1��λ�þ�����ͨ��   
//--------------------------------------------------------------------------------
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
	AEGfxTexture        *pTex;      //����

	Matrix2D           transform;   //�任����ÿһ֡����ҪΪһ���������
}GameObj;


//����ͼ��
typedef struct Mapdata{
	int **mapData, **CollisionData;//��ͼ���ݺ���ײ����
	int BINARY_MAP_HEIGHT, BINARY_MAP_WIDTH;//��ͼ�ĸߺͿ�
}Map;

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
static GameObj* pStone;

//�������
static GameObj* pTrap;

//ˮ��������
static GameObj* pStrawberry;
static GameObj* pWatermelon;
static GameObj* pApple;

//ũ��������
static GameObj* pBoss;

//������
static GameObj* pDog[5];

//��ͼ����
static GameObj* Level1_Map;
Map *pMap;




static int BurglarLives;     //С��������
static int BurglarBlood;     //С��Ѫ��
static unsigned long sScore; //�񵽵�ˮ����

//ˮ������
static int Fruit_NUM = 0;
//ˮ������ʱ����
static int TimeTot = 0;
//��ʱ���幷�˶�ʱ��
static int TimeTot1 = 0;

//BOSS���б��
static int BossDirectionX = 0;
static int BossDirectionY = 0;


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
static float obj2X, obj2Y;
static Vector2D stemp;
static Vector2D stempBoss;
static Vector2D stempBurglar;
static int stateboss = 0;
static int state = 0;
static int state2 = 1;
static int state3 = 0;
static int state4 = 0;
static int state5 = 0;


static AEGfxTexture *pTex2;		//����dog������
static AEGfxTexture *pTexStone; //�ƶ�ʯͷ����
static AEGfxTexture *pTexTrap; //��������
static AEGfxTexture *pTexMap;	//����Map������
static AEGfxTexture *pTex_Bossblood;		//BOSSѪ��������
static AEGfxTexture *pTexStrawberry;		//��ݮ������
static AEGfxTexture *pTexWatermelon;		//���ϵ�����
static AEGfxTexture *pTexApple;		//ƻ��������

static float MapX, MapY;		// ����Map��λ��
//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------
// ����/ɾ����Ϸ����
static GameObj*		gameObjCreate(unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir);
static void			gameObjDestroy(GameObj* pInst);

//��ͼ��ȡ����ͼ����ת��
static void mapLoad(Map* pMap);
int WorldTransToMapdata(Map *pMap, Vector2D *pVec, int choose);

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

void Load1(void)
{
	GameObjBase* pObjBase;

	// ��ʼ����Ϸ��������ʵ���б�
	memset(sGameObjBaseList, 0, sizeof(GameObjBase) * GAME_OBJ_BASE_NUM_MAX);
	sGameObjBaseNum = 0;

	//��ͼ��Ϣ��ȡ
	pMap = (Map *)malloc(sizeof(struct Mapdata));
	mapLoad(pMap);

	BurglarLives = 3;   //��ʼ��С��������

	// ���������ʵ��	
	// =====================
	//����
	// =====================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLAR;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 0.125f, 1.0f,
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 0.125f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 0.125f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
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
		-40.0f, -40.0f, 0x00FF00FF, 0.0f, 1.0f,
		40.0f, -40.0f, 0x00FFFF00, 1.0f, 1.0f,
		-40.0f, 40.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		40.0f, -40.0f, 0x00FFFFFF, 1.0f, 1.0f,
		40.0f, 40.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-40.0f, 40.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Bullet object!!");
	pTexStone = AEGfxTextureLoad("stone.png");


	// =======================
	// ��ֹ�������ʯͷ���ߴ��С���򻯳������ζ���
	// =======================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_STONE_STATIC;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-40.0f, -40.0f, 0x00FF00FF, 0.0f, 1.0f,
		40.0f, -40.0f, 0x00FFFF00, 1.0f, 1.0f,
		-40.0f, 40.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		40.0f, -40.0f, 0x00FFFFFF, 1.0f, 1.0f,
		40.0f, 40.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-40.0f, 40.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Bullet object!!");
	pTexStone = AEGfxTextureLoad("stone.png");

	// =========================
	// ���ü�������
	// =========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_TRAP_IN;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 1.0f, 1.0f,
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 1.0f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);

	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");
	pTexTrap = AEGfxTextureLoad("trap.png");; //��������

	//������������
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_TRAP_OUT;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 1.0f, 1.0f,
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 1.0f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// ũ����������������ƴ�ӵ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BOSS;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-20.0f, -20.0f, 0x00FF00FF, 0.0f, 1.0f,
		20.0f, -20.0f, 0x00FFFF00, 1.0f, 1.0f,
		-20.0f, 20.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		20.0f, -20.0f, 0x00FFFFFF, 1.0f, 1.0f,
		20.0f, 20.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-20.0f, 20.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");
	
	// ========================
	// ��������������ƴ�ӵ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_DOG;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 1.0f, 1.0f,
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 1.0f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
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
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 1.0f, 1.0f,
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 1.0f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");
	pTexStrawberry = AEGfxTextureLoad("straw.png");//��������

	// ========================
	// ƻ��������������ƴ�ӵ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_APPLE;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 1.0f, 1.0f,
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 1.0f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");
	pTexApple = AEGfxTextureLoad("apple.png");//��������

	// ========================
	// ����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_WATERMELON;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 1.0f, 1.0f,
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 1.0f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");
	pTexWatermelon = AEGfxTextureLoad("watermelon.png");//��������


	// ========================
	// BOSSѪ��������������ƴ�ӵĳ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BOSSBLOOD;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-25.0f, 3.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 ��ɫ
		25.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 ��ɫ
		-25.0f, -3.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF ��ɫ
	AEGfxTriAdd(
		25.0f, -3.0f, 0x01FF0000, 1.0f, 1.0f,
		25.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,
		-25.0f, -3.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// С��Ѫ��������������ƴ�ӵĳ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARBLOOD;

	AEGfxMeshStart();
	AEGfxTriAdd(
	    -25.0f, 3.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 ��ɫ
		25.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 ��ɫ
		-25.0f, -3.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF ��ɫ
	AEGfxTriAdd(
		25.0f, -3.0f, 0x01FF0000, 1.0f, 1.0f,
		25.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,
		-25.0f, -3.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// С��������������������ƴ�ӵĳ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARLIVES;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-36.0f, 12.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 ��ɫ
		36.0f, 12.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 ��ɫ
		-36.0f, -12.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF ��ɫ
	AEGfxTriAdd(
		36.0f, -12.0f, 0x01FF0000, 1.0f, 1.0f,
		36.0f, 12.0f, 0xFFFF0000, 1.0f, 0.0f,
		-36.0f, -12.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// =====================
	//��ͼ
	// =====================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_MAP;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-400.0f, -300.0f, 0x00FF00FF, 0.0f, 1.0f,
		400.0f, -300.0f, 0x00FFFF00, 1.0f, 1.0f,
		-400.0f, 300.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		400.0f, -300.0f, 0x00FFFFFF, 1.0f, 1.0f,
		400.0f, 300.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-400.0f, 300.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create object!!");
	pTexMap = AEGfxTextureLoad("map1.png");
	// ========================
	// С������������������ƴ�ӵĳ�����
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARSCORES;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-50.0f, 3.0f, 0xFFFF0000, 0.0f, 0.0f,    //0x01FF0000 ��ɫ
		50.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 ��ɫ
		-50.0f, -3.0f, 0xFFFF0000, 0.0f, 1.0f);   //0xFFFFFFFF ��ɫ
	AEGfxTriAdd(
		50.0f, -3.0f, 0xFFFF0000, 1.0f, 1.0f,
		50.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,
		-50.0f, -3.0f, 0xFFFF0000, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// ʧ��ҳ��
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_LOSE;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-320.0f, 240.0f, 0xFFFF0000, 0.0f, 0.0f,    //0x01FF0000 ��ɫ
		320.0f, 240.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 ��ɫ
		-320.0f, -240.0f, 0xFFFF0000, 0.0f, 1.0f);   //0xFFFFFFFF ��ɫ
	AEGfxTriAdd(
		320.0f, -240.0f, 0xFFFF0000, 1.0f, 1.0f,
		320.0f, 240.0f, 0xFFFF0000, 1.0f, 0.0f,
		-320.0f, -240.0f, 0xFFFF0000, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// ʤ��ҳ��
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_WIN;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-320.0f, 240.0f, 0xFFFF0000, 0.0f, 0.0f,    //0x01FF0000 ��ɫ
		320.0f, 240.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 ��ɫ
		-320.0f, -240.0f, 0xFFFF0000, 0.0f, 1.0f);   //0xFFFFFFFF ��ɫ
	AEGfxTriAdd(
		320.0f, -240.0f, 0xFFFF0000, 1.0f, 1.0f,
		320.0f, 240.0f, 0xFFFF0000, 1.0f, 0.0f,
		-320.0f, -240.0f, 0xFFFF0000, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ǩ��
	fprintf(fp, "Level1:Load\n");




}

void Ini1(void)
{
	
	//ˮ������
	Fruit_NUM = 0;
	//ˮ������ʱ����
	TimeTot = 0;
	//��ʱ���幷�˶�ʱ��
	TimeTot1 = 0;
	

	// ���ǵĳ�ʼλ��
	obj1X = -50.0f;
	obj1Y = 0.0f;
	//��ͼ�ĳ�ʼλ��
	MapX = 0.0f;
	MapY = 0.0f;

	memset(sGameObjList, 0, sizeof(GameObj)*GAME_OBJ_NUM_MAX);
	GameObj* pObj;
	int i;

	BossBlood = 2;      //��ʼ��BOSSѪ��
	pTex_Bossblood = AEGfxTextureLoad("full_live_boss.png");  //����bossѪ������

	BurglarBlood = 3;   //��ʼ��С��Ѫ��

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


	//��ʼ����ֹ��ʯͷ
	for (i = 0; i < 3; i++)
	{
		pObj = gameObjCreate(TYPE_STONE_STATIC, 3.0f, 0, 0, 0.0f);
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

	//��ʼ��������������
	for (int a = 0; a < 3; a++)
	{
		pObj = gameObjCreate(TYPE_TRAP_IN, 10.0f, 0, 0, 0.0f);
		AE_ASSERT(pObj);
		//��ʼ������λ�ü��������

		pObj->posCurr.x = a*100.0f;
		pObj->posCurr.y = -300.0f;
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
	pBoss->pTex = AEGfxTextureLoad("Boss.png");

	//������ʵ���� �� ��ʼ��
	for (i = 0; i < DOG_NUM; i++)
	{
		// ʵ����
		pObj = gameObjCreate(TYPE_DOG, 10.0f, 0, 0, 0.0f);
		pDog[i] = pObj;//��������ָ��
		AE_ASSERT(pObj);

		// ��ʼ��: ����λ�� ����ͳߴ��С
		switch (i)
		{
		case 0:
			pObj->posCurr.x = -100.0f;
			pObj->posCurr.y = -50.0f;
			//pObj->velCurr.x = 2.0f;
			//pObj->velCurr.y = 2.0f;
			break;
		case 1:
			pObj->posCurr.x = -360.0f;
			pObj->posCurr.y = 250.0f;
			//pObj->velCurr.x = -3.0f;
			//pObj->velCurr.y = -2.0f;
			break;
		case 2:
			pObj->posCurr.x = -350.0f;
			pObj->posCurr.y = -250.0f;
			//pObj->velCurr.x = 1.5f;
			//pObj->velCurr.y = -1.5f;
			break;
		case 3:
			pObj->posCurr.x = 350.0f;
			pObj->posCurr.y = 0.0f;
			//pObj->velCurr.x = -3.0f;
			//pObj->velCurr.y = 2.0f;
			break;
		case 4:
			pObj->posCurr.x = -200.0f;
			pObj->posCurr.y = 250.0f;
			//pObj->velCurr.x = -2.0f;
			//pObj->velCurr.y = 2.0f;
			break;
		}

		pObj->dirCurr = acosf(pObj->posCurr.x / ((float)sqrt(pObj->posCurr.x*pObj->posCurr.x + pObj->posCurr.y * pObj->posCurr.y))) - PI;

		pObj->scale = 10.0f;
	}

	
	//ũ����Ѫ����ʼ��
	pObj = gameObjCreate(TYPE_BOSSBLOOD, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//��ʼ��Ѫ��λ��
	pObj->posCurr.x = pBoss->posCurr.x;
	pObj->posCurr.y = pBoss->posCurr.y + 35.0f;

	//����Ѫ����ʼ��
	pObj = gameObjCreate(TYPE_BURGLARBLOOD, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//��ʼ��Ѫ��λ�ü�����
	pObj->posCurr.x = Burglar->posCurr.x;
	pObj->posCurr.y = Burglar->posCurr.y + 35.0f;
	pObj->pTex = AEGfxTextureLoad("life_full.png");

	//����������ʼ��
	pObj = gameObjCreate(TYPE_BURGLARLIVES, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//��ʼ����������ʾλ�ü�����
	pObj->posCurr.x = AEGfxGetWinMinX() + 60.0f;
	pObj->posCurr.y = AEGfxGetWinMaxY() - 15.0f;
	if (BurglarLives == 3)
		pObj->pTex = AEGfxTextureLoad("Burglar_lives3.png");
	else if (BurglarLives == 2)
		pObj->pTex = AEGfxTextureLoad("Burglar_lives2.png");
	else if (BurglarLives == 1)
		pObj->pTex = AEGfxTextureLoad("Burglar_lives1.png");

	//��ʼ�����Ƿ���
	pObj = gameObjCreate(TYPE_BURGLARSCORES, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//��ʼ����������ʾλ��
	pObj->posCurr.x = AEGfxGetWinMinX() + 60.0f;
	pObj->posCurr.y = AEGfxGetWinMaxY() - 40.0f;

	//��ʼ��ʧ��ҳ��
	pObj = gameObjCreate(TYPE_LOSE, 1.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	pObj->posCurr.x = 0.0f;
	pObj->posCurr.y = 0.0f;
	pObj->pTex = AEGfxTextureLoad("gameover.png");

	//��ʼ��ʤ��ҳ��
	pObj = gameObjCreate(TYPE_WIN, 1.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	pObj->posCurr.x = 0.0f;
	pObj->posCurr.y = 0.0f;
	pObj->pTex = AEGfxTextureLoad("win.png");

	// ��ͼ����ʵ����
	Level1_Map = gameObjCreate(TYPE_MAP, 1.0f, 0, 0, 0.0f);
	AE_ASSERT(Level1_Map);
	Level1_Map->posCurr.x = AEGfxGetWinMaxX();
	Level1_Map->posCurr.y = AEGfxGetWinMaxY();

	Level1_Map->dirCurr = acosf(Level1_Map->posCurr.x / ((float)sqrt(Level1_Map->posCurr.x*Level1_Map->posCurr.x + Level1_Map->posCurr.y * Level1_Map->posCurr.y))) - PI;
	Level1_Map->scale = 1.0f;

}

void Update1(void)
{
	//unsigned long i;
	float winMaxX, winMaxY, winMinX, winMinY;
	double frameTime;
	
	

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
	if (BurglarLives <= 0 && KeyPressed[KeyLeftBottom])
	{
		Next = GS_MENU;
	}
	if (sScore >= 50 && KeyPressed[KeyLeftBottom])
	{
		Next = GS_L2;
	}

	obj2X = obj1X;
	obj2Y = obj1Y;

	// ���ǵ��ƶ�
	if (KeyPressed[KeyUp])
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
    
	stempBurglar.x = obj1X;
	stempBurglar.y = obj1Y;

	if (WorldTransToMapdata(pMap,&stempBurglar, 1)==0)
	{
		obj1X = obj2X;
		obj1Y = obj2Y;
	}


	//����������ʯͷ����
	if ((KeyPressed[KeyLeftBottom]) && (Burglar->flag& FLAG_ACTIVE) && (StoneCount>0))
	{
		
		pStone = gameObjCreate(TYPE_STONE, 3.0f, 0, 0, 0.0f);;
		AE_ASSERT(pStone);
		// ʵ����
		// ��ʼ��: ����λ�� ����ͳߴ��С
		float Xx = Burglar->posCurr.x;
		float Yy = Burglar->posCurr.y;
		//������λ�ò������������λ��
		pStone->posCurr.x += Xx;
		pStone->posCurr.y += Yy;
		
		//ʯͷ��һ
		StoneCount -= 1;	
	}


	//�������ˮ����ÿ��3�����һ����һ������������10��ˮ��
	TimeTot++;
	if (TimeTot == 180 && Fruit_NUM<10)
	{
	loop:;

		int Xx, Yy;

		Xx = rand();
		Yy = rand();

		int FlagFruit;

		FlagFruit = ((int)rand())%3;
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
		if (FlagFruit == 2)
		{
			GameObj* pFruit = gameObjCreate(TYPE_WATERMELON, 3.0f, 0, 0, 0.0f);
			// ��ʼ��: ����λ�� ����ͳߴ��С
			pFruit->posCurr.x = Xx;
			pFruit->posCurr.y = Yy;
			//�ж��Ƿ��ܹ�����
			if (WorldTransToMapdata(pMap, &pFruit->posCurr, 1) == 1)
			{
				Fruit_NUM++;
				TimeTot = 0;

				AE_ASSERT(pFruit);
				// ʵ����

				pFruit->scale = 5.0f;
			}
			else
			{
				gameObjDestroy(pFruit);//�����������������
				goto loop;
			}
		}
		else if (FlagFruit == 1)
		{
			GameObj* pFruit = gameObjCreate(TYPE_STRAWBERRY, 3.0f, 0, 0, 0.0f);
			// ��ʼ��: ����λ�� ����ͳߴ��С
			pFruit->posCurr.x = Xx;
			pFruit->posCurr.y = Yy;
			//�ж��Ƿ��ܹ�����
			if (WorldTransToMapdata(pMap, &pFruit->posCurr, 1) == 1)
			{
				Fruit_NUM++;
				TimeTot = 0;

				AE_ASSERT(pFruit);
				// ʵ����

				pFruit->scale = 5.0f;
			}
			else
			{
				gameObjDestroy(pFruit);//�����������������
				goto loop;
			}
		}
		else if (FlagFruit == 0)
		{
			GameObj* pFruit = gameObjCreate(TYPE_APPLE, 3.0f, 0, 0, 0.0f);
			// ��ʼ��: ����λ�� ����ͳߴ��С
			pFruit->posCurr.x = Xx;
			pFruit->posCurr.y = Yy;
			//�ж��Ƿ��ܹ�����
			if (WorldTransToMapdata(pMap, &pFruit->posCurr, 1) == 1)
			{
				Fruit_NUM++;
				TimeTot = 0;

				AE_ASSERT(pFruit);
				// ʵ����

				pFruit->scale = 5.0f;
			}
			else
			{
				gameObjDestroy(pFruit);//�����������������
				goto loop;
			}
		}
	}
	else if (TimeTot == 200) TimeTot = 0;


	
	//�������ж����Ծ�������
	for (int i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{

		GameObj* pObj = sGameObjList + i;

		//ʯͷ�ķ���
		if (pObj->flag && pObj->pObject->type == TYPE_STONE)
		{
			//ʯͷ�������λ���˶�
			pStone->velCurr.x = (posX - Burglar->posCurr.x) / 10.0f;
			pStone->velCurr.y = (posY - Burglar->posCurr.y) / 10.0f;

			pObj->posCurr.x += pObj->velCurr.x;
			pObj->posCurr.y += pObj->velCurr.y;

			//��ײ��������boss
			for (int j = 1; j < GAME_OBJ_NUM_MAX; j++)
			{

				GameObj* pObj0 = sGameObjList + j;
				if (pObj0->flag && pObj0->pObject->type == TYPE_BOSS)
				{
					if (StaticPointToStaticCircle(&pObj->posCurr, &pObj0->posCurr, 10.0))
					{
						gameObjDestroy(pObj);
						BossBlood--;
						pTex_Bossblood = AEGfxTextureLoad("half_live_boss.png");  //����bossѪ������
					}
					if (BossBlood <= 0)
					{
						gameObjDestroy(pObj0);
						for (int k = 0; k < GAME_OBJ_NUM_MAX; k++)
						{     //����bossѪ��
							GameObj* pObj1 = sGameObjList + k;
							if (pObj1->flag && pObj1->pObject->type == TYPE_BOSSBLOOD)
							{
								gameObjDestroy(pObj1);
							}
						}

					}
				}
				else if (pObj0->flag && pObj0->pObject->type == TYPE_DOG)
				{
					if (StaticPointToStaticCircle(&pObj->posCurr, &pObj0->posCurr, 10.0))
					{
						gameObjDestroy(pObj);
						gameObjDestroy(pObj0);
					}
				}
			}
		}

		//��ʯͷ
		if (pObj->flag&&pObj->pObject->type == TYPE_STONE_STATIC)
		{
			if (StaticRectToStaticRect(&Burglar->posCurr, 33, 30, &pObj->posCurr, 15, 15))
			{
				gameObjDestroy(pObj);
				StoneCount += 1;
			}
		}


		//����������ʱ
		if (pObj->flag && pObj->pObject->type == TYPE_TRAP_IN)
		{
			if (Burglar->flag&FLAG_ACTIVE)
			{
				if (((pObj->posCurr.x - Burglar->posCurr.x)*(pObj->posCurr.x - Burglar->posCurr.x) + (pObj->posCurr.y - Burglar->posCurr.y) *(pObj->posCurr.y - Burglar->posCurr.y)) < 4000)
				{
					pTrap = gameObjCreate(TYPE_TRAP_OUT, 10.0f, 0, 0, 0.0f);
					pTrap->posCurr.x = pObj->posCurr.x;
					pTrap->posCurr.y = pObj->posCurr.y;
					pTrap->scale = 10.0;

					if (StaticRectToStaticRect(&Burglar->posCurr, 30, 30, &pObj->posCurr, 40, 40) && pObj->flag)
					{
						Burglar->scale -= 0.5;
					}
					if (Burglar->scale < 1.0f)
					{
						gameObjDestroy(Burglar);//������ײ�����ٶ���
						BurglarLives -= 1;   //������������һ
						if (BurglarLives > 0)
							Next = GS_Restart;
					}
				}
			}

		}
		//Զ������ʱ
		if (pObj->flag && pObj->pObject->type == TYPE_TRAP_OUT)
		{
			if (((pObj->posCurr.x - Burglar->posCurr.x)*(pObj->posCurr.x - Burglar->posCurr.x) + (pObj->posCurr.y - Burglar->posCurr.y) *(pObj->posCurr.y - Burglar->posCurr.y))>40000)
				gameObjDestroy(pObj);
		}

		//Boss ���˶�����ײ
		if (pObj->flag && pObj->pObject->type == TYPE_BOSS)
		{

			if (Burglar->flag&FLAG_ACTIVE && ((pObj->posCurr.x - Burglar->posCurr.x)*(pObj->posCurr.x - Burglar->posCurr.x) + (pObj->posCurr.y - Burglar->posCurr.y)*(pObj->posCurr.y - Burglar->posCurr.y)) < 20000.0f)
			{
				pObj->velCurr.x = (Burglar->posCurr.x - pObj->posCurr.x) / 2.0f;
				pObj->velCurr.y = (Burglar->posCurr.y - pObj->posCurr.y) / 2.0f;

				if (pObj->posCurr.x == Burglar->posCurr.x)
				{
					pObj->velCurr.y = 0.0f;
				}
				if (pObj->posCurr.y == Burglar->posCurr.y)
				{
					pObj->velCurr.x = 0.0f;
				}
				//ũ�������������ƶ�
				pObj->posCurr.x += pObj->velCurr.x / 40.0f;
				pObj->posCurr.y += pObj->velCurr.y / 40.0f;

			}//if

			else
			{
				stempBoss.x = pObj->posCurr.x;
				stempBoss.y = pObj->posCurr.y;
				if (stateboss == 0){
					pObj->posCurr.x += 2.5f;
					if (WorldTransToMapdata(pMap, &stempBoss, 1) == 0){
						if (Burglar->posCurr.y - stempBoss.y > 0)
							stateboss = 2;
						else if (Burglar->posCurr.y - stempBoss.y < 0)
							stateboss = 3;
						else if (Burglar->posCurr.x - stempBoss.x < 0)
							stateboss = 1;
					}
				}

				else if (stateboss == 1){
					pObj->posCurr.x -= 2.5f;
					if (WorldTransToMapdata(pMap, &stempBoss, 1) == 0){
						if (Burglar->posCurr.y - stempBoss.y > 0)
							stateboss = 2;
						else if (Burglar->posCurr.y - stempBoss.y < 0)
							stateboss = 3;
						else if (Burglar->posCurr.x - stempBoss.x > 0)
							stateboss = 0;
					}
				}
				else if (stateboss == 2){
					pObj->posCurr.y += 2.5f;
					if (WorldTransToMapdata(pMap, &stempBoss, 1) == 0){
						if (Burglar->posCurr.x - stempBoss.x < 0)
							stateboss = 1;
						else if (Burglar->posCurr.x - stempBoss.x > 0)
							stateboss = 0;
						else if (Burglar->posCurr.y - stempBoss.y < 0)
							stateboss = 3;
					}
				}

				else if (stateboss == 3){
					pObj->posCurr.y -= 2.5f;
					if (WorldTransToMapdata(pMap, &stempBoss, 1) == 0){
						if (Burglar->posCurr.x - stempBoss.x < 0)
							stateboss = 1;
						else if (Burglar->posCurr.y - stempBoss.y > 0)
							stateboss = 2;
						else if (Burglar->posCurr.x - stempBoss.x > 0)
							stateboss = 0;
					}
				}

			}//else


			//������ײ����������
			if (StaticRectToStaticRect(&Burglar->posCurr, 15.0f, 15.0f, &pObj->posCurr, 15.0f, 15.0f))
			{
				gameObjDestroy(Burglar);
				BurglarLives -= 1;   //������������һ
				if (BurglarLives > 0)
					Next = GS_Restart;
				Matrix2DScale(&pObj->transform, 0.5, 1);
			}
		}

		//���ı߽����
		if (pObj->flag&&pObj->pObject->type == TYPE_DOG)
		{
			if (pObj->posCurr.x > winMaxX || pObj->posCurr.x < winMinX)
			{
				pObj->posCurr.x = -pObj->posCurr.x;
			}
			
			if (pObj->posCurr.y < winMinY || pObj->posCurr.y > winMaxY)
			{
				pObj->posCurr.y = -pObj->posCurr.y;
			}

			//�Ƿ��빷������ײ
			if (StaticRectToStaticRect(&Burglar->posCurr, 15, 20, &pObj->posCurr, 20, 20) && pObj->flag)
			{  //�빷������ײ��Ѫ����һ
				BurglarBlood -= 1;
				if (BurglarBlood <= 0)
				{
					BurglarLives -= 1;
					if (BurglarLives > 0)
						Next = GS_Restart;
				}
			}
		}

		//������
		if (pObj->flag&&pObj->pObject->type == TYPE_WATERMELON)
		{
			if (StaticRectToStaticRect(&Burglar->posCurr, 15.0,20.0,&pObj->posCurr, 10.0,10.0))
			{
				gameObjDestroy(pObj);
				Fruit_NUM -= 1;
				sScore += 20;
			}
		}

		//�񵽲�ݮ
		if (pObj->flag&&pObj->pObject->type == TYPE_STRAWBERRY)
		{
			if (StaticRectToStaticRect(&Burglar->posCurr, 15.0, 20.0, &pObj->posCurr, 10.0, 10.0))
			{
				gameObjDestroy(pObj);
				Fruit_NUM -= 1;
				sScore += 10;
			}
		}

		//��ƻ��
		if (pObj->flag&&pObj->pObject->type == TYPE_APPLE)
		{
			if (StaticRectToStaticRect(&Burglar->posCurr, 15.0, 20.0, &pObj->posCurr, 10.0, 10.0))
			{
				gameObjDestroy(pObj);
				Fruit_NUM -= 1;
				sScore += 10;
			}
		}

		if (pObj->flag && pObj->pObject->type == TYPE_BURGLARBLOOD)
		{
			//����Ѫ��λ��
			pObj->posCurr.x = Burglar->posCurr.x;
			pObj->posCurr.y = Burglar->posCurr.y + 35.0f;
			if (BurglarBlood == 3)
			    pObj->pTex = AEGfxTextureLoad("life_full.png");//����ȫѪ����
			else if(BurglarBlood==2)
				pObj->pTex = AEGfxTextureLoad("life_warning.png");//�����Ѫ����
			else if(BurglarBlood == 1)
				pObj->pTex = AEGfxTextureLoad("life_dangerous.png");//�����ķ�֮һѪ����

			if (!Burglar->flag&FLAG_ACTIVE)
			{
				gameObjDestroy(pObj);   //�������ǵ�ͬʱ���������ǵ�Ѫ�� 
			}
		}

		if (pObj->flag && pObj->pObject->type == TYPE_BOSSBLOOD)
		{
			//��������Ѫ��λ��
			pObj->posCurr.x = pBoss->posCurr.x;
			pObj->posCurr.y = pBoss->posCurr.y + 35.0f;
			
		}

	}//for
	/*
	//�����˶�
   TimeTot1++;
	if (TimeTot1 < 300)
	{
		pDog[0]->posCurr.x += 1.5;
		pDog[1]->posCurr.x -= 1.5;
		pDog[2]->posCurr.y += 1.5;
		pDog[3]->posCurr.y -= 1.5;
		pDog[4]->posCurr.x += 1.5;
	}
	if (TimeTot1 >= 300 && TimeTot1 < 600)
	{
		pDog[0]->posCurr.x -= 1.5;
		pDog[1]->posCurr.x += 1.5;
		pDog[2]->posCurr.y -= 1.5;
		pDog[3]->posCurr.y += 1.5;
		pDog[4]->posCurr.x -= 1.5;
	}
	if (TimeTot1 >= 600 && TimeTot1 < 900)
	{
		pDog[0]->posCurr.y += 1.5;
		pDog[1]->posCurr.y -= 1.5;
		pDog[2]->posCurr.x += 1.5;
		pDog[3]->posCurr.x -= 1.5;
		pDog[4]->posCurr.y += 1.5;
	}
	if (TimeTot1 >= 900 && TimeTot1 < 1200)
	{
		pDog[0]->posCurr.y -= 1.5;
		pDog[1]->posCurr.y += 1.5;
		pDog[2]->posCurr.x -= 1.5;
		pDog[3]->posCurr.x += 1.5;
		pDog[4]->posCurr.y -= 1.5;
	}
	if (TimeTot1 == 1200)
		TimeTot1 = 0;*/    
	
	//�����˶�byС��
/*	stemp.x = pDog[1]->posCurr.x;
	stemp.y = pDog[1]->posCurr.y;
	if (state == 0){
		pDog[1]->posCurr.x += 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 0){
			if (Burglar->posCurr.y - stemp.y > 0)
				state = 2;
			else if (Burglar->posCurr.y - stemp.y < 0)
				state = 3;
			else if (Burglar->posCurr.x - stemp.x < 0)
				state = 1;
		}
	}

	else if (state == 1){
		pDog[1]->posCurr.x -= 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 0){
			if (Burglar->posCurr.y - stemp.y > 0)
				state = 2;
			else if (Burglar->posCurr.y - stemp.y < 0)
				state = 3;
			else if (Burglar->posCurr.x - stemp.x > 0)
			state = 0;
		}
	}
	else if (state == 2){
		pDog[1]->posCurr.y += 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 0){
			if (Burglar->posCurr.x - stemp.x < 0)
				state = 1;
			else if (Burglar->posCurr.x - stemp.x > 0)
				state = 0;
			else if (Burglar->posCurr.y - stemp.y < 0)
				state = 3;
		}
	}
	
	else if (state == 3){
		pDog[1]->posCurr.y -= 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 0){
			if (Burglar->posCurr.x - stemp.x < 0)
				state = 1;
			else if (Burglar->posCurr.y - stemp.y > 0)
				state = 2;
			else if (Burglar->posCurr.x - stemp.x > 0)
				state = 0;
		}
	}*/

	//�����˶�SLL-1
	stemp.x = pDog[0]->posCurr.x;
	stemp.y = pDog[0]->posCurr.y;
	if (state == 0)
	{
		stemp.x += 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[0]->posCurr.x += 1.5f;
			state = 0;
		}
		else
		{
			stemp.x = pDog[0]->posCurr.x;
			stemp.y += 1.5f;
			if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
			{
				pDog[0]->posCurr.y += 1.5f;
				state = 1;
			}
			else
			{
				stemp.y = pDog[0]->posCurr.y;
				stemp.x -= 1.5f;
				if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
				{
					pDog[0]->posCurr.x -= 1.5f;
					state = 2;
				}
				else
				{
					pDog[0]->posCurr.y -= 1.5f;
					state = 3;
				}
			}
		}
	}
	else if (state == 1)
	{
		stemp.y += 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[0]->posCurr.y += 1.5f;
			state = 1;
		}
		else
		{
			stemp.y = pDog[0]->posCurr.y;
			stemp.x -= 1.5f;
			if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
			{
				pDog[0]->posCurr.x -= 1.5f;
				state = 2;
			}
			else
			{
				stemp.x = pDog[0]->posCurr.x;
				stemp.y -= 1.5f;
				if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
				{
					pDog[0]->posCurr.y -= 1.5f;
					state = 3;
				}
				else
				{
					pDog[0]->posCurr.x += 1.5f;
					state = 0;
				}
			}
		}
	}
	else if (state == 2)
	{
		stemp.x -= 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[0]->posCurr.x -= 1.5f;
			state = 2;
		}
		else
		{
			stemp.x = pDog[0]->posCurr.x;
			stemp.y -= 1.5f;
			if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
			{
				pDog[0]->posCurr.y -= 1.5f;
				state = 3;
			}
			else
			{
				stemp.y = pDog[0]->posCurr.y;
				stemp.x += 1.5f;
				if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
				{
					pDog[0]->posCurr.x += 1.5f;
					state = 0;
				}
				else
				{
					pDog[0]->posCurr.y += 1.5f;
					state = 1;
				}
			}
		}
	}
	else if (state == 3)
	{
		stemp.y -= 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[0]->posCurr.y -= 1.5f;
			state = 3;
		}
		else
		{
			stemp.y = pDog[0]->posCurr.y;
			stemp.x += 1.5f;
			if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
			{
				pDog[0]->posCurr.x += 1.5f;
				state = 0;
			}
			else
			{
				stemp.x = pDog[0]->posCurr.x;
				stemp.y += 1.5f;
				if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
				{
					pDog[0]->posCurr.y += 1.5f;
					state = 1;
				}
				else
				{
					pDog[0]->posCurr.x -= 1.5f;
					state = 2;
				}
			}

		}
	}

	//�����˶�SLL-2
	stemp.x = pDog[1]->posCurr.x;
	stemp.y = pDog[1]->posCurr.y;
	if (state2 == 0)
	{
		stemp.y -= 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[1]->posCurr.y -= 1.5f;
			state2 = 0;
		}
		else
		{
			pDog[1]->posCurr.y += 1.5f;
			state2 = 1;
		}
	}
	else if (state2 == 1)
	{
		stemp.y += 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[1]->posCurr.y += 1.5f;
			state2 = 1;
		}
		else
		{
			pDog[1]->posCurr.y -= 1.5f;
			state2 = 0;
		}
	}

	//�����˶�SLL-3
	stemp.x = pDog[2]->posCurr.x;
	stemp.y = pDog[2]->posCurr.y;
	if (state3 == 0)
	{
		stemp.x += 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[2]->posCurr.x += 1.5f;
			state3 = 0;
		}
		else
		{
			stemp.x = pDog[2]->posCurr.x;
			stemp.y += 1.5f;
			if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
			{
				pDog[2]->posCurr.y += 1.5f;
				state3 = 1;
			}
			else
			{
				stemp.y = pDog[2]->posCurr.y;
				stemp.x -= 1.5f;
				if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
				{
					pDog[2]->posCurr.x -= 1.5f;
					state3 = 2;
				}
				else
				{
					pDog[2]->posCurr.y -= 1.5f;
					state3 = 3;
				}
			}
		}
	}
	else if (state3 == 1)
	{
		stemp.y += 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[2]->posCurr.y += 1.5f;
			state3 = 1;
		}
		else
		{
			stemp.y = pDog[2]->posCurr.y;
			stemp.x -= 1.5f;
			if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
			{
				pDog[2]->posCurr.x -= 1.5f;
				state3 = 2;
			}
			else
			{
				stemp.x = pDog[2]->posCurr.x;
				stemp.y -= 1.5f;
				if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
				{
					pDog[2]->posCurr.y -= 1.5f;
					state3 = 3;
				}
				else
				{
					pDog[2]->posCurr.x += 1.5f;
					state3 = 0;
				}
			}
		}
	}
	else if (state3 == 2)
	{
		stemp.x -= 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[2]->posCurr.x -= 1.5f;
			state3 = 2;
		}
		else
		{
			stemp.x = pDog[2]->posCurr.x;
			stemp.y -= 1.5f;
			if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
			{
				pDog[2]->posCurr.y -= 1.5f;
				state3 = 3;
			}
			else
			{
				stemp.y = pDog[2]->posCurr.y;
				stemp.x += 1.5f;
				if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
				{
					pDog[2]->posCurr.x += 1.5f;
					state3 = 0;
				}
				else
				{
					pDog[2]->posCurr.y += 1.5f;
					state3 = 1;
				}
			}
		}
	}
	else if (state3 == 3)
	{
		stemp.y -= 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[2]->posCurr.y -= 1.5f;
			state3 = 3;
		}
		else
		{
			stemp.y = pDog[2]->posCurr.y;
			stemp.x += 1.5f;
			if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
			{
				pDog[2]->posCurr.x += 1.5f;
				state3 = 0;
			}
			else
			{
				stemp.x = pDog[2]->posCurr.x;
				stemp.y += 1.5f;
				if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
				{
					pDog[2]->posCurr.y += 1.5f;
					state3 = 1;
				}
				else
				{
					pDog[2]->posCurr.x -= 1.5f;
					state3 = 2;
				}
			}

		}
	}

	//�����˶�SLL-4
	stemp.x = pDog[3]->posCurr.x;
	stemp.y = pDog[3]->posCurr.y;
	if (state4 == 0)
	{
		stemp.y -= 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[3]->posCurr.y -= 1.5f;
			state4 = 0;
		}
		else
		{
			pDog[3]->posCurr.y += 1.5f;
			state4 = 1;
		}
	}
	else if (state4 == 1)
	{
		stemp.y += 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[3]->posCurr.y += 1.5f;
			state4 = 1;
		}
		else
		{
			pDog[3]->posCurr.y -= 1.5f;
			state4 = 0;
		}
	}

	//�����˶�SLL-5
	stemp.x = pDog[4]->posCurr.x;
	stemp.y = pDog[4]->posCurr.y;
	if (state5 == 0)
	{
		stemp.x += 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[4]->posCurr.x += 1.5f;
			state5 = 0;
		}
		else
		{
			pDog[4]->posCurr.x -= 1.5f;
			state5 = 1;
		}
	}
	else if (state5 == 1)
	{
		stemp.x -= 1.5f;
		if (WorldTransToMapdata(pMap, &stemp, 1) == 1)
		{
			pDog[4]->posCurr.x -= 1.5f;
			state5 = 1;
		}
		else
		{
			pDog[4]->posCurr.x += 1.5f;
			state5 = 0;
		}
	}

	


	//���ո��
	if (KeyPressed[KeySpace])
	{
		StoneCount = 100;
	}
	// ��������
	Input_Initialize();

	// ǩ��
	fprintf(fp, "Level1:Update\n");

}

void Draw1(void)
{
	unsigned long i;

	//��������ͼ
	if (Level1_Map->flag&FLAG_ACTIVE)
	{
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // �����������û���ģʽΪ����
		AEGfxSetPosition(0.0f, 0.0f);
		// Set texture for object background
		AEGfxTextureSet(pTexMap, 0.0f, 0.0f); // ����1������ƫ����(x,y)
		AEGfxSetTransparency(1.0f);
		AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
		// Drawing the mesh (list of triangles)
		AEGfxMeshDraw(Level1_Map->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
	}


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

		//�����ӳ�ȥ��ʯͷ����
		if ((pInst->pObject->type == TYPE_STONE) && (Burglar->flag & FLAG_ACTIVE))
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

		//�������ص�����
		if (pInst->pObject->type == TYPE_TRAP_IN)
		{

			//if (KeyPressed[KeySpace])
			//	{
			// Drawing object stone
			// Set position for object stone
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);   // �����������û���ģʽΪ����
												   //AEGfxSetPosition(Burglar->posCurr.x+100.0f, Burglar->posCurr.y+10.0f);
												   // Set texture for object stone
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			//AEGfxTextureSet(pTexStone, 0.0f, 0.0f); // ����1������ƫ����(x,y)
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			// Drawing the mesh (list of triangles)
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
			//}
		}

		//��������
		if (pInst->pObject->type == TYPE_TRAP_OUT)
		{
			//if (KeyPressed[KeySpace])
			//	{
			// Drawing object stone
			// Set position for object stone
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // �����������û���ģʽΪ����
													 //AEGfxSetPosition(Burglar->posCurr.x+100.0f, Burglar->posCurr.y+10.0f);
													 // Set texture for object stone
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			AEGfxTextureSet(pTexTrap, 0.0f, 0.0f); // ����1������ƫ����(x,y)
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
			// ����
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			// ������
			AEGfxSetTransparency(1);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//����BOSSѪ������
		if (pInst->pObject->type == TYPE_BOSSBLOOD)
		{
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// ����Ѫ��������λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ����
			AEGfxTextureSet(pTex_Bossblood, 0.0f, 0.0f);
			// ��Ѫ��
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

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


		//�������϶���
		if (pInst->pObject->type == TYPE_WATERMELON)
		{
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// �������ϵ�����λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ������
			AEGfxTextureSet(pTexWatermelon, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// ����������
			//AEGfxSetTransparency(1);
			//AEGfxSetBlendColor(1.0f, 1.0f, 1.0, 1.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//���Ʋ�ݮ����
		if (pInst->pObject->type == TYPE_STRAWBERRY)
		{
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// �������ϵ�����λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ������
			AEGfxTextureSet(pTexStrawberry, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// ����������
			//AEGfxSetTransparency(1);
			//AEGfxSetBlendColor(1.0f, 1.0f, 1.0, 1.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//����ƻ������
		if (pInst->pObject->type == TYPE_APPLE)
		{
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// �������ϵ�����λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ������
			AEGfxTextureSet(pTexApple, 0.0f, 0.0f);
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
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// �������ϵ�����λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ������
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// ������Ѫ��
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		if (pInst->pObject->type == TYPE_BURGLARLIVES)
		{  //����С��������
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// ����С��������������λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ����
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			// ��������
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		/*if (pInst->pObject->type == TYPE_BURGLARSCORES)
		{  //����С������
			// ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			// ����С������������λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ����
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			// ������
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}*/

		if (pInst->pObject->type == TYPE_LOSE && BurglarLives <= 0)
		{  //����ʧ��ҳ��
		   // ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// ��������λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ����
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			// ��ʧ��ҳ��
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		if (pInst->pObject->type == TYPE_WIN && sScore >= 50)
		{  //����ʤ��ҳ��
		   // ���û���ģʽ(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// ��������λ��
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// ����
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			// ��ʤ��ҳ��
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}
		
	}
	// ǩ��
	fprintf(fp, "Level1:Draw\n");
}

void Free1(void)
{
	gameObjDestroy(Burglar);
	//gameObjDestroy();
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

	//�ͷŵ�ͼռ�Ĵ洢�ռ�
	for (int i = 0; i < pMap->BINARY_MAP_HEIGHT; ++i)
		free(pMap->mapData[i]);// = (int *)malloc((pMap->BINARY_MAP_WIDTH)* sizeof(int *));
	free(pMap->mapData);//= (int **)malloc((pMap->BINARY_MAP_HEIGHT)* sizeof(int *));
	for (int i = 0; i < pMap->BINARY_MAP_HEIGHT; ++i)
		free(pMap->CollisionData[i]);// = (int *)malloc((pMap->BINARY_MAP_WIDTH)* sizeof(int *));
	free(pMap->CollisionData);
	free(pMap);
	
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

void mapLoad(Map* pMap)//��ͼ��ȡ
{
	FILE *fp;
	char c;
	int H, W;
	int i, j;
	if ((fp = fopen("map1.txt", "r")) == NULL)
	{
		printf("Error opening data file\n");
		exit(-1);
	}
	//H = (int *)malloc(sizeof(int));
	fscanf(fp, "%d", &W);//���߶�
	//W = (int *)malloc(sizeof(int));
	fscanf(fp, "%d", &H);//�����
	pMap->BINARY_MAP_HEIGHT = H;
	pMap->BINARY_MAP_WIDTH = W;
	//printf("%d\n", pMap->BINARY_MAP_HEIGHT);
	//printf("%d\n", pMap->BINARY_MAP_WIDTH);
	(pMap->mapData) = (int **)malloc((pMap->BINARY_MAP_HEIGHT)* sizeof(int *));
	for (i = 0; i < pMap->BINARY_MAP_HEIGHT; ++i)
		pMap->mapData[i] = (int *)malloc((pMap->BINARY_MAP_WIDTH)* sizeof(int *));
	(pMap->CollisionData) = (int **)malloc((pMap->BINARY_MAP_HEIGHT)* sizeof(int *));
	for (i = 0; i < pMap->BINARY_MAP_HEIGHT; ++i)
		pMap->CollisionData[i] = (int *)malloc((pMap->BINARY_MAP_WIDTH)* sizeof(int *));
	for (i = pMap->BINARY_MAP_HEIGHT - 1; i >= 0; --i)
	{
		for (j = 0; j < pMap->BINARY_MAP_WIDTH; ++j)//��mapdata
		{
			if (isdigit(c = fgetc(fp)))
				ungetc(c, fp);
			fscanf(fp, "%d", &(pMap->mapData[i][j]));
			//printf("%d\t", pMap->mapData[i][j]);
		}
	}
	for (i = 0; i < pMap->BINARY_MAP_HEIGHT; i++)
		for (j = 0; j < pMap->BINARY_MAP_WIDTH; j++)
		{
			if (pMap->mapData[i][j] > CollisionMin)//������ڱ߽���ײ�����ֵ
				pMap->CollisionData[i][j] = 0;//����collisiondataΪ��
			else pMap->CollisionData[i][j] = pMap->mapData[i][j];
		}
	fclose(fp);
}

/*void MapTransform(GameObj* pInst, int MAP_WIDTH, int MAP_HEIGHT)//��ͼλ�ü���
{
//�м����
Matrix2D mapTrans;
Matrix2D mapScale;
Matrix2D mapTransform;

//��ͼ�任����ļ���
Matrix2DIdentity(&mapTrans);
Matrix2DTranslate(&mapTrans, -MAP_WIDTH / 2.f, -MAP_HEIGHT / 2.f);
Matrix2DIdentity(&mapScale);
Matrix2DScale(&mapScale, 20, 20);
Matrix2DConcat(&mapTransform, &mapScale, &mapTrans);
}*/

int WorldTransToMapdata(Map *pMap, Vector2D *pVec, int choose)
{
	//��Ԫ���С����
	float HEIGHT = 600.0f / (pMap->BINARY_MAP_HEIGHT);
	float WIDTH = 800.0f / (pMap->BINARY_MAP_WIDTH);

	int Xx = pVec->x;
	int Yy = pVec->y;
	//�������������ڵ�ͼ�����е�λ��
	int   MapdataX = (Xx + 400.0f) / WIDTH;
	int   MapdataY = (Yy + 300.0f) / HEIGHT;

	//������ײ����
	if ((0 <= MapdataX) &&(MapdataX <= 29) && (0 <= MapdataY)&&(MapdataY <= 19)){
		//������ײ����
		if (choose == 1)
			return (pMap->CollisionData[MapdataY][MapdataX]);
		else if (choose == 2)
			return (pMap->mapData[MapdataY][MapdataX]);
	}
	else return NULL;
}