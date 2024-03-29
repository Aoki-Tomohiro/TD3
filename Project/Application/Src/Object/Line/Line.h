#pragma once
#include "Engine/2D/Sprite.h"
#include "Engine/Math/MathFunction.h"

class Line
{
public:
	void Initialize();

	void Update();

	void Draw();

private:
	std::unique_ptr<Sprite> lineSprite_ = nullptr;

	Vector2 position_{};
};

