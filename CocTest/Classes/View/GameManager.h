#ifndef _GMEEMANAGER_H_
#define _GMEEMANAGER_H_
#include "../Utils/Singleton.h"
#include "Scene/CBaseScene.h"
#include "Scene/CTestScene.h"
#include "Scene/CMenuScene.h"

using namespace Utils;
namespace View
{
	enum 
	{
		SCENE_INIT = 1,
		SCENE_LOGIN = 2,
		SCENE_MENU = 3,
	};

	class GameManager
	{
	public:
		void DisplayNowScene(int sID);
	private:
		GameManager() {}
		~GameManager() {}
	private:
		std::map<UInt8, CBaseScene*> _allScenesMap;
		//CBaseScene *actScene;

		DECLARE_SINGLETON_CLASS(GameManager);
	};
	typedef Singleton<GameManager> _gamemanager;
}
#endif