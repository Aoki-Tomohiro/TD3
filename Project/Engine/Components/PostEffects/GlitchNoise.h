#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <chrono>

class GlitchNoise
{
public:
	void Initialize();

	void Update();

	const UploadBuffer* GetConstBuffer() const { return constBuff_.get(); };

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	const float GetBlockSize() const { return blockSize_; };

	void SetBlockSize(const float blockSize) { blockSize_ = blockSize; };

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	bool isEnable_ = false;

	float blockSize_ = 1.0f;

	float time_ = 0.0f;

	float noiseSpeed_ = 10.0f;

	float glitchIntensity_ = 0.1f;
};

