#pragma once
#include "Engine/3D/Model/Model.h"
#include <list>

class Copy
{
public:
	void Initialize(Model* model, const std::vector<Vector3>& playerPositions);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	std::vector<Vector3> playerPositions_{};

	int currentIndex_ = 0;
};

