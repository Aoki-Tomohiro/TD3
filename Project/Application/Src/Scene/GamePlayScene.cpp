#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Application/Src/Scene/GameClearScene.h"
#include "Application/Src/Scene/StageSelectScene.h"

uint32_t GamePlayScene::currentStageNumber = StageSelectScene::stageNumber_;

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//ステージ番号の読み込み
	GamePlayScene::currentStageNumber = StageSelectScene::stageNumber_;

	//カメラの初期化
	camera_.Initialize();
	camera_.fov_ = 45.0f * 3.141592654f / 180.0f;

	
	//コピーの数のスプライトの生成
	timeCountSprites_[0].reset(Sprite::Create("Numbers/0.png", timeCountSpritePositions_[0]));
	timeCountSprites_[1].reset(Sprite::Create("Numbers/0.png", timeCountSpritePositions_[1]));

	//敵の生成
	enemyModel_.reset(ModelManager::CreateFromModelFile("Human.gltf", Opaque));
	enemyModel_->GetMaterial(0)->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(enemyModel_.get(), currentStageNumber);
	enemyManager_->SaveReverseData();
	enemyNum_ = int(enemyManager_->GetEnemies().size());

	//衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//プレイヤーを生成
	playerModel_.reset(ModelManager::CreateFromModelFile("Human.gltf", Opaque));
	playerModel_->GetMaterial(0)->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	weaponModel_.reset(ModelManager::CreateFromModelFile("Cube.obj", Transparent));
	std::vector<Model*> playerModels = { playerModel_.get(),weaponModel_.get() };
	player_ = std::make_unique<Player>();
	player_->Initialzie(playerModels);

	//ブロックを生成
	blockModel_.reset(ModelManager::Create());
	blockModel_->GetMaterial(1)->SetColor({ 0.196f,0.196f,0.196f,1.0f });
	blockManager_ = std::make_unique<BlockManager>();
	blockManager_->Initialize(blockModel_.get(),currentStageNumber);

	//コピーを生成
	copyModel_.reset(ModelManager::CreateFromModelFile("Human.gltf", Opaque));
	copyModel_->GetMaterial(0)->SetColor({ 0.2118f, 0.8196f, 0.7137f, 1.0f });
	copyManager_ = std::make_unique<CopyManager>();
	copyManager_->Initialize();
	copyManager_->SetPlayerData(player_->GetWorldPosition(), player_->GetWeapon()->GetIsAttack(), player_->GetAnimationNumber(), player_->GetAnimationTime());

	//背景の生成
	backGroundGenkoModel_.reset(ModelManager::CreateFromModelFile("genko.gltf", Opaque));
	backGroundFrameModel_.reset(ModelManager::CreateFromModelFile("youtubes.gltf", Opaque));
	backGroundMovieModel_.reset(ModelManager::CreateFromModelFile("Plane.obj", Opaque));
	backGroundMovieModel_->GetMaterial(1)->SetTexture(Renderer::GetInstance()->GetBackGroundColorDescriptorHandle());
	backGroundMovieModel_->GetMaterial(1)->SetEnableLighting(false);
	std::vector<Model*> backGroundModels = { backGroundGenkoModel_.get(),backGroundFrameModel_.get(),backGroundMovieModel_.get() };
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize(backGroundModels);

	//スコアの生成
	score_ = std::make_unique<Score>();
	score_->Initialize();

	TextureManager::Load("back.png");
	backSprite_.reset(Sprite::Create("back.png", { 0.0f,0.0f }));
	backSprite_->SetColor({ 1.0f,1.0f,1.0f,0.8f });
	TextureManager::Load("pause.png");
	pauseSprite_.reset(Sprite::Create("pause.png", { 0.0f,0.0f }));
	TextureManager::Load("pauseUI.png");
	pauseUISprite_.reset(Sprite::Create("pauseUI.png", { 80.0f,600.0f }));
	TextureManager::Load("yaji.png");
	yajiSprite_.reset(Sprite::Create("yaji.png", { 0.0f,0.0f }));


	//倍速スプライト
	TextureManager::Load("yajirusiz.png");
	doubleSprite_.reset(Sprite::Create("yajirusiz.png", { 640.0f,360.0f }));
	doubleSprite_->SetAnchorPoint({ 0.5f,0.5f });

	//巻き戻しスプライト
	//TextureManager::Load("pause.png");
	reversedSprite_.reset(Sprite::Create("yajirusiz.png", { 640.0f,360.0f }));
	reversedSprite_->SetAnchorPoint({ 0.5f,0.5f });

	//パーティクルマネージャーのインスタンスを取得
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//Switchの生成
	switchManager_ = std::make_unique<SwitchManager>();
	switchManager_->Initialize(currentStageNumber);
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	

	//ポーズメニュ
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_START) || input_->IsPushKeyEnter(DIK_P))
	{
		if (!pause_) {
			pause_ = true;
		}
		else {
			pause_ = false;
			rule_ = false;
		}


	}

	if (!pause_) {
		//逆再生中だったら
		if (isReversed_)
		{
			//過去のプレイヤーの情報があるとき
			if (!reversePlayerPositions.empty())
			{
				//逆再生処理
				Reverse();

				//パーティクルの更新
				particleManager_->Update();
			}
			//過去のプレイヤーの情報がなくなったら
			else
			{
				//逆再生のフラグを折る
				isReversed_ = false;
				//コピーを出す
				copyManager_->AddCopy();
				//プレイヤーのアニメーションを再生
				player_->PlayAnimation();
				//最初の情報を保存
				copyManager_->SetPlayerData(player_->GetWorldPosition(), player_->GetWeapon()->GetIsAttack(), player_->GetAnimationNumber(), player_->GetAnimationTime());
				enemyManager_->SaveReverseData();
				//スコアをリセット
				score_->Reset();
				//倍速をなくす
				isDoubleSpeed_ = false;
				copyManager_->SetIsDoubleSpeed(false);
				enemyManager_->SetIsDoubleSpeed(false);
				//ノイズエフェクト無効化
				PostEffects::GetInstance()->GetGlitchNoise()->SetIsEnable(false);
			}
			//後の処理を飛ばす
			return;
		}

		//プレイヤーの更新
		player_->Update();

		//ブロックの更新
		blockManager_->Update();

		//スイッチの更新
		switchManager_->Update();

		//敵にブロックの情報を渡す
		const std::vector<std::unique_ptr<Block>>& blocks = blockManager_->GetBlocks();
		for (const std::unique_ptr<Block>& block : blocks)
		{
			enemyManager_->SetBlockData(block->GetWorldPosition(), block->GetSize());
		}

		//コピーの更新
		copyManager_->Update();

		//敵の更新
		enemyManager_->SetPlayerPosition(player_->GetWorldPosition());
		enemyManager_->SetCopy(copyManager_->GetCopies());
		enemyManager_->Update();

		//背景の更新
		backGround_->SetPlayerPosition(player_->GetWorldPosition());
		backGround_->Update();

		//カメラの更新
		camera_.UpdateMatrix();

		//コライダーをクリア
		collisionManager_->ClearColliderList();
		//プレイヤー
		collisionManager_->SetColliderList(player_.get());
		//敵
		const std::vector<std::unique_ptr<Enemy>>& enemies = enemyManager_->GetEnemies();
		for (const std::unique_ptr<Enemy>& enemy : enemies)
		{
			//編集中なら飛ばす
			if (enemy->GetIsEdit())
			{
				continue;
			}

			//ColliderListに登録
			if (enemy->GetIsActive())
			{
				collisionManager_->SetColliderList(enemy.get());
			}
		}
		//武器
		collisionManager_->SetColliderList(player_->GetWeapon());
		//ブロック
		for (const std::unique_ptr<Block>& block : blocks)
		{
			collisionManager_->SetColliderList(block.get());
		}
		//スイッチ
		const std::vector<std::unique_ptr<Switch>>& switches = switchManager_->GetSwitches();
		for (const std::unique_ptr<Switch>& Switch : switches)
		{
			collisionManager_->SetColliderList(Switch.get());
			Wall* wall = Switch->GetWall();
			if (wall->GetIsActive())
			{
				collisionManager_->SetColliderList(wall);
			}
		}
		//コピー
		const std::vector<std::unique_ptr<Copy>>& copies = copyManager_->GetCopies();
		for (const std::unique_ptr<Copy>& copy : copies)
		{
			collisionManager_->SetColliderList(copy.get());
			collisionManager_->SetColliderList(copy->GetWeapon());
		}
		//衝突判定
		collisionManager_->CheckAllCollisions();

		//プレイヤーが動いているときののデータを保存
		if (!player_->GetIsStop())
		{
			//プレイヤーの座標を保存
			copyManager_->SetPlayerData(player_->GetWorldPosition(), player_->GetWeapon()->GetIsAttack(), player_->GetAnimationNumber(), player_->GetAnimationTime());
		}

		//逆再生に必要なプレイヤーのデータを保存
		reversePlayerPositions.push_back({ player_->GetWorldPosition(), player_->GetWeapon()->GetIsAttack(), player_->GetAnimationNumber(), player_->GetAnimationTime() });
		if (isDoubleSpeed_)
		{
			for (uint32_t i = 0; i < 4; i++)
			{
				reversePlayerPositions.push_back({ player_->GetWorldPosition(), player_->GetWeapon()->GetIsAttack(), player_->GetAnimationNumber(), player_->GetAnimationTime() });
			}
		}

		//敵の逆再生時に必要なデータを保存
		enemyManager_->SaveReverseData();

		//スコアの更新
		score_->Update(player_.get(), copies);

		//評価の計算
		CalculateRating();

		//パーティクルの更新
		particleManager_->Update();

		//ゲームクリア
		bool isClear = true;
		bool isOver = true;
		if (enemies.size() != 0)
		{
			for (const std::unique_ptr<Enemy>& enemy : enemies)
			{
				if (!enemy->GetIsGameOver())
				{
					isOver = false;
				}


				if (!enemy->GetIsResult())
				{
					isClear = false;
				}
			}
			if (isClear)
			{
				isFadeOut_ = true;
				nextScene_ = kClear;
				copyManager_->SetIsDoubleSpeed(false);
				enemyManager_->SetIsDoubleSpeed(false);
				GameClearScene::SetTimeCount(int(dislikes_));
				GameClearScene::SetScore(score_->GetScore());
			}

			if (isOver)
			{
				isFadeOut_ = true;
				nextScene_ = kOver;
				copyManager_->SetIsDoubleSpeed(false);
				enemyManager_->SetIsDoubleSpeed(false);
			}
		}

		//ゲームオーバー
		if (dislikes_ >= 99) {
			isFadeOut_ = true;
			nextScene_ = kOver;
		}

		//ゲームーオーバー
		int time = 60 - int(dislikes_);
		if (time < 0)
		{
			sceneManager_->ChangeScene("GameOverScene");
		}
		else
		{
			if (time > 99)
			{
				timeCountSprites_[0]->SetTexture("Numbers/9.png");
				timeCountSprites_[1]->SetTexture("Numbers/9.png");
			}
			else
			{
				timeCountSprites_[0]->SetTexture("Numbers/" + std::to_string(time / 10) + ".png");
				time %= 10;
				timeCountSprites_[1]->SetTexture("Numbers/" + std::to_string(time) + ".png");
			}

			//スプライトの座標を設定
			for (uint32_t i = 0; i < 2; i++)
			{
				timeCountSprites_[i]->SetPosition(timeCountSpritePositions_[i]);
				timeCountSprites_[i]->SetScale(SpriteSize_[i]);
			}
		}

		//プレイヤーが攻撃終わった後のコピーの動きを倍速にする
		if (player_->GetIsStop())
		{
			if (!isDoubleSpeed_)
			{
				if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
				{
					isDoubleSpeed_ = true;
					copyManager_->SetIsDoubleSpeed(true);
					enemyManager_->SetIsDoubleSpeed(true);
					PostEffects::GetInstance()->GetGlitchNoise()->SetIsEnable(true);
					PostEffects::GetInstance()->GetGlitchNoise()->SetNoiseType(1);
				}

				if (input_->IsPushKeyEnter(DIK_F))
				{
					isDoubleSpeed_ = true;
					copyManager_->SetIsDoubleSpeed(true);
					enemyManager_->SetIsDoubleSpeed(true);
					PostEffects::GetInstance()->GetGlitchNoise()->SetIsEnable(true);
					PostEffects::GetInstance()->GetGlitchNoise()->SetNoiseType(1);
				}
			}
		}

		//リセットのフラグ
		bool isReset = true;
		if (isClear)
		{
			isReset = false;
		}
		if (!player_->GetIsStop())
		{
			isReset = false;
		}
		for (const std::unique_ptr<Copy>& copy : copies)
		{
			if (copy->GetIsActive())
			{
				isReset = false;
			}
		}

		//プレイヤーの攻撃が終了したらリセット
		
		if (isReset)
		{
			//逆再生のフラグを立てる
			isReversed_ = true;
			//アニメーションを停止
			player_->StopAnimation();
			//リセット
			Reset();
			//ノイズエフェクトを有効化
			PostEffects::GetInstance()->GetGlitchNoise()->SetIsEnable(true);
			PostEffects::GetInstance()->GetGlitchNoise()->SetNoiseType(0);
		}
		
	}
  	
	//ポーズ
	Pause();

	//トランジション
	Transition();

	//ImGui
	ImGui::Begin("GamePlayScene");
	if (ImGui::TreeNode("Rating")) {
		ImGui::Text("EnemyNum %d", enemyNum_);
		ImGui::Text("Likes%d", likes_);
		ImGui::Text("disLikes %f", dislikes_);
		ImGui::Text("totalScore %d", totaScore_);
		ImGui::TreePop();
	}
	ImGui::DragFloat2("A", &a.x);
	ImGui::Text("%f", cursorPosition_.y);
	ImGui::DragFloat2("SpritePositon[0]", &timeCountSpritePositions_[0].x);
	ImGui::DragFloat2("SpritePositon[1]", &timeCountSpritePositions_[1].x);
	ImGui::DragFloat2("SpriteSize[0]", &SpriteSize_[0].x,1.0f,1.0f,3.0f);
	ImGui::DragFloat2("SpriteSize[1]", &SpriteSize_[1].x,1.0f,1.0f,3.0f);
	ImGui::End();
}

void GamePlayScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景の描画
	backGround_->DrawSprite();

	

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//プレイヤーの描画
	player_->Draw(camera_);

	//敵の描画
	enemyManager_->Draw(camera_);

	//ブロックの描画
	blockManager_->Draw(camera_);

	//スイッチの描画
	switchManager_->Draw(camera_);

	//コピーの描画
	copyManager_->Draw(camera_);

	//背景の描画
	backGround_->Draw(camera_);

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクルの描画
	particleManager_->Draw(camera_);

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion

#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//プレイヤーのUIの描画
	player_->DrawUI(camera_);

	//コピー上限のUIの描画
	copyManager_->DrawUI();

	//timeの描画
	for (uint32_t i = 0; i < 2; i++)
	{
		timeCountSprites_[i]->Draw();
	}

	//スコアの描画
	score_->Draw();

	

	if (pause_) {
		backSprite_->Draw();
		yajiSprite_->Draw();
		pauseSprite_->Draw();
	}
	else {

	}
	pauseUISprite_->Draw();
	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GamePlayScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);


	if (isReversed_) {
		reversedSprite_->Draw();
	}

	if (isDoubleSpeed_ && !isReversed_) {
		doubleSprite_->Draw();
	}

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GamePlayScene::DrawBackGround()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

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

#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GamePlayScene::Reset()
{
	//プレイヤーをリセット
	player_->Reset();
	
	//コピーをリセット
	copyManager_->Reset();

	//敵をリセット
	enemyManager_->Reset();
}

void GamePlayScene::CalculateRating() {
	//毎秒今いる敵の数*1ずつ減っていく
	dislikes_ += (1.0f / 60.0f);

	const std::vector<std::unique_ptr<Enemy>>& enemies = enemyManager_->GetEnemies();
	for (int i = 0; i < copyManager_->GetCopies().size(); ++i)
	{
		if (copyManager_->GetCopies()[i]->GetWeapon()->GetIsAttack() && !player_->GetWeapon()->GetIsAttack()) {
			num_ = 0;

			for (int i = 0; i < enemies.size(); ++i)
			{
				if (!enemies[i].get()->GetIsActive()) {
					num_++;
				}
			}

			if (defeatedEnemyCount - num_ <= -2) {
				likes_ += 50 * (num_ - defeatedEnemyCount);
			}
			else {
				if (defeatedEnemyCount - num_ != 0) {
					likes_ += 25;
				}

			}


			defeatedEnemyCount = num_;

			enemyNum_ = enemyNum_ - defeatedEnemyCount;
		}

	}


	if (player_->GetWeapon()->GetIsAttack())
	{
		num_ = 0;
		for (int i = 0; i < enemies.size(); ++i)
		{
			if (!enemies[i].get()->GetIsActive()) {
				num_++;
			}
		}

		if (defeatedEnemyCount - num_ <= -2) {
			likes_ += 100 * (num_ - defeatedEnemyCount);
		}

		if (defeatedEnemyCount - num_ == -1) {
			likes_ += 50;
		}



		defeatedEnemyCount = 0;
		enemyNum_ = int(enemies.size());
	}
	totaScore_ = likes_ - int(dislikes_);
}

void GamePlayScene::Reverse()
{
	//プレイヤーを逆再生
	auto it = reversePlayerPositions.back();
	reversePlayerPositions.pop_back();
	player_->SetPositions(std::get<0>(it), std::get<1>(it), std::get<2>(it), std::get<3>(it));
	for (uint32_t i = 0; i < stepSize_; ++i)
	{
		if (!reversePlayerPositions.empty())
		{
			reversePlayerPositions.pop_back();
		}
		else
		{
			//位置ズレ対策
			player_->SetPositions({ 0.0f,-10.0f,0.0f }, false, 1, 0.0f);
		}
	}

	//敵を逆再生
	enemyManager_->Reverse(stepSize_);

	//コピーを逆再生
	copyManager_->Reverse(stepSize_);
}

void GamePlayScene::Transition() {
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

			if (nextScene_ == kTitle) {
				sceneManager_->ChangeScene("GameTitleScene");
			}
			if (nextScene_ == kSelect) {
				sceneManager_->ChangeScene("StageSelectScene");
			}
			if (nextScene_ == kClear) {
				sceneManager_->ChangeScene("GameClearScene");
			}
			if (nextScene_ == kOver) {
				sceneManager_->ChangeScene("GameOverScene");
			}
			
			
			timer_ = 0.0f;
		}
	}

	PostEffects::GetInstance()->GetVignette()->SetIntensity(timer_);

}

void GamePlayScene::Pause() {


	


	if (pause_) {

		if (!isCursorMovementEnabled_) {
			const uint32_t kEnableTime = 10;
			if (++cursorMovementEnableTimer_ >= kEnableTime) {
				isCursorMovementEnabled_ = true;
				cursorMovementEnableTimer_ = 0;
			}
		}

		bool isSelect = false;

		const float threshold = 0.8f;

		Vector3 stickTilt{
			0.0f,
			input_->GetLeftStickY(),
			0.0f
		};

		if (Mathf::Length(stickTilt) > threshold && !rule_) {
			if (isCursorMovementEnabled_) {

				//上下にカーソルを動かす
				if (stickTilt.y < -threshold) {
					cursorPosition_.y += cursorVelocity_.y;
					isCursorMovementEnabled_ = false;
				}
				else if (stickTilt.y > threshold) {
					cursorPosition_.y -= cursorVelocity_.y;
					isCursorMovementEnabled_ = false;
				}
			}
		}

		if (isCursorMovementEnabled_) {
			if (input_->IsPushKeyEnter(DIK_S)) {
				cursorPosition_.y += cursorVelocity_.y;
				isCursorMovementEnabled_ = false;
			}
			else if (input_->IsPushKeyEnter(DIK_W))
			{
				cursorPosition_.y -= cursorVelocity_.y;
				isCursorMovementEnabled_ = false;
			}
		}

		

		const float kPosMinY = -40.0f;
		const float kPosMaxY = 160.0f;

		cursorPosition_.y = std::clamp(cursorPosition_.y, kPosMinY, kPosMaxY);

		//スプライトの座標を設定
		yajiSprite_->SetPosition(cursorPosition_);

		//ゲームタイトル画面
		if (cursorPosition_.y == -40.0f && (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A)||input_->IsPushKeyEnter(DIK_SPACE)))
		{
			isFadeOut_ = true;
			nextScene_ = kTitle;
		}

		//セレクト画面
		if (cursorPosition_.y == 60.0f && (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A)||input_->IsPushKeyEnter(DIK_SPACE))) {
			isFadeOut_ = true;
			nextScene_ = kSelect;
		}

		//ルール画面
		if (rule_ && input_->IsPressButtonEnter(XINPUT_GAMEPAD_A)) {
			rule_ = false;
		}
		else if (cursorPosition_.y == 120.0f && input_->IsPressButtonEnter(XINPUT_GAMEPAD_A)) {
			rule_ = true;
		}

	}
	else {
		cursorPosition_.y = -40.0f;
	}
}