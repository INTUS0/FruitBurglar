/* Project:		GSMWinStep
File Name:	Level2.c
Author(former):		刘芳
Author INT US
Date:
Purpose:		关卡2  */

#include "Level2.h"
#include "GameStateList.h"
#include "System.h"
#include <stdio.h>
#include "AEEngine.h"
#include "Input.h"
#include "Matrix2D.h"
#include "Vector2D.h"
#include "Math2D.h"
#include <Windows.h>
//------------------------------------------------------------------------------
// Private Consts:
//------------------------------------------------------------------------------
#define GAME_OBJ_BASE_NUM_MAX	32			// 对象类型（对象基类）数目上限
#define GAME_OBJ_NUM_MAX		2048		// 对象数目上限

#define BURGLAR_INITIAL_NUM			3		// 主角的lives数目
#define FRUIT_NUM				3		// 水果数目
#define BURGLAR_SIZE					30.0f	// 主角尺寸
#define BURGLER_ACCEL_FORWARD			50.0f	// 主角前向加速度(m/s^2)
#define BURGLAR_ACCEL_BACKWARD			-100.0f	// 主角后向加速度(m/s^2)

#define FLAG_ACTIVE					0x00000001  // 活动对象标志

//------------------------------------------------------------------------------
// Private Structures:
//------------------------------------------------------------------------------
//游戏对象基类
typedef struct
{
	unsigned long		type;		// 游戏对象类型
	AEGfxVertexList*	pMesh;		// 形状
}GameObjBase;

//游戏对象类
typedef struct
{
	GameObjBase*       pObject;     //指向基类
	unsigned long      flag;        //活动标志
	float              scale;       //尺寸
	Vector2D           posCurr;     //当前位置
	Vector2D           velCurr;     //当前速度
	float              dirCurr;     //当前方向

	Matrix2D           transform;   //变换矩阵，每一帧都需要为一个对象计算
}GameObj;


//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private Variables:
//------------------------------------------------------------------------------
// 游戏对象基类（类型）列表

static GameObjBase		sGameObjBaseList[GAME_OBJ_BASE_NUM_MAX];	// 该数组中的元素是游戏对象基类的实例：形状和类型
static unsigned long	sGameObjBaseNum;							// 已定义的游戏对象基类

// 游戏对象列表
static GameObj			sGameObjList[GAME_OBJ_NUM_MAX];				// 该数组中的元素是游戏对象的实例
static unsigned long	sGameObjNum;								// 游戏对象的个数

//小盗对象,player
static GameObj* Burglar;
//Burglar = (GameObj*)malloc(sizeof(GameObj));

//石头对象
static GameObj* pStone;
//陷阱对象
static GameObj* pTrap;

//农场主对象
static GameObj* pBoss;
//狗
static GameObj* pDog[5];

static AEGfxTexture *pTexMap;		//对象Map的纹理

//小盗血量
static int BurglarBlood;
static unsigned long sScore; //捡到的水果数

//水果数量
static int Fruit_NUM = 0;
//水果产生时间间隔
static int TimeTot = 0;
//临时定义狗运动时间
static int TimeTot1 = 0;


//狗数量
static int DOG_NUM = 5;

//农场主血量
static int BossBlood;

//捡到的石头数量
static int StoneCount = 0;

//帧率控制常量
static int AnimationCounter = 0; // 控制播放帧
static int AnimationFPS = 12;
static float ElapsedTime = 0.0f;

static AEGfxTexture *pTex1;		// 对象Burglar的纹理
static AEGfxVertexList*	pMesh1;	// 对象Burglar的网格(mesh)模型
static float obj1X, obj1Y;		// 对象burglar的位置

static AEGfxTexture *pTex2;		//对象dog的纹理
static AEGfxTexture *pTexStone; //移动石头纹理
static AEGfxTexture *pTexTrap; //陷阱纹理

int mapinfo[50][50];//地图数组

//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------
// 创建/删除游戏对象
static GameObj*		gameObjCreate(unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir);
static void			gameObjDestroy(GameObj* pInst);
//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

void Load2(void)
{
	GameObjBase* pObjBase;

	// 初始化游戏对象基类的实例列表
	memset(sGameObjBaseList, 0, sizeof(GameObjBase) * GAME_OBJ_BASE_NUM_MAX);

	sGameObjBaseNum = 0;

	// 创建基类的实例	
	// =====================
	//主角
	// =====================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLAR;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 0.125f, 1.0f,
		-15.0f, 12.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 0.125f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 0.125f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create object!!");
	pTex1 = AEGfxTextureLoad("PlayerRun.png");//载入纹理

	// =======================
	// 发射出去的石头：尺寸很小，简化成三角形定义
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
	// 静止被捡起的石头：尺寸很小，简化成三角形定义
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
	// 看得见的陷阱
	// =========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_TRAP_IN;

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
	pTexTrap = AEGfxTextureLoad("planetTexture.png");; //陷阱纹理

	//看不见的陷阱
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_TRAP_OUT;

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
	// 农场主：两个三角形拼接的菱形
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
	// 狗：两个三角形拼接的菱形
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_DOG;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-15.0f, -9.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -9.0f, 0x00FFFF00, 1.0f, 1.0f,
		-15.0f, 9.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -9.0f, 0x00FFFFFF, 1.0f, 1.0f,
		15.0f, 9.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-15.0f, 9.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");
	pTex2 = AEGfxTextureLoad("Dog1.png");//载入纹理

	// ========================
	// 草莓：两个三角形拼接的菱形
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
	// 西瓜：圆形
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
	// BOSS血量：两个三角形拼接的长方形
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BOSSBLOOD;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-20.0f, 4.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 黑色
		20.0f, 6.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 红色
		-20.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF 白色
	AEGfxTriAdd(
		20.0f, -2.0f, 0x01FF0000, 1.0f, 1.0f,
		20.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,
		-20.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// 小盗血量：两个三角形拼接的长方形
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARBLOOD;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-20.0f, 4.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 黑色
		28.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 红色
		-20.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF 白色
	AEGfxTriAdd(
		16.0f, -4.0f, 0x01FF0000, 1.0f, 1.0f,
		20.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,
		-20.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// 小盗生命数：两个三角形拼接的长方形
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARLIVES;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-30.0f, 4.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 黑色
		30.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 红色
		-30.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF 白色
	AEGfxTriAdd(
		30.0f, -4.0f, 0x01FF0000, 1.0f, 1.0f,
		30.0f, 4.0f, 0xFFFF0000, 1.0f, 0.0f,
		-30.0f, -4.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// =====================
	//地图
	// =====================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_MAP;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-6.0f, -6.0f, 0x00FF00FF, 0.0f, 1.0f,
		6.0f, -6.0f, 0x00FFFF00, 1.0f, 1.0f,
		-6.0f, 6.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		7.0f, -7.0f, 0x00FFFFFF, 1.0f, 1.0f,
		7.0f, 7.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-7.0f, 7.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create object!!");
	pTexMap = AEGfxTextureLoad("map1.png");

	// 签到
	fprintf(fp, "Level2:Load\n");
}

void Ini2(void)
{
	//水果数量
	Fruit_NUM = 0;
	//水果产生时间间隔
	TimeTot = 0;
	//临时定义狗运动时间
	TimeTot1 = 0;
	//捡到的石头数量
	static int StoneCount = 10;

	// 对象1的初始位置
	obj1X = -10.0f;
	obj1Y = 0.0f;
	memset(sGameObjList, 0, sizeof(GameObj)*GAME_OBJ_NUM_MAX);
	GameObj* pObj;
	int i;

	BossBlood = 2;      //初始化BOSS血量

	// 为开始画对象做准备
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// 对象实例化：游戏开始只有小盗和狗和农场主需要实例化
	// 小盗对象实例化
	Burglar = gameObjCreate(TYPE_BURGLAR, BURGLAR_SIZE, 0, 0, 0.0f);
	AE_ASSERT(Burglar);
	Burglar->posCurr.x = AEGfxGetWinMaxX()-20;
	Burglar->posCurr.y = 100.0f;
	Burglar->dirCurr = acosf(Burglar->posCurr.x / ((float)sqrt(Burglar->posCurr.x*Burglar->posCurr.x + Burglar->posCurr.y * Burglar->posCurr.y))) - PI;
	Burglar->scale = 10.0f;

	//初始化静止的石头
	for (i = 0; i < 3; i++)
	{
		pObj = gameObjCreate(TYPE_STONE_STATIC, 3.0f, 0, 0, 0.0f);
		AE_ASSERT(pObj);
		// 实例化
		// 初始化: 坐标位置 朝向和尺寸大小
		switch (i)
		{
		case 0:
			pObj->posCurr.x = 100.0f;
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

	//初始化看不见的陷阱
	for (int a = 0; a < 3; a++)
	{
		pObj = gameObjCreate(TYPE_TRAP_IN, 10.0f, 0, 0, 0.0f);
		AE_ASSERT(pObj);
		//初始化陷阱位置及朝向比例

		pObj->posCurr.x = a*100.0f;
		pObj->posCurr.y = -100.0f;
		pObj->dirCurr = acosf(pObj->posCurr.x / ((float)sqrt(pObj->posCurr.x*pObj->posCurr.x + pObj->posCurr.y * pObj->posCurr.y))) - PI;

		pObj->scale = 10.0f;
	}

	//初始化农场主
	pBoss = gameObjCreate(TYPE_BOSS, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pBoss);
	//初始化农场主位置及朝向比例
	pBoss->posCurr.x = 100.0f;
	pBoss->posCurr.y = 100;
	pBoss->dirCurr = acosf(pBoss->posCurr.x / ((float)sqrt(pBoss->posCurr.x*pBoss->posCurr.x + pBoss->posCurr.y * pBoss->posCurr.y))) - PI;
	pBoss->scale = 10.0f;

	//狗对象实例化 并 初始化
	for (i = 0; i < DOG_NUM; i++)
	{
		// 实例化
		pObj = gameObjCreate(TYPE_DOG, 10.0f, 0, 0, 0.0f);
		pDog[i] = pObj;//给狗分配指针
		AE_ASSERT(pObj);
		pObj->posCurr.x = 100.0f;
		pObj->posCurr.y = 100.0f;
		
		pObj->dirCurr = acosf(pObj->posCurr.x / ((float)sqrt(pObj->posCurr.x*pObj->posCurr.x + pObj->posCurr.y * pObj->posCurr.y))) - PI;

		pObj->scale = 10.0f;
	}


	//农场主血量初始化
	pObj = gameObjCreate(TYPE_BOSSBLOOD, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//初始化血量位置
	pObj->posCurr.x = pBoss->posCurr.x;
	pObj->posCurr.y = pBoss->posCurr.y + 35.0f;

	//主角血量初始化
	pObj = gameObjCreate(TYPE_BURGLARBLOOD, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//初始化血量位置
	pObj->posCurr.x = Burglar->posCurr.x;
	pObj->posCurr.y = Burglar->posCurr.y + 35.0f;

	//地图的引入
	FILE *fp = NULL;
	if ((fp = fopen("wdp.txt", "r")) != NULL)
	{
		int length = 0, width = 0;
		fscanf(fp, "%d%d", &length, &width);
		int i = 0, j = 0;
		for (; i < length; i++)
		{
			for (j = 0; j < width; j++)
			{
				fscanf(fp, "%d", &mapinfo[i][j]);
			}
		}

		//读入地图
		for (i = 0; i < length; i++)
		{
			for (j = 0; j < width; j++)
			{
				if (mapinfo[i][j] == 1)
				{
					//画地图
					pObj = gameObjCreate(TYPE_MAP, 1.0f, 0, 0, 0.0f);
					AE_ASSERT(pObj);
					pObj->posCurr.y = i * 40 - 400.0f;
					pObj->posCurr.x = j*15 - 300.0f;
				}
			}
		}
	}
	else if ((fp = fopen("wdp.txt", "r")) == NULL)
	{
		KeyPressed[KeyESC] = TRUE;
	}

	// 签到
	fprintf(fp, "Level2:Initialize\n");
}

void Update2(void)
{
	//unsigned long i;
	float winMaxX, winMaxY, winMinX, winMinY;
	double frameTime;



	// ==========================================================================================
	// 获取窗口四条边的坐标，当窗口发生移动或缩放，以下值会发生变化
	// ==========================================================================================
	winMaxX = AEGfxGetWinMaxX();
	winMaxY = AEGfxGetWinMaxY();
	winMinX = AEGfxGetWinMinX();
	winMinY = AEGfxGetWinMinY();

	// ======================
	// 帧时间：相当于zero中的dt
	// ======================
	frameTime = AEFrameRateControllerGetFrameTime();

	// =========================
	// 游戏逻辑响应输入
	// =========================
	// 状态切换
	if (KeyPressed[KeyR])
		Next = GS_Restart;
	if (KeyPressed[KeyESC])
		Next = GS_Quit;
	if (KeyPressed[Key1])
		Next = GS_L1;
	if (KeyPressed[KeyMenu])
		Next = GS_MENU;


	// 主角的移动
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

	//鼠标左键控制石头生成
	if ((KeyPressed[KeyLeftBottom]) && (Burglar->flag& FLAG_ACTIVE))//&& (StoneCount>0))
	{

		pStone = gameObjCreate(TYPE_STONE, 3.0f, 0, 0, 0.0f);;
		AE_ASSERT(pStone);
		// 实例化
		// 初始化: 坐标位置 朝向和尺寸大小
		//在主角位置产生，射向鼠标位置
		pStone->posCurr.x = Burglar->posCurr.x;
		pStone->posCurr.y = Burglar->posCurr.y;

		//石头减一
		StoneCount -= 1;
	}


	//随机产生水果，每隔3秒产生一个，一个界面最多产生10个水果
	TimeTot++;
	if (TimeTot == 180 && Fruit_NUM<10)
	{
		Fruit_NUM++;
		int Xx, Yy;
		Xx = rand();
		Yy = rand();

		//控制水果在界面内产生
		while (Xx > 800.0f) //控制水果在界面宽度内产生
		{
			Xx = (int)rand();
		}
		while (Yy > 600.0f)//控制水果在界面高度度内产生
		{
			Yy = (int)rand();
		}
		Xx = Xx - 400;
		Yy = Yy - 300;

		TimeTot = 0;
		GameObj* pFruit = gameObjCreate(TYPE_WATERMELON, 3.0f, 0, 0, 0.0f);;
		AE_ASSERT(pFruit);
		// 实例化

		// 初始化: 坐标位置 朝向和尺寸大小
		pFruit->posCurr.x = Xx;
		pFruit->posCurr.y = Yy;

		pFruit->scale = 5.0f;
	}
	else if (TimeTot == 200) TimeTot = 0;



	//遍历所有对象以决定操作
	for (int i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{

		GameObj* pObj = sGameObjList + i;

		//石头的发射
		if (pObj->flag && pObj->pObject->type == TYPE_STONE)
		{
			//石头朝着鼠标位置运动
			pStone->velCurr.x = (posX - Burglar->posCurr.x) / 10.0f;
			pStone->velCurr.y = (posY - Burglar->posCurr.y) / 10.0f;

			pObj->posCurr.x += pObj->velCurr.x;
			pObj->posCurr.y += pObj->velCurr.y;

			//碰撞到狗或者boss
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

		//捡石头
		if (pObj->flag&&pObj->pObject->type == TYPE_STONE_STATIC)
		{
			if (StaticRectToStaticRect(&Burglar->posCurr, 33, 30, &pObj->posCurr, 15, 15))
			{
				gameObjDestroy(pObj);
				StoneCount += 1;
			}
		}


		//离陷阱距离近时
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
						gameObjDestroy(Burglar);//碰撞了，减少scale，并销毁对象
					}
				}
			}

		}
		//远离陷阱时
		if (pObj->flag && pObj->pObject->type == TYPE_TRAP_OUT)
		{
			if (((pObj->posCurr.x - Burglar->posCurr.x)*(pObj->posCurr.x - Burglar->posCurr.x) + (pObj->posCurr.y - Burglar->posCurr.y) *(pObj->posCurr.y - Burglar->posCurr.y))>40000)
				gameObjDestroy(pObj);
		}

		//Boss 的运动跟碰撞
		if (pObj->flag && pObj->pObject->type == TYPE_BOSS)
		{

			//发生碰撞，主角死亡
			if (StaticRectToStaticRect(&Burglar->posCurr, 15.0f, 15.0f, &pObj->posCurr, 15.0f, 15.0f))
			{
				gameObjDestroy(Burglar);
			}
		}

		//狗的边界控制
		if (pObj->flag&&pObj->pObject->type == TYPE_DOG)
		{

			if (pObj->posCurr.x >= winMaxX || pObj->posCurr.x <= winMinX)
			{
				pObj->posCurr.x = -pObj->posCurr.x;

			}

			if (pObj->posCurr.y <= winMinY || pObj->posCurr.y >= winMaxY)
			{
				pObj->posCurr.y = -pObj->posCurr.y;
			}



			//是否与狗发生碰撞
			if (StaticRectToStaticRect(&Burglar->posCurr, 30, 30, &pObj->posCurr, 30, 30) && pObj->flag)
			{
				Burglar->scale -= 0.5;
			}
			if (Burglar->scale < 1.0f)
			{
				gameObjDestroy(Burglar);//碰撞了，减少scale，并销毁对象
			}
		}

		//捡西瓜
		if (pObj->flag&&pObj->pObject->type == TYPE_WATERMELON)
		{
			if (StaticPointToStaticCircle(&Burglar->posCurr, &pObj->posCurr, 10.0))
			{
				gameObjDestroy(pObj);
				Fruit_NUM -= 1;
				sScore += 10;
			}
		}


		if (pObj->flag && pObj->pObject->type == TYPE_BURGLARBLOOD)
		{
			//更新血量位置
			pObj->posCurr.x = Burglar->posCurr.x;
			pObj->posCurr.y = Burglar->posCurr.y + 35.0f;

			if (!Burglar->flag&FLAG_ACTIVE)
			{
				gameObjDestroy(pObj);   //销毁主角的同时，销毁主角的血量 
			}
		}

	}//for

	//狗运动
	for (int a = 0; a < 5; a++)
	{
		srand(time());
		int PosDogX = (pDog[a]->posCurr.x + 400) / 40;
		int PosDogY = (pDog[a]->posCurr.y + 300) / 15;
		if (mapinfo[PosDogX - 1][PosDogY])
		{
			pDog[a]->posCurr.x += 1;
		}
		else if (mapinfo[PosDogX + 1][PosDogY])
		{
			pDog[a]->posCurr.x -= 1;
		}

		else if (mapinfo[PosDogX][PosDogY + 1])
		{
			pDog[a]->posCurr.y -= 1;
		}
		else  if (mapinfo[PosDogX][PosDogY - 1])
		{
			pDog[a]->posCurr.y += 1;
		}
		//else
		//{
		//	pDog[a]->posCurr.x += 2;
		//}
	}
	
	//狗的运动
	TimeTot1++;
	int x;
	if (TimeTot1 < 300)
	{
		
		pDog[0]->posCurr.x += 1;
		pDog[1]->posCurr.x -= 1;
		pDog[2]->posCurr.y += 1;
		pDog[3]->posCurr.y -= 1;
		pDog[4]->posCurr.x += 1;
	}
	if (TimeTot1 >= 300 && TimeTot1 < 600)
	{
		pDog[0]->posCurr.x -= 1;
		pDog[1]->posCurr.x += 1;
		pDog[2]->posCurr.y -= 1;
		pDog[3]->posCurr.y += 1;
		pDog[4]->posCurr.y -= 1;
	}
	if (TimeTot1 >= 600 && TimeTot1 < 900)
	{
		pDog[0]->posCurr.y += 1;
		pDog[1]->posCurr.y -= 1;
		pDog[2]->posCurr.x += 1;
		pDog[3]->posCurr.x -= 1;
		pDog[4]->posCurr.y += 1;
	}
	if (TimeTot1 >= 900 && TimeTot1 < 1200)
	{
		pDog[0]->posCurr.y -= 1;
		pDog[1]->posCurr.y += 1;
		pDog[2]->posCurr.x -= 1;
		pDog[3]->posCurr.x += 1;
		pDog[4]->posCurr.y -= 1;
	}
	if (TimeTot1 == 1200)
	{
		TimeTot1 = 0;
	}
	
	
		//按空格键
	if (KeyPressed[KeySpace])
	{

	}
	// 输入重置
	Input_Initialize();

	// 签到
	fprintf(fp, "Level2:Update\n");
}

void Draw2(void)
{
	unsigned long i;

	// 画主角，单独画
	if (Burglar->flag& FLAG_ACTIVE)
	{
		//主角移动帧率时间
		float frameTime = AEFrameRateControllerGetFrameTime()*10.0f;

		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetPosition(obj1X, obj1Y);
		AEGfxTextureSet(pTex1, obj1X, obj1Y);//设置猴子位置
		//更新猴子位置,同时更新相机位置
		Burglar->posCurr.x = obj1X;
		Burglar->posCurr.y = obj1Y;
		//相机的移动
		//AEGfxSetCamPosition(obj1X, obj1Y);

		//实现主角的动图效果
		ElapsedTime += frameTime;               // 游戏帧一帧的时间
		if (ElapsedTime < 1.f / AnimationFPS)	// 如果时间还不到动画帧一帧需要的时间
			return;								// 则返回
		else
			ElapsedTime = 0.0f;                 // 否则重置elapsedTime

		AEGfxTextureSet(pTex1, AnimationCounter*0.125f, 0.0f);
		AnimationCounter += 1;
		if (AnimationCounter > 7)
			AnimationCounter = 0;


		AEGfxSetTransparency(1.0f);
		AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
		AEGfxMeshDraw(Burglar->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
	}

	// 逐个绘制对象列表中的所有对象
	for (i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{
		GameObj* pInst = sGameObjList + i;

		// 跳过非活动对象
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		//绘制扔出去的石头对象
		if ((pInst->pObject->type == TYPE_STONE) && (Burglar->flag & FLAG_ACTIVE))
		{
			//if (KeyPressed[KeySpace])
			//	{
			// Drawing object stone
			// Set position for object stone
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // 必须最先设置绘制模式为纹理
			//AEGfxSetPosition(Burglar->posCurr.x+100.0f, Burglar->posCurr.y+10.0f);
			// Set texture for object stone
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			AEGfxTextureSet(pTexStone, 0.0f, 0.0f); // 参数1：纹理，偏移量(x,y)
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			// Drawing the mesh (list of triangles)
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
			//}
		}

		//绘制被捡起来的石头
		if (pInst->pObject->type == TYPE_STONE_STATIC)
		{
			//if (KeyPressed[KeySpace])
			//	{
			// Drawing object stone
			// Set position for object stone
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // 必须最先设置绘制模式为纹理
			//AEGfxSetPosition(Burglar->posCurr.x+100.0f, Burglar->posCurr.y+10.0f);
			// Set texture for object stone
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			AEGfxTextureSet(pTexStone, 0.0f, 0.0f); // 参数1：纹理，偏移量(x,y)
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			// Drawing the mesh (list of triangles)
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
			//}
		}

		//绘制隐藏的陷阱
		if (pInst->pObject->type == TYPE_TRAP_IN)
		{

			//if (KeyPressed[KeySpace])
			//	{
			// Drawing object stone
			// Set position for object stone
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);   // 必须最先设置绘制模式为纹理
			//AEGfxSetPosition(Burglar->posCurr.x+100.0f, Burglar->posCurr.y+10.0f);
			// Set texture for object stone
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			//AEGfxTextureSet(pTexStone, 0.0f, 0.0f); // 参数1：纹理，偏移量(x,y)
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			// Drawing the mesh (list of triangles)
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
			//}
		}

		//绘制陷阱
		if (pInst->pObject->type == TYPE_TRAP_OUT)
		{
			//if (KeyPressed[KeySpace])
			//	{
			// Drawing object stone
			// Set position for object stone
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // 必须最先设置绘制模式为纹理
			//AEGfxSetPosition(Burglar->posCurr.x+100.0f, Burglar->posCurr.y+10.0f);
			// Set texture for object stone
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			AEGfxTextureSet(pTexTrap, 0.0f, 0.0f); // 参数1：纹理，偏移量(x,y)
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			// Drawing the mesh (list of triangles)
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
			//}
		}


		//绘制boss
		if (pInst->pObject->type == TYPE_BOSS)
		{
			// 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置狗的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 无纹理
			AEGfxTextureSet(pTexStone, 0.0f, 0.0f);
			// 画对象狗
			AEGfxSetTransparency(1);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//绘制狗对象
		if (pInst->pObject->type == TYPE_DOG)
		{
			// 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置狗的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 无纹理
			AEGfxTextureSet(pTex2, 0.0f, 0.0f);
			// 画对象狗
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}


		//绘制西瓜对象
		if (pInst->pObject->type == TYPE_WATERMELON)
		{
			// 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			// 设置西瓜的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 无纹理
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// 画对象西瓜
			//AEGfxSetTransparency(1);
			//AEGfxSetBlendColor(1.0f, 1.0f, 1.0, 1.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//绘制主角血量
		if (pInst->pObject->type == TYPE_BURGLARBLOOD)
		{
			// 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			// 设置西瓜的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 无纹理
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// 画对象西瓜
			//AEGfxSetTransparency(1);
			//AEGfxSetBlendColor(1.0f, 1.0f, 1.0, 1.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}//if

		//画地图对象
		if (pInst->pObject->type == TYPE_MAP)
		{
			//if (KeyPressed[KeySpace])
			//	{
			// Drawing object stone
			// Set position for object stone
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // 必须最先设置绘制模式为纹理
			//AEGfxSetPosition(Burglar->posCurr.x+100.0f, Burglar->posCurr.y+10.0f);
			// Set texture for object stone
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			AEGfxTextureSet(pTexMap, 0.0f, 0.0f); // 参数1：纹理，偏移量(x,y)
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			// Drawing the mesh (list of triangles)
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
			//}
		}

	}//for
	// 签到
	fprintf(fp, "Level2:Draw\n");
}

void Free2(void)
{
	gameObjDestroy(Burglar);

	for (int i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{
		GameObj* pObj = sGameObjList + i;
		if (pObj->flag)
			gameObjDestroy(pObj);
	}
	// 签到
	fprintf(fp, "Level2:Free\n");
}

void Unload2(void)
{
	AEGfxMeshFree(Burglar->pObject->pMesh);		// 注销创建的对象
	//AEGfxTextureUnload(pTex1); //销毁猴子
	// 签到
	fprintf(fp, "Level2:Unload\n");
}

//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------
GameObj* gameObjCreate(unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir)
{
	unsigned long i;
	Vector2D zero = { 0.0f, 0.0f };

	AE_ASSERT_PARM(type < sGameObjBaseNum);

	// 找游戏对象列表中没用过的位置
	for (i = 0; i < GAME_OBJ_NUM_MAX; i++)
	{
		GameObj* pInst = sGameObjList + i;

		// 找非活动对象的位置
		if (pInst->flag == 0)
		{
			// 找到了
			pInst->pObject = sGameObjBaseList + type;
			pInst->flag = FLAG_ACTIVE;
			pInst->scale = scale;
			pInst->posCurr = pPos ? *pPos : zero;
			pInst->velCurr = pVel ? *pVel : zero;
			pInst->dirCurr = dir;

			// 返回新创建的对象实例
			return pInst;
		}
	}

	// 位置满了
	return 0;
}

// ---------------------------------------------------------------------------

void gameObjDestroy(GameObj* pInst)
{
	// 若已经销毁
	if (pInst->flag == 0)
		return;

	// 销毁
	pInst->flag = 0;
}
