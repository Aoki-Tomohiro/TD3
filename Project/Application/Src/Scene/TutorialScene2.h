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

class TutorialScene2 : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void DrawBackGround() override;

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
	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Model> weaponModel_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;

	//敵
	std::unique_ptr<Model> enemyModel_ = nullptr;
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;
	//std::vector<std::unique_ptr<Enemy>> enemies_{};

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

	//FollowCamera
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;

	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//コントローラーのUI
	std::unique_ptr<Sprite> contSprite_ = nullptr;
	Vector2 spritePosition_ = { 0.0f,540.0f };
	Vector2 spriteScale_ = { 360.0f,220.0f };

	//逆再生中か
	bool isReversed_ = false;
	std::vector<std::tuple<Vector3, bool, uint32_t, float>> reversePlayerPositions{};

	//オーディオハンドル
	uint32_t whiffAudioHandle_ = 0;

	//巻き戻しのデータスキップ数
	uint32_t stepSize_ = 4;

	//チュートリアルのスプライト
	std::unique_ptr<Sprite> tutorialSprite_ = nullptr;
	std::unique_ptr<Sprite> numberSprite_ = nullptr;
};

