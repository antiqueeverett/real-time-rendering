/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/
#ifndef GEN_HPP
#define GEN_HPP

#include <rtr/ren/particleData.hpp>

class ParticleGenerator{
public:
	ParticleGenerator() { }
	virtual ~ParticleGenerator() { }

	virtual void generate(float dt, ParticleData *p, size_t start_Id, size_t end_id) = 0;
};

class PointPosGen : public ParticleGenerator {
public:
	glm::fvec4 m_pos;

	PointPosGen() : m_pos(glm::fvec4{0.0, 0.0, 0.0, 0.0}) {}
	PointPosGen(glm::fvec4 pos) : m_pos{pos} {}


	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
};

class PlanePosGen : public ParticleGenerator {
public:
	glm::fvec4 m_pos;
	glm::fvec4 m_norm;
	float m_rad;

	PlanePosGen() : m_pos(glm::fvec4{0.0, 0.0, 0.0, 0.0}), m_norm(glm::fvec4{0.0, 1.0, 0.0, 0.0}), m_rad{1.0f} {}
	PlanePosGen(glm::fvec4 pos, glm::fvec4 norm) : m_pos{pos}, m_norm{glm::normalize(norm)}, m_rad{1.0f} {}
	

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
};

class BoxPosGen : public ParticleGenerator {
public:
	glm::fvec4 m_pos;
	float m_offset;

	BoxPosGen() : m_pos(glm::fvec4{0.0, 0.0, 0.0, 0.0}), m_offset(0.0) {}
	BoxPosGen(glm::fvec4 pos, float offset) : m_pos{pos}, m_offset{offset} {}

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
};

class SpherePosGen : public ParticleGenerator {
public: 
	glm::fvec4 m_pos;
	float m_min_r, m_max_r;

	SpherePosGen() : m_min_r{0.0f}, m_max_r{1.0f} {}
	SpherePosGen(float r) : m_pos{glm::fvec4{0.0f}}, m_min_r{r}, m_max_r{r} {}
	SpherePosGen(float min_r, float max_r) : m_pos{glm::fvec4{0.0f}}, m_min_r{min_r}, m_max_r{max_r} {}
	SpherePosGen(glm::fvec4 pos, float min_r, float max_r) : m_pos{pos} , m_min_r{min_r}, m_max_r{max_r} {}

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
};

class RotatingPosGen: public ParticleGenerator {
public:
	glm::fvec4 m_pos;
	float m_min_r, m_max_r;
	float m_ticks = 0.f;

	RotatingPosGen() : m_pos{0.f}, m_min_r{0.0f}, m_max_r{1.0f} {}
	RotatingPosGen(glm::fvec4 pos) : m_pos{pos}, m_min_r{0.0f}, m_max_r{1.0f} {}
	RotatingPosGen(float r) : m_pos{0.0f}, m_min_r{r}, m_max_r{r} {}
	RotatingPosGen(glm::fvec4 pos, float r) : m_pos{pos}, m_min_r{r}, m_max_r{r} {}
	RotatingPosGen(glm::fvec4 pos, float min_r, float max_r) : m_pos{pos}, m_min_r{min_r}, m_max_r{max_r} {}

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;

};

class BasicColorGen : public ParticleGenerator {
public:
	glm::vec4 m_min_start_col;
	glm::vec4 m_max_start_col;
	glm::vec4 m_min_end_col;
	glm::vec4 m_max_end_col;

	BasicColorGen()
	: m_min_start_col(0.0)
	, m_max_start_col(0.0)
	, m_min_end_col(0.0)
	, m_max_end_col(0.0) { }

	BasicColorGen(glm::fvec4 s, glm::fvec4 e)
	: m_min_start_col{s}
	, m_max_start_col{s}
	, m_min_end_col{e}
	, m_max_end_col{e} { }

	BasicColorGen(glm::fvec4 min_s, glm::fvec4 max_s, glm::fvec4 min_e, glm::fvec4 max_e)
	: m_min_start_col{min_s}
	, m_max_start_col{max_s}
	, m_min_end_col{min_e}
	, m_max_end_col{max_e} { }

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
};

class BasicVelGen : public ParticleGenerator {
public:
	glm::vec4 m_min_vel;
	glm::vec4 m_max_vel;

	BasicVelGen() : m_min_vel(0.0), m_max_vel(0.0) { }
	BasicVelGen(glm::fvec4 min_vel, glm::fvec4 max_vel) : m_min_vel{min_vel}, m_max_vel{max_vel} {}

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
};

class SphereVelGen : public ParticleGenerator {
public:
	float m_min_vel;
	float m_max_vel;

	SphereVelGen() : m_min_vel(0.0), m_max_vel(0.0) { }
	SphereVelGen(float min_vel, float max_vel) : m_min_vel{min_vel}, m_max_vel{max_vel} {}

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
};

class ConeVelGen : public ParticleGenerator {
public:
	float m_min_vel;
	float m_max_vel;
	glm::fvec4 m_dir;
	float m_rad;

	ConeVelGen() : m_min_vel(0.5), m_max_vel(2.0), m_dir(glm::fvec4{0.0, -3.0, -1.0, 0.0}), m_rad(0.032) {
		m_dir = glm::normalize(m_dir);
		calc_rot_mat();
	}

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
	void set_dir(glm::fvec4 dir) {m_dir = dir;}
private:
	glm::fmat4 m_rot_mat;

	glm::fvec3 rotate(glm::fvec3 vec);
	void calc_rot_mat();

};

class BasicTimeGen : public ParticleGenerator {
public:
	float m_min_t;
	float m_max_t;

	BasicTimeGen() : m_min_t(0.0), m_max_t(0.0) { }
	BasicTimeGen(float min_t, float max_t) : m_min_t{min_t}, m_max_t{max_t} {}

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
};

class GaussTimeGen : public ParticleGenerator {
public:
	float m_mean;
	float m_deviation;

	GaussTimeGen() : m_mean(1.0), m_deviation(0.5) { }
	GaussTimeGen(float mean, float dev) : m_mean{mean}, m_deviation{dev} {}

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;
};

class VelTimeGen : public ParticleGenerator {
public:
	float m_mean;
	float m_deviation;
	glm::fvec4 m_velocity;

	VelTimeGen() : m_mean(1.0), m_deviation(0.5), m_velocity(glm::fvec4{0.0}) { }
	VelTimeGen(float mean, float dev, glm::fvec4 vel) : m_mean{mean}, m_deviation{dev}, m_velocity{vel} {}

	virtual void generate(float dt, ParticleData *p, size_t start_id, size_t end_id) override;	
};

#endif //GEN_HPP