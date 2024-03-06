#include "components/ParticleComponent.h"
#include "components/CompReflectionMacro.h"


RTTR_REGISTRATION
{
	using namespace TDS;

	
rttr::registration::class_<Particle_Component>("Particle Component")
.method("SetLife", &Particle_Component::SetLife)
.property("Life", &Particle_Component::GetLife)
.method("SetSpawn Offset", &Particle_Component::SetSpawnOffset)
.property("Spawn Offset", &Particle_Component::GetSpawnoffset)
.method("SetVelocity", &Particle_Component::SetVelocity)
.property("Velocity", &Particle_Component::GetVelocity)
.method("SetAcceleration", &Particle_Component::SetAcceleration)
.property("Acceleration", &Particle_Component::GetAcceleration)
.method("SetSize", &Particle_Component::SetSize)
.property("Size", &Particle_Component::GetSize)
.method("SetColor", rttr::select_overload<void(Vec4)>(&Particle_Component::SetColor))
.method("SetColor", rttr::select_overload<void(float, float, float, float)>(&Particle_Component::SetColor))
.property("Color", &Particle_Component::GetColor)
//.method for setting enums type?
.property("Type", &Particle_Component::GetMeshType)
.property("SpawnInterval", &Particle_Component::spawninterval)
.property("SpawnTimer", &Particle_Component::spawntimer)
.property("MaxParticles", &Particle_Component::maxparticles);

}

namespace TDS {
	Particle_Component* GetParticle_Component(EntityID entityID) {
		return ecs.getComponent<Particle_Component>(entityID);
	}

	Particle_Component::Particle_Component() : Emitter(), type(CUBE), spawninterval(1.f), spawntimer(0.f), maxparticles(50) {}
}