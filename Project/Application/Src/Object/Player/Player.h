#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Player/Weapon.h"
#include <optional>

class Player : public Collider
{
public:
	void Initialzie(std::vector<Model*> models);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const;

	Weapon* GetWeapon() { return weapon_.get(); };

private:
	enum class Behavior
	{
		kRoot,
		kJump,
		kAttack,
	};

	void BehaviorRootInitialize();

	void BehaviorRootUpdate();

	void BehaviorJumpInitialize();

	void BehaviorJumpUpdate();

	void BehaviorAttackInitialize();

	void BehaviorAttackUpdate();

	void ApplyGlobalVariables();

private:
	Input* input_ = nullptr;

	std::vector<Model*> models_{};

	WorldTransform worldTransform_{};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Quaternion destinationQuaternion_{ 0.0f,0.707f,0.0f,0.707f };

	Vector3 velocity_{};

	float speed_ = 0.3f;

	float gravity_ = 0.05f;

	float jumpFirstSpeed_ = 0.8f;

	std::unique_ptr<Weapon> weapon_ = nullptr;
};

