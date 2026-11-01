#pragma once
#ifndef __SEE_MENU_H__
#define __SEE_MENU_H__
#include "cocos2d.h"
USING_NS_CC;
class seescene : public Layer  //类似hellowordScene
{
public:
	static Scene* createScene() {
		auto scene = Scene::create();
		auto layer = seescene::create();
		scene->addChild(layer);
		return scene;
	};
	virtual bool init();
	void to_menuScene(Ref* pSender);
	// a selector callback
	CREATE_FUNC(seescene);
};

#endif // __SEE_MENU_H__
