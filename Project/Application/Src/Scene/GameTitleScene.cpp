#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include <numbers>

bool GameTitleScene::isStart_ = false;

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
	bgmHandle_ = audio_->LoadAudioFile("BGM.wav");
	decisionHandle_ = audio_->LoadAudioFile("Decision.wav");
	titleBgmHandle_ = audio_->LoadAudioFile("titleBGM.wav");

	//BGMの再生
	if (!isStart_)
	{
		
		isStart_ = true;
	}

	audio_->PlayAudio(titleBgmHandle_, true, 0.5f);

	//カメラの初期化
	camera_.Initialize();
	//camera_.translation_ = { 60.0f,0.0f,-70.0f };
	//camera_.rotation_.y = -0.785f;
	camera_.translation_ = { 49.0f,0.0f,-33.0f };
	camera_.rotation_.y = -0.695f;

	//壁の生成
	wallModel_ = ModelManager::CreateFromModelFile("asiba.gltf", "Ground", Opaque);
	wallWorldTransform_.Initialize();
	wallWorldTransform_.rotation_.y = std::numbers::pi_v<float>;

	//地面の生成
	groundModel_ = ModelManager::CreateFromModelFile("aoasi.gltf", "aoasi", Opaque);
	for (int i = 0; i < groundWorldTransforms_.size(); i++)
	{
		groundWorldTransforms_[i].Initialize();
		groundWorldTransforms_[i].translation_ = { 0.0f,-5.1f,-15.5f + i * -16.0f };
	}

	//プレイヤーの生成
	playerModel_ = ModelManager::CreateFromModelFile("Human.gltf", "Player", Opaque);
	playerModel_->GetMaterial(0)->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	weaponModel_ = ModelManager::CreateFromModelFile("Cube.obj", "PlayerWeapon", Transparent);
	std::vector<Model*> playerModels = { playerModel_,weaponModel_ };
	player_ = std::make_unique<Player>();
	player_->Initialzie(playerModels);
	player_->SetPosition({ -80.0f,-4.2f,-16.2f });
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update() 
{
	//プレイヤーの更新
	player_->TitleUpdate();

	//壁のワールドトランスフォームの更新
	wallWorldTransform_.UpdateMatrixFromEuler();

	//地面のワールドトランスフォームの更新
	for (int i = 0; i < groundWorldTransforms_.size(); i++)
	{
		groundWorldTransforms_[i].UpdateMatrixFromEuler();
	}

	//カメラの更新
	camera_.UpdateMatrix();

	if (!isFadeIn_ && !isFadeOut_) {
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
	
	

	if (input_->IsPushKeyEnter(DIK_S))
	{
		sceneManager_->ChangeScene("StageSelectScene");
		audio_->StopAudio(titleBgmHandle_);
		audio_->PlayAudio(decisionHandle_, false, 0.4f);
	}

	Transition();

	

	//スプライトの座標を設定
	pushASprite_->SetPosition(pushASpritePosition_);
	//titleSprite_->SetPosition(titleSpritePosition_);
	//titleSprite_->SetSize(titleSpriteSize_);

	ImGui::Begin("GameTitleScene");
	ImGui::Text("A or SPACE : GamePlayScene");
	ImGui::DragFloat2("PushASpritePosition", &pushASpritePosition_.x);
	ImGui::DragFloat2("TitleSpritePosition", &titleSpritePosition_.x);
	ImGui::DragFloat2("TitleSpriteSize", &titleSpriteSize_.x);
	ImGui::SliderFloat("Intensity", &timer_, 0.0f, 5.0f);
	ImGui::DragFloat3("CameraTranslation", &camera_.translation_.x, 0.1f);
	ImGui::DragFloat3("CameraRotation", &camera_.rotation_.x, 0.01f);
	ImGui::DragFloat3("WallTranslation", &wallWorldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("WallScale", &wallWorldTransform_.scale_.x, 0.01f);
	ImGui::DragFloat3("GroundTranslation1", &groundWorldTransforms_[0].translation_.x, 0.1f);
	ImGui::DragFloat3("GroundRotation1", &groundWorldTransforms_[0].rotation_.x, 0.01f);
	ImGui::DragFloat3("GroundScale1", &groundWorldTransforms_[0].scale_.x, 0.01f);
	ImGui::DragFloat3("GroundTranslation2", &groundWorldTransforms_[1].translation_.x, 0.1f);
	ImGui::DragFloat3("GroundRotation2", &groundWorldTransforms_[1].rotation_.x, 0.01f);
	ImGui::DragFloat3("GroundScale2", &groundWorldTransforms_[1].scale_.x, 0.01f);
	ImGui::End();
}

void GameTitleScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景のスプライトの描画
	//backGroundSprite_->Draw();

	//タイトルのスプライトの描画
	//titleSprite_->Draw();

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//プレイヤーの描画
	player_->Draw(camera_);

	//壁の描画
	wallModel_->Draw(wallWorldTransform_, camera_);

	//地面の描画
	for (int i = 0; i < groundWorldTransforms_.size(); i++)
	{
		groundModel_->Draw(groundWorldTransforms_[i], camera_);
	}

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion

#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	

	//PushAのスプライトの描画
	//pushASprite_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameTitleScene::DrawUI() 
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	////背景のスプライトの描画
	//backGroundSprite_->Draw();

	////タイトルのスプライトの描画
	//titleSprite_->Draw();

	////PushAのスプライトの描画
	//pushASprite_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameTitleScene::DrawBackGround()
{

}

void GameTitleScene::Transition() {
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
			sceneManager_->ChangeScene("TutorialScene1");
			audio_->StopAudio(titleBgmHandle_);
			timer_ = 0.0f;
		}
	}

	PostEffects::GetInstance()->GetVignette()->SetIntensity(timer_);

}