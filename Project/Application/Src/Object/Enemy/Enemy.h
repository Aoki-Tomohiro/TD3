#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"

class Enemy : public Collider
{
public:
	static const int kActiveTime = 60 * 5;

	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const bool GetIsActive() const { return isActive_; };

	const bool GetIsCopied() const { return isCopied_; };

	void SetIsCopied(const bool isCopied) { isCopied_ = isCopied; };

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	bool isActive_ = false;

	int activeTimer_ = 0;

	bool isCopied_ = false;
};

