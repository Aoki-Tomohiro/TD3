#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"
#include "Application/Src/Object/Enemy/EnemyManager.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Block/BlockManager.h"
#include "Application/Src/Object/Player/CopyManager.h"
#include "Application/Src/Object/BackGround/BackGround.h"
#include "Application/Src/Object/Score/Score.h"

enum changeScene
{
	kTitle,
	kSelect,
	kClear,
	kOver,
};

class GamePlayScene : public IScene
{
public:
	static const uint32_t kMaxStageCount = 6;

	static uint32_t currentStageNumber;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void DrawBackGround() override;

	void Reset();

	void Transition();

	void Pause();

private:
	
	void CalculateRating();

	void Reverse();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//カメラ
	Camera camera_{};

	//タイマーのスプライト
	std::array<std::unique_ptr<Sprite>, 2> timeCountSprites_{};
	Vector2 timeCountSpritePositions_[2]{ {516.0f,11.0f},{595.0f,11.0f} };
	Vector2 SpriteSize_[2] = { {1.0f,1.0f},{1.0f,1.0f} };

	//評価
	int likes_;
	float dislikes_;
	int totaScore_;
	//一回の攻撃で倒した敵の数
	int num_;

	//敵
	std::unique_ptr<Model> enemyModel_ = nullptr;
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;
	//std::vector<std::unique_ptr<Enemy>> enemies_{};
	int enemyNum_;//今の敵の数
	int defeatedEnemyCount;//今倒している敵の数

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//プレイヤー
	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Model> weaponModel_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;

	//ブロック
	std::unique_ptr<Model> blockModel_ = nullptr;
	std::unique_ptr<BlockManager> blockManager_ = nullptr;

	//コピー
	std::unique_ptr<Model> copyModel_ = nullptr;
	std::unique_ptr<CopyManager> copyManager_ = nullptr;

	//背景
	std::unique_ptr<Model> backGroundGenkoModel_ = nullptr;
	std::unique_ptr<Model> backGroundMovieModel_ = nullptr;
	std::unique_ptr<Model> backGroundFrameModel_ = nullptr;
	std::unique_ptr<BackGround> backGround_ = nullptr;	
	
	//矢印のテクスチャ
	std::unique_ptr<Sprite> yajiSprite_ = nullptr;
	
	//ポーズ用スプライト
	std::unique_ptr<Sprite> backSprite_ = nullptr;
	std::unique_ptr<Sprite> pauseSprite_ = nullptr;
	std::unique_ptr<Sprite> pauseUISprite_ = nullptr;

	//スコア
	std::unique_ptr<Score> score_ = nullptr;

	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//逆再生中か
	bool isReversed_ = false;
	std::vector<std::tuple<Vector3, bool, uint32_t, float>> reversePlayerPositions{};

	//倍速中か
	bool isDoubleSpeed_ = false;

	//巻き戻しのデータスキップ数
	uint32_t stepSize_ = 4;

	//トランジションのフラグ
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	int nextScene_ = kClear;
	float timer_ = 0.0f;

	//ポーズメニューフラグ
	bool pause_ = false;
	bool rule_;
	//カーソルの座標
	Vector2 cursorPosition_{ -15.0f,-40.0f };
	Vector2 cursorVelocity_{ 40.0f,100.0f };
	//カーソルの移動のフラグ
	bool isCursorMovementEnabled_ = true;
	uint32_t cursorMovementEnableTimer_ = 0;

	Vector2 a = { 0.0f,0.0f };
};

