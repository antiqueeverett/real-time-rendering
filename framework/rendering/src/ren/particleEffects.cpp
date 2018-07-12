#define GLM_ENABLE_EXPERIMENTAL
/******************************************************************************
 * Concept & Code from  Bartłomiej Filipek									  *
 * https://github.com/fenbf/particles 										  *
 * https://www.bfilipek.com/2014/03/three-particle-effects.html				  *
 ******************************************************************************/
#include <rtr/ren/particleEffects.hpp>

void FlameEffect::init(size_t numParticles, Camera* cam) {
	m_sys = std::make_shared<ParticleSystem>(numParticles, cam);
	//Generators
	m_posGen = std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{0.f, 0.f, 0.f, 0.0f}));
	m_velGen = std::make_shared<SphereVelGen>(SphereVelGen(0.01f, 0.2f));
	m_colGen = std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{0.5f, 0.02f, 0.001f, 0.0f}, 
											   				 glm::fvec4{0.8f, 0.1f, 0.002f, 0.0f}, 
											   				 glm::fvec4{0.2f, 0.0f, 0.0f, 0.0f},
											   	 			 glm::fvec4{0.3f, 0.0f, 0.0f, 0.0f}));
	m_timeGen = std::make_shared<VelTimeGen>(VelTimeGen(0.8f, 0.4f, glm::fvec4{0.0, 3.0, 0.0, 0.0}));
	//create emiiter
	auto emmit = std::make_shared<ParticleEmitter>(100.f);
	emmit->addGenerator(m_posGen);
	emmit->addGenerator(m_velGen);
	emmit->addGenerator(m_colGen);
	emmit->addGenerator(m_timeGen);
	//Updaters
	m_accUp = std::make_shared<BasicAccUpdater>(glm::fvec4{0.0f, 8.f, 0.0f, 0.0f});
	m_velUp = std::make_shared<BasicVelUpdater>();
	m_noiseUp = std::make_shared<NoiseVelocityUpdater>(0.6f, 2.0f);
	m_posUp = std::make_shared<BasicPosUpdater>();
	m_colUp = std::make_shared<BasicColorUpdater>();
	m_timeUp = std::make_shared<BasicTimeUpdater>();

	m_sys->addUpdater(m_accUp);
	m_sys->addUpdater(m_velUp);
	m_sys->addUpdater(m_noiseUp);
	m_sys->addUpdater(m_posUp);
	m_sys->addUpdater(m_colUp);
	m_sys->addUpdater(m_timeUp);

	m_sys->addEmitter(emmit);

}



void FlameEffect::update(float dt) {
	return;
}

void FlameThrowerEffect::init(size_t numParticles, Camera* cam) {
	m_sys = std::make_shared<ParticleSystem>(numParticles, cam);
	//Generators
	m_posGen = std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{0.f, 0.f, 0.f, 0.0f}));
	m_velGen = std::make_shared<ConeVelGen>(ConeVelGen(1.0f, 3.0f, glm::fvec3{0.0, -3.0, -1.0}, 0.032f));

	m_colGen = std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{0.5f, 0.02f, 0.005f, 0.0f}, 
											   				 glm::fvec4{0.8f, 0.1f, 0.006f, 0.0f}, 
											   				 glm::fvec4{0.2f, 0.0f, 0.0f, 0.0f},
											   	 			 glm::fvec4{0.3f, 0.0f, 0.0f, 0.0f}));
	m_timeGen = std::make_shared<GaussTimeGen>(GaussTimeGen(1.f, 1.5f));
	//create emiiter
	auto emmit = std::make_shared<ParticleEmitter>(100.f);
	emmit->addGenerator(m_posGen);
	emmit->addGenerator(m_velGen);
	emmit->addGenerator(m_colGen);
	emmit->addGenerator(m_timeGen);
	//Updaters
	m_accUp = std::make_shared<BasicAccUpdater>(glm::fvec4{0.0f, 4.f, 0.0f, 0.0f});
	m_velUp = std::make_shared<BasicVelUpdater>();
	m_noiseUp = std::make_shared<NoiseVelocityUpdater>(0.6f, 2.0f);
	m_posUp = std::make_shared<BasicPosUpdater>();
	m_colUp = std::make_shared<BasicColorUpdater>();
	m_timeUp = std::make_shared<BasicTimeUpdater>();

	m_sys->addUpdater(m_accUp);
	m_sys->addUpdater(m_velUp);
	m_sys->addUpdater(m_noiseUp);
	m_sys->addUpdater(m_posUp);
	m_sys->addUpdater(m_colUp);
	m_sys->addUpdater(m_timeUp);

	m_sys->addEmitter(emmit);

}



void FlameThrowerEffect::update(float dt) {
	return;
}

void FlameThrowerEffect::setPos(glm::fvec3 pos){
	m_posGen->m_pos = glm::fvec4(pos, 1.0f);
}

void FlameThrowerEffect::setDir(glm::fvec3 dir){
	m_velGen->set_dir(dir);
}

void TrailEffect::init(size_t numParticles, Camera* cam) {
	m_sys = std::make_shared<ParticleSystem>(numParticles, cam);
	//Generators
	m_posGen = std::make_shared<SpherePosGen>(SpherePosGen(0.1f));
	m_velGen = std::make_shared<ConeVelGen>(ConeVelGen(1.0f, 3.0f, glm::fvec3{0.0, -3.0, -1.0}, 0.032f));

	m_colGen = std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{0.5f, 0.02f, 0.005f, 0.0f}, 
											   				 glm::fvec4{0.8f, 0.1f, 0.006f, 0.0f}, 
											   				 glm::fvec4{0.2f, 0.0f, 0.0f, 0.0f},
											   	 			 glm::fvec4{0.3f, 0.0f, 0.0f, 0.0f}));
	m_timeGen = std::make_shared<GaussTimeGen>(GaussTimeGen(1.f, 1.5f));
	//create emiiter
	auto emmit = std::make_shared<ParticleEmitter>(100.f);
	emmit->addGenerator(m_posGen);
	emmit->addGenerator(m_velGen);
	emmit->addGenerator(m_colGen);
	emmit->addGenerator(m_timeGen);
	//Updaters
	m_accUp = std::make_shared<BasicAccUpdater>(glm::fvec4{0.0f, 4.f, 0.0f, 0.0f});
	m_velUp = std::make_shared<BasicVelUpdater>();
	m_noiseUp = std::make_shared<NoiseVelocityUpdater>(0.6f, 2.0f);
	m_posUp = std::make_shared<BasicPosUpdater>();
	m_colUp = std::make_shared<BasicColorUpdater>();
	m_timeUp = std::make_shared<BasicTimeUpdater>();

	//m_sys->addUpdater(m_accUp);
	m_sys->addUpdater(m_velUp);
	m_sys->addUpdater(m_noiseUp);
	m_sys->addUpdater(m_posUp);
	m_sys->addUpdater(m_colUp);
	m_sys->addUpdater(m_timeUp);

	m_sys->addEmitter(emmit);

}



void TrailEffect::update(float dt) {
	return;
}

void TrailEffect::setPos(glm::fvec3 pos){
	m_posGen->m_pos = glm::fvec4(pos, 1.0f);
}

void TrailEffect::setDir(glm::fvec3 dir){
	m_velGen->set_dir(dir);
}

void BlackHoleEffect::init(size_t numParticles, Camera* cam) {
	m_sys = std::make_shared<ParticleSystem>(numParticles, cam);
	//Generators
	m_posGen.push_back(std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{4.f, 1.f, 0.f, 1.0f})));
	m_velGen.push_back(std::make_shared<ConeVelGen>(ConeVelGen(0.5f, 2.0f, glm::fvec3{0.0, -1.0, -1.0}, 0.032f)));

	m_posGen.push_back(std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{-4.f, 1.f, 0.f, 1.0f})));
	m_velGen.push_back(std::make_shared<ConeVelGen>(ConeVelGen(0.5f, 2.0f, glm::fvec3{0.0, 1.0, -1.0}, 0.032f)));


	m_colGen.push_back(std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{1.0f, 0.0f, 0.0f, 1.0f}, glm::fvec4{0.1f, 0.0f, 0.0f, 1.0f})));
	m_colGen.push_back(std::make_shared<BasicColorGen>(BasicColorGen(glm::fvec4{0.0f, 1.0f, 0.0f, 1.0f}, glm::fvec4{0.0f, 0.1f, 0.0f, 1.0f})));

	m_posGen.push_back(std::make_shared<PointPosGen>(PointPosGen(glm::fvec4{0.f, 0.f, 0.f, 1.0f})));

	std::shared_ptr<OrbitVelGen> orbit = std::make_shared<OrbitVelGen>(OrbitVelGen());
	std::shared_ptr<SpherePosGen> sphere = std::make_shared<SpherePosGen>(SpherePosGen(10.0f));
	
	m_timeGen = std::make_shared<BasicTimeGen>(BasicTimeGen(20.f, 20.f));
	//create emiiter
	/*
	auto emmit = std::make_shared<ParticleEmitter>(10.f);
	emmit->addGenerator(m_posGen[0]);
	emmit->addGenerator(m_velGen[0]);
	emmit->addGenerator(m_colGen[0]);
	emmit->addGenerator(m_timeGen);

	auto emmit2 = std::make_shared<ParticleEmitter>(10.f);
	emmit2->addGenerator(m_posGen[1]);
	emmit2->addGenerator(m_velGen[1]);
	emmit2->addGenerator(m_colGen[1]);
	emmit2->addGenerator(m_timeGen);
	*/

	auto emmit3 = std::make_shared<ParticleEmitter>(numParticles);
	emmit3->addGenerator(sphere);
	emmit3->addGenerator(m_colGen[0]);
	emmit3->addGenerator(orbit);
	emmit3->addGenerator(m_timeGen);

	//Updaters
	m_velUp = std::make_shared<BasicVelUpdater>();
	m_posUp = std::make_shared<BasicPosUpdater>();
	m_remUp = std::make_shared<PositionRemover>();
	m_timeUp = std::make_shared<BasicTimeUpdater>();
	m_colUp = std::make_shared<VelColorUpdater>(glm::fvec4{-0.5f, -0.5f, -0.5f, 0.0f}, glm::fvec4{2.0f});
	m_attUp = std::make_shared<AttractorUpdater>();

	m_attUp->m_attractors.push_back(glm::fvec4{0.0f, 0.0f, 0.0f, 1.f});


	m_sys->addUpdater(m_attUp);
	m_sys->addUpdater(m_velUp);
	m_sys->addUpdater(m_posUp);
	m_sys->addUpdater(m_remUp);
	//m_sys->addUpdater(m_timeUp);
	m_sys->addUpdater(m_colUp);


	//m_sys->addEmitter(emmit2);
	//m_sys->addEmitter(emmit);
	m_sys->addEmitter(emmit3);

}



void BlackHoleEffect::update(float dt) {
	return;
}


void WheatherEffect::init(size_t numParticles, Camera* cam) {
	m_sys = std::make_shared<ParticleSystem>(numParticles, cam);
	//Generators
	m_posGen = std::make_shared<PlanePosGen>(PlanePosGen(glm::fvec3{0.0f, 100.0f, 0.0f}, glm::fvec3{0.0f, 1.0f, 0.0f}));
	m_posGen->m_rad = 40.0f;
	auto emmit = std::make_shared<ParticleEmitter>(100);
	emmit->addGenerator(m_posGen);
	//Updaters
	m_accUp = std::make_shared<GravityUpdater>();
	m_velUp = std::make_shared<BasicVelUpdater>();
	m_posUp = std::make_shared<BasicPosUpdater>();
	m_remUp = std::make_shared<CubeRemover>();
	m_transUp = std::make_shared<TranslationUpdater>();

	m_remUp->m_min = glm::fvec4{-50.0f, -50.0f, -50.0f, 1.0f};
	m_remUp->m_max = glm::fvec4{50.0f, 101.0f, 50.0f, 1.0f};

	m_sys->addUpdater(m_accUp);
	m_sys->addUpdater(m_velUp);
	m_sys->addUpdater(m_posUp);
	m_sys->addUpdater(m_transUp);
	m_sys->addUpdater(m_remUp);

	m_sys->addEmitter(emmit);

}

void WheatherEffect::move(glm::fvec3 mov){
	m_transUp->m_translate = glm::fvec4(mov, 0.0f);
}


void WheatherEffect::update(float dt) {
	return;
}