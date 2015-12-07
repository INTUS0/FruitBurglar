#include "Vector2D.h"

#define EPSILON 0.0001
#define PIToRad 3.1415926535897/180.0

// ---------------------------------------------------------------------------

void Vector2DZero(Vector2D *pResult)
{
	pResult->x= 0.0f;
	pResult->y = 0.0f;

}

// ---------------------------------------------------------------------------

void Vector2DSet(Vector2D *pResult, float x, float y)
{
	pResult->x = x;
	pResult->y = y;
}

// ---------------------------------------------------------------------------

void Vector2DNeg(Vector2D *pResult, Vector2D *pVec0)
{
	pResult->x = 0.0-pVec0->x;
	pResult->y = 0.0-pVec0->y;
}

// ---------------------------------------------------------------------------

void Vector2DAdd(Vector2D *pResult, Vector2D *pVec0, Vector2D *pVec1)
{
	pResult->x = pVec0->x + pVec1->x;
	pResult->y = pVec0->y + pVec1->y;
}

// ---------------------------------------------------------------------------

void Vector2DSub(Vector2D *pResult, Vector2D *pVec0, Vector2D *pVec1)
{
	pResult->x = pVec1->x - pVec0->x;
	pResult->y = pVec1->y - pVec0->y;
}

// ---------------------------------------------------------------------------

void Vector2DNormalize(Vector2D *pResult, Vector2D *pVec0)
{
	pResult->x = pResult->x / Vector2DLength(pVec0);
	pResult->y = pResult->y / Vector2DLength(pVec0);

}

// ---------------------------------------------------------------------------

void Vector2DScale(Vector2D *pResult, Vector2D *pVec0, float c)
{
	pResult->x = c*pVec0->x;
	pResult->y = c*pVec0->y;
}

// ---------------------------------------------------------------------------

void Vector2DScaleAdd(Vector2D *pResult, Vector2D *pVec0, Vector2D *pVec1, float c)
{
	pResult->x = c*pVec0->x+ pVec1->x;
	pResult->y = c*pVec0->y+ pVec1->y;
}

// ---------------------------------------------------------------------------

void Vector2DScaleSub(Vector2D *pResult, Vector2D *pVec0, Vector2D *pVec1, float c)
{
	pResult->x = c*pVec0->x - pVec1->x;
	pResult->y = c*pVec0->y - pVec1->y;
}

// ---------------------------------------------------------------------------

float Vector2DLength(Vector2D *pVec0)
{
	return sqrt(pVec0->x*pVec0->x + pVec0->y*pVec0->y);
}

// ---------------------------------------------------------------------------

float Vector2DSquareLength(Vector2D *pVec0)
{
	return (pVec0->x*pVec0->x + pVec0->y*pVec0->y);
}

// ---------------------------------------------------------------------------

float Vector2DDistance(Vector2D *pVec0, Vector2D *pVec1)
{

	return sqrt((pVec0->x- pVec1->x)*(pVec0->x - pVec1->x)+ (pVec0->y - pVec1->y)*(pVec0->y - pVec1->y));
}

// ---------------------------------------------------------------------------

float Vector2DSquareDistance(Vector2D *pVec0, Vector2D *pVec1)
{
	return ((pVec0->x - pVec1->x)*(pVec0->x - pVec1->x) + (pVec0->y - pVec1->y)*(pVec0->y - pVec1->y));
}

// ---------------------------------------------------------------------------

float Vector2DDotProduct(Vector2D *pVec0, Vector2D *pVec1)
{
	return (pVec0->x*pVec1->x+ pVec0->y*pVec1->y);
}

// ---------------------------------------------------------------------------

void Vector2DFromAngleDeg(Vector2D *pResult, float angle)
{
	pResult->x = cos(angle*PIToRad);
	pResult->y = sin(angle*PIToRad);
}

// ---------------------------------------------------------------------------

void Vector2DFromAngleRad(Vector2D *pResult, float angle)
{
	pResult->x = cos(angle);
	pResult->y = sin(angle);
}

// ---------------------------------------------------------------------------
