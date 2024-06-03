#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Player/Weapon.h"
#include <optional>

class Player : public Collider
{
public:
	~Player();

	void Initialzie(std::vector<Model*> models);

	void Update();

	void Draw(const Camera& camera);

	void DrawUI(const Camera& camera);

	void Reset();

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const;

	Weapon* GetWeapon() { return weapon_.get(); };

	const Quaternion& GetQuaternion() const { return worldTransform_.quaternion_; };

	const bool GetIsStop() const { return isStop_; };

	void SetPositions(const Vector3& position, const bool isAttack, const uint32_t animationNumber, const float animationTime);

	void SetIsTutorial(const bool isTutorial) { isTutorial_ = isTutorial; };

	const uint32_t GetAnimationNumber() const { return animationNumber_; };

	const float GetAnimationTime() const { return animationTime_; };

	void StopAnimation() { models_[0]->GetAnimation()->StopAnimation(); };

	void PlayAnimation() { models_[0]->GetAnimation()->PlayAnimation(); };

	bool GetIsMove() { return isMove_; }

	void SetCutIn(bool flag) { cutIn_ = flag; }

	void TitleUpdate();

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	void SetIsEditing(const bool isEditing) { isEditing_ = isEditing; };

private:
	enum class Behavior
	{
		kRoot,
		kJump,
		kAttack,
	};

	void BehaviorRootInitialize();

	void BehaviorRootUpdate();

	void BehaviorJumpInitialize();

	void BehaviorJumpUpdate();

	void BehaviorAttackInitialize();

	void BehaviorAttackUpdate();

	void UpdateMovementRestriction();

	void UpdateMovementRestrictionSprite(const Camera& camera);

	void ApplyGlobalVariables();

	void Landing();
private:
	//インプット
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//モデル
	std::vector<Model*> models_{};

	//ワールドトランスフォーム
	WorldTransform worldTransform_{};

	//状態
	Behavior behavior_ = Behavior::kRoot;

	//Behaviorのリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//クォータニオン
	Quaternion destinationQuaternion_{ 0.0f,0.707f,0.0f,0.707f };

	//速度ベクトル
	Vector3 velocity_{};

	//X軸の移動速度
	float speed_ = 0.3f;

	//重力加速度
	float gravity_ = 0.05f;

	//ジャンプの初速度
	float jumpFirstSpeed_ = 1.0f;

	//武器
	std::unique_ptr<Weapon> weapon_ = nullptr;

	//着地しているか
	bool isLanded_ = false;

	//移動できるか
	bool isMove_ = true;

	//止まった状態か
	bool isStop_ = false;

	//移動制限時間
	int movementRestrictionTime_ = 60 * 10;

	//移動制限のタイマー
	int movementRestrictionTimer_ = movementRestrictionTime_;

	//制限時間のスプライト
	std::unique_ptr<Sprite> movementRestrictionSprite_ = nullptr;

	//制限時間のスプライトのサイズ
	Vector2 movementRestrictionSpriteSize_ = { 64.0f,12.0f };

	//オーディオハンドル
	uint32_t whiffAudioHandle_ = 0;
	uint32_t moveAudioHandle_ = 0;
	int moveAudioTimer_ = 0;
	int moveAudioWaitTime_ = 340;

	//チュートリアルかどうか
	bool isTutorial_ = false;

	//アニメーションの番号
	uint32_t animationNumber_ = 0;

	//アニメーションタイム
	float animationTime_ = 0.0f;

	
	//パーティクル
	Vector3 prePos_;
	ParticleSystem* particleSystem_;
	Vector2 azimuth_ = {0,0};
	Vector2 PopLifeTime = { 0.1f, 0.6f };
	int PopCount = 1;
	float PopFrequency = 4.0f;
	float DeleteTime = 1.0f;
	std::unique_ptr <Model> particleModel_;
	bool landing_;

	//カットインフラグ
	bool cutIn_;

	bool isEditing_ = false;
};

