#include "Matrix2D.h"
#define PIToRad 3.1415926535897/180.0
/*
��pResult����Ϊ��λ����
*/
void Matrix2DIdentity(Matrix2D *pResult)
{
	int i = 0, j = 0;
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			if (i == j)pResult->m[i][j] = 1;
			else pResult->m[i][j] = 0;
		}
	}
}

// ---------------------------------------------------------------------------

/*
����pMtx��ת�ã���������ŵ�pResult
*/
void Matrix2DTranspose(Matrix2D *pResult, Matrix2D *pMtx)
{
	int i = 0, j = 0;
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			pResult->m[i][j] = pMtx->m[j][i];
		}
	}
}

// ---------------------------------------------------------------------------

/*
pResult = pMtx0 * pMtx1
*/
void Matrix2DConcat(Matrix2D *pResult, Matrix2D *pMtx0, Matrix2D *pMtx1)
{
	int i = 0, j = 0;
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			pResult->m[i][j] = pMtx0->m[i][0] * pMtx1->m[0][j] +
				pMtx0->m[i][1] * pMtx1->m[1][j] +
				pMtx0->m[i][2] * pMtx1->m[2][j];
		}
	}
}

// ---------------------------------------------------------------------------

/*
��pResult����Ϊƽ�ƾ���ƽ��λ��Ϊx��y
*/
void Matrix2DTranslate(Matrix2D *pResult, float x, float y)
{
	Matrix2DIdentity(pResult);
	pResult->m[0][2] = x;
	pResult->m[1][2] = y;
}

// ---------------------------------------------------------------------------

/*
��pResult����Ϊ���ž���x��y�ֱ�Ϊˮƽ�ʹ�ֱ��������ű���
*/
void Matrix2DScale(Matrix2D *pResult, float x, float y)
{
	Matrix2DIdentity(pResult);
	pResult->m[0][0] = x;
	pResult->m[1][1] = y;
}

// ---------------------------------------------------------------------------

/*
��pResult����Ϊ��ת������ת��ΪAngle��Ϊ����
*/
void Matrix2DRotDeg(Matrix2D *pResult, float Angle)
{
	Matrix2DIdentity(pResult);
	pResult->m[0][0] = cos(Angle*PIToRad);
	pResult->m[1][0] = sin(Angle*PIToRad);
	pResult->m[0][1] = -sin(Angle*PIToRad);
	pResult->m[1][1] = cos(Angle*PIToRad);
}

// ---------------------------------------------------------------------------

/*
��pResult����Ϊ��ת������ת��ΪAngle��Ϊ����
*/
void Matrix2DRotRad(Matrix2D *pResult, float Angle)
{
	Matrix2DIdentity(pResult);
	pResult->m[0][0] = cos(Angle);
	pResult->m[1][0] = sin(Angle);
	pResult->m[0][1] = -sin(Angle);
	pResult->m[1][1] = cos(Angle);
}

// ---------------------------------------------------------------------------

/*
Result = Mtx * Vec
*/
void Matrix2DMultVec(Vector2D *pResult, Matrix2D *pMtx, Vector2D *pVec)
{
	pResult->x = pMtx->m[0][0] * pVec->x + pMtx->m[0][1] * pVec->y;
	pResult->y = pMtx->m[1][0] * pVec->x + pMtx->m[1][1] * pVec->y;
}

// ---------------------------------------------------------------------------
