#pragma once
#include "Engine/3D/Model/Model.h"
#include "Application/Src/Object/Player/Weapon.h"
#include <list>

class Copy
{
public:
	void Initialize(std::vector<Model*> models, const std::vector<std::tuple<Vector3, Quaternion, bool>>& playerPositions);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

	Weapon* GetWeapon() { return weapon_.get(); };

private:
	std::vector<Model*> models_{};

	WorldTransform worldTransform_{};

	std::vector<std::tuple<Vector3, Quaternion, bool>> playerPositions_{};

	int currentIndex_ = 0;

	std::unique_ptr<Weapon> weapon_ = nullptr;
};

