#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "obj_loader.h"


/**
 *
 * Implementing expected openGL Data format:
 *
 * Vertex class:
 * representation of data in the vertex format that openGL expects us to conform to
 *     - Data is represented as a 3 dimension vector (or vector point)
 *         x, y, x components that vary between -1 and 1 
 * 
 */

class Vertex
{
	public:
		Vertex(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& normal = glm::vec3(0,0,0))
		{	
			//vertex mesh data
			this ->pos = pos;
			this ->texCoord = texCoord;
			this ->normal = normal;
		}

		inline glm::vec3* GetPos(){return &pos;}
		inline glm::vec2* GetTexCoord(){return &texCoord;}
		inline glm::vec3* GetNormal(){return &normal;}

	private: 
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 normal;

	protected:

};

class Mesh
{
	public:
		Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
		Mesh(const std::string& fileName);

		//send data through the pipe-line to produce the final image
		void Draw();

		virtual ~Mesh();

	private:
		Mesh(const Mesh& other);
		void operator=(const Mesh& other);

		void InitMesh(const IndexedModel& model);
 	
 		//referring to every buffer array 
		enum
		{
			POSITION_VB, 	//vertex position buffer
			TEXCOORD_VB,	//vertex texture co-ordinate buffer
			NORMAL_VB,		//vertex normal buffer

			INDEX_VB,		//vertex index buffer

			NUM_BUFFERS 	//recording how many buffers we have in this enumeration 
		};

		//"GLuint" how openGL refers to all data within the GPU
		GLuint vertexArrayObject; 				//refer to mesh data from openGL 3.0 onwards
		GLuint vertexArrayBuffers[NUM_BUFFERS]; // creating a mesh data buffer array 

		//state magnitude of vertex array to openGL
		unsigned int drawCount;

	protected:
	
};

#endif // MESH_H

/**
 *
 * Mesh creation - the act of taking 3D data and sending it to the GPU
 *
 * 
 */