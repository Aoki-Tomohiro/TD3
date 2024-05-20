#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/2D/Sprite.h"

class BackGround
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void DrawSprite();

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	std::unique_ptr<Sprite> sprite_ = nullptr;
};

