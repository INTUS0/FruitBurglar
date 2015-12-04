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
#define GAME_OBJ_BASE_NUM_MAX	32			// 对象类型（对象基类）数目上限#define GAME_OBJ_NUM_MAX		2048		// 对象数目上限

//------------------------------------------------------------------------------
// Private Structures:
//------------------------------------------------------------------------------

//游戏对象基类typedef struct{	unsigned long		type;		// 游戏对象类型	AEGfxVertexList*	pMesh;		// 形状}GameObjBase;//游戏对象类typedef struct{	GameObjBase*       pObject;     //指向基类	unsigned long      flag;        //活动标志	float              scale;       //尺寸	AEVec2             posCurr;     //当前位置	AEVec2             velCurr;     //当前速度	float              dirCurr;     //当前方向	AEMtx33            transform;   //变换矩阵，每一帧都需要为一个对象计算}GameObj;//private variables// 游戏对象基类（类型）列表static GameObjBase		sGameObjBaseList[GAME_OBJ_BASE_NUM_MAX];	// 该数组中的元素是游戏对象基类的实例：形状和类型static unsigned long	sGameObjBaseNum;							// 已定义的游戏对象基类// 游戏对象列表static GameObj			sGameObjList[GAME_OBJ_NUM_MAX];				// 该数组中的元素是游戏对象的实例static unsigned long	sGameObjNum;								// 游戏对象的个数//小盗对象,playerstatic GameObj* Burglar;//小盗血量static int BurglarBlood;//水果数量static int FruitNumber;//狗数量static int DogNumber;//农场主血量static int BossBlood;

//------------------------------------------------------------------------------
// Private Variables monkey:
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
	
	/*
	//新代码，初始化对象
	GameObjBase* pObjBase;

	// 初始化游戏对象基类的实例列表
	memset(sGameObjBaseList, 0, sizeof(GameObjBase) * GAME_OBJ_BASE_NUM_MAX);
	sGameObjBaseNum = 0;

	// 创建基类的实例	
	// =====================
	// Burglar -主角
	// =====================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_BURGLAR;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f, 0.5f, 0x01FF0000, 0.0f, 0.0f,
		-0.5f, -0.5f, 0xFFFF0000, 0.0f, 0.0f,
		0.5f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);
	pObjBase->pMesh = AEGfxMeshEnd();
	AE_ASSERT_MESG(pObjBase->pMesh, "Failed to create object!!");

	// =======================
	// 石头：尺寸很小，简化成三角形定义
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
	//狗：六个三角形弄成“圆”
	// =========================
	pObjBase = sGameObjBaseList + sGameObjBaseNum++;
	pObjBase->type = TYPE_DOG;

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
	// 导弹：两个三角形拼接的菱形
	//水果：西瓜
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
	*/
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
	

	//新代码，画对象


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

	if (KeyPressed[KeyRightBottom])
	{
		
	}
	
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
/*
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------
GameObj* gameObjCreate(unsigned long type, float scale, AEVec2* pPos, AEVec2* pVel, float dir)
{
	unsigned long i;
	AEVec2 zero = { 0.0f, 0.0f };

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
*/