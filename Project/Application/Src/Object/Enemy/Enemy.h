#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Math/MathFunction.h"
#include "Astar.h"
#include <optional>

class Enemy : public Collider
{
public:
	static const int kActiveTime = 60 * 10;

	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	const bool GetIsActive() const { return isActive_; };

	const bool GetIsCopied() const { return isCopied_; };

	void SetIsCopied(const bool isCopied) { isCopied_ = isCopied; };

	//プレイヤー関連
	void SetPlayerPosition(const Vector3& pos) { playerPosition_ = {(36 + pos.x)/2.0f ,(36 - pos.y)/2.0f ,pos.z}; };

	//ブロック関連

	void SetBlockPosition(const Vector3& pos) { 
		blockPosition_[blockCount_] = {(36 + pos.x) / 2.0f,(36 - pos.y) / 2.0f,pos.z};
		blockCount_++;
	}

	void SetBlockSize(const Vector3& size) {
		blockSize_[blockSizeCount_] = size/2;
		blockSizeCount_++;

	}

private:
	//プレイヤーの状態
	enum class Behavior
	{
		kRoot,//通常状態
		kJump,//ジャンプ中
	};

	void BehaviorRootInitialize();

	void BehaviorRootUpdate();

	void BehaviorJumpInitialize();

	void BehaviorJumpUpdate();

	//プレイヤーとの距離の計算
	void DistanceFunction();


	//経路探索関数
	void FindPath();


private:
	Model* model_ = nullptr;


	WorldTransform worldTransform_{};


	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Vector3 velocity_{};

	Vector3 enemyPosition_{};
	Vector3 blockPosition_[36] = {};
	Vector3 blockSize_[36] = {};

	bool isActive_ = false;

	int activeTimer_ = 0;

	bool isCopied_ = false;

	int blockCount_ = 0;
	int blockSizeCount_ = 0;

	bool jump_ = false;

	//マップチップ
	int map[36][36] = {};
	//プレイヤー座標
	Vector3 playerPosition_{};

	//経路探索
	std::vector<Node*> path_;
	std::vector<std::vector<int>> findMap;
	int moveCount_;

	float easingParameter_ = 0.0f;
	Vector3 startPosition_{};
};

