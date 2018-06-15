#include <rtr/ren/headers.hpp>
#include <rtr/ren/particleHeaders.hpp>
#include <rtr/shaders/SimpleShaders.h>
#include <rtr/shaders/TextureShaders.h>
#include <rtr/shaders/ParticleShaders.h>




void make_camera_matrix(std::vector<float> const& _cam_position = {0.0f, 0.0f, 5.0f},
                        std::vector<float> const& _up_vec       = {0.0f, 1.0f, 0.0f},
                        std::vector<float> const& _forward_vec  = {0.0f, 0.0f, -1.0f});

int32_t window_width_  = 1280;
int32_t window_height_ = 720;

//model-to-world space representation
glm::mat4 model_matrix_ = glm::mat4(0.0f);

//inverse world-to-camera space transformation
glm::mat4 camera_matrix_ = glm::mat4(0.0f);

//camera-to-screen space transformation
glm::mat4 projection_matrix_ = glm::mat4(0.0f);
//3d mesh input file

//color for 3d model
glm::fvec3 color_{0.f, 0.9f, 0.9f};

std::shared_ptr<ParticleSystem> m_particle_system;
std::shared_ptr<ParticleRenderer> m_rend = std::make_shared<ParticleRenderer> ();

SimpleShaders* shader_ = nullptr;
TextureShaders* texture_shader_ = nullptr;
ParticleShaders* particle_shader_ = nullptr;

bool render_texture = false;

enum class vis_modes {
  flat_color_vis, //0
  normal_vis, // 1
  uv_vis, // 2

  num_vis_modes // 3
};

vis_modes visualization_mode_ = vis_modes::normal_vis;
uint32_t num_vertices_{0};
uint64_t elapsed_ms_{0};

//create a perspective projection matrix given the aspect ratio of the window
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

void createShaders(){
  texture_shader_->loadVertGeomFragShaders("../resources/shaders/particleQuad.vert", "../resources/shaders/particleFire.geom", "../resources/shaders/particleFire.frag");
  texture_shader_->locateUniforms();

  particle_shader_->loadVertexFragmentShaders("../resources/shaders/particlePoint.vert", "../resources/shaders/particlePoint.frag");
  particle_shader_->locateUniforms();
}

//render callback function; called every frame
void glut_display() {

  //set the viewport, background color, and reset default framebuffer
  glViewport(0, 0, (GLsizei)window_width_, (GLsizei)window_height_);
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /////////////////////////////////////////////////////////// static camera
  float circle_radius = 6.f;
  float cam_z = -circle_radius;
  float cam_x = 0.0f;
  
  std::vector<float> cam_pos {cam_x,  0.0f, cam_z};
  std::vector<float> cam_up  {0.0f,   1.0f, 0.0f};
  std::vector<float> cam_fwd {-cam_x, 0.0f, -cam_z};
  /////////////////////////////////////////////////////////// moving camera
  /*
  float cam_sensitivity   = 0.001f;
  float scaled_time_input = -cam_sensitivity * elapsed_ms_; 
  float cos_of_input = std::cos(scaled_time_input);
  float sin_of_input = std::sin(scaled_time_input);

  float circle_radius = 6.f;
  float cam_x = circle_radius * cos_of_input;
  float cam_z = circle_radius * sin_of_input;
  std::vector<float> cam_pos {cam_x,  0.0f, cam_z};
  std::vector<float> cam_up  {0.0f,   1.0f, 0.0f};
  std::vector<float> cam_fwd {-cam_x, 0.0f, -cam_z};

*/
  /////////////////////////////////////////////////////////// camera

  make_camera_matrix(cam_pos, cam_up, cam_fwd);

  if(render_texture){
    texture_shader_->setCameraMatrix(camera_matrix_);
    texture_shader_->setProjectionMatrix(projection_matrix_);
    texture_shader_->setAtlasDim();
    texture_shader_->activate();
  } else {
    particle_shader_->setCameraMatrix(camera_matrix_);
    particle_shader_->setProjectionMatrix(projection_matrix_);
    particle_shader_->activate();
  }

  m_particle_system->update(16.0f / 500.f);
  m_rend->update();
  m_rend->render();


  glUseProgram(0);
  
  //swap front- and back-buffer of the default framebuffer
  glutSwapBuffers();
}

//keyboard events you want to register
void glut_keyboard(uint8_t _key, int32_t _x, int32_t _y) {
  switch (_key) {
    //escape key
    case 27:
      exit(0);
      break;
    case ' ':
      visualization_mode_ = vis_modes((1+uint32_t(visualization_mode_) ) % uint32_t(vis_modes::num_vis_modes));
      break;
    case '.':
      glutFullScreenToggle();
      break;
    case 'u':
      m_particle_system->toggleUpdate();
      break;
    case 'e':
      m_particle_system->toggleEmit();
      break;
    case 't':
      render_texture = !render_texture;
      break;
    case 's':
      m_particle_system->toggleSort();
      break;
    case 'd':
      createShaders();
      break;
    default:
      break;

  }
}

//what happens when the mouse is moved over the window
void glut_motion(int32_t _x, int32_t _y) {
}

//what happens if mouse buttons change their state on top of the window
void glut_mouse(int32_t _button, int32_t _state, int32_t _x, int32_t _y) {
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

void createSys(){
  m_particle_system = std::make_shared<ParticleSystem>(100000);
  auto emmit = std::make_shared<ParticleEmitter>(100.f);

  auto posGenerator = std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{0.f, 0.f, 0.f, 0.0f}));
  auto colGenerator = std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{1.0f}, glm::fvec4{1.0f}, glm::fvec4{0.0f}, glm::fvec4{1.0f}));
  auto velGenerator = std::make_shared<BasicVelGen>(BasicVelGen(glm::fvec4{0.0f, 0.2f, 0.0f, 0.0f}, glm::fvec4{0.0f, 0.5f, 0.0f, 0.0f}));
  auto sphereVelGenerator = std::make_shared<SphereVelGen>(SphereVelGen(0.01f, 0.2f));
  auto timeGenerator = std::make_shared<BasicTimeGen>(BasicTimeGen(2.0f, 6.0f));
  auto spherePosGenerator = std::make_shared<SpherePosGen>(SpherePosGen(0.09f));
  auto rotPosGenerator = std::make_shared<RotatingPosGen>(RotatingPosGen(1.5f));
  auto planePosGenerator = std::make_shared<PlanePosGen>(PlanePosGen(glm::fvec4{0.0f, -1.0f, 0.0f, 0.0f}, glm::fvec4{0.0f, 1.0f, 0.5f, 0.0f}));
  auto gaussTimeGenerator = std::make_shared<GaussTimeGen>(GaussTimeGen(2.f, 0.7f));
  auto fireColGen = std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{0.5f, 0.02f, 0.005f, 0.0f}, glm::fvec4{0.8f, 0.1f, 0.006f, 0.0f}, glm::fvec4{0.2f, 0.0f, 0.0f, 0.0f}, glm::fvec4{0.3f, 0.0f, 0.0f, 0.0f}));
  auto velTimeGen = std::make_shared<VelTimeGen>(VelTimeGen(0.8f, 0.4f, glm::fvec4{0.0, 2.0, 0.0, 0.0}));
  auto coneGen = std::make_shared<ConeVelGen>(ConeVelGen());

  emmit->addGenerator(posGenerator);
  //emmit->addGenerator(spherePosGenerator);
  //emmit->addGenerator(planePosGenerator);
  //emmit->addGenerator(rotPosGenerator);
  //emmit->addGenerator(colGenerator);
  emmit->addGenerator(fireColGen);
  emmit->addGenerator(sphereVelGenerator);
  //emmit->addGenerator(coneGen);
  //emmit->addGenerator(velGenerator);
  //emmit->addGenerator(timeGenerator);
  //emmit->addGenerator(gaussTimeGenerator);
  emmit->addGenerator(velTimeGen);

  m_particle_system->addEmitter(emmit);

  //m_particle_system->addUpdater(std::make_shared<GravityUpdater>());
  m_particle_system->addUpdater(std::make_shared<BasicAccUpdater>(glm::fvec4{0.0f, 6.f, 0.0f, 0.0f}));
  m_particle_system->addUpdater(std::make_shared<BasicVelUpdater>());
  m_particle_system->addUpdater(std::make_shared<NoiseVelocityUpdater>(0.6f, 2.0f));
  m_particle_system->addUpdater(std::make_shared<BasicPosUpdater>());
  m_particle_system->addUpdater(std::make_shared<BasicColorUpdater>());
  m_particle_system->addUpdater(std::make_shared<BasicTimeUpdater>());

  m_rend->generate(m_particle_system.get());
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

  createSys();

  texture_shader_ = new TextureShaders(400, "../resources/textures/explosion.png", 8);

  particle_shader_ = new ParticleShaders();

  createShaders();


  model_matrix_ = {
  1.0f, 0.0f, 0.0f, 0,
  0.0f, 1.0f, 0.0f, 0,
  0.0f, 0.0f, 1.0f, 3,
  0.0f, 0.0f, 0.0f, 1.0f
  };  
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

