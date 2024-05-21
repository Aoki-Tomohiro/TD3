#include "BackGround.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/ImGuiManager.h"
#include <numbers>

void BackGround::Initialize(std::vector<Model*> models)
{
	//モデルの初期化
	models_ = models;

	//ワールドトランスフォームの初期化
	for (uint32_t i = 0; i < kCountOfParts; ++i)
	{
		worldTransforms_[i].Initialize();
	}
	//worldTransforms_[kFrame].translation_ = {21.6f,-9.0f,1.6f};
	//worldTransforms_[kFrame].scale_ = {2.92f,1.35f,1.0f};
	//worldTransforms_[kFrame].rotation_.y = std::numbers::pi_v<float>;
	//worldTransforms_[kMovie].translation_ = { -0.2f,5.9f,2.0f };
	//worldTransforms_[kMovie].scale_ = { 34.6f,12.98f,1.0f };
	//worldTransforms_[kMovie].rotation_.y = std::numbers::pi_v<float>;
	worldTransforms_[kFrame].translation_ = { 21.6f,-19.0f,1.6f };
	worldTransforms_[kFrame].scale_ = { 2.92f,1.8f,1.0f };
	worldTransforms_[kFrame].rotation_.y = std::numbers::pi_v<float>;
	worldTransforms_[kMovie].translation_ = { -0.2f,0.9f,1.4f };
	worldTransforms_[kMovie].scale_ = { 34.12f,17.05f,1.0f };
	worldTransforms_[kMovie].rotation_.y = std::numbers::pi_v<float>;
}

void BackGround::Update()
{
	//ワールドトランスフォームの更新
	for (uint32_t i = 0; i < kCountOfParts; ++i)
	{
		worldTransforms_[i].UpdateMatrixFromEuler();
	}

	ImGui::Begin("BackGround");
	ImGui::DragFloat3("FrameTranslate", &worldTransforms_[kFrame].translation_.x, 0.1f);
	ImGui::DragFloat3("FrameRotate", &worldTransforms_[kFrame].rotation_.x, 0.01f);
	ImGui::DragFloat3("FrameScale", &worldTransforms_[kFrame].scale_.x, 0.01f);
	ImGui::DragFloat3("MovieTranslate", &worldTransforms_[kMovie].translation_.x, 0.1f);
	ImGui::DragFloat3("MovieRotate", &worldTransforms_[kMovie].rotation_.x, 0.01f);
	ImGui::DragFloat3("MovieScale", &worldTransforms_[kMovie].scale_.x, 0.01f);
	ImGui::End();
}

void BackGround::Draw(const Camera& camera)
{
	for (uint32_t i = 0; i < kCountOfParts; ++i)
	{
		models_[i]->Draw(worldTransforms_[i], camera);
	}
}