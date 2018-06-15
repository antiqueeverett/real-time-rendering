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
		acc[i] += m_grav * dt;
	}
}

void BasicAccUpdater::update(float dt, ParticleData *p){
	size_t end_id = p->m_count_alive;

	glm::fvec4* acc = p->m_acc.get();

	for (size_t i = 0; i < end_id; ++i) {
		acc[i] += m_acceleration * dt;
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