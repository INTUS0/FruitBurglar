/* Project:		GSMPre
   File Name:	Level2.h
   Author:		刘芳
   Date:		2015-9-15
   Purpose:		关卡2的属性和函数声明*/

#ifndef _Level2
#define _Level2

//------------------------------------------------------------------------------
// Typedef:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Consts:
//------------------------------------------------------------------------------
#define TYPE_BURGLAR  0
#define TYPE_STONE  1
#define TYPE_STONE_STATIC 2
#define TYPE_TRAP_IN  3
#define TYPE_TRAP_OUT 4
#define TYPE_BOSS  5
#define TYPE_DOG  6
#define TYPE_STRAWBERRY 7
#define TYPE_WATERMELON 8
#define TYPE_BOSSBLOOD 9
#define TYPE_BURGLARBLOOD 10
#define TYPE_BURGLARLIVES 11
#define TYPE_MAP 12
#define TYPE_NUM 13

//------------------------------------------------------------------------------
// Public Structures:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

// 关卡2的6个状态函数
void Load2(void);
void Ini2(void);
void Update2(void);
void Draw2(void);
void Free2(void);
void Unload2(void);

#endif