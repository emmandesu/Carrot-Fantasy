#pragma once
#ifndef __SET_SCENE_H__
#define __SET_SCENE_H__
#include "cocos2d.h"
USING_NS_CC;
class setscene : public Layer  //类似hellowordScene
{
public:
	static Scene* createScene() {
		auto scene = Scene::create();
		auto layer = setscene::create();
		scene->addChild(layer);
		return scene;
	};
	virtual bool init();
	// a selector callback
	void to_menuScene(Ref* pSender);
	void paly_music(Ref* pSender);

	CREATE_FUNC(setscene);
};

#endif // __SET_SCENE_H__
