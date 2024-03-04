#ifndef ParticleComponent
#define ParticleComponent

#include "ecs/ecs.h"

namespace TDS {
	enum ParticleMesh {
		CUBE = 0,
		SPHERE,
		CAPSULE,
		MAX_MESHES
	};
	struct Particle {
		Vec3 Size{ 10.f,10.f,10.f };
		Vec3 Rotation{ 0.f,0.f,0.f };
		Vec3 Position{ 0.f,0.f,0.f };
		Vec3 Velocity{ 0.f,10.f,0.f };
		Vec3 Acceleration{ 0.f, 10.f, 0.f };
		float Age{ 0.f };
		Vec3 Color{ 0.f,0.f,1.f };
		bool isActive{ true };
	};

	struct ParticleEmitter {
		Vec4 Color;
		Vec3 Spawnoffset;
		Vec3 Velocity;
		Vec3 Acceleration;
		float Life;
		Vec3 Position;
		Vec3 Size;

		float spawninterval, spawntimer;
		uint32_t maxparticles;
	};

	class Particle_Component : public IComponent {
	public:
		//getter
		ParticleEmitter& GetEmitter() { return Emitter; }
		ParticleMesh& GetMeshType() { return type; }
	private:
		ParticleEmitter Emitter;
		ParticleMesh type{ CUBE };
	};
}
#endif