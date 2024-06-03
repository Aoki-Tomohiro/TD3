#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/EnemyManager.h"
#include "Application/Src/Object/Block/BlockManager.h"
#include "Application/Src/Object/Player/CopyManager.h"
#include "Application/Src/Object/BackGround/BackGround.h"
#include "Application/Src/Object/Score/Score.h"
#include "Application/Src/Object/Switch/SwitchManager.h"

class TutorialScene2 : public IScene
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
	void Reset();

	void Reverse();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//カメラ
	Camera camera_{};

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//プレイヤー
	Model* playerModel_ = nullptr;
	Model* weaponModel_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;

	//敵
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;

	//ブロック
	Model* blockModel_ = nullptr;
	std::unique_ptr<BlockManager> blockManager_ = nullptr;

	//コピー
	std::unique_ptr<CopyManager> copyManager_ = nullptr;

	//背景
	std::unique_ptr<BackGround> backGround_ = nullptr;

	//スコア
	std::unique_ptr<Score> score_ = nullptr;

	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//逆再生中か
	bool isReversed_ = false;
	std::vector<std::tuple<Vector3, bool, uint32_t, float>> reversePlayerPositions{};

	//オーディオハンドル
	uint32_t whiffAudioHandle_ = 0;
	uint32_t tutorialBGMHandle_ = 0;

	//巻き戻しのデータスキップ数
	uint32_t stepSize_ = 4;

	//倍速中か
	bool isDoubleSpeed_ = false;

	//チュートリアルのスプライト
	std::unique_ptr<Sprite> tutorialSprite_ = nullptr;
	std::unique_ptr<Sprite> numberSprite_ = nullptr;
	Vector2 tutorialSpritePosition_{ 446.0f,25.0f };
	Vector2 tutorialSpriteScale_{ 0.6f,0.6f };
	Vector2 numberSpritePosition_{ 780.0f,25.0f };
	Vector2 numberSpriteScale_{ 0.6f,0.6f };

	//トランジションのフラグ
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	float timer_ = 0.0f;

	//スイッチ
	std::unique_ptr<SwitchManager> switchManager_ = nullptr;

	//倍速の時のスプライト
	std::unique_ptr<Sprite> doubleSprite_ = nullptr;
	//巻き戻し時のスプライト
	std::unique_ptr<Sprite> reversedSprite_ = nullptr;

	//スコア説明
	std::unique_ptr <Sprite> ruleSprite_ = nullptr;
	Vector2 rulePos_ = {320.0f,285.0f};
	Vector2 ruleSize_ = { 0.3f,0.3f };

	//UI
	std::unique_ptr<Sprite> botanUI_ = nullptr;
	std::unique_ptr<Sprite> rbUI_ = nullptr;

	//敵を倒すチュートリアルのスプライト
	std::unique_ptr<Sprite> knockDownsprite_ = nullptr;
	uint32_t knockDownAnimationNumber_ = 0;
	uint32_t knockDownFrameCounter_ = 0;
	Vector2 knockDownPosition_{ 1022.0f,415.0f};
	Vector2 knockDownScale_{ 0.6f,0.6f };
};

