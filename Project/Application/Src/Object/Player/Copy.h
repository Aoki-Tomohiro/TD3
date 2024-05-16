#pragma once
#include "Engine/3D/Model/ModelManager.h"
#include "Application/Src/Object/Player/Weapon.h"
#include <list>

class Copy : public Collider
{
public:
	void Initialize(Model* model, const std::vector<std::tuple<Vector3, Quaternion, bool, uint32_t, float>>& playerPositions);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const;

	Weapon* GetWeapon() { return weapon_.get(); };

	void Reverse();

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	Vector3 velocity_{};

	float gravity_ = 0.05f;

	std::vector<std::tuple<Vector3, Quaternion, bool, uint32_t, float>> playerPositions_{};

	int currentIndex_ = 0;

	std::unique_ptr<Model> weaponModel_ = nullptr;

	std::unique_ptr<Weapon> weapon_ = nullptr;
};

