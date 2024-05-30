#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Math/MathFunction.h"
#include "Astar.h"
#include <optional>
#include "Application/Src/Object/Player/Copy.h"
#include "Engine/Components/Particle/ParticleManager.h"

class Enemy : public Collider
{
public:
	static const int kActiveTime = 60 * 10;

	void Initialize(Model* model, const Vector3& position);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(Collider* collider) override;
	

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	const bool GetIsActive() const { return isActive_; };
	
	const bool GetIsGameOver() const { return isGameOver_; }

	const bool GetIsCopied() const { return isCopied_; };

	const bool GetIsResult() const { return isResult_; }

	void SetIsCopied(const bool isCopied) { isCopied_ = isCopied; };

	//プレイヤー関連
	void SetPlayerPosition(const Vector3& pos) { playerPosition_ = {(36 + pos.x)/2.0f ,(36 - pos.y)/2.0f ,pos.z}; };
	void SetCopy(Copy* copy) {copy_.push_back(copy);};
	void ClearCopy() { copy_.clear(); };
	//ブロック関連
	void SetBlockPosition(const Vector3& pos) { 
		blockPosition_[blockCount_] = {(36 + pos.x) / 2.0f,(36 - pos.y) / 2.0f,pos.z};
		blockCount_++;
	}

	void SetBlockSize(const Vector3& size) {
		blockSize_[blockSizeCount_] = size/2;
		blockSizeCount_++;

	}

	void Reset();

	void Reverse(const uint32_t stepSize);

	void SetIsTutorial(const bool isTutorial) { isTutorial_ = isTutorial; };

	void SetColor(const Vector4& color) { model_->GetMaterial(0)->SetColor(color); };

	void SetIsEdit(const bool isEdit) { isEdit_ = isEdit; };

	const bool GetIsEdit()const { return isEdit_; };

	void UpdateMatrix();

	void SaveReverseData();

	void SetIsDoubleSpeed(const bool isDoubleSpeed) { isDoubleSpeed_ = isDoubleSpeed; };

private:
	//プレイヤーの状態
	enum class Behavior
	{
		kRoot,//通常状態
		kJump,//ジャンプ中
	};

	void BehaviorRootInitialize();

	void BehaviorRootUpdate();

	void BehaviorJumpInitialize();

	void BehaviorJumpUpdate();

	//プレイヤーとの距離の計算
	void DistanceFunction();


	//経路探索関数
	void FindPath();


private:
	//オーディオ
	Audio* audio_ = nullptr;

	std::unique_ptr<Model> model_ = nullptr;


	std::unique_ptr<Model>impactScopeModel_ = nullptr;
	WorldTransform worldTransform_{};
	WorldTransform impactScopeWorldTransform_{};


	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Vector3 velocity_{};

	Vector3 enemyPosition_{};
	Vector3 blockPosition_[36] = {};
	Vector3 blockSize_[36] = {};

	bool isActive_ = true;
	bool isGameOver_ = false;

	int activeTimer_ = 0;

	bool isCopied_ = false;

	int blockCount_ = 0;
	int blockSizeCount_ = 0;

	bool blockHit_ = false;

	//ジャンプの情報
	bool jump_ = false;
	

	//逃げるフラグと時間
	bool isEscaping_ = false;

	//マップチップ
	int map[36][36] = {};
	//プレイヤー座標
	Vector3 playerPosition_{};


	//コピーの座標
	std::vector<Copy*> copy_{};
	int copyNum_;

	//経路探索
	std::vector<Node*> path_;
	std::vector<std::vector<int>> findMap;
	int moveCount_;
	Vector3 nextPosition_;
	Vector2 nextMapPosition_;
	bool search_ = false;

	//初期座標
	Vector3 startPosition_{};

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;

	//逆再生時のデータ
	std::vector<std::tuple<Vector3, uint32_t, float>> reverseData_{};

	//チュートリアルか
	bool isTutorial_ = false;

	uint32_t animationNumber_ = 0;
	uint32_t waitAnimationCoolTimer_ = 0;

	//クォータニオン
	Quaternion destinationQuaternion_{ 0.0f,0.707f,0.0f,0.707f };

	bool isEdit_ = false;

	uint32_t attackAudioHandle_ = 0;

	bool isDoubleSpeed_ = false;

	//ジャンプする方向
	int dir_ = 0;

	bool isResult_ = false;
};

