#include "EnglishUI.h"
#include"menuScene.h"
#include"seeScene.h"
#include"music.h"
extern Music a;
USING_NS_CC;
using namespace ui;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in seeScene.cpp\n");
}

bool seescene::init() {
	if (!Layer::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size screenSize = Director::getInstance()->getWinSize();    //获取屏幕大小

	//放背景
	auto backgroundImage = EnglishUI::sprite("see.png");
	float scaleX = screenSize.width / backgroundImage->getContentSize().width;     //获得缩放倍数
	float scaleY = screenSize.height / backgroundImage->getContentSize().height;
	if (backgroundImage == nullptr)
		problemLoading("'see.png'");
	else
	{
		backgroundImage->setScaleX(scaleX);
		backgroundImage->setScaleY(scaleY);
		backgroundImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
		this->addChild(backgroundImage, 0);
	}
	//添加返回按钮
	auto menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto returnButton = EnglishUI::menuItem("CarrotGuardRes/UI/SettingReturnNormal.png", "CarrotGuardRes/UI/SettingReturnSelected.png", CC_CALLBACK_1(seescene::to_menuScene, this));
	if (returnButton == nullptr ||
		returnButton->getContentSize().width <= 0 ||
		returnButton->getContentSize().height <= 0)
	{
		problemLoading("'SettingReturnNormal.png' and 'SettingReturnSelected.png'");
	}
	else
	{
		float x = origin.x + returnButton->getContentSize().width / 2 + visibleSize.width * 0.05f;
		float y = origin.y + visibleSize.height - returnButton->getContentSize().width / 2 - visibleSize.height * 0.04f;
		returnButton->setScale(1.5);
		returnButton->setPosition(Vec2(x, y));
		menu->addChild(returnButton);
	}
	return true;
}

void seescene::to_menuScene(Ref* pSender) {
	a.button_music();
	Director::getInstance()->popScene();
}

