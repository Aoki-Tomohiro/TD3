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