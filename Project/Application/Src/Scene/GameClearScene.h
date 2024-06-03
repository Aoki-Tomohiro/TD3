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

	void DrawBackGround() override;

	static void SetTimeCount(const int copyCount) { timeCount_ = copyCount; };

	static void SetScore(const int score) { score_ = score; };

	void Transition();

	static int kEnemyNumber;
private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	std::unique_ptr<Sprite> resultSprite_ = nullptr;

	std::array<std::unique_ptr<Sprite>, 2> timeCountSprites_{};

	Vector2 timeCountSpritePositions_[2]{ {-83.0f,11.0f},{196.0f,11.0f} };

	Vector2 SpriteSize_[2] = { {3.0f,3.0f},{3.0f,3.0f} };

	std::unique_ptr<Sprite> scoreSprite_ = nullptr;

	Vector2 scoreSpritePosition_ = { -15.0f,405.0f };

	Vector2 scoreSpriteSize_ = { 330.0f,330.0f };
	
	uint32_t decisionHandle_ = 0;
	uint32_t clearBGMHandle_ = 0;

	static int timeCount_;

	static int score_;

	


	std::array<std::unique_ptr<Sprite>, 5> resultSprites_{};

	std::array<Vector2, 5> position_{};

	std::array<Vector2, 5> scale_{};

	//トランジションのフラグ
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	float timer_ = 0.0f;
};

