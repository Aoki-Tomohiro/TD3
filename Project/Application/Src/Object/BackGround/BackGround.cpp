#include "BackGround.h"
#include "Engine/Base/ImGuiManager.h"
#include <numbers>

void BackGround::Initialize(Model* model)
{
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = { -14.8f,-25.5f,5.0f };
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	worldTransform_.scale_ = { 20.3f,3.83f,1.0f };
}

void BackGround::Update()
{
	worldTransform_.UpdateMatrixFromEuler();

	ImGui::Begin("BackGround");
	ImGui::DragFloat3("Translate", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("Rotate", &worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &worldTransform_.scale_.x, 0.01f);
	ImGui::End();
}

void BackGround::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}