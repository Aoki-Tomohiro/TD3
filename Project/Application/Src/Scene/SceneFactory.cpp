#include "SceneFactory.h"

IScene* SceneFactory::CreateScene(const std::string& sceneName) 
{
	//次のシーンを生成
	IScene* newScene = nullptr;

	if (sceneName == "GameTitleScene") 
	{
		newScene = new GameTitleScene();
	}
	else if (sceneName == "GamePlayScene")
	{
		newScene = new GamePlayScene();
	}
	else if (sceneName == "GameClearScene") 
	{
		newScene = new GameClearScene();
	}
	else if (sceneName == "GameOverScene") 
	{
		newScene = new GameOverScene();
	}
	else if (sceneName == "LoadScene")
	{
		newScene = new LoadScene();
	}
	else if (sceneName == "TutorialScene1")
	{
		newScene = new TutorialScene1();
	}
	else if (sceneName == "TutorialScene2")
	{
		newScene = new TutorialScene2();
	}
	else if (sceneName == "StageSelectScene") 
	{
		newScene = new StageSelectScene();
	}

	return newScene;
}