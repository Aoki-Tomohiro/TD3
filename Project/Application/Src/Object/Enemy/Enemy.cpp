#include "Enemy.h"
#include "Engine/Base/ImGuiManager.h"

void Enemy::Initialize(Model* model)
{
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_.y = -10.0f;
}

void Enemy::Update()
{
	if (++activeTimer_ > kActiveTime)
	{
		isActive_ = true;
	}

	worldTransform_.UpdateMatrixFromEuler();

	ImGui::Begin("Enemy");
	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x);
	ImGui::End();
}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Enemy::OnCollision(Collider* collider)
{

}

const Vector3 Enemy::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}