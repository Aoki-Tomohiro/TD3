#include "Enemy.h"
#include "Engine/Base/ImGuiManager.h"

void Enemy::Initialize(Model* model)
{
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_.y = -10.0f;

	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Enemy::Update()
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

	DistanceFunction();
	
	worldTransform_.UpdateMatrixFromEuler();
	
	ImGui::Begin("Enemy");
	ImGui::Text("PlayerPos X%d,Y%d", int(playerPosition_.x), int(playerPosition_.y));
	ImGui::Text("MapPlayerPos %d", map[int(playerPosition_.x)][int(playerPosition_.y)]);
	ImGui::Text("MapPlayerPos %d", map[int(35)][int(45)]);
	/*
	 for (int y = 0; y < 720; ++y) {
        ImGui::BeginGroup(); // 次の行に表示するセルをグループ化する

        for (int x = 0; x < 1280; ++x) {
            ImGui::Text("%d", map[x][y]); // 配列の要素を表示
            ImGui::SameLine(); // 次のセルを同じ行に表示する
        }

        ImGui::EndGroup(); // グループ終了
    }*/
	ImGui::End();
	
}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Enemy::BehaviorRootInitialize() {

}

void Enemy::BehaviorRootUpdate() {
	velocity_.x = 0.0f;
	//移動
	worldTransform_.translation_ += velocity_;
	//重力加速度
	const float kGravity = 0.05f;
	//加速ベクトル
	Vector3 accelerationVector = { 0.0f, -kGravity, 0.0f };
	//加速
	//velocity_ += accelerationVector;

	if (false) {
		behaviorRequest_ = Behavior::kJump;
		const float kJumpFirstSpeed = 0.8f;
		worldTransform_.translation_.y += kJumpFirstSpeed;
	}

}

void Enemy::BehaviorJumpInitialize()
{
	const float kJumpFirstSpeed = 0.8f;
	velocity_.y = kJumpFirstSpeed;
}

void Enemy::BehaviorJumpUpdate()
{
	velocity_.x = 0.0f;
	worldTransform_.translation_ += velocity_;
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_ += accelerationVector;

	if (worldTransform_.translation_.y <= -10.0f)
	{
		worldTransform_.translation_.y = -10.0f;
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Enemy::DistanceFunction() {
	
	
	//脅威値
	for (int y = 0; y < 70; ++y) {
		for (int x = 0; x < 70; ++x) {

			//プレイヤーの位置からの差
			int differenceX = x - int(playerPosition_.x);
			int differenceY = y - int(playerPosition_.y);
			//整数に直す
			if (differenceX < 0) {differenceX *= -1;}
			if (differenceY < 0) {differenceY *= -1;}

			if (differenceX <= 27 && differenceY <= 25) { map[x][y] = 0; }
			if (differenceX <= 25 && differenceY <= 23) { map[x][y] = 1; }
			if (differenceX <= 23 && differenceY <= 21) { map[x][y] = 2; }
			if (differenceX <= 21 && differenceY <= 19) { map[x][y] = 3; }
			if (differenceX <= 19 && differenceY <= 17) { map[x][y] = 4; }
			if (differenceX <= 17 && differenceY <= 15) { map[x][y] = 5; }
			if (differenceX <= 15 && differenceY <= 13) { map[x][y] = 6; }
			if (differenceX <= 13 && differenceY <= 11) { map[x][y] = 7; }
			if (differenceX <= 11 && differenceY <= 9) { map[x][y] = 8; }
			if (differenceX <= 9 && differenceY <= 7) { map[x][y] = 9; }
			if (differenceX <= 7 && differenceY <= 5)  {map[x][y] = 10;}

		
		}
	}

}

void Enemy::OnCollision(Collider* collider)
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
const Vector3 Enemy::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}