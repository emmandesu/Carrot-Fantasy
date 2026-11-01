#include "EnglishUI.h"
#include"menuScene.h"
#include"setScene.h"
#include"music.h"
extern Music a;
USING_NS_CC;
using namespace ui;
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in setScene.cpp\n");
}

bool setscene::init() {
	if (!Layer::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size screenSize = Director::getInstance()->getWinSize();    //获取屏幕大小

	//放背景
	auto backgroundImage = EnglishUI::sprite("CarrotGuardRes/UI/SettingBackground.png");
	float scaleX = screenSize.width / backgroundImage->getContentSize().width;     //获得缩放倍数
	float scaleY = screenSize.height / backgroundImage->getContentSize().height;
	if (backgroundImage == nullptr)
		problemLoading("'SettingBackground.png'");
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

	auto returnButton = EnglishUI::menuItem("CarrotGuardRes/UI/SettingReturnNormal.png", "CarrotGuardRes/UI/SettingReturnSelected.png", CC_CALLBACK_1(setscene::to_menuScene, this));
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
		returnButton->setScale(1.8f * scaleX);
		returnButton->setPosition(Vec2(x, y));
		menu->addChild(returnButton);
	}

	//添加音乐按钮
	auto musicLabel_1 = Label::createWithSystemFont(EnglishUI::text("settings.audio"), "Arial", 27);
	musicLabel_1->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.75));
	musicLabel_1->setColor(Color3B::BLACK);
	this->addChild(musicLabel_1, 2);
	auto playMusicButton = EnglishUI::menuItem("CarrotGuardRes/UI/MusicOn.png", "CarrotGuardRes/UI/MusicClose.png", CC_CALLBACK_1(setscene::paly_music, this));
	if (playMusicButton == nullptr)
	{
		problemLoading("'MusicOn.png' and 'MusicClose.png'");
	}
	else
	{
		playMusicButton->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.64));
		playMusicButton->setScale(1.7f * scaleX);
		menu->addChild(playMusicButton);
		if (a.is_play()) {
			playMusicButton->setNormalImage(EnglishUI::sprite("CarrotGuardRes/UI/MusicOn.png"));
			playMusicButton->setSelectedImage(EnglishUI::sprite("CarrotGuardRes/UI/MusicOn.png"));
		}
		else {
			playMusicButton->setNormalImage(EnglishUI::sprite("CarrotGuardRes/UI/MusicClose.png"));
			playMusicButton->setSelectedImage(EnglishUI::sprite("CarrotGuardRes/UI/MusicClose.png"));
		}
	}

	// 添加作者信息
	auto authorLabel_1 = Label::createWithSystemFont(EnglishUI::text("settings.developers"), "Arial", 27);
	auto authorLabel_2 = Label::createWithSystemFont(EnglishUI::text("settings.date"), "Arial", 27);
	auto authorLabel_3 = Label::createWithSystemFont(EnglishUI::text("settings.edition"), "Arial", 27);
	authorLabel_1->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.4));
	authorLabel_2->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.3));
	authorLabel_3->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.5));
	authorLabel_1->setColor(Color3B::WHITE);
	authorLabel_2->setColor(Color3B::WHITE);
	authorLabel_3->setColor(Color3B::BLACK);
	this->addChild(authorLabel_1, 2);
	this->addChild(authorLabel_2, 2);
	this->addChild(authorLabel_3, 2);

	return true;
}
// a selector callback
void setscene::to_menuScene(Ref* pSender) {
	a.button_music();
	Director::getInstance()->popScene();
}
void setscene::paly_music(Ref* pSender) {
	a.button_music();
	MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
	//点之前没有音效
	if (!a.is_play()) {
		button->setNormalImage(EnglishUI::sprite("CarrotGuardRes/UI/soundOn.png"));
		button->setSelectedImage(EnglishUI::sprite("CarrotGuardRes/UI/soundOn.png"));
		a.set_music(1);
		a.background_music();
	}
	//点之前设置音效
	else {
		button->setNormalImage(EnglishUI::sprite("CarrotGuardRes/UI/soundClose.png"));
		button->setSelectedImage(EnglishUI::sprite("CarrotGuardRes/UI/soundClose.png"));
		a.set_music(0);
		a.background_music();
	}

}
