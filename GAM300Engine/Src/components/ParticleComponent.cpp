#include "components/ParticleComponent.h"
#include "components/CompReflectionMacro.h"


RTTR_REGISTRATION
{
	using namespace TDS;

	rttr::registration::class_<Particle_Component>("Particle Component")
		.property("Life", &Particle_Component::GetLife, &Particle_Component::SetLife)
		.property("Spawn Offset", &Particle_Component::GetSpawnoffset, &Particle_Component::SetSpawnOffset)
		.property("Velocity", &Particle_Component::GetVelocity, &Particle_Component::SetVelocity)
		.property("Acceleration", &Particle_Component::GetAcceleration, &Particle_Component::SetAcceleration)
		.property("Size", &Particle_Component::GetSize, &Particle_Component::SetSize)
		.property("Color", &Particle_Component::GetColor, &Particle_Component::SetColor)
		.property("ParticleMesh", &Particle_Component::type)
		.property("SpawnInterval", &Particle_Component::spawninterval)
		.property("SpawnTimer", &Particle_Component::spawntimer)
		.property("MaxParticles", &Particle_Component::maxparticles);

	rttr::registration::enumeration<ParticleMesh>("ParticleMesh")
	(
		rttr::value("CUBE", ParticleMesh::CUBE),
		rttr::value("SPHERE", ParticleMesh::SPHERE),
		rttr::value("CAPSULE", ParticleMesh::CAPSULE)
	);
}

//.method("SetSpawn Offset", &Particle_Component::SetSpawnOffset)
//.method("SetVelocity", &Particle_Component::SetVelocity)
//.method("SetAcceleration", &Particle_Component::SetAcceleration)
//.method("SetSize", &Particle_Component::SetSize)
//.property("Size", &Particle_Component::GetSize)
//.method("SetColor", rttr::select_overload<void(Vec4)>(&Particle_Component::SetColor))
//.method("SetColor", rttr::select_overload<void(float, float, float, float)>(&Particle_Component::SetColor))
//.property("Color", &Particle_Component::GetColor)
////.method for setting enums type?
//.property("Type", &Particle_Component::GetMeshType)
//.property("SpawnInterval", &Particle_Component::spawninterval)
//.property("SpawnTimer", &Particle_Component::spawntimer)
//.property("MaxParticles", &Particle_Component::maxparticles);
//
//}


namespace TDS {
	Particle_Component* GetParticle_Component(EntityID entityID) {
		return ecs.getComponent<Particle_Component>(entityID);
	}

	Particle_Component::Particle_Component() : Emitter(), type(CUBE), spawninterval(1.f), spawntimer(0.f), maxparticles(50) {}
}