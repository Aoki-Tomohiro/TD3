#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Math/MathFunction.h"
#include <optional>

class Player : public Collider
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

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

private:
	Input* input_ = nullptr;

	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Vector3 velocity_{};
};

