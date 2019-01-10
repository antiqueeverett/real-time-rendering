/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/
#ifndef PARTICLEEFFECT_HPP
#define PARTICLEEFFECT_HPP

#include <rtr/ren/headers.hpp>
#include <rtr/ren/particleHeaders.hpp>
#include <rtr/camera/Camera.h>

class ParticleEffect {
public:
	ParticleEffect(){}
	virtual ~ParticleEffect(){}

	virtual void init(size_t numParticles, Camera* cam) = 0;
	virtual void initRenderer() { m_rend->generate(m_sys.get());}
	virtual void reset() { m_sys->reset();}
	virtual void clean() { m_rend->destroy();}
	virtual void toggleUpdate() { m_sys->toggleUpdate(); }
	virtual void toggleEmit() { m_sys->toggleEmit(); }
	
	virtual void update(float dt) = 0;
	virtual void cpuUpdate(float dt) { m_sys->update(dt);}
	virtual void gpuUpdate() { m_rend->update();}
	virtual void render() { m_rend->render();}
    virtual void emit() { m_sys->emit();}

	virtual int numAllParticles() { return m_sys->getCount();}
	virtual int numAliveParticles() { return m_sys->getAliveCount();};

	virtual void fixedParticles(unsigned int i) { m_sys->finalData()->m_pos.get()[i].w = 1; };
	virtual void movingParticles(unsigned int i) { m_sys->finalData()->m_pos.get()[i].w = 0; };

protected:
	std::shared_ptr<ParticleSystem> m_sys;
	std::shared_ptr<ParticleRenderer> m_rend = std::make_shared<ParticleRenderer> ();

};

class FlameEffect : public ParticleEffect {
public:
	FlameEffect(){}
	~FlameEffect(){}

	void init(size_t numParticles, Camera* cam) override;
	void update(float dt) override;
	void setPos(glm::fvec3 pos);
	void setColor(glm::fvec3 col);

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
	std::shared_ptr<TranslationUpdater> m_transUp;



};


class FlameThrowerEffect : public ParticleEffect {
public:
	FlameThrowerEffect(){}
	~FlameThrowerEffect(){}

	void init(size_t numParticles, Camera* cam) override;
	void update(float dt) override;

	void setPos(glm::fvec3 pos);
	void setDir(glm::fvec3 dir);

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
class TrailEffect : public ParticleEffect {
public:
	TrailEffect(){}
	~TrailEffect(){}

	void init(size_t numParticles, Camera* cam) override;
	void update(float dt) override;

	void setPos(glm::fvec3 pos);
	void setDir(glm::fvec3 dir);

private:
	//generators
	std::shared_ptr<RotatingPosGen> m_posGen;
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

class BlackHoleEffect : public ParticleEffect {
public:
	BlackHoleEffect(){}
	~BlackHoleEffect(){}

	void init(size_t numParticles, Camera* cam) override;
	void update(float dt) override;


private:
	//generators
	std::vector<std::shared_ptr<PointPosGen>> m_posGen;
	std::vector<std::shared_ptr<ConeVelGen>> m_velGen;
	std::vector<std::shared_ptr<BasicColorGen>> m_colGen;
	std::shared_ptr<BasicTimeGen> m_timeGen;
	//updaters
	std::shared_ptr<BasicVelUpdater> m_velUp;
	std::shared_ptr<BasicPosUpdater> m_posUp;
	std::shared_ptr<PositionRemover> m_remUp;
	std::shared_ptr<BasicTimeUpdater> m_timeUp;
	std::shared_ptr<VelColorUpdater> m_colUp;
	std::shared_ptr<AttractorUpdater> m_attUp;
	

};

class FireRing : public ParticleEffect {
public:
	FireRing(){}
	~FireRing(){}

	void init(size_t numParticles, Camera* cam) override;
	void update(float dt) override;
	void move(glm::fvec3 mov);
	void randPos(glm::fvec3 min, glm::fvec3 max);
	void setColor(glm::fvec3 col);

	void initRenderer() ;
	void reset() ;
	void clean() ;
	void toggleUpdate() ;
	void toggleEmit() ;
	
	void cpuUpdate(float dt) ;
	void gpuUpdate() ;
	void render() ;

	glm::fvec3 m_pos = glm::fvec3{0.0f};
private:
	std::vector<std::shared_ptr<FlameEffect>> m_flames;
};

class ClothEffect : public ParticleEffect {
public:
	ClothEffect(){}
	~ClothEffect(){}

	void init(size_t numParticles, Camera* cam) override;
	void update(float dt) override;

    glm::fvec4 m_gridPos;
    glm::fmat4 m_gridRot;
    int m_gridW;
    int m_gridH;
    int m_gridD;

private:
	std::shared_ptr<BasicColorGen> m_colGen;
	std::shared_ptr<GridPosGen> m_posGen;
    std::shared_ptr<PrevPosGen> m_prevGen;
    std::shared_ptr<SpringGen> m_springGen;

    std::shared_ptr<GravityUpdater> m_gravUp;
    std::shared_ptr<VerletPosUpdater> m_posUp;
    std::shared_ptr<BasicColorUpdater> m_colUp;
    std::shared_ptr<SpringUpdater> m_structUp;
    std::shared_ptr<SpringUpdater> m_shearUp;
    std::shared_ptr<SpringUpdater> m_bendUp;
    std::shared_ptr<StretchUpdater> m_stretchUp;

};
#endif