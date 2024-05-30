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
	//衝突判定の設定
	AABB aabb = {
	.min{-worldTransform_.scale_.x,-worldTransform_.scale_.y,-worldTransform_.scale_.z},
	.max{worldTransform_.scale_.x,worldTransform_.scale_.y,worldTransform_.scale_.z},
	};
	SetAABB(aabb);

	//色を変える
	if (isActive_)
	{
		model_->GetMaterial(1)->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	else
	{
		model_->GetMaterial(1)->SetColor({ 1.0f,1.0f,1.0f,0.25f });
	}

	//編集中なら
	if (isEdit_)
	{
		model_->GetMaterial(1)->SetColor({ 1.0f,0.5f,0.0f,1.0f });
	}

	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();
}

void Wall::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera);
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
