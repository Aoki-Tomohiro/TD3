#pragma once
#include "Switch.h"

class SwitchManager
{
public:
	void Initialize(uint32_t stageNumber);

	void Update();

	void Draw(const Camera& camera);

	void AddSwitch(const Vector3& switchPosition, const Vector3& wallPosition, const Vector3& wallScale, const Switch::Type type);

	const std::vector<std::unique_ptr<Switch>>& GetSwitches() { return switches_; };

private:
	void SaveData();

	void LoadFile();

private:
	std::vector<std::unique_ptr<Switch>> switches_{};

	uint32_t switchCount_ = 0;

	uint32_t stageNumber_ = 0;
};

