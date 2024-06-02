#include "BackGround.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include <numbers>

void BackGround::Initialize()
{
	//背景スプライトの作成
	for (uint32_t i = 0; i < backGroundSprite_.size(); i++)
	{
		std::string textureName = "Back" + std::to_string(i + 1) + ".png";
		if (i == 2)
		{
			backGroundSpritePosition_[i].x = -40.0f;
		}
		else if (i == 3)
		{
			backGroundSpritePosition_[i].x = -80.0f;
		}
		backGroundSpritePosition_[i].y = -140.0f;
		TextureManager::Load(textureName);
		backGroundSprite_[i].reset(Sprite::Create(textureName, backGroundSpritePosition_[i]));
	}

	//ワールドトランスフォームの初期化
	for (uint32_t i = 0; i < kCountOfParts; ++i)
	{
		worldTransforms_[i].Initialize();
	}
	worldTransforms_[kGenko].Initialize();
	worldTransforms_[kGenko].translation_ = { -14.8f,-25.5f,5.0f };
	worldTransforms_[kGenko].rotation_.y = std::numbers::pi_v<float>;
	worldTransforms_[kGenko].scale_ = {20.3f,3.83f,1.0f};
	worldTransforms_[kFrame].translation_ = {21.6f,-9.0f,1.6f};
	worldTransforms_[kFrame].scale_ = {2.92f,1.35f,1.0f};
	worldTransforms_[kFrame].rotation_.y = std::numbers::pi_v<float>;
	worldTransforms_[kMovie].translation_ = { -0.2f,5.9f,2.0f };
	worldTransforms_[kMovie].scale_ = { 34.6f,12.98f,1.0f };
	worldTransforms_[kMovie].rotation_.y = std::numbers::pi_v<float>;
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
	//スプライトの座標を計算
	float currentPlayerPosition = playerPosition_.x + 36.0f;
	float easingParameter = 1.0f / (72.0f / currentPlayerPosition);
	backGroundSpritePosition_[1].x = Mathf::Lerp(0.0f, -10.0f, easingParameter);
	backGroundSpritePosition_[2].x = Mathf::Lerp(-30.0f, -50.0f, easingParameter);
	backGroundSpritePosition_[3].x = Mathf::Lerp(-70.0f, -150.0f, easingParameter);
	for (uint32_t i = 0; i < backGroundSprite_.size(); i++)
	{
		backGroundSprite_[i]->SetPosition(backGroundSpritePosition_[i]);
	}

	ImGui::Begin("BackGround");
	ImGui::DragFloat3("FrameTranslate", &worldTransforms_[kFrame].translation_.x, 0.1f);
	ImGui::DragFloat3("FrameRotate", &worldTransforms_[kFrame].rotation_.x, 0.01f);
	ImGui::DragFloat3("FrameScale", &worldTransforms_[kFrame].scale_.x, 0.01f);
	ImGui::DragFloat3("MovieTranslate", &worldTransforms_[kMovie].translation_.x, 0.1f);
	ImGui::DragFloat3("MovieRotate", &worldTransforms_[kMovie].rotation_.x, 0.01f);
	ImGui::DragFloat3("MovieScale", &worldTransforms_[kMovie].scale_.x, 0.01f);
	ImGui::DragFloat2("BackGroundPosition0", &backGroundSpritePosition_[0].x);
	ImGui::DragFloat2("BackGroundPosition1", &backGroundSpritePosition_[1].x);
	ImGui::DragFloat2("BackGroundPosition2", &backGroundSpritePosition_[2].x);
	ImGui::DragFloat2("BackGroundPosition3", &backGroundSpritePosition_[3].x);
	ImGui::End();
}

void BackGround::Draw(const Camera& camera)
{
	//for (uint32_t i = 0; i < kCountOfParts; ++i)
	//{
	//	models_[i]->Draw(worldTransforms_[i], camera);
	//}
}

void BackGround::DrawSprite()
{
	for (uint32_t i = 0; i < backGroundSprite_.size(); ++i)
	{
		backGroundSprite_[i]->Draw();
	}
}