#include "GameOverScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include <Engine/Components/PostEffects/PostEffects.h>

int GameOverScene::copyCount_ = 0;

void GameOverScene::Initialize() 
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//リザルト画面のスプライトの作成
	TextureManager::Load("gameover.png");
	resultSprite_.reset(Sprite::Create("gameover.png", { 0.0f,0.0f }));

	//音声データの読み込み
	decisionHandle_ = audio_->LoadAudioFile("Decision.wav");
	overBGMHandle_ = audio_->LoadAudioFile("overBGM.wav");

	audio_->PlayAudio(overBGMHandle_, true, 0.5f);

}

void GameOverScene::Finalize() 
{

}

void GameOverScene::Update()
{
	//タイトル画面に戻る
	if (!isFadeOut_ && !isFadeIn_) {
		if (input_->IsControllerConnected())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
			{
				isFadeOut_ = true;
				isSelect_ = true;
				audio_->PlayAudio(decisionHandle_, false, 0.4f);
				
			}

			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
			{
				isFadeOut_ = true;
				isPlay_ = true;
				audio_->PlayAudio(decisionHandle_, false, 0.4f);
			}
		}

		if (input_->IsPushKeyEnter(DIK_SPACE))
		{
			isFadeOut_ = true;
			isSelect_ = true;
			audio_->PlayAudio(decisionHandle_, false, 0.4f);
		}
		if (input_->IsPushKeyEnter(DIK_R))
		{
			isFadeOut_ = true;
			isPlay_ = true;
			audio_->PlayAudio(decisionHandle_, false, 0.4f);
		}
	}
	
	//トランジション
	Transition();

}

void GameOverScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//リザルトのスプライトの描画
	resultSprite_->Draw();

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

void GameOverScene::DrawUI() 
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameOverScene::DrawBackGround()
{

}

void GameOverScene::Transition() {
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
			if (isSelect_)
			{
				
				sceneManager_->ChangeScene("StageSelectScene");
				copyCount_ = 0;
			}

			if (isPlay_)
			{
				sceneManager_->ChangeScene("GamePlayScene");
				copyCount_ = 0;
			}
			audio_->StopAudio(overBGMHandle_);
			timer_ = 0.0f;
		}
	}

	PostEffects::GetInstance()->GetVignette()->SetIntensity(timer_);

}