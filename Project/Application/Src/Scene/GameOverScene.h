#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"

class GameOverScene : public IScene 
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void DrawBackGround() override;

	static void SetCopyCount(const int copyCount) { copyCount_ = copyCount; };

	void Transition();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	std::unique_ptr<Sprite> resultSprite_ = nullptr;

	uint32_t decisionHandle_ = 0;
	uint32_t overBGMHandle_ = 0;

	static int copyCount_;

	//トランジションのフラグ
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	bool isSelect_ = false;
	bool isPlay_ = false;
	float timer_ = 0.0f;
};

