#pragma once
#include "Application/Src/Object/Player/Copy.h"

class CopyManager
{
public:
	void Initialize(const std::vector<Model*> models);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

	const std::vector<std::unique_ptr<Copy>>& GetCopies() const { return copies_; };

	void SetPlayerPosition(const Vector3& playerPosition, const Quaternion& quaternion, const bool isAttack) { playerPositions_.push_back({ playerPosition,quaternion, isAttack }); };

private:
	std::vector<Model*> models_{};

	std::vector<std::unique_ptr<Copy>> copies_{};

	std::vector<std::tuple<Vector3, Quaternion, bool>> playerPositions_{};
};

