/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/

#include <rtr/ren/particleGenerators.hpp>

/******************************************************************************
void generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	for (size_t i = start_id; i <= end_id; ++i) {
		//do stuff
	}
}
******************************************************************************/

void PointPosGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	for (size_t i = start_id; i <= end_id; ++i) {
		p->m_pos[i] = m_pos;
	}
}

void PlanePosGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	glm::fvec3 h_vec, dir;
	for (size_t i = start_id; i <= end_id; ++i) {
		h_vec = glm::ballRand(m_rad);
		dir = glm::cross(h_vec, glm::fvec3{m_norm.x, m_norm.y, m_norm.z});

		p->m_pos[i].x = m_pos.x + dir.x;
		p->m_pos[i].y = m_pos.y + dir.y;
		p->m_pos[i].z = m_pos.z + dir.z;
	}
}

void BoxPosGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	glm::fvec4 min_pos = m_pos - glm::fvec4{m_offset};
	glm::fvec4 max_pos = m_pos + glm::fvec4{m_offset};

	for (size_t i = start_id; i <= end_id; ++i) {
		p->m_pos[i] = glm::linearRand(min_pos, max_pos);

	}	
}

void SpherePosGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	//float phi, theta, rad, r;
	glm::fvec3 dir;
	float rad;
	for (size_t i = start_id; i <= end_id; ++i) {
		rad = glm::linearRand(m_min_r, m_max_r);
		dir = glm::ballRand(rad);

		p->m_pos[i].x = m_pos.x + dir.x;
		p->m_pos[i].y = m_pos.y + dir.y;
		p->m_pos[i].z = m_pos.z + dir.z;
		
	}
}

void RotatingPosGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	float angle = m_ticks * M_PI;
	m_ticks += dt * 10;
	for (size_t i = start_id; i <= end_id; ++i) {
		float r = glm::linearRand(m_min_r, m_max_r);

		p->m_pos[i].x = m_pos.x + r * cos(angle);
		p->m_pos[i].y = m_pos.y + r * sin(angle);
		p->m_pos[i].z = m_pos.z;
		
	}
}

void GridPosGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id) {
	glm::fvec4 new_pos;
	new_pos.w = 1;

    auto pos = p->m_pos.get();
    auto ind = p->m_indices;
	for(unsigned int i = start_id; i <= end_id; ++i){
		if(i / m_h > m_w) break;
		new_pos.x = (i % m_w) * m_d;
        new_pos.y = 0;
        new_pos.z = (i / m_w) * m_d;

		new_pos = m_rot * new_pos;

		pos[i].x = new_pos.x + m_pos.x;
		pos[i].y = new_pos.y + m_pos.y;
		pos[i].z = new_pos.z + m_pos.z;


	}
}

void PrevPosGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id) {
  for (size_t i = start_id; i <= end_id; ++i) {
    p->m_pos_prev[i] = p->m_pos[i];
  }
}

void BasicColorGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	for (size_t i = start_id; i <= end_id; ++i){
		p->m_start_col[i] = glm::linearRand(m_min_start_col, m_max_start_col);
		p->m_end_col[i] = glm::linearRand(m_min_end_col, m_max_end_col);
	}
}

void BasicVelGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	for (size_t i = start_id; i <= end_id; ++i) {
		p->m_vel[i] = glm::linearRand(m_min_vel, m_max_vel);
	}
}

void SphereVelGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	float phi, theta, v, r;
	glm::fvec3 dir;
	for (size_t i = start_id; i <= end_id; ++i) {
		//phi = glm::linearRand(-M_PI, M_PI);
		//theta = glm::linearRand(-M_PI, M_PI);
		v = glm::linearRand(m_min_vel, m_max_vel);
		dir = glm::sphericalRand(v);

		/*r = v * sin(phi);
		p->m_vel[i].x = r * cos(theta);
		p->m_vel[i].y = r * sin(theta);
		p->m_vel[i].z = v * cos(phi);*/

		p->m_vel[i].x = dir.x;
		p->m_vel[i].y = dir.y;
		p->m_vel[i].z = dir.z;
		
	}
}


void OrbitVelGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	glm::fvec3 dir, vel, down = glm::fvec3{0.0f, -1.0f, 0.0f};
	for (size_t i = start_id; i <= end_id; ++i) {
		dir = glm::fvec3{p->m_pos[i] - m_center};
		vel = glm::normalize(glm::cross(dir, down)) * m_vel;

		p->m_vel[i].x = vel.x;
		p->m_vel[i].y = vel.y;
		p->m_vel[i].z = vel.z;
		
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////77777
void ConeVelGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	float v; 
	glm::fvec2 r;
	glm::fvec3 vel;
	for (size_t i = start_id; i <= end_id; ++i) {
		r = glm::diskRand(m_rad);
		v = glm::linearRand(m_min_vel, m_max_vel);
		vel = rotate(glm::fvec3(r.x, 1.0, r.y) * v);

		p->m_vel[i].x = vel.x;
		p->m_vel[i].y = vel.y;
		p->m_vel[i].z = vel.z;
	}
}

glm::fvec3 ConeVelGen::rotate(glm::fvec3 vec){
	if(m_dir == glm::fvec3{0.0, 1.0, 0.0}){
		return vec;
	} else if(m_dir == glm::fvec3{0.0, -1.0, 0.0}){
		return -vec;
	} else {
		return glm::fvec3(m_rot_mat * glm::fvec4(vec, 1.0));
	}
}

void ConeVelGen::calc_rot_mat(){
	glm::fvec3 rot_axis = glm::cross(m_dir, glm::fvec3{0.0, 1.0, 0.0});
	float rot_angle = glm::acos(glm::dot(m_dir, glm::fvec3{0.0, 1.0, 0.0})/glm::length(m_dir));
	m_rot_mat = glm::rotate(glm::fmat4{1.0}, rot_angle, -rot_axis);
}

void ConeVelGen::set_dir(glm::fvec3 dir) {
	m_dir = glm::normalize(dir);
	calc_rot_mat();
}


//////////////////////////////////////////////////////////////////////////////////////////////77777

void BasicTimeGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	for (size_t i = start_id; i <= end_id; ++i) {
		p->m_time[i].x = p->m_time[i].y = glm::linearRand(m_min_t, m_max_t);
		p->m_time[i].z = (float)0.0;
		p->m_time[i].w = (float)1.0 / p->m_time[i].x;
	}
}

void GaussTimeGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	for (size_t i = start_id; i <= end_id; ++i) {
		p->m_time[i].x = p->m_time[i].y = glm::gaussRand(m_mean, m_deviation);
		p->m_time[i].z = (float)0.0;
		p->m_time[i].w = (float)1.0 / p->m_time[i].x;
	}
}

void VelTimeGen::generate(float dt, ParticleData *p, size_t start_id, size_t end_id){
	float s;
	for (size_t i = start_id; i <= end_id; ++i) {
		s = glm::dot(m_velocity, glm::normalize(p->m_vel[i]));

		p->m_time[i].x = p->m_time[i].y = glm::gaussRand(s * m_mean,  m_deviation);
		p->m_time[i].z = (float)0.0;
		p->m_time[i].w = (float)1.0 / p->m_time[i].x;
	}
}

