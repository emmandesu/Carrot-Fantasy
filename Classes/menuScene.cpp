#include "EnglishUI.h"
#include"menuScene.h"
#include"setScene.h"
#include"advantureScene.h"
#include"seeScene.h"
#include"music.h"
using namespace ui;
extern Music a;
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in menuScene.cpp\n");
}

bool MenuScene::init() {
    if (!Scene::init())
        return false;
    auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //获取屏幕大小

	//放背景
	auto backgroundImage = EnglishUI::sprite("CarrotGuardRes/UI/BasicBackground.png");
	float scaleX = screenSize.width / backgroundImage->getContentSize().width;     //获得缩放倍数
	float scaleY = screenSize.height / backgroundImage->getContentSize().height;
	if (backgroundImage == nullptr)
		problemLoading("'BasicBackground.png'");
	else
	{
		backgroundImage->setScaleX(scaleX);
		backgroundImage->setScaleY(scaleY);
		backgroundImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
		this->addChild(backgroundImage, 0);
	}

	//添加背景左叶子图片
	auto leaf1 = EnglishUI::sprite("CarrotGuardRes/UI/LeafBackground_1.png");
	if (leaf1 == nullptr)
		problemLoading("'LeafBackground_1.png'");
	else
	{
		leaf1->setScale(1.35f * scaleX);
		leaf1->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.06f, visibleSize.height / 2 + origin.y + visibleSize.height * 0.18f));
		this->addChild(leaf1, 0);
	}

	//添加背景中间叶子图片
	auto leaf2 = EnglishUI::sprite("CarrotGuardRes/UI/LeafBackground_2.png");
	if (leaf2 == nullptr)
	{
		problemLoading("'LeafBackground_2.png'");
	}
	else
	{
		leaf2->setScale(1.5f * scaleX);
		leaf2->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + visibleSize.height * 0.20f));
		this->addChild(leaf2, 0);
	}

	//添加背景右叶子图片
	auto leaf3 = EnglishUI::sprite("CarrotGuardRes/UI/LeafBackground_3.png");
	if (leaf3 == nullptr)
	{
		problemLoading("'LeafBackground_3.png'");
	}
	else
	{
		leaf3->setScale(scaleX);
		leaf3->setPosition(Vec2(visibleSize.width / 2 + origin.x + visibleSize.width * 0.075f, visibleSize.height / 2 + origin.y + visibleSize.height * 0.18f));
		this->addChild(leaf3, 0);
	}

	//添加背景萝卜图片
	auto carrot = EnglishUI::sprite("CarrotGuardRes/UI/CarrotBackground.png");
	if (carrot == nullptr)
		problemLoading("'CarrotBackground.png'");
	else
	{
		carrot->setScale(1.5f * scaleX);
		carrot->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.005f, visibleSize.height / 2 + origin.y + visibleSize.height * 0.06f));
		this->addChild(carrot, 0);
	}

	//添加背景名称图片
	auto NameImage = EnglishUI::sprite("CarrotGuardRes/UI/NameBackground.png");
	if (NameImage == nullptr)
		problemLoading("'NameBackground.png'");
	else
	{
		NameImage->setScale(1.6f * scaleX);
		NameImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - visibleSize.height * 0.055f));
		this->addChild(NameImage, 0);
	}

	// create menu, it's an autorelease object,添加菜单
	auto menu = Menu::create(NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);



	//添加关闭按钮
	auto close = EnglishUI::menuItem("CarrotGuardRes/UI/CloseNormal.png", "CarrotGuardRes/UI/CloseSelected.png", CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
	if (close == nullptr ||
		close->getContentSize().width <= 0 ||
		close->getContentSize().height <= 0)
	{
		problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
	}
	else
	{
		float x = origin.x + visibleSize.width - close->getContentSize().width / 2;
		float y = origin.y + close->getContentSize().height / 2;
		close->setScale(scaleX);
		close->setPosition(Vec2(x, y));
		menu->addChild(close);  //加入到菜单中

	}


	//添加冒险模式按钮
	auto advanture = EnglishUI::menuItem("CarrotGuardRes/UI/AdvantureButtonNormal.png", "CarrotGuardRes/UI/AdvantureButtonSelected.png", CC_CALLBACK_1(MenuScene::to_AdvantureScene, this));
	if (advanture == nullptr ||
		advanture->getContentSize().width <= 0 ||
		advanture->getContentSize().height <= 0)
	{
		problemLoading("'AdvantureButtonNormal.png' and 'AdvantureButtonSelected.png'");
	}
	else
	{
		float x = visibleSize.width * 0.2;
		float y = visibleSize.height * 0.15;
		advanture->setScale(1.5f * scaleX);
		advanture->setPosition(Vec2(x, y));
		menu->addChild(advanture);
	}

	//添加boss模式按钮
	auto boss = EnglishUI::menuItem("CarrotGuardRes/UI/BossButtonNormal.PNG", "CarrotGuardRes/UI/BossButtonSelected.png", CC_CALLBACK_1(MenuScene::to_boss, this));
	if (boss == nullptr ||
		boss->getContentSize().width <= 0 ||
		boss->getContentSize().height <= 0)
	{
		problemLoading("'CarrotGuardRes/UI/BossButtonNormal.png'and 'CarrotGuardRes/UI/BossButtonSelected.png'");
	}
	else
	{
		float x = visibleSize.width * 0.5;
		float y = visibleSize.height * 0.14;
		boss->setScale(scaleX);
		boss->setPosition(Vec2(x, y));
		menu->addChild(boss);
	}

	//添加观战模式按钮
	auto see = EnglishUI::menuItem("CarrotGuardRes/UI/WitnessButtonNormal.png", "CarrotGuardRes/UI/WitnessButtonSelected.png", CC_CALLBACK_1(MenuScene::to_seeScene, this));
	if (see == nullptr ||
		see->getContentSize().width <= 0 ||
		see->getContentSize().height <= 0)
	{
		problemLoading("'CarrotGuardRes/UI/WitnessButtonNormal.png'and 'CarrotGuardRes/UI/WitnessButtonSelected.png'");
	}
	else
	{
		float x = visibleSize.width * 0.8;
		float y = visibleSize.height * 0.15;
		see->setScale(1.5 * scaleX);
		see->setPosition(Vec2(x, y));
		menu->addChild(see);
	}

	//设置界面按钮
	auto set = EnglishUI::menuItem("CarrotGuardRes/UI/SettingNormal.png", "CarrotGuardRes/UI/SettingSelected.png", CC_CALLBACK_1(MenuScene::to_SetScene, this));
	if (set == nullptr||
		set->getContentSize().width <= 0 ||
		set->getContentSize().height <= 0)
	{
		problemLoading("'SettingNormal.png' and 'SettingSelected.png'");
	}
	else
	{
		set->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.3f, visibleSize.height / 2 + origin.y - visibleSize.height * 0.135f));
		set->setScale(1.7f * scaleX);
		menu->addChild(set);
	}
	return true;
}

// a selector callback
void MenuScene::menuCloseCallback(Ref* pSender) {
	a.button_music();
	Director::getInstance()->end();
}

//去关卡选择界面
void MenuScene::to_AdvantureScene(Ref* pSender) {
	a.button_music();
	auto advantureScene = advanturescene::createScene();
	Director::getInstance()->pushScene(advantureScene);
}

//去设置界面
void MenuScene::to_SetScene(Ref* pSender) {
	a.button_music();
	auto settingScene = setscene::createScene();
	Director::getInstance()->pushScene(settingScene);
}

//这是观战导向，先不弄
void MenuScene::to_seeScene(Ref* pSender) {
	a.button_music();
	auto witnessScene = seescene::createScene();
	Director::getInstance()->pushScene(witnessScene);
}
void MenuScene::to_boss(Ref* pSender) {
	a.button_music();
	auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//添加boss界面
	auto boss1 = EnglishUI::sprite("CarrotGuardRes/UI/un_select1.png");
	boss1->setTag(100);
	if (boss1 == nullptr)
		problemLoading("'un_select1.png'");
	else
	{
		boss1->setScale(0.8);
		boss1->setPosition(Vec2(visibleSize.width / 2 , visibleSize.height / 2 ));
		this->addChild(boss1,2);
	}
	auto menu1 = Menu::create(NULL);
	menu1->setPosition(Vec2::ZERO);
	this->addChild(menu1, 3);
	auto know = EnglishUI::menuItem("CarrotGuardRes/UI/know.png", "CarrotGuardRes/UI/know.png", CC_CALLBACK_1(MenuScene::know_Callback, this));
	if (know == nullptr ||
		know->getContentSize().width <= 0 ||
		know->getContentSize().height <= 0)
	{
		problemLoading("'know.png'");
	}
	else
	{
		float x = origin.x + visibleSize.width/1.8 ;
		float y = origin.y + visibleSize.width / 3.8;
		know->setScale(1);
		know->setPosition(Vec2(x, y));
		menu1->addChild(know);  //加入到菜单中
	}
}

void MenuScene::know_Callback(Ref* pSender) {
	auto sprite = this->getChildByTag(100);
	// 如果找到了精灵，就从其所在父节点中移除它
	if (sprite)
		this->removeChild(sprite);
	auto menu = ((MenuItem*)pSender)->getParent();
	// 从菜单中移除按钮
	menu->removeChild((MenuItem*)pSender);
}
