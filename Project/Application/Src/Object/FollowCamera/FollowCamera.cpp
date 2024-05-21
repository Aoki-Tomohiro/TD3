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
}