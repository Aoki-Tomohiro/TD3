#pragma once
#include "Application/Src/Object/Player/Copy.h"

class CopyManager
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

	void SetPlayerPosition(const Vector3& playerPosition) { playerPositions_.push_back(playerPosition); };

private:
	Model* model_ = nullptr;

	std::vector<std::unique_ptr<Copy>> copies_{};

	std::vector<Vector3> playerPositions_{};
};

