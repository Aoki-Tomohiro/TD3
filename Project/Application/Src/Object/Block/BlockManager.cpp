#include "BlockManager.h"

void BlockManager::Initialize(Model* model)
{
	//モデルの初期化
	assert(model);
	model_ = model;
}

void BlockManager::Update()
{
	//ブロックの更新
	for (std::unique_ptr<Block>& block : blocks_)
	{
		block->Update();
	}

	//ImGui
	ImGui::Begin("BlockManager");
	//ID
	int id = 0;
	//各ブロックの調整
	for (std::unique_ptr<Block>& block : blocks_)
	{
		//座標を取得
		Vector3 position = block->GetPosition();

		//スケールを取得
		Vector3 scale = block->GetScale();

		//名前を設定
		std::string str = "Block" + std::to_string(id);

		//調整
		if (ImGui::TreeNode(str.c_str()))
		{
			ImGui::DragFloat3("Position", &position.x, 0.1f);
			ImGui::DragFloat3("Scale", &scale.x, 0.1f);
			ImGui::TreePop();
		}

		//座標を設定
		block->SetPosition(position);

		//スケールを設定
		block->SetScale(scale);

		//IDをインクリメント
		id++;
	}
	ImGui::End();
}

void BlockManager::Draw(const Camera& camera)
{
	//ブロックの描画
	for (std::unique_ptr<Block>& block : blocks_)
	{
		block->Draw(camera);
	}
}

void BlockManager::AddBlock(const Vector3& position, const Vector3& scale)
{
	Block* block = new Block();
	block->Initialize(model_, position, scale);
	blocks_.push_back(std::unique_ptr<Block>(block));
	blockNum++;
}
