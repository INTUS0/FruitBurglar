#include "Matrix2D.h"
#define PIToRad 3.1415926535897/180.0
/*
将pResult设置为单位矩阵
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
计算pMtx的转置，并将结果放到pResult
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
将pResult设置为平移矩阵，平移位移为x和y
*/
void Matrix2DTranslate(Matrix2D *pResult, float x, float y)
{
	Matrix2DIdentity(pResult);
	pResult->m[0][2] = x;
	pResult->m[1][2] = y;
}

// ---------------------------------------------------------------------------

/*
将pResult设置为缩放矩阵，x和y分别为水平和垂直方向的缩放比例
*/
void Matrix2DScale(Matrix2D *pResult, float x, float y)
{
	Matrix2DIdentity(pResult);
	pResult->m[0][0] = x;
	pResult->m[1][1] = y;
}

// ---------------------------------------------------------------------------

/*
将pResult设置为旋转矩阵，旋转量为Angle，为度数
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
将pResult设置为旋转矩阵，旋转量为Angle，为弧度
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
