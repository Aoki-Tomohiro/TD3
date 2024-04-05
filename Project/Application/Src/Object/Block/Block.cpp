#include "Block.h"

void Block::Initialize(Model* model, const Vector3& position, const Vector3& scale)
{
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = scale;

	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeBlock);
	SetCollisionMask(kCollisionMaskBlock);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	AABB aabb = {
	.min{-worldTransform_.scale_.x,-worldTransform_.scale_.y,-worldTransform_.scale_.z},
	.max{worldTransform_.scale_.x,worldTransform_.scale_.y,worldTransform_.scale_.z},
	};
	SetAABB(aabb);
}

void Block::Update()
{
	worldTransform_.UpdateMatrixFromEuler();
}

void Block::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Block::OnCollision(Collider* collider)
{

}

const Vector3 Block::GetWorldPosition() const {
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}