#ifndef _TERRAIN_H
#define _TERRAIN_H

#include "rtr/terrain/VertexArrayObject.h"
#include <vector>

class Terrain : public VertexArrayObject {

public:
    Terrain(int resolution, int tileNumber);
    ~Terrain();
    
    void setHeight(int x, int z, float height);
    float getHeight(int x, int z) const;
	int calcIndex(int x, int z);

	void setVAOPositions();

private:     
    int mResolution;    
	int mTileNumber;
	std::vector<float> mHeight;
};


#endif

