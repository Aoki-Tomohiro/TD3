#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"

class Block : public Collider
{
public:
	void Initialize(Model* model, const Vector3& position, const Vector3& scale);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};
};

