#include "Math2D.h"
#include "stdio.h"

/*
����pP�Ƿ���Բ��Բ��pCenter���뾶pRadius��������ײ
*/
int StaticPointToStaticCircle(Vector2D *pP, Vector2D *pCenter, float Radius)
{
	int CollisionFlag = 0;
	float distance;
	distance =Vector2DDistance(pP, pCenter);
	if (distance <= Radius) CollisionFlag = 1;
	return CollisionFlag;
}


/*
����pPos�Ƿ�����Σ�����pRect�����Width���߶�Height��������ײ
*/
int StaticPointToStaticRect(Vector2D *pPos, Vector2D *pRect, float Width, float Height)
{
	int CollisionFlag = 1;
	float left = 0, right = 0, top = 0, bottom = 0;
	left = pRect->x - Width / 2.0;
	right = pRect->x + Width / 2.0;
	top = pRect->y + Height / 2.0;
	bottom = pRect->y - Height / 2.0;

	if (pPos->x < left) CollisionFlag = 0;
	if (pPos->x > right) CollisionFlag = 0;
	if (pPos->y < bottom) CollisionFlag = 0;
	if (pPos->y > top) CollisionFlag = 0;
	return CollisionFlag; 
}

/*
�������Բ�Ƿ�����ײ
Circle0: Բ��pCenter0���뾶pRadius0
Circle1: Բ��pCenter1���뾶pRadius1
*/
int StaticCircleToStaticCircle(Vector2D *pCenter0, float Radius0, Vector2D *pCenter1, float Radius1)
{
	int CollisionFlag = 0;
	float TempDistance = 0.0f;
	TempDistance = Vector2DDistance(pCenter0,pCenter1);
	if (TempDistance <= Radius0 + Radius1);
	{
	    CollisionFlag =1;
	}
  return CollisionFlag;
}

/*
������������Ƿ�����ײ
Rectangle0: ����pRect0�����Width0���߶�Height0
Rectangle1: ����pRect1�����Width1���߶�Height1
*/
int StaticRectToStaticRect(Vector2D *pRect0, float Width0, float Height0, Vector2D *pRect1, float Width1, float Height1)
{
	float LeftA = 0, LeftB = 0;
	float RightA = 0, RightB = 0;
	float TopA = 0, TopB = 0;
	float BottomA = 0, BottomB = 0;
	LeftA = pRect0->x - Width0 / 2.0;
	RightA = pRect0->x + Width0 / 2.0;
	TopA = pRect0->y + Height0 / 2.0;
	BottomA = pRect0->y - Height0 / 2.0;

	LeftB = pRect1->x - Width1 / 2.0;
	RightB = pRect1->x + Width1 / 2.0;
	TopB = pRect1->y + Height1 / 2.0;
	BottomB = pRect1->y - Height1/ 2.0;

	int CollisionFlag = 1;//��ײ��־
	if (LeftA > RightB) CollisionFlag = 0;
	if (LeftB>RightA) CollisionFlag = 0;
	if (TopA<BottomB) CollisionFlag = 0;
	if (TopB<BottomA) CollisionFlag = 0;


   return CollisionFlag;
}
