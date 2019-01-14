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

//model-to-world space representation
glm::mat4 model_matrix_ = glm::mat4();

//inverse world-to-camera space transformation
glm::mat4 camera_matrix_ = glm::mat4(0.0f);

//camera-to-screen space transformation
glm::mat4 projection_matrix_ = glm::mat4(0.0f);

//3d mesh input file
std::string obj_file_ = "../resources/objects/sphere.obj";

Object* object_;

SimpleShaders* shader_;
ParticleShaders* particle_shader_;

Camera* camera;

ClothEffect* effect_;

bool render_texture = false;

uint64_t elapsed_ms_{0};

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static bool show_demo_window = true;
static bool show_another_window = false;
ImGuiIO* io;

void createShaders(){
  particle_shader_->loadVertexFragmentShaders("../resources/shaders/particlePoint.vert", "../resources/shaders/particlePoint.frag");
  //particle_shader_->loadVertGeomFragShaders("../resources/shaders/particleQuad.vert", "../resources/shaders/particleQuad.geom", "../resources/shaders/particlePoint.frag");
  particle_shader_->locateUniforms();
  particle_shader_->mDrawMode = GL_LINE;
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

  effect_ ->cpuUpdate(16.f/1000.f);
  effect_ ->gpuUpdate();
  effect_ ->render();

  //unbind, unuse
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);
}

void gui_display()
{
  static float f = 0.0f;
  static int counter = 0;

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
  ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

  if (ImGui::Button("Reset"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    effect_->reset();

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
  } else {
    camera->mouseMove(_x, _y);
  }
}

//what happens if mouse buttons change their state on top of the window
void glut_mouse(int32_t _button, int32_t _state, int32_t _x, int32_t _y) {
  if(io->WantCaptureMouse){
    ImGui_ImplFreeGLUT_MouseFunc(_button, _state, _x, _y);
  } else {
    camera->mouseButton(_button, _state, _x, _y);
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
  glutPassiveMotionFunc(ImGui_ImplFreeGLUT_MotionFunc);
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
