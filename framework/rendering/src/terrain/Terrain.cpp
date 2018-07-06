#include "rtr/terrain/Terrain.h"

#include <glm/glm.hpp>
#include <stdlib.h>
#include <GL/freeglut.h>

using namespace glm;

Terrain::Terrain(int resolution, int tileNumber, int subDivide, int stretchFactor)
{
    mResolution = resolution;
    mHeight.resize((resolution*subDivide)*(resolution*subDivide) ); //mHeight.size = resolution^2 => store all values needed for grid in this vector
	mTileNumber = tileNumber;
	mSubdivide = subDivide;
	mFactor = static_cast<float>(1.0f / mSubdivide);
	mStretchFactor = stretchFactor;
	// flat grid
    for (int i = 0 ; i < (resolution*subDivide)*(resolution*subDivide); i++)
		mHeight[i] = 0.0f;
	
	setVAOPositions();
}

Terrain::Terrain(int resolution)
{
	mResolution = resolution;
	mHeight.resize((resolution*mSubdivide)*(resolution*mSubdivide));
	for (int i = 0; i < (resolution*mSubdivide)*(resolution*mSubdivide); i++)
		mHeight[i] = 0.0f;

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
	for (float z = 1.0; z < mResolution - 1; z = z + mFactor) {
		for (float x = 1.0; x < mResolution - 1; x = x + mFactor) {
			// calculate normals
			addVertex3f(static_cast<float>(x+mStretchFactor), 0.0, static_cast<float>(z+mStretchFactor));
			// calculate texture coordinates
			float s = x / static_cast<float>(mResolution - 1) * mTileNumber;
			float t = z / static_cast<float>(mResolution - 1) * mTileNumber;
			addTexCoord2f(s, t);

			if (z < mResolution - 3 && x < mResolution - 3) {

				addIndex1ui(calcIndex(x, z));
				addIndex1ui(calcIndex(x + (1 * mFactor), z));
				addIndex1ui(calcIndex(x + (1 * mFactor), z + (1 * mFactor)));

				addIndex1ui(calcIndex(x, z));
				addIndex1ui(calcIndex(x + (1 * mFactor), z + (1 * mFactor)));
				addIndex1ui(calcIndex(x, z + (1 * mFactor)));
			}
		}
    }
	end();
}

// Will later be used for creating noise on the collision mesh, dummy function for now
float Terrain::noise(int x, int z)
{
	return rand() % 100;
}

// Will set height of a low-res terrain mesh for collision
std::vector<float>  Terrain::makeCollisionMesh()
{
	for (int z = 1; z < mResolution - 1; z++) {
		for (int x = 1; x < mResolution - 1; x++) {
			noise(x, z);
		}
	}
	return mHeight;
}

void Terrain::setHeight(float x, float z, float height)
{
	if (getHeight(x, z) == 0)
		mHeight[(z*mSubdivide) * (mResolution*mSubdivide) + (x*mSubdivide)] = height;
}

float Terrain::getHeight(float x, float z) const
{
	return 0.0f;
}

int Terrain::calcIndex(float x, float z)
{
	return (z*mSubdivide)*((mResolution*mSubdivide) - 2) + (x*mSubdivide);
}





