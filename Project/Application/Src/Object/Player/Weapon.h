#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Utilities/GlobalVariables.h"

class Weapon : public Collider
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void SetParent(const WorldTransform& parent) { worldTransform_.SetParent(&parent); };

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const override;

	const bool GetIsAttack() const { return isAttack_; };

	void SetIsAttack(const bool isAttack) { isAttack_ = isAttack; };

private:
	void ApplyGlobalVariables();

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	bool isAttack_ = false;
};

