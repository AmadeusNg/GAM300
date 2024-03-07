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
		float minlife{ 1.f }, maxlife{ 10.f };
		Vec3 minSpawnoffset{ 0.f,0.f,0.f }, maxSpawnoffset{ 0.f,0.f,0.f };
		Vec3 minVelocity{ 0.f,10.f,0.f }, maxVelocity{ 0.f,15.f,0.f };
		Vec3 minAcceleration{ 0.f,10.f,0.f }, maxAcceleration{ 0.f,15.f,0.f };
		Vec3 minSize{ 1.f,1.f,1.f }, maxSize{ 10.f,10.f,10.f };
		Vec3 Position{ 0.f,0.f,0.f };
		Vec4 Color{ 0.f,1.f,1.f,1.f };
	};

	struct Particle_Emitter_PushData {
		int particleamt;
		ParticleEmitter emitter;
	};

	class Particle_Component : public IComponent {
	public:
		DLL_API Particle_Component();
		DLL_API ~Particle_Component() = default;


		//getter
		float& GetMinLife()						{ return Emitter.minlife; };
		float& GetMaxLife()						{ return Emitter.maxlife; };
		Vec3& GetMinSpawnoffset()				{ return Emitter.minSpawnoffset; };
		Vec3& GetMaxSpawnoffset()				{ return Emitter.maxSpawnoffset; };
		Vec3& GetMinVelocity()					{ return Emitter.minVelocity; };
		Vec3& GetMaxVelocity()					{ return Emitter.maxVelocity; };
		Vec3& GetMinAcceleration()				{ return Emitter.minAcceleration; };
		Vec3& GetMaxAcceleration()				{ return Emitter.maxAcceleration; };
		Vec3& GetMinSize()						{ return Emitter.minSize; };
		Vec3& GetMaxSize()						{ return Emitter.maxSize; };
		Vec3& GetPosition()						{ return Emitter.Position; };
		Vec4& GetColor()						{ return Emitter.Color; };
		ParticleEmitter& GetEmitter()			{ return Emitter; }
		ParticleMesh& GetMeshType()				{ return type; }
		float& GetSpawnInterval()				{ return spawninterval; }
		float& GetSpawnTimer()					{ return spawntimer; }
		uint32_t& GetMaxParticles()				{ return maxparticles; }

		//setters
		void SetMinLife(float& input)			{ Emitter.minlife = input; }
		void SetMaxLife(float& input)			{ Emitter.maxlife = input; }
		void SetMinSpawnOffset(Vec3& input)		{ Emitter.minSpawnoffset = input; }
		void SetMaxSpawnOffset(Vec3& input)		{ Emitter.maxSpawnoffset = input; }
		void SetMinVelocity(Vec3& input)		{ Emitter.minVelocity = input; }
		void SetMaxVelocity(Vec3& input)		{ Emitter.maxVelocity = input; }
		void SetMinAcceleration(Vec3& input)	{ Emitter.minAcceleration = input; }
		void SetMaxAcceleration(Vec3& input)	{ Emitter.maxAcceleration = input; }
		void SetMinSize(Vec3& input)			{ Emitter.minSize = input; }
		void SetMaxSize(Vec3& input)			{ Emitter.maxSize = input; }
		void SetColor(Vec4& input)				{ Emitter.Color = input; }
		void SetSpawnTimer(float& input)		{ spawntimer = input; }
		//void SetPosition(Vec3 input)							{ Emitter.Position = input; }

		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND;

	private:
		ParticleEmitter Emitter;
		ParticleMesh type{ CUBE };
		float spawninterval, spawntimer;
		uint32_t maxparticles;
	};
	DLL_API Particle_Component* GetParticle_Component(EntityID entityID);
}
#endif