#include "Enemy.h"
#include "Engine/Base/ImGuiManager.h"
#include <Engine/3D/Model/ModelManager.h>

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

	for (int y = 0; y < 36; ++y) {
		for (int x = 0; x < 36; ++x) {
			map[x][y] = 0;
		}
	}
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
	for (int i = 0; i < 70; i++) {
		//worldTransformMap_[i].UpdateMatrixFromEuler();
	}

	


	ImGui::Begin("Enemy");
	ImGui::Text("PlayerPos X%d,Y%d", int(playerPosition_.x), int(playerPosition_.y));
	ImGui::Text("MapPlayerPos %d", map[int(playerPosition_.x)][int(playerPosition_.y)]);
	ImGui::Text("MapEnemyPos %d", map[int(enemyPosition_.x)][int(enemyPosition_.y)]);
	ImGui::Text("MapEnemyPos+10 %d", map[int(enemyPosition_.x+10)][int(enemyPosition_.y)]);


	 for (int y = 0; y < 36; ++y) {
        ImGui::BeginGroup(); // 次の行に表示するセルをグループ化する

        for (int x = 0; x < 36; ++x) {
            ImGui::Text("%d", map[x][y]); // 配列の要素を表示
            ImGui::SameLine(); // 次のセルを同じ行に表示する
        }

        ImGui::EndGroup(); // グループ終了
    }
	ImGui::End();

	
}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
	for (int i = 0; i < 80; i++) {
		//mapModel_[i]->Draw(worldTransformMap_[i], camera);
	}


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
	velocity_ += accelerationVector;



	moveCount_--;
	// エネミーの移動
	if (moveCount_ <= 0 && !path_.empty()) {
		//目的地の位置を次のノードの位置に
		playerPosition_.x = float(path_[1]->x);
		playerPosition_.y = float(path_[1]->y);
		// パスを更新
		path_.erase(path_.begin(), path_.begin() + 1);
		//ナップチップの重みごとに次の移動にかかる時間を変更
		
		if (map[int(enemyPosition_.x)][int(enemyPosition_.y)] != 9) {
				moveCount_ = 10;
		}
		else if (map[int(enemyPosition_.x)][int(enemyPosition_.y)] == 9) {
				moveCount_ = 60;
		}
		

	}

	velocity_ = playerPosition_ - worldTransform_.translation_;




	//地面より下に行かないようにする
	if (worldTransform_.translation_.y <= -10.0f)
	{
		worldTransform_.translation_.y = -10.0f;
	}


	if (jump_) {
		behaviorRequest_ = Behavior::kJump;
		const float kJumpFirstSpeed = 0.8f;
		worldTransform_.translation_.y += kJumpFirstSpeed;
	}

}

void Enemy::BehaviorJumpInitialize()
{
	const float kJumpFirstSpeed = 0.8f;
	velocity_.y = kJumpFirstSpeed;

	jump_ = false;
}

void Enemy::BehaviorJumpUpdate()
{
	//velocity_.x = 0.0f;
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

//脅威度マップ
void Enemy::DistanceFunction() {
	
	//敵の場所をマップチップに落とし込む
	enemyPosition_.x = (35 +worldTransform_.translation_.x)/2.0f;
	enemyPosition_.y = (35 -worldTransform_.translation_.y)/2.0f;

	
	//脅威値
	for (int y = 0; y < 36; ++y) {
		for (int x = 0; x < 36; ++x) {

			//プレイヤーの位置からの差
			int differenceX = x - int(playerPosition_.x);
			int differenceY = y - int(playerPosition_.y);

			//整数に直す
			if (differenceX < 0) {differenceX *= -1;}
			if (differenceY < 0) {differenceY *= -1;}

			//脅威度の設定
			if (differenceX < 10 && differenceY < 10) { map[x][y] = 0; }
			if (differenceX < 9 && differenceY < 9) { map[x][y] = 1; }
			if (differenceX < 8 && differenceY < 8) { map[x][y] = 2; }
			if (differenceX < 7 && differenceY < 7) { map[x][y] = 3; }
			if (differenceX < 6 && differenceY < 6) { map[x][y] = 4; }
			if (differenceX < 5 && differenceY < 5) { map[x][y] = 5; }
			if (differenceX < 4 && differenceY < 4) { map[x][y] = 6; }
			if (differenceX < 3 && differenceY < 3) { map[x][y] = 7; }
			if (differenceX < 2 && differenceY < 2) { map[x][y] = 8; }
			if (differenceX < 1 && differenceY < 1) { map[x][y] = 9; }
			
		}
	}

	//ブロックの番号１０
	for (int i = 0; i <= blockCount_; i++) {
		int blockMapPosX = int(blockPosition_[i].x);
		int blockMapPosY = int(blockPosition_[i].y);
		map[blockMapPosX][blockMapPosY] = 10;
		//ブロックの位置
		for (int j = 0; j <= blockSize_[i].x; j++) {
			if (blockMapPosX + j <= 35) {
				map[blockMapPosX + j][blockMapPosY] = 10;
			}
			if (blockMapPosY - j > 0) {
				map[blockMapPosX - j][blockMapPosY] = 10;
			}
		}
	}

	blockCount_ = 0;
	blockSizeCount_ = 0;
}

//経路探索
void Enemy::FindPath() {

	for (int y = 0; y < 36; ++y) {
		for (int x = 0; x < 36; ++x) {
			findMap[x][y] = map[x][y];
		}
	}
	
	path_ = findPath(findMap, int(enemyPosition_.x),int( enemyPosition_.y), int(playerPosition_.x), int(playerPosition_.y));
	
	
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