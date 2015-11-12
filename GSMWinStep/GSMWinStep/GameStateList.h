/* Project:		GSMPre
   File Name:	GameStateList.h
   Author:		����
   Date:		2015-9-15
   Purpose:		��Ϸ״̬��������ҪΪGame Flow��״̬�л����� */

#ifndef _GameStateList
#define _GameStateList

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
enum
{
	GS_L1,              // Level1
	GS_L2,              // Level2
	GS_NUM,             // Total Number
	GS_Restart,			// Restart
	GS_Quit				// Quit
};

int Current, Previous, Next;  // ״ָ̬ʾ��

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

#endif