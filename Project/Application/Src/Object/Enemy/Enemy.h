#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/Particle/ParticleManager.h"

class Enemy : public Collider
{
public:
	void Initialize(Model* model,const Vector3& position);

	void Update();

	void Draw(const Camera& camera);

	const Vector3& GetPosition() const { return worldTransform_.translation_; };

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; };

	const Vector3& GetScale() const { return worldTransform_.scale_; };

	const bool GetisAlive() const { return isAlive_; };

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const override;

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	ParticleSystem* particleSystem_ = nullptr;

	bool isAlive_ = true;
};

