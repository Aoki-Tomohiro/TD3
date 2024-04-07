#include "Enemy.h"

void Enemy::Initialize(Model* model,const Vector3& position)
{
	//モデルの初期化
	assert(model);
	model_ = model;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	//パーティクルシステムの初期化
	particleSystem_ = ParticleManager::Create("Enemy");

	//衝突判定の初期化
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Enemy::Update()
{
	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();
}

void Enemy::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera);
}

void Enemy::OnCollision(Collider* collider)
{
	isAlive_ = false;
	ParticleEmitter* emitter = ParticleEmitterBuilder()
		.SetDeleteTime(1.0f)
		.SetPopArea({0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f})
		.SetPopAzimuth(0.0f,360.0f)
		.SetPopColor({1.0f,1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f,1.0f})
		.SetPopCount(100)
		.SetPopElevation(0.0f,0.0f)
		.SetPopFrequency(2.0f)
		.SetPopLifeTime(0.2f,0.4f)
		.SetPopScale({0.1f,0.1f,0.1f},{0.2f,0.2f,0.2f})
		.SetPopVelocity({0.4f,0.4f,0.4f},{0.6f,0.6f,0.6f})
		.SetTranslation(GetWorldPosition())
		.Build();
	particleSystem_->AddParticleEmitter(emitter);
}

const Vector3 Enemy::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}