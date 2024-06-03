#include "GameClearScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Application/Src/Scene/StageSelectScene.h"
#include "Application/Src/Scene/GamePlayScene.h"
#include "Application/Src/Scene/StageSelectScene.h"
#include "Engine/Base/ImGuiManager.h"
#include <Engine/Components/PostEffects/PostEffects.h>

int GameClearScene::timeCount_ = 0;
int GameClearScene::score_ = 0;
int GameClearScene::kEnemyNumber = 0;

void GameClearScene::Initialize() 
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//リザルト画面のスプライトの作成
	TextureManager::Load("clears.png");
	resultSprite_.reset(Sprite::Create("clears.png", { 0.0f,0.0f }));

	//スコアのスプライトの生成
	TextureManager::Load("Scores/S.png");
	TextureManager::Load("Scores/A.png");
	TextureManager::Load("Scores/B.png");
	TextureManager::Load("Scores/C.png");
	scoreSprite_.reset(Sprite::Create("Scores/S.png", scoreSpritePosition_));

	//数字のテクスチャの読み込み
	for (uint32_t i = 0; i < 10; i++)
	{
		std::string textureName = "Numbers/" + std::to_string(i) + ".png";
		TextureManager::Load(textureName);
	}
	//コピーの数のスプライトの生成
	timeCountSprites_[0].reset(Sprite::Create("Numbers/0.png", timeCountSpritePositions_[0]));
	timeCountSprites_[1].reset(Sprite::Create("Numbers/0.png", timeCountSpritePositions_[1]));

	//音声データの読み込み
	decisionHandle_ = audio_->LoadAudioFile("Decision.wav");
	clearBGMHandle_ = audio_->LoadAudioFile("clearBGM.wav");

	audio_->PlayAudio(clearBGMHandle_, true, 0.1f);

	//リザルトのスプライトの生成
	for (uint32_t i = 0; i < resultSprites_.size(); i++)
	{
		resultSprites_[i].reset(Sprite::Create("Numbers/0.png", { 0.0f,0.0f }));
		resultSprites_[i]->SetPosition({ -20.0f + float(i * 110.0f),100.0f });
		resultSprites_[i]->SetScale({ 1.4f,1.4f });
	}
}

void GameClearScene::Finalize() 
{

}

void GameClearScene::Update() 
{
	//スコアのテクスチャを設定
	int currentScore = score_;
	resultSprites_[0]->SetTexture("Numbers/" + std::to_string(currentScore / 10000) + ".png");
	currentScore %= 10000;
	resultSprites_[1]->SetTexture("Numbers/" + std::to_string(currentScore / 1000) + ".png");
	currentScore %= 1000;
	resultSprites_[2]->SetTexture("Numbers/" + std::to_string(currentScore / 100) + ".png");
	currentScore %= 100;
	resultSprites_[3]->SetTexture("Numbers/" + std::to_string(currentScore / 10) + ".png");
	currentScore %= 10;
	resultSprites_[4]->SetTexture("Numbers/" + std::to_string(currentScore) + ".png");


	//コピーの数のテクスチャを設定
	int score = timeCount_;
	if (score > 99)
	{
		timeCountSprites_[0]->SetTexture("Numbers/9.png");
		timeCountSprites_[1]->SetTexture("Numbers/9.png");
	}
	else
	{
		timeCountSprites_[0]->SetTexture("Numbers/" + std::to_string(score / 10) + ".png");
		score %= 10;
		timeCountSprites_[1]->SetTexture("Numbers/" + std::to_string(score) + ".png");
	}

	//スコアのテクスチャを設定
	if (kEnemyNumber < 3) {
		if (score_ >= 1500)
		{
			scoreSprite_->SetTexture("Scores/S.png");
		}
		else if (score_ >= 1400 && score_ < 1500)
		{
			scoreSprite_->SetTexture("Scores/A.png");
		}
		else if (score_ >= 1300 && score_ < 1400)
		{
			scoreSprite_->SetTexture("Scores/B.png");
		}
		else if (score_ < 1300)
		{
			scoreSprite_->SetTexture("Scores/C.png");
		}
	}
	else {
		if (score_ >= 2400)
		{
			scoreSprite_->SetTexture("Scores/S.png");
		}
		else if (score_ >= 2100 && score_ < 2400)
		{
			scoreSprite_->SetTexture("Scores/A.png");
		}
		else if (score_ >= 1800 && score_ < 2100)
		{
			scoreSprite_->SetTexture("Scores/B.png");
		}
		else if (score_ < 1800)
		{
			scoreSprite_->SetTexture("Scores/C.png");
		}
	}
	

	//スプライトの座標を設定
	for (uint32_t i = 0; i < 2; i++)
	{
		timeCountSprites_[i]->SetPosition(timeCountSpritePositions_[i]);
		timeCountSprites_[i]->SetScale(SpriteSize_[i]);
	}
	scoreSprite_->SetPosition(scoreSpritePosition_);
	scoreSprite_->SetSize(scoreSpriteSize_);


	//タイトル画面に戻る
	if(!isFadeOut_ && !isFadeIn_){
		if (input_->IsControllerConnected())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
			{
				isFadeOut_ = true;
				audio_->PlayAudio(decisionHandle_, false, 0.4f);
				
			}
		}

		if (input_->IsPushKeyEnter(DIK_SPACE))
		{
			isFadeOut_ = true;
			audio_->PlayAudio(decisionHandle_, false, 0.4f);
			
		}
	}
	
	//トランジション
	Transition();

	ImGui::Begin("GameClearScene");
	ImGui::DragInt("CopyCount", &timeCount_);
	ImGui::DragFloat2("SpritePositon[0]", &timeCountSpritePositions_[0].x);
	ImGui::DragFloat2("SpritePositon[1]", &timeCountSpritePositions_[1].x);
	ImGui::DragFloat2("SpriteSize[0]", &SpriteSize_[0].x);
	ImGui::DragFloat2("SpriteSize[1]", &SpriteSize_[1].x);
	ImGui::DragFloat2("ScoreSpritePosition", &scoreSpritePosition_.x);
	ImGui::DragFloat2("ScoreSpriteSize", &scoreSpriteSize_.x);
	//for (uint32_t i = 0; i < resultSprites_.size(); i++)
	//{
	//	std::string positionName = "ScorePosition" + std::to_string(i);
	//	std::string scaleName = "ScoreScale" + std::to_string(i);
	//	ImGui::DragFloat2(positionName.c_str(), &position_[i].x);
	//	ImGui::DragFloat2(scaleName.c_str(), &scale_[i].x);
	//	resultSprites_[i]->SetPosition(position_[i]);
	//	resultSprites_[i]->SetScale(scale_[i]);
	//}
	ImGui::End();
	
}

void GameClearScene::Draw() 
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//リザルトのスプライトの描画
	resultSprite_->Draw();

	//スコアのスプライトの描画
	scoreSprite_->Draw();

	////コピーの数の描画
	//for (uint32_t i = 0; i < 2; i++)
	//{
	//	timeCountSprites_[i]->Draw();
	//}

	for (uint32_t i = 0; i < resultSprites_.size(); i++)
	{
		resultSprites_[i]->Draw();
	}

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion
}

void GameClearScene::DrawUI() 
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameClearScene::DrawBackGround()
{

}

void GameClearScene::Transition() {
	//フェードインの処理
	if (isFadeIn_)
	{
		timer_ += 1.0f / 10.0f;

		if (timer_ >= 3.0f)
		{
			timer_ = 3.0f;
			isFadeIn_ = false;
		}
	}

	//フェードアウトの処理
	if (isFadeOut_)
	{
		timer_ -= 1.0f / 10.0f;
		if (timer_ <= 0.0f)
		{
			if (GamePlayScene::currentStageNumber == StageSelectScene::GetMaxStageCount() - 1)
			{
				sceneManager_->ChangeScene("GameTitleScene");
				StageSelectScene::preSelectNumber_ = 0;
				timeCount_ = 0;
			}
			else
			{
				sceneManager_->ChangeScene("StageSelectScene");
				timeCount_ = 0;
			}
			audio_->StopAudio(clearBGMHandle_);
			timer_ = 0.0f;
		}
	}

	PostEffects::GetInstance()->GetVignette()->SetIntensity(timer_);

}