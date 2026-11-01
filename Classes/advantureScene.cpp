#include "EnglishUI.h"
#include"menuScene.h"
#include"advantureScene.h"
#include"themeScene.h"
#include"music.h"
#include "ui/CocosGUI.h"
bool level_is_win[3] = { false };
bool isNewGame[3] = { true,true,true };
float beishu=1.0f;
extern Music a;
USING_NS_CC;
using namespace ui;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in seeScene.cpp\n");
}
void advanturescene::clearRelatedButtons() {
	auto sprite = this->getChildByTag(100);
	// 如果找到了精灵，就从其所在父节点中移除它
	if (sprite)
		this->removeChild(sprite);
	auto know1 = this->getChildByName("know");
	if (know1)
		this->removeChildByName("know");
}

bool advanturescene::init() {
	if (!Layer::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size screenSize = Director::getInstance()->getWinSize();    //获取屏幕大小

	//添加提示词
	auto topLabel = Label::createWithSystemFont(EnglishUI::text("theme.choose"), "Arial", 32);
	topLabel->setColor(Color3B::WHITE);
	topLabel->setPosition(screenSize.width * 0.49, screenSize.height * 0.95);
	this->addChild(topLabel, 2);
	//添加背景
	auto backgroundImage = EnglishUI::sprite("CarrotGuardRes/UI/AdvantureBackground.png");
	float scaleX = screenSize.width / backgroundImage->getContentSize().width;     //获得缩放倍数
	float scaleY = screenSize.height / backgroundImage->getContentSize().height;
	if (backgroundImage == nullptr)
		problemLoading("'AdvantureBackground.png'");
	else
	{
		backgroundImage->setScaleX(scaleX);
		backgroundImage->setScaleY(scaleY);
		backgroundImage->setPosition(Vec2(screenSize.width / 2 + origin.x, screenSize.height / 2 + origin.y));
		this->addChild(backgroundImage, 0);
	}

	//添加返回按钮,回menu
	auto menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto returnButton = EnglishUI::menuItem("CarrotGuardRes/UI/SettingReturnNormal.png", "CarrotGuardRes/UI/SettingReturnSelected.png", CC_CALLBACK_1(advanturescene::to_menuScene, this));
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

	//添加左右翻页按钮
   // 创建PageView
	auto pageView = PageView::create();
	pageView->setContentSize(Size(screenSize.width, screenSize.height));
	pageView->setPosition(Vec2::ZERO);
	this->addChild(pageView);

	// 定义地图图片数组
	std::vector<std::string> mapImages = {
		"CarrotGuardRes/UI/advanture1.png",
		"CarrotGuardRes/UI/advanture2.png",
		 "CarrotGuardRes/UI/advanture3.png",
		 "CarrotGuardRes/UI/advanture4.png",
		 "CarrotGuardRes/UI/advanture5.png"
	};


	// 添加页面
	for (int i = 0; i < mapImages.size(); ++i) {
		Layout* layout = Layout::create();
		layout->setContentSize(Size(screenSize.width, screenSize.height));
		ImageView* imageView = EnglishUI::imageView(mapImages[i]);
		imageView->setContentSize(Size(screenSize.width, screenSize.height));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
		imageView->setScale(0.6);  //待定
		layout->addChild(imageView, 1);
		pageView->addPage(layout);
	}


	// 添加触摸事件监听器
	pageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			a.button_music();
			PageView* pageView = dynamic_cast<PageView*>(pSender);
			int currentIndex = pageView->getCurrentPageIndex();
			Scene* themeScene;
			Size visibleSize;
			Vec2 origin;
			Sprite* boss1;
			Menu* menu1;
			MenuItemImage* know;
			// 处理页面切换完成后的逻辑
			switch (currentIndex) {
			case 0:
				themeScene=themescene::createScene();
				Director::getInstance()->pushScene(themeScene);
				break;
			case 1:
			case 2:
			case 3:
			case 4:
				visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
				origin = Director::getInstance()->getVisibleOrigin();
				//添加锁定界面
				boss1 = EnglishUI::sprite("CarrotGuardRes/UI/un_select.png");
				boss1->setTag(100);
				if (boss1 == nullptr)
					problemLoading("'un_select.png'");
				else
				{
					boss1->setScale(0.55);
					boss1->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
					this->addChild(boss1, 2);
				}

				menu1 = Menu::create(NULL);
				menu1->setPosition(Vec2::ZERO);
				this->addChild(menu1, 3);
				//添加关闭按钮
				know = EnglishUI::menuItem("CarrotGuardRes/UI/know.png", "CarrotGuardRes/UI/know.png", CC_CALLBACK_1(advanturescene::know_Callback, this));
				if (know == nullptr ||
					know->getContentSize().width <= 0 ||
					know->getContentSize().height <= 0)
				{
					problemLoading("'know.png'");
				}
				else
				{
					float x = origin.x + visibleSize.width / 1.8;
					float y = origin.y + visibleSize.width / 3.8;
					know->setScale(1);
					know->setPosition(Vec2(x, y));
					menu1->addChild(know);  //加入到菜单中
				}
				break;
			default:
				break;
			}
		}
		});

	// 添加左右翻页按钮
	auto leftButton = EnglishUI::button("CarrotGuardRes/UI/leftButtonNormal.png", "CarrotGuardRes/UI/leftButtonSelected.png");
	leftButton->setPosition(Vec2(screenSize.width * 0.1, screenSize.height / 2));
	leftButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			a.button_music();
			int currentIndex = pageView->getCurrentPageIndex();
			CCLOG("currentIndex:  %d", currentIndex);
			if (currentIndex > 0)
				pageView->scrollToPage(currentIndex - 1);
			clearRelatedButtons();
		}
		});
	this->addChild(leftButton, 2);

	auto rightButton = EnglishUI::button("CarrotGuardRes/UI/rightButtonNormal.png", "CarrotGuardRes/UI/rightButtonSelected.png");
	rightButton->setPosition(Vec2(screenSize.width * 0.9, screenSize.height / 2));
	rightButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			a.button_music();
			int currentIndex = pageView->getCurrentPageIndex();
			if (currentIndex < 0)
				currentIndex = 0;
			CCLOG("currentIndex:  %d", currentIndex);
			if (currentIndex < mapImages.size() - 1)
				pageView->scrollToPage(currentIndex + 1);
			clearRelatedButtons();
		}
		});
	this->addChild(rightButton, 2);
	return true;
}

void advanturescene::to_menuScene(Ref* pSender) {
	a.button_music();
	Director::getInstance()->popScene();
}

void advanturescene::know_Callback(Ref* pSender) {
	auto sprite = this->getChildByTag(100);
	// 如果找到了精灵，就从其所在父节点中移除它
	if (sprite)
		this->removeChild(sprite);
	// 获取按钮所在的菜单（假设按钮的父节点是菜单，根据实际情况可能有所不同）
	auto menu = ((MenuItem*)pSender)->getParent();
	// 从菜单中移除按钮
	menu->removeChild((MenuItem*)pSender);
}
