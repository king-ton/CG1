#ifndef  CG_MATH_H
#define  CG_MATH_H
#include  "CG.h"

#include <math.h>
#include <cmath>
#include <stdio.h>
//#include <assert.h>

#define ISZERO(x) (fabs(x)<1.0e-5)

class  CGMath
{
public:
	static CGVec4 add(const CGVec4& a, const CGVec4 &b)
	{
		//assert(!(!ISZERO(a[W]) && !ISZERO(b[W]))); // two points cannot be added    
		CGVec4 c;
		c[X]=a[X]+b[X]; c[Y]=a[Y]+b[Y]; c[Z]=a[Z]+b[Z]; c[W]=a[W]+b[W];
		return c;
	}
	static CGVec4 sub(const CGVec4& a, const CGVec4 &b)
	{
		CGVec4 c;
		c[X]=a[X]-b[X]; c[Y]=a[Y]-b[Y]; c[Z]=a[Z]-b[Z]; c[W]=a[W]-b[W];
		return c;
	}
	static CGVec4 scale(const CGVec4& a, float s)
	{
		CGVec4 c;
		//assert(ISZERO(a[W])); // points cannot be scaled 
		c[X]=s*a[X]; c[Y]=s*a[Y]; c[Z]=s*a[Z]; c[W]=s*a[W];
		return c;
	}
	static float dot(const CGVec4& a, const CGVec4& b)
	{
		//assert(!(!ISZERO(a[W]) && !ISZERO(b[W]))); // scalar product only with vector(s)
		return a[X]*b[X]+a[Y]*b[Y]+a[Z]*b[Z]+a[W]*b[W];
	}
	static CGVec4 cross(const CGVec4& a, const CGVec4& b)
	{
		//assert(ISZERO(a[W]) && ISZERO(b[W])); // no cross product for points
		CGVec4 c;
		c[X]=a[Y]*b[Z]-a[Z]*b[Y];
		c[Y]=a[Z]*b[X]-a[X]*b[Z];
		c[Z]=a[X]*b[Y]-a[Y]*b[X];
		c[W]=0.0f;
		return c;
	}
	static CGVec4 mul(const CGVec4& a, const CGVec4& b)
	{
		CGVec4 c;    
		c[X]=a[X]*b[X]; c[Y]=a[Y]*b[Y]; c[Z]=a[Z]*b[Z]; c[W]=a[W]*b[W];
		return c;
	}
	static float length(const CGVec4& a)
	{
		return sqrt(dot(a,a));
	}
	static CGVec4 normalize(const CGVec4& a)
	{
		return scale(a,1.0f/length(a));    
	}
	static CGVec4 divw(const CGVec4& a)
	{
		CGVec4 c;
		//assert(!ISZERO(a[W]));
		float s=1.0f/a[W];
		c[X]=s*a[X]; c[Y]=s*a[Y]; c[Z]=s*a[Z]; c[W]=1.0f;
		return c;
	}
	static CGVec4 max(const CGVec4& a, float m)
	{
		CGVec4 c;    
		for (int i=0; i<4; i++)
			c[i]=a[i]>m?a[i]:m;
		return c;
	}
	static CGVec4 min(const CGVec4& a, float n)
	{
		CGVec4 c;    
		for (int i=0; i<4;i++)
			c[i]=a[i]>n?n:a[i];
		return c;
	}
	static CGVec4 max(const CGVec4& a, const CGVec4& b)
	{
		CGVec4 c;    
		for (int i=0; i<4; i++)
			c[i]=a[i]>b[i]?a[i]:b[i];
		return c;
	}
	static CGVec4 min(const CGVec4& a, const CGVec4& b)
	{
		CGVec4 c;    
		for (int i=0; i<4; i++)
			c[i]=a[i]>b[i]?b[i]:a[i];
		return c;
	}
	static CGVec4 clamp(const CGVec4& a, float m, float n)
	{
		return max(min(a,n),m);    
	}
	static CGVec4 clamp(const CGVec4& a, const CGVec4& m, const CGVec4& n)
	{
		return max(min(a,n),m);    
	}
	static CGVec4 abs(const CGVec4& a)
	{
		CGVec4 c;    
		for (int i=0; i<4; i++)
			c[i]=fabs(a[i]);
		return c;
	}
	static CGVec4 ceil(const CGVec4& a)
	{
		CGVec4 c;    
		for (int i=0; i<4; i++)
			c[i]=std::ceil(a[i]);
		return c;
	}
	static CGVec4 floor(const CGVec4& a)
	{
		CGVec4 c;    
		for (int i=0; i<4; i++)
			c[i]=std::floor(a[i]);
		return c;
	}
	static CGVec4 fract(const CGVec4& a)
	{
		CGVec4 c;    
		for (int i=0; i<4; i++)
			c[i]=a[i]-std::floor(a[i]);
		return c;
	}
	static float distance(const CGVec4& a, const CGVec4& b)
	{
		//assert(ISZERO(a[W]-b[W])); // a[W] must equal b[W]     
		return length(sub(a,b));
	}
	static void debugPrint(const CGVec4& a, const char *prefix=NULL)
	{
		printf("%s: %f %f %f %f\n",prefix?prefix:"",a[X],a[Y],a[Z],a[W]);
	}
};
#endif  /*  CG_MATH_H  */
