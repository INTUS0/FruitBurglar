/* Project:		GSMPre
   File Name:	Level1.h
   Author:		INT US
   Date:		2015-9-15
   Purpose:		关卡1的属性和函数声明*/

#ifndef _Level1
#define _Level1

//------------------------------------------------------------------------------
// Typedef:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Consts:
//------------------------------------------------------------------------------

/*
enum OBJ_TYPE
{
	//游戏对象列表
	TYPE_BURGLAR = 0,
	TYPE_STONE,
	TYPE_TRAP,
	TYPE_BOSS,
	TYPE_DOG,
	TYPE_STRAWBERRY,
	TYPE_WATERMELON,
	TYPE_NUM
};
*/
#define TYPE_BURGLAR  0
#define TYPE_STONE  1
#define TYPE_STONE_STATIC 2
#define TYPE_TRAP_IN  3
#define TYPE_TRAP_OUT 4
#define TYPE_BOSS  5
#define TYPE_DOG  6
#define TYPE_STRAWBERRY 7
#define TYPE_WATERMELON 8
#define TYPE_APPLE 9
#define TYPE_BOSSBLOOD 10
#define TYPE_BURGLARBLOOD 11
#define TYPE_BURGLARLIVES 12
#define TYPE_MAP 13
#define TYPE_BURGLARSCORES 14
#define TYPE_LOSE 15
#define TYPE_WIN 16
#define TYPE_NUM 17

//------------------------------------------------------------------------------
// Public Structures

//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

// 6个状态函数
void Load1(void);
void Ini1(void);
void Update1(void);
void Draw1(void);
void Free1(void);
void Unload1(void);

#endif