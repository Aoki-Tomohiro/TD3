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

	const std::list<std::unique_ptr<Copy>>& GetCopies() const { return copies_; };

	void SetPlayerData(const Vector3& playerPosition, const bool isAttack, const uint32_t animationNumber, const float animationTime) { playerPositions_.push_back({ playerPosition, isAttack,animationNumber,animationTime }); };

	std::vector<std::tuple<Vector3, bool, uint32_t, float>>& GetPlayerPositions() { return playerPositions_; };

	void SetIsDoubleSpeed(const bool isDoubleSpeed);

private:
	void ApplyGlobalVariables();

private:
	Model* model_ = nullptr;

	std::list<std::unique_ptr<Copy>> copies_{};

	std::vector<std::tuple<Vector3, bool, uint32_t, float>> playerPositions_{};

	int copyCount_ = 0;

	int maxCopyCount_ = 5;

	std::vector<Vector4> copyColors_{};

	//コピーの上限のスプライト
	std::array<std::unique_ptr<Sprite>, 3> maxCopySprites_{};
	Vector2 spritePositions_[3]{ {1012.0f,30.0f},{1090.0f,30.0f},{1140.0f,30.0f} };
	Vector2 spriteScale_[3] = { {0.8f,0.8f},{0.8f,0.8f},{0.8f,0.8f} };
};

