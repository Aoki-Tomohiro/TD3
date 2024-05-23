#include "GameOverScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

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
}

void GameOverScene::Finalize() 
{

}

void GameOverScene::Update()
{
	//タイトル画面に戻る
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GameTitleScene");
			audio_->PlayAudio(decisionHandle_, false, 0.4f);
			copyCount_ = 0;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
			sceneManager_->ChangeScene("GamePlayScene");
			audio_->PlayAudio(decisionHandle_, false, 0.4f);
			copyCount_ = 0;
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameTitleScene");
		audio_->PlayAudio(decisionHandle_, false, 0.4f);
		copyCount_ = 0;
	}
	if (input_->IsPushKeyEnter(DIK_R))
	{
		sceneManager_->ChangeScene("GamePlayScene");
		audio_->PlayAudio(decisionHandle_, false, 0.4f);
		copyCount_ = 0;
	}
}

void GameOverScene::Draw()
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

void GameOverScene::DrawUI() 
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//リザルトのスプライトの描画
	resultSprite_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameOverScene::DrawBackGround()
{

}