#include "Config.h"
#include "CTestScene.h"
#include "Utils/CharacterConver.h"
#include "../GameManager.h"
#include "../GameResources.h"
namespace View
{
	void CTestScene::InitScene()
	{
		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

		CCLayer *testLayer = CCLayer::create();
	
		CCSprite *pbgSprite = CCSprite::create(s_pPathBackGround1);

		CCLabelTTF* label = CCLabelTTF::create("first scene", "Arial", 20);
		//#endif
		CCMenuItemLabel* pMenuItem = CCMenuItemLabel::create(label, this, menu_selector(CTestScene::OnTextmenu_Click));


		 CCMenu* pMenu =CCMenu::create(pMenuItem, NULL);

		 pbgSprite->setPosition(ccp(origin.x + visibleSize.width/2 , origin.y + visibleSize.height/2));
		 pMenu->setPosition(ccp(origin.x + visibleSize.width/2 , origin.y + visibleSize.height/2));

		testLayer->addChild(pbgSprite);
		testLayer->addChild(pMenu);
		testLayer->scheduleOnce(schedule_selector(CTestScene::ShowOtherScene), 3.0f);

		this->addChild(testLayer);

		testLayer->setTouchEnabled(true);
	}

	void CTestScene::runThisScene( )
	{
		//show some effect

		InitScene();

		if (!CCDirector::sharedDirector()->getRunningScene())
		{
			CCDirector::sharedDirector()->runWithScene(this);
		}
		else
		{
			CCDirector::sharedDirector()->replaceScene(this);
		}
	}

	void CTestScene::OnTextmenu_Click(CCObject *pSender)
	{
		printf("nothing");

		_gamemanager::instance()->DisplayNowScene(SCENE_MENU);
	}

	void CTestScene::ShowOtherScene(float t)
	{
	    _gamemanager::instance()->DisplayNowScene(SCENE_MENU);
		printf("do nothing");
	}
}