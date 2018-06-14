#include "rtr/terrain/Skybox.h"



Skybox::Skybox(float size):
	mSize(size)
{
	drawVAOPostions();
}


Skybox::~Skybox()
{
}

void Skybox::drawVAOPostions()
{
	begin(GL_TRIANGLES);

	addVertex3f(-mSize, mSize, -mSize);
	addVertex3f(-mSize, -mSize, -mSize);
	addVertex3f(mSize, -mSize, -mSize);
	addVertex3f(mSize, -mSize, -mSize);
	addVertex3f(mSize, mSize, -mSize);
	addVertex3f(-mSize, mSize, -mSize);

	addVertex3f(-mSize, -mSize, mSize);
	addVertex3f(-mSize, -mSize, -mSize);
	addVertex3f(-mSize, mSize, -mSize);
	addVertex3f(-mSize, mSize, -mSize);
	addVertex3f(-mSize, mSize, mSize);
	addVertex3f(-mSize, -mSize, mSize);

	addVertex3f(mSize, -mSize, -mSize);
	addVertex3f(mSize, -mSize, mSize);
	addVertex3f(mSize, mSize, mSize);
	addVertex3f(mSize, mSize, mSize);
	addVertex3f(mSize, mSize, -mSize);
	addVertex3f(mSize, -mSize, -mSize);

	addVertex3f(-mSize, -mSize, mSize);
	addVertex3f(-mSize, mSize, mSize);
	addVertex3f(mSize, mSize, mSize);
	addVertex3f(mSize, mSize, mSize);
	addVertex3f(mSize, -mSize, mSize);
	addVertex3f(-mSize, -mSize, mSize);

	addVertex3f(-mSize, mSize, -mSize);
	addVertex3f(mSize, mSize, -mSize);
	addVertex3f(mSize, mSize, mSize);
	addVertex3f(mSize, mSize, mSize);
	addVertex3f(-mSize, mSize, mSize);
	addVertex3f(-mSize, mSize, -mSize);

	addVertex3f(-mSize, -mSize, -mSize);
	addVertex3f(-mSize, -mSize, mSize);
	addVertex3f(mSize, -mSize, -mSize);
	addVertex3f(mSize, -mSize, -mSize);
	addVertex3f(-mSize, -mSize, mSize);
	addVertex3f(mSize, -mSize, mSize);

	end();
}