#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"

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
	enemyModel_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemyModel_.get());

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
		enemy_->SetBlockPosition(block.get()->GetWorldPosition());
		enemy_->SetBlockSize(block.get()->GetSize());
	}
	
	//敵の更新
	enemy_->SetPlayerPosition(player_->GetWorldPosition());
	enemy_->Update();

	//コピーの更新
	copyManager_->Update();

	//カメラの更新
	camera_.UpdateMatrix();

	//コライダーをクリア
	collisionManager_->ClearColliderList();
	//プレイヤー
	collisionManager_->SetColliderList(player_.get());
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
	//衝突判定
	collisionManager_->CheckAllCollisions();

	//プレイヤーの座標を保存
	copyManager_->SetPlayerPosition(player_->GetWorldPosition());

	//リセット処理
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
			Reset();
		}
	}

	//ImGui
	ImGui::Begin("GamePlayScene");
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
 
	enemy_->Draw(camera_);

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


void GamePlayScene::Reset()
{
	//プレイヤーをリセット
	player_->Reset();
	
	//コピーをリセット
	copyManager_->Reset();
}
