#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/2D/Sprite.h"

class BackGround
{
public:
	enum Parts
	{
		kGenko,
		kFrame,
		kMovie,
		kCountOfParts
	};

	void Initialize(std::vector<Model*> models);

	void Update();

	void Draw(const Camera& camera);

	void DrawSprite();

private:
	std::vector<Model*> models_{};

	std::unique_ptr<Sprite> sprite_ = nullptr;

	Vector2 spritePosition_{ 0.0f,-112.0f };

	Vector2 spriteScale_{1.0f,1.0f};

	WorldTransform worldTransforms_[kCountOfParts]{};
};

