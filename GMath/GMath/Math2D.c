#include "Math2D.h"
#include "stdio.h"

/*
检测点pP是否与圆（圆心pCenter，半径pRadius）发生碰撞
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
检测点pPos是否与矩形（中心pRect，宽度Width，高度Height）发生碰撞
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
检测两个圆是否发生碰撞
Circle0: 圆心pCenter0，半径pRadius0
Circle1: 圆心pCenter1，半径pRadius1
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
检测两个矩形是否发生碰撞
Rectangle0: 中心pRect0，宽度Width0，高度Height0
Rectangle1: 中心pRect1，宽度Width1，高度Height1
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

	int CollisionFlag = 1;//碰撞标志
	if (LeftA > RightB) CollisionFlag = 0;
	if (LeftB>RightA) CollisionFlag = 0;
	if (TopA<BottomB) CollisionFlag = 0;
	if (TopB<BottomA) CollisionFlag = 0;


   return CollisionFlag;
}
