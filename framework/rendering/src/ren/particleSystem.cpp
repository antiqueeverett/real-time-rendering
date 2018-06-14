/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/

#include <rtr/ren/particleSystem.hpp>

ParticleSystem::ParticleSystem(size_t maxCount){
	m_count = maxCount;
	m_particles.generate(maxCount);
	for (size_t i = 0; i < maxCount; ++i){
		m_particles.m_alive[i] = false;
	}
	
}

void ParticleSystem::update(float dt){
	if(do_update) {
		if(do_emit) {
			for (auto& em: m_emitters){
					em->emit(dt, &m_particles);
				}
		}

		if(getAliveCount() > 0){
			for (size_t i = 0; i < m_count; ++i){
				m_particles.m_acc[i] = glm::vec4(0.0f);
			}
	

			for(auto& up : m_updaters) {
				up->update(dt, &m_particles);
			}

			if(do_sort) {sort();}
		}
	}
	
}

void ParticleSystem::reset(){
	m_particles.m_count_alive = 0;
}

void ParticleSystem::sort(){
	size_t alive = getAliveCount();
	size_t end_id = alive - 1;
	//std::cout << alive << "[System::sort()]"  << std::endl;
	
	if(alive > 0){
		size_t index[m_count];
		for(size_t i = 0; i < m_count; ++i){ index[i] = i; }
	
		std::sort(index, index + end_id, [&](size_t const& a, size_t const& b) -> bool {
			return m_particles.m_pos[a].z < m_particles.m_pos[b].z;});

		//sortPos(index);
		sortCol(index);
		sortStartCol(index);
		sortEndCol(index);
		//sortVel(index);
		//sortAcc(index);
		//sortTime(index);
	
		std::unique_ptr<glm::vec4[]> new_pos = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);
		std::unique_ptr<glm::vec4[]> new_vel = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);
		std::unique_ptr<glm::vec4[]> new_time = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);

		for(size_t i = 0; i < m_count; ++i){
			new_pos[i] = m_particles.m_pos[index[i]];
			new_vel[i] = m_particles.m_vel[index[i]];
			new_time[i] = m_particles.m_time[index[i]];
			
		}

		m_particles.m_pos = std::move(new_pos);
		m_particles.m_vel = std::move(new_vel);
		m_particles.m_time = std::move(new_time);

	}


}

void ParticleSystem::sortPos(const size_t index[]){
	std::unique_ptr<glm::vec4[]> new_pos = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);

	for(size_t i = 0; i < m_count; ++i){
		new_pos[i] = m_particles.m_pos[index[i]];
	}

	m_particles.m_pos = std::move(new_pos);
}

void ParticleSystem::sortCol(const size_t index[]){
	std::unique_ptr<glm::vec4[]> new_col = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);

	for(size_t i = 0; i < m_count; ++i){
		new_col[i] = m_particles.m_col[index[i]];
	}

	m_particles.m_col = std::move(new_col);
}

void ParticleSystem::sortStartCol(const size_t index[]){
	std::unique_ptr<glm::vec4[]> new_start_col = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);

	for(size_t i = 0; i < m_count; ++i){
		new_start_col[i] = m_particles.m_start_col[index[i]];
	}

	m_particles.m_start_col = std::move(new_start_col);
}

void ParticleSystem::sortEndCol(const size_t index[]){
	std::unique_ptr<glm::vec4[]> new_end_col = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);

	for(size_t i = 0; i < m_count; ++i){
		new_end_col[i] = m_particles.m_end_col[index[i]];
	}

	m_particles.m_end_col = std::move(new_end_col);
}

void ParticleSystem::sortVel(const size_t index[]){
	std::unique_ptr<glm::vec4[]> new_vel = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);

	for(size_t i = 0; i < m_count; ++i){
		new_vel[i] = m_particles.m_vel[index[i]];
	}

	m_particles.m_vel = std::move(new_vel);
}

void ParticleSystem::sortAcc(const size_t index[]){
	std::unique_ptr<glm::vec4[]> new_acc = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);

	for(size_t i = 0; i < m_count; ++i){
		new_acc[i] = m_particles.m_acc[index[i]];
	}

	m_particles.m_acc = std::move(new_acc);
}

void ParticleSystem::sortTime(const size_t index[]){
	std::unique_ptr<glm::vec4[]> new_time = std::unique_ptr<glm::vec4[]>(new glm::vec4[m_count]);

	for(size_t i = 0; i < m_count; ++i){
		new_time[i] = m_particles.m_time[index[i]];
	}

	m_particles.m_time = std::move(new_time);
}