#include "Player.h"
#include <Engine/Components/Particle/ParticleManager.h>

void Player::Initialzie(std::vector<Model*> models)
{
	//モデルの初期化
	models_ = models;

	//Inputクラスのインスタンスを取得
	input_ = Input::GetInstance();

	//Audioクラスのインスタンスを取得
	audio_ = Audio::GetInstance();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_.y = -10.0f;
	worldTransform_.quaternion_ = destinationQuaternion_;

	//武器の生成
	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize(models_[1]);
	weapon_->SetParent(worldTransform_);

	//制限時間のスプライトの生成
	movementRestrictionSprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	movementRestrictionSprite_->SetAnchorPoint({ 0.5f,0.5f });
	movementRestrictionSprite_->SetSize(movementRestrictionSpriteSize_);

	//音声データの読み込み
	moveAudioHandle_ = audio_->SoundLoadWave("Application/Resources/Sounds/Move.wav");
	attackAudioHandle_ = audio_->SoundLoadWave("Application/Resources/Sounds/Attack.wav");

	//パーティルの作成
	particleSystem_ = ParticleManager::Create("Move");
	//particleModel_.reset(ModelManager::CreateFromOBJ("BoxParticle", Opaque));
	//particleSystem_->SetModel(particleModel_.get());

	//衝突判定の初期化
	AABB aabb = {
	.min{-worldTransform_.scale_.x,-worldTransform_.scale_.y,-worldTransform_.scale_.z},
	.max{worldTransform_.scale_.x,worldTransform_.scale_.y,worldTransform_.scale_.z}
	};
	SetAABB(aabb);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//環境変数の初期化
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Speed", speed_);
	globalVariables->AddItem(groupName, "Gravity", gravity_);
	globalVariables->AddItem(groupName, "JumpFirstSpeed", jumpFirstSpeed_);
	globalVariables->AddItem(groupName, "MovementRestrictionTime", movementRestrictionTime_);
}

void Player::Update()
{
	if (!isTutorial_)
	{
		//動けない状態かつ着地している状態の時にストップフラグを立てる
		isStop_ = !isMove_ && isLanded_;

		//移動制限の処理
		UpdateMovementRestriction();
	}

	//Behaviorの遷移処理
	if (behaviorRequest_)
	{
		behavior_ = behaviorRequest_.value();
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}

	//Behaviorの実行
	switch (behavior_)
	{
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

	//着地フラグをリセット
	isLanded_ = false;

	//地面に埋まらないようにする
	if (worldTransform_.translation_.y <= -10.0f)
	{
		worldTransform_.translation_.y = -10.0f;
		velocity_.y = 0.0f;
	}

	//ワールドトランスフォームの更新
	worldTransform_.quaternion_ = Mathf::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f);
	worldTransform_.UpdateMatrixFromQuaternion();

	//武器の更新
	weapon_->Update();

	//着地のパーティクル
	Landing();

	//環境変数の適用
	ApplyGlobalVariables();
	
	ParticleEmitter* newParticleEmitter = ParticleEmitterBuilder()
		.SetEmitterName("Move")
		.SetTranslation({ 5.0f, 10.0f, 0.0f})
		.SetPopArea({ -2.0f,0.0f,0.0f }, { 2.0f,0.0f,0.0f })
		.SetPopRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
		.SetPopScale({ 0.1f, 0.1f,0.1f }, { 0.3f ,0.3f ,0.3f })
		.SetPopAzimuth(azimuth_.x,azimuth_.y)
		.SetPopElevation(00.0f, 00.0f)
		.SetPopVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
		.SetPopColor({ 1.0f ,1.0f ,1.0f ,0.3f }, { 1.0f ,1.0f ,1.0f ,0.6f })
		.SetPopLifeTime(PopLifeTime.x, PopLifeTime.y)
		.SetPopCount(PopCount)
		.SetPopFrequency(PopFrequency)
		.SetDeleteTime(DeleteTime)
		.Build();
	particleSystem_->AddParticleEmitter(newParticleEmitter);

	//ImGui
	ImGui::Begin("Player");
	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x);
	ImGui::DragFloat4("Quaternion", &worldTransform_.quaternion_.x);
	ImGui::DragFloat3("Velocity", &velocity_.x);
	ImGui::DragFloat2("MovementRestrictionSpriteSize", &movementRestrictionSpriteSize_.x);
	ImGui::Checkbox("isLanded", &isLanded_);
	ImGui::Text("MovementRestrictionTimer : %d", movementRestrictionTimer_);
	ImGui::DragInt("MoveSEWaitTime", &moveAudioWaitTime_);
	if (ImGui::TreeNode("Particle")) {
		ImGui::DragFloat2("Azimuth", &azimuth_.x, 1.0f, 0.0f, 360.0f);
		ImGui::DragFloat2("PopLifeTime", &PopLifeTime.x,0.1f,0.1f,1.0f);
		ImGui::DragInt("PopCount", &PopCount, 1,1, 20);
		ImGui::DragFloat("PopFrequency", &PopFrequency,0.1f, 0.1f, 10.0f);
		ImGui::DragFloat("DeleteTime", &DeleteTime,0.1f, 0.1f, 10.0f);
		ImGui::TreePop();
	}
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	//モデルの描画
	models_[0]->Draw(worldTransform_, camera);

	//武器の描画
	weapon_->Draw(camera);
}

void Player::DrawUI(const Camera& camera)
{
	if (!isTutorial_)
	{
		//移動制限のスプライトの座標を設定
		UpdateMovementRestrictionSprite(camera);

		//移動制限のスプライトを描画
		movementRestrictionSprite_->Draw();
	}
	else
	{
		//武器のUIの描画
		weapon_->DrawUI(camera);
	}
}

void Player::Reset()
{
	//destinationQuaternion_ = { 0.0f,0.707f,0.0f,0.707f };
	//worldTransform_.quaternion_ = destinationQuaternion_;
	//worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
	isMove_ = true;
	movementRestrictionTimer_ = movementRestrictionTime_;
	Vector4 color = { 0.0f,0.0f,1.0f,1.0f };
	models_[0]->SetColor(color);
}

void Player::OnCollision(Collider* collider)
{
	AABB aabbA = {
	.min{worldTransform_.translation_.x + GetAABB().min.x,worldTransform_.translation_.y + GetAABB().min.y,worldTransform_.translation_.z + GetAABB().min.z},
	.max{worldTransform_.translation_.x + GetAABB().max.x,worldTransform_.translation_.y + GetAABB().max.y,worldTransform_.translation_.z + GetAABB().max.z},
	};
	AABB aabbB = {
		.min{collider->GetWorldTransform().translation_.x + collider->GetAABB().min.x,collider->GetWorldTransform().translation_.y + collider->GetAABB().min.y,collider->GetWorldTransform().translation_.z + collider->GetAABB().min.z},
		.max{collider->GetWorldTransform().translation_.x + collider->GetAABB().max.x,collider->GetWorldTransform().translation_.y + collider->GetAABB().max.y,collider->GetWorldTransform().translation_.z + collider->GetAABB().max.z},
	};

	Vector3 overlapAxis = {
		std::min<float>(aabbA.max.x,aabbB.max.x) - std::max<float>(aabbA.min.x,aabbB.min.x),
		std::min<float>(aabbA.max.y,aabbB.max.y) - std::max<float>(aabbA.min.y,aabbB.min.y),
		std::min<float>(aabbA.max.z,aabbB.max.z) - std::max<float>(aabbA.min.z,aabbB.min.z),
	};

	Vector3 directionAxis{};
	if (overlapAxis.x < overlapAxis.y && overlapAxis.x < overlapAxis.z) {
		//X軸方向で最小の重なりが発生している場合
		directionAxis.x = (worldTransform_.translation_.x < collider->GetWorldTransform().translation_.x) ? -1.0f : 1.0f;
		directionAxis.y = 0.0f;
	}
	else if (overlapAxis.y < overlapAxis.x && overlapAxis.y < overlapAxis.z) {
		//Y軸方向で最小の重なりが発生している場合
		directionAxis.y = (worldTransform_.translation_.y < collider->GetWorldTransform().translation_.y) ? -1.0f : 1.0f;
		directionAxis.x = 0.0f;
		if (behaviorRequest_ != Behavior::kAttack)
		{
			behaviorRequest_ = Behavior::kRoot;
		}
		if (directionAxis.y == 1.0f)
		{

			velocity_.y = 0.0f;
			isLanded_ = true;
		}
	}
	else if (overlapAxis.z < overlapAxis.x && overlapAxis.z < overlapAxis.y)
	{
		directionAxis.z = (worldTransform_.translation_.z < collider->GetWorldTransform().translation_.z) ? -1.0f : 1.0f;
		directionAxis.x = 0.0f;
		directionAxis.y = 0.0f;
	}

	worldTransform_.translation_ += overlapAxis * directionAxis;
	worldTransform_.UpdateMatrixFromQuaternion();
	weapon_->Update();
}

const Vector3 Player::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}

void Player::BehaviorRootInitialize()
{
	//当たり判定をなくす
	weapon_->SetIsAttack(false);
}

void Player::BehaviorRootUpdate()
{
	//閾値
	const float threshold = 0.2f;
	velocity_.x = 0.0f;

	//コントローラーが接続されているときかつ移動出来るとき
	if (input_->IsControllerConnected())
	{
		//スティックの入力を取得
		velocity_.x = input_->GetLeftStickX();
	}

	//キーボード入力
	if (input_->IsPushKey(DIK_A))
	{
		velocity_.x = -1.0f;
	}
	else if (input_->IsPushKey(DIK_D))
	{
		velocity_.x = 1.0f;
	}

	//スティックの入力が閾値を超えていたら速度を設定
	if (std::abs(velocity_.x) > threshold && isMove_)
	{
		//速度を設定
		velocity_.x = Mathf::Normalize(velocity_).x * speed_;

		//回転ベクトルを作成
		Vector3 rotateVector = { Mathf::Normalize(velocity_).x, 0.0f, 0.0f };

		//外積を計算
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f, 0.0f, 1.0f }, rotateVector));

		//内積を計算
		float dot = Mathf::Dot({ 0.0f, 0.0f, 1.0f }, rotateVector);

		//クォータニオンを作成
		destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));

		//移動中に音声を再生する
		if (++moveAudioTimer_ > moveAudioWaitTime_)
		{
			audio_->SoundPlayWave(moveAudioHandle_, false, 0.4f);
			moveAudioTimer_ = 0;
		}
		
		if (cross.y == 1)
		{
			
			ParticleEmitter* newParticleEmitter = ParticleEmitterBuilder()
				.SetTranslation({ worldTransform_.translation_.x - 1.0f, worldTransform_.translation_.y-1.0f, worldTransform_.translation_.z})
				.SetPopArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetPopRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetPopScale({ 0.1f, 0.1f,0.1f }, { 0.3f ,0.3f ,0.3f })
				.SetPopAzimuth(90.0f, 135.0f)
				.SetPopElevation(0.0f, 0.0f)
				.SetPopVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetPopColor({ 1.0f ,1.0f ,1.0f ,0.3f }, { 1.0f ,1.0f ,1.0f ,0.6f })
				.SetPopLifeTime(0.1f, 0.6f)
				.SetPopCount(1)
				.SetPopFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}
		else if (cross.y == -1)
		{
			ParticleEmitter* newParticleEmitter = ParticleEmitterBuilder()
				.SetEmitterName("Move")
				.SetTranslation({ worldTransform_.translation_.x + 1.0f, worldTransform_.translation_.y-1.0f, worldTransform_.translation_.z})
				.SetPopArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetPopRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetPopScale({ 0.1f, 0.1f,0.1f }, { 0.3f ,0.3f ,0.3f })
				.SetPopAzimuth(0.0f, 0.0f)
				.SetPopElevation(0.0f, 0.0f)
				.SetPopVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetPopColor({ 1.0f ,1.0f ,1.0f ,0.3f }, { 1.0f ,1.0f ,1.0f ,0.6f })
				.SetPopLifeTime(0.1f, 0.6f)
				.SetPopCount(1)
				.SetPopFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
 			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

	}
	else
	{
		//速度を0で初期化
		velocity_.x = 0.0f;
		//移動中の音声のタイマーを初期化
		moveAudioTimer_ = 0;
	}

	//加速ベクトル
	Vector3 accelerationVector = { 0.0f, -gravity_, 0.0f };
	//加速
	velocity_ += accelerationVector;
	//速度を加算
	worldTransform_.translation_ += velocity_;

	//地面に埋まらないようにする
	if (worldTransform_.translation_.y <= -10.0f && !isLanded_)
	{
		worldTransform_.translation_.y = -10.0f;
		velocity_.y = 0.0f;
		isLanded_ = true;

	}

	//コントローラーが接続されているとき
	if (input_->IsControllerConnected())
	{
		//ジャンプ状態に変更
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && isLanded_ && isMove_)
		{
			behaviorRequest_ = Behavior::kJump;
			worldTransform_.translation_.y += jumpFirstSpeed_;
		}

		//攻撃行動に変更
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			behaviorRequest_ = Behavior::kAttack;
		}
	}

	//キーボード入力
	if (input_->IsPushKeyEnter(DIK_SPACE) && isLanded_ && isMove_)
	{
		behaviorRequest_ = Behavior::kJump;
		worldTransform_.translation_.y += jumpFirstSpeed_;
	}

	if (input_->IsPushKeyEnter(DIK_E))
	{
		behaviorRequest_ = Behavior::kAttack;
	}
}

void Player::BehaviorJumpInitialize()
{
	//ジャンプの初速度を設定
	velocity_.y = jumpFirstSpeed_;
}

void Player::BehaviorJumpUpdate()
{
	//閾値
	const float threshold = 0.2f;
	velocity_.x = 0.0f;

	//コントローラーが接続されているときかつ移動出来るとき
	if (input_->IsControllerConnected())
	{
		//スティックの入力を取得
		velocity_.x = input_->GetLeftStickX();
	}

	//キーボード入力
	if (input_->IsPushKey(DIK_A))
	{
		velocity_.x = -1.0f;
	}
	else if (input_->IsPushKey(DIK_D))
	{
		velocity_.x = 1.0f;
	}

	//スティックの入力が閾値を超えていたら
	if (std::abs(velocity_.x) > threshold && isMove_)
	{
		//速度を設定
		velocity_.x = Mathf::Normalize(velocity_).x * speed_;

		//回転ベクトルを作成
		Vector3 rotateVector = { Mathf::Normalize(velocity_).x, 0.0f, 0.0f };

		//外積を計算
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f, 0.0f, 1.0f }, rotateVector));

		//内積を計算
		float dot = Mathf::Dot({ 0.0f, 0.0f, 1.0f }, rotateVector);

		//クォータニオンを作成
		destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	}
	else
	{
		//速度を0で初期化
		velocity_.x = 0.0f;
	}

	//加速ベクトル
	Vector3 accelerationVector = { 0.0f, -gravity_, 0.0f };
	//加速
	velocity_ += accelerationVector;
	//速度を加算
	worldTransform_.translation_ += velocity_;

	//地面についたら通常状態に戻す
	if (worldTransform_.translation_.y <= -10.0f && !isLanded_)
	{
	
		behaviorRequest_ = Behavior::kRoot;
		worldTransform_.translation_.y = -10.0f;
		velocity_.y = 0.0f;
		isLanded_ = true;
	}

	//コントローラーが接続されているとき
	if (input_->IsControllerConnected())
	{
		//攻撃行動に変更
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			behaviorRequest_ = Behavior::kAttack;
		}
	}

	//キーボード入力
	if (input_->IsPushKeyEnter(DIK_E))
	{
		behaviorRequest_ = Behavior::kAttack;
	}
}

void Player::BehaviorAttackInitialize()
{
	//当たり判定をつける
	weapon_->SetIsAttack(true);
}

void Player::BehaviorAttackUpdate()
{
	//通常状態に戻る
	behaviorRequest_ = Behavior::kRoot;
	audio_->SoundPlayWave(attackAudioHandle_, false, 0.4f);
}

void Player::UpdateMovementRestriction()
{
	//移動ベクトルが0ではないとき
	if (velocity_ != Vector3(0.0f, 0.0f, 0.0f) && isMove_)
	{
		//移動制限のタイマーが0以下になったときに動けないようにする
		if (--movementRestrictionTimer_ < 0)
		{
			isMove_ = false;
		}
	}

	//移動制限時間が短くなったら
	const int divisor = 4;
	if (movementRestrictionTimer_ < movementRestrictionTime_ / divisor)
	{
		//モデルの色を変更
		Vector4 color = { 1.0f,0.25f,0.0f,1.0f };
		models_[0]->SetColor(color);
	}
}

void Player::UpdateMovementRestrictionSprite(const Camera& camera)
{
	//ビューポート行列を計算
	Matrix4x4 matViewport = Mathf::MakeViewportMatrix(0, 0, Application::kClientWidth, Application::kClientHeight, 0, 1);
	//ビュー行列とプロジェクション行列とビューポート行列を合成
	Matrix4x4 matViewProjectionViewport = camera.matView_ * camera.matProjection_ * matViewport;
	//スクリーン座標に変換
	Vector3 offset = { 0.0f,2.0f,0.0f };
	Vector3 spritePosition = GetWorldPosition() + offset;
	spritePosition = Mathf::Transform(spritePosition, matViewProjectionViewport);
	//スプライトに座標を設定
	movementRestrictionSprite_->SetPosition({ spritePosition.x,spritePosition.y });

	//スプライトのスケールを設定
	Vector2 currentSize = { movementRestrictionSpriteSize_.x * float(movementRestrictionTimer_) / float(movementRestrictionTime_) ,movementRestrictionSpriteSize_.y };
	currentSize.x = currentSize.x < 0 ? 0.0f : currentSize.x;
	movementRestrictionSprite_->SetSize(currentSize);
}

void Player::Landing() {
	if (velocity_.y == 0) {

		if (prePos_.y != worldTransform_.translation_.y) {
			prePos_.y = worldTransform_.translation_.y;
			landing_ = true;
			ParticleEmitter* newParticleEmitter = ParticleEmitterBuilder()
				.SetTranslation({ worldTransform_.translation_.x, worldTransform_.translation_.y - 1.5f, worldTransform_.translation_.z })
				.SetPopArea({ -1.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f })
				.SetPopRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetPopScale({ 0.2f, 0.2f,0.2f }, { 0.4f ,0.4f ,0.4f })
				.SetPopAzimuth(45.0f, 135.0f)
				.SetPopElevation(0.0f, 0.0f)
				.SetPopVelocity({ -0.05f ,0.05f ,0.05f }, { 0.05f ,0.08f ,0.08f })
				.SetPopColor({ 1.0f ,1.0f ,1.0f ,0.3f }, { 1.0f ,1.0f ,1.0f ,0.6f })
				.SetPopLifeTime(0.1f, 1.0f)
				.SetPopCount(30)
				.SetPopFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}
		else {
			landing_ = false;
		}

	}
	else {

		if (prePos_.y != worldTransform_.translation_.y) {
			prePos_.y = worldTransform_.translation_.y;
			if (!landing_ && velocity_.y == -0.05f) {
				landing_ = true;

				ParticleEmitter* newParticleEmitter = ParticleEmitterBuilder()
					.SetTranslation({ worldTransform_.translation_.x, worldTransform_.translation_.y - 1.5f, worldTransform_.translation_.z })
					.SetPopArea({ -1.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f })
					.SetPopRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetPopScale({ 0.2f, 0.2f,0.2f }, { 0.4f ,0.4f ,0.4f })
					.SetPopAzimuth(45.0f, 135.0f)
					.SetPopElevation(0.0f, 0.0f)
					.SetPopVelocity({ -0.05f ,0.05f ,0.05f }, { 0.05f ,0.08f ,0.08f })
					.SetPopColor({ 1.0f ,1.0f ,1.0f ,0.3f }, { 1.0f ,1.0f ,1.0f ,0.6f })
					.SetPopLifeTime(0.1f, 1.0f)
					.SetPopCount(30)
					.SetPopFrequency(4.0f)
					.SetDeleteTime(1.0f)
					.Build();
				particleSystem_->AddParticleEmitter(newParticleEmitter);
			}

		}
		else
		{
			landing_ = false;
		}

	}
}

void Player::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	speed_ = globalVariables->GetFloatValue(groupName, "Speed");
	gravity_ = globalVariables->GetFloatValue(groupName, "Gravity");
	jumpFirstSpeed_ = globalVariables->GetFloatValue(groupName, "JumpFirstSpeed");
	movementRestrictionTime_ = globalVariables->GetIntValue(groupName, "MovementRestrictionTime");
}