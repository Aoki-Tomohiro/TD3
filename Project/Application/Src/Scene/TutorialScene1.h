#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/EnemyManager.h"
#include "Application/Src/Object/Block/BlockManager.h"
#include "Application/Src/Object/Player/CopyManager.h"
#include "Application/Src/Object/BackGround/BackGround.h"
#include "Application/Src/Object/FollowCamera/FollowCamera.h"
#include "Application/Src/Object/Score/Score.h"
#include "Application/Src/Object/Switch/SwitchManager.h"

class TutorialScene1 : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void DrawBackGround() override;

	void Transition();

	static uint32_t tutoBGMHandle_;
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

	//UI
	std::unique_ptr<Sprite> botanUI_ = nullptr;
	std::unique_ptr<Sprite> rbUI_ = nullptr;

	//ブロック
	Model* blockModel_ = nullptr;
	std::unique_ptr<BlockManager> blockManager_ = nullptr;

	//FollowCamera
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;

	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//背景
	std::unique_ptr<Model> backGroundGenkoModel_ = nullptr;
	std::unique_ptr<Model> backGroundMovieModel_ = nullptr;
	std::unique_ptr<Model> backGroundFrameModel_ = nullptr;
	std::unique_ptr<BackGround> backGround_ = nullptr;

	//スコア
	std::unique_ptr<Score> score_ = nullptr;

	//オーディオハンドル
	uint32_t whiffAudioHandle_ = 0;
	uint32_t tutorialBGMHandle_ = 0;

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
};

