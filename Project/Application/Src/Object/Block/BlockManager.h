#pragma once
#include "Application/Src/Object/Block/Block.h"
#include "Engine/Base/ImGuiManager.h"

class BlockManager
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera& camera);

	void AddBlock(const Vector3& position, const Vector3& scale);

	const std::vector<std::unique_ptr<Block>>& GetBlocks() { return blocks_; };

private:
	Model* model_ = nullptr;

	std::vector<std::unique_ptr<Block>> blocks_{};

	int blockNum = 0;
};

