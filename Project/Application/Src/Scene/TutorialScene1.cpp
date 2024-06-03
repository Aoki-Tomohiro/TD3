#include "TutorialScene1.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include <Engine/Components/PostEffects/PostEffects.h>

uint32_t TutorialScene1::tutoBGMHandle_ = 0;

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
	playerModel_ = ModelManager::CreateFromModelFile("Human.gltf", "Player", Opaque);
	playerModel_->GetMaterial(0)->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	weaponModel_ = ModelManager::CreateFromModelFile("Cube.obj", "PlayerWeapon", Transparent);
	std::vector<Model*> playerModels = { playerModel_,weaponModel_ };
	player_ = std::make_unique<Player>();
	player_->Initialzie(playerModels);
	player_->SetIsTutorial(true);

	//敵の生成
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(0);
	enemyManager_->SaveReverseData();
	enemyManager_->SetIsTutorial(true);

	//ブロックを生成
	blockManager_ = std::make_unique<BlockManager>();
	blockManager_->Initialize(0);

	//背景の生成
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize();

	//スコアの生成
	score_ = std::make_unique<Score>();
	score_->Initialize();

	//FollowCameraの生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&player_->GetWorldTransform());

	//パーティクルマネージャーのインスタンスを取得
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//音声データ読み込み
	whiffAudioHandle_ = audio_->LoadAudioFile("Application/Resources/Sounds/Whiff.wav");
	tutorialBGMHandle_ = audio_->LoadAudioFile("tutorialBGM.wav");
	tutoBGMHandle_ = audio_->LoadAudioFile("tutorialBGM.wav");
	audio_->PlayAudio(tutoBGMHandle_, true, 0.5f);
	//audio_->PlayAudio(tutorialBGMHandle_, true, 0.5f);

	//チュートリアルのスプライトの生成
	TextureManager::Load("Tutorial.png");
	TextureManager::Load("Numbers/1.png");
	tutorialSprite_.reset(Sprite::Create("Tutorial.png", tutorialSpritePosition_));
	tutorialSprite_->SetScale({ 0.6f,0.6f });
	numberSprite_.reset(Sprite::Create("Numbers/1.png", numberSpritePosition_));
	numberSprite_->SetScale({ 0.6f,0.6f });
	//UI
	TextureManager::Load("botan.png");
	botanUI_.reset(Sprite::Create("botan.png", { 1000.0f,580.0f }));;
	
	//Switchの生成
	switchManager_ = std::make_unique<SwitchManager>();
	switchManager_->Initialize(0);
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

	//スイッチの更新
	switchManager_->Update();

	//敵の更新
	enemyManager_->Update();

	//背景の更新
	backGround_->SetPlayerPosition(player_->GetWorldPosition());
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
	for (const std::unique_ptr<Block>& block : blockManager_->GetBlocks())
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
	//衝突判定
	collisionManager_->CheckAllCollisions();

	//ゲームクリアのフラグ
	bool isClear = true;
	//敵が存在するとき
	if (enemies.size() != 0)
	{
		//生きている敵がいるか確認する
		for (const std::unique_ptr<Enemy>& enemy : enemies)
		{
			if (enemy->GetIsActive())
			{
				isClear = false;
			}
		}
		//ゲームクリアのフラグが立っていたらシーンを変える
		if (isClear)
		{
			isFadeOut_ = true;
		}
	}

	//空振りの音を鳴らす
	if (player_->GetWeapon()->GetIsAttack() && !isClear)
	{
		if (!player_->GetWeapon()->GetIsHit())
		{
			audio_->PlayAudio(whiffAudioHandle_, 0, 0.4f);
		}
	}

	//パーティクルの更新
	particleManager_->Update();

	ImGui::Begin("TutorialScene1");
	ImGui::DragFloat2("TutorialSpritePosition", &tutorialSpritePosition_.x);
	ImGui::DragFloat2("TutorialSpriteScale", &tutorialSpriteScale_.x);
	ImGui::DragFloat2("NumberSpritePosition", &numberSpritePosition_.x);
	ImGui::DragFloat2("NumberSpriteScale", &numberSpriteScale_.x);
	ImGui::End();
	tutorialSprite_->SetPosition(tutorialSpritePosition_);
	tutorialSprite_->SetScale(tutorialSpriteScale_);
	numberSprite_->SetPosition(numberSpritePosition_);
	numberSprite_->SetScale(numberSpriteScale_);
	//トランジション
	Transition();
}

void TutorialScene1::Draw()
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

	//チュートリアルのスプライトの描画
	tutorialSprite_->Draw();
	numberSprite_->Draw();

	//スコアの描画
	score_->Draw();

	botanUI_->Draw();
	

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void TutorialScene1::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

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

void TutorialScene1::Transition() {
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
			sceneManager_->ChangeScene("TutorialScene2");
			//audio_->StopAudio(tutorialBGMHandle_);
			timer_ = 0.0f;
		}
	}

	PostEffects::GetInstance()->GetVignette()->SetIntensity(timer_);

}