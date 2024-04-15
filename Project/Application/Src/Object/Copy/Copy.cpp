#include "Copy.h"
#include "Engine/Base/ImGuiManager.h"

void Copy::Initialize(Model* model)
{
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_.y = -10.0f;
}

void Copy::Update()
{
	if (++activeTimer_ > kActiveTime)
	{
		isActive_ = true;
	}
	if ((activeTimer_ - kActiveTime) / 60 >= 10) {
		isActive_ = false;
	}

	worldTransform_.UpdateMatrixFromEuler();

	ImGui::Begin("Enemy");
	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x);
	ImGui::Text("activeTimer%d", (activeTimer_ - kActiveTime) / 60);
	ImGui::End();
}

void Copy::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

const Vector3 Copy::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}