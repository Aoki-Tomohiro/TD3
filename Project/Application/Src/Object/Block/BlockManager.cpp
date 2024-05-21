#include "BlockManager.h"
#include "Application/Src/Scene/GamePlayScene.h"
#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <map>
#include <Engine/Externals/nlohmann/json.hpp>

void BlockManager::Initialize(Model* model)
{
	//モデルの初期化
	assert(model);
	model_ = model;

	//ファイル読み込み
	LoadFile();
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

	// 削除対象のブロックインデックス
	int deleteIndex = -1;

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
			block->SetColor({ 1.0f,0.5f,0.0f,1.0f });
			ImGui::DragFloat3("Position", &position.x, 0.1f);
			ImGui::DragFloat3("Scale", &scale.x, 0.1f);

			// 削除ボタンを追加
			if (ImGui::Button("Delete"))
			{
				deleteIndex = id; // 削除対象のインデックスを記録
			}

			ImGui::TreePop();
		}
		else
		{
			block->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		}

		//座標を設定
		block->SetPosition(position);

		//スケールを設定
		block->SetScale(scale);

		//IDをインクリメント
		id++;
	}

	// ブロックを削除
	if (deleteIndex != -1)
	{
		blocks_.erase(blocks_.begin() + deleteIndex);
	}

	//ブロックの追加
	if (ImGui::Button("AddBlock"))
	{
		AddBlock({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f });
	}

	//保存
	if (ImGui::Button("Save"))
	{
		SaveData();
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
	block->Initialize(position, scale);
	blocks_.push_back(std::unique_ptr<Block>(block));
	blockNum++;
}

void BlockManager::SaveData()
{
	nlohmann::json root;
	//nlohmann::json型のデータコンテナにグループの全データをまとめる
	root = nlohmann::json::object();
	//jsonオブジェクトの登録
	const std::string groupName = "Stage" + std::to_string(GamePlayScene::currentStageNumber);
	root[groupName] = nlohmann::json::object();

	//全てのブロックについて
	uint32_t id = 0;
	for (std::unique_ptr<Block>& block : blocks_)
	{
		//jsonオブジェクトの登録
		const std::string ItemName = "Block" + std::to_string(id);
		root[groupName][ItemName] = nlohmann::json::object();

		//座標
		Vector3 position = block->GetPosition();
		root[groupName][ItemName]["Position"] = nlohmann::json::array({ position.x, position.y, position.z });

		//スケール
		Vector3 scale = block->GetScale();
		root[groupName][ItemName]["Scale"] = nlohmann::json::array({ scale.x, scale.y, scale.z });

		//IDをインクリメント
		id++;
	}

	//ディレクトリがなければ作成する
	std::filesystem::path dir("Application/Resources/Editor/");
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

void BlockManager::LoadFile()
{
	// 読み込むJSONファイルのパス
	const std::string groupName = "Stage" + std::to_string(GamePlayScene::currentStageNumber);
	std::filesystem::path filePath = "Application/Resources/Editor/" + groupName + ".json";

	// 読み込み用ファイルストリーム
	std::ifstream ifs(filePath);

	// ファイルオープン失敗
	if (ifs.fail())
	{
		std::string message = "Failed to open data file for read.";
		MessageBoxA(nullptr, message.c_str(), groupName.c_str(), 0);
		assert(0);
		return;
	}

	nlohmann::json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// グループを検索
	nlohmann::json::iterator itGroup = root.find(groupName);
	// 未登録チェック
	assert(itGroup != root.end());

	// 各ブロックデータの処理
	for (auto& [blockName, blockData] : itGroup->items())
	{
		// ブロック名が"Block"で始まることを確認
		if (blockName.find("Block") == 0)
		{
			// ブロックの位置とスケールを取得
			Vector3 position = Vector3(blockData["Position"][0], blockData["Position"][1], blockData["Position"][2]);
			Vector3 scale = Vector3(blockData["Scale"][0], blockData["Scale"][1], blockData["Scale"][2]);

			// ブロックの追加
			AddBlock(position, scale);
		}
	}
}