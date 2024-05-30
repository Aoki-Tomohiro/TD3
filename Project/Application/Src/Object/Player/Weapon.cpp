#include "Weapon.h"

void Weapon::Initialize(Model* model)
{
	//モデルの初期化
	assert(model);
	model_ = model;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0.0f,2.0f,5.0f };
	worldTransform_.scale_ = { 0.1f,1.5f,1.4f };
	worldTransformCollision_.Initialize();
	worldTransformCollision_.scale_ = { 3.0f,3.0f,0.1f };

	//スプライトの生成
	TextureManager::Load("x.png");
	xButtonSprite_.reset(Sprite::Create("x.png", { 0.0f,0.0f }));
	xButtonSprite_->SetSize({ 48.0f,48.0f });

	//衝突判定の初期化
	SetCollisionAttribute(kCollisionAttributeNoWeapon);
	SetCollisionMask(kCollisionMaskNoWeapon);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//環境変数の初期化
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Weapon";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Offset", offset_);
	globalVariables->AddItem(groupName, "Scale", worldTransform_.scale_);
}

void Weapon::Update()
{
	//Xボタンのスプライトをみえないようにする
	xButtonSpriteVisible_ = false;
	hitCount_ = 0;

	//AABBのサイズを設定
	worldTransformCollision_.scale_ = { worldTransform_.scale_.z,worldTransform_.scale_.y,worldTransform_.scale_.x };
	AABB aabb = {
	.min{-worldTransformCollision_.scale_.x, -worldTransformCollision_.scale_.y, -worldTransformCollision_.scale_.z},
	.max{worldTransformCollision_.scale_.x, worldTransformCollision_.scale_.y,worldTransformCollision_.scale_.z}
	};
	SetAABB(aabb);

	//攻撃時に赤くする
	Vector4 color{};
	if (isAttack_)
	{
		color = Vector4{ 1.0f,0.0f,0.0f,0.3f };
		SetCollisionAttribute(kCollisionAttributeWeapon);
		SetCollisionMask(kCollisionMaskWeapon);
	}
	else
	{
		color = Vector4{ 0.0f,1.0f,1.0f,0.3f };
		SetCollisionAttribute(kCollisionAttributeNoWeapon);
		SetCollisionMask(kCollisionMaskNoWeapon);
	}
	model_->GetMaterial(1)->SetColor(color);

	//当たり判定の位置を決める
	Vector3 offset = offset_;
	offset = Mathf::TransformNormal(offset, worldTransform_.parent_->matWorld_);
	worldTransformCollision_.translation_ = worldTransform_.parent_->translation_;
	worldTransformCollision_.translation_ += offset;

	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();
	worldTransformCollision_.UpdateMatrixFromEuler();

	//環境変数の適応
	ApplyGlobalVariables();
}

void Weapon::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransformCollision_, camera);
}

void Weapon::DrawUI(const Camera& camera)
{
	if (xButtonSpriteVisible_)
	{
		//ビューポート行列を計算
		Matrix4x4 matViewport = Mathf::MakeViewportMatrix(0, 0, Application::kClientWidth, Application::kClientHeight, 0, 1);
		//ビュー行列とプロジェクション行列とビューポート行列を合成
		Matrix4x4 matViewProjectionViewport = camera.matView_ * camera.matProjection_ * matViewport;
		//スクリーン座標に変換
		Vector3 offset = { 0.0f,8.0f,0.0f };
		Vector3 spritePosition = { worldTransform_.parent_->translation_.x,worldTransform_.parent_->translation_.y,worldTransform_.parent_->translation_.z };
		spritePosition += offset;
		spritePosition = Mathf::Transform(spritePosition, matViewProjectionViewport);
		//スプライトに座標を設定
		xButtonSprite_->SetPosition({ spritePosition.x,spritePosition.y });
		//描画
		xButtonSprite_->Draw();
	}
}

void Weapon::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy)
	{
		xButtonSpriteVisible_ = true;
		isHit_ = true;
		hitCount_++;
	}
}

const Vector3 Weapon::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransformCollision_.matWorld_.m[3][0];
	pos.y = worldTransformCollision_.matWorld_.m[3][1];
	pos.z = worldTransformCollision_.matWorld_.m[3][2];
	return pos;
}

void Weapon::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Weapon";
	offset_ = globalVariables->GetVector3Value(groupName, "Offset");
	worldTransform_.scale_ = globalVariables->GetVector3Value(groupName, "Scale");
}