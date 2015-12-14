/* Project:		GSMPre
   File Name:	Level1.h
   Author:		����
   Date:		2015-9-15
   Purpose:		�ؿ�1�����Ժͺ�������*/

#ifndef _Level1
#define _Level1

//------------------------------------------------------------------------------
// Typedef:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Consts:
//------------------------------------------------------------------------------

/*
enum OBJ_TYPE{	//��Ϸ�����б�	TYPE_BURGLAR = 0,	TYPE_STONE,	TYPE_TRAP,	TYPE_BOSS,	TYPE_DOG,	TYPE_STRAWBERRY,	TYPE_WATERMELON,	TYPE_NUM};
*/
#define TYPE_BURGLAR  0
#define TYPE_STONE  1
#define TYPE_STONE_STATIC 2
#define TYPE_TRAP  3
#define TYPE_BOSS  4
#define TYPE_DOG  5
#define TYPE_STRAWBERRY 6
#define TYPE_WATERMELON 7
#define TYPE_NUM 8

//------------------------------------------------------------------------------
// Public Structures

//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

// 6��״̬����
void Load1(void);
void Ini1(void);
void Update1(void);
void Draw1(void);
void Free1(void);
void Unload1(void);

#endif