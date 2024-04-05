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
#include "Application/Src/Object/Copy/Copy.h"
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

	//プレイヤー
	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;

	//コピー
	std::unique_ptr<Model> copyModel_ = nullptr;
	std::vector<std::unique_ptr<Copy>> copys_{};
	std::vector<std::unique_ptr<Copy>> copys2_{};
	std::vector<std::unique_ptr<Copy>> copys3_{};
	std::vector<std::unique_ptr<Copy>> copys4_{};
	std::vector<std::unique_ptr<Copy>> copys5_{};

	//敵
	std::unique_ptr<Model> enemyModel_ = nullptr;
	std::unique_ptr<Enemy> enemy_ = nullptr;

	//ブロック
	std::unique_ptr<Model> blockModel_ = nullptr;
	std::vector<std::unique_ptr<Block>> blocks_{};

	//CollisionManager
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//プレイヤーの座標を保存するキュー
	std::queue<Vector3> playerPosition_{};
	std::queue<Vector3> playerPosition2_{};
	std::queue<Vector3> playerPosition3_{};
	std::queue<Vector3> playerPosition4_{};
	std::queue<Vector3> playerPosition5_{};
	std::vector<std::queue<Vector3>> copyPosition_{};
	std::vector<std::queue<Vector3>> copyPosition2_{};
	std::vector<std::queue<Vector3>> copyPosition3_{};
	std::vector<std::queue<Vector3>> copyPosition4_{};
	std::vector<std::queue<Vector3>> copyPosition5_{};
	int enemyIndex_ = 0;

	//新しいコピーが出てくるまでの時間
	int popTimer_;

	bool popflag1_=false;
	bool popflag2_=false;
	bool popflag3_ = false;
	bool popflag4_ = false;
};

