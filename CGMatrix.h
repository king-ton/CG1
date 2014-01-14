#ifndef CG_MATRIX_H
#define CG_MATRIX_H
#include "CG.h"
#include <stdlib.h>

class CGMatrix4x4
{
public:
	// --- static members for constructing single transformation ---

	/// returns an instance of the identity matrix.
	static CGMatrix4x4 getIdentityMatrix();
	/// returns an instance of a translation matrix.
	static CGMatrix4x4 getTranslationMatrix(float x, float y, float z);
	/// returns an instance of a rotation matrix around the X axis
	/// rotation has to be in the _mathematically_ correct sense
	/// angle is in degrees
	static CGMatrix4x4 getRotationMatrixX(float angle);
	/// returns an instance of a rotation matrix around the Y axis
	/// rotation has to be in the _mathematically_ correct sense
	/// angle is in degrees
	static CGMatrix4x4 getRotationMatrixY(float angle);
	/// returns an instance of a rotation matrix around the Z axis
	/// rotation has to be in the _mathematically_ correct sense
	/// angle is in degrees
	static CGMatrix4x4 getRotationMatrixZ(float angle);
	/// returns an instance of a scale matrix
	static CGMatrix4x4 getScaleMatrix(float x, float y, float z);
	/// returns an instance of a rotation matrix around an arbitrary axis
	/// angle is in degrees
	/// ZUSATAUFGABE in PVL2
	static CGMatrix4x4 getRotationMatrix(float angle, float x, float y, float z);
	///  returns a perspective projection matrix for a specified frustum
	///  NICHT TEIL DER PVL2! diese Funktion wird im Verlauf der normalen Uebung implementiert
	static CGMatrix4x4 getFrustum(float left, float right, float bottom, float top, float zNear, float zFar);

	// --- non-static members for modifying the current matrix (this) ---

	// Hint 1: you can use *this = ... to assign the whole matrix.
	// Hint 2: use the static members and the multiplication operator whenever possible.

	/// constructor initializes the matrix as identity matrix
	CGMatrix4x4();
	/// copy the contents of the matrix to the array "floats"
	/// floats must be at least 16 elements big
	/// floats will have column-major format
	void getFloatsToColMajor(float *floats) const;
	/// copy the contents of the matrix to the array "floats"
	/// floats must be at least 16 elements big
	/// floats will have row-major format
	void getFloatsToRowMajor(float *floats) const;
	/// set the matrix to the contents of the the array "floats"
	/// floats must be at least 16 elements big and have column-major format
	void setFloatsFromColMajor(const float *floats);
	/// set the matrix to the contents of the the array "floats"
	/// floats must be at least 16 elements big and have row-major format
	void setFloatsFromRowMajor(const float *floats);
	/// set the matrix to Identity
	void identity();
	/// transpose the matrix
	void transpose();
	/// invert the matrix
	void invert();
	/// print the matrix to the console (stdout/cout)
	/// prints the matrix as 4 lines with 4 columns each,
	/// separated by a whitespace and aligned column-wise
	/// each line is optionally prefixed with a string
	void debugPrint(const char *prefix=NULL) const;
	/// multiply two matrices, return result as new matrix
	CGMatrix4x4 operator* (const CGMatrix4x4& m) const;
	/// multiply matrix with vector, return result as new vector
	CGVec4 operator* (const CGVec4& v) const;
	///  apply perspective projection matrix to the matrix
	///  NICHT TEIL DER PVL2!
	void frustum(float left, float right, float bottom, float top, float zNear, float zFar);
private:
	/// the real matrix data
	float m_matrix[4][4];
};
#endif /* CG_MATRIX_H */
