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

	const bool GetIsCopied() const { return isCopied_; };

	void SetIsCopied(const bool isCopied) { isCopied_ = isCopied; };

	const bool GetIsCopied2() const { return isCopied2_; };

	void SetIsCopied2(const bool isCopied) { isCopied2_ = isCopied; };

	const bool GetIsCopied3() const { return isCopied3_; };

	void SetIsCopied3(const bool isCopied) { isCopied3_ = isCopied; };

	const bool GetIsCopied4() const { return isCopied4_; };

	void SetIsCopied4(const bool isCopied) { isCopied4_ = isCopied; };

	const bool GetIsCopied5() const { return isCopied5_; };

	void SetIsCopied5(const bool isCopied) { isCopied5_ = isCopied; };

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

	bool isCopied_ = false;
	bool isCopied2_ = false;
	bool isCopied3_ = false;
	bool isCopied4_ = false;
	bool isCopied5_ = false;
};

