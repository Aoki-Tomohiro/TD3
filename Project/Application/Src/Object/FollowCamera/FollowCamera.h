#pragma once
#include "Engine/3D/Model/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"

class FollowCamera
{
public:
	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target) { target_ = target; };

	const Camera& GetCamera() const { return camera_; };

private:
	Camera camera_{};

	const WorldTransform* target_ = nullptr;

	Vector3 offset_{ 0.0f,0.0f,-20.0f };
};

