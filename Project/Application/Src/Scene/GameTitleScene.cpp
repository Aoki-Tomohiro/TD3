#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include <numbers>
#include "StageSelectScene.h"

bool GameTitleScene::isStart_ = false;

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();
	StageSelectScene::stageNumber_ = 0;

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
	wallModel_->GetMaterial(0)->SetColor({ 0.0f, 0.047f, 0.196f, 1.0f });
	wallModel_->GetMaterial(1)->SetColor({ 0.0f, 0.047f, 0.196f, 1.0f });
	wallModel_->GetMaterial(2)->SetColor({ 0.0f, 0.047f, 0.196f, 1.0f });
	wallModel_->GetMaterial(3)->SetColor({ 0.0f, 0.047f, 0.196f, 1.0f });
	wallModel_->GetMaterial(4)->SetColor({ 0.0f, 0.047f, 0.196f, 1.0f });
	for (int i = 0; i < wallWorldTransforms_.size(); i++)
	{
		wallWorldTransforms_[i].Initialize();
		wallWorldTransforms_[i].translation_.y = i * 10.0f;
		wallWorldTransforms_[i].rotation_.y = std::numbers::pi_v<float>;
	}

	//地面の生成
	TextureManager::Load("aoo.png");
	groundModel_ = ModelManager::CreateFromModelFile("aoasi.gltf", "Block", Opaque);
	groundModel_->GetMaterial(0)->SetTexture("aoo.png");
	//groundModel_->GetMaterial(0)->SetColor({ -0.5f,1.0f,1.0f,1.0f });
	groundModel_->GetMaterial(0)->SetUVScale({ 5.0f,1.0f });
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
	player_->Initialzie(playerModels, { -80.0f,-4.2f,-16.2f });

	//敵の生成
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize({ -60.0f,-4.2f,-16.2f }, 0);

	//フォントのモデルの生成
	fontModel_ = ModelManager::CreateFromModelFile("Font.gltf", "Font", Opaque);
	fontWorldTransform_.Initialize();
	//fontWorldTransform_.translation_ = { 16.2f,11.2f,-9.8f };
	//fontWorldTransform_.rotation_ = { std::numbers::pi_v<float> / 2.0f ,-std::numbers::pi_v<float> ,0.0f };
	//fontWorldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	fontWorldTransform_.translation_ = { 34.4f,4.6f,-25.1f };
	fontWorldTransform_.rotation_ = { std::numbers::pi_v<float> / 2.0f ,-std::numbers::pi_v<float> -0.695f ,0.0f };
	fontWorldTransform_.scale_ = { 3.0f,3.0f,3.0f };

	//Aボタンのモデルの生成
	AButtonModel_ = ModelManager::CreateFromModelFile("A.gltf", "A", Opaque);
	aButtonWorldTransform_.Initialize();
	//aButtonWorldTransform_.translation_ = { 22.5f,0.0f,-9.8f };
	//aButtonWorldTransform_.rotation_ = { std::numbers::pi_v<float> / 2.0f ,-std::numbers::pi_v<float> ,0.0f };
	//aButtonWorldTransform_.scale_ = { 4.0f,4.0f,4.0f };
	aButtonWorldTransform_.translation_ = { 39.6f,-2.5f,-25.1f };
	aButtonWorldTransform_.rotation_ = { 1.511f ,-std::numbers::pi_v<float> -0.695f ,0.0f };
	aButtonWorldTransform_.scale_ = { 2.0f,2.0f,2.0f };
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update() 
{
	//プレイヤーの更新
	player_->TitleUpdate();

	//敵の更新
	enemy_->TitleUpdate();

	//壁のワールドトランスフォームの更新
	for (int i = 0; i < wallWorldTransforms_.size(); i++)
	{
		wallWorldTransforms_[i].UpdateMatrixFromEuler();
	}

	//地面のワールドトランスフォームの更新
	for (int i = 0; i < groundWorldTransforms_.size(); i++)
	{
		groundWorldTransforms_[i].UpdateMatrixFromEuler();
	}

	//フォントのワールドトランスフォームの更新
	fontWorldTransform_.UpdateMatrixFromEuler();

	//Aボタンのワールドトランスフォームの更新
	aButtonWorldTransform_.UpdateMatrixFromEuler();

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
	
	LightManager::GetInstance()->GetDirectionalLight(0).SetDirection(direction_);
	LightManager::GetInstance()->GetDirectionalLight(0).SetIntensity(intensity_);

	if (input_->IsPushKeyEnter(DIK_S))
	{
		sceneManager_->ChangeScene("StageSelectScene");
		audio_->StopAudio(titleBgmHandle_);
		audio_->PlayAudio(decisionHandle_, false, 0.4f);
		LightManager::GetInstance()->GetDirectionalLight(0).SetDirection({ 0.0f,-1.0f,0.0f });
		LightManager::GetInstance()->GetDirectionalLight(0).SetIntensity(1.0f);
	}

	Transition();

	

	//スプライトの座標を設定
	pushASprite_->SetPosition(pushASpritePosition_);
	//titleSprite_->SetPosition(titleSpritePosition_);
	//titleSprite_->SetSize(titleSpriteSize_);

	ImGui::Begin("GameTitleScene");
	ImGui::Text("A or SPACE : GamePlayScene");
	ImGui::DragFloat3("FontTranslation", &fontWorldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("FontRotation", &fontWorldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("FontScale", &fontWorldTransform_.scale_.x, 0.01f);
	ImGui::DragFloat3("AbuttonTranslation", &aButtonWorldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("AbuttonRotation", &aButtonWorldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("AbuttonScale", &aButtonWorldTransform_.scale_.x, 0.01f);
	ImGui::DragFloat3("LightDirection", &direction_.x, 0.01f);
	ImGui::DragFloat("LightIntensity", &intensity_, 0.01f);
	ImGui::End();
	direction_ = Mathf::Normalize(direction_);
}

void GameTitleScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景のスプライトの描画
	//backGroundSprite_->Draw();

	//タイトルのスプライトの描画
	titleSprite_->Draw();

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//プレイヤーの描画
	player_->Draw(camera_);

	//敵の描画
	enemy_->Draw(camera_);

	//壁の描画
	for (int i = 0; i < wallWorldTransforms_.size(); i++)
	{
		wallModel_->Draw(wallWorldTransforms_[i], camera_);
	}

	//地面の描画
	for (int i = 0; i < groundWorldTransforms_.size(); i++)
	{
		groundModel_->Draw(groundWorldTransforms_[i], camera_);
	}

	//フォントのモデルの描画
	fontModel_->Draw(fontWorldTransform_, camera_);

	//Aボタンの描画
	AButtonModel_->Draw(aButtonWorldTransform_, camera_);

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
			LightManager::GetInstance()->GetDirectionalLight(0).SetDirection({ 0.0f,-1.0f,0.0f });
			LightManager::GetInstance()->GetDirectionalLight(0).SetIntensity(1.0f);
		}
	}

	PostEffects::GetInstance()->GetVignette()->SetIntensity(timer_);

}