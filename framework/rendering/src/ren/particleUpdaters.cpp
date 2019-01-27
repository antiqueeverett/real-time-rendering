/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/

#include <rtr/ren/particleUpdaters.hpp>


/******************************************************************************
void update(float dt, ParticleData *p){
	size_t end_id = p->m_count_alive;
	for (size_t i = 0; i < end_id; ++i) {
		//do stuff
	}
}
******************************************************************************/

void GravityUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  glm::fvec4* acc = p->m_acc.get();

  for (size_t i = 0; i < end_id; ++i) {
    acc[i] += m_grav;
  }
}

void BasicAccUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  auto acc = p->m_acc.get();
  auto mass = p->m_mass.get();

  for (size_t i = 0; i < end_id; ++i) {
    acc[i] += m_acceleration / mass[i];
  }
}

void NoiseVelocityUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;
  float dist;
  glm::fvec2 dir;

  glm::fvec4* vel = p->m_vel.get();
  m_frequency += dt;

  for (size_t i = 0; i < end_id; ++i) {
    dir = glm::circularRand(1.0f);
    dist = m_amplitude * sin(m_frequency);

    vel[i].x += dir.x * dist * dt;
    vel[i].z += dir.y * dist * dt;

  }
}

void AttractorUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  glm::fvec4* acc = p->m_acc.get();
  glm::fvec4* pos = p->m_pos.get();

  float dist;
  glm::fvec4 dir;

  for (size_t i = 0; i < end_id; ++i) {
    for(auto const& att : m_attractors){
      dir = att - pos[i];
      dist = glm::length(dir);

      acc[i] += dir * att.w / (dist*dist);
    }
  }
}

void BasicVelUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  glm::fvec4* acc = p->m_acc.get();
  glm::fvec4* vel = p->m_vel.get();

  for (size_t i = 0; i < end_id; ++i) {
    vel[i] += acc[i] * dt;
  }
}

void BasicPosUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  glm::fvec4* pos = p->m_pos.get();
  glm::fvec4* vel = p->m_vel.get();

  for (size_t i = 0; i < end_id; ++i) {
    pos[i] += vel[i] * dt;
  }
}

void VerletPosUpdater::update(float dt, ParticleData *p) {
  size_t end_id = p->m_count_alive;

  glm::fvec4* pos = p->m_pos.get();
  glm::fvec4* prev_pos = p->m_pos_prev.get();
  glm::fvec4* acc = p->m_acc.get();
  glm::fvec4 temp;

  for (size_t i = 0; i < end_id; ++i) {
    if(pos[i].w == 0.f) {
      temp = pos[i];
      pos[i] += (pos[i] - prev_pos[i]) * (1.f - m_damp) + acc[i] * dt * dt;
      prev_pos[i] = temp;
      pos[i].w = temp.w;
    }
  }
}

void TranslationUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  glm::fvec4* pos = p->m_pos.get();

  for (size_t i = 0; i < end_id; ++i) {
    pos[i] += m_translate;
  }

  m_translate = glm::fvec4{0.0f};
}

void PositionRemover::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  glm::fvec4* pos = p->m_pos.get();

  for (size_t i = 0; i < end_id; ++i) {
    if (glm::length(glm::fvec3{pos[i]})<m_dist){
      p->kill(i);
    }
  }
}

void CubeRemover::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  glm::fvec4* pos = p->m_pos.get();

  for (size_t i = 0; i < end_id; ++i) {
    if (pos[i].x < m_min.x || pos[i].x > m_max.x){
      p->kill(i);
    } else if (pos[i].y < m_min.y || pos[i].y > m_max.y){
      p->kill(i);
    } else if (pos[i].z < m_min.z || pos[i].z > m_max.z){
      p->kill(i);
    }
  }
}

void BasicColorUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  glm::fvec4* col = p->m_col.get();
  glm::fvec4* start_col = p->m_start_col.get();
  glm::fvec4* end_col = p->m_end_col.get();
  glm::fvec4* time = p->m_time.get();

  for (size_t i = 0; i < end_id; ++i) {
    col[i] = glm::mix(start_col[i], end_col[i], time[i].z);
  }
}

void VelColorUpdater::update(float dt, ParticleData *p) {
  size_t end_id = p->m_count_alive;
  glm::fvec4* col = p->m_col.get();
  glm::fvec4* start_col = p->m_start_col.get();
  glm::fvec4* end_col = p->m_end_col.get();
  glm::fvec4* t = p->m_time.get();
  glm::fvec4* vel = p->m_vel.get();

  float scaler, scaleg, scaleb;
  float diffr = m_maxVel.x - m_minVel.x;
  float diffg = m_maxVel.y - m_minVel.y;
  float diffb = m_maxVel.z - m_minVel.z;
  for (size_t i = 0; i < end_id; ++i) {
    scaler = (vel[i].x - m_minVel.x) / diffr;
    scaleg = (vel[i].y - m_minVel.y) / diffg;
    scaleb = (vel[i].z - m_minVel.z) / diffb;
    col[i].r = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
    col[i].g = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
    col[i].b = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
    col[i].a = glm::mix(start_col[i].a, end_col[i].a, t[i].z);
  }
}

void BasicTimeUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  glm::fvec4* time = p->m_time.get();

  for (size_t i = 0; i < end_id; ++i) {
    time[i].x -= dt;
    time[i].z = 1.0f - (time[i].x * time[i].w);

    if(time[i].x < 0.0f){
      p->kill(i);
      end_id = p->m_count_alive < p->m_count ? p->m_count_alive : p->m_count;
    }
  }
}

void SpringUpdater::update(float dt, ParticleData *p) {

  auto pos = p->m_pos.get();
  auto acc = p->m_acc.get();
  auto mass = p->m_mass.get();

  glm::fvec3 dist, f;

  for(auto& s : *m_spring){
    dist = glm::fvec3(pos[(int)s.x] - pos[(int)s.y]);

    f = m_k * glm::normalize(dist) * (glm::length(dist) - s.z);
    acc[(int)s.x] -= glm::fvec4(f, 0.f) / mass[(int)s.x];
    acc[(int)s.y] += glm::fvec4(f, 0.f) / mass[(int)s.y];
  }
}

void StretchUpdater::update(float dt, ParticleData *p) {
  glm::fvec4* pos = p->m_pos.get();
  auto struct_con = p->m_struct_con;
  auto shear_con = p->m_shear_con;
  auto bend_con = p->m_bend_con;

  for(int i = 0; i < m_iter; ++i){
    for(auto& s : *struct_con){
      maxStretch(s.z, pos[(int)s.x], pos[(int)s.y]);
    }
    for(auto& s : *shear_con){
      maxStretch(s.z, pos[(int)s.x], pos[(int)s.y]);
    }
    for(auto& s : *bend_con){
      maxStretch(s.z, pos[(int)s.x], pos[(int)s.y]);
    }
  }
}

void StretchUpdater::maxStretch(float rest, glm::fvec4& pos_1, glm::fvec4& pos_2) {
  glm::fvec3 dist = glm::fvec3(pos_1 - pos_2);
  glm::fvec3 dir = glm::normalize(dist);
  float len = glm::length(dist);
  //satisfy max distance constraint
  if ((len > rest * m_max) || (len < rest * m_min)) {
    glm::fvec3 f = dist * (1 - (rest * m_max) / len);

    glm::fvec3 corr = f * (1 - pos_1.w) * 0.5f * (1 + pos_2.w);
    pos_1 -= glm::fvec4(corr, 0.f);

    corr = f * (1 - pos_2.w) * 0.5f * (1 + pos_1.w);
    pos_2 += glm::fvec4(corr, 0.f);
  }
}

void NormalUpdater::update(float dt, ParticleData *p) {
  size_t end_id = p->m_count_alive;

  auto normal = p->m_normal.get();
  auto pos = p->m_pos.get();
  auto ind = p->m_indices;

  unsigned int a, b, c;
  glm::vec3 x, y, z, u, v, n;

  for(int i = 0; i < ind->size() / 3; ++i){
    a = ind->at(3 * i);
    b = ind->at(3 * i + 1);
    c = ind->at(3 * i + 2);

    x = glm::vec3(pos[a]);
    y = glm::vec3(pos[b]);
    z = glm::vec3(pos[c]);

    u = x - z; v = y - z;
    n = glm::normalize(glm::cross(u, v));

    normal[a] += glm::vec4(n, 0.f);
    normal[b] += glm::vec4(n, 0.f);
    normal[c] += glm::vec4(n, 0.f);

  }

  for (size_t i = 0; i < end_id; ++i) {
    normal[i] = glm::normalize(normal[i]);
  }

}

void SphereCollisionUpdater::update(float dt, ParticleData *p) {
  size_t end_id = p->m_count_alive;

  auto pos = p->m_pos.get();
  auto prev = p->m_pos_prev.get();
  glm::vec3 dist, mov, hit, norm, newpos, betw;

  for (size_t i = 0; i < end_id; ++i) {
    dist = glm::vec3(pos[i]) - m_pos;
    if(glm::length(dist) < m_r){
      mov = glm::normalize(dist) * m_r + m_pos;
      glm::intersectLineSphere(glm::vec3(pos[i]), glm::vec3(prev[i]), m_pos, m_r, hit, norm, newpos, betw);
      betw = hit - mov;
      newpos = mov + m_f * betw;
      newpos = glm::normalize(newpos - m_pos) * m_r + m_pos;
      pos[i].x = newpos.x;
      pos[i].y = newpos.y;
      pos[i].z = newpos.z;
    }
  }
}

void ClothCollisionUpdater::update(float dt, ParticleData *p){
  size_t end_id = p->m_count_alive;

  auto pos = p->m_pos.get();
  auto prev = p->m_pos_prev.get();
  glm::fvec3 dist, corr;
  float err;
  for(int k = 0; k < m_it; ++k) {
    for (size_t i = 0; i < end_id; ++i) {
      for (size_t j = i + 1; j < end_id; ++j) {
        if (m_collide[i][j]) {
          dist = glm::vec3(pos[i] - pos[j]);
          if(glm::length(dist) < m_dist){
            err = m_dist - glm::length(dist);

            corr = err * (1 - pos[i].w) * 0.5f * (1 + pos[j].w) * glm::normalize(dist);
            pos[i] += glm::vec4(corr, 0.f);
            corr = err * (1 - pos[j].w) * 0.5f * (1 + pos[i].w) * glm::normalize(dist);
            pos[j] -= glm::vec4(corr, 0.f);

          }

        }
      }
    }
  }
}


void ClothCollisionUpdater::init(ParticleData *p) {
  size_t count = p->m_count;
  m_collide.clear();

  for(int i = 0; i < count; ++i){
    std::vector<bool> vec;
    for(int j = 0; j < count; ++j){
      if(i == j) { vec.push_back(false); }
      else { vec.push_back(true); }
    }
    m_collide.push_back(vec);
  }

  auto struc = p->m_struct_con;
  for(auto const& s : *struc){
    m_collide[(int)s.x][(int)s.y] = false;
    m_collide[(int)s.y][(int)s.x] = false;
  }
  auto shear = p->m_shear_con;
  for(auto const& s : *shear){
    m_collide[(int)s.x][(int)s.y] = false;
    m_collide[(int)s.y][(int)s.x] = false;
  }
  auto bend = p->m_bend_con;
  for(auto const& s : *bend){
    m_collide[(int)s.x][(int)s.y] = false;
    m_collide[(int)s.y][(int)s.x] = false;
  }
}

void WindForceUpdater::update(float dt, ParticleData *p) {
  auto tris = *p->m_indices.get();
  auto mass = p->m_mass.get();
  auto acc = p->m_acc.get();
  auto norm = p->m_normal.get();

  unsigned int x, y, z;
  glm::vec3 normal, f, w;

  w.x = m_wind.x * glm::linearRand(0.9f, 1.f);
  w.y = m_wind.y * glm::linearRand(0.9f, 1.f);
  w.z = m_wind.z * glm::linearRand(0.9f, 1.f);

  for(int i = 0; i < tris.size() / 3; ++i){
    x = tris[3 * i]; y = tris[3 * i + 1]; z = tris[3 * i + 2];

    normal = glm::normalize(glm::vec3(norm[x] + norm[y] + norm[z]));
    f = normal * glm::dot(w, normal);

    acc[x] += glm::vec4(f / mass[x], 0);
    acc[y] += glm::vec4(f / mass[y], 0);
    acc[z] += glm::vec4(f / mass[z], 0);
  }
}

/*
void SphereFrictionUpdater::update(float dt, ParticleData *p) {
  //if(m_friction <= 0.1) return;
  size_t end_id = p->m_count_alive;
  std::cout << "FRICTION" << std::endl;
  auto pos = p->m_pos.get();
  auto acc = p->m_acc.get();
  glm::vec3 dir, dist, nacc;
  for (size_t i = 0; i < end_id; ++i) {
    glm::vec3 a_bef = glm::vec3(acc[i]);
    dist = glm::vec3(pos[i]) - m_pos;
    if(glm::length(dist) <= r){
      nacc = glm::normalize(glm::vec3(acc[i]));
      dir = glm::cross(dist, nacc);
      if(glm::length(dir) == 0.f) continue;
      dir = glm::normalize(dir) * -m_friction * glm::dot(glm::vec3(acc[i]), dir);
      std::cout << i << ":" << acc[i].x << "," << acc[i].y << "," << acc[i].z << ";" << a_bef.x << "," << a_bef.y << "," << a_bef.z << ";" << dist.x << "," << dist.y << "," << dist.z << std::endl;

      acc[i].x += dir.x;
      acc[i].y += dir.y;
      acc[i].z += dir.z;
    }

  }
}
 */
