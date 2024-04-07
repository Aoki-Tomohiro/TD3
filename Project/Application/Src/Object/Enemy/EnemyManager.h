#pragma once
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Engine/Base/ImGuiManager.h"

class EnemyManager
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void AddEnemy(const Vector3& position);

	const std::list<std::unique_ptr<Enemy>>& GetEnemies() { return enemies_; };

private:
	Model* model_ = nullptr;

	std::list<std::unique_ptr<Enemy>> enemies_{};
};

