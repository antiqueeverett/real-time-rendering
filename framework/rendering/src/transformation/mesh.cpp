#include <rtr/transformation/mesh.h>
#include <rtr/transformation/obj_loader.h>
#include <vector>

//use the constructor to represent a mesh within openGL
Mesh::Mesh(const std::string& fileName)
{
  IndexedModel model = OBJModel(fileName).ToIndexedModel();
  InitMesh(model);
}

Mesh::Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{ 
  IndexedModel model;

  for(unsigned int i = 0; i < numVertices; i++)
  {
    model.positions.push_back(*vertices[i].GetPos());
    model.texCoords.push_back(*vertices[i].GetTexCoord());
    model.normals.push_back(*vertices[i].GetNormal());
  }

   for(unsigned int i = 0; i < numIndices; i++)
   {
    model.indices.push_back(indices[i]);
   }

   InitMesh(model);

}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vertexArrayObject);

}

void Mesh::InitMesh(const IndexedModel& model)
{
  drawCount = model.indices.size();

  glGenVertexArrays(1, &vertexArrayObject);

  //bind vertexArray object 
  glBindVertexArray(vertexArrayObject);

  //once we have the vertex Arraky object, include the vertext buffers (buffers refer to data on the GPU)
  glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

  //move data onto the GPU
  glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[POSITION_VB]);
  glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //move data onto the GPU
  glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]);
  glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  //move data onto the GPU
  glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[NORMAL_VB]);
  glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(model.normals[0]), &model.normals[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //move data onto the GPU
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[INDEX_VB]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW);

  //"glEnableVertexAttribArray" :specify vertice attributes to openGL 

  //any operation that affects a vertex array after this point no longer will
  glBindVertexArray(0);
}

void Mesh::Draw()
{
 	glBindVertexArray(vertexArrayObject);                         //draw array of data
  glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);  //set mode to varient of triangles
 	//glDrawArrays(GL_TRIANGLES, 0, drawCount);

 	glBindVertexArray(0);
}