/* Project:		GSMWinStep
File Name:	Level1.c
Author(former):		刘芳
Author  INT US
Date:		2015-9-15 -- 2015-1-20
Purpose:		关卡1  */

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
#define GAME_OBJ_BASE_NUM_MAX	32			// 对象类型（对象基类）数目上限
#define GAME_OBJ_NUM_MAX		4096		// 对象数目上限

#define BURGLAR_INITIAL_NUM			3		// 主角的lives数目
#define FRUIT_NUM				3		// 水果数目
#define BURGLAR_SIZE					30.0f	// 主角尺寸
#define BURGLER_ACCEL_FORWARD			50.0f	// 主角前向加速度(m/s^2)
#define BURGLAR_ACCEL_BACKWARD			-100.0f	// 主角后向加速度(m/s^2)

#define FLAG_ACTIVE					0x00000001  // 活动对象标志

#define CollisionMin                1   //地图中大于1的位置均不能通过   
//--------------------------------------------------------------------------------
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
	AEGfxTexture        *pTex;      //纹理

	Matrix2D           transform;   //变换矩阵，每一帧都需要为一个对象计算
}GameObj;


//方块图类
typedef struct Mapdata{
	int **mapData, **CollisionData;//地图数据和碰撞数据
	int BINARY_MAP_HEIGHT, BINARY_MAP_WIDTH;//地图的高和宽
}Map;

//private variables
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

//水果对象们
static GameObj* pStrawberry;
static GameObj* pWatermelon;
static GameObj* pApple;

//农场主对象
static GameObj* pBoss;

//狗对象
static GameObj* pDog[5];

//地图对象
static GameObj* Level1_Map;
Map *pMap;




static int BurglarLives;     //小盗生命数
static int BurglarBlood;     //小盗血量
static unsigned long sScore; //捡到的水果数

//水果数量
static int Fruit_NUM = 0;
//水果产生时间间隔
static int TimeTot = 0;
//临时定义狗运动时间
static int TimeTot1 = 0;

//BOSS运行标记
static int BossDirectionX = 0;
static int BossDirectionY = 0;


//狗数量
static int DOG_NUM = 5;

//农场主血量
static int BossBlood;

//捡到的石头数量
static int StoneCount=0;

//帧率控制常量
static int AnimationCounter = 0; // 控制播放帧
static int AnimationFPS = 12;
static float ElapsedTime = 0.0f;


//------------------------------------------------------------------------------
// Private Variables monkey:
//------------------------------------------------------------------------------
static AEGfxTexture *pTex1;		// 对象Burglar的纹理
static AEGfxVertexList*	pMesh1;	// 对象Burglar的网格(mesh)模型
static float obj1X, obj1Y;		// 对象burglar的位置
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


static AEGfxTexture *pTex2;		//对象dog的纹理
static AEGfxTexture *pTexStone; //移动石头纹理
static AEGfxTexture *pTexTrap; //陷阱纹理
static AEGfxTexture *pTexMap;	//对象Map的纹理
static AEGfxTexture *pTex_Bossblood;		//BOSS血量的纹理
static AEGfxTexture *pTexStrawberry;		//草莓的纹理
static AEGfxTexture *pTexWatermelon;		//西瓜的纹理
static AEGfxTexture *pTexApple;		//苹果的纹理

static float MapX, MapY;		// 对象Map的位置
//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------
// 创建/删除游戏对象
static GameObj*		gameObjCreate(unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir);
static void			gameObjDestroy(GameObj* pInst);

//地图读取及地图坐标转换
static void mapLoad(Map* pMap);
int WorldTransToMapdata(Map *pMap, Vector2D *pVec, int choose);

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

void Load1(void)
{
	GameObjBase* pObjBase;

	// 初始化游戏对象基类的实例列表
	memset(sGameObjBaseList, 0, sizeof(GameObjBase) * GAME_OBJ_BASE_NUM_MAX);
	sGameObjBaseNum = 0;

	//地图信息读取
	pMap = (Map *)malloc(sizeof(struct Mapdata));
	mapLoad(pMap);

	BurglarLives = 3;   //初始化小盗生命数

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
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
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
	// 静止被捡起的石头：尺寸很小，简化成三角形定义
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
	// 看得见的陷阱
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
	pTexTrap = AEGfxTextureLoad("trap.png");; //陷阱纹理

	//看不见的陷阱
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
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 1.0f, 1.0f,
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 1.0f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
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
		-15.0f, -15.0f, 0x00FF00FF, 0.0f, 1.0f,
		15.0f, -15.0f, 0x00FFFF00, 1.0f, 1.0f,
		-15.0f, 15.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		15.0f, -15.0f, 0x00FFFFFF, 1.0f, 1.0f,
		15.0f, 15.0f, 0x00FFFFFF, 1.0f, 0.0f,
		-15.0f, 15.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");
	pTexStrawberry = AEGfxTextureLoad("straw.png");//载入纹理

	// ========================
	// 苹果：两个三角形拼接的菱形
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
	pTexApple = AEGfxTextureLoad("apple.png");//载入纹理

	// ========================
	// 西瓜
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
	pTexWatermelon = AEGfxTextureLoad("watermelon.png");//载入纹理


	// ========================
	// BOSS血量：两个三角形拼接的长方形
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BOSSBLOOD;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-25.0f, 3.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 黑色
		25.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 红色
		-25.0f, -3.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF 白色
	AEGfxTriAdd(
		25.0f, -3.0f, 0x01FF0000, 1.0f, 1.0f,
		25.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,
		-25.0f, -3.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// 小盗血量：两个三角形拼接的长方形
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARBLOOD;

	AEGfxMeshStart();
	AEGfxTriAdd(
	    -25.0f, 3.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 黑色
		25.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 红色
		-25.0f, -3.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF 白色
	AEGfxTriAdd(
		25.0f, -3.0f, 0x01FF0000, 1.0f, 1.0f,
		25.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,
		-25.0f, -3.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// 小盗生命数：两个三角形拼接的长方形
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARLIVES;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-36.0f, 12.0f, 0x01FF0000, 0.0f, 0.0f,    //0x01FF0000 黑色
		36.0f, 12.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 红色
		-36.0f, -12.0f, 0xFFFFFFFF, 0.0f, 1.0f);   //0xFFFFFFFF 白色
	AEGfxTriAdd(
		36.0f, -12.0f, 0x01FF0000, 1.0f, 1.0f,
		36.0f, 12.0f, 0xFFFF0000, 1.0f, 0.0f,
		-36.0f, -12.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// =====================
	//地图
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
	// 小盗分数：两个三角形拼接的长方形
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLARSCORES;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-50.0f, 3.0f, 0xFFFF0000, 0.0f, 0.0f,    //0x01FF0000 黑色
		50.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 红色
		-50.0f, -3.0f, 0xFFFF0000, 0.0f, 1.0f);   //0xFFFFFFFF 白色
	AEGfxTriAdd(
		50.0f, -3.0f, 0xFFFF0000, 1.0f, 1.0f,
		50.0f, 3.0f, 0xFFFF0000, 1.0f, 0.0f,
		-50.0f, -3.0f, 0xFFFF0000, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// 失败页面
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_LOSE;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-320.0f, 240.0f, 0xFFFF0000, 0.0f, 0.0f,    //0x01FF0000 黑色
		320.0f, 240.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 红色
		-320.0f, -240.0f, 0xFFFF0000, 0.0f, 1.0f);   //0xFFFFFFFF 白色
	AEGfxTriAdd(
		320.0f, -240.0f, 0xFFFF0000, 1.0f, 1.0f,
		320.0f, 240.0f, 0xFFFF0000, 1.0f, 0.0f,
		-320.0f, -240.0f, 0xFFFF0000, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// ========================
	// 胜利页面
	// ========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_WIN;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-320.0f, 240.0f, 0xFFFF0000, 0.0f, 0.0f,    //0x01FF0000 黑色
		320.0f, 240.0f, 0xFFFF0000, 1.0f, 0.0f,   //0xFFFF0000 红色
		-320.0f, -240.0f, 0xFFFF0000, 0.0f, 1.0f);   //0xFFFFFFFF 白色
	AEGfxTriAdd(
		320.0f, -240.0f, 0xFFFF0000, 1.0f, 1.0f,
		320.0f, 240.0f, 0xFFFF0000, 1.0f, 0.0f,
		-320.0f, -240.0f, 0xFFFF0000, 0.0f, 1.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create Asteroid object!!");

	// 签到
	fprintf(fp, "Level1:Load\n");




}

void Ini1(void)
{
	
	//水果数量
	Fruit_NUM = 0;
	//水果产生时间间隔
	TimeTot = 0;
	//临时定义狗运动时间
	TimeTot1 = 0;
	

	// 主角的初始位置
	obj1X = -50.0f;
	obj1Y = 0.0f;
	//地图的初始位置
	MapX = 0.0f;
	MapY = 0.0f;

	memset(sGameObjList, 0, sizeof(GameObj)*GAME_OBJ_NUM_MAX);
	GameObj* pObj;
	int i;

	BossBlood = 2;      //初始化BOSS血量
	pTex_Bossblood = AEGfxTextureLoad("full_live_boss.png");  //载入boss血量纹理

	BurglarBlood = 3;   //初始化小盗血量

	// 为开始画对象做准备
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	// 对象实例化：游戏开始只有小盗和狗和农场主需要实例化
	// 小盗对象实例化
	Burglar = gameObjCreate(TYPE_BURGLAR, BURGLAR_SIZE, 0, 0, 0.0f);
	AE_ASSERT(Burglar);
	Burglar->posCurr.x = AEGfxGetWinMaxX();
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

	//初始化看不见的陷阱
	for (int a = 0; a < 3; a++)
	{
		pObj = gameObjCreate(TYPE_TRAP_IN, 10.0f, 0, 0, 0.0f);
		AE_ASSERT(pObj);
		//初始化陷阱位置及朝向比例

		pObj->posCurr.x = a*100.0f;
		pObj->posCurr.y = -300.0f;
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
	pBoss->pTex = AEGfxTextureLoad("Boss.png");

	//狗对象实例化 并 初始化
	for (i = 0; i < DOG_NUM; i++)
	{
		// 实例化
		pObj = gameObjCreate(TYPE_DOG, 10.0f, 0, 0, 0.0f);
		pDog[i] = pObj;//给狗分配指针
		AE_ASSERT(pObj);

		// 初始化: 坐标位置 朝向和尺寸大小
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

	
	//农场主血量初始化
	pObj = gameObjCreate(TYPE_BOSSBLOOD, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//初始化血量位置
	pObj->posCurr.x = pBoss->posCurr.x;
	pObj->posCurr.y = pBoss->posCurr.y + 35.0f;

	//主角血量初始化
	pObj = gameObjCreate(TYPE_BURGLARBLOOD, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//初始化血量位置及纹理
	pObj->posCurr.x = Burglar->posCurr.x;
	pObj->posCurr.y = Burglar->posCurr.y + 35.0f;
	pObj->pTex = AEGfxTextureLoad("life_full.png");

	//主角命数初始化
	pObj = gameObjCreate(TYPE_BURGLARLIVES, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//初始化生命数显示位置及纹理
	pObj->posCurr.x = AEGfxGetWinMinX() + 60.0f;
	pObj->posCurr.y = AEGfxGetWinMaxY() - 15.0f;
	if (BurglarLives == 3)
		pObj->pTex = AEGfxTextureLoad("Burglar_lives3.png");
	else if (BurglarLives == 2)
		pObj->pTex = AEGfxTextureLoad("Burglar_lives2.png");
	else if (BurglarLives == 1)
		pObj->pTex = AEGfxTextureLoad("Burglar_lives1.png");

	//初始化主角分数
	pObj = gameObjCreate(TYPE_BURGLARSCORES, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//初始化生命数显示位置
	pObj->posCurr.x = AEGfxGetWinMinX() + 60.0f;
	pObj->posCurr.y = AEGfxGetWinMaxY() - 40.0f;

	//初始化失败页面
	pObj = gameObjCreate(TYPE_LOSE, 1.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	pObj->posCurr.x = 0.0f;
	pObj->posCurr.y = 0.0f;
	pObj->pTex = AEGfxTextureLoad("gameover.png");

	//初始化胜利页面
	pObj = gameObjCreate(TYPE_WIN, 1.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	pObj->posCurr.x = 0.0f;
	pObj->posCurr.y = 0.0f;
	pObj->pTex = AEGfxTextureLoad("win.png");

	// 地图对象实例化
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
    
	stempBurglar.x = obj1X;
	stempBurglar.y = obj1Y;

	if (WorldTransToMapdata(pMap,&stempBurglar, 1)==0)
	{
		obj1X = obj2X;
		obj1Y = obj2Y;
	}


	//鼠标左键控制石头生成
	if ((KeyPressed[KeyLeftBottom]) && (Burglar->flag& FLAG_ACTIVE) && (StoneCount>0))
	{
		
		pStone = gameObjCreate(TYPE_STONE, 3.0f, 0, 0, 0.0f);;
		AE_ASSERT(pStone);
		// 实例化
		// 初始化: 坐标位置 朝向和尺寸大小
		float Xx = Burglar->posCurr.x;
		float Yy = Burglar->posCurr.y;
		//在主角位置产生，射向鼠标位置
		pStone->posCurr.x += Xx;
		pStone->posCurr.y += Yy;
		
		//石头减一
		StoneCount -= 1;	
	}


	//随机产生水果，每隔3秒产生一个，一个界面最多产生10个水果
	TimeTot++;
	if (TimeTot == 180 && Fruit_NUM<10)
	{
	loop:;

		int Xx, Yy;

		Xx = rand();
		Yy = rand();

		int FlagFruit;

		FlagFruit = ((int)rand())%3;
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
		if (FlagFruit == 2)
		{
			GameObj* pFruit = gameObjCreate(TYPE_WATERMELON, 3.0f, 0, 0, 0.0f);
			// 初始化: 坐标位置 朝向和尺寸大小
			pFruit->posCurr.x = Xx;
			pFruit->posCurr.y = Yy;
			//判断是否能够生成
			if (WorldTransToMapdata(pMap, &pFruit->posCurr, 1) == 1)
			{
				Fruit_NUM++;
				TimeTot = 0;

				AE_ASSERT(pFruit);
				// 实例化

				pFruit->scale = 5.0f;
			}
			else
			{
				gameObjDestroy(pFruit);//如果不能生成则销毁
				goto loop;
			}
		}
		else if (FlagFruit == 1)
		{
			GameObj* pFruit = gameObjCreate(TYPE_STRAWBERRY, 3.0f, 0, 0, 0.0f);
			// 初始化: 坐标位置 朝向和尺寸大小
			pFruit->posCurr.x = Xx;
			pFruit->posCurr.y = Yy;
			//判断是否能够生成
			if (WorldTransToMapdata(pMap, &pFruit->posCurr, 1) == 1)
			{
				Fruit_NUM++;
				TimeTot = 0;

				AE_ASSERT(pFruit);
				// 实例化

				pFruit->scale = 5.0f;
			}
			else
			{
				gameObjDestroy(pFruit);//如果不能生成则销毁
				goto loop;
			}
		}
		else if (FlagFruit == 0)
		{
			GameObj* pFruit = gameObjCreate(TYPE_APPLE, 3.0f, 0, 0, 0.0f);
			// 初始化: 坐标位置 朝向和尺寸大小
			pFruit->posCurr.x = Xx;
			pFruit->posCurr.y = Yy;
			//判断是否能够生成
			if (WorldTransToMapdata(pMap, &pFruit->posCurr, 1) == 1)
			{
				Fruit_NUM++;
				TimeTot = 0;

				AE_ASSERT(pFruit);
				// 实例化

				pFruit->scale = 5.0f;
			}
			else
			{
				gameObjDestroy(pFruit);//如果不能生成则销毁
				goto loop;
			}
		}
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
				if (pObj0->flag && pObj0->pObject->type == TYPE_BOSS)
				{
					if (StaticPointToStaticCircle(&pObj->posCurr, &pObj0->posCurr, 10.0))
					{
						gameObjDestroy(pObj);
						BossBlood--;
						pTex_Bossblood = AEGfxTextureLoad("half_live_boss.png");  //载入boss血量纹理
					}
					if (BossBlood <= 0)
					{
						gameObjDestroy(pObj0);
						for (int k = 0; k < GAME_OBJ_NUM_MAX; k++)
						{     //销毁boss血量
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
						gameObjDestroy(Burglar);//发生碰撞，销毁对象
						BurglarLives -= 1;   //主角生命数减一
						if (BurglarLives > 0)
							Next = GS_Restart;
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
				//农场主跟着主角移动
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


			//发生碰撞，主角死亡
			if (StaticRectToStaticRect(&Burglar->posCurr, 15.0f, 15.0f, &pObj->posCurr, 15.0f, 15.0f))
			{
				gameObjDestroy(Burglar);
				BurglarLives -= 1;   //主角生命数减一
				if (BurglarLives > 0)
					Next = GS_Restart;
				Matrix2DScale(&pObj->transform, 0.5, 1);
			}
		}

		//狗的边界控制
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

			//是否与狗发生碰撞
			if (StaticRectToStaticRect(&Burglar->posCurr, 15, 20, &pObj->posCurr, 20, 20) && pObj->flag)
			{  //与狗发生碰撞，血量减一
				BurglarBlood -= 1;
				if (BurglarBlood <= 0)
				{
					BurglarLives -= 1;
					if (BurglarLives > 0)
						Next = GS_Restart;
				}
			}
		}

		//捡西瓜
		if (pObj->flag&&pObj->pObject->type == TYPE_WATERMELON)
		{
			if (StaticRectToStaticRect(&Burglar->posCurr, 15.0,20.0,&pObj->posCurr, 10.0,10.0))
			{
				gameObjDestroy(pObj);
				Fruit_NUM -= 1;
				sScore += 20;
			}
		}

		//捡到草莓
		if (pObj->flag&&pObj->pObject->type == TYPE_STRAWBERRY)
		{
			if (StaticRectToStaticRect(&Burglar->posCurr, 15.0, 20.0, &pObj->posCurr, 10.0, 10.0))
			{
				gameObjDestroy(pObj);
				Fruit_NUM -= 1;
				sScore += 10;
			}
		}

		//捡到苹果
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
			//更新血量位置
			pObj->posCurr.x = Burglar->posCurr.x;
			pObj->posCurr.y = Burglar->posCurr.y + 35.0f;
			if (BurglarBlood == 3)
			    pObj->pTex = AEGfxTextureLoad("life_full.png");//载入全血纹理
			else if(BurglarBlood==2)
				pObj->pTex = AEGfxTextureLoad("life_warning.png");//载入半血纹理
			else if(BurglarBlood == 1)
				pObj->pTex = AEGfxTextureLoad("life_dangerous.png");//载入四分之一血纹理

			if (!Burglar->flag&FLAG_ACTIVE)
			{
				gameObjDestroy(pObj);   //销毁主角的同时，销毁主角的血量 
			}
		}

		if (pObj->flag && pObj->pObject->type == TYPE_BOSSBLOOD)
		{
			//更新主角血量位置
			pObj->posCurr.x = pBoss->posCurr.x;
			pObj->posCurr.y = pBoss->posCurr.y + 35.0f;
			
		}

	}//for
	/*
	//狗的运动
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
	
	//狗的运动by小明
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

	//狗的运动SLL-1
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

	//狗的运动SLL-2
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

	//狗的运动SLL-3
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

	//狗的运动SLL-4
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

	//狗的运动SLL-5
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

	


	//按空格键
	if (KeyPressed[KeySpace])
	{
		StoneCount = 100;
	}
	// 输入重置
	Input_Initialize();

	// 签到
	fprintf(fp, "Level1:Update\n");

}

void Draw1(void)
{
	unsigned long i;

	//单独画地图
	if (Level1_Map->flag&FLAG_ACTIVE)
	{
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);   // 必须最先设置绘制模式为纹理
		AEGfxSetPosition(0.0f, 0.0f);
		// Set texture for object background
		AEGfxTextureSet(pTexMap, 0.0f, 0.0f); // 参数1：纹理，偏移量(x,y)
		AEGfxSetTransparency(1.0f);
		AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
		// Drawing the mesh (list of triangles)
		AEGfxMeshDraw(Level1_Map->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
	}


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
			// 纹理
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			// 画对象狗
			AEGfxSetTransparency(1);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//创建BOSS血量对象
		if (pInst->pObject->type == TYPE_BOSSBLOOD)
		{
			// 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置血量的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 纹理
			AEGfxTextureSet(pTex_Bossblood, 0.0f, 0.0f);
			// 画血量
			AEGfxSetTransparency(1.0f);
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
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置西瓜的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 无纹理
			AEGfxTextureSet(pTexWatermelon, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// 画对象西瓜
			//AEGfxSetTransparency(1);
			//AEGfxSetBlendColor(1.0f, 1.0f, 1.0, 1.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//绘制草莓对象
		if (pInst->pObject->type == TYPE_STRAWBERRY)
		{
			// 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置西瓜的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 无纹理
			AEGfxTextureSet(pTexStrawberry, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// 画对象西瓜
			//AEGfxSetTransparency(1);
			//AEGfxSetBlendColor(1.0f, 1.0f, 1.0, 1.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		//绘制苹果对象
		if (pInst->pObject->type == TYPE_APPLE)
		{
			// 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置西瓜的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 无纹理
			AEGfxTextureSet(pTexApple, 0.0f, 0.0f);
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
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置西瓜的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 无纹理
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
			// 画主角血量
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		if (pInst->pObject->type == TYPE_BURGLARLIVES)
		{  //创建小盗生命数
			// 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置小盗生命数的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 纹理
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			// 画生命数
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		/*if (pInst->pObject->type == TYPE_BURGLARSCORES)
		{  //创建小盗分数
			// 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			// 设置小盗分数的坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 纹理
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			// 画分数
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}*/

		if (pInst->pObject->type == TYPE_LOSE && BurglarLives <= 0)
		{  //创建失败页面
		   // 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 纹理
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			// 画失败页面
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}

		if (pInst->pObject->type == TYPE_WIN && sScore >= 50)
		{  //创建胜利页面
		   // 设置绘制模式(Color or texture)
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			// 设置坐标位置
			AEGfxSetPosition(pInst->posCurr.x, pInst->posCurr.y);
			// 纹理
			AEGfxTextureSet(pInst->pTex, 0.0f, 0.0f);
			// 画胜利页面
			AEGfxSetTransparency(1.0f);
			AEGfxSetBlendColor(0.0f, 0.0f, 0.0, 0.0f);
			AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
		}
		
	}
	// 签到
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



	// 签到
	fprintf(fp, "Level1:Free\n");

}

void Unload1(void)
{
	AEGfxMeshFree(Burglar->pObject->pMesh);		// 注销创建的对象
	//AEGfxTextureUnload(pTex1); //销毁猴子

	//释放地图占的存储空间
	for (int i = 0; i < pMap->BINARY_MAP_HEIGHT; ++i)
		free(pMap->mapData[i]);// = (int *)malloc((pMap->BINARY_MAP_WIDTH)* sizeof(int *));
	free(pMap->mapData);//= (int **)malloc((pMap->BINARY_MAP_HEIGHT)* sizeof(int *));
	for (int i = 0; i < pMap->BINARY_MAP_HEIGHT; ++i)
		free(pMap->CollisionData[i]);// = (int *)malloc((pMap->BINARY_MAP_WIDTH)* sizeof(int *));
	free(pMap->CollisionData);
	free(pMap);
	
	// 签到
	fprintf(fp, "Level1:Unload\n");
}

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

void mapLoad(Map* pMap)//地图读取
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
	fscanf(fp, "%d", &W);//读高度
	//W = (int *)malloc(sizeof(int));
	fscanf(fp, "%d", &H);//读宽度
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
		for (j = 0; j < pMap->BINARY_MAP_WIDTH; ++j)//读mapdata
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
			if (pMap->mapData[i][j] > CollisionMin)//如果大于边界碰撞检测数值
				pMap->CollisionData[i][j] = 0;//设置collisiondata为零
			else pMap->CollisionData[i][j] = pMap->mapData[i][j];
		}
	fclose(fp);
}

/*void MapTransform(GameObj* pInst, int MAP_WIDTH, int MAP_HEIGHT)//地图位置计算
{
//中间变量
Matrix2D mapTrans;
Matrix2D mapScale;
Matrix2D mapTransform;

//地图变换矩阵的计算
Matrix2DIdentity(&mapTrans);
Matrix2DTranslate(&mapTrans, -MAP_WIDTH / 2.f, -MAP_HEIGHT / 2.f);
Matrix2DIdentity(&mapScale);
Matrix2DScale(&mapScale, 20, 20);
Matrix2DConcat(&mapTransform, &mapScale, &mapTrans);
}*/

int WorldTransToMapdata(Map *pMap, Vector2D *pVec, int choose)
{
	//单元格大小计算
	float HEIGHT = 600.0f / (pMap->BINARY_MAP_HEIGHT);
	float WIDTH = 800.0f / (pMap->BINARY_MAP_WIDTH);

	int Xx = pVec->x;
	int Yy = pVec->y;
	//计算世界坐标在地图坐标中的位置
	int   MapdataX = (Xx + 400.0f) / WIDTH;
	int   MapdataY = (Yy + 300.0f) / HEIGHT;

	//返回碰撞数据
	if ((0 <= MapdataX) &&(MapdataX <= 29) && (0 <= MapdataY)&&(MapdataY <= 19)){
		//返回碰撞数据
		if (choose == 1)
			return (pMap->CollisionData[MapdataY][MapdataX]);
		else if (choose == 2)
			return (pMap->mapData[MapdataY][MapdataX]);
	}
	else return NULL;
}