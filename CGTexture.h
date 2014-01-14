#ifndef CG_TEXTURE_H
#define CG_TEXTURE_H

#include "CG.h"

//---------------------------------------------------------------------------
class CGTexture2D
{
public:
	/// Available texture lookup filters.
	enum filterModes {CG_NEAREST, CG_LINEAR};
	/// Available texture wrap modes (both directions).
	enum wrapModes {CG_CLAMP, CG_REPEAT};
	/// Constructor
	CGTexture2D();
	/// Destructor
	~CGTexture2D();
	/// Initialize the texture here.
	void allocate(int width, int height, unsigned char* data4ub);
	/// Choose filter to be used in magnification and minification.
	void setFilterMode(int mode);
	/// Choose wrap mode to be used in all dimensions.
	void setWrapMode(int mode);
	/// Get Texel data as float vector
	CGVec4 fetchTexel(int x, int y) const;
	/// Sample with filter.
	CGVec4 sample(const CGVec4 &texcoord) const;
private:
	int width, height;
	int filterMode, wrapMode;
	unsigned char* data; 
};

#endif /* !CGTEXTURE_H */
