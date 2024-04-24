#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"
#include <array>

class GameClearScene : public IScene 
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	int copyCount_ = 0;

	std::unique_ptr<Sprite> resultSprite_ = nullptr;

	std::array<std::unique_ptr<Sprite>, 2> copyCountSprites_{};

	Vector2 copyCountSpritePositions_[2]{ {800.0f,430.0f},{870.0f,430.0f} };

	std::unique_ptr<Sprite> scoreSprite_ = nullptr;

	Vector2 scoreSpritePosition_ = { 600.0f,44.0f };

	Vector2 scoreSpriteSize_ = { 130.0f,130.0f };
	
	uint32_t decisionHandle_ = 0;
};

