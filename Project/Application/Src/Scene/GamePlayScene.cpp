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

	//背景の生成
	backGroundGenkoModel_.reset(ModelManager::CreateFromModelFile("genko.gltf", Opaque));
	backGroundFrameModel_.reset(ModelManager::CreateFromModelFile("youtubes.gltf", Opaque));
	backGroundMovieModel_.reset(ModelManager::CreateFromModelFile("Plane.obj", Opaque));
	backGroundMovieModel_->GetMaterial(1)->SetTexture(Renderer::GetInstance()->GetBackGroundColorDescriptorHandle());
	backGroundMovieModel_->GetMaterial(1)->SetEnableLighting(false);
	std::vector<Model*> backGroundModels = { backGroundGenkoModel_.get(),backGroundFrameModel_.get(),backGroundMovieModel_.get() };
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize(backGroundModels);

	//FollowCameraの生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&player_->GetWorldTransform());

	//スプライトの生成
	TextureManager::Load("cont.png");
	contSprite_.reset(Sprite::Create("cont.png", spritePosition_));

	//パーティクルマネージャーのインスタンスを取得
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//音声データ読み込み
	whiffAudioHandle_ = audio_->LoadAudioFile("Application/Resources/Sounds/Whiff.wav");
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	if (!isReversed_)
	{
		//プレイヤーの更新
		player_->Update();

		//ブロックの更新
		blockManager_->Update();

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
		backGround_->Update();

		//FollowCameraの更新
		followCamera_->Update();

		//カメラの更新
		camera_.UpdateMatrix();

		//コライダーをクリア
		collisionManager_->ClearColliderList();
		//プレイヤー
		collisionManager_->SetColliderList(player_.get());
		//敵
		for (const std::unique_ptr<Enemy>& enemy : enemyManager_->GetEnemies())
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
		//コピー
		for (const std::unique_ptr<Copy>& copy : copyManager_->GetCopies())
		{
			collisionManager_->SetColliderList(copy.get());
			collisionManager_->SetColliderList(copy->GetWeapon());
		}
		//衝突判定
		collisionManager_->CheckAllCollisions();

		//プレイヤーが動けるときに逆再生時のデータを保存
		if (!player_->GetIsStop())
		{
			//プレイヤーの座標を保存
			copyManager_->SetPlayerPosition(player_->GetWorldPosition(), player_->GetWeapon()->GetIsAttack(), player_->GetAnimationNumber(), player_->GetAnimationTime());
		}

		//敵の逆再生時に必要なデータを保存
		enemyManager_->SaveEnemyPositions();

		//ゲームクリア
		bool isClear = true;
		const std::vector<std::unique_ptr<Enemy>>& enemies = enemyManager_->GetEnemies();
		if (enemies.size() != 0)
		{
			for (const std::unique_ptr<Enemy>& enemy : enemies)
			{
				if (enemy->GetIsActive())
				{
					isClear = false;
				}
			}
			if (isClear)
			{
				sceneManager_->ChangeScene("GameClearScene");
				GameClearScene::SetTimeCount(int(dislikes_));
			}
		}

		//ゲームオーバー
		if (dislikes_ >= 99) {
			sceneManager_->ChangeScene("GameOverScene");
		}

		//リセット処理
		if (player_->GetWeapon()->GetIsAttack() && !isClear)
		{
			if (!player_->GetWeapon()->GetIsHit())
			{
				audio_->PlayAudio(whiffAudioHandle_, 0, 0.4f);
			}
			reversePlayerPositions = copyManager_->GetPlayerPositions();
			isReversed_ = true;
			player_->StopAnimation();
			CalculateRating();
			Reset();
		}

		//コントローラーのUIの座標とサイズを設定
		contSprite_->SetPosition(spritePosition_);
		contSprite_->SetSize(spriteScale_);

		//評価の計算
		CalculateRating();

		//ノイズエフェクトを無効化
		PostEffects::GetInstance()->GetGlitchNoise()->SetIsEnable(false);
	}
	else
	{
		if (!reversePlayerPositions.empty())
		{
			//逆再生処理
			Reverse();

			//ノイズエフェクトを有効化
			PostEffects::GetInstance()->GetGlitchNoise()->SetIsEnable(true);

			//FollowCameraの更新
			followCamera_->Update();
		}
		else
		{
			isReversed_ = false;
			copyManager_->AddCopy();
			player_->PlayAnimation();
		}
	}


	//パーティクルの更新
	particleManager_->Update();

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

	

	//ImGui
	ImGui::Begin("GamePlayScene");
	ImGui::DragFloat2("SpritePosition", &spritePosition_.x, 0.1f);
	ImGui::DragFloat2("SpriteScale", &spriteScale_.x, 0.01f);
	if (ImGui::TreeNode("Rating")) {
		ImGui::Text("EnemyNum %d", enemyNum_);
		ImGui::Text("Likes%d", likes_);
		ImGui::Text("disLikes %f", dislikes_);
		ImGui::Text("totalScore %d", totaScore_);
		ImGui::TreePop();
	}

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
}

void GamePlayScene::DrawUI()
{
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
	}

	//敵を逆再生
	enemyManager_->Reverse(stepSize_);

	//コピーを逆再生
	copyManager_->Reverse(stepSize_);
}