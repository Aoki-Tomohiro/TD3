#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"

class GameTitleScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void DrawBackGround() override;

	void Transition();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	uint32_t bgmHandle_ = 0;

	uint32_t titleBgmHandle_ = 0;

	uint32_t decisionHandle_ = 0;

	std::unique_ptr<Sprite> backGroundSprite_ = nullptr;

	std::unique_ptr<Sprite> pushASprite_ = nullptr;

	Vector2 pushASpritePosition_ = { 290.0f,500.0f };

	std::unique_ptr<Sprite> titleSprite_ = nullptr;

	//Vector2 titleSpritePosition_ = { 400.0f,120.0f };
	Vector2 titleSpritePosition_ = { 0.0f,0.0f };

	//Vector2 titleSpriteSize_ = { 441.0f,251.0f };
	Vector2 titleSpriteSize_ = { 661.0f,376.0f };

	static bool isStart_;

	//トランジションのフラグ
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	float timer_ = 0.0f;

	Camera camera_{};

	//
	Model* wallModel_ = nullptr;
	WorldTransform wallWorldTransform_{};
	Model* groundModel_ = nullptr;
	std::array<WorldTransform, 2> groundWorldTransforms_{};
	Model* playerModel_ = nullptr;
	Model* weaponModel_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<Enemy> enemy_ = nullptr;
};

