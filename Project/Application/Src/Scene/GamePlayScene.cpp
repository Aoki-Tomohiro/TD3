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

	//プレイヤーの生成
	playerModel_.reset(ModelManager::Create());
	playerModel_->SetColor({ 0.0f,0.0f,1.0f,1.0f });
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModel_.get());

	//敵の生成
	enemyModel_.reset(ModelManager::Create());
	enemyModel_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	AddEnemy();
	AddEnemy2();
	AddEnemy3();
	AddEnemy4();
	AddEnemy5();
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
	popTimer_++;


	if (popTimer_ >= 60 * 10) {popflag1_ = true;}
	if (popTimer_ >= 60 * 20) {popflag2_ = true;}
	if (popTimer_ >= 60 * 30) {popflag3_ = true;}
	if (popTimer_ >= 60 * 40) {popflag4_ = true;}

	//敵の更新
	for (std::unique_ptr<Enemy> &enemy : enemies_)
	{
		enemy->Update();
		if(!enemy->GetIsActive()){
			//delete enemy.get();
		}

	}

	if (popflag1_) {
		//敵の更新
		for (std::unique_ptr<Enemy>& enemy : enemies2_)
		{
			enemy->Update();
			if (!enemy->GetIsActive()) {
				//delete enemy.get();
			}

		}
	}
	if (popflag2_) {
		//敵の更新
		for (std::unique_ptr<Enemy>& enemy : enemies3_)
		{
			enemy->Update();
			if (!enemy->GetIsActive()) {
				//delete enemy.get();
			}

		}


	}
	if (popflag3_) {
		//敵の更新
		for (std::unique_ptr<Enemy>& enemy : enemies4_)
		{
			enemy->Update();
			if (!enemy->GetIsActive()) {
				//delete enemy.get();
			}

		}


	}
	if (popflag4_) {
		//敵の更新
		for (std::unique_ptr<Enemy>& enemy : enemies5_)
		{
			enemy->Update();
			if (!enemy->GetIsActive()) {
				//delete enemy.get();
			}

		}


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
			
			//enemies_[0]->SetPosition(playerPosition_.front());
			player_->SetIsCopied(true);
		}
		else {

		}

		
	}

	for (int i = 1; i < enemies_.size(); i++)
	{
		if (!enemies_[i]->GetIsActive())
		{
			playerPosition_.push(player_->GetWorldPosition());
			enemyPosition_[i - 1].push(enemies_[i - 1]->GetWorldPosition());
		}
		else
		{
			enemyPosition_[i - 1].pop();
			enemyPosition_[i - 1].push(enemies_[i - 1]->GetWorldPosition());
			//enemyPosition_[i - 1].push(player_->GetWorldPosition());
			enemies_[i]->SetPosition(enemyPosition_[i - 1].front());
			if (!enemies_[i - 1]->GetIsCopied())
			{
				AddEnemy();
				//enemies_[i-1]->SetPosition(player_->GetWorldTransform().translation_);
				enemies_[i - 1]->SetIsCopied(true);
			}

			
		}
	}



	if (popflag1_) {UpdataEnemy2();}
	if (popflag2_) {UpdataEnemy3();}
	if (popflag3_) {UpdataEnemy4();}
	if (popflag4_) {UpdataEnemy5();}


	ImGui::Begin("GamePlayScene");
	ImGui::Text("Translation%fX,%fY,%fZ", player_->GetWorldPosition().x,player_->GetWorldPosition().y,player_->GetWorldPosition().z);
	ImGui::Text("PopTime%d",popTimer_/60);
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


	for (const std::unique_ptr<Enemy>& enemy : enemies2_)
	{
		if (enemy->GetIsActive())
		{
			enemy->Draw(camera_);
		}
	}

	for (const std::unique_ptr<Enemy>& enemy : enemies3_)
	{
		if (enemy->GetIsActive())
		{
			enemy->Draw(camera_);
		}
	}
	for (const std::unique_ptr<Enemy>& enemy : enemies4_)
	{
		if (enemy->GetIsActive())
		{
			enemy->Draw(camera_);
		}
	}

	for (const std::unique_ptr<Enemy>& enemy : enemies5_)
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
void GamePlayScene::AddEnemy2()
{
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(enemyModel_.get());
	enemies2_.push_back(std::unique_ptr<Enemy>(newEnemy));
	enemyPosition2_.push_back(std::queue<Vector3>());
}
void GamePlayScene::AddEnemy3()
{
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(enemyModel_.get());
	enemies3_.push_back(std::unique_ptr<Enemy>(newEnemy));
	enemyPosition3_.push_back(std::queue<Vector3>());
}
void GamePlayScene::AddEnemy4()
{
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(enemyModel_.get());
	enemies4_.push_back(std::unique_ptr<Enemy>(newEnemy));
	enemyPosition4_.push_back(std::queue<Vector3>());
}
void GamePlayScene::AddEnemy5()
{
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(enemyModel_.get());
	enemies5_.push_back(std::unique_ptr<Enemy>(newEnemy));
	enemyPosition5_.push_back(std::queue<Vector3>());
}



void GamePlayScene::UpdataEnemy2() {



	//プレイヤーの座標を保存
	if (!enemies2_[0]->GetIsActive())
	{
		playerPosition2_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition2_.pop();
		playerPosition2_.push(player_->GetWorldPosition());
		enemies2_[0]->SetPosition(playerPosition2_.front());
		if (!player_->GetIsCopied2())
		{
			AddEnemy2();
			//enemies_[0]->SetPosition(playerPosition_.front());
			player_->SetIsCopied2(true);
		}
		else {

		}


	}

	for (int i = 1; i < enemies2_.size(); i++)
	{
		if (!enemies2_[i]->GetIsActive())
		{
			playerPosition2_.push(player_->GetWorldPosition());
			enemyPosition2_[i - 1].push(enemies2_[i - 1]->GetWorldPosition());
		}
		else
		{
			enemyPosition2_[i - 1].pop();
			enemyPosition2_[i - 1].push(enemies2_[i - 1]->GetWorldPosition());
			//enemyPosition_[i - 1].push(player_->GetWorldPosition());
			enemies2_[i]->SetPosition(enemyPosition2_[i - 1].front());
			if (!enemies2_[i - 1]->GetIsCopied())
			{
				AddEnemy2();
				//enemies_[i-1]->SetPosition(player_->GetWorldTransform().translation_);
				enemies2_[i - 1]->SetIsCopied(true);
			}


		}
	}

}

void GamePlayScene::UpdataEnemy3() {



	//プレイヤーの座標を保存
	if (!enemies3_[0]->GetIsActive())
	{
		playerPosition3_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition3_.pop();
		playerPosition3_.push(player_->GetWorldPosition());
		enemies3_[0]->SetPosition(playerPosition3_.front());
		if (!player_->GetIsCopied3())
		{
			AddEnemy3();
			//enemies_[0]->SetPosition(playerPosition_.front());
			player_->SetIsCopied3(true);
		}
		else {

		}


	}

	for (int i = 1; i < enemies3_.size(); i++)
	{
		if (!enemies3_[i]->GetIsActive())
		{
			playerPosition3_.push(player_->GetWorldPosition());
			enemyPosition3_[i - 1].push(enemies3_[i - 1]->GetWorldPosition());
		}
		else
		{
			enemyPosition3_[i - 1].pop();
			enemyPosition3_[i - 1].push(enemies3_[i - 1]->GetWorldPosition());
			//enemyPosition_[i - 1].push(player_->GetWorldPosition());
			enemies3_[i]->SetPosition(enemyPosition3_[i - 1].front());
			if (!enemies3_[i - 1]->GetIsCopied())
			{
				AddEnemy3();
				//enemies_[i-1]->SetPosition(player_->GetWorldTransform().translation_);
				enemies3_[i - 1]->SetIsCopied(true);
			}


		}
	}

}
void GamePlayScene::UpdataEnemy4() {



	//プレイヤーの座標を保存
	if (!enemies4_[0]->GetIsActive())
	{
		playerPosition4_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition4_.pop();
		playerPosition4_.push(player_->GetWorldPosition());
		enemies4_[0]->SetPosition(playerPosition4_.front());
		if (!player_->GetIsCopied3())
		{
			AddEnemy4();
			player_->SetIsCopied3(true);
		}
		else {

		}


	}

	for (int i = 1; i < enemies4_.size(); i++)
	{
		if (!enemies4_[i]->GetIsActive())
		{
			playerPosition4_.push(player_->GetWorldPosition());
			enemyPosition4_[i - 1].push(enemies4_[i - 1]->GetWorldPosition());
		}
		else
		{
			enemyPosition4_[i - 1].pop();
			enemyPosition4_[i - 1].push(enemies4_[i - 1]->GetWorldPosition());
			enemies4_[i]->SetPosition(enemyPosition4_[i - 1].front());
			if (!enemies4_[i - 1]->GetIsCopied())
			{
				AddEnemy4();
				enemies4_[i - 1]->SetIsCopied(true);
			}


		}
	}

}
void GamePlayScene::UpdataEnemy5() {



	//プレイヤーの座標を保存
	if (!enemies5_[0]->GetIsActive())
	{
		playerPosition5_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition5_.pop();
		playerPosition5_.push(player_->GetWorldPosition());
		enemies5_[0]->SetPosition(playerPosition5_.front());
		if (!player_->GetIsCopied3())
		{
			AddEnemy5();
			player_->SetIsCopied3(true);
		}
		else {

		}


	}

	for (int i = 1; i < enemies5_.size(); i++)
	{
		if (!enemies5_[i]->GetIsActive())
		{
			playerPosition5_.push(player_->GetWorldPosition());
			enemyPosition5_[i - 1].push(enemies5_[i - 1]->GetWorldPosition());
		}
		else
		{
			enemyPosition5_[i - 1].pop();
			enemyPosition5_[i - 1].push(enemies5_[i - 1]->GetWorldPosition());
			enemies5_[i]->SetPosition(enemyPosition5_[i - 1].front());
			if (!enemies5_[i - 1]->GetIsCopied())
			{
				AddEnemy5();
				enemies5_[i - 1]->SetIsCopied(true);
			}


		}
	}

}
