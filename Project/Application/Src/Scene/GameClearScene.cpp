#include "GameClearScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"

int GameClearScene::timeCount_ = 0;

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

	/*/数字のテクスチャの読み込み
	for (uint32_t i = 0; i < 10; i++)
	{
		std::string textureName = "Numbers/" + std::to_string(i) + ".png";
		TextureManager::Load(textureName);
	}*/
	//コピーの数のスプライトの生成
	timeCountSprites_[0].reset(Sprite::Create("Numbers/0.png", timeCountSpritePositions_[0]));
	timeCountSprites_[1].reset(Sprite::Create("Numbers/0.png", timeCountSpritePositions_[1]));

	//音声データの読み込み
	decisionHandle_ = audio_->LoadAudioFile("Decision.wav");
}

void GameClearScene::Finalize() 
{

}

void GameClearScene::Update() 
{
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
	if (timeCount_ < 21)
	{
		scoreSprite_->SetTexture("Scores/S.png");
	}
	else if (timeCount_ >= 21 && timeCount_ < 31)
	{
		scoreSprite_->SetTexture("Scores/A.png");
	}
	else if (timeCount_ >= 31 && timeCount_ < 41)
	{
		scoreSprite_->SetTexture("Scores/B.png");
	}
	else if (timeCount_ >= 41)
	{
		scoreSprite_->SetTexture("Scores/C.png");
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
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("StageSelectScene");
			audio_->PlayAudio(decisionHandle_, false, 0.4f);
			timeCount_ = 0;
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		sceneManager_->ChangeScene("StageSelectScene");
		audio_->PlayAudio(decisionHandle_, false, 0.4f);
		timeCount_ = 0;
	}

	ImGui::Begin("GameClearScene");
	ImGui::DragInt("CopyCount", &timeCount_);
	ImGui::DragFloat2("SpritePositon[0]", &timeCountSpritePositions_[0].x);
	ImGui::DragFloat2("SpritePositon[1]", &timeCountSpritePositions_[1].x);
	ImGui::DragFloat2("SpriteSize[0]", &SpriteSize_[0].x);
	ImGui::DragFloat2("SpriteSize[1]", &SpriteSize_[1].x);
	ImGui::DragFloat2("ScoreSpritePosition", &scoreSpritePosition_.x);
	ImGui::DragFloat2("ScoreSpriteSize", &scoreSpriteSize_.x);
	ImGui::End();
}

void GameClearScene::Draw() 
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

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

	//リザルトのスプライトの描画
	resultSprite_->Draw();

	//スコアのスプライトの描画
	scoreSprite_->Draw();

	//コピーの数の描画
	for (uint32_t i = 0; i < 2; i++)
	{
		timeCountSprites_[i]->Draw();
	}

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameClearScene::DrawBackGround()
{

}