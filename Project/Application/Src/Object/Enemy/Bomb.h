#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Math/MathFunction.h"

class Bomb : public Collider
{
public:
	void Initialize(Model* model, const Vector3& position);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	Vector3 velocity_{};

	float gravity_ = 0.05f;

	bool isMove_ = false;
};

