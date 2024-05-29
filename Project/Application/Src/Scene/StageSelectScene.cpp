#include "StageSelectScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include <Engine/Math/MathFunction.h>
#include <numbers>
#include <Engine/Components/PostEffects/PostEffects.h>

uint32_t StageSelectScene::stageNumber_ = 0;
uint32_t StageSelectScene::preSelectNumber_ = 0;

void StageSelectScene::Initialize() {
	//camera_.Initialize();
	//camera_.fov_ = 45.0f * 3.141592654f / 180.0f;

	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//前の選択したステージに設定
	selectNumber_ = preSelectNumber_;

	//背景スプライトの生成
	TextureManager::Load("backs.png");
	backGroundSprite_.reset(Sprite::Create("backs.png", { 0.0f,0.0f }));

	//矢印のスプライトの生成
	TextureManager::Load("Arrow.png");
	for (uint32_t i = 0; i < 2; ++i)
	{
		arrowSprites_[i].reset(Sprite::Create("Arrow.png", { 0.0f,0.0f }));
		arrowSprites_[i]->SetAnchorPoint({ 0.5f,0.5f });
	}
	arrowSprites_[0]->SetRotation(std::numbers::pi_v<float>);
	arrowSpriteTargetPosition_[0] = { 345.0f,444.0f };
	arrowSpritePosition_[0] = arrowSpriteTargetPosition_[0];
	arrowSpriteTargetPosition_[1] = { 988.0f,444.0f };
	arrowSpritePosition_[1] = arrowSpriteTargetPosition_[1];

	//チュートリアルのスプライトの生成
	TextureManager::Load("Tutorial.png");
	for (uint32_t i = 0; i < tutorialSprites_.size(); ++i)
	{
		tutorialSprites_[i].reset(Sprite::Create("Tutorial.png", { 0.0f,0.0f }));
		tutorialSpriteTargetPosition_[i] = { 350.0f + delta_ * i, 140.0f };
		tutorialSpritePosition_[i] = tutorialSpriteTargetPosition_[i];
	}

	//ステージのスプライト生成
	TextureManager::Load("Stage.png");
	for (uint32_t i = 0; i < 10; i++)
	{
		std::string textureName = "Numbers/" + std::to_string(i) + ".png";
		TextureManager::Load(textureName);
	}
	for (uint32_t i = 0; i < kMaxStages; ++i)
	{
		stageSprites_[i].reset(Sprite::Create("Stage.png", { 0.0f,0.0f }));
		numberSprites_[i].reset(Sprite::Create("Numbers/" + std::to_string(i + 1) + ".png", { 0.0f,0.0f }));
		stageSpriteTargetPosition_[i] = { 430.0f + (tutorialSprites_.size() + i) * delta_,142.0f};
		stageSpritePosition_[i] = stageSpriteTargetPosition_[i];
		numberSpriteTargetPosition_[i] = { 760.0f + (tutorialSprites_.size() + i) * delta_,142.0f };
		numberSpritePosition_[i] = numberSpriteTargetPosition_[i];
	}

	//ステージ画面のスプライトの作成
	for (uint32_t i = 0; i < stageScreenSprites_.size(); ++i)
	{
		stageScreenSprites_[i].reset(Sprite::Create("white.png", { 0.0f,0.0f }));
		stageScreenSpriteTargetPosition_[i] = { 418.0f + delta_ * i,300.0f };
		stageScreenSpritePosition_[i] = stageScreenSpriteTargetPosition_[i];
		stageScreenSpriteSize_[i] = { 500.0f,280.0f };
	}

	//PushAのスプライトの作成
	TextureManager::Load("PushA2.png");
	pushASprite_.reset(Sprite::Create("PushA2.png", { 0.0f,0.0f }));
	pushASpritePosition_ = { 890.0f,600.0f };
	pushASpriteScale_ = { 0.6f,0.6f };

	//音声データの読み込み
	decisionHandle_ = audio_->LoadAudioFile("Decision.wav");

	//座標を合わせる
	for (uint32_t i = 0; i < preSelectNumber_; i++)
	{
		for (uint32_t i = 0; i < tutorialSprites_.size(); i++)
		{
			tutorialSpriteTargetPosition_[i].x -= delta_;
			tutorialSpritePosition_[i] = tutorialSpriteTargetPosition_[i];
		}
		for (uint32_t i = 0; i < kMaxStages; i++)
		{
			stageSpriteTargetPosition_[i].x -= delta_;
			stageSpritePosition_[i] = stageSpriteTargetPosition_[i];
			numberSpriteTargetPosition_[i].x -= delta_;
			numberSpritePosition_[i] = numberSpriteTargetPosition_[i];
		}
		for (uint32_t i = 0; i < stageScreenSprites_.size(); i++)
		{
			stageScreenSpriteTargetPosition_[i].x -= delta_;
			stageScreenSpritePosition_[i] = stageScreenSpriteTargetPosition_[i];
		}
	}
}

void StageSelectScene::Finalize() {

}

void StageSelectScene::Update() {
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
				float delta = 0.0f;
				//左右にカーソルを動かす
				if (stickTilt.x < -threshold) {
					--selectNumber_;
					isMovementEnabled_ = false;
					delta = delta_;
				}
				else if (stickTilt.x > threshold) {
					++selectNumber_;
					isMovementEnabled_ = false;
					delta = -delta_;
				}
				//ステージ上限を超えないようにする
				if (selectNumber_ < 0)
				{
					selectNumber_ = 0;
				}
				else if (selectNumber_ > kMaxTutorial + kMaxStages - 1)
				{
					selectNumber_ = kMaxTutorial + kMaxStages - 1;
				}
				//ステージ上限を超えていなかったらスプライトをずらす
				else
				{
					//目標座標を決める
					for (uint32_t i = 0; i < tutorialSprites_.size(); ++i)
					{
						tutorialSpriteTargetPosition_[i].x += delta;
					}
					for (uint32_t i = 0; i < kMaxStages; ++i)
					{
						stageSpriteTargetPosition_[i].x += delta;
						numberSpriteTargetPosition_[i].x += delta;
					}
					for (uint32_t i = 0; i < stageScreenSprites_.size(); ++i)
					{
						stageScreenSpriteTargetPosition_[i].x += delta;
					}
				}
			}
		}
	}

	//キーボード操作
	if (isMovementEnabled_)
	{
		float delta = 0.0f;
		if (input_->IsPushKeyEnter(DIK_D)) {
			++selectNumber_;
			isMovementEnabled_ = false;
			delta = -delta_;
		}
		else if (input_->IsPushKeyEnter(DIK_A)) {
			--selectNumber_;
			isMovementEnabled_ = false;
			delta = delta_;
		}

		if (selectNumber_ < 0)
		{
			selectNumber_ = 0;
		}
		else if (selectNumber_ > kMaxTutorial + kMaxStages - 1)
		{
			selectNumber_ = kMaxTutorial + kMaxStages - 1;
		}
		else
		{
			//目標座標を決める
			for (uint32_t i = 0; i < tutorialSprites_.size(); ++i)
			{
				tutorialSpriteTargetPosition_[i].x += delta;
			}
			for (uint32_t i = 0; i < kMaxStages; ++i)
			{
				stageSpriteTargetPosition_[i].x += delta;
				numberSpriteTargetPosition_[i].x += delta;
			}
			for (uint32_t i = 0; i < stageScreenSprites_.size(); ++i)
			{
				stageScreenSpriteTargetPosition_[i].x += delta;
			}
		}
	}

	if (!isMovementEnabled_)
	{
		easingParameter_ += 1.0f / 20.0f;
		//チュートリアル
		for (uint32_t i = 0; i < tutorialSprites_.size(); ++i)
		{
			tutorialSpritePosition_[i].x = Mathf::Lerp(tutorialSpritePosition_[i].x, tutorialSpriteTargetPosition_[i].x, easingParameter_);
		}
		//ステージ
		for (uint32_t i = 0; i < kMaxStages; ++i)
		{
			stageSpritePosition_[i].x = Mathf::Lerp(stageSpritePosition_[i].x, stageSpriteTargetPosition_[i].x, easingParameter_);
			numberSpritePosition_[i].x = Mathf::Lerp(numberSpritePosition_[i].x, numberSpriteTargetPosition_[i].x, easingParameter_);
		}
		//ステージ画面
		for (uint32_t i = 0; i < stageScreenSprites_.size(); ++i)
		{
			stageScreenSpritePosition_[i].x = Mathf::Lerp(stageScreenSpritePosition_[i].x, stageScreenSpriteTargetPosition_[i].x, easingParameter_);
		}

		if (easingParameter_ >= 1.0f)
		{
			easingParameter_ = 0.0f;
			isMovementEnabled_ = true;
		}
	}

	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			if (selectNumber_ == 0)
			{
				sceneManager_->ChangeScene("TutorialScene1");
			}
			else
			{
				preSelectNumber_ = selectNumber_;
				StageSelectScene::stageNumber_ = selectNumber_ + 2;
				sceneManager_->ChangeScene("GamePlayScene");
			}
			isFadeOut_ = true;
			audio_->PlayAudio(decisionHandle_, false, 0.4f);
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		if (selectNumber_ == 0)
		{
			sceneManager_->ChangeScene("TutorialScene1");
		}
		else
		{
			preSelectNumber_ = selectNumber_;
			StageSelectScene::stageNumber_ = selectNumber_ + 2;;
			sceneManager_->ChangeScene("GamePlayScene");
		}
		isFadeOut_ = true;
		audio_->PlayAudio(decisionHandle_, false, 0.4f);
	}


	ImGui::Begin("SelectScene");
	ImGui::Text("SelectNumber : %d", selectNumber_);
	//矢印
	for (uint32_t i = 0; i < arrowSprites_.size(); i++)
	{
		std::string positionName = "ArrowSpritePosition" + std::to_string(i);
		ImGui::DragFloat2(positionName.c_str(), &arrowSpritePosition_[i].x);
		std::string sizeName = "ArrowSpriteSize" + std::to_string(i);
		ImGui::DragFloat2(sizeName.c_str(), &arrowSpriteSize_[i].x);
		arrowSprites_[i]->SetPosition(arrowSpritePosition_[i]);
		//arrowSprites_[i]->SetSize(arrowSpriteSize_[i]);
	}
	//チュートリアル
	for (uint32_t i = 0; i < tutorialSprites_.size(); i++)
	{
		std::string positionName = "TutorialSpritePosition" + std::to_string(i);
		ImGui::DragFloat2(positionName.c_str(), &tutorialSpritePosition_[i].x);
		std::string sizeName = "TutorialSpriteSize" + std::to_string(i);
		ImGui::DragFloat2(sizeName.c_str(), &tutorialSpriteSize_[i].x);
		tutorialSprites_[i]->SetPosition(tutorialSpritePosition_[i]);
		//tutorialSprites_[i]->SetSize(tutorialSpriteSize_[i]);
	}
	//ステージ
	for (uint32_t i = 0; i < kMaxStages; i++)
	{
		std::string stagePositionName = "StageSpritePosition" + std::to_string(i);
		ImGui::DragFloat2(stagePositionName.c_str(), &stageSpritePosition_[i].x);
		std::string stageSizeName = "StageSpriteSize" + std::to_string(i);
		ImGui::DragFloat2(stageSizeName.c_str(), &stageSpriteSize_[i].x);
		stageSprites_[i]->SetPosition(stageSpritePosition_[i]);
		//stageSprites_[i]->SetSize(stageSpriteSize_[i]);

		std::string numberPositionName = "NumberSpritePosition" + std::to_string(i);
		ImGui::DragFloat2(numberPositionName.c_str(), &numberSpritePosition_[i].x);
		std::string numberSizeName = "NumberSpriteSize" + std::to_string(i);
		ImGui::DragFloat2(numberSizeName.c_str(), &numberSpriteSize_[i].x);
		numberSprites_[i]->SetPosition(numberSpritePosition_[i]);
		//numberSprites_[i]->SetSize(numberSpriteSize_[i]);
	}
	//ステージ画面
	for (uint32_t i = 0; i < stageScreenSprites_.size(); i++)
	{
		std::string positionName = "StageScreenPosition" + std::to_string(i);
		ImGui::DragFloat2(positionName.c_str(), &stageScreenSpritePosition_[i].x);
		std::string sizeName = "StageScreenSpriteSize" + std::to_string(i);
		ImGui::DragFloat2(sizeName.c_str(), &stageScreenSpriteSize_[i].x);
		stageScreenSprites_[i]->SetPosition(stageScreenSpritePosition_[i]);
		stageScreenSprites_[i]->SetSize(stageScreenSpriteSize_[i]);
	}
	//トランジション
	Transition();
	//PushA
	ImGui::DragFloat2("PushAPosition", &pushASpritePosition_.x);
	ImGui::DragFloat2("PushAScale", &pushASpriteScale_.x);
	pushASprite_->SetPosition(pushASpritePosition_);
	pushASprite_->SetScale(pushASpriteScale_);
	ImGui::End();
}

void StageSelectScene::Draw(){
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライトの描画
	backGroundSprite_->Draw();

	//矢印のスプライトの描画
	if (isMovementEnabled_)
	{
		for (uint32_t i = 0; i < arrowSprites_.size(); i++)
		{
			arrowSprites_[i]->Draw();
		}
	}

	//チュートリアルのスプライトの描画
	for (uint32_t i = 0; i < tutorialSprites_.size(); i++)
	{
		tutorialSprites_[i]->Draw();
	}

	//ステージのスプライトの描画
	for (uint32_t i = 0; i < kMaxStages; ++i)
	{
		stageSprites_[i]->Draw();
		numberSprites_[i]->Draw();
	}

	//ステージ画面のスプライトの描画
	for (uint32_t i = 0; i < stageScreenSprites_.size(); i++)
	{
		stageScreenSprites_[i]->Draw();
	}

	//PushAのスプライトの描画
	pushASprite_->Draw();


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

	
	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void StageSelectScene::DrawBackGround() {

}

void StageSelectScene::Transition() {
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
			if (selectNumber_ == 0)
			{
				sceneManager_->ChangeScene("TutorialScene1");
			}
			else
			{
				preSelectNumber_ = selectNumber_;
				StageSelectScene::stageNumber_ = selectNumber_ + 1;
				sceneManager_->ChangeScene("GamePlayScene");
			}
			timer_ = 0.0f;
		}
	}

	PostEffects::GetInstance()->GetVignette()->SetIntensity(timer_);

}