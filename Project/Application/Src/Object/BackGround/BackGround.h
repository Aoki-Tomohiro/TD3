#pragma once
#include "Engine/3D/Model/Model.h"

class BackGround
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};
};

