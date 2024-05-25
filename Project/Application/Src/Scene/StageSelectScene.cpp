#include "StageSelectScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include <Engine/Math/MathFunction.h>

uint32_t StageSelectScene::stageNumber_ = 0;

void StageSelectScene::Initialize() {
	//camera_.Initialize();
	//camera_.fov_ = 45.0f * 3.141592654f / 180.0f;

	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();


	// 数字のテクスチャの読み込み
	for (uint32_t i = 0; i < 10; i++)
	{
		std::string textureName = "Numbers/" + std::to_string(i) + ".png";
		TextureManager::Load(textureName);
	}
	//コピーの数のスプライトの生成
	numberSprites_[0].reset(Sprite::Create("Numbers/1.png", numberPositions_[0]));
	numberSprites_[1].reset(Sprite::Create("Numbers/2.png", numberPositions_[1]));
	numberSprites_[2].reset(Sprite::Create("Numbers/3.png", numberPositions_[2]));

	selectNumber_ = 1;

	//音声データの読み込み
	decisionHandle_ = audio_->LoadAudioFile("Decision.wav");
}

void StageSelectScene::Finalize() {

}

void StageSelectScene::Update() {

	if (!isMovementEnabled_) {
		const uint32_t kEnableTime = 15;
		if (++MovementEnableTimer_ >= kEnableTime) {
			isMovementEnabled_ = true;
			MovementEnableTimer_ = 0;
		}
	}

	//コントローラー操作
	if (input_->IsControllerConnected())
	{
			bool isSelect = false;

			const float threshold = 0.8f;

			Vector3 stickTilt{
				input_->GetLeftStickX(),
				input_->GetLeftStickY(),
				0.0f
			};

			if (Mathf::Length(stickTilt) > threshold) {
				if (isMovementEnabled_) {
					//左右にカーソルを動かす
					if (stickTilt.x < -threshold) {
						++selectNumber_;
						isMovementEnabled_ = false;
					}
					else if (stickTilt.x > threshold) {
						--selectNumber_;
						isMovementEnabled_ = false;
					}
				}
			}
	}



	if (input_->IsPushKeyEnter(DIK_D)) {
		++selectNumber_;
	}
	else if (input_->IsPushKeyEnter(DIK_A)) {
		--selectNumber_;
	}
	if (selectNumber_ <= 0) {
		selectNumber_ = 1;
	}
	else if (selectNumber_ >= 4) {
		selectNumber_ = 3;
	}

	if (selectNumber_ == 1) {
		SpriteSize_[0] = { 5.0f,5.0f };
		SpriteSize_[1] = { 3.0f,3.0f };
		SpriteSize_[2] = { 3.0f,3.0f };
		numberPositions_[0] = { -120.0f,11.0f };
		numberPositions_[1] = {380.0f,11.0f };
		numberPositions_[2] = {700.0f,11.0f };
	}
	else if (selectNumber_ == 2) {
		SpriteSize_[1] = { 5.0f,5.0f };
		SpriteSize_[0] = { 3.0f,3.0f };
		SpriteSize_[2] = { 3.0f,3.0f };
		numberPositions_[1] = { 200.0f,11.0f };
		numberPositions_[0] = { 60.0f,11.0f };
		numberPositions_[2] = { 700.0f,11.0f };
	}
	else if(selectNumber_ == 3){
		SpriteSize_[2] = { 5.0f,5.0f };
		SpriteSize_[0] = { 3.0f,3.0f };
		SpriteSize_[1] = { 3.0f,3.0f };
		numberPositions_[2] = { 560.0f,11.0f };
		numberPositions_[0] = { 60.0f,11.0f };
		numberPositions_[1] = { 380.0f,11.0f };
	}


	//スプライトの座標を設定
	for (uint32_t i = 0; i < 3; i++)
	{
		numberSprites_[i]->SetPosition(numberPositions_[i]);
		numberSprites_[i]->SetScale(SpriteSize_[i]);
	}

	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			StageSelectScene::stageNumber_ = selectNumber_ + 1;
			sceneManager_->ChangeScene("GamePlayScene");
			audio_->PlayAudio(decisionHandle_, false, 0.4f);
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		StageSelectScene::stageNumber_ = selectNumber_ + 1;
		sceneManager_->ChangeScene("GamePlayScene");
		audio_->PlayAudio(decisionHandle_, false, 0.4f);
	}


	ImGui::Begin("SelectScene");
	ImGui::DragFloat2("SpritePositon[0]", &numberPositions_[0].x);
	ImGui::DragFloat2("SpritePositon[1]", &numberPositions_[1].x);
	ImGui::DragFloat2("SpritePositon[2]", &numberPositions_[2].x);
	ImGui::DragFloat2("SpriteSize[0]", &SpriteSize_[0].x);
	ImGui::DragFloat2("SpriteSize[1]", &SpriteSize_[1].x);
	ImGui::DragFloat2("SpriteSize[2]", &SpriteSize_[2].x);
	ImGui::End();
}

void StageSelectScene::Draw(){
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

void StageSelectScene::DrawUI() {
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//コピーの数の描画
	for (uint32_t i = 0; i < 3; i++)
	{
		numberSprites_[i]->Draw();
	}

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void StageSelectScene::DrawBackGround() {

}