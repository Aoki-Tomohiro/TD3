#pragma once
#include "Engine/2D/Sprite.h"

class Player;
class Copy;

class Score
{
public:
	void Initialize();

	void Update(Player* player, const std::vector<std::unique_ptr<Copy>>& copies);

	void Draw();

	void Reset() { score_ = 0; };

private:
	std::array<std::unique_ptr<Sprite>, 5> scoreSprites_{};

	std::array<Vector2, 5> position_{};

	std::array<Vector2, 5> scale_{};

	int score_ = 0;
};

