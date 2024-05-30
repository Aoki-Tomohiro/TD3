#include "Switch.h"

void Switch::Initialize(const Vector3& switchPosition, const Vector3& wallPosition, const Vector3& wallScale)
{
	//モデルの生成
	model_.reset(ModelManager::Create());

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = switchPosition;
	worldTransform_.scale_ = { 1.0f,0.2f,1.0f };

	//壁の生成
	wall_ = std::make_unique<Wall>();
	wall_->Initialize(wallPosition, wallScale);

	//衝突判定の初期化
	AABB aabb = {
	.min{-worldTransform_.scale_.x,-worldTransform_.scale_.y,-worldTransform_.scale_.z},
	.max{worldTransform_.scale_.x,worldTransform_.scale_.y,worldTransform_.scale_.z},
	};
	SetAABB(aabb);

	SetCollisionAttribute(kCollisionAttributeSwitch);
	SetCollisionMask(kCollisionMaskSwitch);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Switch::Update()
{
	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();

	//壁の更新
	wall_->Update();

	//押されていたら壁を出す
	if (isPressed_)
	{
		wall_->SetIsActive(true);
	}
	else
	{
		wall_->SetIsActive(false);
	}

	//フラグを初期化
	isPressed_ = false;
}

void Switch::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera);

	//壁の描画
	wall_->Draw(camera);
}

void Switch::OnCollision(Collider* collider)
{
	//押された状態にする
	isPressed_ = true;
}

const Vector3 Switch::GetWorldPosition() const
{
	Vector3 worldPosition{};
	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];
	return worldPosition;
}

void Switch::SetColor(const Vector4& switchColor, const Vector4& wallColor)
{
	model_->GetMaterial(0)->SetColor(switchColor);
	wall_->SetColor(wallColor);
}