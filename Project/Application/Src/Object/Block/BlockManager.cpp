#include "BlockManager.h"
#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <map>
#include <Engine/Externals/nlohmann/json.hpp>
#include <Engine/Base/TextureManager.h>

void BlockManager::Initialize(uint32_t stageNumber)
{
	//ステージの設定
	stageNumber_ = stageNumber;

	//地面の追加
	AddBlock({ 0.0f,-16.0f,0.0f }, { 1.0f,1.0f,1.0f }, true);

	//ファイル読み込み
	LoadFile();

	TextureManager::Load("aoo.png");
	aoasiModel_ = ModelManager::CreateFromModelFile("aoasi.gltf", "Block", Opaque);
	aoasiModel_->GetMaterial(0)->SetTexture("aoo.png");
	aoasiModel_->GetMaterial(0)->SetColor({ -0.5f,1.0f,1.0f,1.0f });
	aoasiModel_->GetMaterial(0)->SetUVScale({ 5.0f,1.0f });
	worldTransForm.Initialize();
	worldTransForm.translation_ = {0.0f,-10.85f,-1.3f};
	
}

void BlockManager::Update()
{
	
	worldTransForm.UpdateMatrixFromEuler();

	//ブロックの更新
	for (std::unique_ptr<Block>& block : blocks_)
	{
		block->Update();
	}

	//ImGui
	ImGui::Begin("BlockManager");

	//削除対象のブロックインデックス
	int deleteIndex = -1;

	//ID
	int id = 0;

	//各ブロックの調整
	for (std::unique_ptr<Block>& block : blocks_)
	{
		//名前を設定
		std::string str = "Block" + std::to_string(id);

		//調整
		if (ImGui::TreeNode(str.c_str()))
		{
			//座標を取得
			Vector3 position = block->GetPosition();

			//スケールを取得
			Vector3 scale = block->GetScale();

			//座標とスケールを変更
			ImGui::DragFloat3("Position", &position.x, 0.1f);
			ImGui::DragFloat3("Scale", &scale.x, 0.1f);

			//色を変更
			block->SetColor({ 1.0f,0.5f,0.0f,1.0f });

			//座標を設定
			block->SetPosition(position);

			//スケールを設定
			block->SetScale(scale);

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
			/*block->SetColor({ 0.196f,0.196f,0.196f,1.0f });*/
			/*block->SetColor({ 0.0f, 0.1059f, 0.4039f, 1.0f });*/
			block->SetColor({ 0.0f, 0.047f, 0.196f, 1.0f });
		}

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
		AddBlock({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, false);
	}

	//保存
	if (ImGui::Button("Save"))
	{
		SaveData();
	}

	ImGui::DragFloat3("transform", &worldTransForm.translation_.x);
	ImGui::DragFloat3("rotation", &worldTransForm.rotation_.x);
	ImGui::DragFloat3("size", &worldTransForm.scale_.x);
	ImGui::DragFloat4("Color", &color_.x);
	ImGui::End();
}

void BlockManager::Draw(const Camera& camera)
{
	//ブロックの描画
	for (std::unique_ptr<Block>& block : blocks_)
	{
		block->Draw(camera);
	}

	aoasiModel_->Draw(worldTransForm, camera);
}

void BlockManager::AddBlock(const Vector3& position, const Vector3& scale, const bool isGround)
{
	Block* block = new Block();
	block->Initialize(position, scale, blockNum, isGround);
	blocks_.push_back(std::unique_ptr<Block>(block));
	blockNum++;
}

void BlockManager::SaveData()
{
	nlohmann::json root;
	//nlohmann::json型のデータコンテナにグループの全データをまとめる
	root = nlohmann::json::object();
	//jsonオブジェクトの登録
	const std::string groupName = "Stage" + std::to_string(stageNumber_);
	root[groupName] = nlohmann::json::object();

	//全てのブロックについて
	uint32_t id = 0;
	for (std::unique_ptr<Block>& block : blocks_)
	{
		//最初のブロックは飛ばす
		if (id == 0)
		{
			id++;
			continue;
		}

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
	std::filesystem::path dir("Application/Resources/Editor/Blocks");
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
	const std::string groupName = "Stage" + std::to_string(stageNumber_);
	std::filesystem::path filePath = "Application/Resources/Editor/Blocks/" + groupName + ".json";

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
	for (auto& [blockName, blockData] : itGroup->items())
	{
		// ブロック名が"Block"で始まることを確認
		if (blockName.find("Block") == 0)
		{
			// ブロックの位置とスケールを取得
			Vector3 position = Vector3(blockData["Position"][0], blockData["Position"][1], blockData["Position"][2]);
			Vector3 scale = Vector3(blockData["Scale"][0], blockData["Scale"][1], blockData["Scale"][2]);

			// ブロックの追加
			AddBlock(position, scale, false);
		}
	}
}