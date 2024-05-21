#include "TutorialScene1.h"
#include "Engine/Framework/Scene/SceneManager.h"

void TutorialScene1::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//カメラの初期化
	camera_.Initialize();
	camera_.fov_ = 45.0f * 3.141592654f / 180.0f;

	//衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//プレイヤーを生成
	playerModel_.reset(ModelManager::CreateFromModelFile("Human.gltf", Opaque));
	playerModel_->GetMaterial(0)->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	weaponModel_.reset(ModelManager::CreateFromModelFile("Cube.obj", Transparent));
	std::vector<Model*> playerModels = { playerModel_.get(),weaponModel_.get() };
	player_ = std::make_unique<Player>();
	player_->Initialzie(playerModels);
	player_->SetIsTutorial(true);

	//敵の生成
	enemyModel_.reset(ModelManager::CreateFromModelFile("Human.gltf", Opaque));
	enemyModel_->GetMaterial(0)->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	Enemy* enemy = new Enemy();
	enemy->Initialize(enemyModel_.get(), { 10.0f,-10.0f,0.0f });
	enemy->SetIsTutorial(true);
	enemies_.push_back(std::unique_ptr<Enemy>(enemy));

	//ブロックを生成
	blockModel_.reset(ModelManager::Create());
	blockManager_ = std::make_unique<BlockManager>();
	blockManager_->Initialize(blockModel_.get());
	blockManager_->AddBlock({ 0.0f,-16.0f,0.0f }, { 50.0f,5.0f,1.0f });

	//背景の生成
	backGroundFrameModel_.reset(ModelManager::CreateFromModelFile("youtubes.gltf", Opaque));
	backGroundMovieModel_.reset(ModelManager::CreateFromModelFile("Plane.obj", Opaque));
	backGroundMovieModel_->GetMaterial(1)->SetTexture(Renderer::GetInstance()->GetBackGroundColorDescriptorHandle());
	backGroundMovieModel_->GetMaterial(1)->SetEnableLighting(false);
	std::vector<Model*> backGroundModels = { backGroundFrameModel_.get(),backGroundMovieModel_.get() };
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
}

void TutorialScene1::Finalize()
{
}

void TutorialScene1::Update()
{
	//プレイヤーの更新
	player_->Update();

	//ブロックの更新
	blockManager_->Update();

	//敵の更新
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		if (enemy->GetIsActive())
		{
			enemy->Update();
		}
	}

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
	for (const std::unique_ptr<Block>& block : blockManager_->GetBlocks())
	{
		collisionManager_->SetColliderList(block.get());
	}
	//衝突判定
	collisionManager_->CheckAllCollisions();

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
		sceneManager_->ChangeScene("TutorialScene2");
	}

	//コントローラーのUIの座標とサイズを設定
	contSprite_->SetPosition(spritePosition_);
	contSprite_->SetSize(spriteScale_);

	//パーティクルの更新
	particleManager_->Update();
}

void TutorialScene1::Draw()
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
	//背景の描画
	backGround_->Draw(camera_);

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

void TutorialScene1::DrawUI()
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

void TutorialScene1::DrawBackGround()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

#pragma region 3Dオブジェクト描画
	//プレイヤーの描画
	player_->Draw(followCamera_->GetCamera());

	//敵の描画
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		if (enemy->GetIsActive())
		{
			enemy->Draw(followCamera_->GetCamera());
		}
	}

	//ブロックの描画
	blockManager_->Draw(followCamera_->GetCamera());

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクルの描画
	particleManager_->Draw(followCamera_->GetCamera());

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion
}