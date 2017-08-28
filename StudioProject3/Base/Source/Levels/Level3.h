#ifndef LEVEL3_H
#define LEVEL3_H

#include "Scene.h"
#include "Mtx44.h"
#include "../PlayerInfo/PlayerInfo.h"
#include "../GroundEntity.h"
#include "../FPSCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "../GenericEntity.h"
#include "../Enemy/Enemy3D.h"
#include "../Hardware Abstraction/Keyboard.h"
#include "../Hardware Abstraction/mouse.h"
#include "../MiniMap\Minimap.h"
#include "../CameraEffects\CameraEffects.h"
#include "../SpriteEntity.h"
#include "../Furniture\Furniture.h"
#include "../Bomb.h"

class ShaderProgram;
class SceneManager;
class TextEntity;
class Light;

class Level3 : public Scene
{
public:
	Level3();
	~Level3();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	Level3(SceneManager* _sceneMgr); // This is used to register to SceneManager

	ShaderProgram* currProg;
	CPlayerInfo* playerInfo;
	GroundEntity* groundEntity;
	FPSCamera camera;
	TextEntity* textObj[2];
	Light* lights[2];
	Vector3 test;
	Vector3 storedMiddle;
	Vector3 topLeft, botRight;
	int numberOfSelected;
	bool bMstate = false;
	bool bSelection = false;
	bool bSelected = false;

	CMinimap *theMinimap;
	EntityBase* testTrack;
	Bomb* BombTarget;
	EntityBase* IndicatorTarget;

	static Level3* sInstance; // The pointer to the object that gets registered
	CKeyboard* theKeyboard;
	CMouse* theMouse;

	float elapsed_time;
	float spawnDelay = 0.f;
	const float coolDown = 30.f;
};

#endif