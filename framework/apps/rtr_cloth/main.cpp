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

int32_t window_width_  = 800;
int32_t window_height_ = 800;

//3d mesh input file
std::string obj_file_ = "../resources/objects/sphere.obj";

Object* object_;
SimpleShaders* shader_;
ParticleShaders* particle_shader_;
Camera* camera;
ClothEffect* effect_;

bool render_texture = false;

uint64_t elapsed_ms_{0};
//GUi
ImGuiIO* io;
bool gui_fix_ = false;

//uniforms
int color_mode_ = 0;
glm::vec3 pick_color_;

void createShaders(){
  particle_shader_->loadVertexFragmentShaders("../resources/shaders/particlePoint.vert", "../resources/shaders/particlePoint.frag");
  //particle_shader_->loadVertGeomFragShaders("../resources/shaders/particleQuad.vert", "../resources/shaders/particleQuad.geom", "../resources/shaders/particlePoint.frag");
  particle_shader_->locateUniforms();
  particle_shader_->mDrawMode = GL_LINE;
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

  // update camera
  camera->update();

  //glDisable(GL_BLEND);
  shader_->activate();

  //upload model, camera and projection matrices to GPU (1 matrix, transposed, address beginnings of data block)
  glUniformMatrix4fv(shader_->getUniform("model_matrix"), 1, GL_FALSE, object_->get_model_matrix());
  glUniformMatrix4fv(shader_->getUniform("camera_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
  glUniformMatrix4fv(shader_->getUniform("projection_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));

  //bind the VBO of the model such that the next draw call will render with these vertices
  //object_->activate();
  //draw triangles from the currently bound buffer
  //object_->draw();
  //object_->deactivate();

  //glEnable(GL_BLEND);

  particle_shader_->setCameraMatrix(camera->getViewMatrix());
  particle_shader_->setProjectionMatrix(camera->getProjectionMatrix());
  particle_shader_->activate();

  effect_->update(0.f);
  if(!gui_fix_)effect_ ->cpuUpdate(16.f/1000.f);
  effect_ ->gpuUpdate();
  effect_ ->render();

  //unbind, unuse
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);
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

  ImGui::ColorEdit3("clear color", (float*)&pick_color_); // Edit 3 floats representing a color
  ImGui::RadioButton("Pick Colour", &color_mode_, 1);
  ImGui::RadioButton("Texture", &color_mode_, 2);
  ImGui::RadioButton("Spring Stretch", &color_mode_, 4);
  ImGui::RadioButton("Normals", &color_mode_, 8);
  ImGui::RadioButton("Fixed", &color_mode_, 16);

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
  glClearColor(1.0f, 0.0f, 0.1f, 1.0f);
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
  //glEnable(GL_DEPTH_TEST);
  //glDisable(GL_CULL_FACE);
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
  shader_ = new SimpleShaders();

  ///////////////////////////////////////////////////////////////

  shader_->loadVertexFragmentShaders("../resources/shaders/tutorial.vert", "../resources/shaders/color.frag");
  shader_->addUniform("model_matrix");
  shader_->addUniform("camera_matrix");
  shader_->addUniform("projection_matrix");
  shader_->addUniform("color_mode");

  particle_shader_ = new ParticleShaders();

  createShaders();

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
  effect_->init(10, camera);
  effect_->fixedParticles(45);
  effect_->reset();

  ///////////////////////////////////////////////////////////////

  //start the main loop (which mainly calls the glutDisplayFunc)
  glutMainLoop();

  ///////////////////////////////////////////////////////////////

  // Cleanup
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplFreeGLUT_Shutdown();
  ImGui::DestroyContext();


  return 0;
}
