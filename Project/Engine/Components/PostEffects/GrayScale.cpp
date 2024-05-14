#include "GrayScale.h"

void GrayScale::Initialize()
{
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataGrayScale));
	Update();
}

void GrayScale::Update()
{
	ConstBuffDataGrayScale* grayScaleData = static_cast<ConstBuffDataGrayScale*>(constBuff_->Map());
	grayScaleData->isEnable = isEnable_;
	grayScaleData->isSepiaEnabled = isSepiaEnabled_;
	constBuff_->Unmap();
}