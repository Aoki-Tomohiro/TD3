#include "Enemy.h"
#include "Engine/Base/ImGuiManager.h"
#include <Engine/3D/Model/ModelManager.h>
#include <Application/Src/Scene/GamePlayScene.h>

void Enemy::Initialize(const Vector3& position, const uint32_t id)
{
	audio_ = Audio::GetInstance();

	model_ = ModelManager::CreateFromModelFile("Human.gltf", "Enemy" + std::to_string(id), Opaque);
	model_->GetMaterial(0)->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	model_->GetAnimation()->PlayAnimation();


	impactScopeModel_ = ModelManager::CreateFromModelFile("Light.gltf", "EnemyImpactScope" + std::to_string(id), Transparent);
	impactScopeWorldTransform_.Initialize();
	//impactScopeWorldTransform_.scale_ = { 4.0f,4.0f,0.3f };
	impactScopeWorldTransform_.scale_ = { 4.0f,16.0f,0.3f };

	startPosition_ = position;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.quaternion_ = destinationQuaternion_;
	worldTransform_.scale_ = { 3.0f,3.0f,3.0f };
	worldTransform_.UpdateMatrixFromQuaternion();

	//衝突判定の初期化
	AABB aabb = {
	.min{-1.0f,-1.0f,-1.0f},
	.max{1.0f,3.0f,1.0f}
	};
	//衝突属性を設定
	SetAABB(aabb);
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	for (int y = 0; y < 36; ++y) {
		for (int x = 0; x < 36; ++x) {
			map[x][y] = 0;
		}
	}

	for (int i = 0; i < 36; ++i) {
		findMap.push_back(std::vector<int>(36, 0)); // 36個の要素を0で初期化して追加
	}
	blockHit_ = true;

	nextMapPosition_.x = (36 + worldTransform_.translation_.x) / 2.0f;
	nextMapPosition_.y = (36 - worldTransform_.translation_.y) / 2.0f;

	//パーティクルの初期化
	particleSystem_ = ParticleManager::Create("Enemy");

	attackAudioHandle_ = audio_->LoadAudioFile("Application/Resources/Sounds/Attack.wav");
}

void Enemy::Update()
{
	if (!isTutorial_)
	{
		if (!isEscaping_) {
			DistanceFunction();
			FindPath();
		}
	

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
	}

	//画面端に行ったら逃げる
	if (isEscaping_) {
		animationNumber_ = 4;
		if (int(enemyPosition_.x) <= 0) {
			worldTransform_.translation_.x -= 0.3f;
		}

		if (int(enemyPosition_.x) >= 35) {
			worldTransform_.translation_.x += 0.3f;
		}
		
		
		if (worldTransform_.translation_.x <= -36.5f || worldTransform_.translation_.x >= 36.5f) {
			isGameOver_ = true;
		}
	}

	//ワールドトランスフォームの更新
	worldTransform_.quaternion_ = Mathf::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f);
	impactScopeWorldTransform_.translation_ = worldTransform_.translation_ + impactScopeOffset_;
	worldTransform_.UpdateMatrixFromQuaternion();
	impactScopeWorldTransform_.UpdateMatrixFromEuler();

	

	//モデルの更新
	model_->Update(worldTransform_, animationNumber_);

	//影響範囲
	impactScopeModel_->GetMaterial(0)->SetColor(Vector4{ 1.0f,0.0f,0.0f,0.3f });

	ImGui::Begin("Enemy");
	ImGui::Text("PlayerPos X%d,Y%d", int(playerPosition_.x), int(playerPosition_.y));
	ImGui::Text("MapPlayerPos %d", map[int(playerPosition_.x)][int(playerPosition_.y)]);
	ImGui::Text("enemyPos X%d,Y%d", int(enemyPosition_.x), int(enemyPosition_.y));
	//ImGui::Text("MapNextPos %d", map[int(nextPosition_.x)][int(nextPosition_.y)]);
	if(copy_.size() !=0){
		ImGui::Text("CopyPos X%d,Y%d", int(copy_[0]->GetWorldPosition().x), int(copy_[0]->GetWorldPosition().y));
	}
	
	ImGui::Text("MapEnemyPos %d", map[int(enemyPosition_.x)][int(enemyPosition_.y)]);
	ImGui::Text("MapEnemyPos+1 %d", map[int(enemyPosition_.x)][int(enemyPosition_.y) +1]);
	ImGui::DragFloat3("worldPos", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("velocity", &velocity_.x, 0.1f);
	ImGui::Text("%d", blockHit_);
	for (int y = 0; y < 36; ++y) {
		ImGui::BeginGroup(); // 次の行に表示するセルをグループ化する
		for (int x = 0; x < 36; ++x) {
			ImGui::Text("%d", map[x][y]); // 配列の要素を表示
			ImGui::SameLine(); // 次のセルを同じ行に表示する
		}

		ImGui::EndGroup(); // グループ終了
	}
	ImGui::DragFloat3("ImpactScopeOffset", &impactScopeOffset_.x, 0.1f);
	ImGui::DragFloat3("ImpactScopeScale", &impactScopeWorldTransform_.scale_.x, 0.1f);
	ImGui::End();


}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
	if (!isTutorial_)
	{
		impactScopeModel_->Draw(impactScopeWorldTransform_, camera);
	}
}

void Enemy::Reset()
{
	isActive_ = true;
	isEscaping_ = false;
	isResult_ = false;
	isGameOver_ = false;
	blockHit_ = false;
	search_ = false;
	jump_ = false;
	velocity_.y = 0;
	//worldTransform_.translation_ = startPosition_;
}

void Enemy::BehaviorRootInitialize() {
	moveCount_ = 0;
	blockHit_ = true;
	model_->GetAnimation()->SetLoop(true);
}

void Enemy::BehaviorRootUpdate() {

	velocity_.x = 0.0f;
	//移動
	//worldTransform_.translation_ += velocity_;
	//重力加速度
	const float kGravity = 0.05f;
	//加速ベクトル
	Vector3 accelerationVector = { 0.0f, -kGravity, 0.0f };
	//加速
	velocity_ += accelerationVector;

	float speed = 0.3f;
	if (isDoubleSpeed_)
	{
		speed *= 4.0f;
	}

	if (map[int(enemyPosition_.x)][int(enemyPosition_.y) + 1] == 10) {
		velocity_.y = 0.00f;
		velocity_.x = 0.00f;

		
	}
	else {

		if (velocity_.y >= -1.0f) {
			if (playerPosition_.x < enemyPosition_.x) {
				if (map[int(enemyPosition_.x-1)][int(enemyPosition_.y) + 1] != 10) {
					velocity_.x = -speed;
				}else if (map[int(enemyPosition_.x + 1)][int(enemyPosition_.y) + 1] != 10) {
					velocity_.x = speed;
				}
				
			}
			else {
				if (map[int(enemyPosition_.x - 1)][int(enemyPosition_.y) + 1] != 10) {
					velocity_.x = -speed;
				}
				else if (map[int(enemyPosition_.x + 1)][int(enemyPosition_.y) + 1] != 10) {
					velocity_.x = speed;
				}
			}
		}
		else {
			velocity_.x = 0.0f;
		}


		if (velocity_.x == speed) {
			if (map[int(enemyPosition_.x) + 1][int(enemyPosition_.y)] >= 3) {
				velocity_.x = 0.0f;
			}
		}
		else {
			if (map[int(enemyPosition_.x) - 1][int(enemyPosition_.y)] >= 3) {
				velocity_.x = 0.0f;
			}
		}

		if (map[int(enemyPosition_.x)][int(enemyPosition_.y)] == 0) {
			velocity_.x = 0.0f;
		}

		worldTransform_.translation_ += velocity_;
	}

	//上にプレイヤーがいた時の移動
	if (int(enemyPosition_.y - 2) == int(playerPosition_.y) && velocity_.x == 0.0f) {

		if (int(playerPosition_.x) == int(enemyPosition_.x - 1) || int(playerPosition_.x) == int(enemyPosition_.x + 1) || int(playerPosition_.x) == int(enemyPosition_.x)) {
			if (enemyPosition_.x < playerPosition_.x && worldTransform_.translation_.x > -32.0f) {
				velocity_.x = -speed;

				worldTransform_.translation_ += velocity_;
			}

			if (enemyPosition_.x > playerPosition_.x && worldTransform_.translation_.x < 32.0f) {
				velocity_.x = speed;
				worldTransform_.translation_ += velocity_;
			}
		}
	}

	moveCount_--;
	// エネミーの移動
	if (velocity_.x == 0.0f &&path_.size() > 1) {
		waitAnimationCoolTimer_ = 0;
		//目的地の位置を次のノードの位置に
		nextPosition_.x = float(path_[1]->x - 18) * 2;
		nextPosition_.y = float(path_[1]->y - 18) * -2;


		


		for (int i = 0; i < copy_.size(); ++i) {

			if (velocity_.x == 0.3 && worldTransform_.translation_.y == copy_[i]->GetWorldPosition().x) {
				if (worldTransform_.translation_.x + speed == copy_[i]->GetWorldPosition().x) {
					//velocity_.x = 0.0f;
				}
			}
			else if (velocity_.x == -0.3 && worldTransform_.translation_.y == copy_[i]->GetWorldPosition().x) {
				if (worldTransform_.translation_.x - speed == copy_[i]->GetWorldPosition().x) {
					//velocity_.x = 0.0f;
				}
			}
			



		}

		float differenceX = float(nextPosition_.x) - worldTransform_.translation_.x;
		if (differenceX < 0) { differenceX *= -1; }


		if (differenceX <= 0.2f) {
			velocity_.x = 0.0f;
		}



		if (nextPosition_.x > worldTransform_.translation_.x) {
			velocity_.x = speed;
		}
		else {
			velocity_.x = -speed;

		}

		if (velocity_.x != 0.0f)
		{
			animationNumber_ = 4;
			if (velocity_.x == speed)
			{
				destinationQuaternion_ = { 0.0f,0.707f,0.0f,0.707f };
			}
			else
			{
				destinationQuaternion_ = { 0.0f,-0.707f,0.0f,0.707f };
			}
		}

		

		if (velocity_.x == speed) {
			if (map[int(enemyPosition_.x) + 1][int(enemyPosition_.y)] >= 3) {
				velocity_.x = 0.0f;
			}
		}
		else {
			if (map[int(enemyPosition_.x) - 1][int(enemyPosition_.y)] >= 3) {
				velocity_.x = 0.0f;
			}
		}

		if (map[int(enemyPosition_.x)][int(enemyPosition_.y)] == 0) {
			velocity_.x = 0.0f;
		}
		worldTransform_.translation_ += velocity_;

		//velocity_ = enemyPosition_ - worldTransform_.translation_;
		if (worldTransform_.translation_ == velocity_) {
			// パスを更新
			path_.erase(path_.begin(), path_.begin() + 1);
		}

		moveCount_ = 10;
	}
	else
	{
		animationNumber_ = 1;
		if (++waitAnimationCoolTimer_ > 60)
		{
			animationNumber_ = 0;
			destinationQuaternion_ = { 0.0f,1.0f,0.0f,0.0f };
		}
	}

	
	if (int(enemyPosition_.x) <= 0 || int(enemyPosition_.x) >= 35) {
		isEscaping_ = true;
	}
	

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
	const float kJumpFirstSpeed =1.0f;
	velocity_.y = kJumpFirstSpeed;
	model_->GetAnimation()->SetAnimationTime(0.0f);
	model_->GetAnimation()->SetLoop(false);
	animationNumber_ = 2;
	float speed = 0.3f;
	if (isDoubleSpeed_)
	{
		speed *= 4.0f;
	}

	/*
	if (playerPosition_.y <= enemyPosition_.y) {
		for (int i = 0; i <= blockCount_; i++) {
			int blockMapPosX = int(blockPosition_[i].x);
			int blockMapPosY = int(blockPosition_[i].y);

			if (enemyPosition_.y < blockMapPosY) {
				if (enemyPosition_.x < blockMapPosX) {
					velocity_.x = 0.3f;
				}
				else {
					velocity_.x = -0.3f;
				}
			}
		}
	}*/

	if (dir_ == 1) {
		velocity_.x = speed;
	}
	else if(dir_ == 2 ){
		velocity_.x = -speed;
	}

	blockHit_ = false;
	search_ = false;
	jump_ = false;
}

void Enemy::BehaviorJumpUpdate()
{
	//velocity_.x = 0.0f;
	worldTransform_.translation_ += velocity_;
	float kGravityAcceleration = 0.055f;
	if (isDoubleSpeed_)
	{
		kGravityAcceleration *= 4.0f;
	}
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_ += accelerationVector;


	nextMapPosition_.x = (36 + worldTransform_.translation_.x) / 2.0f;
	nextMapPosition_.y = (36 - worldTransform_.translation_.y) / 2.0f;

	if (worldTransform_.translation_.y <= -10.0f || velocity_.y <= -0.7f)
	{
		//worldTransform_.translation_.y = -10.0f;
		behaviorRequest_ = Behavior::kRoot;
	}
}

//脅威度マップ
void Enemy::DistanceFunction() {

	//敵の場所をマップチップに落とし込む
	enemyPosition_.x = (36 + worldTransform_.translation_.x) / 2.0f;
	enemyPosition_.y = (36 - worldTransform_.translation_.y) / 2.0f;

	for (int y = 0; y < 36; ++y) {
		for (int x = 0; x < 36; ++x) {
			map[x][y] = 0;
		}
	}

	//脅威値
	//プレーヤー
	for (int y = 0; y < 36; ++y) {
		for (int x = 0; x < 36; ++x) {

			//プレイヤーの位置からの差
			int differenceX = x - int(playerPosition_.x);
			int differenceY = y - int(playerPosition_.y);

			//整数に直す
			if (differenceX < 0) { differenceX *= -1; }
			if (differenceY < 0) { differenceY *= -1; }

			//脅威度の設定 3が自分の位置

			if (differenceX < 3 && differenceY < 3) {
				if (map[x][y] <= 1) {
					map[x][y] = 1;
				}

			}
			if (differenceX < 2 && differenceY < 2) {
				if (map[x][y] <= 2) {
					map[x][y] = 2;
				}

			}
			if (differenceX < 1 && differenceY < 1) {
				if (map[x][y] <= 3) {
					map[x][y] = 3;
				}
			}


		}
	}

	
	//ブロックの番号１０
	for (int i = 0; i <= blockCount_; i++) {
		int blockMapPosX = int(blockPosition_[i].x);
		int blockMapPosY = int(blockPosition_[i].y);
		map[blockMapPosX][blockMapPosY] = 10;



		//ブロックの位置
		for (int j = 0; j <= blockSize_[i].x; j++) {
			//上側
			if (blockMapPosX + j < 36) {
				map[blockMapPosX + j][blockMapPosY] = 10;
				//右側
				if (playerPosition_.y > blockMapPosY) {
					//ブロックより下にいる時
					map[blockMapPosX + j][blockMapPosY - 1] = 0;
					map[blockMapPosX + j][blockMapPosY - 2] = 0;
					map[blockMapPosX + j][blockMapPosY - 3] = 0;
				}
				else {
					//ブロックより上にいる時
					map[blockMapPosX + j][blockMapPosY + 1] = 0;
					map[blockMapPosX + j][blockMapPosY + 2] = 0;
					map[blockMapPosX + j][blockMapPosY + 3] = 0;
				}

			}
			//下側
			if (blockMapPosY - j > 0) {
				map[blockMapPosX - j][blockMapPosY] = 10;
				//左側
				if (playerPosition_.y > blockMapPosY) {
					//ブロックより下にいる時
					map[blockMapPosX - j][blockMapPosY - 1] = 0;
					map[blockMapPosX - j][blockMapPosY - 2] = 0;
					map[blockMapPosX - j][blockMapPosY - 3] = 0;

				}
				else {
					//ブロックより上にいる時
					map[blockMapPosX - 1][blockMapPosY + 1] = 0;
					map[blockMapPosX - 1][blockMapPosY + 2] = 0;
					map[blockMapPosX - 1][blockMapPosY + 3] = 0;
					map[blockMapPosX - 2][blockMapPosY + 1] = 0;
					map[blockMapPosX - 2][blockMapPosY + 2] = 0;
					map[blockMapPosX - 2][blockMapPosY + 3] = 0;
					map[blockMapPosX - 3][blockMapPosY + 1] = 0;
					map[blockMapPosX - 3][blockMapPosY + 2] = 0;
					map[blockMapPosX - 3][blockMapPosY + 3] = 0;

				}
			}
		}
	}




	//コピー
	for (int y = 0; y < 36; ++y) {
		for (int x = 0; x < 36; ++x) {
			for (int i = 0; i < copy_.size(); ++i) {

				//プレイヤーの位置からの差
				int differenceX = x - int((36 + copy_[i]->GetWorldPosition().x) / 2.0f);
				int differenceY = y - int((36 - copy_[i]->GetWorldPosition().y) / 2.0f);

				//整数に直す
				if (differenceX < 0) { differenceX *= -1; }
				if (differenceY < 0) { differenceY *= -1; }

				//脅威度の設定 3が自分の位置
				
				if (differenceX < 3 && differenceY < 3) {
					if (map[x][y] <= 1) {
						map[x][y] = 1;
					}

				}
				if (differenceX < 2 && differenceY < 2) {
					if (map[x][y] <= 2) {
						map[x][y] = 2;
					}

				}
				if (differenceX < 1 && differenceY < 1) {
					if (map[x][y] <= 3) {
						map[x][y] = 3;
					}
				}
			}


		}
	}

	
	
	
	
	


	//敵の位置から最も近い0を検索
	if (map[int(enemyPosition_.x)][int(enemyPosition_.y)] != 0) {
		search_ = true;
		for (int y = 0; y < 24; ++y) {
			for (int x = 0; x < 36; ++x) {
				int differenceX = x - int(enemyPosition_.x);
				int differenceY = y - int(enemyPosition_.y);


				//整数に直す
				if (differenceX < 0) { differenceX *= -1; }
				if (differenceY < 0) { differenceY *= -1; }

				if (differenceX <= 1 && differenceY <= 1 && y > 16) {

					if (map[x][y] < map[int(enemyPosition_.x)][int(enemyPosition_.y)]) {
						nextMapPosition_.x = float(x);
						nextMapPosition_.y = float(y);
						break;
					}
				}
				if (differenceX <= 2 && differenceY <= 2 && y > 16) {

					if (map[x][y] < map[int(enemyPosition_.x)][int(enemyPosition_.y)]) {
						nextMapPosition_.x = float(x);
						nextMapPosition_.y = float(y);
						
						break;
					}
				}
				
				if (differenceX <= 5 && differenceY <= 5 && y > 16) {

				

					for (int x = 1; x < 5; x++) {
						if (map[int(enemyPosition_.x)][int(enemyPosition_.y) + 1] == 10 || int(enemyPosition_.y) == 23) {

							if (map[int(enemyPosition_.x) + 3][int(enemyPosition_.y) - x] == 10 && map[int(enemyPosition_.x) + 2][int(enemyPosition_.y) - x] != 10 && map[int(enemyPosition_.x) + 1][int(enemyPosition_.y - x)] != 10 && map[int(enemyPosition_.x)][int(enemyPosition_.y - x)] != 10) {
								if (map[int(enemyPosition_.x) + 3][int(enemyPosition_.y - 5)] == 0 && map[int(enemyPosition_.x)][int(enemyPosition_.y)] != 3 && map[int(enemyPosition_.x)][int(enemyPosition_.y) - 1] != 3) {
									jump_ = true;
									dir_ = 1;
								}
								else {
									jump_ = false;
									dir_ = 0;
								}
								break;

							}
							if (map[int(enemyPosition_.x) - 3][int(enemyPosition_.y) - x] == 10 && map[int(enemyPosition_.x) - 2][int(enemyPosition_.y) - x] != 10 && map[int(enemyPosition_.x) - 1][int(enemyPosition_.y - x)] != 10 && map[int(enemyPosition_.x)][int(enemyPosition_.y - x)] != 10) {
								if (map[int(enemyPosition_.x) - 3][int(enemyPosition_.y - 5)] == 0 && map[int(enemyPosition_.x)][int(enemyPosition_.y)] != 3 && map[int(enemyPosition_.x)][int(enemyPosition_.y) - 1] != 3) {
									jump_ = true;
									dir_ = 2;

								}
								else {
									jump_ = false;
									dir_ = 0;

								}
								break;
							}

						}

					}

					


				}
				

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

			findMap[y][x] = map[y][x];
		}
	}

	if (!jump_) {
		path_ = findPath(findMap, int(enemyPosition_.x), int(enemyPosition_.y), int(nextMapPosition_.x), int(nextMapPosition_.y));

	}

}
void Enemy::Reverse(const uint32_t stepSize)
{
	//過去のデータがからではなかったら
	if (!reverseData_.empty())
	{
		//前の座標を取得
		Vector3 prePosition = GetWorldPosition();

		//過去のデータを取得
		Vector3 position{};
		uint32_t animationNumber = 0;
		float animationTime = 0.0f;
		std::tie(position, animationNumber, animationTime) = reverseData_.back();
		reverseData_.pop_back();

		//座標を代入
		worldTransform_.translation_ = position;

		//移動していたら
		if (prePosition.x != position.x)
		{
			//回転させる
			if (prePosition.x > position.x)
			{
				destinationQuaternion_ = { 0.0f,0.707f,0.0f,0.707f };
			}
			else
			{
				destinationQuaternion_ = { 0.0f,-0.707f,0.0f,0.707f };
			}
		}

		//ワールドトランスフォームの更新
		worldTransform_.quaternion_ = Mathf::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f);
		impactScopeWorldTransform_.translation_ = worldTransform_.translation_ + impactScopeOffset_;
		worldTransform_.UpdateMatrixFromQuaternion();
		impactScopeWorldTransform_.UpdateMatrixFromEuler();

		//アニメーションの更新
		model_->GetAnimation()->SetAnimationTime(animationTime);
		model_->Update(worldTransform_, animationNumber);

		//巻き戻しの速度を上げるためにデータをスキップ
		for (uint32_t i = 0; i < stepSize; ++i)
		{
			if (!reverseData_.empty())
			{
				reverseData_.pop_back();
			}
		}
	}
}

void Enemy::UpdateMatrix()
{
	worldTransform_.UpdateMatrixFromQuaternion();
}

void Enemy::SaveReverseData()
{
	//巻き戻し用のデータを保存
	reverseData_.push_back({ GetWorldPosition(),animationNumber_,model_->GetAnimation()->GetAnimationTime() });
}

void Enemy::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeBlock || collider->GetCollisionAttribute() == kCollisionAttributeWall)
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



			if (directionAxis.y == 1.0f)
			{
				//velocity_.x = 0.0f;
			}
			

		}
		else if (overlapAxis.z < overlapAxis.x && overlapAxis.z < overlapAxis.y)
		{
			directionAxis.z = (worldTransform_.translation_.z < collider->GetWorldTransform().translation_.z) ? -1.0f : 1.0f;
			directionAxis.x = 0.0f;
			directionAxis.y = 0.0f;
		}

		if (behaviorRequest_ == Behavior::kJump) {
			search_ = true;
		}


		worldTransform_.translation_ += overlapAxis * directionAxis;
		worldTransform_.UpdateMatrixFromQuaternion();
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributeWeapon)
	{
		ParticleEmitter* emitter = ParticleEmitterBuilder()
			.SetDeleteTime(1.0f)
			.SetPopArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetPopAzimuth(0.0f, 360.0f)
			.SetPopColor({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f })
			.SetPopCount(100)
			.SetPopElevation(0.0f, 0.0f)
			.SetPopFrequency(2.0f)
			.SetPopLifeTime(0.2f, 0.4f)
			.SetPopScale({ 0.1f,0.1f,0.1f }, { 0.2f,0.2f,0.2f })
			.SetPopVelocity({ 0.4f,0.4f,0.4f }, { 0.6f,0.6f,0.6f })
			.SetTranslation(GetWorldPosition())
			.Build();
		particleSystem_->AddParticleEmitter(emitter);
		isActive_ = false;
		audio_->PlayAudio(attackAudioHandle_, false, 0.4f);
	}
	if (!isActive_ || isEscaping_) {
		isResult_ = true;
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