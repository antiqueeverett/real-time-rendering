/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/
#ifndef UPDATER_HPP
#define UPDATER_HPP

#include <rtr/ren/particleData.hpp>

class ParticleUpdater{
public:
	ParticleUpdater() { }
	virtual ~ParticleUpdater() { }									// base destructors should be virtual, else
																	// Base b* = new Derived (); delete b 
																	// leads to undefinded behaviour
	virtual void update(float dt, ParticleData *p) = 0;
};

class GravityUpdater : public ParticleUpdater {
public:
	glm::fvec4 m_grav;

	GravityUpdater() : m_grav{0.0, -9.81, 0.0, 0.0} {}
	GravityUpdater(float grav) : m_grav{0.0, -grav, 0.0, 0.0} {}

	virtual void update(float dt, ParticleData *p) override;
};

class NoiseVelocityUpdater : public ParticleUpdater {
public:
	float m_amplitude;
	float m_frequency;

	NoiseVelocityUpdater() : m_amplitude(1.0f), m_frequency(1.0f) {}
	NoiseVelocityUpdater(float amp, float freq) : m_amplitude{amp}, m_frequency{freq} {}

	virtual void update(float dt, ParticleData *p) override;
};

class AttractorUpdater : public ParticleUpdater {
public:
	std::vector<glm::vec4> m_attractors;

	virtual void update(float dt, ParticleData *p) override;
};

class BasicAccUpdater : public ParticleUpdater {
public:
	glm::fvec4 m_acceleration;
	
	BasicAccUpdater(glm::fvec4 acc) : m_acceleration{acc} {}

	virtual void update(float dt, ParticleData *p) override;
};

class BasicVelUpdater : public ParticleUpdater {
public:
	virtual void update(float dt, ParticleData *p) override;
};

class BasicPosUpdater : public ParticleUpdater {
public:
	virtual void update(float dt, ParticleData *p) override;
};

class VerletPosUpdater : public ParticleUpdater {
public:
	float m_damp;

	VerletPosUpdater() : m_damp(0.f) {}
	VerletPosUpdater(float damp) : m_damp(damp) {}
	virtual void update(float dt, ParticleData *p) override;
};

class TranslationUpdater : public ParticleUpdater {
public:
	glm::fvec4 m_translate;

	TranslationUpdater() : m_translate(glm::fvec4{0.0f}) {}
	TranslationUpdater(glm::fvec3 dist) : m_translate(glm::fvec4{dist, 1.0f}) {}

	virtual void update(float dt, ParticleData *p) override;
};

class PositionRemover : public ParticleUpdater {
public:
	float m_dist;

	PositionRemover() : m_dist(1.0f) {}
	PositionRemover(float dist) : m_dist(dist) {}

	virtual void update(float dt, ParticleData *p) override;
	
};

class CubeRemover : public ParticleUpdater {
public:
	glm::fvec4 m_min;
	glm::fvec4 m_max;

	virtual void update(float dt, ParticleData *p) override;
	
};

class BasicColorUpdater : public ParticleUpdater {
public:
	virtual void update(float dt, ParticleData *p) override; 
};


class VelColorUpdater : public ParticleUpdater {
public:
	glm::vec4 m_minVel;
	glm::vec4 m_maxVel;

	VelColorUpdater() : m_minVel(0.0), m_maxVel(1.0) { }
	VelColorUpdater(glm::fvec4 min, glm::fvec4 max) : m_minVel(min), m_maxVel(max) { }


	virtual void update(float dt, ParticleData *p) override;
};

class BasicTimeUpdater : public ParticleUpdater {
public:
	virtual void update(float dt, ParticleData *p) override; 
};

class SpringUpdater : public ParticleUpdater {
public:
	float m_k;
    std::shared_ptr<std::vector<glm::vec3>> m_spring;

	SpringUpdater(std::shared_ptr<std::vector<glm::vec3>> springs) : m_k(1.0f), m_spring(springs){}

	virtual void update(float dt, ParticleData *p) override;
};

class StretchUpdater : public ParticleUpdater {
public:
    float m_max;
    int m_iter;

    StretchUpdater() : m_max(1.2f), m_iter(3){}
    StretchUpdater(float max, int iter) : m_max(max), m_iter(iter){}

    virtual void update(float dt, ParticleData *p) override;
    void maxStretch(float rest, glm::fvec4& pos_1, glm::fvec4& pos_2);

};

#endif //UPDATER_HPP