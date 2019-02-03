#include <rtr/ren/headers.hpp>
#include <rtr/ren/particleHeaders.hpp>
#include <rtr/camera/Camera.h>
#include <rtr/shaders/SimpleShaders.h>
#include <rtr/shaders/TextureShaders.h>
#include <rtr/shaders/ParticleShaders.h>
#include <rtr/object/object.hpp>
#include <rtr/transformation/mesh.h>
#include <rtr/imgui/imgui.h>
#include <rtr/imgui/imgui_impl_freeglut.h>
#include <rtr/imgui/imgui_impl_opengl2.h>

#define CAM_MAT "camera_matrix"
#define PROJ_MAT "projection_matrix"
#define MODEL_MAT "model_matrix"
#define NORM_MAT "normal_matrix"
#define COLOR_MODE "color_mode"
#define SUN "sun_pos"
#define AMB "amb"
#define DIFF "diff"
#define SPEC "spec"
#define IN_COL "in_color"
#define TEX "texture1"

#define GUI_STD 0
#define GUI_FIX 1
#define GUI_DRAG 2
#define GUI_PAUSE 3


#define PICK 1
#define TEXTURE 2
#define SPRING 4
#define NORMALS 8
#define FIXED 16


int32_t window_width_  = 1080;
int32_t window_height_ = 720;

//3d mesh input file
std::string sphere_file_ = "../resources/objects/sphere.obj";
std::string cube_file_ = "../resources/objects/cubemod.obj";

Object* sphere_, *cube_;
SimpleShaders* object_shader_;
TextureShaders* cloth_shader_;
Camera* camera;
ClothEffect* effect_;

bool render_texture = false;

uint64_t elapsed_ms_{0};
uint64_t real_elapsed_ms{0};
uint64_t old_elapsed_ms{0};

//GUi
ImGuiIO* io;
int gui_mode_ = GUI_STD;
bool setup_window_ = false;
bool shader_window_ = false;
bool interact_window_ = false;
bool show_fixed_ = false;

//uniforms
int color_mode_ = 1;
int rotation_ = 0; //xz-plane
glm::vec3 amb_, diff_, spec_, sun_;
glm::vec3 clear_color_ = glm::vec3(0.5f, 0.5f, 0.5f);
static int current_tex_ = 1;
std::vector<glm::vec2> tex_dims_;
glm::vec2 dim_tex_0_ {512, 512}; //brick
glm::vec2 dim_tex_1 {96, 96}; //cubone.png
glm::vec2 dim_tex_2 {500, 500}; //swampert.jpg
glm::vec2 dim_tex_3 {1280, 640}; //flag.png
glm::vec2 dim_tex_4 {390, 400}; //pikachu.png
glm::vec2 dim_tex_5 {580, 383}; //sweater.jpg
glm::vec2 dim_tex_6 {280, 280}; //vrgroup.png
const char* list[] = {"Cubone", "Swampert", "Flag", "Pikachu", "Sweater", "VR"};


float drag_distance_;

void createShaders(){

  cloth_shader_->loadVertexFragmentShaders("../resources/shaders/cloth.vert.glsl", "../resources/shaders/cloth.frag.glsl");
  cloth_shader_->mDrawMode = GL_TRIANGLES;
  cloth_shader_->addUniform(CAM_MAT);
  cloth_shader_->addUniform(PROJ_MAT);
  cloth_shader_->addUniform(COLOR_MODE);
  cloth_shader_->addUniform(AMB);
  cloth_shader_->addUniform(DIFF);
  cloth_shader_->addUniform(SPEC);
  cloth_shader_->addUniform(SUN);
  cloth_shader_->addUniform(NORM_MAT);
  cloth_shader_->addUniform(TEX);
  {
    cloth_shader_->addTexture(dim_tex_1.x, dim_tex_1.y, "texture1", "../resources/textures/Cloth/cubone.png");
    tex_dims_.push_back(dim_tex_1);
    cloth_shader_->addTexture(dim_tex_2.x, dim_tex_2.y, "texture1", "../resources/textures/Cloth/swampert.jpg");
    tex_dims_.push_back(dim_tex_2);
    cloth_shader_->addTexture(dim_tex_3.x, dim_tex_3.y, "texture1", "../resources/textures/Cloth/flag.png");
    tex_dims_.push_back(dim_tex_3);
    cloth_shader_->addTexture(dim_tex_4.x, dim_tex_4.y, "texture1", "../resources/textures/Cloth/pikachu.jpg");
    tex_dims_.push_back(dim_tex_4);
    cloth_shader_->addTexture(dim_tex_5.x, dim_tex_5.y, "texture1", "../resources/textures/Cloth/sweater1.jpg");
    tex_dims_.push_back(dim_tex_5);
    cloth_shader_->addTexture(dim_tex_6.x, dim_tex_6.y, "texture1", "../resources/textures/Cloth/vrgroup.png");
    tex_dims_.push_back(dim_tex_6);
  }

  object_shader_->loadVertexFragmentShaders("../resources/shaders/sphere.vert.glsl", "../resources/shaders/sphere.frag.glsl");
  object_shader_->addUniform(MODEL_MAT);
  object_shader_->addUniform(CAM_MAT);
  object_shader_->addUniform(PROJ_MAT);
  object_shader_->addUniform(IN_COL);


}

glm::vec3 rayCast(int x, int y){
  float n_x = (2.0f * x) / window_width_ - 1.0f;
  float n_y = 1.0f - (2.0f * y) / window_height_;

  glm::vec4 ray = glm::vec4(n_x, n_y, -1.f, 1.f);
  ray = glm::inverse(camera->getProjectionMatrix()) * ray;
  ray = glm::inverse(camera->getViewMatrix()) * glm::vec4(ray.x, ray.y, -1.f, 0.f);

  return glm::normalize(glm::vec3(ray));
}

void drag(int x, int y){
  glm::vec3 dir = glm::normalize(camera->getViewDir());
  glm::vec3 ray = glm::normalize(rayCast(x, y));
  float result;

  glm::intersectRayPlane(
          camera->getPosition(), ray,
          camera->getPosition() + dir * drag_distance_, -dir,
          result
  );
  glm::vec3 pos = camera->getPosition() + result * ray;
  effect_->m_dragMutex.lock();
  effect_->m_dragParticle.x = pos.x;
  effect_->m_dragParticle.y = pos.y;
  effect_->m_dragParticle.z = pos.z;
  effect_->m_dragMutex.unlock();
}


//render callback function; called every frame
void glut_display() {
  glDisable(GL_BLEND);

  // update camera
  camera->update();

  if(effect_->m_sphere) {
    sphere_->translation_matrix_ = glm::translate(glm::mat4(), effect_->m_spherePos);
    sphere_->scale_matrix_ = glm::scale(glm::mat4(), glm::vec3(effect_->m_sphereRad * 0.95));

    object_shader_->activate();

    //upload model, camera and projection matrices to GPU (1 matrix, transposed, address beginnings of data block)
    glUniformMatrix4fv(object_shader_->getUniform(MODEL_MAT), 1, GL_FALSE, sphere_->get_model_matrix());
    glUniformMatrix4fv(object_shader_->getUniform(CAM_MAT), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(object_shader_->getUniform(PROJ_MAT), 1, GL_FALSE,
                       glm::value_ptr(camera->getProjectionMatrix()));
    glUniform4fv(object_shader_->getUniform(IN_COL), 1, glm::value_ptr(glm::vec4(1.f)));


    //bind the VBO of the model such that the next draw call will render with these vertices
    sphere_->activate();
    //draw triangles from the currently bound buffer
    sphere_->draw();
    sphere_->deactivate();
  }

  if(effect_->m_cube) {
    cube_->translation_matrix_ = glm::translate(glm::mat4(), effect_->m_cubePos);
    cube_->scale_matrix_ = glm::scale(glm::mat4(), glm::vec3(effect_->m_cubeDim));

    object_shader_->activate();

    //upload model, camera and projection matrices to GPU (1 matrix, transposed, address beginnings of data block)
    glUniformMatrix4fv(object_shader_->getUniform(MODEL_MAT), 1, GL_FALSE, cube_->get_model_matrix());
    glUniformMatrix4fv(object_shader_->getUniform(CAM_MAT), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(object_shader_->getUniform(PROJ_MAT), 1, GL_FALSE,
                       glm::value_ptr(camera->getProjectionMatrix()));
    glUniform4fv(object_shader_->getUniform(IN_COL), 1, glm::value_ptr(glm::vec4(1.f)));


    //bind the VBO of the model such that the next draw call will render with these vertices
    cube_->activate();
    //draw triangles from the currently bound buffer
    cube_->draw();
    cube_->deactivate();
  }

  if(gui_mode_ == GUI_DRAG && glm::length(effect_->m_dragParticle) != 0){
    sphere_->translation_matrix_ = glm::translate(glm::mat4(), glm::vec3(effect_->m_dragParticle));
    sphere_->scale_matrix_ = glm::scale(glm::mat4(), glm::vec3(effect_->m_gridD / 4.f));

    object_shader_->activate();

    //upload model, camera and projection matrices to GPU (1 matrix, transposed, address beginnings of data block)
    glUniformMatrix4fv(object_shader_->getUniform(MODEL_MAT), 1, GL_FALSE, sphere_->get_model_matrix());
    glUniformMatrix4fv(object_shader_->getUniform(CAM_MAT), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(object_shader_->getUniform(PROJ_MAT), 1, GL_FALSE,
                       glm::value_ptr(camera->getProjectionMatrix()));
    glUniform4fv(object_shader_->getUniform(IN_COL), 1, glm::value_ptr(glm::vec4(0.2f, 1.f, 0.f, 1.f)));


    //bind the VBO of the model such that the next draw call will render with these vertices
    sphere_->activate();
    //draw triangles from the currently bound buffer
    sphere_->draw();
    sphere_->deactivate();
  } else if(gui_mode_ == GUI_FIX || show_fixed_){
    object_shader_->activate();
    sphere_->activate();
    sphere_->scale_matrix_ = glm::scale(glm::mat4(), glm::vec3(effect_->m_gridD / 4.f));
    glUniformMatrix4fv(object_shader_->getUniform(CAM_MAT), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(object_shader_->getUniform(PROJ_MAT), 1, GL_FALSE,
                       glm::value_ptr(camera->getProjectionMatrix()));
    glUniform4fv(object_shader_->getUniform(IN_COL), 1, glm::value_ptr(glm::vec4(0.8f, 0.f, 0.f, 1.f)));

    auto fixed = effect_->getfixedParticles();
    for(auto const& pos : fixed){
      sphere_->translation_matrix_ = glm::translate(glm::mat4(), pos);
      glUniformMatrix4fv(object_shader_->getUniform(MODEL_MAT), 1, GL_FALSE, sphere_->get_model_matrix());
      sphere_->draw();
    }

    sphere_->deactivate();

  }

  cloth_shader_->activate();
  glUniformMatrix4fv(cloth_shader_->getUniform(CAM_MAT), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
  glUniformMatrix4fv(cloth_shader_->getUniform(PROJ_MAT), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
  glUniformMatrix4fv(cloth_shader_->getUniform(NORM_MAT), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(camera->getViewMatrix()))));
  glUniform1i(cloth_shader_->getUniform(COLOR_MODE), color_mode_);
  glUniform3fv(cloth_shader_->getUniform(AMB), 1, glm::value_ptr(amb_));
  glUniform3fv(cloth_shader_->getUniform(DIFF), 1, glm::value_ptr(diff_));
  glUniform3fv(cloth_shader_->getUniform(SPEC), 1, glm::value_ptr(spec_));
  glUniform3fv(cloth_shader_->getUniform(SUN), 1, glm::value_ptr(sun_ * 1000.f));
  glUniform1i(cloth_shader_->getUniform(TEX), (current_tex_ + 1));

  real_elapsed_ms = glutGet(GLUT_ELAPSED_TIME);
  effect_->update(0.f);
  if(gui_mode_ == GUI_STD || gui_mode_ == GUI_DRAG)effect_ ->cpuUpdate(16.f/1000.f);
  effect_ ->gpuUpdate();
  effect_ ->render();
  old_elapsed_ms = real_elapsed_ms;

  cloth_shader_->deactivate();
  //unbind, unuse
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);
  glEnable(GL_BLEND);

}

void gui_display()
{
  if(ImGui::Begin("Sloth Simulation")) {

    if(ImGui::Button("Show Setup Options")) setup_window_ = true;
    if(ImGui::Button("Show Shader Options")) shader_window_ = true;
    if(ImGui::Button("Show Interactive Controls")) interact_window_ = true;

    ImGui::Separator();
    if (ImGui::Button("Reset")) {                           // Buttons return true when clicked (most widgets return true when edited/activated)
      if(color_mode_ == TEXTURE){
        effect_->m_gridH = effect_->m_gridW * (tex_dims_[current_tex_].y / tex_dims_[current_tex_].x);
      }
      if(rotation_ == 0) {effect_->m_gridRot = glm::fmat4();}
      else if (rotation_ == 1) {effect_->m_gridRot = glm::rotate(glm::fmat4(), (float)(M_PI / 2.f), glm::vec3(1.f, 0.f, 0.f));}
      else if (rotation_ == 2) {effect_->m_gridRot = glm::rotate(glm::fmat4(), (float)(M_PI / 2.f), glm::vec3(0.f, 0.f, 1.f));}
      effect_->reset();
    }

    ImGui::Separator();
    ImGui::RadioButton("Play", &gui_mode_, GUI_STD);
    ImGui::RadioButton("Pause", &gui_mode_, GUI_PAUSE);
    ImGui::RadioButton("Fix Points", &gui_mode_, GUI_FIX);
    ImGui::RadioButton("Drag Points", &gui_mode_, GUI_DRAG);
    if (ImGui::Button("Release all Points")) effect_->movingAllParticles();
    ImGui::Checkbox("Show fixed Points", &show_fixed_);

    ImGui::Separator();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);

  }
  ImGui::End();

  if(setup_window_){

    if(ImGui::Begin("Setup a new Cloth", &setup_window_)){

      ImGui::Text("Customize new Cloth");
      ImGui::Checkbox("Wind", &effect_->m_wind);
      ImGui::Checkbox("Gravity", &effect_->m_gravity);
      ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
      ImGui::InputFloat("Mass", &effect_->m_mass);

      ImGui::Separator();
      ImGui::Checkbox("Structural Springs", &effect_->m_structure);
      ImGui::Checkbox("Shear Springs", &effect_->m_shear);
      ImGui::Checkbox("Bend Springs", &effect_->m_bend);

      ImGui::Separator();
      ImGui::Checkbox("Self-Collision", &effect_->m_collision);
      ImGui::Separator();
      ImGui::Checkbox("Sphere-Collision", &effect_->m_sphere);
      {
        ImGui::Text("Sphere Position");
        ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
        ImGui::SliderFloat("##spx", &effect_->m_spherePos.x, -10.f, 10.f); ImGui::SameLine();
        ImGui::SliderFloat("##spy", &effect_->m_spherePos.y, -10.f, 10.f); ImGui::SameLine();
        ImGui::SliderFloat("##spz", &effect_->m_spherePos.z, -10.f, 10.f);
        ImGui::PopItemWidth();
      }
      ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
      ImGui::InputFloat("Sphere Radius", &effect_->m_sphereRad);
      ImGui::Separator();
      ImGui::Checkbox("Cube-Collision", &effect_->m_cube);
      {
        ImGui::Text("Cube Position");
        ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
        ImGui::SliderFloat("##cpx", &effect_->m_cubePos.x, -10.f, 10.f); ImGui::SameLine();
        ImGui::SliderFloat("##cpy", &effect_->m_cubePos.y, -10.f, 10.f); ImGui::SameLine();
        ImGui::SliderFloat("##cpz", &effect_->m_cubePos.z, -10.f, 10.f);
        ImGui::PopItemWidth();
      }
      ImGui::InputFloat("Cube Dim", &effect_->m_cubeDim);


      ImGui::Separator();
      ImGui::Checkbox("Restrict Stretch", &effect_->m_stretch);
      ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
      ImGui::InputInt("Iterations of Constraint Tests", &effect_->m_stretchIter);

      ImGui::Separator();
      ImGui::Text("Cloth dimension");
      ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
      ImGui::InputInt("Width", &effect_->m_gridW);
      ImGui::InputInt("Height", &effect_->m_gridH);
      ImGui::InputFloat("Step Size", &effect_->m_gridD);
      ImGui::Text("Rotation");
      ImGui::RadioButton("XZ-plane", &rotation_, 0); ImGui::SameLine();
      ImGui::RadioButton("XY-plane", &rotation_, 1); ImGui::SameLine();
      ImGui::RadioButton("YZ-plane", &rotation_, 2);
    }

    ImGui::End();
  }

  if(shader_window_){
    if(ImGui::Begin("Shader Options", &shader_window_)){
      {
        ImGui::Text("Sun Position");
        ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
        ImGui::SliderFloat("##supx", &sun_.x, -1.f, 1.f); ImGui::SameLine();
        ImGui::SliderFloat("##supy", &sun_.y, 0.f, 1.f); ImGui::SameLine();
        ImGui::SliderFloat("##supz", &sun_.z, -1.f, 1.f);
        ImGui::PopItemWidth();
      }
      ImGui::ColorEdit3("Pick Amb", (float *) &amb_); // Edit 3 floats representing a color
      ImGui::ColorEdit3("Pick Diff", (float *) &diff_); // Edit 3 floats representing a color
      ImGui::ColorEdit3("Pick Spec", (float *) &spec_); // Edit 3 floats representing a color
      ImGui::RadioButton("Pick Colour", &color_mode_, PICK);
      ImGui::RadioButton("Texture", &color_mode_, TEXTURE);
      ImGui::RadioButton("Spring Stretch", &color_mode_, SPRING);
      ImGui::RadioButton("Normals", &color_mode_, NORMALS);

      ImGui::Text("Pick Texture");
      ImGui::ListBox("", &current_tex_, list, IM_ARRAYSIZE(list), 4);

      ImGui::ColorEdit3("clear colour", (float *) &clear_color_);
    }

    ImGui::End();
  }

  if(interact_window_){
    if(ImGui::Begin("Interactive Controls")){
      ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
      ImGui::SliderFloat("Damping", &effect_->m_damp, 0.f, 1.f);
      ImGui::InputFloat("Resistance", &effect_->m_res);

      ImGui::InputFloat("Structural k", &effect_->m_kStruct);
      ImGui::InputFloat("Shear k", &effect_->m_kShear);
      ImGui::InputFloat("Bend k", &effect_->m_kBend);

      ImGui::Separator();
      ImGui::SliderFloat("Max Stretch", &effect_->m_maxStretch, 1.f, 2.f);
      ImGui::SliderFloat("Min Stretch", &effect_->m_minStretch, 0.f, 1.f);

      ImGui::Separator();
      ImGui::SliderFloat("Collision Distance", &effect_->m_collisionDist, 0.f, 2.f);
      ImGui::Separator();
      ImGui::SliderFloat("Sphere Friction", &effect_->m_sphereFric, 0.0f, 1.0f);

      ImGui::Separator();
      ImGui::Text("Wind Direction");
      ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
      ImGui::SliderFloat("##wx", &effect_->m_windVec.x, -2.f, 2.f, "%.3f", 3.f); ImGui::SameLine();
      ImGui::SliderFloat("##wy", &effect_->m_windVec.y, -2.f, 2.f, "%.3f", 3.f); ImGui::SameLine();
      ImGui::SliderFloat("##wz", &effect_->m_windVec.z, -2.f, 2.f, "%.3f", 3.f);
      ImGui::PopItemWidth();

    }

    ImGui::End();
  }

}

void glut_display_func()
{
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL2_NewFrame();
  ImGui_ImplFreeGLUT_NewFrame();

  //set the viewport, background color, and reset default framebuffer
  glViewport(0, 0, (GLsizei)window_width_, (GLsizei)window_height_);
  glClearColor(clear_color_.x, clear_color_.y, clear_color_.y, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glut_display();
  gui_display();

  // Rendering
  ImGui::Render();
  glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL2+ code.
  ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
  glutSwapBuffers();
}

//what happens when the mouse is moved over the window
void glut_motion(int32_t _x, int32_t _y) {
  if(io->WantCaptureMouse){
    ImGui_ImplFreeGLUT_MotionFunc(_x, _y);
  } else if (gui_mode_ != GUI_FIX && gui_mode_ != GUI_DRAG) {
    camera->mouseMove(_x, _y);
  }
}

//what happens if mouse buttons change their state on top of the window
void glut_mouse(int32_t _button, int32_t _state, int32_t _x, int32_t _y) {
  if(io->WantCaptureMouse){
    ImGui_ImplFreeGLUT_MouseFunc(_button, _state, _x, _y);
  } else if (gui_mode_ == GUI_FIX && _button == GLUT_LEFT_BUTTON
             && _state == GLUT_DOWN) {
    effect_->fixedParticles(effect_->findClosest(rayCast(_x, _y)));
  } else if (gui_mode_ == GUI_DRAG&& _button == GLUT_LEFT_BUTTON
            && _state == GLUT_DOWN) {
    if(glm::length(effect_->m_dragParticle) == 0.f) {
      effect_->m_dragMutex.lock();
      effect_->setDrag(rayCast(_x, _y));
      effect_->m_dragMutex.unlock();
      drag_distance_ = glm::dot(
              glm::normalize(camera->getViewDir()),
              glm::vec3(effect_->m_dragParticle) - camera->getPosition()
      );
    } else {
      effect_->m_dragMutex.lock();
      effect_->releaseDrag();
      effect_->m_dragMutex.unlock();
    }
  } else if (gui_mode_ != GUI_FIX && gui_mode_ != GUI_DRAG){
    camera->mouseButton(_button, _state, _x, _y);
  }
}

void glut_mouse_passive(int32_t _x, int32_t _y) {
  ImGui_ImplFreeGLUT_MotionFunc(_x, _y);
  if(!io->WantCaptureMouse && GUI_DRAG && glm::length(effect_->m_dragParticle) != 0){
    drag(_x, _y);
  }
}

//keyboard events you want to register
void glut_keyboard(uint8_t _key, int32_t _x, int32_t _y) {

  if(io->WantCaptureKeyboard){
    ImGui_ImplFreeGLUT_KeyboardFunc(_key, _x, _y);
  } else {
    switch (_key) {
      //escape key
      case 27:
        exit(0);
        break;
      case '.':
        glutFullScreenToggle();
        break;
      case ' ':
        camera->stop();
        break;
      case 'q':
        effect_->toggleUpdate();
        break;
      default:
        std::cout << "key pressed" << std::endl;
        break;
    }
  }
}

//on resize window, re-create projection matrix
void glut_resize(int32_t _width, int32_t _height) {
  window_width_ = _width;
  window_height_ = _height;
  camera->updateProjection(_width/(float)_height);
  ImGui_ImplFreeGLUT_ReshapeFunc(_width, _height);
}

//on every 16 ms
static void glut_timer(int32_t _e) {

  glutPostRedisplay();
  glutTimerFunc(16, glut_timer, 1);
  elapsed_ms_ += 16;
}

//entry point
int32_t main(int32_t argc, char* argv[]) {

  glutInit(&argc, argv);


  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
  glutInitWindowSize(window_width_, window_height_);
  glutCreateWindow("Dear ImGui FreeGLUT+OpenGL2 Example");
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glewExperimental = GL_TRUE;
  glewInit();

  //register glut-callbacks
  //the arguments are pointers to the functions we defined above
  glutDisplayFunc(glut_display_func);
  glutReshapeFunc(glut_resize);
  glutKeyboardFunc(glut_keyboard);
  glutKeyboardUpFunc(ImGui_ImplFreeGLUT_KeyboardUpFunc);
  glutSpecialFunc(ImGui_ImplFreeGLUT_SpecialFunc);
  glutSpecialUpFunc(ImGui_ImplFreeGLUT_SpecialUpFunc);
  glutMotionFunc(glut_motion);
  glutPassiveMotionFunc(glut_mouse_passive);
  glutMouseFunc(glut_mouse);
  glutMouseWheelFunc(ImGui_ImplFreeGLUT_MouseWheelFunc);

  glutTimerFunc(16, glut_timer, 1);

  glutShowWindow();

  ///////////////////////////////////////////////////////////////

  // Setup Dear ImGui context
  ImGui::CreateContext();
  io = &ImGui::GetIO();
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  ImGui_ImplFreeGLUT_Init();
  ImGui_ImplOpenGL2_Init();
  //ImGui_ImplFreeGLUT_InstallFuncs();
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  ///////////////////////////////////////////////////////////////

  camera = new Camera((window_width_/(float)window_height_), glm::fvec3{0.0f, 0.f, 0.f});
  camera->setPosition(glm::fvec3{0.f, 3.f, 20.f});
  camera->setViewDir(glm::fvec3{0.f, 0.f, -20.f});

  ///////////////////////////////////////////////////////////////

  object_shader_ = new SimpleShaders();
  cloth_shader_ = new TextureShaders(0, "", 1);
  createShaders();
  ///////////////////////////////////////////////////////////////

  sphere_ = new Object(sphere_file_, (model::POSITION | model::TEXCOORD));
  cube_ = new Object(cube_file_, (model::POSITION));

  effect_ = new ClothEffect();
  effect_->m_gridRot = glm::rotate(glm::fmat4(), (float)(M_PI / 2.f), glm::vec3(1.f, 0.f, 0.f));
  effect_->m_gridW = 30;
  effect_->m_gridH = 30;
  effect_->m_gridD = 0.4;
  effect_->m_damp = 0.01f;
  effect_->m_kStruct = 1.f;
  effect_->m_kShear = 1.f;
  effect_->m_kBend = 10.f;
  effect_->m_minStretch = 0.f;
  effect_->m_maxStretch = 1.2f;
  effect_->m_sphereRad = 5.f;
  effect_->m_spherePos = glm::vec3(0, -6, 0);
  effect_->m_collisionDist = 1.8f;
  effect_->m_stretchIter = 3;
  effect_->m_mass = 100;
  effect_->m_collision = true;
  effect_->m_wind = true;
  effect_->m_windVec = glm::vec3(1.5f, 0.1f, 0.f);
  effect_->m_res = 1.f;
  effect_->init(10, camera);
  effect_->reset();
  for(int i = 0; i < 30; ++i){
    effect_->fixedParticles(i * 30);
  }

  current_tex_ = 1;
  color_mode_ = TEXTURE;
  ///////////////////////////////////////////////////////////////
  old_elapsed_ms = glutGet(GLUT_ELAPSED_TIME);
  //start the main loop (which mainly calls the glutDisplayFunc)
  glutMainLoop();

  ///////////////////////////////////////////////////////////////

  // Cleanup
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplFreeGLUT_Shutdown();
  ImGui::DestroyContext();


  return 0;
}
