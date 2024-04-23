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

	const Vector3 GetSize()  const { return worldTransform_.scale_; }
	
	const Vector3& GetPosition() const { return worldTransform_.translation_; };

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; };

	const Vector3& GetScale() const { return worldTransform_.scale_; };

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const override;

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};
};

