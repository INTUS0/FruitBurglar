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
#include "Matrix2D.h"
#include "Vector2D.h"
#include "Math2D.h"


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

	AEMtx33            transform;   //变换矩阵，每一帧都需要为一个对象计算
}GameObj;


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
GameObj* pStone;

//小盗血量
static int BurglarBlood;
static unsigned long sScore; //捡到的水果数

//水果数量
static int Fruit_NUM = 20;

//狗数量
static int DOG_NUM = 5;

//农场主血量
static int BossBlood;

//帧率控制常量
static int AnimationCounter = 0; // 控制播放帧
static int AnimationFPS = 12;
static float ElapsedTime = 0.0f;


//------------------------------------------------------------------------------
// Private Variables monkey:
//------------------------------------------------------------------------------
static AEGfxTexture *pTex1;		// 对象Burglar的纹理
static AEGfxVertexList*	pMesh1;				// 对象1的网格(mesh)模型
static float obj1X, obj1Y;		// 对象1的位置

static AEGfxTexture *pTex2;		//对象dog的纹理
static AEGfxTexture *pTexStone;

//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------
// 创建/删除游戏对象
static GameObj*		gameObjCreate(unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir);
static void			gameObjDestroy(GameObj* pInst);

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

void Load1(void)
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
		-20.0f, -20.0f, 0x00FF00FF, 0.0f, 1.0f,
		20.0f, -20.0f, 0x00FFFF00, 0.125f, 1.0f,
		-20.0f, 20.0f, 0x00F00FFF, 0.0f, 0.0f);
	AEGfxTriAdd(
		20.0f, -20.0f, 0x00FFFFFF, 0.125f, 1.0f,
		20.0f, 20.0f, 0x00FFFFFF, 0.125f, 0.0f,
		-20.0f, 20.0f, 0x00FFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create object!!");
	pTex1 = AEGfxTextureLoad("PlayerRun.png");//载入纹理

	// =======================
	// 石头：尺寸很小，简化成三角形定义
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

	// =========================
	// 陷阱：用六个三角形拼成一个“圆形”
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
	// 农场主：两个三角形拼接的菱形
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
	// 狗：两个三角形拼接的菱形
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
	// 西瓜：两个三角形拼接的菱形
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


	// 签到
	fprintf(fp, "Level1:Load\n");




}

void Ini1(void)
{

	// 对象1的初始位置
	obj1X = 0.0f;
	obj1Y = 0.0f;
	memset(sGameObjList, 0, sizeof(GameObj)*GAME_OBJ_NUM_MAX);
	GameObj* pObj;
	int i;

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

	//狗对象实例化 并 初始化
	for (i = 0; i < DOG_NUM; i++)
	{
		// 实例化
		pObj = gameObjCreate(TYPE_DOG, 10.0f, 0, 0, 0.0f);
		AE_ASSERT(pObj);

		// 初始化: 坐标位置 朝向和尺寸大小
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
	//初始化农场主
	pObj = gameObjCreate(TYPE_BOSS, 10.0f, 0, 0, 0.0f);
	AE_ASSERT(pObj);
	//初始化农场主位置及朝向比例
	pObj->posCurr.x = 100.0f;
	pObj->posCurr.y = 100;
	pObj->dirCurr = acosf(pObj->posCurr.x / ((float)sqrt(pObj->posCurr.x*pObj->posCurr.x + pObj->posCurr.y * pObj->posCurr.y))) - PI;

	pObj->scale = 10.0f;
	
}

void Update1(void)
{
	//unsigned long i;
	float winMaxX, winMaxY, winMinX, winMinY;
	double frameTime;

	
	//GameObj* pObj;//指向狗的指针

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

	
	// 对象移动
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
	if( (KeyPressed[KeyRightBottom] )&& (Burglar->flag& FLAG_ACTIVE))
	{
	//obj1X = posX;
	//obj1Y = posY;//鼠标右键坐标赋给石头
		pStone = gameObjCreate(TYPE_STONE, 3.0f, 0, 0, 0.0f);;
		AE_ASSERT(pStone);
		// 实例化

		// 初始化: 坐标位置 朝向和尺寸大小
		pStone->posCurr.x = posX;
		pStone->posCurr.y = posY;

		pStone->dirCurr = acosf(pStone->posCurr.x / ((float)sqrt(pStone->posCurr.x*pStone->posCurr.x + pStone->posCurr.y * pStone->posCurr.y))) - PI;

		pStone->scale = 5.0f;
	}

	//检测是否发生了碰撞
	for (int i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{
		GameObj* pObj = sGameObjList + i;
		if (pObj->flag&&pObj->pObject->type==TYPE_DOG)
		{
			//狗的运动
			if (pObj->posCurr.x>30)
			{
				pObj->velCurr.x = -0.5;
				pObj->velCurr.y = -0.5;
			}
			else if (pObj->posCurr.x<30)
			{
				pObj->velCurr.x = 0.5;
				pObj->velCurr.y = -0.5;
			}
			if (pObj->posCurr.y < -50 && pObj->posCurr.x<30)
			{
				pObj->velCurr.x = 0.5;
				pObj->velCurr.y = 0.5;
			}
			
			pObj->posCurr.x += pObj->velCurr.x;
			pObj->posCurr.y += pObj->velCurr.y;

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
		
			
	}
	if (KeyPressed[KeySpace])
	{
		pStone = gameObjCreate(TYPE_STONE, 10.0f, 0, 0, 0.0f);;
		AE_ASSERT(pStone);

		// 实例化

		// 初始化: 坐标位置 朝向和尺寸大小
		pStone->posCurr.x = (Burglar->posCurr.x+100.0f);
		pStone->posCurr.y = (Burglar->posCurr.y+10.0f);

		pStone->dirCurr = acosf(pStone->posCurr.x / ((float)sqrt(pStone->posCurr.x*pStone->posCurr.x + pStone->posCurr.y * pStone->posCurr.y))) - PI;

		pStone->scale = 5.0f;
	}
	// 输入重置
	Input_Initialize();

	// 签到
	fprintf(fp, "Level1:Update\n");




}

void Draw1(void)
{
	unsigned long i;
	// 画对象1
	
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

		//创建狗对象
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

		//创建石头对象
		if(( pInst->pObject->type == TYPE_STONE ) && ( Burglar->flag & FLAG_ACTIVE ))
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
		
	}
	// 签到
	fprintf(fp, "Level1:Draw\n");
}

void Free1(void)
{
	gameObjDestroy(Burglar);
	/*
	for (int i = 1; i < GAME_OBJ_NUM_MAX; i++)
	{
		GameObj* pObj = sGameObjList + i;
		if (pObj->flag)
			gameObjDestroy(pObj);
	}
	// 签到
	fprintf(fp, "Level1:Free\n");
	*/
}

void Unload1(void)
{
	AEGfxMeshFree(Burglar->pObject->pMesh);		// 注销创建的对象
	//AEGfxTextureUnload(pTex1); //销毁猴子

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