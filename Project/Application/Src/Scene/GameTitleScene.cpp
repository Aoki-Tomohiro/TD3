#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//スプライトの生成
	backGroundSprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	backGroundSprite_->SetSize({ 1280.0f,720.0f });
	TextureManager::Load("PushA.png");
	pushASprite_.reset(Sprite::Create("PushA.png", pushASpritePosition_));
	TextureManager::Load("Title.png");
	titleSprite_.reset(Sprite::Create("Title.png", titleSpritePosition_));

	//音声データの読み込み
	bgmHandle_ = audio_->SoundLoadWave("Application/Resources/Sounds/BGM.wav");
	decisionHandle_ = audio_->SoundLoadWave("Application/Resources/Sounds/Decision.wav");

	//BGMの再生
	audio_->SoundPlayWave(bgmHandle_, true, 0.1f);
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update() 
{
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GamePlayScene");
			audio_->SoundPlayWave(decisionHandle_, false, 0.4f);
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GamePlayScene");
		audio_->SoundPlayWave(decisionHandle_, false, 0.4f);
	}

	//スプライトの座標を設定
	pushASprite_->SetPosition(pushASpritePosition_);
	titleSprite_->SetPosition(titleSpritePosition_);
	titleSprite_->SetSize(titleSpriteSize_);

	ImGui::Begin("GameTitleScene");
	ImGui::Text("A or SPACE : GamePlayScene");
	ImGui::DragFloat2("PushASpritePosition", &pushASpritePosition_.x);
	ImGui::DragFloat2("TitleSpritePosition", &titleSpritePosition_.x);
	ImGui::DragFloat2("TitleSpriteSize", &titleSpriteSize_.x);
	ImGui::End();
}

void GameTitleScene::Draw()
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

void GameTitleScene::DrawUI() 
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景のスプライトの描画
	backGroundSprite_->Draw();

	//タイトルのスプライトの描画
	titleSprite_->Draw();

	//PushAのスプライトの描画
	pushASprite_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}