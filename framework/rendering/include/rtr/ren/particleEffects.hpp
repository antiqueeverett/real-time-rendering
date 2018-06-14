/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/
#ifndef PARTICLEEFFECT_HPP
#define PARTICLEEFFECT_HPP

#include <rtr/ren/headers.hpp>
#include <rtr/ren/particleHeaders.hpp>

class ParticleEffect {
public:
	ParticleEffect(){}
	virtual ~ParticleEffect(){}

	virtual void init(size_t numParticles) = 0;
	virtual void initRenderer() { m_rend->generate(m_sys.get());}
	virtual void reset() { m_sys->reset();}
	virtual void clean() { m_rend->destroy();}
	
	virtual void update(float dt) = 0;
	virtual void cpuUpdate(float dt) { m_sys->update(dt);}
	virtual void gpuUpdate() { m_rend->update();}
	virtual void render() { m_rend->render();}

	virtual int numAllParticles() { return m_sys->getCount();}
	virtual int numAliveParticles() { return m_sys->getAliveCount();};

protected:
	std::shared_ptr<ParticleSystem> m_sys;
	std::shared_ptr<ParticleRenderer> m_rend = std::make_shared<ParticleRenderer> ();

};

class FlameEffect : public ParticleEffect {
public:
	FlameEffect(){}
	~FlameEffect(){}

	void init(size_t numParticles) override;
	void update(float dt) override;

private:
	//generators
	std::shared_ptr<PointPosGen> m_posGen;
	std::shared_ptr<SphereVelGen> m_velGen;
	std::shared_ptr<BasicColorGen> m_colGen;
	std::shared_ptr<VelTimeGen> m_timeGen;
	//updaters
	std::shared_ptr<BasicAccUpdater> m_accUp;
	std::shared_ptr<BasicVelUpdater> m_velUp;
	std::shared_ptr<NoiseVelocityUpdater> m_noiseUp;
	std::shared_ptr<BasicPosUpdater> m_posUp;
	std::shared_ptr<BasicColorUpdater> m_colUp;
	std::shared_ptr<BasicTimeUpdater> m_timeUp;

};


class FlameThrowerEffect : public ParticleEffect {
public:
	FlameThrowerEffect(){}
	~FlameThrowerEffect(){}

	void init(size_t numParticles) override;
	void update(float dt) override;

	void setPos(glm::fvec3 pos);

private:
	//generators
	std::shared_ptr<PointPosGen> m_posGen;
	std::shared_ptr<ConeVelGen> m_velGen;
	std::shared_ptr<BasicColorGen> m_colGen;
	std::shared_ptr<GaussTimeGen> m_timeGen;
	//updaters
	std::shared_ptr<BasicAccUpdater> m_accUp;
	std::shared_ptr<BasicVelUpdater> m_velUp;
	std::shared_ptr<NoiseVelocityUpdater> m_noiseUp;
	std::shared_ptr<BasicPosUpdater> m_posUp;
	std::shared_ptr<BasicColorUpdater> m_colUp;
	std::shared_ptr<BasicTimeUpdater> m_timeUp;

};
#endif //PARTICLEEFFECT