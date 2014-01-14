#ifndef CG_FRAMEBUFFER_H
#define CG_FRAMEBUFFER_H

//---------------------------------------------------------------------------

class CGFrameBuffer
{
public:
	bool allocate(int width, int height);
	int getWidth();
	int getHeight();
	class CGBuffer4UB
	{
	public:
		CGBuffer4UB();
		~CGBuffer4UB();
		bool reserve(int width, int height);
		void get(int x, int y, float *rgba);
		void set(int x, int y, const float *rgba);
		void clear(const float *rgba);
		unsigned char* getDataPointer();
		int getWidth();
		int getHeight();
	private:
		int width, height;
		unsigned char* pBufferData;
	} colorBuffer;

	class CGBuffer1f
	{
	public:
		CGBuffer1f();
		~CGBuffer1f();
		bool reserve(int width, int height);
		float get(int x, int y);
		void set(int x, int y, const float z);
		void clear();
		float* getDataPointer();
		int getWidth();
		int getHeight();
	private:
		int width, height;
		float* pBufferData;
	} depthBuffer;
};

//---------------------------------------------------------------------------
#endif
