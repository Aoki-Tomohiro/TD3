#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Components/Collision/CollisionManager.h"

#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Block/Block.h"
#include <queue>

class GamePlayScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
	void AddBlock(const Vector3& position, const Vector3& scale);

	void AddEnemy();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//カメラ
	Camera camera_{};

	//プレイヤー
	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;

	//敵
	std::unique_ptr<Model> enemyModel_ = nullptr;
	std::vector<std::unique_ptr<Enemy>> enemies_{};

	//ブロック
	std::unique_ptr<Model> blockModel_ = nullptr;
	std::vector<std::unique_ptr<Block>> blocks_{};

	//CollisionManager
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//プレイヤーの座標を保存するキュー
	std::queue<Vector3> playerPosition_{};
	std::vector<std::queue<Vector3>> enemyPosition_{};
	int enemyIndex_ = 0;
};

