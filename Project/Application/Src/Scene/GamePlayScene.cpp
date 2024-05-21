#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Application/Src/Scene/GameClearScene.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//カメラの初期化
	camera_.Initialize();
	camera_.fov_ = 45.0f * 3.141592654f / 180.0f;



	//敵の生成
	enemyModel_.reset(ModelManager::CreateFromModelFile("Human.gltf", Opaque));
	enemyModel_->GetMaterial(0)->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	AddEnemy({ 0.0f,2.0f,0.0f });
	AddEnemy({ 10.0f,-10.0f,0.0f });
	AddEnemy({ -10.0f,-10.0f,0.0f });
	enemyNum_ = int(enemies_.size());
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
	blockManager_ = std::make_unique<BlockManager>();
	blockManager_->Initialize(blockModel_.get());
	blockManager_->AddBlock({ -10.0f,-6.0f,0.0f }, { 5.0f,1.0f,1.0f });
	blockManager_->AddBlock({ 0.0f,0.0f,0.0f }, { 5.0f,1.0f,1.0f });
	blockManager_->AddBlock({ 10.0f,-6.0f,0.0f }, { 5.0f,1.0f,1.0f });
	blockManager_->AddBlock({ 0.0f,-16.0f,0.0f }, { 50.0f,5.0f,1.0f });

	//コピーを生成
	copyModel_.reset(ModelManager::CreateFromModelFile("Human.gltf", Opaque));
	copyModel_->GetMaterial(0)->SetColor({ 0.2118f, 0.8196f, 0.7137f, 1.0f });
	copyManager_ = std::make_unique<CopyManager>();
	copyManager_->Initialize(copyModel_.get());

	//背景の生成
	backGroundModel_.reset(ModelManager::CreateFromModelFile("genko.gltf", Opaque));
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize(backGroundModel_.get());

	//スプライトの生成
	TextureManager::Load("cont.png");
	contSprite_.reset(Sprite::Create("cont.png", spritePosition_));

	//パーティクルマネージャーのインスタンスを取得
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();
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
		const std::vector<std::unique_ptr<Block>>& blocks = blockManager_->GetBlocks();
		for (const std::unique_ptr<Block>& block : blocks)
		{
			for (const std::unique_ptr<Enemy>& enemy : enemies_)
			{
				if (enemy->GetIsActive())
				{
					enemy->SetBlockPosition(block.get()->GetWorldPosition());
					enemy->SetBlockSize(block.get()->GetSize());
				}
			}
		}

		//コピーの更新
		copyManager_->Update();

		//敵の更新
		for (const std::unique_ptr<Enemy>& enemy : enemies_)
		{
			if (enemy->GetIsActive())
			{
				enemy->SetPlayerPosition(player_->GetWorldPosition());
				enemy->ClearCopy();
				for (const std::unique_ptr<Copy>& copy : copyManager_->GetCopies())
				{
					enemy->SetCopy(copy.get());
				}

				enemy->Update();
			}
		}

		//背景の更新
		backGround_->Update();

		//カメラの更新
		camera_.UpdateMatrix();

		//コライダーをクリア
		collisionManager_->ClearColliderList();
		//プレイヤー
		collisionManager_->SetColliderList(player_.get());
		for (const std::unique_ptr<Enemy>& enemy : enemies_)
		{
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

		//プレイヤーが動けるとき
		if (!player_->GetIsStop())
		{
			//プレイヤーの座標を保存
			copyManager_->SetPlayerPosition(player_->GetWorldPosition(), player_->GetQuaternion(), player_->GetWeapon()->GetIsAttack(), player_->GetAnimationNumber(), player_->GetAnimationTime());
		}

		//ゲームクリア
		bool isClear = true;
		for (const std::unique_ptr<Enemy>& enemy : enemies_)
		{
			if (enemy->GetIsActive())
			{
				isClear = false;
			}
		}
		if (isClear)
		{
			sceneManager_->ChangeScene("GameClearScene");
			GameClearScene::SetCopyCount(copyManager_->GetCopyCount());
		}

		//リセット処理
		if (player_->GetWeapon()->GetIsAttack() && !isClear)
		{
			reversePlayerPositions = copyManager_->GetPlayerPositions();
			isReversed_ = true;
			player_->StopAnimation();
			CalculateRating();
			Reset();
		}

		//コントローラーのUIの座標とサイズを設定
		contSprite_->SetPosition(spritePosition_);
		contSprite_->SetSize(spriteScale_);

		//ノイズエフェクトを無効化
		PostEffects::GetInstance()->GetGlitchNoise()->SetIsEnable(false);
	}
	else
	{
		if (!reversePlayerPositions.empty())
		{
			//プレイヤーを逆再生
			auto it = reversePlayerPositions.back();
			reversePlayerPositions.pop_back();
			player_->SetPosition(std::get<0>(it), std::get<1>(it), std::get<2>(it), std::get<3>(it), std::get<4>(it));
			player_->GetWeapon()->Update();

			//敵を逆再生
			for (const std::unique_ptr<Enemy>& enemy : enemies_)
			{
				enemy->Reverse();
			}

			//コピーを逆再生
			copyManager_->Reverse();

			//ノイズエフェクトを有効化
			PostEffects::GetInstance()->GetGlitchNoise()->SetIsEnable(true);
		}
		else
		{
			isReversed_ = false;
			copyManager_->AddCopy();
			player_->PlayAnimation();
		}
	}

	//評価の計算
	CalculateRating();
	//パーティクルの更新
	particleManager_->Update();

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
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		if (enemy->GetIsActive())
		{
			enemy->Draw(camera_);
		}
	}

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

	//スプライトの描画
	contSprite_->Draw();

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
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->Reset();
	}
}

void GamePlayScene::AddEnemy(const Vector3& position)
{
	Enemy* enemy = new Enemy();
	enemy->Initialize(enemyModel_.get(), position);
	enemies_.push_back(std::unique_ptr<Enemy>(enemy));
	
}

void GamePlayScene::CalculateRating() {
	//毎秒今いる敵の数*1ずつ減っていく
	dislikes_ += (1.0f / 60.0f) * float(enemyNum_);

	for (int i = 0; i < copyManager_->GetCopies().size(); ++i)
	{
		if (copyManager_->GetCopies()[i]->GetWeapon()->GetIsAttack() && !player_->GetWeapon()->GetIsAttack()) {
			num_ = 0;
			
			for (int i = 0; i < enemies_.size(); ++i)
			{
				if (!enemies_[i].get()->GetIsActive()) {
					num_++;
				}
			}

			if (defeatedEnemyCount - num_ <= -2) {
				likes_ += 100 * (num_ - defeatedEnemyCount);
			}
			else {
				likes_ += 50;
			}

			
			defeatedEnemyCount = num_;

			enemyNum_ = enemyNum_ - defeatedEnemyCount;
		}

	}


	if (player_->GetWeapon()->GetIsAttack())
	{
		num_ = 0;
		for (int i = 0; i < enemies_.size(); ++i)
		{
			if (!enemies_[i].get()->GetIsActive()) {
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
		enemyNum_ = int(enemies_.size());
	}
	totaScore_ = likes_ - int(dislikes_);
}