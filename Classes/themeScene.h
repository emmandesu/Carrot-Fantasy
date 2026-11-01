#pragma once
#ifndef __THEME_SCENE_H__
#define __THEME_SCENE_H__
#include "cocos2d.h"
USING_NS_CC;
class themescene : public Layer  //类似hellowordScene
{
public:
	static Scene* createScene() {
		auto scene = Scene::create();
		auto layer = themescene::create();
		scene->addChild(layer);
		return scene;
	};
	virtual bool init();
	void clearRelatedButtons();  //清除按钮
	// a selector callback
	void to_advantureScene(Ref* pSender);
	void know_Callback(Ref* pSender);
	void loadGameState();
	CREATE_FUNC(themescene);
};

#endif // __THEME_SCENE_H__
