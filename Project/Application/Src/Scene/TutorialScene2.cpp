#include "TutorialScene2.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"

void TutorialScene2::Initialize()
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

	//敵の生成
	enemyModel_.reset(ModelManager::CreateFromModelFile("Human.gltf", Opaque));
	enemyModel_->GetMaterial(0)->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(enemyModel_.get(), 1);
	enemyManager_->SaveReverseData();
	enemyManager_->SetIsTutorial(true);

	//ブロックを生成
	blockModel_.reset(ModelManager::Create());
	blockModel_->GetMaterial(1)->SetColor({ 0.196f,0.196f,0.196f,1.0f });
	blockManager_ = std::make_unique<BlockManager>();
	blockManager_->Initialize(blockModel_.get(), 1);

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

	//パーティクルマネージャーのインスタンスを取得
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//音声データ読み込み
	whiffAudioHandle_ = audio_->LoadAudioFile("Application/Resources/Sounds/Whiff.wav");

	//チュートリアルのスプライトの生成
	TextureManager::Load("Tutorial.png");
	TextureManager::Load("Numbers/2.png");
	tutorialSprite_.reset(Sprite::Create("Tutorial.png", tutorialSpritePosition_));
	tutorialSprite_->SetScale({ 0.6f,0.6f });
	numberSprite_.reset(Sprite::Create("Numbers/2.png", numberSpritePosition_));
	numberSprite_->SetScale({ 0.6f,0.6f });
}

void TutorialScene2::Finalize()
{
}

void TutorialScene2::Update()
{
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

	//コピーの更新
	copyManager_->Update();

	//敵の更新
	enemyManager_->Update();

	//背景の更新
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
	const std::vector<std::unique_ptr<Block>>& blocks = blockManager_->GetBlocks();
	for (const std::unique_ptr<Block>& block : blocks)
	{
		collisionManager_->SetColliderList(block.get());
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

	//プレイヤーが動けるときに逆再生時のデータを保存
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

	//スコアの更新
	score_->Update(player_.get(), copies);

	//パーティクルの更新
	particleManager_->Update();

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
			copyManager_->SetIsDoubleSpeed(false);
			enemyManager_->SetIsDoubleSpeed(false);
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


	ImGui::Begin("TutorialScene2");
	ImGui::DragFloat2("TutorialSpritePosition", &tutorialSpritePosition_.x);
	ImGui::DragFloat2("TutorialSpriteScale", &tutorialSpriteScale_.x);
	ImGui::DragFloat2("NumberSpritePosition", &numberSpritePosition_.x);
	ImGui::DragFloat2("NumberSpriteScale", &numberSpriteScale_.x);
	ImGui::End();
	tutorialSprite_->SetPosition(tutorialSpritePosition_);
	tutorialSprite_->SetScale(tutorialSpriteScale_);
	numberSprite_->SetPosition(numberSpritePosition_);
	numberSprite_->SetScale(numberSpriteScale_);
	//パーティクルの更新
	particleManager_->Update();

	
	//トランジション
	Transition();
}

void TutorialScene2::Draw()
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

void TutorialScene2::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//プレイヤーのUIの描画
	player_->DrawUI(camera_);

	//コピー上限のUIの描画
	copyManager_->DrawUI();

	//チュートリアルのスプライトの描画
	tutorialSprite_->Draw();
	numberSprite_->Draw();

	//スコアの描画
	score_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void TutorialScene2::DrawBackGround()
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

void TutorialScene2::Reset()
{
	//プレイヤーをリセット
	player_->Reset();

	//コピーをリセット
	copyManager_->Reset();

	//敵をリセット
	for (const std::unique_ptr<Enemy>& enemy : enemyManager_->GetEnemies())
	{
		enemy->Reset();
	}
}

void TutorialScene2::Reverse()
{
	//逆再生データを取得
	auto it = reversePlayerPositions.back();
	reversePlayerPositions.pop_back();

	//プレイヤーを逆再生
	player_->SetPositions(std::get<0>(it), std::get<1>(it), std::get<2>(it), std::get<3>(it));

	//巻き戻しの速度を上げるためにデータをスキップ
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

void TutorialScene2::Transition() {
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
			sceneManager_->ChangeScene("TutorialScene3");
			timer_ = 0.0f;
		}
	}

	PostEffects::GetInstance()->GetVignette()->SetIntensity(timer_);

}