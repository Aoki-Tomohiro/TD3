#include "Line.h"

void Line::Initialize()
{
	lineSprite_.reset(Sprite::Create("white.png", position_));
	lineSprite_->SetSize({ 1280.0f,1.0f });
}

void Line::Update()
{
	position_.y += 720.0f / (60.0f * 5.0f);
	if (position_.y >= 720.0f)
	{
		position_.y = 0.0f;
	}
	lineSprite_->SetPosition(position_);
}

void Line::Draw()
{
	lineSprite_->Draw();
}