#include "Block.h"

void Block::Initialize(const Vector3& position, const Vector3& scale)
{
	//モデルの初期化
	model_.reset(ModelManager::Create());

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = scale;

	//衝突判定の初期化
	AABB aabb = {
	.min{-worldTransform_.scale_.x,-worldTransform_.scale_.y,-worldTransform_.scale_.z},
	.max{worldTransform_.scale_.x,worldTransform_.scale_.y,worldTransform_.scale_.z},
	};
	SetAABB(aabb);

	SetCollisionAttribute(kCollisionAttributeBlock);
	SetCollisionMask(kCollisionMaskBlock);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

}

void Block::Update()
{
	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();
}

void Block::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera);
}

void Block::OnCollision(Collider* collider)
{

}

const Vector3 Block::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}