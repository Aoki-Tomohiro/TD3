#include "Wall.h"

void Wall::Initialize(const Vector3& position, const Vector3& scale)
{
	//モデルの生成
	model_.reset(ModelManager::CreateFromModelFile("Cube.obj", Transparent));

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

	SetCollisionAttribute(kCollisionAttributeWall);
	SetCollisionMask(kCollisionMaskWall);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Wall::Update()
{
	//衝突判定の初期化
	AABB aabb = {
	.min{-worldTransform_.scale_.x,-worldTransform_.scale_.y,-worldTransform_.scale_.z},
	.max{worldTransform_.scale_.x,worldTransform_.scale_.y,worldTransform_.scale_.z},
	};
	SetAABB(aabb);

	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();
}

void Wall::Draw(const Camera& camera)
{
	if (isActive_)
	{
		//モデルの描画
		model_->Draw(worldTransform_, camera);
	}
}

void Wall::OnCollision(Collider* collider)
{

}

const Vector3 Wall::GetWorldPosition() const
{
	Vector3 worldPosition{};
	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];
	return worldPosition;
}
