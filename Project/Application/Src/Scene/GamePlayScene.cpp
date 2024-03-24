#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//カメラの初期化
	camera_.Initialize();
	camera_.fov_ = 45.0f * 3.141592654f / 180.0f;

	//プレイヤーの生成
	playerModel_.reset(ModelManager::Create());
	playerModel_->SetColor({ 0.0f,0.0f,1.0f,1.0f });
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModel_.get());

	//敵の生成
	enemyModel_.reset(ModelManager::Create());
	enemyModel_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	AddEnemy();

	//ブロックの生成
	blockModel_.reset(ModelManager::Create());
	AddBlock({ -10.0f,-6.0f,0.0f }, { 5.0f,1.0f,1.0f });
	AddBlock({ 0.0f,0.0f,0.0f }, { 5.0f,1.0f,1.0f });
	AddBlock({ 10.0f,-6.0f,0.0f }, { 5.0f,1.0f,1.0f });
	AddBlock({ 0.0f,-16.0f,0.0f }, { 50.0f,5.0f,1.0f });

	//CollisionManagerの生成
	collisionManager_ = std::make_unique<CollisionManager>();
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//敵の更新
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->Update();
	}

	//プレイヤーの更新
	player_->Update();

	//ブロックの更新
	for (const std::unique_ptr<Block>& block : blocks_)
	{
		block->Update();
	}

	//カメラの更新
	camera_.UpdateMatrix();

	//衝突判定
	collisionManager_->ClearColliderList();
	collisionManager_->SetColliderList(player_.get());
	for (const std::unique_ptr<Block>& block : blocks_)
	{
		collisionManager_->SetColliderList(block.get());
	}
	collisionManager_->CheckAllCollisions();

	//プレイヤーの座標を保存
	if (!enemies_[0]->GetIsActive())
	{
		playerPosition_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition_.pop();
		playerPosition_.push(player_->GetWorldPosition());
		enemies_[0]->SetPosition(playerPosition_.front());
		if (!player_->GetIsCopied())
		{
			AddEnemy();
			player_->SetIsCopied(true);
		}
	}

	for (int i = 1; i < enemies_.size(); i++)
	{
		if (!enemies_[i]->GetIsActive())
		{
			enemyPosition_[i - 1].push(enemies_[i - 1]->GetWorldPosition());
		}
		else
		{
			enemyPosition_[i - 1].pop();
			enemyPosition_[i - 1].push(enemies_[i - 1]->GetWorldPosition());
			enemies_[i]->SetPosition(enemyPosition_[i - 1].front());
			if (!enemies_[i - 1]->GetIsCopied())
			{
				AddEnemy();
				enemies_[i - 1]->SetIsCopied(true);
			}
		}
	}
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
	for (const std::unique_ptr<Block>& block : blocks_)
	{
		block->Draw(camera_);
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
}

void GamePlayScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GamePlayScene::AddBlock(const Vector3& position, const Vector3& scale)
{
	Block* newBlock = new Block();
	newBlock->Initialize(blockModel_.get(), position, scale);
	blocks_.push_back(std::unique_ptr<Block>(newBlock));
}

void GamePlayScene::AddEnemy()
{
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(enemyModel_.get());
	enemies_.push_back(std::unique_ptr<Enemy>(newEnemy));
	enemyPosition_.push_back(std::queue<Vector3>());
}