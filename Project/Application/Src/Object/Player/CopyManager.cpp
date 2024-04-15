#include "CopyManager.h"

void CopyManager::Initialize(Model* model)
{
	//モデルの初期化
	assert(model);
	model_ = model;
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
	copy->Initialize(model_, playerPositions_);
	copies_.push_back(std::unique_ptr<Copy>(copy));
	playerPositions_.clear();
}