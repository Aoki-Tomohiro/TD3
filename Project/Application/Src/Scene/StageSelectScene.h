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

	void Transition();

	static const uint32_t GetMaxStageCount(){ return kMaxStageCount; };

	static uint32_t stageNumber_;
	static uint32_t preSelectNumber_;
private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//カメラ
	//Camera camera_{};

	

	int selectNumber_ = 0;

	//チュートリアル数
	static const uint32_t kMaxTutorial = 1;

	//ステージ数
	static const uint32_t kMaxStages = 8;

	//ステージの最大数
	static const uint32_t kMaxStageCount = kMaxTutorial + kMaxStages + 2;

	//背景スプライト
	std::unique_ptr<Sprite> backGroundSprite_ = nullptr;

	//矢印のスプライト
	std::array<std::unique_ptr<Sprite>, 2> arrowSprites_{};
	std::array<Vector2, 2> arrowSpritePosition_{};
	std::array<Vector2, 2> arrowSpriteTargetPosition_{};
	std::array<Vector2, 2> arrowSpriteSize_{};

	//チュートリアルのスプライト
	std::array<std::unique_ptr<Sprite>, kMaxTutorial> tutorialSprites_{};
	std::array<Vector2, kMaxTutorial> tutorialSpritePosition_{};
	std::array<Vector2, kMaxTutorial> tutorialSpriteTargetPosition_{};
	std::array<Vector2, kMaxTutorial> tutorialSpriteSize_{};

	//ステージのスプライト
	std::array<std::unique_ptr<Sprite>, kMaxStages> stageSprites_{};
	std::array<Vector2, kMaxStages> stageSpritePosition_{};
	std::array<Vector2, kMaxStages> stageSpriteTargetPosition_{};
	std::array<Vector2, kMaxStages> stageSpriteSize_{};

	//数字のスプライトの描画
	std::array<std::unique_ptr<Sprite>, kMaxStages> numberSprites_{};
	std::array<Vector2, kMaxStages> numberSpritePosition_{};
	std::array<Vector2, kMaxStages> numberSpriteTargetPosition_{};
	std::array<Vector2, kMaxStages> numberSpriteSize_{};

	//ステージ画面のスプライト
	std::array<std::unique_ptr<Sprite>, kMaxTutorial + kMaxStages> stageScreenSprites_{};
	std::array<Vector2, kMaxTutorial + kMaxStages> stageScreenSpritePosition_{};
	std::array<Vector2, kMaxTutorial + kMaxStages> stageScreenSpriteTargetPosition_{};
	std::array<Vector2, kMaxTutorial + kMaxStages> stageScreenSpriteSize_{};

	//PushAスプライト
	std::unique_ptr<Sprite> pushASprite_ = nullptr;
	Vector2 pushASpritePosition_{};
	Vector2 pushASpriteScale_{};

	//Vector2 numberPositions_[3]{ {60.0f,11.0f},{380.0f,11.0f}, {700.0f,11.0f} };

	//Vector2 SpriteSize_[3] = { {3.0f,3.0f},{3.0f,3.0f},{3.0f,3.0f} };
	uint32_t selectSEHandle_ = 0;
	uint32_t decisionHandle_ = 0;
	uint32_t selectBGMHandle_ = 0;

	bool isMovementEnabled_ = true;
	int MovementEnableTimer_ = 0;
	float easingParameter_ = 0.0f;
	float delta_ = 1280.0f;

	//トランジションのフラグ
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	float timer_ = 0.0f;
};