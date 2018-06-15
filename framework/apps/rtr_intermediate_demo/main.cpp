
#include <rtr/ren/headers.hpp>
#include <rtr/ren/particleHeaders.hpp>
#include <rtr/camera/Camera.h>
#include <rtr/shaders/SimpleShaders.h>
#include <rtr/shaders/TextureShaders.h>
#include <rtr/shaders/ParticleShaders.h>
#include <rtr/object/object.hpp>

int32_t window_width_  = 1280;
int32_t window_height_ = 720;

//model-to-world space representation
glm::mat4 model_matrix_ = glm::mat4();

//inverse world-to-camera space transformation
glm::mat4 camera_matrix_ = glm::mat4(0.0f);

//camera-to-screen space transformation
glm::mat4 projection_matrix_ = glm::mat4(0.0f);

//3d mesh input file
std::string obj_file_ = "../resources/objects/DRAGNFLY.OBJ";

Object* dragonfly;

SimpleShaders* shader_;
TextureShaders* texture_shader_;
ParticleShaders* particle_shader_;

Camera* camera;

FlameThrowerEffect* fire_;

bool render_texture = false;

uint64_t elapsed_ms_{0};

void createShaders(){
  texture_shader_->loadVertGeomFragShaders("../resources/shaders/particleQuad.vert", "../resources/shaders/particleFire.geom", "../resources/shaders/particleFire.frag");
  texture_shader_->locateUniforms();

  particle_shader_->loadVertexFragmentShaders("../resources/shaders/particlePoint.vert", "../resources/shaders/particlePoint.frag");
  particle_shader_->locateUniforms();
}

void make_projection_matrix() {
  
  float pi = 4.0*std::atan(1.0);
  float deg2rad = pi/180.f;
  float field_of_view = 50.f;
  float near_plane = 0.01f;
  float far_plane = 100.f;
  float aspect_ratio = window_width_/(float)window_height_;
  float maxy = std::tan(deg2rad * (field_of_view * 0.5f)) * near_plane;
  float maxx = maxy*aspect_ratio;

  float left = -maxx;
  float right = maxx;
  float bottom = -maxy;
  float top = maxy;

  float A = (right+left)/(right-left);
  float B = (top+bottom)/(top-bottom);
  float C = -(far_plane+near_plane)/(far_plane-near_plane);
  float D = -(2.0f*far_plane*near_plane)/(far_plane-near_plane);
  float E = 2.0f*near_plane/(top-bottom);
  float F = 2.0f*near_plane/(right-left);

 projection_matrix_ = {
    F, 0, A, 0,
    0, E, B, 0,
    0, 0, C, D,
    0, 0, -1, 0
  };


}

//create a view matrix given the camera position and its camera frame
void make_camera_matrix(std::vector<float> const& _cam_position,
                        std::vector<float> const& _up_vec,
                        std::vector<float> const& _forward_vec) {
  

  //normalize input vectors
  float length_up_vec      = 0.f;
  float length_forward_vec = 0.f;

  for(int32_t dim_idx = 0; dim_idx < 3; ++dim_idx) {
    length_up_vec      += _up_vec[dim_idx] * _up_vec[dim_idx];
    length_forward_vec += _forward_vec[dim_idx] * _forward_vec[dim_idx];
  }

  length_up_vec      = std::sqrt(length_up_vec);
  length_forward_vec = std::sqrt(length_forward_vec); 

  //camera axis in world space
  std::vector<float>     up = {_up_vec[0]/length_up_vec, _up_vec[1]/length_up_vec, _up_vec[2]/length_up_vec};
  std::vector<float>     forward = {_forward_vec[0]/length_forward_vec, _forward_vec[1]/length_forward_vec, _forward_vec[2]/length_forward_vec};

  //compute right vector through cross product
  std::vector<float> right_vec{up[1]*forward[2]-up[2]*forward[1], 
                               up[2]*forward[0]-up[0]*forward[2], 
                               up[0]*forward[1]-up[1]*forward[0]};
 
  //normalize right
  float length_right_vec = 0.f;
  for(int32_t dim_idx = 0; dim_idx < 3; ++dim_idx) {
    length_right_vec += right_vec[dim_idx] * right_vec[dim_idx];
  }
  length_right_vec = std::sqrt(length_right_vec);
  std::vector<float> right = {right_vec[0]/length_right_vec, right_vec[1]/length_right_vec, right_vec[2]/length_right_vec};

  camera_matrix_ = { right[0], up[0], forward[0],   -_cam_position[0],
    right[1], up[1], forward[1],   -_cam_position[1],
    right[2], up[2], forward[2],   -_cam_position[2],
    0.0f,  0.0f,       0.0f,                1.0f };
}

//render callback function; called every frame
void glut_display() {

  //set the viewport, background color, and reset default framebuffer
  glViewport(0, 0, (GLsizei)window_width_, (GLsizei)window_height_);
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // update camera
  camera->update();

  glDisable(GL_BLEND);
  shader_->activate();


  //upload model, camera and projection matrices to GPU (1 matrix, transposed, address beginnings of data block)
  glUniformMatrix4fv(shader_->getUniform("model_matrix"), 1, GL_FALSE, dragonfly->get_model_matrix());
  glUniformMatrix4fv(shader_->getUniform("camera_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
  glUniformMatrix4fv(shader_->getUniform("projection_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));

  //bind the VBO of the model such that the next draw call will render with these vertices
  dragonfly->activate();

  
  //draw triangles from the currently bound buffer
  dragonfly->draw();
  dragonfly->deactivate();

  glEnable(GL_BLEND);

  if(render_texture){
    texture_shader_->setCameraMatrix(camera->getViewMatrix());
    texture_shader_->setProjectionMatrix(camera->getProjectionMatrix());
    texture_shader_->setAtlasDim();
    texture_shader_->activate();
  } else {
    particle_shader_->setCameraMatrix(camera->getViewMatrix());
    particle_shader_->setProjectionMatrix(camera->getProjectionMatrix());
    particle_shader_->activate();
  }

  fire_->cpuUpdate(16.f/500.f);
  fire_->gpuUpdate();
  fire_->render();

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
    case 't':
      render_texture = !render_texture;
    default:
      break;

  }
}

//on resize window, re-create projection matrix
void glut_resize(int32_t _width, int32_t _height) {
  window_width_ = _width;
  window_height_ = _height;
  make_projection_matrix();
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

  make_projection_matrix();


  camera = new Camera((window_width_/window_height_), glm::fvec3{0.0f, 0.f, 0.f});

  shader_ = new SimpleShaders();


  shader_->loadVertexFragmentShaders("../resources/shaders/tutorial.vert", "../resources/shaders/tutorial.frag"); 
  shader_->addUniform("model_matrix");
  shader_->addUniform("camera_matrix");
  shader_->addUniform("projection_matrix");

  texture_shader_ = new TextureShaders(400, "../resources/textures/explosion.png", 8);

  particle_shader_ = new ParticleShaders();

  createShaders();

  dragonfly = new Object(obj_file_, (model::POSITION | model::TEXCOORD));
  dragonfly->scale(scale);

  fire_ = new FlameThrowerEffect();
  fire_->init(1000, camera);
  fire_->initRenderer();

  fire_->setPos(fire_pos * scale);
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