#pragma once
#include "Engine/3D/Model/Model.h"
#include "Application/Src/Object/Player/Weapon.h"
#include <list>

class Copy
{
public:
	void Initialize(std::vector<Model*> models, const std::vector<std::variant<Vector3, bool>>& playerPositions);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

private:
	std::vector<Model*> models_{};

	WorldTransform worldTransform_{};

	std::vector<std::variant<Vector3, bool>> playerPositions_{};

	int currentIndex_ = 0;

	std::unique_ptr<Weapon> weapon_ = nullptr;
};

