#include "Score.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Base/TextureManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/CopyManager.h"

void Score::Initialize()
{
	//数字のテクスチャの読み込み
	for (uint32_t i = 0; i < 10; i++)
	{
		std::string textureName = "Numbers/" + std::to_string(i) + ".png";
		TextureManager::Load(textureName);
	}

	//スプライトの生成
	for (uint32_t i = 0; i < scoreSprites_.size(); i++)
	{
		scoreSprites_[i].reset(Sprite::Create("Numbers/0.png", { 0.0f,0.0f }));
		scoreSprites_[i]->SetPosition({ float(i * 65.0f),30.0f });
		scoreSprites_[i]->SetScale({ 0.8f,0.8f });
	}
}

void Score::Update(Player* player, const std::list<std::unique_ptr<Copy>>& copies)
{
	//加算される基本のポイント
	const float point = 500.0f;
	//倍率
	float multiplier = 1.0f;

	//プレイヤーが倒した時
	if ((player->GetWeapon()->GetIsAttack() || !player->GetIsMove()))
	{
		//プレイヤーの攻撃が当たっていなかったら
		if (player->GetWeapon()->GetIsHit())
		{
			score_ += int(point * multiplier) * player->GetWeapon()->GetHitCount();
		}
	}
	multiplier += 0.2f;

	//コピーが倒した時
	for (const std::unique_ptr<Copy>& copy : copies)
	{
		if (copy->GetIsActive())
		{
			if (copy->GetIsEnemyDefeated())
			{
				score_ += int(point * multiplier) * copy->GetWeapon()->GetHitCount();
			}
		}
		multiplier += 0.2f;
	}

	//スプライトのテクスチャ変更
	int currentScore = score_;
	scoreSprites_[0]->SetTexture("Numbers/" + std::to_string(currentScore / 10000) + ".png");
	currentScore %= 10000;
	scoreSprites_[1]->SetTexture("Numbers/" + std::to_string(currentScore / 1000) + ".png");
	currentScore %= 1000;
	scoreSprites_[2]->SetTexture("Numbers/" + std::to_string(currentScore / 100) + ".png");
	currentScore %= 100;
	scoreSprites_[3]->SetTexture("Numbers/" + std::to_string(currentScore / 10) + ".png");
	currentScore %= 10;
	scoreSprites_[4]->SetTexture("Numbers/" + std::to_string(currentScore) + ".png");
}

void Score::Draw()
{
	for (uint32_t i = 0; i < scoreSprites_.size(); i++)
	{
		scoreSprites_[i]->Draw();
	}
}