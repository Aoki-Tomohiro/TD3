#include "Switch.h"
#include "Engine/Math/MathFunction.h"

void Switch::Initialize(const Vector3& switchPosition, const Vector3& wallPosition, const Vector3& wallScale, const Type type, const uint32_t id)
{
	//モデルの生成
	model_ = ModelManager::CreateFromModelFile("Cube.obj", "Switch" + std::to_string(id), Opaque);

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = switchPosition;
	worldTransform_.scale_ = { 1.0f,0.2f,1.0f };

	//壁の生成
	wall_ = std::make_unique<Wall>();
	wall_->Initialize(wallPosition, wallScale, id);

	//スイッチの種類を設定
	type_ = type;

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

	switch (type_)
	{
	//出現壁の場合
	case Type::APPEARANCE:
		if (isPressed_)
		{
			wall_->SetIsActive(true);
			worldTransform_.scale_.y = Mathf::Lerp(worldTransform_.scale_.y, 0.0f, 0.1f);
		}
		else
		{
			wall_->SetIsActive(false);
			worldTransform_.scale_.y = Mathf::Lerp(worldTransform_.scale_.y, 0.2f, 0.1f);
		}
		break;
	//消失壁の場合
	case Type::DISAPPEARANCE:
		if (isPressed_)
		{
			wall_->SetIsActive(false);
			worldTransform_.scale_.y = Mathf::Lerp(worldTransform_.scale_.y, 0.0f, 0.1f);
		}
		else
		{
			wall_->SetIsActive(true);
			worldTransform_.scale_.y = Mathf::Lerp(worldTransform_.scale_.y, 0.2f, 0.1f);
		}
		break;
	}

	//色を変更
	if (isEdit_)
	{
		model_->GetMaterial(1)->SetColor({ 1.0f,0.5f,0.0f,1.0f });
		wall_->SetIsEdit(true);
	}
	else
	{
		model_->GetMaterial(1)->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		wall_->SetIsEdit(false);
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