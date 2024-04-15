#pragma once
#include "Application/Src/Object/Player/Copy.h"

class CopyManager
{
public:
	void Initialize(const std::vector<Model*> models);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

	void SetPlayerPosition(const Vector3& playerPosition, const bool isAttack) {
		playerPositions_.push_back(playerPosition); 
		playerPositions_.push_back(isAttack);
	};

private:
	std::vector<Model*> models_{};

	std::vector<std::unique_ptr<Copy>> copies_{};

	std::vector<std::variant<Vector3, bool>> playerPositions_{};
};

