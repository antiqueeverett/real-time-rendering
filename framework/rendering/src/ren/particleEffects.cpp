#define GLM_ENABLE_EXPERIMENTAL
/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/
#include <rtr/ren/particleEffects.hpp>
#include <rtr/ren/clothSystem.h>

void FlameEffect::init(size_t numParticles, Camera* cam) {
  m_cam.reset(cam);
  m_sys = std::make_shared<ParticleSystem>(numParticles, m_cam);
  //Generators
  m_posGen = std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{0.f, 0.0f, 0.f, 1.0f}));
  m_velGen = std::make_shared<SphereVelGen>(SphereVelGen(0.01f, 0.2f));
  m_colGen = std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{0.9f, 0.1f, 0.01f, 1.0f},
                                                           glm::fvec4{0.9f, 0.1f, 0.01f, 1.0f}));
  m_timeGen = std::make_shared<VelTimeGen>(VelTimeGen(0.8f, 0.4f, glm::fvec4{0.0, 3.0, 0.0, 0.0}));
  //create emiiter
  auto emmit = std::make_shared<ParticleEmitter>(10.f);
  emmit->addGenerator(m_posGen);
  emmit->addGenerator(m_velGen);
  emmit->addGenerator(m_colGen);
  emmit->addGenerator(m_timeGen);
  //Updaters
  m_accUp = std::make_shared<BasicAccUpdater>(glm::fvec4{0.0f, 12.f, 0.0f, 0.0f});
  m_velUp = std::make_shared<BasicVelUpdater>();
  m_noiseUp = std::make_shared<NoiseVelocityUpdater>(0.6f, 2.0f);
  m_posUp = std::make_shared<BasicPosUpdater>();
  m_colUp = std::make_shared<BasicColorUpdater>();
  m_timeUp = std::make_shared<BasicTimeUpdater>();
  m_transUp = std::make_shared<TranslationUpdater>();

  m_sys->addUpdater(m_accUp);
  m_sys->addUpdater(m_velUp);
  m_sys->addUpdater(m_noiseUp);
  m_sys->addUpdater(m_posUp);
  m_sys->addUpdater(m_transUp);
  m_sys->addUpdater(m_colUp);
  m_sys->addUpdater(m_timeUp);

  m_sys->addEmitter(emmit);

}



void FlameEffect::update(float dt) {
  return;
}

void FlameEffect::setPos(glm::fvec3 pos){
  //m_sys->reset();
  m_transUp->m_translate = glm::fvec4(pos, 0.0f);
  m_posGen->m_pos += glm::fvec4{pos, 0.0f};
}

void FlameEffect::setColor(glm::fvec3 col){
  m_colGen->m_min_start_col = glm::fvec4{col, 1.0f};
  m_colGen->m_max_start_col = glm::fvec4{col, 1.0f};
  m_colGen->m_min_end_col = glm::fvec4{col, 1.0f};
  m_colGen->m_max_end_col = glm::fvec4{col, 1.0f};

}



void FlameThrowerEffect::init(size_t numParticles, Camera* cam) {
  m_cam.reset(cam);
  m_sys = std::make_shared<ParticleSystem>(numParticles, m_cam);
  //Generators
  m_posGen = std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{0.f, 0.f, 0.f, 0.0f}));
  m_velGen = std::make_shared<ConeVelGen>(ConeVelGen(1.0f, 3.0f, glm::fvec3{0.0, -3.0, -1.0}, 0.032f));

  m_colGen = std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{0.5f, 0.02f, 0.005f, 0.0f},
                                                           glm::fvec4{0.8f, 0.1f, 0.006f, 0.0f},
                                                           glm::fvec4{0.2f, 0.0f, 0.0f, 0.0f},
                                                           glm::fvec4{0.3f, 0.0f, 0.0f, 0.0f}));
  m_timeGen = std::make_shared<GaussTimeGen>(GaussTimeGen(1.f, 1.5f));
  //create emiiter
  auto emmit = std::make_shared<ParticleEmitter>(100.f);
  emmit->addGenerator(m_posGen);
  //emmit->addGenerator(m_velGen);
  emmit->addGenerator(m_colGen);
  emmit->addGenerator(m_timeGen);
  //Updaters
  m_accUp = std::make_shared<BasicAccUpdater>(glm::fvec4{0.0f, 4.f, 0.0f, 0.0f});
  m_velUp = std::make_shared<BasicVelUpdater>();
  m_noiseUp = std::make_shared<NoiseVelocityUpdater>(0.6f, 2.0f);
  m_posUp = std::make_shared<BasicPosUpdater>();
  m_colUp = std::make_shared<BasicColorUpdater>();
  m_timeUp = std::make_shared<BasicTimeUpdater>();


  m_sys->addUpdater(m_accUp);
  m_sys->addUpdater(m_velUp);
  m_sys->addUpdater(m_noiseUp);
  m_sys->addUpdater(m_posUp);
  m_sys->addUpdater(m_colUp);
  m_sys->addUpdater(m_timeUp);

  m_sys->addEmitter(emmit);

}



void FlameThrowerEffect::update(float dt) {
  return;
}

void FlameThrowerEffect::setPos(glm::fvec3 pos){
  m_posGen->m_pos = glm::fvec4(pos, 1.0f);
}

void FlameThrowerEffect::setDir(glm::fvec3 dir){
  m_velGen->set_dir(dir);
}

void TrailEffect::init(size_t numParticles, Camera* cam) {
  m_cam.reset(cam);
  m_sys = std::make_shared<ParticleSystem>(numParticles, m_cam);
  //Generators
  m_posGen = std::make_shared<RotatingPosGen>(RotatingPosGen());
  m_posGen->m_pos = glm::fvec4{0.0f, 80.0f, 10.0f, 1.0f};
  m_posGen->m_min_r = 5.0f; m_posGen->m_max_r = 7.0f;
  m_velGen = std::make_shared<ConeVelGen>(ConeVelGen(1.0f, 3.0f, glm::fvec3{0.0, -3.0, -1.0}, 0.032f));

  m_colGen = std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{0.5f, 0.02f, 0.005f, 0.0f},
                                                           glm::fvec4{0.8f, 0.1f, 0.006f, 0.0f},
                                                           glm::fvec4{0.2f, 0.0f, 0.0f, 0.0f},
                                                           glm::fvec4{0.3f, 0.0f, 0.0f, 0.0f}));
  m_timeGen = std::make_shared<GaussTimeGen>(GaussTimeGen(1.f, 1.5f));
  //create emiiter
  auto emmit = std::make_shared<ParticleEmitter>(2.f);
  emmit->addGenerator(m_posGen);
  //emmit->addGenerator(m_velGen);
  emmit->addGenerator(m_colGen);
  emmit->addGenerator(m_timeGen);
  //Updaters
  m_accUp = std::make_shared<BasicAccUpdater>(glm::fvec4{0.0f, 4.f, 0.0f, 0.0f});
  m_velUp = std::make_shared<BasicVelUpdater>();
  m_noiseUp = std::make_shared<NoiseVelocityUpdater>(0.6f, 2.0f);
  m_posUp = std::make_shared<BasicPosUpdater>();
  m_colUp = std::make_shared<BasicColorUpdater>();
  m_timeUp = std::make_shared<BasicTimeUpdater>();

  //m_sys->addUpdater(m_accUp);
  m_sys->addUpdater(m_velUp);
  m_sys->addUpdater(m_noiseUp);
  m_sys->addUpdater(m_posUp);
  m_sys->addUpdater(m_colUp);
  m_sys->addUpdater(m_timeUp);

  m_sys->addEmitter(emmit);

}



void TrailEffect::update(float dt) {
  return;
}

void TrailEffect::setPos(glm::fvec3 pos){
  //m_posGen->m_pos = glm::fvec4(pos, 1.0f);
}

void TrailEffect::setDir(glm::fvec3 dir){
  //m_velGen->set_dir(dir);
}

void BlackHoleEffect::init(size_t numParticles, Camera* cam) {
  m_cam.reset(cam);
  m_sys = std::make_shared<ParticleSystem>(numParticles, m_cam);
  //Generators
  m_posGen.push_back(std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{4.f, 1.f, 0.f, 1.0f})));
  m_velGen.push_back(std::make_shared<ConeVelGen>(ConeVelGen(0.5f, 2.0f, glm::fvec3{0.0, -1.0, -1.0}, 0.032f)));

  m_posGen.push_back(std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{-4.f, 1.f, 0.f, 1.0f})));
  m_velGen.push_back(std::make_shared<ConeVelGen>(ConeVelGen(0.5f, 2.0f, glm::fvec3{0.0, 1.0, -1.0}, 0.032f)));


  m_colGen.push_back(std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{1.0f, 0.0f, 0.0f, 1.0f}, glm::fvec4{1.f, 0.0f, 0.0f, 1.0f})));
  m_colGen.push_back(std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{0.0f, 1.0f, 0.0f, 1.0f}, glm::fvec4{0.0f, 1.f, 0.0f, 1.0f})));

  m_posGen.push_back(std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{0.f, 0.f, 0.f, 1.0f})));

  std::shared_ptr<OrbitVelGen> orbit = std::make_shared<OrbitVelGen>(OrbitVelGen());
  std::shared_ptr<SpherePosGen> sphere = std::make_shared<SpherePosGen>(SpherePosGen(10.0f));

  m_timeGen = std::make_shared<BasicTimeGen>(BasicTimeGen(20.f, 20.f));
  //create emiiter

  auto emmit = std::make_shared<ParticleEmitter>(10.f);
  emmit->addGenerator(m_posGen[0]);
  emmit->addGenerator(m_velGen[0]);
  emmit->addGenerator(m_colGen[0]);
  emmit->addGenerator(m_timeGen);

  auto emmit2 = std::make_shared<ParticleEmitter>(10.f);
  emmit2->addGenerator(m_posGen[1]);
  emmit2->addGenerator(m_velGen[1]);
  emmit2->addGenerator(m_colGen[1]);
  emmit2->addGenerator(m_timeGen);


  auto emmit3 = std::make_shared<ParticleEmitter>(numParticles);
  emmit3->addGenerator(sphere);
  emmit3->addGenerator(m_colGen[0]);
  emmit3->addGenerator(orbit);
  emmit3->addGenerator(m_timeGen);

  //Updaters
  m_velUp = std::make_shared<BasicVelUpdater>();
  m_posUp = std::make_shared<BasicPosUpdater>();
  m_remUp = std::make_shared<PositionRemover>();
  m_timeUp = std::make_shared<BasicTimeUpdater>();
  m_colUp = std::make_shared<VelColorUpdater>(glm::fvec4{-2.0f, -2.0f, -2.0f, 2.0f}, glm::fvec4{2.0f});
  m_attUp = std::make_shared<AttractorUpdater>();

  m_attUp->m_attractors.push_back(glm::fvec4{0.0f, 0.0f, 0.0f, 2.f});


  m_sys->addUpdater(m_attUp);
  m_sys->addUpdater(m_velUp);
  m_sys->addUpdater(m_posUp);
  m_sys->addUpdater(m_remUp);
  //m_sys->addUpdater(m_timeUp);
  m_sys->addUpdater(m_colUp);


  m_sys->addEmitter(emmit2);
  m_sys->addEmitter(emmit);
  //m_sys->addEmitter(emmit3);

}



void BlackHoleEffect::update(float dt) {
  return;
}

void FireRing::init(size_t numParticles, Camera* cam) {
  m_cam.reset(cam);
  glm::fvec3 pos;
  int numParticlesPerFlame = numParticles / 36;
  for (int i = 0; i < 36; i ++){
    m_flames.push_back(std::make_shared<FlameEffect>(FlameEffect()));
    m_flames.at(i)->init(numParticlesPerFlame, cam);
    pos = 8.f * glm::fvec3{glm::cos((i * M_PI)/18), glm::sin((i * M_PI)/18), 0.0f};
    m_flames.at(i)->setPos(pos);
  }

}

void FireRing::move(glm::fvec3 mov){
  m_pos += mov;
  glm::fvec3 pos;
  for (int i = 0; i < 36; i ++){
    m_flames.at(i)->setPos(mov);
  }
}


void FireRing::update(float dt) {
  return;
}

void FireRing::initRenderer(){
  for(auto i : m_flames){
    i->initRenderer();
  }
}

void FireRing::reset(){
  for(auto i : m_flames){
    i->reset();
  }
}
void FireRing::clean(){
  for(auto i : m_flames){
    i->clean();
  }
}
void FireRing::toggleUpdate(){
  for(auto i : m_flames){
    i->toggleUpdate();
  }
}
void FireRing::toggleEmit(){
  for(auto i : m_flames){
    i->toggleEmit();
  }
}

void FireRing::cpuUpdate(float dt){
  for(auto i : m_flames){
    i->cpuUpdate(dt);
  }
}
void FireRing::gpuUpdate(){
  for(auto i : m_flames){
    i->gpuUpdate();
  }
}
void FireRing::render(){
  for(auto i : m_flames){
    i->render();
  }
}

void FireRing::setColor(glm::fvec3 col){
  for(auto i : m_flames){
    i->setColor(col);
  }
}

void ClothEffect::reset() {
  m_sys.reset(new ClothSystem(m_gridW * m_gridH, m_cam));
  m_rend.reset(new ParticleRenderer());
  m_fixed.clear();

  m_posGen->m_pos = m_gridPos;
  m_posGen->m_rot = m_gridRot;
  m_posGen->m_w = m_gridW;
  m_posGen->m_h = m_gridH;
  m_posGen->m_d = m_gridD;

  m_structUp.reset(new SpringUpdater(m_sys->finalData()->m_struct_con));
  m_shearUp.reset(new SpringUpdater(m_sys->finalData()->m_shear_con));
  m_bendUp.reset(new SpringUpdater(m_sys->finalData()->m_bend_con));

  m_posUp->m_damp = m_damp;
  m_structUp->m_k = m_kStruct;
  m_shearUp->m_k = m_kShear;
  m_bendUp->m_k = m_kBend;
  m_stretchUp->m_min = m_minStretch;
  m_stretchUp->m_max = m_maxStretch;

  m_sys->addUpdater(m_colUp);
  if(m_structure) m_sys->addUpdater(m_structUp);
  if(m_shear) m_sys->addUpdater(m_shearUp);
  if(m_bend) m_sys->addUpdater(m_bendUp);
  if(m_gravity) m_sys->addUpdater(m_gravUp);
  m_sys->addUpdater(m_posUp);
  if(m_stretch) m_sys->addUpdater(m_stretchUp);
  m_sys->addUpdater(m_normUp);

  auto emmit = std::make_shared<ParticleEmitter>(m_gridW * m_gridH);
  emmit->addGenerator(m_posGen);
  emmit->addGenerator(m_colGen);
  emmit->addGenerator(m_prevGen);
  emmit->addGenerator(m_springGen);
  m_sys->addEmitter(emmit);
  m_sys->toggleSort();

  //create mesh faces
  auto ind = m_sys->finalData()->m_indices;
  for(int h = 0; h < m_gridH - 1; ++h){
    for(int w = 0; w < m_gridW - 1; ++w) {
      unsigned int top_left = w % m_gridW + h * m_gridW;
      unsigned int top_right = top_left + 1;
      unsigned int bot_left = top_left + m_gridW;
      unsigned int bot_right = bot_left + 1;
      ind->push_back(top_left);
      ind->push_back(bot_right);
      ind->push_back(top_right);
      ind->push_back(top_left);
      ind->push_back(bot_left);
      ind->push_back(bot_right);
    }
  }

  //create springs
  auto struc = m_sys->finalData()->m_struct_con;
  for(int h = 0; h < m_gridH; ++h){
    for(int w = 0; w < m_gridW; ++w) {
      if(w < m_gridW - 1) {
        unsigned int left = w % m_gridW + h * m_gridW;
        unsigned int right = left + 1;
        struc->push_back(glm::fvec3(left, right, 0));
      }

      if(h < m_gridH - 1) {
        unsigned int top = w % m_gridW + h * m_gridW;
        unsigned int bottom = top + m_gridW;
        struc->push_back(glm::fvec3(top, bottom, 0));
      }
    }
  }

  auto shear = m_sys->finalData()->m_shear_con;
  for(int h = 0; h < m_gridH - 1; ++h){
    for(int w = 0; w < m_gridW - 1; ++w) {
      unsigned int top_left = w % m_gridW + h * m_gridW;
      unsigned int top_right = top_left + 1;
      unsigned int bot_left = top_left + m_gridW;
      unsigned int bot_right = bot_left + 1;
      shear->push_back(glm::fvec3(top_left, bot_right, 0));
      shear->push_back(glm::fvec3(top_right, bot_left, 0));

    }
  }

  auto bend = m_sys->finalData()->m_bend_con;
  for(int h = 0; h < m_gridH; ++h) {
    for (int w = 0; w < m_gridW; ++w) {
      if (w < m_gridW - 2) {
        unsigned int left = w % m_gridW + h * m_gridW;
        unsigned int right = left + 2;
        bend->push_back(glm::fvec3(left, right, 0));
      }
      if (h < m_gridH - 2) {
        unsigned int top = w % m_gridW + h * m_gridW;
        unsigned int bottom = top + 2 * m_gridW;
        bend->push_back(glm::fvec3(top, bottom, 0));
      }
    }
  }

  emit();

  auto uv = m_sys->finalData()->m_uv.get();
  for(int h = 0; h < m_gridH; ++h) {
    for (int w = 0; w < m_gridW; ++w) {
      float u = w / (float)m_gridW;
      float v = -(h / (float)m_gridH) + 1;

      uv[h * m_gridW + w] = glm::vec4(u, v, 0, 0);
    }
  }

  initRenderer();


}

void ClothEffect::init(size_t numParticles, Camera *cam) {
  m_cam.reset(cam);

  m_colGen = std::make_shared<BasicColorGen>();
  m_colGen->m_min_end_col = glm::fvec4{1.0f};
  m_colGen->m_max_end_col = glm::fvec4{1.0f};

  m_posGen = std::make_shared<GridPosGen>();
  m_prevGen = std::make_shared<PrevPosGen>();
  m_springGen = std::make_shared<SpringGen>();

  m_colUp = std::make_shared<BasicColorUpdater>();
  m_gravUp = std::make_shared<GravityUpdater>();
  m_posUp = std::make_shared<VerletPosUpdater>();
  m_stretchUp = std::make_shared<StretchUpdater>();
  m_normUp = std::make_shared<NormalUpdater>();

}

void ClothEffect::update(float dt) {
  auto pos = m_sys->finalData()->m_pos.get();
  for(unsigned int i = 0; i < m_sys->getCount(); ++i){
    if(m_fixed.count(i)){
      pos[i].w = 1.f;
    } else {
      pos[i].w = 0.f;
    }
  }
}

int ClothEffect::findClosest(glm::vec3 ray){
  auto pos = m_sys->finalData()->m_pos.get();
  float l = glm::length(ray);
  float dist = FLT_MAX, temp = 0;
  unsigned int result;
  for(unsigned int i = 0; i < m_sys->getCount(); ++i){
    temp = glm::length(glm::cross(ray, m_cam->getPosition() - glm::vec3(pos[i]))) / l;
    if(temp < dist){
      dist = temp;
      result = i;
    }
  }

  return result;
}
