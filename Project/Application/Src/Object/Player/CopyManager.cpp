#include "CopyManager.h"
#include "Engine/Utilities/GlobalVariables.h"

void CopyManager::Initialize()
{
	//環境変数の初期化
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "CopyManager";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "MaxCopyCount", maxCopyCount_);

	//数字のテクスチャの読み込み
	for (uint32_t i = 0; i < 10; i++)
	{
		std::string textureName = "Numbers/" + std::to_string(i) + ".png";
		TextureManager::Load(textureName);
	}
	TextureManager::Load("Numbers/Slash.png");

	//コピー上限のスプライトの生成
	for (uint32_t i = 0; i < maxCopySprites_.size(); ++i)
	{
		if (i == 1)
		{
			maxCopySprites_[i].reset(Sprite::Create("Numbers/Slash.png", { 0.0f,0.0f }));
			continue;
		}
		maxCopySprites_[i].reset(Sprite::Create("Numbers/0.png", { 0.0f,0.0f }));
	}

	//コピーの色の初期化
	copyColors_.push_back(Vector4{ 0.667f, 1.0f, 0.667f, 1.0f });
	copyColors_.push_back(Vector4{ 0.333f, 1.0f, 0.333f, 1.0f });
	copyColors_.push_back(Vector4{ 0.0f, 1.0f, 0.0f, 1.0f });
	copyColors_.push_back(Vector4{ 0.0f, 0.667f, 0.0f, 1.0f });
	copyColors_.push_back(Vector4{ 0.0f, 0.333f, 0.0f, 1.0f });
}

void CopyManager::Update()
{
	//コピーの色を変更
	int i = 0;
	for (std::unique_ptr<Copy>& copy : copies_)
	{
		copy->SetColor(copyColors_[i]);
		i++;
	}

	//コピーの更新
	for (std::unique_ptr<Copy>& copy : copies_)
	{
		copy->Update();
	}

	ApplyGlobalVariables();

	ImGui::Begin("CopyManager");
	for (uint32_t i = 0; i < maxCopySprites_.size(); ++i)
	{
		std::string positionName = "MaxCopySpritePosition" + std::to_string(i);
		ImGui::DragFloat2(positionName.c_str(), &spritePositions_[i].x);
		std::string sizeName = "MaxCopySpriteSize" + std::to_string(i);
		ImGui::DragFloat2(sizeName.c_str(), &spriteScale_[i].x);
		maxCopySprites_[i]->SetPosition(spritePositions_[i]);
		maxCopySprites_[i]->SetScale(spriteScale_[i]);
	}
	maxCopySprites_[0]->SetTexture("Numbers/" + std::to_string(copyCount_) + ".png");
	maxCopySprites_[2]->SetTexture("Numbers/" + std::to_string(maxCopyCount_) + ".png");
	ImGui::End();
}

void CopyManager::Draw(const Camera& camera)
{
	//コピーの描画
	for (std::unique_ptr<Copy>& copy : copies_)
	{
		copy->Draw(camera);
	}
}

void CopyManager::DrawUI()
{
	for (uint32_t i = 0; i < maxCopySprites_.size(); ++i)
	{
		maxCopySprites_[i]->Draw();
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
		copies_.pop_back();
	}
	else
	{
		copyCount_++;
	}
	//新しいコピーの生成
	Copy* copy = new Copy();
	copy->Initialize(playerPositions_);
	copies_.push_front(std::unique_ptr<Copy>(copy));
	playerPositions_.clear();
}

void CopyManager::SetIsDoubleSpeed(const bool isDoubleSpeed)
{
	for (std::unique_ptr<Copy>& copy : copies_)
	{
		copy->SetIsDoubleSpeed(isDoubleSpeed);
	}
}

void CopyManager::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "CopyManager";
	maxCopyCount_ = globalVariables->GetIntValue(groupName, "MaxCopyCount");
}
