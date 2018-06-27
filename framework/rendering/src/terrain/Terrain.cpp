#include "rtr/terrain/Terrain.h"

#include <glm/glm.hpp>
#include <stdlib.h>
#include <GL/freeglut.h>

using namespace glm;

Terrain::Terrain(int resolution, int tileNumber, int subDivide)
{
    mResolution = resolution;
    mHeight.resize((resolution*subDivide*resolution*subDivide) ); //mHeight.size = resolution^2 => store all values needed for grid in this vector
	mTileNumber = tileNumber;
	mSubdivide = subDivide;
	mFactor = static_cast<float>(1.0f / mSubdivide);

	// flat grid
    for (int i = 0 ; i < resolution*resolution ; i++)
		mHeight[i] = 0.0f;

	setVAOPositions();
}

Terrain::Terrain(int resolution)
{
	mResolution = resolution;
	mHeight.resize(resolution*resolution);
	for (int i = 0; i < resolution*resolution; i++)
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
	/*
	* TODO: wert für 0.5/0.35 wenn subdivide 2/3, für die +1 Sachen
	*/
	for (float z = 1.0; z < mResolution - 1; z = z + mFactor) {
		for (float x = 1.0; x < mResolution - 1; x = x + mFactor) {
			// calculate normals
			n.x = getHeight(x - (1 * mFactor), z) - getHeight(x + (1 * mFactor), z);
			n.y = 2.0f;
			n.z = getHeight(x, z - (1 * mFactor)) - getHeight(x, z + (1 * mFactor));
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
	for (float z = 0.0 ; z < mResolution-3 ; z = z + mFactor) {
        for (float x = 0.0 ; x < mResolution-3 ; x = x + mFactor) {

			addIndex1ui( calcIndex(x, z) );
			addIndex1ui( calcIndex(x+ (1 * mFactor), z) );
			addIndex1ui( calcIndex(x+ (1 * mFactor), z+ (1 * mFactor)) );

			addIndex1ui( calcIndex(x, z) );
			addIndex1ui( calcIndex(x+(1*mFactor), z+ (1 * mFactor)) );
			addIndex1ui( calcIndex(x, z+ (1 * mFactor)) );

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
    return mHeight[(z*mSubdivide) * (mResolution*mSubdivide) + (x*mSubdivide)];
}

int Terrain::calcIndex(float x, float z)
{
    return (z*mSubdivide)*((mResolution*mSubdivide) -2) + (x*mSubdivide);
}





