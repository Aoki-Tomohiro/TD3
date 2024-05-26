#pragma once
#include "Engine/3D/Model/ModelManager.h"
#include "Application/Src/Object/Player/Weapon.h"
#include <list>

class Copy : public Collider
{
public:
	void Initialize(const std::vector<std::tuple<Vector3, bool, uint32_t, float>>& playerPositions);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const Vector3 GetWorldPosition() const;

	Weapon* GetWeapon() { return weapon_.get(); };

	void Reverse(const uint32_t stepSize);

private:
	std::unique_ptr<Model> model_ = nullptr;
	//std::unique_ptr<Model>impactScopeModel_ = nullptr;
	WorldTransform worldTransform_{};
	//WorldTransform impactScopeWorldTransform_{};
	Quaternion destinationQuaternion_{ 0.0f,0.707f,0.0f,0.707f };

	Vector3 velocity_{};

	float gravity_ = 0.05f;

	std::vector<std::tuple<Vector3, bool, uint32_t, float>> playerPositions_{};

	int currentIndex_ = 0;

	std::unique_ptr<Model> weaponModel_ = nullptr;

	std::unique_ptr<Weapon> weapon_ = nullptr;

	bool isReverse_ = false;
};

