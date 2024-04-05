#include "Player.h"
#include "Engine/Base/ImGuiManager.h"

void Player::Initialize(Model* model)
{
	input_ = Input::GetInstance();

	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_.y = -10.0f;

	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Player::Update()
{
	//Behaviorの遷移処理
	if (behaviorRequest_)
	{
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kJump:
			BehaviorJumpInitialize();
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
	}

	worldTransform_.UpdateMatrixFromEuler();

	ImGui::Begin("Player");
	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Player::BehaviorRootInitialize() {

}

void Player::BehaviorRootUpdate() {
	velocity_.x = 0.0f;
	//移動
	worldTransform_.translation_ += velocity_;
	//重力加速度
	const float kGravity = 0.05f;
	//加速ベクトル
	Vector3 accelerationVector = { 0.0f, -kGravity, 0.0f };
	//加速
	velocity_ += accelerationVector;

	if (input_->IsControllerConnected())
	{
		//しきい値
		const float threshold = 0.7f;

		//移動フラグ
		bool isMoving = false;

		//移動量
		Vector3 move = {
			input_->GetLeftStickX(),
			0.0f,
			0.0f,
		};

		//スティックの押し込みが遊び範囲を超えていたら移動フラグをtrueにする
		if (Mathf::Length(move) > threshold)
		{
			isMoving = true;
		}

		//スティックによる入力がある場合
		if (isMoving)
		{
			//速さ
			const float speed = 0.3f;

			//移動量に速さを反映
			move = Mathf::Normalize(move) * speed;

			//移動
			worldTransform_.translation_ += move;
			velocity_.x = move.x;
		}
		else
		{
			move = { 0.0f,0.0f,0.0f };
			velocity_.x = 0.0f;
		}
	}

	if (input_->IsPushKey(DIK_A)) {
		worldTransform_.translation_.x -= 0.3f;
	}

	if (input_->IsPushKey(DIK_D)) {
		worldTransform_.translation_.x += 0.3f;
	}


	//地面より下に行かないようにする
	if (worldTransform_.translation_.y <= -10.0f)
	{
		worldTransform_.translation_.y = -10.0f;
	}

	//ジャンプ行動に変更
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A)) 
		{
			behaviorRequest_ = Behavior::kJump;
			const float kJumpFirstSpeed = 0.8f;
			worldTransform_.translation_.y += kJumpFirstSpeed;
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kJump;
		const float kJumpFirstSpeed = 0.8f;
		worldTransform_.translation_.y += kJumpFirstSpeed;
	}

}

void Player::BehaviorJumpInitialize()
{
	const float kJumpFirstSpeed = 0.8f;
	velocity_.y = kJumpFirstSpeed;
}

void Player::BehaviorJumpUpdate()
{
	velocity_.x = 0.0f;
	worldTransform_.translation_ += velocity_;
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_ += accelerationVector;

	if (input_->IsControllerConnected())
	{
		//しきい値
		const float threshold = 0.7f;

		//移動フラグ
		bool isMoving = false;

		//移動量
		Vector3 move = {
			input_->GetLeftStickX(),
			0.0f,
			0.0f,
		};

		//スティックの押し込みが遊び範囲を超えていたら移動フラグをtrueにする
		if (Mathf::Length(move) > threshold)
		{
			isMoving = true;
		}

		//スティックによる入力がある場合
		if (isMoving)
		{
			//速さ
			const float speed = 0.3f;

			//移動量に速さを反映
			move = Mathf::Normalize(move) * speed;

			//移動
			worldTransform_.translation_ += move;
			velocity_.x = move.x;
		}
		else
		{
			move = { 0.0f,0.0f,0.0f };
			velocity_.x = 0.0f;
		}
	}

	if (input_->IsPushKey(DIK_A)) {
		worldTransform_.translation_.x -= 0.3f;
	}

	if (input_->IsPushKey(DIK_D)) {
		worldTransform_.translation_.x += 0.3f;
	}

	if (worldTransform_.translation_.y <= -10.0f)
	{
		worldTransform_.translation_.y = -10.0f;
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeBlock)
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

			//通常状態に戻す
			behaviorRequest_ = Behavior::kRoot;
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
		worldTransform_.UpdateMatrixFromEuler();
	}
}

const Vector3 Player::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}