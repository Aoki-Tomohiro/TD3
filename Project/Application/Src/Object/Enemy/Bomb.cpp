#include "Bomb.h"

void Bomb::Initialize(Model* model, const Vector3& position)
{
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeBomb);
	SetCollisionMask(kCollisionMaskBomb);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Bomb::Update()
{
	//加速ベクトル
	Vector3 accelerationVector = { 0.0f, -gravity_, 0.0f };
	//加速
	velocity_ += accelerationVector;
	//速度を加算
	worldTransform_.translation_ += velocity_;
	//徐々に遅くする
	if (isMove_)
	{
		const float decay = 0.025f;
		if (velocity_.x > 0.0f)
		{
			if (velocity_.x - decay < 0.0f)
			{
				isMove_ = false;
				velocity_.x = 0.0f;
			}
			else
			{
				velocity_.x -= decay;
			}
		}
		else
		{
			if (velocity_.x + decay > 0.0f)
			{
				isMove_ = false;
				velocity_.x = 0.0f;
			}
			else
			{
				velocity_.x += decay;
			}
		}
	}

	//ワールドトランスフォームの更新
	if (velocity_.x != 0.0f)
	{
		worldTransform_.rotation_.z += velocity_.x;
	}
	worldTransform_.UpdateMatrixFromEuler();
}

void Bomb::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Bomb::OnCollision(Collider* collider)
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
		if (collider->GetCollisionAttribute() == kCollisionAttributePlayer)
		{
			isMove_ = true;
			if (directionAxis.x == 1.0f)
			{
				velocity_.x = 0.3f;
			}
			else
			{
				velocity_.x = -0.3f;
			}
		}
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
	worldTransform_.UpdateMatrixFromEuler();
}

const Vector3 Bomb::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}