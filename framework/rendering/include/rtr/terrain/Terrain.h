#ifndef _TERRAIN_H
#define _TERRAIN_H

#include "VertexArrayObject.h"
#include <vector>

class Terrain : public VertexArrayObject {

public:
    Terrain(int resolution, int tileNumber, int subDivide);
	Terrain(int resolution);
    ~Terrain();
    
    void setHeight(float x, float z, float height);
    float getHeight(float x, float z) const;
	int calcIndex(float x, float z);

	void setVAOPositions();
	std::vector<float> makeCollisionMesh();

private:     

	float noise(int x, int z);
    int mResolution;    
	int mTileNumber;
	int mSubdivide;
	float mFactor;
	std::vector<float> mHeight;
};


#endif
