/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/
#ifndef UPDATER_HPP
#define UPDATER_HPP
#define GLM_ENABLE_EXPERIMENTAL

#include <rtr/ren/particleData.hpp>
#include <glm/gtx/intersect.hpp>


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

class WindForceUpdater : public ParticleUpdater {
public:
    glm::vec3 m_wind;
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
    float m_min;
    int m_iter = 3;

    virtual void update(float dt, ParticleData *p) override;
    void maxStretch(float rest, glm::fvec4& pos_1, glm::fvec4& pos_2);

};

class NormalUpdater : public ParticleUpdater {
public:
	virtual void update(float dt, ParticleData *p) override;
};

class SphereCollisionUpdater : public ParticleUpdater {
public:
	float m_r, m_f;
	glm::vec3 m_pos;

	virtual void update(float dt, ParticleData *p) override;
};

class CubeCollisionUpdater : public ParticleUpdater {
public:
	glm::vec3 m_min, m_max;

	virtual void update(float dt, ParticleData *p) override;

};

class ClothCollisionUpdater : public ParticleUpdater {
public:
    float m_dist;
    int m_W, m_H, m_it = 1;
    std::vector<std::vector<bool>> m_collide;

    virtual void update(float dt, ParticleData *p) override;
    void init(ParticleData *p);

};

class GroundCollisionUpdater : public ParticleUpdater {
public:
	float m_height;
	virtual void update(float dt, ParticleData *p) override;

};

class SpringColourUpdater : public ParticleUpdater {
public:
    float m_min, m_max;
    glm::vec3 m_minCol, m_maxCol;

    virtual void update(float dt, ParticleData *p) override;
};

class AirResistanceUpdater : public ParticleUpdater {
public:
	float m_a;
	virtual void update(float dt, ParticleData *p) override;
};
/*
class SphereFrictionUpdater : public ParticleUpdater {
public:
	float r, m_friction = 0.1f;
	glm::vec3 m_pos;

	virtual void update(float dt, ParticleData *p) override;
};
 */
#endif //UPDATER_HPP