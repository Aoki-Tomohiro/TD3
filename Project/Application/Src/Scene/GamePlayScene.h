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
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Block/BlockManager.h"
#include "Application/Src/Object/Player/CopyManager.h"
#include "Application/Src/Object/BackGround/BackGround.h"
#include "Application/Src/Object/FollowCamera/FollowCamera.h"

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

private:
	
	void CalculateRating();

	void AddEnemy(const Vector3& position);

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//カメラ
	Camera camera_{};

	//評価
	int likes_;
	float dislikes_;
	int totaScore_;
	//一回の攻撃で倒した敵の数
	int num_;

	//敵
	std::unique_ptr<Model> enemyModel_ = nullptr;
	std::vector<std::unique_ptr<Enemy>> enemies_{};
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
	std::unique_ptr<Model> backGroundMovieModel_ = nullptr;
	std::unique_ptr<Model> backGroundFrameModel_ = nullptr;
	std::unique_ptr<BackGround> backGround_ = nullptr;

	//FollowCamera
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;

	//コントローラーのUI
	std::unique_ptr<Sprite> contSprite_ = nullptr;
	Vector2 spritePosition_ = { 0.0f,540.0f };
	Vector2 spriteScale_ = { 360.0f,220.0f };

	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//逆再生中か
	bool isReversed_ = false;
	std::vector<std::tuple<Vector3, Quaternion, bool, uint32_t, float>> reversePlayerPositions{};
};

