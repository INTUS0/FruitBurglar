/* Project:		GSMPre
   File Name:	GameStateManager.h
   Author:		����
   Date:		2015-9-15
   Purpose:		��Ϸ״̬������*/

#ifndef _GameStateManager
#define _GameStateManager

//------------------------------------------------------------------------------
// Typedef:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Consts:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Structures:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

// ��Ϸ״̬�������ĳ�ʼ��
void GSM_Initialize(int);

// ��Ϸ״̬�������ĸ���
void GSM_Update(int);

// 6 Function Pointers
void (*pLoad)(void);
void (*pIni)(void);
void (*pUpdate)(void);
void (*pDraw)(void);
void (*pFree)(void);
void (*pUnload)(void);

#endif