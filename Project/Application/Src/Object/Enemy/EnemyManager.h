#pragma once
#include "Application/Src/Object/Enemy/Enemy.h"

class Copy;

class EnemyManager
{
public:
	void Initialize(Model* model, uint32_t stageNumber);

	void Update();

	void Draw(const Camera& camera);

	void Reverse(const uint32_t stepSize);

	void Reset();

	void SetBlockData(const Vector3& position, const Vector3& size);

	void SetPlayerPosition(const Vector3& position);

	void SetCopy(const std::vector<std::unique_ptr<Copy>>& copies);

	void SaveReverseData();

	void SetIsTutorial(const bool isTutorial);

	void AddEnemy(const Vector3& position);

	const std::vector<std::unique_ptr<Enemy>>& GetEnemies() { return enemies_; };

	void SetIsDoubleSpeed(const bool isDoubleSpeed);
	
private:
	void SaveData();

	void LoadFile();

private:
	Model* model_ = nullptr;

	std::vector<std::unique_ptr<Enemy>> enemies_{};

	uint32_t stageNumber_ = 0;
};

