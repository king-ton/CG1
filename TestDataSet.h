#ifndef TESTDATASET_H
#define TESTDATASET_H

//---------------------------------------------------------------------------
// TestDataSet.h
//---------------------------------------------------------------------------
// This class represents simple point data (position+color).
// * Iterate over all items of TestDataSet t using:
//   for(int i=0; i<t.size(); i++) {
//     TestDataSet::TestDataItem item = t.get(i);
//     // item.x is in [0,maxX[
//     // item.r is in [0,1]
//   }
// * Query the maximum dimensions using maxX,maxY,maxZ function.
// * Load new data using loadFromImage() function.
//---------------------------------------------------------------------------

#include <vector>
#include "CGImageFile.h"

//---------------------------------------------------------------------------

class TestDataSet {

public:
  // Constructor, initalize *something*.
  TestDataSet() {
    m_maxX=m_maxY=10; m_maxZ=2;
    for(int ix=m_maxX;ix--;)
      for(int iy=m_maxY;iy--;) {
        TestDataItem item;
        item.x=ix; item.y=iy; item.z=1+cos(float(ix+iy));
        item.r=item.x/m_maxX;
        item.g=item.y/m_maxY;
        item.b=item.z/m_maxZ;
        m_data.push_back(item);
      }
  }
  // Create data from Image file.
  bool loadFromImage(const char* path) {
    int W,H;
    unsigned char* data4ub = cgImageLoad(path,&W,&H);
    if(!data4ub) return false;
    m_data.clear();
    m_maxX = W; m_maxY = H; m_maxZ = 10.0f;
    for(int ix=m_maxX;ix--;)
      for(int iy=m_maxY;iy--;) {
        TestDataItem item;
        unsigned char* rgba = data4ub+(W*iy+ix)*4;
        unsigned char avg = (rgba[0]+rgba[1]+rgba[2])/3;
        if(avg==0) continue; // skip black pixels
        item.x=ix; item.y=H-iy-1; item.z=10.0f*float(avg)/255.0f;
        item.r=float(rgba[0])/255.0f;
        item.g=float(rgba[1])/255.0f;
        item.b=float(rgba[2])/255.0f;
        m_data.push_back(item);
      }
    free(data4ub);
    return true;
  }
  // Call this to 'process' data,
  void process() {
    for(int i=(int)m_data.size(); i--; ) {
      m_data.at(i).x+=0.1f*(float(rand())/RAND_MAX-0.5);
      m_data.at(i).y+=0.1f*(float(rand())/RAND_MAX-0.5);
      m_data.at(i).r+=m_data.at(i).r<=0.9?0.1:-0.9;
      m_data.at(i).g+=m_data.at(i).g<=0.9?0.1:-0.9;
      m_data.at(i).b+=m_data.at(i).b<=0.9?0.1:-0.9;
    }
  }
  // Single data item structure.
  class TestDataItem{ public: float x,y,z, r,g,b; };
  // Query number of data items.
  int size()                { return (int) m_data.size(); }
  // Get specific data item.
  const TestDataItem& get(int i)  { return m_data.at(i); }
  // Get size X (elements always 0<=x<X)
  float maxX() { return m_maxX; }
  // Get size Y (elements always 0<=y<Y)
  float maxY() { return m_maxY; }
  // Get size Z (elements always 0<=z<Z)
  float maxZ() { return m_maxZ; }
private:
  // Internal member for dataset.
  std::vector<TestDataItem> m_data;
  // Internal member for dataset size.
  float m_maxX,m_maxY,m_maxZ;
};

//---------------------------------------------------------------------------
#endif