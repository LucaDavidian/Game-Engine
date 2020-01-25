#include "RenderingSystem.h"
#include "EntitySystem.h"
#include "PositionComponent.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "ShadowComponent.h"
#include "SkyboxComponent.h"
#include "data structures/Vector.h"

void RenderingSystem::Render()
{
	Entity *activeCamera = nullptr;
	Entity *skybox = nullptr;

	Vector<Entity*> lights;

	// dispatch entities to renderers
	for (Entity *entity : EntitySystem::GetInstance().GetEntities())
	{
		if (entity->HasComponent<CameraComponent>())
		{
			CameraComponent *cameraComponent = entity->GetComponent<CameraComponent>();
			if (cameraComponent->IsActive())
				activeCamera = entity;
		}

		if (entity->HasComponent<LightComponent>() && entity->HasComponent<PositionComponent>())  // embed position component in entity?
			lights.InsertLast(entity);

		if (entity->HasComponent<SkyboxComponent>())
			skybox = entity;
		else if (entity->HasComponent<StaticMeshComponent>() && entity->HasComponent<PositionComponent>())
			mStaticEntityRenderer.AddEntity(entity);

		if (entity->HasComponent<ShadowComponent>())
			mShadowRenderer.AddEntity(entity);
	}

	if (activeCamera)
	{
		if (skybox)
		{
			GraphicsSystem::GetInstance().SetDepthStencilState(GraphicsSystem::DepthStencilState::DISABLED);
			mSkyBoxRenderer.Render(skybox, activeCamera);
			GraphicsSystem::GetInstance().SetDepthStencilState(GraphicsSystem::DepthStencilState::ENABLED);
		}
		
		// render shadows to shadow map
		//for (Entity *light : lights)
			//if (light->GetComponent<LightComponent>()->GetType() == LightComponent::Type::DIRECTIONAL || light->GetComponent<LightComponent>()->GetType() == LightComponent::Type::SPOT)
				mShadowRenderer.Render(activeCamera, lights[0], lights[1]);

		// render entities
		mStaticEntityRenderer.Render(activeCamera, lights, mShadowRenderer.GetShadowMap(), mShadowRenderer.GetShadowMapSpot(), mShadowRenderer.GetLightViewProjectionMatrix(), mShadowRenderer.GetLightViewProjectionMatrixSpot(), mShadowRenderer.GetShadowDistance());
	} 

	mGUIRenderer.Render();
}