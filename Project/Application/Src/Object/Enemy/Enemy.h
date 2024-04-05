#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Math/MathFunction.h"
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
	void SetPlayerPosition(const Vector3& pos) { playerPosition_ = {35 + pos.x ,35 - pos.y ,pos.z}; };

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

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Vector3 velocity_{};

	bool isActive_ = false;

	int activeTimer_ = 0;

	bool isCopied_ = false;

	//マップチップ
	int map[70][70] = {};
	//プレイヤー座標
	Vector3 playerPosition_{};
};

