#include "SaveIO.h"
#include <set>
#include "EnglishUI.h"
#include "BaseLevelScene.h"
#include<vector>
#include"MonsterConfigs.h"
#include"Monster.h"
#include "json/document.h"
#include "json/rapidjson.h"
#include "ui/CocosGUI.h"
#include"music.h"
#include"themeScene.h"
#include"Tower.h"
#include"GameManager.h"
#include<string>
#include "json/writer.h"
#include"obpos.h"
#include "json/stringbuffer.h"
#include<fstream>
using namespace rapidjson;
using namespace ui;
//#define DEBUG_MODE
#define CELL_SIZE 64
const Color3B moneyColor(154, 101, 25);

extern float beishu;           //游戏倍数
extern Music a;                //音乐系统
extern bool level_is_win[3];   //通关情况
extern bool isNewGame[3];      //存档情况
GameManager* manager;          //用来管理各部分交互
float tower_jiasu = 1;         //用于塔加速
// 关卡地图文件路径数组
const std::vector<std::string> BaseLevelScene::mapFiles
= {
    "map/map1.tmx",  // 关卡 1
    "map/map2.tmx",  // 关卡 2
    "map/map3.tmx",  // 关卡 3
};


static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BaseLevelScene.cpp\n");
}
/****************标签部分******************/
//更新金币标签
void BaseLevelScene::updateMoney(int add) {
    money += add;
    std::string text = std::to_string(money); // 将数字转换为字符串
    moneyLable->setString(text);
}
//更新当前波数标签
void BaseLevelScene::updateCurrentWaveLabe() {
    _curNumberLabel->setString(StringUtils::format("%d", std::min(manager->getCurrentWaveNum() + 1, manager->getAllWaveNum())));
}
/******************************************/

/****************UI部分******************/
//二倍速按钮
void BaseLevelScene::doublespeed(Ref* pSender) {
    isDoubleSpeed = !isDoubleSpeed; // 切换二倍速状态
    a.button_music();
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isDoubleSpeed) {
        button->setNormalImage(EnglishUI::sprite("CarrotGuardRes/UI/doubleSpeed.png"));
        button->setSelectedImage(EnglishUI::sprite("CarrotGuardRes/UI/doubleSpeed.png"));
        tower_jiasu = 2;
        guaisou_jiansu(2.0f);
        beishu = 2;//实现加速效果
    }
    else {
        button->setNormalImage(EnglishUI::sprite("CarrotGuardRes/UI/normalSpeed.png"));
        button->setSelectedImage(EnglishUI::sprite("CarrotGuardRes/UI/normalSpeed.png"));
        tower_jiasu = 1;
        guaisou_jiansu(1.0f);
        beishu = 1; //实现减速效果
    }
}
//暂停按钮
void BaseLevelScene::pause_all(Ref* pSender) {
    isPaused = !isPaused; // 切换暂停状态
    a.button_music();
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isPaused) {//暂停状态
        button->setNormalImage(EnglishUI::sprite("CarrotGuardRes/UI/continueButton.PNG"));
        button->setSelectedImage(EnglishUI::sprite("CarrotGuardRes/UI/continueButton.PNG"));
        // 添加顶部暂停标识
        auto pauseTop = EnglishUI::sprite("CarrotGuardRes/UI/pausing.png");
        pauseTop->setName("pauseTop");
        pauseTop->setPosition(464, 610);
        pauseTop->setScale(2.0f);
        this->addChild(pauseTop, 10);
        Director::getInstance()->pause();
    }
    else {//恢复
        button->setNormalImage(EnglishUI::sprite("CarrotGuardRes/UI/pauseButton.PNG"));
        button->setSelectedImage(EnglishUI::sprite("CarrotGuardRes/UI/pauseButton.PNG"));
        //移除暂停标识
        this->removeChildByName("pauseTop");
        Director::getInstance()->resume();
    }
}
void BaseLevelScene::wenhao(Ref* pSender) {
    a.button_music();
    auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //获取屏幕大小
    //创建灰色遮罩层
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    this->addChild(menuLayer, 10);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [menuLayer](Touch* touch, Event* event) {
        return true;
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, menuLayer);
    auto jie = EnglishUI::sprite("Carrot/jieshao/jie_0.png");
    jie->setPosition(480, 500);
    jie->setScale(2);
    menuLayer->addChild(jie, 1);
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    //添加左右翻页按钮
   // 创建PageView

    auto pageView = PageView::create();
    pageView->setContentSize(Size(screenSize.width, screenSize.height));
    pageView->setPosition(Vec2::ZERO);
    menuLayer->addChild(pageView);
    // 定义地图图片数组
    std::vector<std::string> mapImages = {
        "Carrot/jieshao/jie_1.png",
        "Carrot/jieshao/jie_2.png",
        "Carrot/jieshao/jie_3.png",
        "Carrot/jieshao/jie_4.png",
        "Carrot/jieshao/jie_5.png"
    };
    // 添加页面
    for (int i = 0; i < mapImages.size(); ++i) {
        Layout* layout = Layout::create();
        layout->setContentSize(Size(screenSize.width, screenSize.height));
        ImageView* imageView = EnglishUI::imageView(mapImages[i]);
        imageView->setContentSize(Size(screenSize.width, screenSize.height));
        imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
        imageView->setScale(2);  //待定
        layout->addChild(imageView, 1);
        pageView->addPage(layout);
    }
    auto continueButton = EnglishUI::menuItem("Carrot/jieshao/cha_0.png", "Carrot/jieshao/cha_1.png");
    continueButton->setPosition(814,503);
    continueButton->setScale(1.9);
    // 继续游戏选项
    continueButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        });
    menu->addChild(continueButton, 1);
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
        }
        });
    menuLayer->addChild(leftButton, 1);
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
        }
        });
    menuLayer->addChild(rightButton, 1);
}
//菜单按钮
void BaseLevelScene::menu_all(Ref* pSender) {
    if (menuVisible || gameFinished) return;
    menuVisible = true;
    a.button_music();
    Director::getInstance()->pause();
    auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //获取屏幕大小
    //创建灰色遮罩层
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    this->addChild(menuLayer, 10);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [menuLayer](Touch* touch, Event* event) {
        return true;
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, menuLayer);
    //添加暂停菜单背景
    auto menuBackground = EnglishUI::sprite("CarrotGuardRes/UI/gameMenu.png");
    menuBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    menuBackground->setScale(1.5f);
    menuLayer->addChild(menuBackground, 0);
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    //添加暂停菜单上相关功能按钮
    auto continueButton = EnglishUI::menuItem("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
    continueButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.649));
    continueButton->setScale(1.5);
    auto restartButton = EnglishUI::menuItem("CarrotGuardRes/UI/restartNormal.png", "CarrotGuardRes/UI/restartSelected.png");
    restartButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.51));
    restartButton->setScale(1.5);
    auto chooseButton = EnglishUI::menuItem("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
    chooseButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.375));
    chooseButton->setScale(1.5);
    // 继续游戏选项
    continueButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        menuVisible = false;
        // 判断在点击菜单按钮之前是否点击过暂停按钮，防止出现bug
        if (isPaused == 0) {
            Director::getInstance()->resume();
        }
        });
    //重新开始游戏选项
    restartButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        // 取消与 GameManager 相关的所有调度器
        GameManager::getInstance()->stopAllSchedulers();
        //取消事件监听
        manager->removeListener();
        auto scene = BaseLevelScene::createScene(levelId);
        scheduler->setTimeScale(1.0f); //实现减速效果
        Director::getInstance()->replaceScene(scene);
        Director::getInstance()->resume();
        });

    //选择关卡选项
    chooseButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        checkpoint();
        this->removeChild(menuLayer);
        // 取消与 GameManager 相关的所有调度器
        GameManager::getInstance()->stopAllSchedulers();
        //取消事件监听
        manager->removeListener();
        auto themeScene = themescene::createScene();
        Director::getInstance()->replaceScene(themeScene);
        Director::getInstance()->resume();
        });
    menu->addChild(continueButton, 1);
    menu->addChild(chooseButton, 1);
    menu->addChild(restartButton, 1);
}
void BaseLevelScene::guaisou_jiansu(float guai_jiansu) {
    for (auto it = manager->monsters.begin(); it != manager->monsters.end(); it++) {
        if ((*it)->getHealth() > 0)
            (*it)->speedaction->setSpeed(guai_jiansu);
    }
}
//技能1按钮
void BaseLevelScene::Jineng1(Ref* pSender) {
    a.button_music();
    if (money >= 200) {
        updateMoney(-200);
        manager->Jineng1();
    }
}
//技能2按钮
void BaseLevelScene::Jineng2(Ref* pSender) {
    a.button_music();
    if (money >= 200) {
        updateMoney(-200);
        guaisou_jiansu(0.01f);
        beishu = 0.01f;
        auto delayaction = Sequence::create(
            DelayTime::create(5.0f),
            CallFunc::create([=] {guaisou_jiansu(1); beishu = 1.0f; }),
            nullptr);
        this->runAction(delayaction);
    }
}
//技能3按钮
void BaseLevelScene::Jineng3(Ref* pSender) {
    a.button_music();
    if (money >= 500) {
        auto bong = Sprite::create();
        if (!bong) {
            CCLOG("Failed to create bong sprite.");
            return;
        }
        bong->setPosition(480, 320);
        bong->setScale(2);
        this->addChild(bong);
        cocos2d::Vector<cocos2d::SpriteFrame*> frames;
        for (int i = 0; i <= 3; ++i) {
            std::string frameName = "Carrot/bong/bong_" + std::to_string(i) + ".png";
            auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 164, 160)); // 假设每张图片的大小是 64x64
            if (frame) {
                frames.pushBack(frame);
            }
            else {
                CCLOG("Failed to load frame: %s", frameName.c_str());
            }
        }
        if (frames.empty()) {
            CCLOG("No frames found for bong, skipping.");
            return;
        }
        auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
        auto animate = cocos2d::Animate::create(animation);
        auto onbong = cocos2d::CallFunc::create([bong]() {
            CCLOG("bong.");
            bong->removeFromParent();
            });
        a.bongSound();
        bong->runAction(cocos2d::Sequence::create(animate,onbong, nullptr));
        updateMoney(-500);
        for (auto it = manager->monsters.begin(); it != manager->monsters.end(); it++) {
            if ((*it)->getHealth() > 0) {
                (*it)->setHealth(0);
                (*it)->toDie(this);
            }
        }
    }
}
//技能4按钮
void BaseLevelScene::Jineng4(Ref* pSender) {
    a.button_music();
    if (money >= 200) {
        updateMoney(-200);
        tower_jiasu = 2;
        auto delayaction = Sequence::create(DelayTime::create(5.0f), CallFunc::create([=] {
            tower_jiasu = 1;
            }), nullptr);
        this->runAction(delayaction);
    }
}
//技能5按钮
void BaseLevelScene::Jineng5(Ref* pSender) {
    a.button_music();
    if (money >= 150) {
        updateMoney(-150);
        guaisou_jiansu(0.5f);
        beishu = 0.5f;
        auto delayaction = Sequence::create(
            DelayTime::create(5.0f),
            CallFunc::create([=] {guaisou_jiansu(1); beishu = 1.0f; }),
            nullptr);
        this->runAction(delayaction);
    }
}
void BaseLevelScene::UpMenuAppear(const Vec2& position)
{
    last_position = position;
    int key = map_data[int(position.x / CELL_SIZE)][int(position.y / CELL_SIZE)].key;
    towers[key]->UpMenuAppear(this, position);
    cell_flag = 0;
}
void BaseLevelScene::UpMenuGone(const Vec2& position)
{
    int key = BaseLevelScene::map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].key;
    towers[key]->UpMenuGone(this);
    if (position.x > last_position.x - CELL_SIZE / 2 && position.x < last_position.x + CELL_SIZE / 2 &&
        position.y > last_position.y - 3 * CELL_SIZE / 2 && position.y < last_position.y - CELL_SIZE / 2) {
        towers[key]->destroy(this);
        delete towers[key];
        towers.erase(key);
        map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag = 0;
    }
    else if (position.x > last_position.x - CELL_SIZE / 2 && position.x < last_position.x + CELL_SIZE / 2 &&
        position.y > last_position.y + CELL_SIZE / 2 && position.y < last_position.y + 3 * CELL_SIZE / 2) {
        towers[key]->update(this, last_position);
    }
    cell_flag = 1;
}
//游戏结束的界面处理
void BaseLevelScene::gameover(bool is_win, int currentWaveNum, int allWaveNum) {
    if (gameFinished) return;
    gameFinished = true;
    unscheduleUpdate();
    manager->stopAllSchedulers();
    Director::getInstance()->pause();
    //Director::getInstance()->pause();
    // 设置灰色遮罩层
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 0));
    menuLayer->setPosition(Vec2::ZERO);
    this->addChild(menuLayer, 10);

    // 创建菜单
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    // 游戏胜利
    if (is_win) {
        level_is_win[levelId - 1] = true;
        saveGameState();
        //添加游戏获胜界面
        auto gameWinBackground = EnglishUI::sprite("CarrotGuardRes/UI/WinGame.png");
        gameWinBackground->setPosition(Vec2(480, 320));
        gameWinBackground->setScale(1.5f);
        menuLayer->addChild(gameWinBackground, 0);
        //添加获胜的金萝卜标识
        auto goldenCarrot = EnglishUI::sprite("CarrotGuardRes/UI/goldenCarrot.png");
        goldenCarrot->setPosition(Vec2(960 * 0.493, 640 * 0.7));
        menuLayer->addChild(goldenCarrot, 0);

        // 胜利的相关提示语

        _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", std::min(currentWaveNum + 1, allWaveNum)), "Arial", 32);
        _curNumberLabel->setColor(Color3B::YELLOW);
        _curNumberLabel->setPosition(960 * 0.51, 640 * 0.54);
        Label* loseWordLeft = Label::createWithSystemFont(EnglishUI::text("game.fought_off"), "Arial", 30);
        loseWordLeft->setPosition(960 * 0.36, 640 * 0.54);
        Label* loseWordRight = Label::createWithSystemFont(EnglishUI::text("game.waves"), "Arial", 30);
        loseWordRight->setPosition(960 * 0.60, 640 * 0.545);

        this->addChild(_curNumberLabel, 10);
        this->addChild(loseWordLeft, 10);
        this->addChild(loseWordRight, 10);
        //继续游戏按钮
        auto continueButton = EnglishUI::menuItem("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
        continueButton->setPosition(Vec2(960 * 0.613, 640 * 0.375));
        continueButton->setScale(1.38);
        continueButton->setCallback([this, menuLayer](Ref* psender) {
            a.button_music();
            //若当前未到开放的最后一关，则进行下一关
            if (levelId < 3) {
                auto scene = BaseLevelScene::createScene(levelId + 1);
                Director::getInstance()->replaceScene(scene);
            }
            //若当前已经是开放的最后一关，则返回选择关卡界面
            else {
                Director::getInstance()->resume();
                auto themeScene = themescene::createScene();
                Director::getInstance()->replaceScene(themeScene);
            }
            });
        menu->addChild(continueButton, 1);
    }
    // 游戏失败
    else {
        auto gameLoseBackground = EnglishUI::sprite("CarrotGuardRes/UI/LoseGame.png");
        gameLoseBackground->setPosition(Vec2(960 / 2 + 960 * 0.01, 640 / 2 + 640 * 0.015));
        gameLoseBackground->setScale(1.5f);
        menuLayer->addChild(gameLoseBackground, 0);

        // 游戏失败的相关提示语

        _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", currentWaveNum), "Arial", 32);// 暂时没搞currnum为什么会大1，所以先-1
        _curNumberLabel->setColor(Color3B::YELLOW);
        _curNumberLabel->setPosition(960 * 0.51, 640 * 0.54);
        Label* loseWordLeft = Label::createWithSystemFont(EnglishUI::text("game.fought_off"), "Arial", 30);

        loseWordLeft->setPosition(960 * 0.36, 640 * 0.54);
        Label* loseWordRight = Label::createWithSystemFont(EnglishUI::text("game.waves"), "Arial", 30);
        loseWordRight->setPosition(960 * 0.60, 640 * 0.54);

        this->addChild(_curNumberLabel, 10);
        this->addChild(loseWordLeft, 10);
        this->addChild(loseWordRight, 10);
        //重新游戏按钮
        auto againButton = EnglishUI::menuItem("CarrotGuardRes/UI/AgainNormal.png", "CarrotGuardRes/UI/AgainSelected.png");
        againButton->setPosition(Vec2(960 * 0.61, 640 * 0.37));
        againButton->setScale(0.9);
        // 重新开始按钮的选项
        againButton->setCallback([this, menuLayer](Ref* psender) {
            a.button_music();
            this->removeChild(menuLayer);
            // 取消与 GameManager 相关的所有调度器
           // GameManager::getInstance()->stopAllSchedulers();
            auto scene = BaseLevelScene::createScene(levelId);
            Director::getInstance()->replaceScene(scene);
            //Director::getInstance()->resume();
            });
        menu->addChild(againButton, 1);

    }
    // 选择游戏关卡按钮
    auto chooseButton = EnglishUI::menuItem("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
    chooseButton->setPosition(Vec2(960 * 0.38, 640 * 0.37));
    chooseButton->setScale(1.4);
    //选择关卡选项
    chooseButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        // 取消与 GameManager 相关的所有调度器
        GameManager::getInstance()->stopAllSchedulers();
        //取消事件监听
        manager->removeListener();
        auto themeScene = themescene::createScene();
        Director::getInstance()->replaceScene(themeScene);
        Director::getInstance()->resume();
        });
    menu->addChild(chooseButton, 1);

}
/******************************************/

/****************初始化部分******************/
// 创建场景时传入关卡编号
Scene* BaseLevelScene::createScene(int level) {
    auto scene = BaseLevelScene::create();  // 创建 BaseLevelScene
    if (scene && scene->initWithLevel(level))
    {
        return scene;
    }
    return scene;
}
// 加载地图
void BaseLevelScene::loadMap() {
    if (levelId <= 0 || levelId > mapFiles.size()) {
        CCLOG("Invalid level number!");
        return;
    }

    // 加载关卡地图
    tileMap = TMXTiledMap::create(mapFiles[levelId - 1]);
    if (tileMap) {
        // 获取屏幕大小
        auto screenSize = Director::getInstance()->getVisibleSize();
        auto tileMapSize = tileMap->getContentSize();
        //获取瓦片大小
        tileSize = tileMap->getTileSize();
        CCLOG("Tile size: (%f, %f)", tileSize.width, tileSize.height);
        // 缩放瓦片地图，使其填满整个屏幕
        tileMap->setScaleX(screenSize.width / tileMapSize.width);
        tileMap->setScaleY(screenSize.height / tileMapSize.height);

        // 设置瓦片地图的锚点为 (0, 0)，使地图左下角与屏幕左下角对齐
        tileMap->setAnchorPoint(Vec2::ZERO);
        tileMap->setPosition(Vec2::ZERO);
        // 获取并隐藏 plantable 层
        plantableLayer = tileMap->getLayer("plantable");
        if (plantableLayer) {
            plantableLayer->setVisible(false);  // 不显示 plantable 层
        }
        else {
            CCLOG("Layer 'plantable' not found in map: %s", mapFiles[levelId - 1].c_str());
        }
        // 将瓦片地图添加到当前场景中
        this->addChild(tileMap, 0);
#ifdef DEBUG_MODE
        drawGrid();
#endif
    }
    else {
        CCLOG("Failed to load map: %s", mapFiles[levelId - 1].c_str());
    }
}
//初始化ui组件
void BaseLevelScene::initUI()
{
    // 1. 显示出现了多少波怪物
    _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", std::min(manager->getCurrentWaveNum(), manager->getAllWaveNum())), "Arial", 32);
    _curNumberLabel->setColor(Color3B::YELLOW);
    _curNumberLabel->setPosition(960 * 0.42, 640 * 0.95);
    this->addChild(_curNumberLabel, 2);
    // 2. 一共有多少波怪物
    _numberLabel = Label::createWithSystemFont(StringUtils::format(EnglishUI::text("game.wave_total").c_str(), manager->getAllWaveNum()), "Arial", 32);
    _numberLabel->setColor(Color3B::YELLOW);
    _numberLabel->setAnchorPoint(Vec2(0, 0.5f));
    _numberLabel->setPosition(960 * 0.44, 640 * 0.95);
    _numberLabel->setScale(0.8f);
    this->addChild(_numberLabel, 2);
    //加money
    moneyLable = Label::createWithTTF("1000", "fonts/arial.ttf", 27);
    moneyLable->setPosition(Vec2(160, 610));
    this->addChild(moneyLable, 3);
    //添加返回按钮
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 2);
    //倍数
    auto doubleSpeedButton = EnglishUI::menuItem("CarrotGuardRes/UI/normalSpeed.png", "CarrotGuardRes/UI/doubleSpeed.png", CC_CALLBACK_1(BaseLevelScene::doublespeed, this));
    if (doubleSpeedButton == nullptr)
    {
        problemLoading("'normalSpeed.png' and 'doubleSpeed.png'");
    }
    else
    {
        doubleSpeedButton->setPosition(670, 610);
        doubleSpeedButton->setScale(1);
        menu->addChild(doubleSpeedButton);
    }
    //暂停
    auto pauseSpeedButton = EnglishUI::menuItem("CarrotGuardRes/UI/pauseButton.PNG", "CarrotGuardRes/UI/continueButton.PNG", CC_CALLBACK_1(BaseLevelScene::pause_all, this));
    if (pauseSpeedButton == nullptr)
    {
        problemLoading("'pauseButton.png' and 'continueButton.png'");
    }
    else
    {
        pauseSpeedButton->setPosition(770, 610);
        pauseSpeedButton->setScale(1);
        menu->addChild(pauseSpeedButton);
    }
    //菜单
    auto menuButton = EnglishUI::menuItem("CarrotGuardRes/UI/gameMenuNormal.png", "CarrotGuardRes/UI/gameMenuSelected.png", CC_CALLBACK_1(BaseLevelScene::menu_all, this));
    if (menuButton == nullptr)
    {
        problemLoading("'gameMenuNormal.png' and 'gameMenuSelected'");
    }
    else
    {
        menuButton->setPosition(870, 610);
        menuButton->setScale(1);
        menu->addChild(menuButton);
    }
    //介绍
    auto wenhaoButton = EnglishUI::menuItem("CarrotGuardRes/UI/wenhao_0.png", "CarrotGuardRes/UI/wenhao_1.png", CC_CALLBACK_1(BaseLevelScene::wenhao, this));
    if (menuButton == nullptr)
    {
        problemLoading("'wenhao_0.png' and 'wenhao_1.png'");
    }
    else
    {
        wenhaoButton->setPosition(350, 28);
        wenhaoButton->setScale(1.4);
        menu->addChild(wenhaoButton);
    }
    auto jineng1Button = EnglishUI::menuItem("Carrot/jineng1.png", "Carrot/jineng1_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng1, this));
    if (jineng1Button == nullptr)
    {
        problemLoading("'jineng1.png'");
    }
    else
    {
        jineng1Button->setPosition(525, 33);//525,33
        jineng1Button->setScale(1.9);
        menu->addChild(jineng1Button);
    }
    auto jineng2Button = EnglishUI::menuItem("Carrot/jineng2.png", "Carrot/jineng2_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng2, this));
    if (jineng2Button == nullptr)
    {
        problemLoading("'jineng2.png'");
    }
    else
    {
        jineng2Button->setPosition(710, 33);//710
        jineng2Button->setScale(1.9);
        menu->addChild(jineng2Button);
    }
    auto jineng3Button = EnglishUI::menuItem("Carrot/jineng3.png", "Carrot/jineng3_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng3, this));
    if (jineng3Button == nullptr)
    {
        problemLoading("'jineng3.png'");
    }
    else
    {
        jineng3Button->setPosition(432, 33);//432
        jineng3Button->setScale(1.7);
        menu->addChild(jineng3Button);
    }
    auto jineng4Button = EnglishUI::menuItem("Carrot/jineng4.png", "Carrot/jineng4_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng4, this));
    if (jineng4Button == nullptr)
    {
        problemLoading("'jineng4.png'");
    }
    else
    {
        jineng4Button->setPosition(615, 33);//615
        jineng4Button->setScale(1.7);
        menu->addChild(jineng4Button);
    }
    auto jineng5Button = EnglishUI::menuItem("Carrot/jineng5.png", "Carrot/jineng5_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng5, this));
    if (jineng5Button == nullptr)
    {
        problemLoading("'jineng5.png'");
    }
    else
    {
        jineng5Button->setPosition(796, 33);
        jineng5Button->setScale(1.7);
        menu->addChild(jineng5Button);
    }

    auto jinengtiao = EnglishUI::sprite("Carrot/jinengtiao.png");
    jinengtiao->setPosition(480, 30);
    jinengtiao->setScale(0.7);
    this->addChild(jinengtiao, 1);
}
//开局倒计时
void BaseLevelScene::CountDown(std::function<void()> onComplete)
{
    auto countBackground = EnglishUI::sprite("CarrotGuardRes/UI/countBackground.PNG");
    auto count1 = EnglishUI::sprite("CarrotGuardRes/UI/countOne.PNG");
    auto count2 = EnglishUI::sprite("CarrotGuardRes/UI/countTwo.PNG");
    auto count3 = EnglishUI::sprite("CarrotGuardRes/UI/countThree.PNG");
    Label* count0 = Label::createWithSystemFont(EnglishUI::text("game.go"), "Arial-BoldMT", 100);

    countBackground->setPosition(480, 320);
    count1->setPosition(480, 320);
    count2->setPosition(480, 320);
    count3->setPosition(480, 320);
    count0->setPosition(480, 320);

    countBackground->setVisible(false);
    count1->setVisible(false);
    count2->setVisible(false);
    count3->setVisible(false);
    count0->setVisible(false);

    this->addChild(countBackground, 2);
    this->addChild(count1, 2);
    this->addChild(count2, 2);
    this->addChild(count3, 2);
    this->addChild(count0, 2);
    // 设置倒数sequence动作
    auto countdown = Sequence::create(
        CallFunc::create([=] {
            countBackground->setVisible(true);
            count3->setVisible(true);
            a.countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count3);
            }),
        CallFunc::create([=] {
            count2->setVisible(true);
            a.countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count2);
            }),
        CallFunc::create([=] {
            count1->setVisible(true);
            a.countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count1);
            count0->setVisible(true);
            a.countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count0);
            this->removeChild(countBackground);

            // 倒计时结束，触发回调
            if (onComplete) {
                onComplete();  // 执行回调函数
            }
            }),
        nullptr  // Sequence结束
    );
    // 运行倒计时动作
    this->runAction(countdown);
}
//初始化关卡
bool BaseLevelScene::initWithLevel(int level)
{
    if (!Scene::init())
    {
        return false;
    }
    Director::getInstance()->resume();
    this->levelId = level;                     //存储关卡编号
    this->loadMap();                           // 加载对应关卡的地图
    manager = GameManager::getInstance(this);  // 初始化GameManager
    manager->initLevel(level, !isNewGame[levelId - 1]);          //初始化怪兽 路径和存档等资源
    initUI();                                 //初始化ui


    // 1. 创建植物图层
    plantsLayer = Layer::create();  // 创建一个新的图层，用于存放植物
    this->addChild(plantsLayer, 10); // 将植物图层添加到场景，zOrder为3，确保它位于其他层之上
    addMouseListener();  // 添加鼠标监听

    cell_flag = 1;
    buy_tower[0].push_back("Towers/affordhuo.png"); buy_tower[1].push_back("Towers/unaffordhuo.png");
    buy_tower[0].push_back("Towers/affordsun.png"); buy_tower[1].push_back("Towers/unaffordsun.png");
    buy_tower[0].push_back("Towers/affordplane.png"); buy_tower[1].push_back("Towers/unaffordplane.png");
    buy_tower[0].push_back("Towers/affordshit.png"); buy_tower[1].push_back("Towers/unaffordshit.png");
    index_table.push_back(0);
    index_table.push_back(1);
    index_table.push_back(2);
    index_table.push_back(3);
    InitMapData();
    if (!isNewGame[levelId - 1])
    {
        if (loadTowerData("level" + std::to_string(levelId) + "_tower.json") == false)
            ProduceObstacles();
    }
    if (isNewGame[levelId - 1])  ProduceObstacles();

    tower_jiasu = 0.01;
    beishu = 1;

    // 调用倒计时函数并传递回调
    CountDown([=] {
        CCLOG("READY");
        //计时结束后才能开始怪兽波
        manager->startMonsterWaves();
        tower_jiasu = 1;
        });

    levelReady = true;
    this->scheduleUpdate();                    //启动更新逻辑


    return true;
}
// 默认的 init 方法
bool BaseLevelScene::init() {
    if (!Scene::init()) {
        return false;
    }
    return true;
}
//更新游戏状态
void BaseLevelScene::update(float deltaTime) {
    if (gameFinished) return;

    //更新当前波数
    updateCurrentWaveLabe();
    //更新炮塔和怪兽交互
    for (auto it = towers.begin(); it != towers.end(); it++) {

        if (it->second->interval >= it->second->interval_table[it->second->GetIndex()]) {
            it->second->attack(this, GameManager::getInstance()->monsters, isTarget, tar_m, tar_o,tower_jiasu);
            continue;
        }

        it->second->interval += deltaTime*tower_jiasu;
    }
    //更新检验输状态
    if(manager->CheckLose())
    {
       gameover(false,manager->getCurrentWaveNum(), manager->getAllWaveNum());
       return;
    }
    //更新检验赢状态
    if(manager->CheckWin())
    {
        gameover(true, manager->getCurrentWaveNum(), manager->getAllWaveNum());
        return;
    }
    time_total += deltaTime;
    if (time_total >= 5) {
        time_total -= 5;
        manager->doudong();
    }
}
//初始化map
void BaseLevelScene::InitMapData()//初始化地图数据
{
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) {
            map_data[i][j].flag = obpos[levelId - 1][i][j];
            map_data[i][j].key = Y * i + j;
        }//3表示此地可用于生成障碍物，在后面生成障碍物时会用到
}
//产生障碍物
void BaseLevelScene::ProduceObstacles()//在场景中生成障碍物
{
    srand(time(0));
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) {
            if (map_data[i][j].flag == 3) {
                int rd;//随机因子
                if (i + 1 < X && j + 1 < Y && map_data[i + 1][j].flag == 3 && map_data[i][j + 1].flag == 3 && map_data[i + 1][j + 1].flag == 3)
                    rd = 10;
                else if (i + 1 < X && map_data[i + 1][j].flag == 3) rd = 6;
                else rd = 5;
                int index = rand() % (rd + 5);
                if (index >= rd) {//并不是在每个可生成障碍物的地方都生成障碍物
                    map_data[i][j].flag = 0;
                    continue;
                }
                map_data[i][j].flag = 2;
                auto obb = new Obstacle(index);
                obb->Produce(this, i, j);
                Obstacles[map_data[i][j].key] = obb;
                if (index == 5) {
                    map_data[i + 1][j].flag = 2;
                    Obstacles[map_data[i + 1][j].key] = obb;
                }
                else if (index > 5) {
                    map_data[i][j + 1].flag = map_data[i + 1][j].flag = map_data[i + 1][j + 1].flag = 2;
                    Obstacles[map_data[i + 1][j].key] = Obstacles[map_data[i][j + 1].key] = Obstacles[map_data[i + 1][j + 1].key] = obb;
                }
            }
        }
}
/******************************************/

/*****************炮塔相关*************************/
 //开启鼠标监听
void BaseLevelScene::addMouseListener() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event*) {
        const auto point = touch->getLocation();
        // Leave the HUD and skill row to their menu widgets, even when paused.
        if (gameFinished || Director::getInstance()->isPaused() ||
            point.x < 0 || point.x >= 960 || point.y < 70 || point.y >= 575)
            return false;
        handlePlant(Vec2(point.x, 640 - point.y));
        return true;
    };
#else
    auto listener = EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(BaseLevelScene::handleMouseDown, this);
#endif
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
//处理鼠标点击事件
void BaseLevelScene::handleMouseDown(EventMouse* event) {
    // 获取鼠标点击位置
    Vec2 mousePos = event->getLocation();
    CCLOG("Mouse clicked at screen position: (%f, %f)", mousePos.x, mousePos.y);
    // 调用 handlePlant 方法来处理种植
    handlePlant(mousePos);
}
//处理种植炮塔的判断
void BaseLevelScene::handlePlant(const Vec2& position) {
    CCLOG("position: (%f, %f)", position.x, position.y);
    // 将鼠标点击的屏幕坐标转换为瓦片坐标
    Vec2 tileCoord = Vec2(floor(position.x / tileSize.width), floor(position.y / tileSize.height));
    //下面日志证明已经实现了网格的转化
    CCLOG("tileCoord: (%f, %f)", tileCoord.x, tileCoord.y);

    // 边界检查
    if (tileCoord.x < 0 || tileCoord.y < 0 ||
        tileCoord.x >= tileMap->getMapSize().width ||
        tileCoord.y >= tileMap->getMapSize().height) {
        CCLOG("Tile coordinate is out of bounds.");
        return;
    }

    if (isTarget != 0) {//如果在有锁定期间点击锁定目标，则取消锁定
        Vec2 pos = Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y);
        if (isTarget == 1) {
            auto m_pos = tar_m->getPosition(); auto m_size = tar_m->getContentSize();
            if (pos.x > m_pos.x - m_size.width / 2 && pos.x < m_pos.x + m_size.width / 2
                && pos.y>m_pos.y - m_size.height / 2 && pos.y < m_pos.y + m_size.height / 2) {
                isTarget = 0;
                tar_m = nullptr;
                lock->removeFromParent();
                return;
            }
        }
        else {
            auto o_pos = tar_o->getPosition(); auto o_size = tar_o->curr->getContentSize();
            if (pos.x > o_pos.x - o_size.width / 2 && pos.x < o_pos.x + o_size.width / 2
                && pos.y>o_pos.y - o_size.height / 2 && pos.y < o_pos.y + o_size.height / 2) {
                isTarget = 0;
                tar_o = nullptr;
                lock->removeFromParent();
                return;
            }
        }
    }

    // 判断该瓦片是否可种植
    auto tileGID = tileMap->getLayer("plantable")->getTileGIDAt(tileCoord);

    if (!cell_flag) {//如果已经出现种植或者升级菜单，则再次点击时判断是否进行种植或升级(删除)操作，并使菜单消失
        if (map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag == 0)
            PlantMenuGone(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        else if (map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag == 1)
            UpMenuGone(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        return;
    }

    if (tileGID != 0) {
        CCLOG("Tile at (%f, %f) is plantable.", tileCoord.x, tileCoord.y);
        // 这里实现种植逻辑
       // 将瓦片坐标转换为屏幕坐标
       // 获取地图的高度（瓦片的行数）
        float mapHeight = tileMap->getMapSize().height;

        // 转换为屏幕坐标
        float screenX = tileCoord.x * tileSize.width + tileSize.width / 2;
        float screenY = (mapHeight - tileCoord.y - 1) * tileSize.height + tileSize.height / 2;

        Vec2 mapPos = Vec2(screenX, screenY);

        // 输出屏幕坐标
        CCLOG("Screen Position: (%f, %f)", mapPos.x, mapPos.y);

        //如果这地方没有塔，那么打开种植菜单，否则打开升级菜单
        if (map_data[int(mapPos.x / CELL_SIZE)][int(mapPos.y / CELL_SIZE)].flag == 0)
            PlantMenuAppear(mapPos);
        else if (map_data[int(mapPos.x / CELL_SIZE)][int(mapPos.y / CELL_SIZE)].flag == 1)
            UpMenuAppear(mapPos);
        else {
            if (isTarget != 0) lock->removeFromParent();
            a.ShootSelect();
            isTarget = 2;
            tar_m = nullptr;
            tar_o = Obstacles[map_data[int(mapPos.x / CELL_SIZE)][int(mapPos.y / CELL_SIZE)].key];
            lock = EnglishUI::sprite("Towers/chosed.png");
            Vec2 o_pos = tar_o->getPosition();
            lock->setPosition(Vec2(o_pos.x, o_pos.y + tar_o->curr->getContentSize().height * 1.0));
            this->addChild(lock);
        }
    }

    else
    {
        Monster* tool = IsTargetMonster(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        if (tool != nullptr) {
            if (isTarget != 0) lock->removeFromParent();
            a.ShootSelect();
            isTarget = 1;
            tar_m = tool;
            tar_o = nullptr;
            lock = EnglishUI::sprite("Towers/chosed.png");
            lock->setPosition(Vec2(tar_m->getContentSize().width * 0.6, tar_m->getContentSize().height * 1.4));
            tar_m->addChild(lock, 100);
            CCLOG("-------------------------------------------------locked");
        }
        CCLOG("Tile at (%f, %f) is not plantable.", tileCoord.x, tileCoord.y);
    }

}
//种植界面出现
void BaseLevelScene::PlantMenuAppear(Vec2 mapPos)
{
    cell_flag = 0;
    auto location = mapPos;
    last_position = location;
    auto cell = EnglishUI::sprite("Towers/cell.png");
    cell->setScale((float)64 / 200);
    cell->setPosition(location);
    this->addChild(cell); curr_cell = cell;
    for (int i = 0, size = buy_tower[0].size(); i < size; i++) {
        int tool = Tower::build_cost[index_table[i]] > money ? 1 : 0;
        auto tower_graph = EnglishUI::sprite(buy_tower[tool][i]);
        tower_graph->setScale((float)CELL_SIZE / tower_graph->getContentSize().width);
        tower_graph->setPosition(Vec2(location.x - CELL_SIZE * (size - 1) / 2 + CELL_SIZE * i, location.y + CELL_SIZE));
        this->addChild(tower_graph); remove_table.push_back(tower_graph);
    }
}
//种植界面消失
void BaseLevelScene::PlantMenuGone(Vec2 position)
{
    auto location = position;
    int size = remove_table.size();
    if ((location.x > last_position.x - CELL_SIZE * size / 2 && location.x < last_position.x + CELL_SIZE * size / 2)
        && (location.y > last_position.y + CELL_SIZE / 2 && location.y < last_position.y + 3 * CELL_SIZE / 2)) {
        int index = index_table[(location.x - last_position.x + CELL_SIZE * size / 2) / CELL_SIZE];
        if (money >= Tower::build_cost[index]) {
            Tower* this_tower = createTower(index);
            this_tower->build(this, last_position);
            updateMoney(-Tower::build_cost[index]);
            a.TowerBuild();
            int x = int(last_position.x / CELL_SIZE), y = int(last_position.y / CELL_SIZE);
            towers[map_data[x][y].key] = this_tower;
            map_data[x][y].flag = 1;
        }
    }
    this->removeChild(curr_cell); curr_cell = nullptr;
    for (int i = 0; i < remove_table.size(); i++) { this->removeChild(remove_table[i]); }
    remove_table.clear();
    cell_flag = 1;
}
//判断怪兽是否被锁定
Monster* BaseLevelScene::IsTargetMonster(const Vec2& pos)//判断是否有怪物被锁定
{
    for (auto it = GameManager::getInstance()->monsters.begin(); it != GameManager::getInstance()->monsters.end(); it++) {
        if ((*it)->getHealth() <= 0)continue;
        auto m_pos = (*it)->getPosition();
        auto m_size = (*it)->getContentSize();
        if (pos.x > m_pos.x - m_size.width && pos.x < m_pos.x + m_size.width
            && pos.y>m_pos.y - m_size.height && pos.y < m_pos.y + m_size.height) return (*it);
    }
    return nullptr;
}
/******************************************/

/*****************工具函数*************************/
 //画网格线
//#ifdef DEBUG_MODE
void BaseLevelScene::drawGrid() {
    // 获取地图的大小和瓦片的大小
    Size mapSize = tileMap->getContentSize();
    float tileWidth = 64.0f;  // 假设每个瓦片宽度为 64 像素
    float tileHeight = 64.0f; // 假设每个瓦片高度为 64 像素

    // 创建一个 DrawNode 用于绘制网格
    auto drawNode = DrawNode::create();
    this->addChild(drawNode, 100);  // 将 DrawNode 添加到场景中，层级设置为 100，确保在地图之上

    // 绘制垂直线
    for (float x = 0; x <= mapSize.width; x += tileWidth) {
        drawNode->drawLine(Vec2(x, 0), Vec2(x, mapSize.height), Color4F::WHITE);
    }

    // 绘制水平线
    for (float y = 0; y <= mapSize.height; y += tileHeight) {
        drawNode->drawLine(Vec2(0, y), Vec2(mapSize.width, y), Color4F::WHITE);
    }
}
//#endif // DEBUG_MODE
//瓦格坐标转地图坐标的工具函数
Vec2 BaseLevelScene::gridToScreenCenter(const Vec2& gridPoint) {
    // 从 tileMap 获取地图高度（网格数量）
    float mapHeight = tileMap->getMapSize().height;
    // 转换为屏幕坐标
    float screenX = gridPoint.x * tileSize.height + tileSize.width / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * tileSize.height + tileSize.height / 2;
    return Vec2(screenX, screenY);
}
/************************************************/


/*****************存档相关*************************/
// 关卡存档函数
void BaseLevelScene::saveGameState() {

    // 创建 JSON 文档对象
    Document document;
    document.SetObject();  // 设置为 JSON 对象

    // 创建一个 JSON 数组，用来存储关卡状态
    rapidjson::Value GameState(kArrayType);

    // 将关卡状态逐个添加到 GameState 数组中
    for (auto level : level_is_win) {
        GameState.PushBack(rapidjson::Value(level), document.GetAllocator());
    }

    // 将 GameState 数组添加到 JSON 文档对象中
    document.AddMember("levels", GameState, document.GetAllocator());

    // 将 JSON 对象转换为字符串
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);  // 写入 JSON 字符串

    // 获取可写路径
    std::string writablePath = FileUtils::getInstance()->getWritablePath();  // 获取可写路径

    // 构造文件路径（假设文件名是 level_state.json）
    std::string filePath = writablePath + "level_state.json";  // 生成完整文件路径

    // 写入文件
    if (SaveIO::writeJSON(filePath, buffer.GetString())) {
        CCLOG("存档成功：%s", filePath.c_str());
    }
    else {
        CCLOG("存档失败：%s", filePath.c_str());
    }
}
//炮塔存档函数
void BaseLevelScene::saveTowerData()
{
    // 创建 JSON 文档对象
    Document document;
    document.SetObject();  // 设置为 JSON 对象
    //创建一个rapidjson数组存储所有信息
    rapidjson::Value towerArray(rapidjson::kArrayType);
    for(int i=0;i<X;i++)
    {
        //创建一个rapidjson数组存储每行信息
     rapidjson::Value rowArray(rapidjson::kArrayType);
     for (int j = 0; j < Y; j++)
     {
     rapidjson::Value towerObj(rapidjson::kObjectType);
     //填充单个单元
     //存储flag
     towerObj.AddMember("flag",map_data[i][j].flag, document.GetAllocator());
     //存储index
     if(map_data[i][j].flag==1)
     {
        towerObj.AddMember("index", towers[map_data[i][j].key]->GetIndex(), document.GetAllocator());
        towerObj.AddMember("data", towers[map_data[i][j].key]->GetGrade(), document.GetAllocator());
     }
     else if(map_data[i][j].flag == 2)
     {
         towerObj.AddMember("index", Obstacles[map_data[i][j].key]->GetIndex(), document.GetAllocator());
         towerObj.AddMember("data", Obstacles[map_data[i][j].key]->getHealth(), document.GetAllocator());
     }
     else {
         towerObj.AddMember("index", 0, document.GetAllocator());
         towerObj.AddMember("data", 0, document.GetAllocator());
     }
     //填充行
     rowArray.PushBack(towerObj, document.GetAllocator());
     }
     //填充文档
     towerArray.PushBack(rowArray, document.GetAllocator());
    }
    document.AddMember("towers", towerArray,document.GetAllocator());
    document.AddMember("carrotHealth", manager->getCarrotHealth(), document.GetAllocator());
    document.AddMember("money", money, document.GetAllocator()); // 存入当前波索引
    // 将 JSON 对象转换为字符串
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);  // 写入 JSON 字符串

    // 获取可写路径
    std::string writablePath = FileUtils::getInstance()->getWritablePath();  // 获取可写路径

    // 构造文件路径（假设文件名是 level_state.json）
    std::string filePath = writablePath + "level"+std::to_string(levelId)+"_tower.json";  // 生成完整文件路径

    // 写入文件
    if (SaveIO::writeJSON(filePath, buffer.GetString())) {
        CCLOG("存档成功：%s", filePath.c_str());
    }
    else {
        CCLOG("存档失败：%s", filePath.c_str());
    }
}
//炮塔读档函数
bool BaseLevelScene::loadTowerData(const std::string& filename)
{
    // 打开 JSON 文件
    std::string writablePath = FileUtils::getInstance()->getWritablePath();  // 获取可写路径
    std::string path = writablePath + filename ;//完整文件路径
    std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);
    // 使用 rapidjson 解析文件内容
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    if (doc.HasParseError() || !doc.IsObject()) {
        CCLOG("Error parsing JSON file: %s", filename.c_str());
        return false;
    }
    // 检查是否存在配置
    if (doc.HasMember("towers") && doc["towers"].IsArray()) {
        // 获取 tower数组
        const rapidjson::Value& towersArray = doc["towers"];
            if (towersArray.IsArray()) {
                for (rapidjson::SizeType i = 0; i < towersArray.Size() && i < X; ++i) {
                    const rapidjson::Value& row = towersArray[i];
                    if (row.IsArray()) {
                        for (rapidjson::SizeType j = 0; j < row.Size() && j < Y; ++j) {
                            const rapidjson::Value& towerObj = row[j];
                            if (towerObj.IsObject()) {
                                // 读取每个塔的 "flag"、"index" 和 "data"
                                int flag = towerObj["flag"].GetInt();
                                int index = towerObj["index"].GetInt();
                                int data = towerObj["data"].GetInt();

                               if(flag==1){
                                   Vec2 pos = Vec2((i+ 0.5) * CELL_SIZE, (j+0.5) * CELL_SIZE);
                                   auto tower=createTower(index,data);
                                   tower->build(this,pos);
                                   towers[map_data[i][j].key]=tower;
                               }
                               else if(flag==2&&map_data[i][j].flag==3){
                                   auto obb = new Obstacle(index);
                                   obb->Produce(this, i, j);
                                    obb->setHealth(data);
                                    obb->updateHealthBar();
                                   Obstacles[map_data[i][j].key] = obb;
                                   if(obb->GetSize()==2){
                                       map_data[i + 1][j].flag = 2;
                                       Obstacles[map_data[i + 1][j].key] = obb;
                                   }
                                   else if (obb->GetSize() ==4) {
                                       map_data[i][j + 1].flag = map_data[i + 1][j].flag = map_data[i + 1][j + 1].flag = 2;
                                       Obstacles[map_data[i + 1][j].key] = Obstacles[map_data[i][j + 1].key] = Obstacles[map_data[i + 1][j + 1].key] = obb;
                                   }
                               }
                               map_data[i][j].flag = flag;
                            }
                        }
                    }
                }
            }
            else {
                CCLOG("Towers data is not an array in level");
                return false;
            }

    }
    if (doc.HasMember("carrotHealth") && doc["carrotHealth"].IsInt())
        manager->restoreCarrotHealth(doc["carrotHealth"].GetInt());
    //读金币
    if (doc.HasMember("money") && doc["money"].IsInt()) {
        money = doc["money"].GetInt();
        updateMoney(0);
        CCLOG("INIT_MONEY:currentIndex: %d", money);
    }
    else {
        CCLOG("No such level: %s", filename);
        return false;
    }
    return true;
}
/************************************************/

// Called on the Cocos GL thread before Android suspends rendering.
void BaseLevelScene::checkpoint() {
    if (!levelReady || gameFinished || levelId < 1 || levelId > 3) return;
    saveGameState();
    saveTowerData();
    manager->saveMonstersDataToJson("level" + std::to_string(levelId) + "Monster.json");
}

BaseLevelScene::~BaseLevelScene() {
    for (const auto& entry : towers) delete entry.second;
    std::set<Obstacle*> unique;
    for (const auto& entry : Obstacles) unique.insert(entry.second);
    for (auto obstacle : unique) delete obstacle;
}
