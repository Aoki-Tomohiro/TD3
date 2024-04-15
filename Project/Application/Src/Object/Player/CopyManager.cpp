#include "CopyManager.h"

void CopyManager::Initialize(const std::vector<Model*> models)
{
	//モデルの初期化
	models_ = models;
}

void CopyManager::Update()
{
	//コピーの更新
	for (std::unique_ptr<Copy>& copy : copies_)
	{
		copy->Update();
	}
}

void CopyManager::Draw(const Camera& camera)
{
	//コピーの描画
	for (std::unique_ptr<Copy>& copy : copies_)
	{
		copy->Draw(camera);
	}
}

void CopyManager::Reset()
{
	//コピーの状態をリセット
	for (std::unique_ptr<Copy>& copy : copies_)
	{
		copy->Reset();
	}

	//新しいコピーの生成
	Copy* copy = new Copy();
	copy->Initialize(models_, playerPositions_);
	copies_.push_back(std::unique_ptr<Copy>(copy));
	playerPositions_.clear();
}