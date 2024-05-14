#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <memory>

class GrayScale
{
public:
	void Initialize();

	void Update();

	const UploadBuffer* GetConstBuffer() const { return constBuff_.get(); };

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const int32_t isEnable) { isEnable_ = isEnable; };

	const bool GetIsSepiaEnabled() const { return isSepiaEnabled_; };

	void SetIsSepiaEnabled(const int32_t isSepiaEnabled) { isSepiaEnabled_ = isSepiaEnabled; };

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	int32_t isEnable_ = false;

	int32_t isSepiaEnabled_ = false;
};

