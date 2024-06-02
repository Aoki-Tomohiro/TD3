#pragma once
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"

class Wall : public Collider
{
public:
	void Initialize(const Vector3& position, const Vector3& scale, const uint32_t id);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const override;

	const Vector3& GetPosition() const { return worldTransform_.translation_; };

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; };

	const Vector3& GetScale() const { return worldTransform_.scale_; };

	const bool GetIsActive() const { return isActive_; };

	void SetIsActive(const bool isActive) { isActive_ = isActive; };

	void SetIsEdit(const bool isEdit) { isEdit_ = isEdit; };

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	bool isActive_ = false;

	bool isEdit_ = false;
};

