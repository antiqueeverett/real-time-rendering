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

class BasicColorUpdater : public ParticleUpdater {
public:
	virtual void update(float dt, ParticleData *p) override; 
};

class BasicTimeUpdater : public ParticleUpdater {
public:
	virtual void update(float dt, ParticleData *p) override; 
};

#endif //UPDATER_HPP