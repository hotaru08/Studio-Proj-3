#ifndef LEVEL4_H
#define LEVEL4_H

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

class ShaderProgram;
class SceneManager;
class TextEntity;
class Light;

class Level4 : public Scene
{
public:
	Level4();
	~Level4();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	Level4(SceneManager* _sceneMgr); // This is used to register to SceneManager

	ShaderProgram* currProg;
	CPlayerInfo* playerInfo;
	GroundEntity* groundEntity;
	FPSCamera camera;
	TextEntity* textObj[1];
	Light* lights[2];

	CMinimap *theMinimap;

	static Level4* sInstance; // The pointer to the object that gets registered
	CKeyboard* theKeyboard;
	CMouse* theMouse;

	float elapsed_time;
};

#endif