#include "CGMatrix.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <stdio.h>

//---------------------------------------------------------------------------
// CGMATRIX4X4 : Static (factory) functions.
//
// Hausaufgabe 2 - Aufgabe 2.1  |  Alle Funktionen implementiert
// Hausaufgabe 2 - Aufgabe 2.2  |  Darstellung von debugPrint verbessert
// Übung 07      - Aufgabe 2a   |  Funktion getFrustum implementiert
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getIdentityMatrix()
{
	CGMatrix4x4 m;
	return m;
}

//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getTranslationMatrix(float x, float y, float z)
{
	CGMatrix4x4 m;
	m.m_matrix[3][0] = x; m.m_matrix[3][1] = y; m.m_matrix[3][2] = z;
	return m;
}

//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getRotationMatrixX(float angle)
{
	return CGMatrix4x4::getRotationMatrix(angle, 1.0f, 0.0f, 0.0f);
}

//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getRotationMatrixY(float angle)
{
	return CGMatrix4x4::getRotationMatrix(angle, 0.0f, 1.0f, 0.0f);
}

//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getRotationMatrixZ(float angle)
{
	return CGMatrix4x4::getRotationMatrix(angle, 0.0f, 0.0f, 1.0f);
}

//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getScaleMatrix(float x, float y, float z)
{
	CGMatrix4x4 m;
	m.m_matrix[0][0] = x; m.m_matrix[1][1] = y; m.m_matrix[2][2] = z;
	return m;
}

//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getRotationMatrix(float angle, float x, float y, float z)
{
	const float c = (float)cosf(angle * (float)M_PI / 180.0f);
	const float one_minus_c = 1.0f - c;
	const float s = (float)sinf(angle * (float)M_PI / 180.0f);
	const float v_len = (float)sqrtf(x*x + y*y + z*z);
	float x_one_minus_c;
	float y_one_minus_c;
	float z_one_minus_c;
	CGMatrix4x4 rot;
	if (v_len == 0.0f)
		return rot;
	if (v_len != 1.0f) {
		x /= v_len; y /= v_len; z /= v_len;
	}
	x_one_minus_c = x * one_minus_c;
	y_one_minus_c = y * one_minus_c;
	z_one_minus_c = z * one_minus_c;
	rot.m_matrix[0][0] = x * x_one_minus_c + c;
	rot.m_matrix[0][1] = x * y_one_minus_c + z * s;
	rot.m_matrix[0][2] = x * z_one_minus_c - y * s;
	rot.m_matrix[1][0] = y * x_one_minus_c - z * s;
	rot.m_matrix[1][1] = y * y_one_minus_c + c;
	rot.m_matrix[1][2] = y * z_one_minus_c + x * s;
	rot.m_matrix[2][0] = z * x_one_minus_c + y * s;
	rot.m_matrix[2][1] = z * y_one_minus_c - x * s;
	rot.m_matrix[2][2] = z * z_one_minus_c + c;
	return rot;
}

//---------------------------------------------------------------------------
// Gibt Projektionsmatrix zurück, für die Transformation von Pyramidenstumpf
// in einen Würfel
//
// Übung 07 - Aufgabe 2a  |  Funktion implementiert
// Übung 08 - Aufgabe 1b  |  Funktion durch Musterlösung ersetzt
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getFrustum(float left, float right, float bottom, float top, float zNear, float zFar)
{
	CGMatrix4x4 f;
	f.m_matrix[0][0] = (2.0f * zNear) / (right - left);
	f.m_matrix[1][1] = (2.0f * zNear) / (top - bottom);
	f.m_matrix[2][0] = (right + left) / (right - left);
	f.m_matrix[2][1] = (top + bottom) / (top - bottom);
	f.m_matrix[2][2] = (-zNear - zFar) / (zFar - zNear);
	f.m_matrix[2][3] = -1.0f;
	f.m_matrix[3][2] = (2.0f * zFar * zNear) / (zNear - zFar);
	f.m_matrix[3][3] = 0.0f;
	return f;
}

//---------------------------------------------------------------------------
// CGMATRIX4X4 : Non-static operants.
//---------------------------------------------------------------------------
CGMatrix4x4::CGMatrix4x4()
{
	identity();
}

//---------------------------------------------------------------------------
void CGMatrix4x4::getFloatsToColMajor(float *floats) const
{
	memcpy(floats, m_matrix, sizeof(float)* 16);
}

//---------------------------------------------------------------------------
void CGMatrix4x4::setFloatsFromColMajor(const float *floats)
{
	memcpy(m_matrix, floats, sizeof(float)* 16);
}

//---------------------------------------------------------------------------
void CGMatrix4x4::getFloatsToRowMajor(float *floats) const
{
	floats[0] = m_matrix[0][0];
	floats[1] = m_matrix[1][0];
	floats[2] = m_matrix[2][0];
	floats[3] = m_matrix[3][0];
	floats[4] = m_matrix[0][1];
	floats[5] = m_matrix[1][1];
	floats[6] = m_matrix[2][1];
	floats[7] = m_matrix[3][1];
	floats[8] = m_matrix[0][2];
	floats[9] = m_matrix[1][2];
	floats[10] = m_matrix[2][2];
	floats[11] = m_matrix[3][2];
	floats[12] = m_matrix[0][3];
	floats[13] = m_matrix[1][3];
	floats[14] = m_matrix[2][3];
	floats[15] = m_matrix[3][3];
}

//---------------------------------------------------------------------------
void CGMatrix4x4::setFloatsFromRowMajor(const float *floats)
{
	m_matrix[0][0] = floats[0];
	m_matrix[0][1] = floats[4];
	m_matrix[0][2] = floats[8];
	m_matrix[0][3] = floats[12];
	m_matrix[1][0] = floats[1];
	m_matrix[1][1] = floats[5];
	m_matrix[1][2] = floats[9];
	m_matrix[1][3] = floats[13];
	m_matrix[2][0] = floats[2];
	m_matrix[2][1] = floats[6];
	m_matrix[2][2] = floats[10];
	m_matrix[2][3] = floats[14];
	m_matrix[3][0] = floats[3];
	m_matrix[3][1] = floats[7];
	m_matrix[3][2] = floats[11];
	m_matrix[3][3] = floats[15];
}

//---------------------------------------------------------------------------
void CGMatrix4x4::identity()
{
	m_matrix[0][0] = 1.0f; m_matrix[1][0] = 0.0f; m_matrix[2][0] = 0.0f; m_matrix[3][0] = 0.0f;
	m_matrix[0][1] = 0.0f; m_matrix[1][1] = 1.0f; m_matrix[2][1] = 0.0f; m_matrix[3][1] = 0.0f;
	m_matrix[0][2] = 0.0f; m_matrix[1][2] = 0.0f; m_matrix[2][2] = 1.0f; m_matrix[3][2] = 0.0f;
	m_matrix[0][3] = 0.0f; m_matrix[1][3] = 0.0f; m_matrix[2][3] = 0.0f; m_matrix[3][3] = 1.0f;

}

//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::operator*(const CGMatrix4x4& b) const
{
	CGMatrix4x4 r;
	for (int i = 0; i<4; i++)
	for (int j = 0; j<4; j++) {
		r.m_matrix[i][j] = 0.0f;
		for (int k = 0; k<4; k++)
			r.m_matrix[i][j] += m_matrix[k][j] * b.m_matrix[i][k];
	}
	return r;
}

//---------------------------------------------------------------------------
void CGMatrix4x4::debugPrint(const char *prefix) const
{
	for (int i = 0; i < 4; i++){
		if (prefix != NULL) {
			printf(prefix);
			printf(": ");
		}
		for (int j = 0; j < 4; j++) {
			printf("%f ", m_matrix[j][i]);
		}
		printf("\n");
	}
	printf("\n");

}

//---------------------------------------------------------------------------
CGVec4 CGMatrix4x4::operator*(const CGVec4& b) const
{
	CGVec4 c;
	for (int i = 0; i<4; i++) {
		c[i] = 0.0f;
		for (int j = 0; j<4; j++)
			c[i] += m_matrix[j][i] * b[j];
	}
	return c;
}

//---------------------------------------------------------------------------
void CGMatrix4x4::transpose()
{
	int i, j;
	for (i = 0; i<4; i++) {
		for (j = 0; j<i; j++) {
			float tmp;
			tmp = m_matrix[i][j];
			m_matrix[i][j] = m_matrix[j][i];
			m_matrix[j][i] = tmp;
		}
	}
}

//---------------------------------------------------------------------------
void CGMatrix4x4::invert()
{
#define m(a,b) m_matrix[b][a]
#define det3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3) (a1 * (b2 * c3 - b3 * c2) + b1 * (c2 * a3 - a2 * c3) + c1 * (a2 * b3 - a3 * b2))
	const float cof00 = det3x3(m(1, 1), m(1, 2), m(1, 3), m(2, 1), m(2, 2), m(2, 3), m(3, 1), m(3, 2), m(3, 3));
	const float cof01 = -det3x3(m(1, 2), m(1, 3), m(1, 0), m(2, 2), m(2, 3), m(2, 0), m(3, 2), m(3, 3), m(3, 0));
	const float cof02 = det3x3(m(1, 3), m(1, 0), m(1, 1), m(2, 3), m(2, 0), m(2, 1), m(3, 3), m(3, 0), m(3, 1));
	const float cof03 = -det3x3(m(1, 0), m(1, 1), m(1, 2), m(2, 0), m(2, 1), m(2, 2), m(3, 0), m(3, 1), m(3, 2));
	const float inv_det = 1.0f / (m(0, 0) * cof00 + m(0, 1) * cof01 + m(0, 2) * cof02 + m(0, 3) * cof03);
	const float cof10 = -det3x3(m(2, 1), m(2, 2), m(2, 3), m(3, 1), m(3, 2), m(3, 3), m(0, 1), m(0, 2), m(0, 3));
	const float cof11 = det3x3(m(2, 2), m(2, 3), m(2, 0), m(3, 2), m(3, 3), m(3, 0), m(0, 2), m(0, 3), m(0, 0));
	const float cof12 = -det3x3(m(2, 3), m(2, 0), m(2, 1), m(3, 3), m(3, 0), m(3, 1), m(0, 3), m(0, 0), m(0, 1));
	const float cof13 = det3x3(m(2, 0), m(2, 1), m(2, 2), m(3, 0), m(3, 1), m(3, 2), m(0, 0), m(0, 1), m(0, 2));
	const float cof20 = det3x3(m(3, 1), m(3, 2), m(3, 3), m(0, 1), m(0, 2), m(0, 3), m(1, 1), m(1, 2), m(1, 3));
	const float cof21 = -det3x3(m(3, 2), m(3, 3), m(3, 0), m(0, 2), m(0, 3), m(0, 0), m(1, 2), m(1, 3), m(1, 0));
	const float cof22 = det3x3(m(3, 3), m(3, 0), m(3, 1), m(0, 3), m(0, 0), m(0, 1), m(1, 3), m(1, 0), m(1, 1));
	const float cof23 = -det3x3(m(3, 0), m(3, 1), m(3, 2), m(0, 0), m(0, 1), m(0, 2), m(1, 0), m(1, 1), m(1, 2));
	const float cof30 = -det3x3(m(0, 1), m(0, 2), m(0, 3), m(1, 1), m(1, 2), m(1, 3), m(2, 1), m(2, 2), m(2, 3));
	const float cof31 = det3x3(m(0, 2), m(0, 3), m(0, 0), m(1, 2), m(1, 3), m(1, 0), m(2, 2), m(2, 3), m(2, 0));
	const float cof32 = -det3x3(m(0, 3), m(0, 0), m(0, 1), m(1, 3), m(1, 0), m(1, 1), m(2, 3), m(2, 0), m(2, 1));
	const float cof33 = det3x3(m(0, 0), m(0, 1), m(0, 2), m(1, 0), m(1, 1), m(1, 2), m(2, 0), m(2, 1), m(2, 2));
#undef det3x3
#undef m
	m_matrix[0][0] = cof00 * inv_det; m_matrix[1][0] = cof10 * inv_det; m_matrix[2][0] = cof20 * inv_det; m_matrix[3][0] = cof30 * inv_det;
	m_matrix[0][1] = cof01 * inv_det; m_matrix[1][1] = cof11 * inv_det; m_matrix[2][1] = cof21 * inv_det; m_matrix[3][1] = cof31 * inv_det;
	m_matrix[0][2] = cof02 * inv_det; m_matrix[1][2] = cof12 * inv_det; m_matrix[2][2] = cof22 * inv_det; m_matrix[3][2] = cof32 * inv_det;
	m_matrix[0][3] = cof03 * inv_det; m_matrix[1][3] = cof13 * inv_det; m_matrix[2][3] = cof23 * inv_det; m_matrix[3][3] = cof33 * inv_det;

}

//---------------------------------------------------------------------------
void CGMatrix4x4::frustum(float left, float right, float bottom, float top, float zNear, float zFar)
{
	*this=*this * CGMatrix4x4::getFrustum(left,right,bottom,top,zNear,zFar);
}
