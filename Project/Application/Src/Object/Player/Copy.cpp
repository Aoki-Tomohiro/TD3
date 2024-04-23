#include "Copy.h"

void Copy::Initialize(Model* model, const std::vector<Vector3>& playerPositions)
{
	//モデルの初期化
	assert(model);
	model_ = model;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//キューの初期化
	playerPositions_ = playerPositions;
}

void Copy::Update()
{
	//座標を設定
	if (currentIndex_ < playerPositions_.size())
	{
		worldTransform_.translation_ = playerPositions_[currentIndex_];
		currentIndex_++;
	}

	//ワールドトランスフォームの行進
	worldTransform_.UpdateMatrixFromEuler();
}

void Copy::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera);
}

void Copy::Reset()
{
	currentIndex_ = 0;
}