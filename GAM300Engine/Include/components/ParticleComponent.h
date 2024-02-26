#ifndef ParticleComponent
#define ParticleComponent

#include "ecs/ecs.h"

namespace TDS {
	enum ParticleMesh {
		CUBE = 0,
		SPHERE,
		CAPSULE
	};
	struct Particle {
		Vec3 Position;
		Vec3 Velocity{ 0.f,10.f,0.f };
		Vec3 Acceleration{ 0.f, 10.f, 0.f };
		Vec3 Size{ 10.f,10.f,10.f };
		Vec3 Color{ 0.f,0.f,1.f };
		float Lifetime{ 10.f };
		float Age{ 0.f };
		ParticleMesh type{ CUBE };
		bool isActive{ true };
	};

	class Particle_Component : public IComponent {
	public:
		//idk what to do with this function
		void resetParticle();
		std::uint32_t GetCurrentParticleCount() { return m_vParticles.size(); }
		std::uint32_t GetDesiredParticleCount() { return m_particlecount; }
		float GetDecayRate() { return m_decayrate; }
		bool GetEmittingState() { return m_isEmiiting; }
		std::vector<Particle> GetParticleVector() { return m_vParticles; }
	private:
		std::vector<Particle> m_vParticles;
		std::uint32_t m_particlecount;
		Vec3 spawnOffset{ 0.f,0.f,0.f };
		float m_decayrate{ 0.1f };
		bool m_isEmiiting{ false };
	};
}
#endif