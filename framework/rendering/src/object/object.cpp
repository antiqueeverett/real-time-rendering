#include <rtr/object/object.hpp>



Object::Object(){}


Object::Object(std::string const& file, model::attrib_flag_t import_attribs)
  :color_{glm::fvec3{1.0f, 1.0f, 1.0f}}
  ,velocity_{glm::fvec3{0.0f, 0.0f, 0.0f}}
  ,file_{file}{
    load_model(import_attribs);
}


float* Object::get_model_matrix(){
  model_matrix_ = translation_matrix_ * rotation_matrix_ * scale_matrix_;
  return glm::value_ptr(model_matrix_);
}

void Object::translate(glm::fvec3 const& v){
  translation_matrix_ = glm::translate(translation_matrix_, v);
}

void Object::rotate(glm::fvec3 const& axis, float r){
  rotation_matrix_ = glm::rotate(rotation_matrix_, r, axis);
}

void Object::scale(float scale){
  scale_matrix_ = glm::scale(scale_matrix_, glm::fvec3{scale, scale, scale});
}

void Object::move(float delta_t){
  translate(velocity_* delta_t);
}

void Object::activate() const {
  glBindVertexArray(m_vertex_AO);
}

void Object::deactivate() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Object::draw() const {
  //draw triangles from the currently bound buffer
  glDrawElements(m_draw_mode, m_num_elements, model::INDEX.type, NULL);
}

void Object::load_model(model::attrib_flag_t import_attribs) {
  m_model = model_loader::obj(file_, import_attribs);
  std::cout << file_ << " loaded" << std::endl;
  // generate vertex array object
  glGenVertexArrays(1, &m_vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(m_vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &m_vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_model.data.size(), m_model.data.data(), GL_STATIC_DRAW);  
  //TODO: SHOULD DEPEND ON INPUT ATTRIBUTES
  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, m_model.vertex_bytes, m_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  // activate third attribute on gpu
  glEnableVertexAttribArray(1);
  // third attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, m_model.vertex_bytes, m_model.offsets[model::TEXCOORD]);
    
  
  // generate generic buffer
  glGenBuffers(1, &m_element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * m_model.indices.size(), m_model.indices.data(), GL_STATIC_DRAW);

   

  // store type of primitive to draw
  m_draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  m_num_elements = GLsizei(m_model.indices.size());

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

