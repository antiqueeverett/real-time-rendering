/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/
#define GLM_ENABLE_EXPERIMENTAL
#include <rtr/ren/particleData.hpp>

////////////////////////////////////////////////////////////////////////////////

void ParticleData::generate(size_t maxSize){
	m_count = maxSize;
	m_count_alive = 0;
	m_pos.reset(new glm::vec4[maxSize]);
	m_pos_prev.reset(new glm::vec4[maxSize]);
	m_col.reset(new glm::vec4[maxSize]);
	m_start_col.reset(new glm::vec4[maxSize]);
	m_end_col.reset(new glm::vec4[maxSize]);
	m_vel.reset(new glm::vec4[maxSize]);
	m_acc.reset(new glm::vec4[maxSize]);
	m_time.reset(new glm::vec4[maxSize]);
	m_alive.reset(new bool[maxSize]);
}

void ParticleData::kill(size_t id){
	//m_alive[id] = false;
	m_alive[m_count_alive - 1] = false;
	swapData(id, m_count_alive - 1);
	m_count_alive --;
}

void ParticleData::wake(size_t id){
	//only called on particles behind last alive particles, no swap necessary
	m_alive[id] = true;
	m_pos[id] = glm::vec4(0.0f);
	m_pos_prev[id] = glm::vec4(0.0f);
	m_col[id] = glm::vec4(0.0f);
	m_start_col[id] = glm::vec4(0.0f);
	m_end_col[id] = glm::vec4(0.0f);
	m_vel[id] = glm::vec4(0.0f);
	m_acc[id] = glm::vec4(0.0f);
	m_time[id] = glm::vec4(0.0f);
	m_count_alive ++;
}

void ParticleData::swapData(size_t a, size_t b){
	// dead particle's data isn't used anymore, no need to copy
	m_pos[a] = m_pos[b];
	m_pos_prev[a] = m_pos_prev[b];
	m_col[a] = m_col[b];
	m_start_col[a] = m_start_col[b];
	m_end_col[a] = m_end_col[b];
	m_vel[a] = m_vel[b];
	m_acc[a] = m_acc[b];
	m_time[a] = m_time[b];
}

////////////////////////////////////////////////////////////////////////////////
