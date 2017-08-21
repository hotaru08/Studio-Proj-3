#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox/SkyBoxEntity.h"

#include "RenderHelper.h"


#include <iostream>
using namespace std;

SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
	: theMinimap(NULL),
	theMouse(NULL),
	theKeyboard(NULL)
{
}

SceneText::SceneText(SceneManager* _sceneMgr)
	: theMinimap(NULL),
	theMouse(NULL),
	theKeyboard(NULL)
{
	_sceneMgr->AddScene("Start", this);
}

SceneText::~SceneText()
{
	if (theMinimap)
	{
		delete theMinimap;
		theMinimap = NULL;
	}
	if (theMouse)
	{
		delete theMouse;
		theMouse = NULL;
	}
	if (theKeyboard)
	{
		delete theKeyboard;
		theKeyboard = NULL;
	}
}

void SceneText::Init()
{
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");
	
	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");
	
	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, 20, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	lights[1] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[1]", lights[1]);
	lights[1]->type = Light::LIGHT_DIRECTIONAL;
	lights[1]->position.Set(1, 1, 0);
	lights[1]->color.Set(1, 1, 0.5f);
	lights[1]->power = 0.4f;
	lights[1]->name = "lights[1]";

	currProg->UpdateInt("numLights", 1);
	currProg->UpdateInt("textEnabled", 0);
	
	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();


//	std::cout << _DEBUG << std::endl;
	int a;

	// Create and attach the camera to the scene
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");
	MeshBuilder::GetInstance()->GenerateCrossHair("crosshair");
	MeshBuilder::GetInstance()->GenerateQuad("quad", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("quad")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
	MeshBuilder::GetInstance()->GenerateRing("ring", Color(1, 0, 1), 36, 1, 0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("sphere", Color(1, 0, 0), 18, 36, 1.0f);
	MeshBuilder::GetInstance()->GenerateSphere("Troopbullet", Color(0, 0, 1), 18, 36, 2.f);
	MeshBuilder::GetInstance()->GenerateCone("cone", Color(0.5f, 1, 0.3f), 36, 10.f, 10.f);
	MeshBuilder::GetInstance()->GenerateCube("cube", Color(1.0f, 1.0f, 0.0f), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kDiffuse.Set(0.99f, 0.99f, 0.99f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kSpecular.Set(0.f, 0.f, 0.f);
	MeshBuilder::GetInstance()->GenerateCube("testTroop", Color(1.f, 0.5f, 0.4f), 1.0f);
	MeshBuilder::GetInstance()->GenerateOBJ("testTroop", "OBJ//PlayerTrooperOBJ.obj");
	MeshBuilder::GetInstance()->GetMesh("testTroop")->textureID = LoadTGA("Image//Troop_TextureTGA.tga");

	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_FRONT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BACK", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_LEFT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_RIGHT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_TOP", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BOTTOM", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_FRONT")->textureID = LoadTGA("Image//SkyBox//boxFront.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BACK")->textureID = LoadTGA("Image//SkyBox//boxBack.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_LEFT")->textureID = LoadTGA("Image//SkyBox//boxLeft.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_RIGHT")->textureID = LoadTGA("Image//SkyBox//boxRight.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_TOP")->textureID = LoadTGA("Image//SkyBox//boxUp.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BOTTOM")->textureID = LoadTGA("Image//SkyBox//boxDown.tga");

	// Create entities into the scene
	Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	Create::Entity("lightball", Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball



	groundEntity = Create::Ground("SKYBOX_BOTTOM", "SKYBOX_BOTTOM");
	Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
											 "SKYBOX_LEFT", "SKYBOX_RIGHT",
											 "SKYBOX_TOP", "SKYBOX_BOTTOM");

	// Customise the ground entity
	groundEntity->SetPosition(Vector3(0, 0, 0));
	groundEntity->SetScale(Vector3(100.0f, 150.0f, 150.0f));
	groundEntity->SetGrids(Vector3(10.0f, 1.0f, 10.0f));
	playerInfo->SetTerrain(groundEntity);

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	for (int i = 0; i < 1; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f,1.0f,0.0f));
	}

	//Minimap
	theMinimap = Create::Minimap(false);
	theMinimap->SetBackground(MeshBuilder::GetInstance()->GenerateQuad("Minimap", Color(1, 1, 1), 1.f));
	theMinimap->GetBackground()->textureID = LoadTGA("Image//SkyBox//boxDown.tga");
	theMinimap->SetBorder(MeshBuilder::GetInstance()->GenerateCircle("MinimapBorder", Color(1, 1, 1), 1.05f));
	theMinimap->SetAvatar(MeshBuilder::GetInstance()->GenerateQuad("MinimapAvatar", Color(1, 1, 1), 0.25f));
	theMinimap->GetAvatar()->textureID = LoadTGA("Image//Avatar.tga");
	theMinimap->SetStencil(MeshBuilder::GetInstance()->GenerateCircle("MinimapStencil", Color(1, 1, 1), 1.f));

	theKeyboard = new CKeyboard();
	theKeyboard->Create(playerInfo);
	theMouse = new CMouse();
	theMouse->Create(playerInfo);

	testTroop[0] = Create::Enemy3D("testTroop", Vector3(0, 10, 200), Vector3(1, 1, 1));
	testTroop[0]->Init();
	testTroop[0]->SetTerrain(groundEntity);
	testTroop[0]->SetType(1);
	testTroop[0]->SetDestination(Vector3(0, 10, 0));

	testTroop[1] = Create::Enemy3D("testTroop", Vector3(40, 10, 200), Vector3(1, 1, 1));
	testTroop[1]->Init();
	testTroop[1]->SetTerrain(groundEntity);
	testTroop[1]->SetType(1);
	testTroop[1]->SetDestination(Vector3(0, 10, 0));

	testTroop[2] = Create::Enemy3D("testTroop", Vector3(0, 10, -100), Vector3(1, 1, 1));
	testTroop[2]->Init();
	testTroop[2]->SetTerrain(groundEntity);
	testTroop[2]->SetType(1);
	testTroop[2]->SetDestination(Vector3(0, 10, 0));


	for (int i = 0; i < 20; ++i)
	{
		newTroop[i] = Create::Enemy3D("testTroop", Vector3(Math::RandFloatMinMax(-200.f, 200.f), 10, Math::RandFloatMinMax(-200.f, 200.f)), Vector3(1, 1, 1));
		newTroop[i]->Init();
		newTroop[i]->SetTerrain(groundEntity);
		newTroop[i]->SetType(1);
		newTroop[i]->SetDestination(Vector3(0, 10, 0));
	}

	// Enemy's Turrets
	for (int i = 0; i < 20; ++i)
	{
		turret[i] = Create::Enemy3D("sphere", Vector3(Math::RandFloatMinMax(-200.f, 200.f), 10, Math::RandFloatMinMax(-200.f, 200.f)), Vector3(4, 4, 4),2);
		turret[i]->Init();
		turret[i]->SetTerrain(groundEntity);
		turret[i]->SetType(2);
	}
}

void SceneText::Update(double dt)
{
	
	if (KeyboardController::GetInstance()->IsKeyDown(VK_SPACE))
	{
		CEnemy3D* newTroop;
		newTroop = Create::Enemy3D("testTroop", Vector3(Math::RandFloatMinMax(-200.f, 200.f), 10, Math::RandFloatMinMax(-200.f, 200.f)), Vector3(1, 1, 1));
		newTroop->Init();
		newTroop->SetTerrain(groundEntity);
		newTroop->SetType(1);
		newTroop->SetDestination(Vector3(0, 10, 0));
		spawnedTroops++;
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('8'))
	{
		//Setting a designated position for all troops
		Vector3 estimatedDestination[22];
		testTroop[0]->SetDestination(Vector3(Math::RandFloatMinMax(-200.f, 200.f), 10, Math::RandFloatMinMax(-200.f, 200.f)));
		testTroop[0]->SetActionDone(false);

		for (int i = 0; i < 22; i++)
		{
			bool tooClose = false;
			estimatedDestination[i].Set(testTroop[0]->GetFinalDestination().x + Math::RandFloatMinMax(1.2 * -22, 1.2 * 22), 10, testTroop[0]->GetFinalDestination().z + Math::RandFloatMinMax(1.2 * -22,1.2 * 22));
			//check for closeness with each troops
			do
			{
				tooClose = false;
				for (int x = 0; x < i; x++)
				{
					if ((estimatedDestination[i] - estimatedDestination[x]).Length() < 5)
					{
						tooClose = true;
						estimatedDestination[i].Set(estimatedDestination[i].x + Math::RandFloatMinMax(-10, 10), 10, estimatedDestination[i].z + Math::RandFloatMinMax(-10, 10));
					}
				}
			}while(tooClose);
		}

		for (int i = 0; i < 20; i++)
		{
			newTroop[i]->SetDestination(estimatedDestination[i]);
			newTroop[i]->SetActionDone(false);
		}
		for (int i = 1; i < 3; i++)
		{
			testTroop[i]->SetDestination(estimatedDestination[i + 19]);
			testTroop[i]->SetActionDone(false);
		}
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('9'))
	{
		for (int i = 0; i < 3; i++)
		{
			testTroop[i]->SetFire(true);
			testTroop[i]->SetFireDestination(Vector3(0, 10, 100)); //This is one of the turret's position
		}
	}

	//You can see the number of spawned Troops troops here
	//std::cout << spawnedTroops << std::endl; 
	// Update our entities
	EntityManager::GetInstance()->Update(dt);

	// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
	if(KeyboardController::GetInstance()->IsKeyDown('1'))
		glEnable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('2'))
		glDisable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(KeyboardController::GetInstance()->IsKeyDown('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	if(KeyboardController::GetInstance()->IsKeyDown('5'))
	{
		lights[0]->type = Light::LIGHT_POINT;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('6'))
	{
		lights[0]->type = Light::LIGHT_DIRECTIONAL;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('7'))
	{
		lights[0]->type = Light::LIGHT_SPOT;
	}

	if(KeyboardController::GetInstance()->IsKeyDown('I'))
		lights[0]->position.z -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('K'))
		lights[0]->position.z += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('J'))
		lights[0]->position.x -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('L'))
		lights[0]->position.x += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('O'))
		lights[0]->position.y -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('P'))
		lights[0]->position.y += (float)(10.f * dt);


	//Hardware Abstraction
	if(theKeyboard)
		theKeyboard->Read(dt);

	if (theMouse)
		theMouse->Read(dt);
	// Update the player position and other details based on keyboard and mouse inputs
	playerInfo->Update(dt);

	GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity
	std::ostringstream ss;
	ss.precision(5);
	float fps = (float)(1.f / dt);
	ss << "FPS: " << fps;
	textObj[0]->SetText(ss.str());
	if (KeyboardController::GetInstance()->IsKeyPressed('0'))
	{
		SceneManager::GetInstance()->SetActiveScene("Level1");
	}
}

void SceneText::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();

	//Enable blend mode
	glEnable(GL_BLEND);

	// Setup 2D pipeline then render 2D
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();
	EntityManager::GetInstance()->RenderUI();

	//Render Minimap
	theMinimap->RenderUI();

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	//Do you rendering on screen here. Centre of screen is (0,0)
	glDisable(GL_BLEND);

}

void SceneText::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE==1
		cout << "Unable to drop PlayerInfo class" << endl;
#endif
	}
	// Delete the lights
	//delete lights[0];
	//delete lights[1];
	GraphicsManager::GetInstance()->RemoveLight("lights[0]");
	GraphicsManager::GetInstance()->RemoveLight("lights[1]");
	EntityManager::GetInstance()->ClearEntityList();
	theMinimap = NULL;
	theMouse = NULL;
	theKeyboard = NULL;
}