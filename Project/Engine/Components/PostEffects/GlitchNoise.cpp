#include "GlitchNoise.h"
#include <cmath>

void GlitchNoise::Initialize()
{
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataGlitchNoise));
	Update();
}

void GlitchNoise::Update()
{
	time_ += 1.0f / 60.0f;
	time_ = std::fmod(time_, 1000.0f);
	ConstBuffDataGlitchNoise* glitchNoiseData = static_cast<ConstBuffDataGlitchNoise*>(constBuff_->Map());
	glitchNoiseData->isEnable = isEnable_;
	glitchNoiseData->blockSize = blockSize_;
	glitchNoiseData->time = time_;
	glitchNoiseData->noiseSpeed = noiseSpeed_;
	glitchNoiseData->glitchIntensity = glitchIntensity_;
	constBuff_->Unmap();
}