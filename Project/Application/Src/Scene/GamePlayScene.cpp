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

	//コピーの生成
	copyModel_.reset(ModelManager::Create());
	copyModel_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	Addcopy();
	Addcopy2();
	Addcopy3();
	Addcopy4();
	Addcopy5();

	//敵の生成
	enemyModel_.reset(ModelManager::Create());
	enemyModel_->SetColor({ 0.0f,1.0f,0.0f,1.0f });
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemyModel_.get());

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

	/*/コピーの更新
	for (std::unique_ptr<Copy> &copy : copys_){copy->Update();}
	if (popflag1_){for (std::unique_ptr<Copy>& copy : copys2_){copy->Update();}}
	if (popflag2_){for (std::unique_ptr<Copy>& copy : copys3_){copy->Update();}}
	if (popflag3_){for (std::unique_ptr<Copy>& copy : copys4_){copy->Update();}}
	if (popflag4_){for (std::unique_ptr<Copy>& copy : copys5_){copy->Update();}}
	*/

	//プレイヤーの更新
	player_->Update();

	//ブロックの更新
	for (const std::unique_ptr<Block>& block : blocks_)
	{
		block->Update();
		
	}

	for (int i =0; i < blocks_.size(); i++) {
		enemy_->SetBlockPosition(blocks_[i].get()->GetWorldPosition());
		enemy_->SetBlockSize(blocks_[i].get()->GetSize());
	}
	
	//敵の更新
	enemy_->SetPlayerPosition(player_->GetWorldPosition());
	enemy_->Update();

	//カメラの更新
	camera_.UpdateMatrix();

	//衝突判定
	collisionManager_->ClearColliderList();
	collisionManager_->SetColliderList(player_.get());
	collisionManager_->SetColliderList(enemy_.get());
	for (const std::unique_ptr<Block>& block : blocks_)
	{
		collisionManager_->SetColliderList(block.get());
	}
	collisionManager_->CheckAllCollisions();

	//プレイヤーの座標を保存
	/*
	Updatacopy1();
	if (popflag1_) {Updatacopy2();}
	if (popflag2_) {Updatacopy3();}
	if (popflag3_) {Updatacopy4();}
	if (popflag4_) {Updatacopy5();}
	*/

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
	enemy_->Draw(camera_);
	//コピーの描画
	for (const std::unique_ptr<Copy>& copy : copys_)
	{
		if (copy->GetIsActive())
		{
			copy->Draw(camera_);
		}
	}

	for (const std::unique_ptr<Copy>& copy : copys2_)
	{
		if (copy->GetIsActive())
		{
			copy->Draw(camera_);
		}
	}
	for (const std::unique_ptr<Copy>& copy : copys3_)
	{
		if (copy->GetIsActive())
		{
			copy->Draw(camera_);
		}
	}
	for (const std::unique_ptr<Copy>& copy : copys4_)
	{
		if (copy->GetIsActive())
		{
			copy->Draw(camera_);
		}
	}
	for (const std::unique_ptr<Copy>& copy : copys5_)
	{
		if (copy->GetIsActive())
		{
			copy->Draw(camera_);
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
void GamePlayScene::Addcopy()
{
	Copy* newcopy = new Copy();
	newcopy->Initialize(copyModel_.get());
	copys_.push_back(std::unique_ptr<Copy>(newcopy));
	copyPosition_.push_back(std::queue<Vector3>());
}
void GamePlayScene::Addcopy2()
{
	Copy* newcopy = new Copy();
	newcopy->Initialize(copyModel_.get());
	copys2_.push_back(std::unique_ptr<Copy>(newcopy));
	copyPosition2_.push_back(std::queue<Vector3>());
}
void GamePlayScene::Addcopy3()
{
	Copy* newcopy = new Copy();
	newcopy->Initialize(copyModel_.get());
	copys3_.push_back(std::unique_ptr<Copy>(newcopy));
	copyPosition3_.push_back(std::queue<Vector3>());
}
void GamePlayScene::Addcopy4()
{
	Copy* newcopy = new Copy();
	newcopy->Initialize(copyModel_.get());
	copys4_.push_back(std::unique_ptr<Copy>(newcopy));
	copyPosition4_.push_back(std::queue<Vector3>());
}
void GamePlayScene::Addcopy5()
{
	Copy* newcopy = new Copy();
	newcopy->Initialize(copyModel_.get());
	copys5_.push_back(std::unique_ptr<Copy>(newcopy));
	copyPosition5_.push_back(std::queue<Vector3>());
}


void GamePlayScene::Updatacopy1() {
	if (!copys_[0]->GetIsActive())
	{
		playerPosition_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition_.pop();
		playerPosition_.push(player_->GetWorldPosition());
		copys_[0]->SetPosition(playerPosition_.front());
		if (!player_->GetIsCopied())
		{
			Addcopy();

			//enemies_[0]->SetPosition(playerPosition_.front());
			player_->SetIsCopied(true);
		}
		else {

		}


	}
	for (int i = 1; i < copys_.size(); i++)
	{
		if (!copys_[i]->GetIsActive())
		{
			playerPosition_.push(player_->GetWorldPosition());
			copyPosition_[i - 1].push(copys_[i - 1]->GetWorldPosition());
		}
		else
		{
			copyPosition_[i - 1].pop();
			copyPosition_[i - 1].push(copys_[i - 1]->GetWorldPosition());
			//enemyPosition_[i - 1].push(player_->GetWorldPosition());
			copys_[i]->SetPosition(copyPosition_[i - 1].front());
			if (!copys_[i - 1]->GetIsCopied())
			{
				Addcopy();
				//enemies_[i-1]->SetPosition(player_->GetWorldTransform().translation_);
				copys_[i - 1]->SetIsCopied(true);
			}


		}
	}
}
void GamePlayScene::Updatacopy2() {



	//プレイヤーの座標を保存
	if (!copys2_[0]->GetIsActive())
	{
		playerPosition2_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition2_.pop();
		playerPosition2_.push(player_->GetWorldPosition());
		copys2_[0]->SetPosition(playerPosition2_.front());
		if (!player_->GetIsCopied2())
		{
			Addcopy2();
			//copy_[0]->SetPosition(playerPosition_.front());
			player_->SetIsCopied2(true);
		}
		else {

		}


	}

	for (int i = 1; i < copys2_.size(); i++)
	{
		if (!copys2_[i]->GetIsActive())
		{
			playerPosition2_.push(player_->GetWorldPosition());
			copyPosition2_[i - 1].push(copys2_[i - 1]->GetWorldPosition());
		}
		else
		{
			copyPosition2_[i - 1].pop();
			copyPosition2_[i - 1].push(copys2_[i - 1]->GetWorldPosition());
			//copyPosition_[i - 1].push(player_->GetWorldPosition());
			copys2_[i]->SetPosition(copyPosition2_[i - 1].front());
			if (!copys2_[i - 1]->GetIsCopied())
			{
				Addcopy2();
				//copy_[i-1]->SetPosition(player_->GetWorldTransform().translation_);
				copys2_[i - 1]->SetIsCopied(true);
			}


		}
	}

}
void GamePlayScene::Updatacopy3() {



	//プレイヤーの座標を保存
	if (!copys3_[0]->GetIsActive())
	{
		playerPosition3_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition3_.pop();
		playerPosition3_.push(player_->GetWorldPosition());
		copys3_[0]->SetPosition(playerPosition3_.front());
		if (!player_->GetIsCopied3())
		{
			Addcopy3();
			//copy_[0]->SetPosition(playerPosition_.front());
			player_->SetIsCopied3(true);
		}
		else {

		}


	}

	for (int i = 1; i < copys3_.size(); i++)
	{
		if (!copys3_[i]->GetIsActive())
		{
			playerPosition3_.push(player_->GetWorldPosition());
			copyPosition3_[i - 1].push(copys3_[i - 1]->GetWorldPosition());
		}
		else
		{
			copyPosition3_[i - 1].pop();
			copyPosition3_[i - 1].push(copys3_[i - 1]->GetWorldPosition());
			//copyPosition_[i - 1].push(player_->GetWorldPosition());
			copys3_[i]->SetPosition(copyPosition3_[i - 1].front());
			if (!copys3_[i - 1]->GetIsCopied())
			{
				Addcopy3();
				//copy_[i-1]->SetPosition(player_->GetWorldTransform().translation_);
				copys3_[i - 1]->SetIsCopied(true);
			}


		}
	}

}
void GamePlayScene::Updatacopy4() {



	//プレイヤーの座標を保存
	if (!copys4_[0]->GetIsActive())
	{
		playerPosition4_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition4_.pop();
		playerPosition4_.push(player_->GetWorldPosition());
		copys4_[0]->SetPosition(playerPosition4_.front());
		if (!player_->GetIsCopied4())
		{
			Addcopy4();
			player_->SetIsCopied4(true);
		}
		else {

		}


	}

	for (int i = 1; i < copys4_.size(); i++)
	{
		if (!copys4_[i]->GetIsActive())
		{
			playerPosition4_.push(player_->GetWorldPosition());
			copyPosition4_[i - 1].push(copys4_[i - 1]->GetWorldPosition());
		}
		else
		{
			copyPosition4_[i - 1].pop();
			copyPosition4_[i - 1].push(copys4_[i - 1]->GetWorldPosition());
			copys4_[i]->SetPosition(copyPosition4_[i - 1].front());
			if (!copys4_[i - 1]->GetIsCopied())
			{
				Addcopy4();
				copys4_[i - 1]->SetIsCopied(true);
			}


		}
	}

}
void GamePlayScene::Updatacopy5() {



	//プレイヤーの座標を保存
	if (!copys5_[0]->GetIsActive())
	{
		playerPosition5_.push(player_->GetWorldPosition());
	}
	else
	{
		playerPosition5_.pop();
		playerPosition5_.push(player_->GetWorldPosition());
		copys5_[0]->SetPosition(playerPosition5_.front());
		if (!player_->GetIsCopied5())
		{
			Addcopy5();
			player_->SetIsCopied5(true);
		}
		else {

		}


	}

	for (int i = 1; i < copys5_.size(); i++)
	{
		if (!copys5_[i]->GetIsActive())
		{
			playerPosition5_.push(player_->GetWorldPosition());
			copyPosition5_[i - 1].push(copys5_[i - 1]->GetWorldPosition());
		}
		else
		{
			copyPosition5_[i - 1].pop();
			copyPosition5_[i - 1].push(copys5_[i - 1]->GetWorldPosition());
			copys5_[i]->SetPosition(copyPosition5_[i - 1].front());
			if (!copys5_[i - 1]->GetIsCopied())
			{
				Addcopy5();
				copys5_[i - 1]->SetIsCopied(true);
			}


		}
	}

}
