#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
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
	enemyModel_.reset(ModelManager::Create());
	enemyModel_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	AddEnemy({ 0.0f,2.0f,0.0f });
	AddEnemy({ 10.0f,-10.0f,0.0f });
	AddEnemy({ -10.0f,-10.0f,0.0f });

	//衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//プレイヤーを生成
	playerModel_.reset(ModelManager::Create());
	playerModel_->SetColor({ 0.0f,0.0f,1.0f,1.0f });
	weaponModel_.reset(ModelManager::CreateFromOBJ("Cube", Transparent));
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
	copyModel_.reset(ModelManager::Create());
	copyModel_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
	copyManager_ = std::make_unique<CopyManager>();
	copyManager_->Initialize(copyModel_.get());

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
	if (player_->GetWeapon()->GetIsAttack())
	{
		collisionManager_->SetColliderList(player_->GetWeapon());
	}
	//ブロック
	for (const std::unique_ptr<Block>& block : blocks)
	{
		collisionManager_->SetColliderList(block.get());
	}
	//コピー
	for (const std::unique_ptr<Copy>& copy : copyManager_->GetCopies())
	{
		collisionManager_->SetColliderList(copy.get());
		if (copy->GetWeapon()->GetIsAttack())
		{
			collisionManager_->SetColliderList(copy->GetWeapon());
		}
	}
	//衝突判定
	collisionManager_->CheckAllCollisions();

	//パーティクルの更新
	particleManager_->Update();

	//プレイヤーが動けるとき
	if (!player_->GetIsStop())
	{
		//プレイヤーの座標を保存
		copyManager_->SetPlayerPosition(player_->GetWorldPosition(), player_->GetQuaternion(), player_->GetWeapon()->GetIsAttack());
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
		Reset();
	}

	//コントローラーのUIの座標とサイズを設定
	contSprite_->SetPosition(spritePosition_);
	contSprite_->SetSize(spriteScale_);

	//ImGui
	ImGui::Begin("GamePlayScene");
	ImGui::DragFloat2("SpritePosition", &spritePosition_.x, 0.1f);
	ImGui::DragFloat2("SpriteScale", &spriteScale_.x, 0.01f);
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