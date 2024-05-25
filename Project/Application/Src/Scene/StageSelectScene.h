#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Sprite.h"


class StageSelectScene : public IScene {
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void DrawBackGround() override;

	static uint32_t stageNumber_;
private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//カメラ
	//Camera camera_{};

	int selectNumber_;

	std::array<std::unique_ptr<Sprite>, 3> numberSprites_{};

	Vector2 numberPositions_[3]{ {60.0f,11.0f},{380.0f,11.0f}, {700.0f,11.0f} };

	Vector2 SpriteSize_[3] = { {3.0f,3.0f},{3.0f,3.0f},{3.0f,3.0f} };

	uint32_t decisionHandle_ = 0;

	bool isMovementEnabled_ = true;
	int MovementEnableTimer_ = 0;
};