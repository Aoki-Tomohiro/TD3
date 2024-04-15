#include "Weapon.h"

void Weapon::Initialize(Model* model)
{
	//モデルの初期化
	assert(model);
	model_ = model;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 1.0f,3.0f,3.0f };

	//衝突判定の初期化
	SetCollisionAttribute(kCollisionAttributeWeapon);
	SetCollisionMask(kCollisionMaskWeapon);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//環境変数の初期化
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Weapon";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Scale", worldTransform_.scale_);
}

void Weapon::Update()
{
	//AABBのサイズを設定
	AABB aabb = {
	.min{.x = -worldTransform_.scale_.x,.y = -worldTransform_.scale_.y,.z = -worldTransform_.scale_.z},
	.max{.x = worldTransform_.scale_.x,.y = worldTransform_.scale_.y,.z = worldTransform_.scale_.z}
	};
	SetAABB(aabb);

	//攻撃時に赤くする
	Vector4 color = isAttack_ ? Vector4{ 1.0f,0.0f,0.0f,0.6f } : Vector4{ 0.0f,1.0f,1.0f,0.6f };
	model_->SetColor(color);

	//ワールドトランスフォームの更新
	worldTransform_.translation_.z = worldTransform_.parent_->scale_.z + worldTransform_.scale_.z;
	worldTransform_.translation_.y = worldTransform_.parent_->scale_.y * 2.0f;
	worldTransform_.UpdateMatrixFromEuler();

	//環境変数の適応
	ApplyGlobalVariables();
}

void Weapon::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera);
}

void Weapon::OnCollision(Collider* collider)
{
}

const Vector3 Weapon::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}

void Weapon::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Weapon";
	worldTransform_.scale_ = globalVariables->GetVector3Value(groupName, "Scale");
}