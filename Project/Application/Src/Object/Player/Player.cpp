#include "Player.h"


void Player::Initialzie(std::vector<Model*> models)
{
	//モデルの初期化
	models_ = models;

	//Inputクラスのインスタンスを取得
	input_ = Input::GetInstance();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.quaternion_ = destinationQuaternion_;

	//武器の生成
	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize(models_[1]);
	weapon_->SetParent(worldTransform_);

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
}

void Player::Update()
{
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

	//環境変数の適用
	ApplyGlobalVariables();

	//ImGui
	ImGui::Begin("Player");
	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x);
	ImGui::DragFloat4("Quaternion", &worldTransform_.quaternion_.x);
	ImGui::DragFloat3("Velocity", &velocity_.x);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{

	//モデルの描画
	models_[0]->Draw(worldTransform_, camera);

	//武器の描画
	weapon_->Draw(camera);
}

void Player::Reset()
{
	destinationQuaternion_ = { 0.0f,0.707f,0.0f,0.707f };
	worldTransform_.quaternion_ = destinationQuaternion_;
	worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
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

	//コントローラーが接続されているとき
	if (input_->IsControllerConnected())
	{
		//スティックの入力を取得
		velocity_.x = input_->GetLeftStickX();
	}

	//スティックの入力が閾値を超えていたら速度を設定
	if (std::abs(velocity_.x) > threshold)
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

	//コントローラーが接続されているとき
	if (input_->IsControllerConnected())
	{
		//ジャンプ状態に変更
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
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


	if (input_->IsPushKey(DIK_A)) {
		worldTransform_.translation_.x -= 0.3f;
	}

	if (input_->IsPushKey(DIK_D)) {
		worldTransform_.translation_.x += 0.3f;
	}

	if (input_->IsPushKeyEnter(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kJump;
		const float kJumpFirstSpeed = 0.8f;
		worldTransform_.translation_.y += kJumpFirstSpeed;
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

	//スティックの入力を取得
	velocity_.x = input_->GetLeftStickX();

	//スティックの入力が閾値を超えていたら
	if (std::abs(velocity_.x) > threshold)
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
	if (worldTransform_.translation_.y <= -10.0f)
	{
		behaviorRequest_ = Behavior::kRoot;
		worldTransform_.translation_.y = -10.0f;
		velocity_.y = 0.0f;
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


	if (input_->IsPushKey(DIK_A)) {
		worldTransform_.translation_.x -= 0.3f;
	}

	if (input_->IsPushKey(DIK_D)) {
		worldTransform_.translation_.x += 0.3f;
	}

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
}

void Player::ApplyGlobalVariables() 
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	speed_ = globalVariables->GetFloatValue(groupName, "Speed");
	gravity_ = globalVariables->GetFloatValue(groupName, "Gravity");
	jumpFirstSpeed_ = globalVariables->GetFloatValue(groupName, "JumpFirstSpeed");
}