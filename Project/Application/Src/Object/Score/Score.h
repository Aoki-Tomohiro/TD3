#pragma once
#include "Engine/2D/Sprite.h"

class Player;
class Copy;

class Score
{
public:
	void Initialize();

	void Update(Player* player, const std::list<std::unique_ptr<Copy>>& copies);

	void Draw();

	void Reset() { score_ = 0; };

	const int GetScore() const { return score_; };

private:
	std::array<std::unique_ptr<Sprite>, 5> scoreSprites_{};

	std::array<Vector2, 5> position_{};

	std::array<Vector2, 5> scale_{};

	int score_ = 0;

	std::unique_ptr<Sprite> scoreRearSprite_ = nullptr;

	Vector2 rearPos_ = { 25.5f,61.0f };
	Vector2 rearSize_ = { 0.85f,0.8f };
};

