#pragma once
#include "Application/Src/Object/Player/Copy.h"

class CopyManager
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void DrawUI();

	void Reset();

	void Reverse(const uint32_t stepSize);

	void AddCopy();

	const int GetCopyCount() const { return int(copies_.size()); };

	const std::vector<std::unique_ptr<Copy>>& GetCopies() const { return copies_; };

	void SetPlayerPosition(const Vector3& playerPosition, const bool isAttack, const uint32_t animationNumber, const float animationTime) { playerPositions_.push_back({ playerPosition, isAttack,animationNumber,animationTime }); };

	std::vector<std::tuple<Vector3, bool, uint32_t, float>>& GetPlayerPositions() { return playerPositions_; };

private:
	void ApplyGlobalVariables();

private:
	Model* model_ = nullptr;

	std::vector<std::unique_ptr<Copy>> copies_{};

	std::vector<std::tuple<Vector3, bool, uint32_t, float>> playerPositions_{};

	int copyCount_ = 0;

	int maxCopyCount_ = 5;

	//コピーの上限のスプライト
	std::array<std::unique_ptr<Sprite>, 3> maxCopySprites_{};
	Vector2 spritePositions_[3]{ {1038.0f,-7.0f},{1089.0f,3.0f},{1143.0f,-7.0f} };
	Vector2 spriteSize_[3] = { {96.0f,96.0f},{96.0f,96.0f},{96.0f,96.0f} };
};

