#include "StageSelectScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include <Engine/Math/MathFunction.h>
#include <numbers>
#include <Engine/Components/PostEffects/PostEffects.h>

uint32_t StageSelectScene::stageNumber_ = 0;
uint32_t StageSelectScene::preSelectNumber_ = 0;
std::array<uint32_t, StageSelectScene::kMaxStages> StageSelectScene::stageScores_{};
std::array<std::string, StageSelectScene::kMaxStages> StageSelectScene::stageEvaluations_;

void StageSelectScene::Initialize() {
	//camera_.Initialize();
	//camera_.fov_ = 45.0f * 3.141592654f / 180.0f;

	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//前の選択したステージに設定
	selectNumber_ = preSelectNumber_;

	//背景スプライトの生成
	TextureManager::Load("selectBack.png");
	backGroundSprite_.reset(Sprite::Create("selectBack.png", { 0.0f,0.0f }));

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
		stageSpriteTargetPosition_[i] = { 430.0f + (tutorialSprites_.size() + i) * delta_,20.0f };
		stageSpritePosition_[i] = stageSpriteTargetPosition_[i];
		numberSpriteTargetPosition_[i] = { 760.0f + (tutorialSprites_.size() + i) * delta_,20.0f };
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

	//スコアのスプライトの生成
	TextureManager::Load("Scores/S.png");
	TextureManager::Load("Scores/A.png");
	TextureManager::Load("Scores/B.png");
	TextureManager::Load("Scores/C.png");
	TextureManager::Load("NoScore.png");
	TextureManager::Load("ScoreFont.png");
	TextureManager::Load("EvaluationFont.png");
	for (uint32_t i = 0; i < scoreSprites_.size(); i++)
	{
		scoreFontSprites_[i].reset(Sprite::Create("ScoreFont.png", { 0.0f,0.0f }));
		for (uint32_t j = 0; j < scoreSprites_[i].size(); j++)
		{
			scoreSprites_[i][j].reset(Sprite::Create("Numbers/0.png", { 0.0f,0.0f }));
		}
	}
	for (uint32_t i = 0; i < evaluationSprites_.size(); i++)
	{
		evaluationFontSprites_[i].reset(Sprite::Create("EvaluationFont.png", { 0.0f,0.0f }));
		evaluationSprites_[i].reset(Sprite::Create("Scores/S.png", { 0.0f,0.0f }));
	}

	//PushAのスプライトの作成
	TextureManager::Load("PushA2.png");
	pushASprite_.reset(Sprite::Create("PushA2.png", { 0.0f,0.0f }));
	pushASpritePosition_ = { 890.0f,600.0f };
	pushASpriteScale_ = { 0.6f,0.6f };

	//音声データの読み込み
	selectSEHandle_ = audio_->LoadAudioFile("selectSE.wav");
	decisionHandle_ = audio_->LoadAudioFile("Decision.wav");
	selectBGMHandle_ = audio_->LoadAudioFile("selectBGM.wav");

	audio_->PlayAudio(selectBGMHandle_, true, 0.3f);


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
					isMovementEnabled_ = false;
					//チュートリアルを選択しているときに左入力した場合最後のステージに飛ばす
					if (selectNumber_ == 0)
					{
						delta = -delta_ * kMaxStages;
						selectNumber_ = kMaxStages;
					}
					else
					{
						--selectNumber_;
						delta = delta_;
					}
					audio_->PlayAudio(selectSEHandle_, false, 0.3f);
				}
				else if (stickTilt.x > threshold) {
					isMovementEnabled_ = false;
					//最後のステージを選択しているときに右入力をした場合チュートリアルに戻す
					if (selectNumber_ == kMaxStages)
					{
						delta = delta_ * kMaxStages;
						selectNumber_ = 0;
					}
					else
					{
						++selectNumber_;
						delta = -delta_;
					}
					audio_->PlayAudio(selectSEHandle_, false, 0.3f);
				}
				//ステージ上限を超えないようにする
				if (selectNumber_ < 0)
				{
					selectNumber_ = 0;
				}
				else if (selectNumber_ > kMaxStages)
				{
					selectNumber_ = kMaxStages;
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
			audio_->PlayAudio(selectSEHandle_, false, 0.3f);
		}
		else if (input_->IsPushKeyEnter(DIK_A)) {
			--selectNumber_;
			isMovementEnabled_ = false;
			delta = delta_;
			audio_->PlayAudio(selectSEHandle_, false, 0.3f);
		}

		if (selectNumber_ < 0)
		{
			selectNumber_ = 0;
		}
		else if (selectNumber_ > kMaxStages)
		{
			selectNumber_ = kMaxStages;
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
		easingParameter_ += 1.0f / 15.0f;
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

	if (!isFadeOut_ && !isFadeIn_) {
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

	//スコアの計算
	for (uint32_t i = 0; i < kMaxStages; i++)
	{
		//スコアのテクスチャを設定
		int currentScore = stageScores_[i];
		scoreSprites_[i][0]->SetTexture("Numbers/" + std::to_string(currentScore / 10000) + ".png");
		currentScore %= 10000;
		scoreSprites_[i][1]->SetTexture("Numbers/" + std::to_string(currentScore / 1000) + ".png");
		currentScore %= 1000;
		scoreSprites_[i][2]->SetTexture("Numbers/" + std::to_string(currentScore / 100) + ".png");
		currentScore %= 100;
		scoreSprites_[i][3]->SetTexture("Numbers/" + std::to_string(currentScore / 10) + ".png");
		currentScore %= 10;
		scoreSprites_[i][4]->SetTexture("Numbers/" + std::to_string(currentScore) + ".png");

		//スコアが無かったら評価を表示しない
		if (stageScores_[i] == 0)
		{
			evaluationSprites_[i]->SetTexture("NoScore.png");
		}
		else
		{
			evaluationSprites_[i]->SetTexture("Scores/" + stageEvaluations_[i] + ".png");
		}
	}

	ImGui::Begin("SelectScene");
	ImGui::Text("SelectNumber : %d", selectNumber_);
	//矢印
	for (uint32_t i = 0; i < arrowSprites_.size(); i++)
	{
		arrowSprites_[i]->SetPosition(arrowSpritePosition_[i]);
	}
	//チュートリアル
	for (uint32_t i = 0; i < tutorialSprites_.size(); i++)
	{
		tutorialSprites_[i]->SetPosition(tutorialSpritePosition_[i]);
	}
	//ステージ
	for (uint32_t i = 0; i < kMaxStages; i++)
	{
		std::string stagePositionName = "StageSpritePosition" + std::to_string(i);
		ImGui::DragFloat2(stagePositionName.c_str(), &stageSpritePosition_[i].x);
		std::string stageSizeName = "StageSpriteSize" + std::to_string(i);
		ImGui::DragFloat2(stageSizeName.c_str(), &stageSpriteSize_[i].x);
		stageSprites_[i]->SetPosition(stageSpritePosition_[i]);
		numberSprites_[i]->SetPosition(numberSpritePosition_[i]);
	}
	//スコア
	for (uint32_t i = 0; i < scoreSprites_.size(); i++)
	{
		Vector2 offset = { scoreOffset_.x + 6.0f,scoreOffset_.y - 40.0f };
		scoreFontSprites_[i]->SetPosition(stageSpritePosition_[i] + offset);
		for (uint32_t j = 0; j < scoreSprites_[i].size(); j++)
		{
			offset = { scoreOffset_.x + (j * scoreInterval_),scoreOffset_.y };
			scoreSprites_[i][j]->SetPosition(stageSpritePosition_[i] + offset);
			scoreSprites_[i][j]->SetScale({0.6f,0.6f});
		}
	}
	for (uint32_t i = 0; i < evaluationSprites_.size(); i++)
	{
		Vector2 evaluationFontOffset = { evaluationOffset_.x + 8.0f,evaluationOffset_.y - 20.0f };
		evaluationFontSprites_[i]->SetPosition(stageSpritePosition_[i] + evaluationFontOffset);
		if (!stageScores_[i] == 0)
		{
			evaluationSprites_[i]->SetPosition(stageSpritePosition_[i] + evaluationOffset_);
		}
		else
		{
			Vector2 noScoreOffset = { evaluationOffset_.x,evaluationOffset_.y - 45.0f };
			evaluationSprites_[i]->SetPosition(stageSpritePosition_[i] + noScoreOffset);
		}
	}
	//ステージ画面
	for (uint32_t i = 0; i < stageScreenSprites_.size(); i++)
	{
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

void StageSelectScene::Draw() {
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

	//スコアのスプライトの描画
	for (uint32_t i = 0; i < scoreSprites_.size(); i++)
	{
		scoreFontSprites_[i]->Draw();
		for (uint32_t j = 0; j < scoreSprites_[i].size(); j++)
		{
			scoreSprites_[i][j]->Draw();
		}
	}
	for (uint32_t i = 0; i < evaluationSprites_.size(); i++)
	{
		evaluationFontSprites_[i]->Draw();
		evaluationSprites_[i]->Draw();
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
				StageSelectScene::stageNumber_ = selectNumber_ + 2;
				sceneManager_->ChangeScene("GamePlayScene");
			}
			audio_->StopAudio(selectBGMHandle_);
			timer_ = 0.0f;
		}
	}

	PostEffects::GetInstance()->GetVignette()->SetIntensity(timer_);

}