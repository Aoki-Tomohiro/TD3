#include "GameManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"

void GameManager::Initialize()
{
	//基底クラスの初期化
	GameCore::Initialize();
	PostEffects::GetInstance()->SetIsEnable(true);
	PostEffects::GetInstance()->GetVignette()->SetIsEnable(true);
	//シーンの生成
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("GameTitleScene");

	//PostEffectsの有効化
	PostEffects* postEffects = PostEffects::GetInstance();
	postEffects->SetIsEnable(true);
	postEffects->GetBloom()->SetIsEnable(true);
	postEffects->GetBloom()->SetBlurCount(1);
}

void GameManager::Finalize()
{
	//基底クラスの終了処理
	GameCore::Finalize();
}

void GameManager::Update()
{
	//基底クラスの更新
	GameCore::Update();
}

void GameManager::Draw()
{
	//基底クラスの描画
	GameCore::Draw();
}