#pragma once
#include "Engine/3D/Model/Model.h"

class Copy
{
public:
	static const int kActiveTime = 60 * 10;

	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	const bool GetIsActive() const { return isActive_; };

	const Vector3 GetWorldPosition() const;

	const bool GetIsCopied() const { return isCopied_; };

	void SetIsCopied(const bool isCopied) { isCopied_ = isCopied; };


private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	bool isActive_ = false;

	int activeTimer_ = 0;

	bool isCopied_ = false;
};


