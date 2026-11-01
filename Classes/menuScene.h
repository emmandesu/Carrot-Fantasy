#pragma once
#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__
#include "cocos2d.h"
USING_NS_CC;

class MenuScene : public Scene  //类似hellowordScene
{
public:
	static Scene* createScene() {
		return MenuScene::create();
	};
	virtual bool init();
	// a selector callback
	void menuCloseCallback(Ref* pSender);
	//去关卡选择界面
	void to_AdvantureScene(Ref* pSender);
	//去设置界面
	void to_SetScene(Ref* pSender);
	//这是观战导向，先不弄
	void to_seeScene(Ref* pSender);
	//去boss
	void to_boss(Ref* pSender);
	void know_Callback(Ref* pSender);
	// implement the "static create()" method manually
	CREATE_FUNC(MenuScene);
};

#endif // __MENU_SCENE_H__
