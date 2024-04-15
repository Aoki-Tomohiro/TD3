#include "Copy.h"

void Copy::Initialize(std::vector<Model*> models, const std::vector<std::tuple<Vector3, Quaternion, bool>>& playerPositions)
{
	//モデルの初期化
	models_ = models;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//キューの初期化
	playerPositions_ = playerPositions;

	//武器の生成
	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize(models_[1]);
	weapon_->SetParent(worldTransform_);
}

void Copy::Update()
{
	//座標を設定
	if (currentIndex_ < playerPositions_.size())
	{
		Vector3 playerPosition{};
		Quaternion quaternion{};
		bool isAttack{};
		std::tie(playerPosition, quaternion, isAttack) = playerPositions_[currentIndex_];
		worldTransform_.translation_ = playerPosition;
		worldTransform_.quaternion_ = quaternion;
		weapon_->SetIsAttack(isAttack);
		currentIndex_++;
	}

	//ワールドトランスフォームの行進
	worldTransform_.UpdateMatrixFromQuaternion();

	//武器の更新
	weapon_->Update();

	//武器の判定をつける
	if (currentIndex_ < playerPositions_.size())
	{
		weapon_->SetIsAttack(std::get<bool>(playerPositions_[currentIndex_]));
	}
}

void Copy::Draw(const Camera& camera)
{
	//モデルの描画
	models_[0]->Draw(worldTransform_, camera);

	//武器の描画
	weapon_->Draw(camera);
}

void Copy::Reset()
{
	currentIndex_ = 0;
}