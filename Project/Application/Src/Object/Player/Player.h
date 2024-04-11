#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Player/Weapon.h"
#include <optional>

class Player : public Collider
{
public:
	void Initialzie(std::vector<Model*> models);

	void Update();

	void Draw(const Camera& camera);

	void DrawUI(const Camera& camera);

	void Reset();

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const;

	Weapon* GetWeapon() { return weapon_.get(); };

	const bool GetIsStop() const { return isStop_; };

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

private:
	//インプット
	Input* input_ = nullptr;

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
	float jumpFirstSpeed_ = 0.8f;

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
};

