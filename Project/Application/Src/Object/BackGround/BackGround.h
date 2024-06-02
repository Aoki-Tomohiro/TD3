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

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void DrawSprite();

	void SetPlayerPosition(const Vector3& playerPosition) { playerPosition_ = playerPosition; };

private:
	static const int kMaxBackGround = 4;

	std::array<std::unique_ptr<Sprite>, kMaxBackGround> backGroundSprite_{};

	std::array<Vector2, kMaxBackGround> backGroundSpritePosition_{};

	Vector3 playerPosition_{};

	WorldTransform worldTransforms_[kCountOfParts]{};
};

