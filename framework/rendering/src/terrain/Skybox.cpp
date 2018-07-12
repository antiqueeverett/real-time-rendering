#include "rtr/terrain/Skybox.h"



Skybox::Skybox(float size, int stretch)
{
	mSize = size + static_cast<float>(stretch);
	drawVAOPostions();
}


Skybox::~Skybox()
{
}

void Skybox::drawVAOPostions()
{
	begin(GL_TRIANGLES);
	addVertex3f(-(mSize/2.0f), (mSize/2.0f), -(mSize/2.0f));
	addVertex3f(-(mSize/2.0f), -(mSize/2.0f), -(mSize/2.0f));
	addVertex3f((mSize/2.0f), -(mSize/2.0f), -(mSize/2.0f));
	addVertex3f((mSize/2.0f), -(mSize/2.0f), -(mSize/2.0f));
	addVertex3f((mSize/2.0f), (mSize/2.0f), -(mSize/2.0f));
	addVertex3f(-(mSize/2.0f), (mSize/2.0f), -(mSize/2.0f));

	addVertex3f(-(mSize/2.0f), -(mSize/2.0f), (mSize/2.0f));
	addVertex3f(-(mSize/2.0f), -(mSize/2.0f), -(mSize/2.0f));
	addVertex3f(-(mSize/2.0f), (mSize/2.0f), -(mSize/2.0f));
	addVertex3f(-(mSize/2.0f), (mSize/2.0f), -(mSize/2.0f));
	addVertex3f(-(mSize/2.0f), (mSize/2.0f), (mSize/2.0f));
	addVertex3f(-(mSize/2.0f), -(mSize/2.0f), (mSize/2.0f));

	addVertex3f((mSize/2.0f), -(mSize/2.0f), -(mSize/2.0f));
	addVertex3f((mSize/2.0f), -(mSize/2.0f), (mSize/2.0f));
	addVertex3f((mSize/2.0f), (mSize/2.0f), (mSize/2.0f));
	addVertex3f((mSize/2.0f), (mSize/2.0f), (mSize/2.0f));
	addVertex3f((mSize/2.0f), (mSize/2.0f), -(mSize/2.0f));
	addVertex3f((mSize/2.0f), -(mSize/2.0f), -(mSize/2.0f));

	addVertex3f(-(mSize/2.0f), -(mSize/2.0f), (mSize/2.0f));
	addVertex3f(-(mSize/2.0f), (mSize/2.0f), (mSize/2.0f));
	addVertex3f((mSize/2.0f), (mSize/2.0f), (mSize/2.0f));
	addVertex3f((mSize/2.0f), (mSize/2.0f), (mSize/2.0f));
	addVertex3f((mSize/2.0f), -(mSize/2.0f), (mSize/2.0f));
	addVertex3f(-(mSize/2.0f), -(mSize/2.0f), (mSize/2.0f));

	addVertex3f(-(mSize/2.0f), (mSize/2.0f), -(mSize/2.0f));
	addVertex3f((mSize/2.0f), (mSize/2.0f), -(mSize/2.0f));
	addVertex3f((mSize/2.0f), (mSize/2.0f), (mSize/2.0f));
	addVertex3f((mSize/2.0f), (mSize/2.0f), (mSize/2.0f));
	addVertex3f(-(mSize/2.0f), (mSize/2.0f), (mSize/2.0f));
	addVertex3f(-(mSize/2.0f), (mSize/2.0f), -(mSize/2.0f));

	addVertex3f(-(mSize/2.0f), -(mSize/2.0f), -(mSize/2.0f));
	addVertex3f(-(mSize/2.0f), -(mSize/2.0f), (mSize/2.0f));
	addVertex3f((mSize/2.0f), -(mSize/2.0f), -(mSize/2.0f));
	addVertex3f((mSize/2.0f), -(mSize/2.0f), -(mSize/2.0f));
	addVertex3f(-(mSize/2.0f), -(mSize/2.0f), (mSize/2.0f));
	addVertex3f((mSize/2.0f), -(mSize/2.0f), (mSize/2.0f));

	end();
}