#include "EnemyManager.h"

void EnemyManager::Initialize(Model* model)
{
	assert(model);
	model_ = model;
}

void EnemyManager::Update()
{
	//敵の削除
	enemies_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		if (!enemy->GetisAlive())
		{
			enemy.reset();
			return true;
		}
		return false;
	});

	//敵の更新
	for (std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->Update();
	}

	//ImGui
	ImGui::Begin("EnemyManager");
	//ID
	int id = 0;
	//各ブロックの調整
	for (std::unique_ptr<Enemy>& enemy : enemies_)
	{
		//座標を取得
		Vector3 position = enemy->GetPosition();

		//スケールを取得
		Vector3 scale = enemy->GetScale();

		//名前を設定
		std::string str = "Enemy" + std::to_string(id);

		//調整
		if (ImGui::TreeNode(str.c_str()))
		{
			ImGui::DragFloat3("Position", &position.x, 0.1f);
			ImGui::DragFloat3("Scale", &scale.x, 0.1f);
			ImGui::TreePop();
		}

		//座標を設定
		enemy->SetPosition(position);

		//スケールを設定
		enemy->SetScale(scale);

		//IDをインクリメント
		id++;
	}
	ImGui::End();
}

void EnemyManager::Draw(const Camera& camera)
{
	//敵の描画
	for (std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->Draw(camera);
	}
}

void EnemyManager::AddEnemy(const Vector3& position)
{
	Enemy* enemy = new Enemy();
	enemy->Initialize(model_, position);
	enemies_.push_back(std::unique_ptr<Enemy>(enemy));
}