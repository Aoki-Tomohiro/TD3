#pragma once
#include "Engine/Base/TextureManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Engine/Math/MathFunction.h"

class Weapon : public Collider
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void DrawUI(const Camera& camera);

	void SetParent(const WorldTransform& parent) { worldTransform_.SetParent(&parent); };

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const override;

	const bool GetIsAttack() const { return isAttack_; };

	void SetIsAttack(const bool isAttack) { isAttack_ = isAttack; };

	const bool GetIsHit() const { return isHit_; };

	void SetIsHit(const bool isHit) { isHit_ = isHit; };

	const uint32_t GetHitCount() const { return hitCount_; };

private:
	void ApplyGlobalVariables();

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	WorldTransform worldTransformCollision_{};

	bool isAttack_ = false;

	std::unique_ptr<Sprite> xButtonSprite_ = nullptr;

	bool xButtonSpriteVisible_ = false;

	Vector3 offset_{ 0.0f,1.0f,3.0f };

	bool isHit_ = false;

	uint32_t hitCount_ = 0;
};

