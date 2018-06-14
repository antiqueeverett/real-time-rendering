#include "rtr/terrain/Terrain.h"

#include <glm/glm.hpp>
#include <stdlib.h>
#include <GL/freeglut.h>

using namespace glm;

Terrain::Terrain(int resolution, int tileNumber)
{
    mResolution = resolution;
    mHeight.resize(resolution*resolution); //mHeight.size = resolution^2 => store all values needed for grid in this vector
	mTileNumber = tileNumber;

	// flat grid
    for (int i = 0 ; i < resolution*resolution ; i++)
		mHeight[i] = 0.0f;

	setVAOPositions();
}

Terrain::~Terrain()
{
}

void Terrain::setVAOPositions()
{
	vec3 n;
	// methods from VertexArrayObject
	begin(GL_TRIANGLES);
	// Create mesh/grid 
    for (int z = 1 ; z < mResolution-1 ; z++) {          
        for (int x = 1 ; x < mResolution-1 ; x++) {

			// calculate normals
			n.x = getHeight(x - 1, z) - getHeight(x + 1, z);
			n.y = 2.0f;
			n.z = getHeight(x, z - 1) - getHeight(x, z + 1);
			n = normalize(n);
			addNormal3f(n.x, n.y, n.z);

 			addVertex3f(static_cast<float>(x), getHeight(x, z), static_cast<float>(z));

			// calculate texture coordinates
			float s = x / static_cast<float>(mResolution-1) * mTileNumber;
			float t = z / static_cast<float>(mResolution - 1) * mTileNumber;
			addTexCoord2f(s,t);
		}
    }

	// 1 quad = 2 triangles, for a connected grid
	for (int z = 0 ; z < mResolution-3 ; z++) {         
        for (int x = 0 ; x < mResolution-3 ; x++) {

			addIndex1ui( calcIndex(x, z) );
			addIndex1ui( calcIndex(x+1, z) );
			addIndex1ui( calcIndex(x+1, z+1) );

			addIndex1ui( calcIndex(x, z) );
			addIndex1ui( calcIndex(x+1, z+1) );
			addIndex1ui( calcIndex(x, z+1) );

		}
    }

	end();
}

void Terrain::setHeight(int x, int z, float height)
{
    if (getHeight(x, z) == 0)
        mHeight[z*mResolution+x] = height;
}

float Terrain::getHeight(int x, int z) const
{
    return mHeight[z*mResolution+x];
}

int Terrain::calcIndex(int x, int z)
{
    return z*(mResolution-2)+x; 
}





