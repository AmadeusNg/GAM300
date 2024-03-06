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
		float Life{10.f};
		Vec3 Spawnoffset{0.f,0.f,0.f};
		Vec3 Velocity{0.f,10.f,0.f};
		Vec3 Acceleration{0.f,10.f,0.f};
		Vec3 Position{ 0.f,0.f,0.f };
		Vec3 Size{1.f,1.f,1.f};
		Vec4 Color{0.f,1.f,1.f,1.f};
	};

	struct Particle_Emitter_PushData {
		int particleamt;
		ParticleEmitter emitter;
	};

	class Particle_Component : public IComponent {
	public:
		DLL_API Particle_Component();
		DLL_API ~Particle_Component();

		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND;

		//getter
		float GetLife()					{ return Emitter.Life; };
		Vec3& GetSpawnoffset()			{ return Emitter.Spawnoffset; };
		Vec3& GetVelocity()				{ return Emitter.Velocity; };
		Vec3& GetAcceleration()			{ return Emitter.Acceleration; };
		Vec3& GetPosition()				{ return Emitter.Position; };
		Vec3& GetSize()					{ return Emitter.Size; };
		Vec4& GetColor()				{ return Emitter.Color; };
		ParticleEmitter& GetEmitter()	{ return Emitter; }
		ParticleMesh& GetMeshType()		{ return type; }
		float& GetSpawnInterval()		{ return spawninterval; }
		float& GetSpawnTimer()			{ return spawntimer; }
		uint32_t& GetMaxParticles()		{ return maxparticles; }

		//setters
		void SetLife(float input)							{ Emitter.Life = input; }
		void SetSpawnOffset(Vec3 input)						{ Emitter.Spawnoffset = input; }
		void SetVelocity(Vec3 input)						{ Emitter.Velocity = input; }
		void SetAcceleration(Vec3 input)					{ Emitter.Acceleration = input; }
		//void SetPosition(Vec3 input)						{ Emitter.Position = input; }
		void SetSize(Vec3 input)							{ Emitter.Size = input; }
		void SetColor(Vec4 input)							{ Emitter.Color = input; }
		void SetColor(float r, float g, float b, float a)	{ Emitter.Color = Vec4(r, g, b, a); }
	private:
		ParticleEmitter Emitter;
		ParticleMesh type{ CUBE };
		float spawninterval, spawntimer;
		uint32_t maxparticles;
	};
	DLL_API Particle_Component* GetParticle_Component(EntityID entityID);
}
#endif