#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <rtr/ren/headers.hpp>
#include <rtr/model/model_loader.hpp>


class Object {

public:

	Object();
	Object(std::string const& file, model::attrib_flag_t import_attribs);
	Object(std::string const& file, glm::fvec3 const& color);
	Object(std::string const& file, glm::fvec3 const& color, glm::fvec3 const& velocity);

	~Object(){std::cout << "deleted - I'll be back" << std::endl;}

	float* get_model_matrix();
	void translate(glm::fvec3 const& v);
	void rotate(glm::fvec3 const& axis, float r);
	void scale(float scale);

	void set_color(glm::fvec3 col) {color_ = col;}
	void set_veloccity(glm::fvec3 vel) {velocity_ = vel;}

	void activate() const;
	void deactivate() const;
	void draw() const;

	void move(float delta_t);


	glm::fvec3 color_;
	glm::fvec3 velocity_;

	glm::fmat4 translation_matrix_ = glm::fmat4{1.0};
	glm::fmat4 rotation_matrix_ = glm::fmat4{1.0};
	glm::fmat4 scale_matrix_ = glm::fmat4{1.0};
	glm::fmat4 model_matrix_ = glm::fmat4{1.0};

private:

	void load_model(model::attrib_flag_t import_attribs);

	std::string file_;
	
	model m_model;
	// vertex array object
	GLuint m_vertex_AO = 0;
	// vertex buffer object
	GLuint m_vertex_BO = 0;
	// index buffer object
	GLuint m_element_BO = 0;
	// primitive type to draw
	GLenum m_draw_mode = GL_NONE;
	// indices number, if EBO exists
	GLsizei m_num_elements = 0;

};

#endif