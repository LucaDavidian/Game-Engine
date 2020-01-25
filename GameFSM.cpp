#include "GameFSM.h"
#include "GUI.h"

MenuGameState GameState::mMenuGameState;
PlayGameState GameState::mPlayGameState;
PauseGameState GameState::mPauseGameState;
ExitGameState GameState::mExitGameState;

/**** main menu game state ****/
void MenuGameState::OnEntry()
{
	GUIPanel *panel = new GUIPanel();
	panel->SetRelativeDimensions(XMFLOAT2(0.5f, 0.5f));
	panel->SetRelativePositionCenter(XMFLOAT2(0.5f, 0.5f));
	panel->SetColor(XMFLOAT3(1.0f, 0.7f, 0.3f));


	GUIButton *button1 = new GUIButton(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	panel->AddChild(button1);
	button1->SetRelativeDimensions(XMFLOAT2(0.35f, 0.15f));
	button1->SetRelativePositionCenter(XMFLOAT2(0.5f, 0.4f));
	button1->SetTexture(&TextureManager::GetInstance().GetTexture("res/button.png"));

	GUILabel *label1 = new GUILabel("Options");
	button1->AddChild(label1);
	label1->SetRelativeDimensions(XMFLOAT2(0.5f, 0.5f));
	label1->SetRelativePositionCenter(XMFLOAT2(0.5, 0.5));


	GUIButton *button2 = new GUIButton(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	panel->AddChild(button2);
	button2->SetRelativeDimensions(XMFLOAT2(0.35f, 0.15f));
	button2->SetRelativePositionCenter(XMFLOAT2(0.5f, 0.6f));
	button2->SetTexture(&TextureManager::GetInstance().GetTexture("res/button.png"));
	button2->SetName("play");
	button2->AddButtonListener(GameFSM::GetInstance(), &GameFSM::OnButtonPressed);

	GUILabel *label2 = new GUILabel("Play");
	button2->AddChild(label2);
	label2->SetRelativeDimensions(XMFLOAT2(0.5f, 0.5f));
	label2->SetRelativePositionCenter(XMFLOAT2(0.5, 0.5));

	
	GUIButton *button3 = new GUIButton(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	panel->AddChild(button3);
	button3->SetRelativeDimensions(XMFLOAT2(0.35f, 0.15f));
	button3->SetRelativePositionCenter(XMFLOAT2(0.5f, 0.8f));
	button3->SetTexture(&TextureManager::GetInstance().GetTexture("res/button.png"));
	button3->SetName("quit");
	button3->AddButtonListener(GameFSM::GetInstance(), &GameFSM::OnButtonPressed);

	GUILabel *label3 = new GUILabel("Quit");
	button3->AddChild(label3);
	label3->SetRelativeDimensions(XMFLOAT2(0.5f, 0.5f));
	label3->SetRelativePositionCenter(XMFLOAT2(0.5, 0.5));

	mMenu = new GUI(panel);  // registers itself to systems
	mMenu->SetVisible(true);
}

void MenuGameState::OnButtonPressed(GameFSM &fsm, const std::string &buttonName)
{
	if (buttonName == "play")
		fsm.ChangeState(&GameState::mPlayGameState);

	if (buttonName == "quit")
		fsm.PushState(&GameState::mExitGameState);
}

void MenuGameState::OnExit()
{
	mMenu->Destroy();
	mMenu = nullptr;
}

#include "EntitySystem.h"

#include "PositionComponent.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"
#include "ArrowInputComponent.h"
#include "LightSwitchInputComponent.h"
#include "LightComponent.h"
#include "ShadowComponent.h"
#include "SkyboxComponent.h"
#include "MotionComponent.h"
#include "PhysicsComponent.h"
#include "ForceComponent.h"
#include "GravityForceGenerator.h"
#include "SpringForceGenerator.h"
#include "DragForceGenerator.h"
#include "CollisionComponent.h"
#include "SphereCollisionComponent.h"
#include "BoxCollisionComponent.h"
#include "PlaneCollisionComponent.h"

#include "ModelLoader.h"
#include "Material.h"
#include "GeometryGenerator.h"

#include "Picker.h"

/**** play game state ****/
void PlayGameState::OnEntry()
{
	/**** create game GUI ****/

	GUIPanel *panel = new GUIPanel();
	panel->SetRelativeDimensions(XMFLOAT2(0.2f, 1.0f));
	panel->SetRelativePositionTL(XMFLOAT2(0.0f, 0.0f));
	panel->SetColor(XMFLOAT3(0.0f, 0.0f, 0.0f));
	panel->SetOpacity(0.3f);

	GUIButton *button1 = new GUIButton(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	panel->AddChild(button1);
	button1->SetRelativeDimensions(XMFLOAT2(0.35f, 0.15f));
	button1->SetRelativePositionCenter(XMFLOAT2(0.3f, 0.8f));
	button1->SetTexture(&TextureManager::GetInstance().GetTexture("res/button.png"));
	button1->SetName("menu");
	button1->AddButtonListener(GameFSM::GetInstance(), &GameFSM::OnButtonPressed);

	GUILabel *label = new GUILabel("Menu");
	button1->AddChild(label);
	label->SetRelativeDimensions(XMFLOAT2(0.5f, 0.1f));
	label->SetRelativePositionCenter(XMFLOAT2(0.5, 0.5));
	
	GUIButton *button2 = new GUIButton(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	panel->AddChild(button2);
	button2->SetRelativeDimensions(XMFLOAT2(0.35f, 0.15f));
	button2->SetRelativePositionCenter(XMFLOAT2(0.7f, 0.8f));
	button2->SetTexture(&TextureManager::GetInstance().GetTexture("res/button.png"));
	button2->SetName("next");
	button2->AddButtonListener(GameFSM::GetInstance(), &GameFSM::OnButtonPressed);

	GUILabel *label2 = new GUILabel("Next");
	button2->AddChild(label2);
	label2->SetRelativeDimensions(XMFLOAT2(0.5f, 0.1f));
	label2->SetRelativePositionCenter(XMFLOAT2(0.5, 0.5));


	/*GUITextField *textField = new GUITextField;
	panel->AddChild(textField);
	textField->SetRelativeDimensions(XMFLOAT2(0.4f, 0.1f));
	textField->SetBackgroundColor(XMFLOAT3(0.4f, 0.4f, 0.4f));
	textField->SetOpacity(0.1f);
	textField->SetRelativePositionTL(XMFLOAT2(0.5f, 0.1f));

	GUILabel *label4 = new GUILabel("Insert Nickname:");
	panel->AddChild(label4);
	label4->SetRelativeDimensions(XMFLOAT2(0.5f, 0.1f));
	label4->SetRelativePositionTR(XMFLOAT2(0.5, 0.1));*/

	mGameGUI = new GUI(panel);  // registers itself to systems
	mGameGUI->SetVisible(true);

	/**** create game entities ****/

	// directional light 
	Entity &light = EntitySystem::GetInstance().AddEntity();
	PositionComponent &positionComponent1 = light.AddComponent<PositionComponent>(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XMConvertToRadians(45.0f), XMConvertToRadians(45.0f), XMConvertToRadians(0.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	light.AddComponent<LightComponent>(LightComponent::Type::DIRECTIONAL, XMFLOAT3(1.0f, 1.0f, 1.0f), 0.0f, 0.7f, 45.0f, &positionComponent1);
	//light.AddComponent<InputComponent>(&positionComponent1);

	// skybox
	Entity &skyBox = EntitySystem::GetInstance().AddEntity();
	skyBox.AddComponent<PositionComponent>(XMFLOAT3(), XMFLOAT3(), XMFLOAT3(1.0f, 1.0f, 1.0f));
	skyBox.AddComponent<StaticMeshComponent>(*GeometryGenerator::GenerateSkybox("res/sky.dds"));
	skyBox.AddComponent<SkyboxComponent>();

	// player + camera
	Entity &player = EntitySystem::GetInstance().AddEntity();

	Material playerMaterial;
	playerMaterial.AddDiffuseMap("materials/futuristic panel/futuristic_diffuse.png");
	playerMaterial.AddSpecularMap("materials/futuristic panel/futuristic_metallic.png");
	playerMaterial.AddNormalMap("materials/futuristic panel/futuristic_normal.png");

	player.AddComponent<StaticMeshComponent>(*GeometryGenerator::GenerateSphere(1.0f, playerMaterial));
	PositionComponent &positionComponentP = player.AddComponent<PositionComponent>(XMFLOAT3(0.0f, 6.0f, -20.0f), XMFLOAT3(XMConvertToRadians(30.0f), XMConvertToRadians(0.0f), XMConvertToRadians(0.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	CameraComponent &cameraComponent = player.AddComponent<CameraComponent>(XMConvertToRadians(45.0f), 3.0f / 2.0f, 0.1f, 500.0f, &positionComponentP, XMFLOAT3(0.0f, 3.0f, -15.0f), XMFLOAT3(XMConvertToRadians(0.0f), XMConvertToRadians(0.0f), XMConvertToRadians(0.0f)));
	cameraComponent.SetActive(true);
	player.AddComponent<LightSwitchInputComponent,InputComponent>(&positionComponentP);
	player.AddComponent<ShadowComponent>();
	player.AddComponent<LightComponent>(LightComponent::Type::SPOT, XMFLOAT3(0.9f, 0.9f, 1.0f), 100.0f, 50.0f, 45.0f, &positionComponentP);
	player.AddComponent<SphereCollisionComponent, CollisionComponent>(1.0f);
	player.AddComponent<MotionComponent>();

	PhysicsComponent &physicsComponent = player.AddComponent<PhysicsComponent>();
	physicsComponent.SetMass(10.0f);

	XMFLOAT3X3 inertiaTensorP{};

	inertiaTensorP._11 = physicsComponent.GetMass() * (0.1f * 0.1f) * 2.0f / 5.0f;
	inertiaTensorP._22 = physicsComponent.GetMass() * (0.1f * 0.1f) * 2.0f / 5.0f;
	inertiaTensorP._33 = physicsComponent.GetMass() * (0.1f * 0.1f) * 2.0f / 5.0f;

	physicsComponent.SetInertiaTensor(inertiaTensorP);

	// sphere 1
	Entity &sphere1 = EntitySystem::GetInstance().AddEntity();
	
	Material sphere1Material;
	sphere1Material.AddDiffuseMap("materials/rusted iron/rust_diffuse.png");
	sphere1Material.AddSpecularMap("materials/rusted iron/rust_metallic.png");
	sphere1Material.AddNormalMap("materials/rusted iron/rust_normal.png");

	sphere1.AddComponent<StaticMeshComponent>(*GeometryGenerator::GenerateSphere(1.0f, sphere1Material));
	PositionComponent &positionComponent0 = sphere1.AddComponent<PositionComponent>(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XMConvertToRadians(90.0f), XMConvertToRadians(0.0f), XMConvertToRadians(0.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	sphere1.AddComponent<MotionComponent>();
	sphere1.AddComponent<ShadowComponent>();
	sphere1.AddComponent<LightComponent>(LightComponent::Type::SPOT, XMFLOAT3(1.0f, 0.2f, 0.9f), 100.0f, 1.0f, 45.0f, &positionComponent0);

	PhysicsComponent &physicsComponent1 = sphere1.AddComponent<PhysicsComponent>();
	physicsComponent1.SetMass(10.0f);

	XMFLOAT3X3 inertiaTensor0{};

	inertiaTensor0._11 = physicsComponent1.GetMass() * (0.1f * 0.1f) * 2.0f / 5.0f;
	inertiaTensor0._22 = physicsComponent1.GetMass() * (0.1f * 0.1f) * 2.0f / 5.0f;
	inertiaTensor0._33 = physicsComponent1.GetMass() * (0.1f * 0.1f) * 2.0f / 5.0f;

	physicsComponent1.SetInertiaTensor(inertiaTensor0);

	ForceComponent &forceComponent1 = sphere1.AddComponent<ForceComponent>();
	forceComponent1.AddForceGenerator(new GravityForceGenerator(10.0f));

	sphere1.AddComponent<SphereCollisionComponent, CollisionComponent>(1.0f);

	// sphere 2
	Entity &sphere2 = EntitySystem::GetInstance().AddEntity();

	Material sphere2Material;
	sphere2Material.AddDiffuseMap("res/checkers.jpg");
	sphere2Material.SetSpecularColor(XMFLOAT3(1.0f, 1.0f, 1.0f));

	sphere2.AddComponent<StaticMeshComponent>(*GeometryGenerator::GenerateSphere(1.0f, sphere2Material));
	PositionComponent &positionComponent2 = sphere2.AddComponent<PositionComponent>(XMFLOAT3(0.0f, 5.0f, 1.0f), XMFLOAT3(XMConvertToRadians(90.0f), XMConvertToRadians(0.0f), XMConvertToRadians(0.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	sphere2.AddComponent<MotionComponent>(XMFLOAT3(2.0f, 0.0f, 0.0f));
	sphere2.AddComponent<ShadowComponent>();
	sphere2.AddComponent<LightComponent>(LightComponent::Type::SPOT, XMFLOAT3(1.0f, 0.1f, 0.0f), 100.0f, 60.0f, 45.0f, &positionComponent2);
	sphere2.AddComponent<LightComponent>(LightComponent::Type::SPOT, XMFLOAT3(0.0f, 0.1f, 1.0f), 100.0f, 60.0f, 45.0f, &positionComponent2);

	PhysicsComponent &pphysicsComponent2 = sphere2.AddComponent<PhysicsComponent>();
	pphysicsComponent2.SetMass(20.0f);

	XMFLOAT3X3 inertiaTensor12{};

	inertiaTensor12._11 = pphysicsComponent2.GetMass() * (0.1f * 0.1f) * 2.0f / 5.0f;
	inertiaTensor12._22 = pphysicsComponent2.GetMass() * (0.1f * 0.1f) * 2.0f / 5.0f;
	inertiaTensor12._33 = pphysicsComponent2.GetMass() * (0.1f * 0.1f) * 2.0f / 5.0f;

	pphysicsComponent2.SetInertiaTensor(inertiaTensor12);

	ForceComponent &forceComponent2 = sphere2.AddComponent<ForceComponent>();
	forceComponent2.AddForceGenerator(new GravityForceGenerator(10.0f));
	forceComponent2.AddForceGenerator(new DragForceGenerator(1.0f, 0.0f));

	sphere2.AddComponent<SphereCollisionComponent, CollisionComponent>(1.0f);

	// box 1
	Entity &box1 = EntitySystem::GetInstance().AddEntity();
	
	Material box1Material;
	box1Material.AddDiffuseMap("materials/concrete/concrete_diffuse.png");
	box1Material.AddNormalMap("materials/concrete/concrete_normal.png");
	box1Material.AddSpecularMap("materials/concrete/concrete_metallic.png");
	
	box1.AddComponent<StaticMeshComponent>(*GeometryGenerator::GenerateBox(XMFLOAT3(2.0f, 4.0f, 3.0f), box1Material));
	box1.AddComponent<PositionComponent>(XMFLOAT3(0.0f, 10.0f, 10.0f), XMFLOAT3(XMConvertToRadians(90.0f), XMConvertToRadians(0.0f), XMConvertToRadians(0.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	box1.AddComponent<MotionComponent>();
	box1.AddComponent<ShadowComponent>();
	
	PhysicsComponent &physicsComponent3 = box1.AddComponent<PhysicsComponent>();
	physicsComponent3.SetMass(500.0f);

	XMFLOAT3X3 inertiaTensor00{};

	inertiaTensor00._11 = physicsComponent3.GetMass() * (4.0f * 4.0f + 3.0f * 3.0f) / 3.0f;
	inertiaTensor00._22 = physicsComponent3.GetMass() * (2.0f * 2.0f + 3.0f * 3.0f) / 3.0f;;
	inertiaTensor00._33 = physicsComponent3.GetMass() * (2.0f * 2.0f + 4.0f * 4.0f) / 3.0f;;

	physicsComponent3.SetInertiaTensor(inertiaTensor00);

	ForceComponent &forceComponent3 = box1.AddComponent<ForceComponent>();
	forceComponent3.AddForceGenerator(new GravityForceGenerator(10.0f));
	forceComponent3.AddForceGenerator(new DragForceGenerator(0.3f, 0.3f));

	box1.AddComponent<BoxCollisionComponent, CollisionComponent>(XMFLOAT3(2.0f, 4.0f, 3.0f));

	// box 2
	Entity &box2 = EntitySystem::GetInstance().AddEntity();
	
	Material box2Material;
	box2Material.AddDiffuseMap("materials/snow/snow_diffuse.png");
	box2Material.AddSpecularMap("materials/snow/snow_metallic.png");
	box2Material.AddNormalMap("materials/snow/snow_normal.png");
	
	box2.AddComponent<StaticMeshComponent>(*GeometryGenerator::GenerateBox(XMFLOAT3(1.0f, 1.0f, 1.0f), box2Material));
	
	PositionComponent &positionComponent3 = box2.AddComponent<PositionComponent>(XMFLOAT3(4.0f, 12.0f, 10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	box2.AddComponent<ShadowComponent>();
	box2.AddComponent<ArrowInputComponent, InputComponent>(&positionComponent3);

	// box 3
	Entity &box3 = EntitySystem::GetInstance().AddEntity();
	
	Material box3Material;
	box3Material.AddDiffuseMap("materials/plastic/plastic_diffuse.png");
	box3Material.AddSpecularMap("materials/plastic/plastic_metallic.png");
	box3Material.AddNormalMap("materials/plastic/plastic_normal.png");
	
	box3.AddComponent<StaticMeshComponent>(*GeometryGenerator::GenerateBox(XMFLOAT3(2.0f, 0.2f, 3.0f), box3Material));
	box3.AddComponent<PositionComponent>(XMFLOAT3(4.0f, 6.0f, 10.0f), XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	box3.AddComponent<ShadowComponent>();
	box3.AddComponent<MotionComponent>(XMFLOAT3(0.0f, 0.0f, 0.0f));
	PhysicsComponent &physicsComponent4 = box3.AddComponent<PhysicsComponent>();
	physicsComponent4.SetMass(50.0f);

	XMFLOAT3X3 inertiaTensor{};

	inertiaTensor._11 = physicsComponent4.GetMass() * (0.2f * 0.2f + 3.0f * 3.0f) / 3.0f;
	inertiaTensor._22 = physicsComponent4.GetMass() * (2.0f * 2.0f + 3.0f * 3.0f) / 3.0f;;
	inertiaTensor._33 = physicsComponent4.GetMass() * (2.0f * 2.0f + 0.2f * 0.2f) / 3.0f;;

	physicsComponent4.SetInertiaTensor(inertiaTensor);

	ForceComponent &forceComponent4 = box3.AddComponent<ForceComponent>();
	forceComponent4.AddForceGenerator(new GravityForceGenerator(10.0f));
	forceComponent4.AddForceGenerator(new SpringForceGenerator(&box2.GetComponent<PositionComponent>()->GetPosition(), XMFLOAT3(2.0f, 0.0f, 0.0f), 150.0f, 5.0f, 40.0f));
	forceComponent4.AddForceGenerator(new DragForceGenerator(1.0f, 4.0f));
	box3.AddComponent<BoxCollisionComponent, CollisionComponent>(XMFLOAT3(2.0f, 0.2f, 3.0f));

	// floor 
	Entity &plane = EntitySystem::GetInstance().AddEntity();
	
	Material planeMaterial;
	planeMaterial.AddDiffuseMap("materials/oakfloor/oakfloor_diffuse.png");
	planeMaterial.AddNormalMap("materials/oakfloor/oakfloor_normal.png");
	planeMaterial.SetSpecularColor(XMFLOAT3(0.25f, 0.2f, 0.2f));

	plane.AddComponent<StaticMeshComponent>(*GeometryGenerator::GeneratePlane(100.0f, 100.0f, planeMaterial));

	plane.AddComponent<PositionComponent>(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	CollisionComponent &c = plane.AddComponent<PlaneCollisionComponent, CollisionComponent>(XMFLOAT3(0.0f, 1.0f, 0.0f));
	c.SetMovable(false);

	// left side wall 
	Entity &side = EntitySystem::GetInstance().AddEntity();

	Material sideMaterial;
	sideMaterial.AddDiffuseMap("materials/blue tiles/blue tiles diffuse.png");
	sideMaterial.AddNormalMap("materials/blue tiles/blue tiles normal.png");
	sideMaterial.SetSpecularColor(XMFLOAT3(0.8f, 0.7f, 1.0f));
	sideMaterial.SetTiling(2.0f, 4.0f);

	side.AddComponent<StaticMeshComponent>(*GeometryGenerator::GeneratePlane(20.0f, 100.0f, sideMaterial));

	side.AddComponent<PositionComponent>(XMFLOAT3(-50.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(-90.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	CollisionComponent &collisionComponent1 = side.AddComponent<PlaneCollisionComponent, CollisionComponent>(XMFLOAT3(1.0f, 0.0f, 0.0f));
	collisionComponent1.SetMovable(false);

	// right side wall 
	Entity &sideR = EntitySystem::GetInstance().AddEntity();

	sideR.AddComponent<StaticMeshComponent>(*GeometryGenerator::GeneratePlane(20.0f, 100.0f, sideMaterial));

	sideR.AddComponent<PositionComponent>(XMFLOAT3(50.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	CollisionComponent &collisionComponent3 = sideR.AddComponent<PlaneCollisionComponent, CollisionComponent>(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	collisionComponent3.SetMovable(false);

	// front wall 
	Entity &front = EntitySystem::GetInstance().AddEntity();

	Material frontMaterial;
	frontMaterial.AddDiffuseMap("materials/dark tiles/dark tiles diffuse.png");
	frontMaterial.AddNormalMap("materials/dark tiles/dark tiles normal.png");
	frontMaterial.SetSpecularColor(XMFLOAT3(0.8f, 0.8f, 1.0f));
	frontMaterial.SetTiling(4.0f, 2.0f);

	front.AddComponent<StaticMeshComponent>(*GeometryGenerator::GeneratePlane(100.0f, 20.0f, frontMaterial));

	front.AddComponent<PositionComponent>(XMFLOAT3(0.0f, 10.0f, -50.0f), XMFLOAT3(XMConvertToRadians(90.0f), 0.0f, XMConvertToRadians(0.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	CollisionComponent &collisionComponent4 = front.AddComponent<PlaneCollisionComponent, CollisionComponent>(XMFLOAT3(0.0f, 0.0f, 1.0f));
	collisionComponent4.SetMovable(false);

	// end wall 
	Entity &end = EntitySystem::GetInstance().AddEntity();

	end.AddComponent<StaticMeshComponent>(*GeometryGenerator::GeneratePlane(100.0f, 20.0f, frontMaterial));

	end.AddComponent<PositionComponent>(XMFLOAT3(0.0f, 10.0f, 50.0f), XMFLOAT3(XMConvertToRadians(-90.0f), 0.0f, XMConvertToRadians(0.0f)), XMFLOAT3(1.0f, 1.0f, 1.0f));
	CollisionComponent &collisionComponent2 = end.AddComponent<PlaneCollisionComponent, CollisionComponent>(XMFLOAT3(0.0f, 0.0f, -1.0f));
	collisionComponent2.SetMovable(false);

	// 3D model
	Entity &crate = EntitySystem::GetInstance().AddEntity();
	crate.AddComponent<PositionComponent>(XMFLOAT3(-10.0f, 2.0f, 0.0f), XMFLOAT3(0.0f, XMConvertToRadians(180.0f), 0.0f), XMFLOAT3(0.02f, 0.02f, 0.02f));
	crate.AddComponent<StaticMeshComponent>(*ModelLoader::GetInstance().LoadStaticModel("models/crate/crate.obj"));
	crate.AddComponent<ShadowComponent>();

	mEntities.InsertLast(&light);
	mEntities.InsertLast(&player);
	mEntities.InsertLast(&sphere1);
	mEntities.InsertLast(&sphere2);
	mEntities.InsertLast(&box1);
	mEntities.InsertLast(&box2);
	mEntities.InsertLast(&box3);
	mEntities.InsertLast(&plane);
	mEntities.InsertLast(&side);
	mEntities.InsertLast(&sideR);
	mEntities.InsertLast(&front);
	mEntities.InsertLast(&skyBox);
	mEntities.InsertLast(&crate);

	Entity *camera = EntitySystem::GetInstance().GetCamera();
	mPicker = new Picker(camera->GetComponent<CameraComponent>());
}

void PlayGameState::OnButtonPressed(GameFSM &fsm, const std::string &buttonName)
{
	if (buttonName == "menu")
		fsm.ChangeState(&GameState::mMenuGameState);
}

void PlayGameState::OnKeyPress(GameFSM &fsm, int key)
{
	if (key == 'p')
		fsm.PushState(&GameState::mPauseGameState);

	if (key == VK_ESCAPE)
		fsm.PushState(&GameState::mExitGameState);
}

void PlayGameState::OnExit()
{
	mGameGUI->Destroy();
	mGameGUI = nullptr;

	for (Entity *entity : mEntities)
		entity->Destroy();

	mEntities.Clear();

	delete mPicker;
}

/**** pause game state ****/
void PauseGameState::OnEntry()
{
	GUIPanel *panel = new GUIPanel();
	panel->SetRelativeDimensions(XMFLOAT2(0.5f, 0.5f));
	panel->SetRelativePositionCenter(XMFLOAT2(0.7f, 0.3f));
	panel->SetColor(XMFLOAT3(1.0f, 0.7f, 0.3f));

	GUILabel *label = new GUILabel("PAUSE");
	panel->AddChild(label);
	label->SetRelativeDimensions(XMFLOAT2(0.5f, 0.1f));
	label->SetRelativePositionCenter(XMFLOAT2(0.5, 0.2));

	GUIButton *button1 = new GUIButton(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	panel->AddChild(button1);
	button1->SetRelativeDimensions(XMFLOAT2(0.35f, 0.15f));
	button1->SetRelativePositionCenter(XMFLOAT2(0.5f, 0.8f));
	button1->SetTexture(&TextureManager::GetInstance().GetTexture("res/button.png"));
	button1->SetName("return");
	button1->AddButtonListener(GameFSM::GetInstance(), &GameFSM::OnButtonPressed);

	GUILabel *label1 = new GUILabel("back to game");
	button1->AddChild(label1);
	label1->SetRelativeDimensions(XMFLOAT2(0.5f, 0.1f));
	label1->SetRelativePositionCenter(XMFLOAT2(0.5, 0.5));

	GUIButton *button2 = new GUIButton(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	panel->AddChild(button2);
	button2->SetRelativeDimensions(XMFLOAT2(0.35f, 0.15f));
	button2->SetRelativePositionCenter(XMFLOAT2(0.5f, 0.6f));
	button2->SetTexture(&TextureManager::GetInstance().GetTexture("res/button.png"));
	button2->SetName("quit");
	button2->AddButtonListener(GameFSM::GetInstance(), &GameFSM::OnButtonPressed);

	GUILabel *label2 = new GUILabel("quit");
	button2->AddChild(label2);
	label2->SetRelativeDimensions(XMFLOAT2(0.5f, 0.1f));
	label2->SetRelativePositionCenter(XMFLOAT2(0.5, 0.5));

	mPauseMenu = new GUI(panel);  // registers itself to systems
	mPauseMenu->SetVisible(true);
}

void PauseGameState::OnButtonPressed(GameFSM &fsm, const std::string &buttonName)
{
	if (buttonName == "return")
		fsm.PopState();

	if (buttonName == "quit")
		fsm.PushState(&GameState::mExitGameState); 
}

void PauseGameState::OnExit()
{
	mPauseMenu->Destroy();
	mPauseMenu = nullptr;
}

/**** exit game state ****/
void ExitGameState::OnEntry()
{
	GUIPanel *panel = new GUIPanel();
	panel->SetRelativeDimensions(XMFLOAT2(0.5f, 0.5f));
	panel->SetRelativePositionCenter(XMFLOAT2(0.5f, 0.4f));
	panel->SetColor(XMFLOAT3(0.5, 0.4, 0.3));

	GUILabel *label = new GUILabel("Are you sure\nyou want to QUIT?");
	panel->AddChild(label);
	label->SetRelativeDimensions(XMFLOAT2(0.5f, 0.1f));
	label->SetRelativePositionCenter(XMFLOAT2(0.5, 0.2));


	GUIButton *button1 = new GUIButton(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	panel->AddChild(button1);
	button1->SetRelativeDimensions(XMFLOAT2(0.35f, 0.15f));
	button1->SetRelativePositionTL(XMFLOAT2(0.1f, 0.8f));
	button1->SetTexture(&TextureManager::GetInstance().GetTexture("res/button.png"));
	button1->SetName("yes");
	button1->AddButtonListener(GameFSM::GetInstance(), &GameFSM::OnButtonPressed);

	GUILabel *label1 = new GUILabel("yes, please");
	button1->AddChild(label1);
	label1->SetRelativeDimensions(XMFLOAT2(0.5f, 0.1f));
	label1->SetRelativePositionCenter(XMFLOAT2(0.5, 0.5));

	GUIButton *button2 = new GUIButton(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	panel->AddChild(button2);
	button2->SetRelativeDimensions(XMFLOAT2(0.35f, 0.15f));
	button2->SetRelativePositionTR(XMFLOAT2(0.9f, 0.8f));
	button2->SetTexture(&TextureManager::GetInstance().GetTexture("res/button.png"));
	button2->SetName("no");
	button2->AddButtonListener(GameFSM::GetInstance(), &GameFSM::OnButtonPressed);

	GUILabel *label2 = new GUILabel("No way!");
	button2->AddChild(label2);
	label2->SetRelativeDimensions(XMFLOAT2(0.5f, 0.1f));
	label2->SetRelativePositionCenter(XMFLOAT2(0.5, 0.5));


	mExitMenu = new GUI(panel);  // registers itself to systems
	mExitMenu->SetVisible(true);
}

void ExitGameState::OnButtonPressed(GameFSM &fsm, const std::string &buttonName)
{
	if (buttonName == "yes")
		PostQuitMessage(0);

	if (buttonName == "no")
		fsm.PopState();
}

void ExitGameState::OnExit()
{
	mExitMenu->Destroy();
	mExitMenu = nullptr;
}