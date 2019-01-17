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
#define COLOR_MODE "color_mode"
#define PICK_COL "pick_color"

int32_t window_width_  = 800;
int32_t window_height_ = 800;

//3d mesh input file
std::string obj_file_ = "../resources/objects/sphere.obj";

Object* object_;
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
bool gui_fix_ = false;

//uniforms
int color_mode_ = 1;
glm::vec3 pick_color_;
glm::vec3 clear_color_ = glm::vec3(0.5f, 0.5f, 0.5f);

void createShaders(){

  cloth_shader_->loadVertexFragmentShaders("../resources/shaders/cloth.vert.glsl", "../resources/shaders/particlePoint.frag");
  cloth_shader_->mDrawMode = GL_TRIANGLES;
  cloth_shader_->addUniform(CAM_MAT);
  cloth_shader_->addUniform(PROJ_MAT);
  cloth_shader_->addUniform(COLOR_MODE);
  cloth_shader_->addUniform(PICK_COL);

  object_shader_->loadVertexFragmentShaders("../resources/shaders/tutorial.vert", "../resources/shaders/color.frag");
  object_shader_->addUniform(MODEL_MAT);
  object_shader_->addUniform(CAM_MAT);
  object_shader_->addUniform(PROJ_MAT);
}

glm::vec3 rayCast(int x, int y){
  float n_x = (2.0f * x) / window_width_ - 1.0f;
  float n_y = 1.0f - (2.0f * y) / window_height_;

  glm::vec4 ray = glm::vec4(n_x, n_y, -1.f, 1.f);
  ray = glm::inverse(camera->getProjectionMatrix()) * ray;
  ray = glm::inverse(camera->getViewMatrix()) * glm::vec4(ray.x, ray.y, -1.f, 0.f);

  return glm::normalize(glm::vec3(ray));
}


//render callback function; called every frame
void glut_display() {
  glDisable(GL_BLEND);

  // update camera
  camera->update();

  object_shader_->activate();

  //upload model, camera and projection matrices to GPU (1 matrix, transposed, address beginnings of data block)
  glUniformMatrix4fv(object_shader_->getUniform(MODEL_MAT), 1, GL_FALSE, object_->get_model_matrix());
  glUniformMatrix4fv(object_shader_->getUniform(CAM_MAT), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
  glUniformMatrix4fv(object_shader_->getUniform(PROJ_MAT), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));

  //bind the VBO of the model such that the next draw call will render with these vertices
  //object_->activate();
  //draw triangles from the currently bound buffer
  //object_->draw();
  //object_->deactivate();

  cloth_shader_->activate();
  glUniformMatrix4fv(cloth_shader_->getUniform(CAM_MAT), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
  glUniformMatrix4fv(cloth_shader_->getUniform(PROJ_MAT), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
  glUniform1i(cloth_shader_->getUniform(COLOR_MODE), color_mode_);
  glUniform3fv(cloth_shader_->getUniform(PICK_COL), 1, glm::value_ptr(pick_color_));

  real_elapsed_ms = glutGet(GLUT_ELAPSED_TIME);
  effect_->update(0.f);
  if(!gui_fix_)effect_ ->cpuUpdate((real_elapsed_ms - old_elapsed_ms)/1000.f);
  effect_ ->gpuUpdate();
  effect_ ->render();
  old_elapsed_ms = real_elapsed_ms;

  //unbind, unuse
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);
  glEnable(GL_BLEND);

}

void gui_display()
{

  ImGui::Begin("Sloth Simulation");

  ImGui::Text("Customize new Cloth");
  ImGui::Checkbox("Gravity", &effect_->m_gravity);
  ImGui::Checkbox("Structural Springs", &effect_->m_structure);
  ImGui::Checkbox("Shear Springs", &effect_->m_shear);
  ImGui::Checkbox("Bend Springs", &effect_->m_bend);
  ImGui::Checkbox("Restrict Stretch", &effect_->m_stretch);

  ImGui::InputFloat("Structural k", &effect_->m_kStruct);
  ImGui::InputFloat("Bend k", &effect_->m_kBend);
  ImGui::InputFloat("Shear k", &effect_->m_kShear);
  ImGui::InputFloat("Max Stretch", &effect_->m_maxStretch);
  ImGui::SliderFloat("Min Stretch", &effect_->m_minStretch, 0.f, 1.f);
  ImGui::SliderFloat("Damping", &effect_->m_damp, 0.f, 1.f);


  ImGui::Text("Cloth dimension");
  ImGui::InputInt("Width", &effect_->m_gridW);
  ImGui::InputInt("Height", &effect_->m_gridH);
  ImGui::InputFloat("Step Size", &effect_->m_gridD);

  if (ImGui::Button("Reset"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    effect_->reset();

  if(effect_->getUpdate()){
    if(ImGui::Button("Pause")) effect_->toggleUpdate();
  } else {
    if(ImGui::Button("Play")) effect_->toggleUpdate();
  }

  ImGui::Checkbox("Fix Points", &gui_fix_);
  if (ImGui::Button("Release all Points")) effect_->movingAllParticles();

  ImGui::ColorEdit3("pick colour", (float*)&pick_color_); // Edit 3 floats representing a color
  ImGui::RadioButton("Pick Colour", &color_mode_, 1);
  ImGui::RadioButton("Texture", &color_mode_, 2);
  ImGui::RadioButton("Spring Stretch", &color_mode_, 4);
  ImGui::RadioButton("Normals", &color_mode_, 8);
  ImGui::RadioButton("Fixed", &color_mode_, 16);


  ImGui::ColorEdit3("clear colour", (float*)&clear_color_);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);


  ImGui::End();
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

  gui_display();
  glut_display();

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
  } else if (!gui_fix_) {
    camera->mouseMove(_x, _y);
  }
}

//what happens if mouse buttons change their state on top of the window
void glut_mouse(int32_t _button, int32_t _state, int32_t _x, int32_t _y) {
  if(io->WantCaptureMouse){
    ImGui_ImplFreeGLUT_MouseFunc(_button, _state, _x, _y);
  } else if (gui_fix_) {
    effect_->fixedParticles(effect_->findClosest(rayCast(_x, _y)));
  } else {
    camera->mouseButton(_button, _state, _x, _y);
  }
}

void glut_mouse_passive(int32_t _x, int32_t _y) {
  ImGui_ImplFreeGLUT_MotionFunc(_x, _y);
  if (gui_fix_ && !io->WantCaptureMouse) {
    auto ray = rayCast(_x, _y);
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
  camera->setPosition(glm::fvec3{0.f, 0.f, 20.f});
  camera->setViewDir(glm::fvec3{0.f, 0.f, -1.f});

  ///////////////////////////////////////////////////////////////

  object_shader_ = new SimpleShaders();
  cloth_shader_ = new TextureShaders(0, "", 1);
  createShaders();
  ///////////////////////////////////////////////////////////////

  object_ = new Object(obj_file_, (model::POSITION | model::TEXCOORD));

  effect_ = new ClothEffect();
  effect_->m_gridPos = glm::fvec4{-5.f, 5.f, 0.f, 1.f};
  //effect_->m_gridRot = glm::rotate(glm::fmat4{}, 2.0f, glm::vec3{1, 0, 0});
  effect_->m_gridW = 10;
  effect_->m_gridH = 10;
  effect_->m_gridD = 1;
  effect_->m_damp = 0.01f;
  effect_->m_kStruct = 1.f;
  effect_->m_kShear = 1.f;
  effect_->m_kBend = 10.f;
  effect_->m_minStretch = 0.f;
  effect_->m_maxStretch = 1.2f;
  effect_->init(10, camera);
  effect_->reset();
  effect_->fixedParticles(45);

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
