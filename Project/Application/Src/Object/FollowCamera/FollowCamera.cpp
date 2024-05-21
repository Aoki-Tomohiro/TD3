#include "FollowCamera.h"
#include "Engine/Base/ImGuiManager.h"

void FollowCamera::Initialize()
{
	camera_.Initialize();
}

void FollowCamera::Update()
{
	//カメラの更新
	camera_.translation_ = target_->translation_ + offset_;
	camera_.UpdateMatrix();

	ImGui::Begin("FollowCamera");
	ImGui::DragFloat3("Offset", &offset_.x,0.1f);
	ImGui::End();
}