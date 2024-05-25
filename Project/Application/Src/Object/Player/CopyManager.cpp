#include "CopyManager.h"
#include "Engine/Utilities/GlobalVariables.h"

void CopyManager::Initialize()
{
	//環境変数の初期化
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "CopyManager";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "MaxCopyCount", maxCopyCount_);
}

void CopyManager::Update()
{
	//コピーの更新
	for (std::unique_ptr<Copy>& copy : copies_)
	{
		copy->Update();
	}

	ApplyGlobalVariables();
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
}

void CopyManager::Reverse(const uint32_t stepSize)
{
	for (std::unique_ptr<Copy>& copy : copies_)
	{
		copy->Reverse(stepSize);
	}
}

void CopyManager::AddCopy()
{
	//コピーの最大数を超えていたら
	if (copyCount_ >= maxCopyCount_)
	{
		copies_.erase(copies_.begin());
	}
	else
	{
		copyCount_++;
	}
	//新しいコピーの生成
	Copy* copy = new Copy();
	copy->Initialize(playerPositions_);
	copies_.push_back(std::unique_ptr<Copy>(copy));
	playerPositions_.clear();
}
void CopyManager::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "CopyManager";
	maxCopyCount_ = globalVariables->GetIntValue(groupName, "MaxCopyCount");
}
