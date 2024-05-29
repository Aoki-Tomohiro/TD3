#include "Copy.h"

void Copy::Initialize(const std::vector<std::tuple<Vector3, bool, uint32_t, float>>& playerPositions)
{
	//モデルの初期化
	model_.reset(ModelManager::CreateFromModelFile("Human.gltf", Opaque));
	model_->GetMaterial(0)->SetColor({ 0.2118f, 0.8196f, 0.7137f, 1.0f });

	//impactScopeModel_.reset(ModelManager::CreateFromModelFile("Cube.obj", Transparent));


	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };

	//impactScopeWorldTransform_.Initialize();
	//impactScopeWorldTransform_.scale_ = { 4.0f,4.0f,0.3f };

	//キューの初期化
	playerPositions_ = playerPositions;

	//武器の生成
	weaponModel_.reset(ModelManager::CreateFromModelFile("Cube.obj", Transparent));
	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize(weaponModel_.get());
	weapon_->SetParent(worldTransform_);

	//衝突判定の初期化
	AABB aabb = {
	.min{-1.0f,-1.0f,-1.0f},
	.max{1.0f,2.0f,1.0f}
	};
	SetAABB(aabb);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Copy::Update()
{
	isReverse_ = false;
	uint32_t animationNumber = 0;
	float animationTime = 0;
	//座標を設定
	if (currentIndex_ < playerPositions_.size())
	{
		//前のフレームのワールド座標を取得
		Vector3 prePlayerPosition = GetWorldPosition();

		//新しく設定する座標と攻撃フラグ
		Vector3 playerPosition{};
		bool isAttack{};

		//データを取得
		std::tie(playerPosition, isAttack,animationNumber,animationTime) = playerPositions_[currentIndex_];

		//座標を代入
		worldTransform_.translation_ = playerPosition;

		//前のフレームの座標と現在のフレームの座標が変わっていたら回転させる
		if (prePlayerPosition.x != playerPosition.x)
		{
			if (prePlayerPosition.x > playerPosition.x)
			{
				destinationQuaternion_ = { 0.0f,-0.707f,0.0f,0.707f };
			}
			else
			{
				destinationQuaternion_ = { 0.0f,0.707f,0.0f,0.707f };
			}
		}

		//攻撃フラグを設定
		weapon_->SetIsAttack(isAttack);

		//インデックスを進める
		currentIndex_++;
	}
	else
	{
		//移動していない状態にする
		isActive_ = false;
		//武器の当たり判定をなくす
		weapon_->SetIsAttack(false);
		//加速ベクトル
		Vector3 accelerationVector = { 0.0f, -gravity_, 0.0f };
		//加速
		velocity_ += accelerationVector;
		//速度を加算
		worldTransform_.translation_ += velocity_;
	}

	//地面に埋まらないようにする
	if (worldTransform_.translation_.y <= -10.0f)
	{
		worldTransform_.translation_.y = -10.0f;
		velocity_.y = 0.0f;
	}

	//ワールドトランスフォームの更新
	worldTransform_.quaternion_ = Mathf::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f);
	//impactScopeWorldTransform_.translation_ = worldTransform_.translation_;
	worldTransform_.UpdateMatrixFromQuaternion();
	//impactScopeWorldTransform_.UpdateMatrixFromEuler();

	//モデルの更新
	model_->GetAnimation()->SetAnimationTime(animationTime);
	model_->Update(worldTransform_, animationNumber);

	//影響範囲
	//impactScopeModel_->GetMaterial(1)->SetColor(Vector4{ 0.0f,1.0f,1.0f,0.3f });

	//攻撃が当たっていたらフラグをtrueにする
	if (weapon_->GetIsAttack() && weapon_->GetIsHit())
	{
		isEnemyDefeated_ = true;
	}
	//ヒットフラグをfalseにする
	weapon_->SetIsHit(false);

	//武器の更新
	weapon_->Update();


	ImGui::Begin("Copy");
	//ImGui::SliderFloat3("Scale", &impactScopeWorldTransform_.scale_.x, 1.0f, 5.0f);
	ImGui::End();
}

void Copy::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera);

	//影響範囲の表示
	if (!isReverse_) {
		//impactScopeModel_->Draw(impactScopeWorldTransform_, camera);
	}
	
	//武器の描画
	weapon_->Draw(camera);
}

void Copy::Reset()
{
	isActive_ = true;
	isEnemyDefeated_ = false;
	velocity_ = { 0.0f,0.0f,0.0f };
}

void Copy::OnCollision(Collider* collider)
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
	}
}

const Vector3 Copy::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}

void Copy::Reverse(const uint32_t stepSize)
{
	isReverse_ = true;
	if (currentIndex_ > 0)
	{
		//インデックスを減らす
		currentIndex_--;

		//巻き戻しの速度を上げるためにデータをスキップ
		for (uint32_t i = 0; i < stepSize; ++i)
		{
			if (currentIndex_ > 0)
			{
				currentIndex_--;
			}
		}

		//前の座標を取得
		Vector3 prePosition = GetWorldPosition();

		//過去のデータを取得
		Vector3 playerPosition{};
		uint32_t animationNumber = 0;
		float animationTime = 0.0f;
		bool isAttack{};
		std::tie(playerPosition, isAttack, animationNumber, animationTime) = playerPositions_[currentIndex_];

		//座標を代入
		worldTransform_.translation_ = playerPosition;

		//移動していたら
		if (prePosition.x != playerPosition.x)
		{
			//回転させる
			if (prePosition.x > playerPosition.x)
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
		worldTransform_.UpdateMatrixFromQuaternion();

		//アニメーションの更新
		model_->GetAnimation()->SetAnimationTime(animationTime);
		model_->Update(worldTransform_, animationNumber);

		//武器の更新
		weapon_->SetIsAttack(isAttack);
		weapon_->Update();
	}
}