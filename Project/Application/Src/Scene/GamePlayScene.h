#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Block/BlockManager.h"
#include "Application/Src/Object/Player/CopyManager.h"

class GamePlayScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void Reset();

private:
	void AddBlock(const Vector3& position, const Vector3& scale);

	void Addcopy();
	void Addcopy2();
	void Addcopy3();
	void Addcopy4();
	void Addcopy5();

	void Updatacopy1();
	void Updatacopy2();
	void Updatacopy3();
	void Updatacopy4();
	void Updatacopy5();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//カメラ
	Camera camera_{};


	//敵
	std::unique_ptr<Model> enemyModel_ = nullptr;
	std::unique_ptr<Enemy> enemy_ = nullptr;


	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//プレイヤー
	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Model> weaponModel_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;

	//ブロック
	std::unique_ptr<Model> blockModel_ = nullptr;
	std::unique_ptr<BlockManager> blockManager_ = nullptr;

	//コピー
	std::unique_ptr<Model> copyModel_ = nullptr;
	std::unique_ptr<CopyManager> copyManager_ = nullptr;

	//コントローラーのUI
	std::unique_ptr<Sprite> contSprite_ = nullptr;
	Vector2 spritePosition_ = { 0.0f,540.0f };
	Vector2 spriteScale_ = { 360.0f,220.0f };
};

