#pragma once
#ifndef __ADVANTURE_SCENE_H__
#define __ADVANTURE_SCENE_H__
#include "cocos2d.h"
USING_NS_CC;
class advanturescene : public Layer  //类似hellowordScene
{
public:
	static Scene* createScene() {
		auto scene = Scene::create();
		auto layer = advanturescene::create();
		scene->addChild(layer);
		return scene;
	};
	virtual bool init();
	void clearRelatedButtons();
	void to_menuScene(Ref* pSender);//回到菜单
	void know_Callback(Ref* pSender);
	CREATE_FUNC(advanturescene);
};

#endif // _ADVANTURE_SCENE_H__
