#include "SwitchManager.h"
#include "Engine/Base/ImGuiManager.h"
#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <map>
#include <Engine/Externals/nlohmann/json.hpp>

void SwitchManager::Initialize(uint32_t stageNumber)
{
	//ステージの設定
	stageNumber_ = stageNumber;

	//ファイル読み込み
	LoadFile();
}

void SwitchManager::Update()
{
	//ブロックの更新
	for (std::unique_ptr<Switch>& Switch : switches_)
	{
		Switch->Update();
	}

	//ImGui
	ImGui::Begin("SwitchManager");

	//削除対象のブロックインデックス
	int deleteIndex = -1;

	//ID
	int id = 0;

	//各スイッチの調整
	for (std::unique_ptr<Switch>& Switch : switches_)
	{
		//名前を設定
		std::string str = "Switch" + std::to_string(id);

		//調整
		if (ImGui::TreeNode(str.c_str()))
		{
			//座標を取得
			Vector3 switchPosition = Switch->GetSwitchPosition();

			//壁の座標を取得
			Vector3 wallPosition = Switch->GetWallPosition();

			//壁のスケールを取得
			Vector3 wallScale = Switch->GetWallScale();

			//スイッチのタイプを取得
			Switch::Type type = Switch->GetType();

			//座標とスケールを変更
			ImGui::DragFloat3("SwitchPosition", &switchPosition.x, 0.1f);
			ImGui::DragFloat3("WallPosition", &wallPosition.x, 0.1f);
			ImGui::DragFloat3("WallScale", &wallScale.x, 0.1f);
			//スイッチのタイプを選択するコンボボックス
			const char* types[] = { "Appearance", "Disappearance" };
			static int selectedItem = static_cast<int>(type);
			ImGui::Combo("Type", &selectedItem, types, IM_ARRAYSIZE(types));
			type = static_cast<Switch::Type>(selectedItem);

			//編集中にする
			Switch->SetIsEdit(true);

			//座標を設定
			Switch->SetSwitchPosition(switchPosition);

			//壁の座標を設定
			Switch->SetWallPosition(wallPosition);

			//壁のスケールを設定
			Switch->SetWallScale(wallScale);

			//スイッチのタイプを設定
			Switch->SetType(type);

			// 削除ボタンを追加
			if (ImGui::Button("Delete"))
			{
				deleteIndex = id; // 削除対象のインデックスを記録
			}

			ImGui::TreePop();
		}
		else
		{
			//編集解除
			Switch->SetIsEdit(false);
		}

		//IDをインクリメント
		id++;
	}

	// ブロックを削除
	if (deleteIndex != -1)
	{
		switches_.erase(switches_.begin() + deleteIndex);
	}

	//ブロックの追加
	if (ImGui::Button("AddSwitch"))
	{
		AddSwitch({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, Switch::Type::APPEARANCE);
	}

	//保存
	if (ImGui::Button("Save"))
	{
		SaveData();
	}

	ImGui::End();
}

void SwitchManager::Draw(const Camera& camera)
{
	//ブロックの描画
	for (std::unique_ptr<Switch>& Switch : switches_)
	{
		Switch->Draw(camera);
	}
}

void SwitchManager::AddSwitch(const Vector3& switchPosition, const Vector3& wallPosition, const Vector3& wallScale, const Switch::Type type)
{
	Switch* newSwitch = new Switch();
	newSwitch->Initialize(switchPosition, wallPosition, wallScale, type, switchCount_);
	switches_.push_back(std::unique_ptr<Switch>(newSwitch));
	switchCount_++;
}


void SwitchManager::SaveData()
{
	nlohmann::json root;
	//nlohmann::json型のデータコンテナにグループの全データをまとめる
	root = nlohmann::json::object();
	//jsonオブジェクトの登録
	const std::string groupName = "Stage" + std::to_string(stageNumber_);
	root[groupName] = nlohmann::json::object();

	//全てのブロックについて
	uint32_t id = 0;
	for (std::unique_ptr<Switch>& Switch : switches_)
	{
		//jsonオブジェクトの登録
		const std::string ItemName = "Switch" + std::to_string(id);
		root[groupName][ItemName] = nlohmann::json::object();

		//スイッチのタイプ
		Switch::Type type = Switch->GetType();
		root[groupName][ItemName]["Type"] = type;

		//スイッチの座標
		Vector3 switchPosition = Switch->GetSwitchPosition();
		root[groupName][ItemName]["SwitchPosition"] = nlohmann::json::array({ switchPosition.x, switchPosition.y, switchPosition.z });

		//壁の座標
		Vector3 wallPosition = Switch->GetWallPosition();
		root[groupName][ItemName]["WallPosition"] = nlohmann::json::array({ wallPosition.x, wallPosition.y, wallPosition.z });

		//壁のスケール
		Vector3 wallScale = Switch->GetWallScale();
		root[groupName][ItemName]["WallScale"] = nlohmann::json::array({ wallScale.x, wallScale.y, wallScale.z });

		//IDをインクリメント
		id++;
	}

	//ディレクトリがなければ作成する
	std::filesystem::path dir("Application/Resources/Editor/Switches");
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

void SwitchManager::LoadFile()
{
	// 読み込むJSONファイルのパス
	const std::string groupName = "Stage" + std::to_string(stageNumber_);
	std::filesystem::path filePath = "Application/Resources/Editor/Switches/" + groupName + ".json";

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
	for (auto& [switchName, switchData] : itGroup->items())
	{
		// ブロック名が"Block"で始まることを確認
		if (switchName.find("Switch") == 0)
		{
			// ブロックの位置とスケールを取得
			Switch::Type type = static_cast<Switch::Type>(switchData["Type"]);
			Vector3 switchPosition = Vector3(switchData["SwitchPosition"][0], switchData["SwitchPosition"][1], switchData["SwitchPosition"][2]);
			Vector3 wallPosition = Vector3(switchData["WallPosition"][0], switchData["WallPosition"][1], switchData["WallPosition"][2]);
			Vector3 wallScale = Vector3(switchData["WallScale"][0], switchData["WallScale"][1], switchData["WallScale"][2]);

			// ブロックの追加
			AddSwitch(switchPosition, wallPosition, wallScale, type);
		}
	}
}