#pragma once
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Wall.h"

class Switch : public Collider
{
public:
	enum class Type {
		APPEARANCE,
		DISAPPEARANCE,
	};

	void Initialize(const Vector3& switchPosition, const Vector3& wallPosition, const Vector3& wallScale, const Type type, const uint32_t id);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const override;

	const Vector3& GetSwitchPosition() const { return worldTransform_.translation_; };

	void SetSwitchPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	const Vector3& GetWallPosition() const { return wall_->GetPosition(); };

	void SetWallPosition(const Vector3& position) { wall_->SetPosition(position); };

	const Vector3& GetWallScale() const { return wall_->GetScale(); };

	void SetWallScale(const Vector3& scale) { wall_->SetScale(scale); };

	const Type GetType() const { return type_; };

	void SetType(const Type switchType) { type_ = switchType; }

	void SetIsEdit(const bool isEdit) { isEdit_ = isEdit; };

	Wall* GetWall() const { return wall_.get(); };

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	std::unique_ptr<Wall> wall_ = nullptr;

	//スイッチのタイプ
	Type type_ = Type::APPEARANCE;

	//踏まれているか
	bool isPressed_ = false;

	//編集中か
	bool isEdit_ = false;
};

