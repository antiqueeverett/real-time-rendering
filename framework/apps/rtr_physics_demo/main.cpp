#include <rtr/ren/headers.hpp>
#include <rtr/ren/particleHeaders.hpp>
#include <rtr/camera/Camera.h>
#include <rtr/shaders/SimpleShaders.h>
#include <rtr/shaders/TextureShaders.h>
#include <rtr/shaders/ParticleShaders.h>
#include <rtr/object/object.hpp>
#include <rtr/transformation/mesh.h>

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

SimpleShaders* object_shader_;
ParticleShaders* cloth_shader_;

Camera* camera;

BlackHoleEffect* effect_;

bool render_texture = false;

uint64_t elapsed_ms_{0};

void createShaders(){
  //cloth_shader_->loadVertexFragmentShaders("../resources/shaders/particlePoint.vert", "../resources/shaders/particlePoint.frag");
  cloth_shader_->loadVertGeomFragShaders("../resources/shaders/particleQuad.vert", "../resources/shaders/particleQuad.geom", "../resources/shaders/particlePoint.frag");
  cloth_shader_->locateUniforms();
}


//render callback function; called every frame
void glut_display() {

  //set the viewport, background color, and reset default framebuffer
  glViewport(0, 0, (GLsizei)window_width_, (GLsizei)window_height_);
  glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // update camera
  camera->update();

  glDisable(GL_BLEND);
  object_shader_->activate();

  //upload model, camera and projection matrices to GPU (1 matrix, transposed, address beginnings of data block)
  glUniformMatrix4fv(object_shader_->getUniform("model_matrix"), 1, GL_FALSE, object_->get_model_matrix());
  glUniformMatrix4fv(object_shader_->getUniform("camera_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
  glUniformMatrix4fv(object_shader_->getUniform("projection_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
  

  //bind the VBO of the model such that the next draw call will render with these vertices
  object_->activate();

  
  //draw triangles from the currently bound buffer
  object_->draw();
  object_->deactivate();

  glEnable(GL_BLEND);

  cloth_shader_->setCameraMatrix(camera->getViewMatrix());
  cloth_shader_->setProjectionMatrix(camera->getProjectionMatrix());
  cloth_shader_->activate();

  effect_ ->cpuUpdate(16.f/1000.f);
  effect_ ->gpuUpdate();
  effect_ ->render();

  //unbind, unuse
  glBindBuffer(GL_ARRAY_BUFFER, 0);


  glUseProgram(0);
  
  //swap front- and back-buffer of the default framebuffer
  glutSwapBuffers();
}

//what happens when the mouse is moved over the window
void glut_motion(int32_t _x, int32_t _y) {
  camera->mouseMove(_x, _y);
  glutPostRedisplay();
}

//what happens if mouse buttons change their state on top of the window
void glut_mouse(int32_t _button, int32_t _state, int32_t _x, int32_t _y) {
  camera->mouseButton(_button, _state, _x, _y);
  glutPostRedisplay();
}

//keyboard events you want to register
void glut_keyboard(uint8_t _key, int32_t _x, int32_t _y) {
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
    case 'e':
      effect_->toggleEmit();
      break;
    case 'w':
      effect_->reset();
      break;
    default:
      break;

  }
}

//on resize window, re-create projection matrix
void glut_resize(int32_t _width, int32_t _height) {
  window_width_ = _width;
  window_height_ = _height;
  camera->updateProjection(_width/(float)_height);
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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(window_width_, window_height_);
  glutInitWindowPosition(64, 64);
  glutCreateWindow(argv[0]);
  glutSetWindowTitle("real-time_rendering");
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glewExperimental = GL_TRUE;
  glewInit();

  glm::fvec3 fire_pos = {0.0, 0.008, 0.01};
  float scale = 50.0;



  camera = new Camera((window_width_/(float)window_height_), glm::fvec3{0.0f, 0.f, 0.f});

  object_shader_ = new SimpleShaders();


  object_shader_->loadVertexFragmentShaders("../resources/shaders/tutorial.vert", "../resources/shaders/color.frag");
  object_shader_->addUniform("model_matrix");
  object_shader_->addUniform("camera_matrix");
  object_shader_->addUniform("projection_matrix");

  cloth_shader_ = new ParticleShaders();

  createShaders();

  object_ = new Object(obj_file_, (model::POSITION | model::TEXCOORD));

  effect_ = new BlackHoleEffect();
  effect_->init(100000, camera);
  effect_->initRenderer();



  //register glut-callbacks
  //the arguments are pointers to the functions we defined above
  glutDisplayFunc(glut_display);
  glutReshapeFunc(glut_resize);
  glutKeyboardFunc(glut_keyboard);
  glutMotionFunc(glut_motion);
  glutMouseFunc(glut_mouse);

  glutShowWindow();

  //set a timeout of 16 ms until glut_timer is triggered
  glutTimerFunc(16, glut_timer, 1);

  //start the main loop (which mainly calls the glutDisplayFunc)
  glutMainLoop();


  return 0;
}
