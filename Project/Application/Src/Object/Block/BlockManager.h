#pragma once
#include "Application/Src/Object/Block/Block.h"
#include "Engine/Base/ImGuiManager.h"

class BlockManager
{
public:
	void Initialize(uint32_t stageNumber);

	void Update();

	void Draw(const Camera& camera);

	void AddBlock(const Vector3& position, const Vector3& scale, const bool isGround);

	const std::vector<std::unique_ptr<Block>>& GetBlocks() { return blocks_; };

private:
	void SaveData();

	void LoadFile();

private:
	std::vector<std::unique_ptr<Block>> blocks_{};

	int blockNum = 0;

	uint32_t stageNumber_ = 0;
};

