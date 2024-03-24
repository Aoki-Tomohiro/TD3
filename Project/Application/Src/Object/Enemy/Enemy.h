#pragma once
#include "Engine/3D/Model/Model.h"

class Enemy
{
public:
	static const int kActiveTime = 60 * 5;

	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	const bool GetIsActive() const { return isActive_; };

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	bool isActive_ = false;
	int activeTimer_ = 0;
};

