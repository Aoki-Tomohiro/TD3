#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/2D/Sprite.h"

class BackGround
{
public:
	enum Parts
	{
		kFrame,
		kMovie,
		kCountOfParts
	};

	void Initialize(std::vector<Model*> models);

	void Update();

	void Draw(const Camera& camera);

private:
	std::vector<Model*> models_{};

	WorldTransform worldTransforms_[kCountOfParts]{};
};

