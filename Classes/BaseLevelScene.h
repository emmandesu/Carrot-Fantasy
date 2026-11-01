#pragma once
#ifndef __BASE_H__
#define __BASE_H__
#include "cocos2d.h"
#include<string>
#include<map>
#include<vector>
#include<Monster.h>
#include"Tower.h"
#include"Obstacle.h"

USING_NS_CC;

#define Y 9
#define X 15

//用来表示地图格子信息的结构体
struct Cell {
    char flag;
    int key;
};
class BaseLevelScene : public cocos2d::Scene {
private:
    cocos2d::Node* plantsLayer;                              //植物图层
    TMXLayer* plantableLayer = nullptr;                      // plantable 层
    static const std::vector<std::string> mapFiles;          // 存储地图文件的路径
    int levelId = 0;
    bool gameFinished = false;
    bool levelReady = false;
    bool menuVisible = false;                                             //关卡编号
    //标签相关
    Label* moneyLable;                                       //显示当前金钱
    Label* _numberLabel;                                     // 显示怪物波数
    Label* _curNumberLabel;                                  // 显示当前怪物波数
    //速度控制相关
    bool isDoubleSpeed = false;                              // 初始化为false，表示游戏初始不是二倍速状态
    bool isPaused = false;                                   // 初始化为false，表示游戏初始不是暂停状态

    //炮塔相关
    std::vector<Sprite*>remove_table;                        //remove时的方格
    Sprite* curr_cell;                                       //种植的方格
    std::vector<std::string>buy_tower[2];                    //存放炮塔的购买图
    std::vector<int>index_table;                             //记录本关用到的炮塔的序号
    int cell_flag;
    Vec2 last_position;                                      //记录上一次的位置

    Scheduler* scheduler = Director::getInstance()->getScheduler();
    float time_total=0;                                      //用于萝卜抖动
    int money = 1000;                                        //钱
public:
    ~BaseLevelScene() override;
    std::map<int, Tower*> towers;                            //记录场上的塔的信息
    std::map<int, Obstacle*> Obstacles;                      //障碍物数组
    Monster* tar_m = nullptr;
    Obstacle* tar_o = nullptr;                               //指向被标记的对象
    char isTarget = 0;                                       //0代表没有被标记的，1代表怪物被标记，2代表障碍物被标记
    Cell map_data[X][Y];                                     //地图管理
    Sprite* lock;                                          //锁定标志，因为同一时间只能有一个对象被锁定，所以将其放在BaseLevelScene中
    int getMoney()const{return money;}
    virtual void update(float deltaTime) override;
    cocos2d::TMXTiledMap* tileMap = nullptr;                // 地图对象
    cocos2d::Size tileSize;                                 //每个瓦片的大小

    //初始化方法
    static cocos2d::Scene* createScene(int level);          // 创建场景时传入关卡编号
    bool init() override;                                    // 默认的 init() 方法
    void initUI();                                          //初始化ui
    bool initWithLevel(int level);                          //重写的init函数
    CREATE_FUNC(BaseLevelScene);
    void loadMap();                                         // 加载地图的函数
    void addMouseListener();                                // 添加鼠标监听器
    void InitMapData();                                     //初始化地图信息
    void ProduceObstacles();                                //产生障碍物
    //炮塔相关
    void handleMouseDown(EventMouse* event);                //鼠标点击时间判断是否种植
    void handlePlant(const Vec2& position);                 // 种植判断
    void PlantMenuAppear(Vec2 mapPos);                      //出现种植菜单
    void PlantMenuGone(Vec2 position);                      //种植菜单消失
    void drawGrid();                                        //画网格线
    void UpMenuAppear(const Vec2& position);                      //升级菜单出现
    void UpMenuGone(const Vec2& position);                        //升级菜单消失
    void saveTowerData();
    bool loadTowerData(const std::string& filename);
    Vec2 gridToScreenCenter(const Vec2& gridPoint);         //将网格坐标转成屏幕坐标的工具函数

    //按钮
    void updateMoney(int add);                                   //更新money，add为变化量
    void doublespeed(Ref* pSender);
    void pause_all(Ref* pSender);
    void menu_all(Ref* pSender);
    void updateCurrentWaveLabe();
    void wenhao(Ref* pSender);
    void CountDown(std::function<void()> onComplete);            //倒计时
    void gameover(bool is_win,int currentWaveNum,int allWaveNum);//结束游戏
    void Jineng1(Ref* pSender);                                  //萝卜血量加1
    void Jineng2(Ref* pSender);                                  //冰冻，怪兽停止
    void Jineng3(Ref* pSender);                                  //炸弹，消灭所有怪兽
    void Jineng4(Ref* pSender);                                  //植物攻速加倍
    void Jineng5(Ref* pSender);                                  //怪兽减速
    void guaisou_jiansu(float guai_jiansu);

    void checkpoint();
    void saveGameState();                                        //通关情况存档
    Monster* IsTargetMonster(const Vec2& pos);                   //检测是否有怪物被锁定

};
#endif
