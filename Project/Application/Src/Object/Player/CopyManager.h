#pragma once
#include "Application/Src/Object/Player/Copy.h"

class CopyManager
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void Reset();

	void Reverse();

	void AddCopy();

	const int GetCopyCount() const { return int(copies_.size()); };

	const std::vector<std::unique_ptr<Copy>>& GetCopies() const { return copies_; };

	void SetPlayerPosition(const Vector3& playerPosition, const Quaternion& quaternion, const bool isAttack, const uint32_t animationNumber, const float animationTime) { playerPositions_.push_back({ playerPosition,quaternion, isAttack,animationNumber,animationTime }); };

	std::vector<std::tuple<Vector3, Quaternion, bool, uint32_t, float>>& GetPlayerPositions() { return playerPositions_; };

private:
	Model* model_ = nullptr;

	std::vector<std::unique_ptr<Copy>> copies_{};

	std::vector<std::tuple<Vector3, Quaternion, bool, uint32_t, float>> playerPositions_{};
};

