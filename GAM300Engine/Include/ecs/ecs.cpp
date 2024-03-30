/*!*************************************************************************
****
\file ecs.cpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines functions for the bunding of the systems to 
		the ECS
****************************************************************************
***/

#include "ecs/systems.h"

#include "physics/physicsSystem.h"
#include "AI/aiSystem.h"
#include "Rendering/RendererSystem.h"
#include "camera/Camerasystem/CameraSystem.h"
#include "Rendering/UiSystem.h"
#include "eventManager/eventHandler.h"
#include "Physics/CollisionSystem.h"
#include "Animation/AnimationSystem.h"
#include "AI/Pathfinder.h"
#include "Rendering/ParticleSystem.h"

namespace TDS
{
	/*!*************************************************************************
	This function binds functions of the systems to the ECS to run
	****************************************************************************/
	void bindSystemFunctions()
	{
		_PhysicsSystem.action(PhysicsSystem::PhysicsSystemInit, PhysicsSystem::PhysicsSystemUpdate);
		_AISystem.action(AISystem::AISystemInit, AISystem::AISystemUpdate);
		_CameraSystem.action(CameraSystem::CameraSystemInit, CameraSystem::CameraSystemUpdate);
		_EventHandler_ChildTransform.action(EventHandler::childTransformationHandlerInit, EventHandler::childTransformationHandlerUpdate);
		_EventHandler_Enable.action(EventHandler::childEnableHandlerInit, EventHandler::childEnableHandlerUpdate);
		_UiSystem.action(UiSystem::Init, UiSystem::Update);
		_ColliderDebugSystem.action(CollisionSystem::CollisionSystemInit, CollisionSystem::CollisionSystemUpdate);
		_AnimationSystem.action(AnimationSystem::Init, AnimationSystem::Update);
		_RenderSystem.action(RendererSystem::OnInit, RendererSystem::OnUpdate);
		_AudioSystem.action(proxy_audio_system::audio_system_init, proxy_audio_system::audio_system_update);
		_PathfinderSystem.action(Pathfinder::Initialize, Pathfinder::Update);
		_ParticleSystem.action(ParticleSystem::Init, ParticleSystem::UpdateSystem);
	}
}
