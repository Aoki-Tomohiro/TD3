#include "EnemyManager.h"
#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <map>
#include <Engine/Externals/nlohmann/json.hpp>

void EnemyManager::Initialize(Model* model, uint32_t stageNumber)
{
	//モデルの初期化
	assert(model);
	model_ = model;

	//ステージの設定
	stageNumber_ = stageNumber;

	//ファイル読み込み
	LoadFile();
}

void EnemyManager::Update()
{
	//敵の更新
	for (std::unique_ptr<Enemy>& enemy : enemies_)
	{
		if (enemy->GetIsEdit())
		{
			continue;
		}

		if (enemy->GetIsActive())
		{
			enemy->Update();
		}
	}

	//ImGui
	ImGui::Begin("EnemyManager");

	//削除対象のブロックインデックス
	int deleteIndex = -1;

	//ID
	int id = 0;

	//各ブロックの調整
	for (std::unique_ptr<Enemy>& enemy : enemies_)
	{
		//名前を設定
		std::string str = "Enemy" + std::to_string(id);

		//調整
		if (ImGui::TreeNode(str.c_str()))
		{
			//座標を取得
			Vector3 position = enemy->GetWorldPosition();

			//座標を変更
			ImGui::DragFloat3("Position", &position.x, 0.1f);

			//編集中にする
			enemy->SetIsEdit(true);

			//色を変更
			enemy->SetColor({ 1.0f,0.5f,0.0f,1.0f });

			//座標を設定
			enemy->SetPosition(position);

			//ワールド行列の更新
			enemy->UpdateMatrix();

			// 削除ボタンを追加
			if (ImGui::Button("Delete"))
			{
				deleteIndex = id; // 削除対象のインデックスを記録
			}

			ImGui::TreePop();
		}
		else
		{
			//色を変更
			enemy->SetColor({ 1.0f,0.0f,0.0f,1.0f });

			//編集してない状態に戻す
			enemy->SetIsEdit(false);
		}

		//IDをインクリメント
		id++;
	}

	// ブロックを削除
	if (deleteIndex != -1)
	{
		enemies_.erase(enemies_.begin() + deleteIndex);
	}

	//ブロックの追加
	if (ImGui::Button("AddEnemy"))
	{
		AddEnemy({ 0.0f,0.0f,0.0f });
	}

	//保存
	if (ImGui::Button("Save"))
	{
		SaveData();
	}

	ImGui::End();
}

void EnemyManager::Draw(const Camera& camera)
{
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		if (enemy->GetIsActive())
		{
			enemy->Draw(camera);
		}
	}
}

void EnemyManager::SetIsTutorial(const bool isTutorial)
{
	for (std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->SetIsTutorial(isTutorial);
	}
}

void EnemyManager::AddEnemy(const Vector3& position)
{
	Enemy* enemy = new Enemy();
	enemy->Initialize(model_, position);
	enemies_.push_back(std::unique_ptr<Enemy>(enemy));
}

void EnemyManager::SaveData()
{
	nlohmann::json root;
	//nlohmann::json型のデータコンテナにグループの全データをまとめる
	root = nlohmann::json::object();
	//jsonオブジェクトの登録
	const std::string groupName = "Stage" + std::to_string(stageNumber_);
	root[groupName] = nlohmann::json::object();

	//全てのブロックについて
	uint32_t id = 0;
	for (std::unique_ptr<Enemy>& enemy : enemies_)
	{
		//jsonオブジェクトの登録
		const std::string ItemName = "Enemy" + std::to_string(id);
		root[groupName][ItemName] = nlohmann::json::object();

		//座標
		Vector3 position = enemy->GetWorldPosition();
		root[groupName][ItemName]["Position"] = nlohmann::json::array({ position.x, position.y, position.z });

		//IDをインクリメント
		id++;
	}

	//ディレクトリがなければ作成する
	std::filesystem::path dir("Application/Resources/Editor/Enemies");
	if (!std::filesystem::exists(dir))
	{
		std::filesystem::create_directory(dir);
	}

	//書き込むJSONファイルのフルパスを合成する
	std::filesystem::path filePath = dir / (groupName + ".json");
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filePath);

	//ファイルオープン失敗？
	if (ofs.fail())
	{
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), groupName.c_str(), 0);
		assert(0);
		return;
	}

	//ファイルにjson文字列を書き込む
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
}

void EnemyManager::LoadFile()
{
	// 読み込むJSONファイルのパス
	const std::string groupName = "Stage" + std::to_string(stageNumber_);
	std::filesystem::path filePath = "Application/Resources/Editor/Enemies/" + groupName + ".json";

	// 読み込み用ファイルストリーム
	std::ifstream ifs(filePath);

	// ファイルオープン失敗
	if (ifs.fail())
	{
		// 新しいファイルを生成
		std::ofstream ofs(filePath);
		if (!ofs)
		{
			std::string message = "Failed open data file for write.";
			MessageBoxA(nullptr, message.c_str(), groupName.c_str(), 0);
			assert(0);
			return;
		}

		// 初期データを書き込む (空のJSONオブジェクトとしている)
		nlohmann::json initialData = {
			{groupName, nlohmann::json::object()}
		};
		ofs << initialData.dump(4); // 4スペースインデントでフォーマット
		ofs.close();

		// 再度ファイルを開く
		ifs.open(filePath);
		if (ifs.fail())
		{
			std::string message = "Failed to open data file for read.";
			MessageBoxA(nullptr, message.c_str(), groupName.c_str(), 0);
			assert(0);
			return;
		}
	}

	nlohmann::json root;
	// JSON文字列からJSONのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// グループを検索
	nlohmann::json::iterator itGroup = root.find(groupName);
	// 未登録チェック
	assert(itGroup != root.end());

	// 各ブロックデータの処理
	for (auto& [enemyName, enemyData] : itGroup->items())
	{
		// ブロック名が"Block"で始まることを確認
		if (enemyName.find("Enemy") == 0)
		{
			// ブロックの位置とスケールを取得
			Vector3 position = Vector3(enemyData["Position"][0], enemyData["Position"][1], enemyData["Position"][2]);

			// 敵の追加
			AddEnemy(position);
		}
	}
}

void EnemyManager::Reverse()
{
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->Reverse();
	}
}

void EnemyManager::Reset()
{
	//敵をリセット
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->Reset();
	}
}

void EnemyManager::SetBlockData(const Vector3& position, const Vector3& size)
{
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		if (enemy->GetIsEdit())
		{
			continue;
		}

		if (enemy->GetIsActive())
		{
			enemy->SetBlockPosition(position);
			enemy->SetBlockSize(size);
		}
	}
}

void EnemyManager::SetPlayerPosition(const Vector3& position)
{
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		if (enemy->GetIsEdit())
		{
			continue;
		}

		if (enemy->GetIsActive())
		{
			enemy->SetPlayerPosition(position);
		}
	}
}

void EnemyManager::SetCopy(const std::vector<std::unique_ptr<Copy>>& copies)
{
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		if (enemy->GetIsEdit())
		{
			continue;
		}

		if (enemy->GetIsActive())
		{
			enemy->ClearCopy();
			for (const std::unique_ptr<Copy>& copy : copies)
			{
				enemy->SetCopy(copy.get());
			}
		}
	}
}

void EnemyManager::SaveEnemyPositions()
{
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->SavePositions();
	}
}